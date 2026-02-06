use log::info;

use crate::qemu_args::qemu_arg_opt;

use memflow::prelude::v1::{
    mem, umem, Address, CTup2, Error, ErrorKind, ErrorOrigin, MemoryMap, Result,
};

#[cfg(all(target_os = "linux", feature = "qmp"))]
use {
    qapi::{qmp, Qmp},
    std::io::{Read, Write},
    std::net::TcpStream,
    std::os::unix::net::UnixStream,
};

#[derive(Debug, Clone)]
struct Mapping {
    pub range_start: umem,
    pub range_end: umem,
    pub remap_start: umem,
}

impl Mapping {
    pub const fn new(range_start: umem, range_end: umem, remap_start: umem) -> Self {
        Self {
            range_start,
            range_end,
            remap_start,
        }
    }
}

pub fn qemu_mem_mappings(
    cmdline: &str,
    qemu_map: &CTup2<Address, umem>,
) -> Result<MemoryMap<(Address, umem)>> {
    let mut mem_map = MemoryMap::new();

    let mappings = if let Ok(mappings) = qmp_get_mtree(cmdline.split_whitespace()) {
        mappings
    } else {
        // find machine architecture and type
        let machine = if !cmdline.is_empty()
            && cmdline
                .split_whitespace()
                .next()
                .unwrap()
                .contains("aarch64")
        {
            "aarch64".into()
        } else {
            qemu_arg_opt(cmdline.split_whitespace(), "-machine", "type")
                .unwrap_or_else(|| "pc".into())
        };
        info!("qemu process started with machine: {}", machine);
        qemu_get_mtree_fallback(&machine, qemu_map)
    };

    // add all mappings
    for mapping in mappings.iter() {
        mem_map.push_range(
            mapping.range_start.into(),
            mapping.range_end.into(),
            qemu_map.0 + mapping.remap_start,
        );
    }

    Ok(mem_map)
}

#[cfg(all(target_os = "linux", feature = "qmp"))]
fn qmp_get_mtree<'a>(cmdline: impl IntoIterator<Item = &'a str>) -> Result<Vec<Mapping>> {
    // -qmp unix:/tmp/qmp-win10-reversing.sock,server,nowait
    let socket_addr = qemu_arg_opt(cmdline, "-qmp", "")
        .ok_or(Error(ErrorOrigin::Connector, ErrorKind::Configuration))?;
    if socket_addr.starts_with("unix:") {
        let socket_path = socket_addr
            .strip_prefix("unix:")
            .ok_or(Error(ErrorOrigin::Connector, ErrorKind::Configuration))?;

        info!("connecting to qmp unix socket at: {}", socket_path);
        let stream = UnixStream::connect(socket_path).map_err(|err| {
            Error(ErrorOrigin::Connector, ErrorKind::Configuration).log_error(err)
        })?;

        qmp_get_mtree_stream(&stream)
    } else if socket_addr.starts_with("tcp:") {
        let socket_url = socket_addr
            .strip_prefix("tcp:")
            .ok_or(Error(ErrorOrigin::Connector, ErrorKind::Configuration))?;

        info!("connecting to qmp tcp socket at: {}", socket_url);

        let stream = TcpStream::connect(socket_url).map_err(|err| {
            Error(ErrorOrigin::Connector, ErrorKind::Configuration).log_error(err)
        })?;

        qmp_get_mtree_stream(&stream)
    } else {
        Err(Error(ErrorOrigin::Connector, ErrorKind::Configuration))
    }
}

#[cfg(all(target_os = "linux", feature = "qmp"))]
fn qmp_get_mtree_stream<S: Read + Write + Clone>(stream: S) -> Result<Vec<Mapping>> {
    let mut qmp = Qmp::from_stream(stream);
    qmp.handshake()
        .map_err(|err| Error(ErrorOrigin::Connector, ErrorKind::Configuration).log_error(err))?;

    let mtreestr = qmp
        .execute(&qmp::human_monitor_command {
            command_line: "info mtree -f".to_owned(),
            cpu_index: None,
        })
        .map_err(|err| Error(ErrorOrigin::Connector, ErrorKind::Configuration).log_error(err))?;

    Ok(qmp_parse_mtree(&mtreestr))
}

#[cfg(not(all(target_os = "linux", feature = "qmp")))]
fn qmp_get_mtree<'a>(_cmdline: impl IntoIterator<Item = &'a str>) -> Result<Vec<Mapping>> {
    Err(Error(
        ErrorOrigin::Connector,
        ErrorKind::UnsupportedOptionalFeature,
    ))
}

#[cfg(all(target_os = "linux", feature = "qmp"))]
fn qmp_parse_mtree(mtreestr: &str) -> Vec<Mapping> {
    let mut mappings = Vec::new();
    let mut system_region = false;
    for line in mtreestr.lines().map(|l| l.trim()) {
        let memory_region = scan_fmt!(line, "Root memory region: {}", String);
        match memory_region.as_deref() {
            Ok("system") => {
                system_region = true;
            }
            Ok(_) => {
                system_region = false;
            }
            _ => (),
        }

        if system_region {
            let range = scan_fmt_some!(line, "{x}-{x} {*[^:]}: pc.ram {*[@]}{x} KVM", [hex umem], [hex umem], [hex umem]);
            if range.0.is_some() && range.1.is_some() {
                // add the mapping here, in case the third entry is None
                // we just add the first start mapping here.
                // this should only ever happen for the first entry which starts/remaps at/to 0.
                mappings.push(Mapping::new(
                    range.0.unwrap(),
                    range.1.unwrap() + 1,
                    range.2.unwrap_or_else(|| range.0.unwrap()),
                ))
            }
        }
    }
    mappings
}

fn qemu_get_mtree_fallback(
    machine: &str,
    &CTup2(_, map_size): &CTup2<Address, umem>,
) -> Vec<Mapping> {
    info!("qemu memory map size: {:x}", map_size);

    if machine.contains("q35") {
        if map_size >= mem::mb(2816) {
            info!("using fallback memory mappings for q35 with more than 2816mb of ram");
            qemu_get_mtree_fallback_q35(map_size)
        } else {
            info!("using fallback memory mappings for q35 with less than 2816mb of ram");
            qemu_get_mtree_fallback_q35_smallmem(map_size)
        }
    } else if machine.contains("aarch64") || machine.contains("virt") {
        info!("using fallback memory mappings for aarch64");
        qemu_get_mtree_fallback_aarch64(map_size)
    } else {
        info!("using fallback memory mappings for pc-i1440fx");
        qemu_get_mtree_fallback_pc(map_size)
    }
}

/// Returns hard-coded mem-mappings for q35 qemu machine types with more than 2816 mb of ram.
fn qemu_get_mtree_fallback_q35(map_size: umem) -> Vec<Mapping> {
    /*
    0000000000000000-000000000009ffff (prio 0, ram): pc.ram KVM
    00000000000c0000-00000000000c3fff (prio 0, rom): pc.ram @00000000000c0000 KVM
    0000000000100000-000000007fffffff (prio 0, ram): pc.ram @0000000000100000 KVM
    0000000100000000-000000047fffffff (prio 0, ram): pc.ram @0000000080000000 KVM
    */
    vec![
        Mapping::new(mem::mb(0), mem::gb(2), mem::mb(0)),
        Mapping::new(mem::gb(4), map_size + mem::gb(2), mem::gb(2)),
    ]
}

/// Returns hard-coded mem-mappings for q35 qemu machine types with less than 2816 mb of ram.
fn qemu_get_mtree_fallback_q35_smallmem(map_size: umem) -> Vec<Mapping> {
    // Same as above but without the second mapping
    vec![Mapping::new(mem::mb(0), map_size, mem::mb(0))]
}

/// Returns hard-coded mem-mappings for aarch64 qemu machine types.
fn qemu_get_mtree_fallback_aarch64(map_size: umem) -> Vec<Mapping> {
    // It is not known for sure whether this is correct for all ARM machines, but
    // it seems like all memory on qemu ARM is shifted by 1GB and is linear from there.
    vec![Mapping::new(mem::gb(1), map_size + mem::gb(1), 0u64)]
}

/// Returns hard-coded mem-mappings for pc-i1440fx qemu machine types.
fn qemu_get_mtree_fallback_pc(map_size: umem) -> Vec<Mapping> {
    /*
    0000000000000000-00000000000bffff (prio 0, ram): pc.ram KVM
    00000000000c0000-00000000000cafff (prio 0, rom): pc.ram @00000000000c0000 KVM
    00000000000cb000-00000000000cdfff (prio 0, ram): pc.ram @00000000000cb000 KVM
    00000000000ce000-00000000000e7fff (prio 0, rom): pc.ram @00000000000ce000 KVM
    00000000000e8000-00000000000effff (prio 0, ram): pc.ram @00000000000e8000 KVM
    00000000000f0000-00000000000fffff (prio 0, rom): pc.ram @00000000000f0000 KVM
    0000000000100000-00000000bfffffff (prio 0, ram): pc.ram @0000000000100000 KVM
    0000000100000000-000000023fffffff (prio 0, ram): pc.ram @00000000c0000000 KVM
    */
    vec![
        Mapping::new(0u64, mem::kb(768), 0u64),
        Mapping::new(mem::kb(812), mem::kb(824), mem::kb(812)),
        Mapping::new(mem::kb(928), mem::kb(960), mem::kb(928)),
        Mapping::new(mem::mb(1), mem::gb(3), mem::mb(1)),
        Mapping::new(mem::gb(4), map_size + mem::gb(1), mem::gb(3)),
    ]
}

#[cfg(test)]
#[cfg(all(target_os = "linux", feature = "qmp"))]
mod tests {
    use super::qmp_parse_mtree;

    #[test]
    fn test_parse_mtree() {
        let mtreestr = r#"
        FlatView #0
        AS \"I/O\", root: io
        Root memory region: io
         0000000000000000-0000000000000007 (prio 0, i/o): dma-chan
         0000000000000008-000000000000000f (prio 0, i/o): dma-cont
         0000000000000010-000000000000001f (prio 0, i/o): io @0000000000000010
         0000000000000020-0000000000000021 (prio 0, i/o): kvm-pic
         0000000000000022-000000000000003f (prio 0, i/o): io @0000000000000022
         0000000000000040-0000000000000043 (prio 0, i/o): kvm-pit
         0000000000000044-000000000000005f (prio 0, i/o): io @0000000000000044
         0000000000000060-0000000000000060 (prio 0, i/o): i8042-data
         0000000000000061-0000000000000061 (prio 0, i/o): pcspk
         0000000000000062-0000000000000063 (prio 0, i/o): io @0000000000000062
         0000000000000064-0000000000000064 (prio 0, i/o): i8042-cmd
         0000000000000065-000000000000006f (prio 0, i/o): io @0000000000000065
         0000000000000070-0000000000000070 (prio 0, i/o): rtc-index
         0000000000000071-0000000000000071 (prio 0, i/o): rtc @0000000000000001
         0000000000000072-000000000000007d (prio 0, i/o): io @0000000000000072
         000000000000007e-000000000000007f (prio 0, i/o): kvmvapic
         0000000000000080-0000000000000080 (prio 0, i/o): ioport80
         0000000000000081-0000000000000083 (prio 0, i/o): dma-page
         0000000000000084-0000000000000086 (prio 0, i/o): io @0000000000000084
         0000000000000087-0000000000000087 (prio 0, i/o): dma-page
         0000000000000088-0000000000000088 (prio 0, i/o): io @0000000000000088
         0000000000000089-000000000000008b (prio 0, i/o): dma-page
         000000000000008c-000000000000008e (prio 0, i/o): io @000000000000008c
         000000000000008f-000000000000008f (prio 0, i/o): dma-page
         0000000000000090-0000000000000091 (prio 0, i/o): io @0000000000000090
         0000000000000092-0000000000000092 (prio 0, i/o): port92
         0000000000000093-000000000000009f (prio 0, i/o): io @0000000000000093
         00000000000000a0-00000000000000a1 (prio 0, i/o): kvm-pic
         00000000000000a2-00000000000000b1 (prio 0, i/o): io @00000000000000a2
         00000000000000b2-00000000000000b3 (prio 0, i/o): apm-io
         00000000000000b4-00000000000000bf (prio 0, i/o): io @00000000000000b4
         00000000000000c0-00000000000000cf (prio 0, i/o): dma-chan
         00000000000000d0-00000000000000df (prio 0, i/o): dma-cont
         00000000000000e0-00000000000000ef (prio 0, i/o): io @00000000000000e0
         00000000000000f0-00000000000000f0 (prio 0, i/o): ioportF0
         00000000000000f1-00000000000004cf (prio 0, i/o): io @00000000000000f1
         00000000000004d0-00000000000004d0 (prio 0, i/o): kvm-elcr
         00000000000004d1-00000000000004d1 (prio 0, i/o): kvm-elcr
         00000000000004d2-000000000000050f (prio 0, i/o): io @00000000000004d2
         0000000000000510-0000000000000511 (prio 0, i/o): fwcfg
         0000000000000512-0000000000000513 (prio 0, i/o): io @0000000000000512
         0000000000000514-000000000000051b (prio 0, i/o): fwcfg.dma
         000000000000051c-00000000000005ff (prio 0, i/o): io @000000000000051c
         0000000000000600-0000000000000603 (prio 0, i/o): acpi-evt
         0000000000000604-0000000000000605 (prio 0, i/o): acpi-cnt
         0000000000000606-0000000000000607 (prio 0, i/o): io @0000000000000606
         0000000000000608-000000000000060b (prio 0, i/o): acpi-tmr
         000000000000060c-000000000000061f (prio 0, i/o): io @000000000000060c
         0000000000000620-000000000000062f (prio 0, i/o): acpi-gpe0
         0000000000000630-0000000000000637 (prio 0, i/o): acpi-smi
         0000000000000638-000000000000065f (prio 0, i/o): io @0000000000000638
         0000000000000660-000000000000067f (prio 0, i/o): sm-tco
         0000000000000680-0000000000000cd7 (prio 0, i/o): io @0000000000000680
         0000000000000cd8-0000000000000ce3 (prio 0, i/o): acpi-cpu-hotplug
         0000000000000ce4-0000000000000cf7 (prio 0, i/o): io @0000000000000ce4
         0000000000000cf8-0000000000000cf8 (prio 0, i/o): pci-conf-idx
         0000000000000cf9-0000000000000cf9 (prio 1, i/o): lpc-reset-control
         0000000000000cfa-0000000000000cfb (prio 0, i/o): pci-conf-idx @0000000000000002
         0000000000000cfc-0000000000000cff (prio 0, i/o): pci-conf-data
         0000000000000d00-000000000000dfff (prio 0, i/o): io @0000000000000d00
         000000000000e000-000000000000e07f (prio 0, i/o): 0000:0c:00.0 BAR 5
         000000000000e080-000000000000efff (prio 0, i/o): io @000000000000e080
         000000000000f000-000000000000f03f (prio 1, i/o): pm-smbus
         000000000000f040-000000000000f05f (prio 1, i/o): ahci-idp
         000000000000f060-000000000000ffff (prio 0, i/o): io @000000000000f060

       FlatView #1
        AS \"memory\", root: system
        AS \"cpu-memory-0\", root: system
        AS \"cpu-memory-1\", root: system
        AS \"cpu-memory-2\", root: system
        AS \"cpu-memory-3\", root: system
        AS \"cpu-memory-4\", root: system
        AS \"cpu-memory-5\", root: system
        AS \"cpu-memory-6\", root: system
        AS \"cpu-memory-7\", root: system
        AS \"cpu-memory-8\", root: system
        AS \"cpu-memory-9\", root: system
        AS \"cpu-memory-10\", root: system
        AS \"cpu-memory-11\", root: system
        AS \"cpu-memory-12\", root: system
        AS \"cpu-memory-13\", root: system
        AS \"cpu-memory-14\", root: system
        AS \"cpu-memory-15\", root: system
        AS \"mch\", root: bus master container
        AS \"ICH9-LPC\", root: bus master container
        AS \"ich9-ahci\", root: bus master container
        AS \"ICH9-SMB\", root: bus master container
        AS \"pcie-root-port\", root: bus master container
        AS \"pcie-root-port\", root: bus master container
        AS \"pcie-root-port\", root: bus master container
        AS \"pcie-root-port\", root: bus master container
        AS \"pcie-root-port\", root: bus master container
        AS \"pcie-root-port\", root: bus master container
        AS \"pcie-root-port\", root: bus master container
        AS \"pcie-root-port\", root: bus master container
        AS \"pcie-root-port\", root: bus master container
        AS \"pcie-root-port\", root: bus master container
        AS \"pcie-root-port\", root: bus master container
        AS \"pcie-root-port\", root: bus master container
        AS \"pcie-root-port\", root: bus master container
        AS \"pcie-pci-bridge\", root: bus master container
        AS \"qemu-xhci\", root: bus master container
        AS \"virtio-blk-pci\", root: bus master container
        AS \"virtio-blk-pci\", root: bus master container
        AS \"virtio-net-pci\", root: bus master container
        AS \"virtio-mouse-pci\", root: bus master container
        AS \"virtio-keyboard-pci\", root: bus master container
        AS \"vfio-pci\", root: bus master container
        AS \"vfio-pci\", root: bus master container
        AS \"vfio-pci\", root: bus master container
        AS \"vfio-pci\", root: bus master container
        AS \"vfio-pci\", root: bus master container
        AS \"ich9-intel-hda\", root: bus master container
        Root memory region: system
         0000000000000000-00000000000bffff (prio 0, ram): pc.ram KVM
         00000000000c0000-00000000000dffff (prio 1, rom): pc.rom KVM
         00000000000e0000-00000000000fffff (prio 1, rom): isa-bios KVM
         0000000000100000-0000000000102fff (prio 0, ram): pc.ram @0000000000100000 KVM
         0000000000103000-0000000000103fff (prio 0, ram): synic-0-msg-page KVM
         0000000000104000-0000000000104fff (prio 0, ram): synic-1-msg-page KVM
         0000000000105000-0000000000105fff (prio 0, ram): synic-2-msg-page KVM
         0000000000106000-0000000000106fff (prio 0, ram): synic-3-msg-page KVM
         0000000000107000-0000000000107fff (prio 0, ram): synic-4-msg-page KVM
         0000000000108000-0000000000108fff (prio 0, ram): synic-5-msg-page KVM
         0000000000109000-0000000000109fff (prio 0, ram): synic-6-msg-page KVM
         000000000010a000-000000000010afff (prio 0, ram): synic-7-msg-page KVM
         000000000010b000-000000000010bfff (prio 0, ram): synic-8-msg-page KVM
         000000000010c000-000000000010cfff (prio 0, ram): synic-9-msg-page KVM
         000000000010d000-000000000010dfff (prio 0, ram): synic-10-msg-page KVM
         000000000010e000-000000000010efff (prio 0, ram): synic-11-msg-page KVM
         000000000010f000-000000000010ffff (prio 0, ram): synic-12-msg-page KVM
         0000000000110000-0000000000110fff (prio 0, ram): synic-13-msg-page KVM
         0000000000111000-0000000000111fff (prio 0, ram): synic-14-msg-page KVM
         0000000000112000-0000000000112fff (prio 0, ram): synic-15-msg-page KVM
         0000000000113000-000000007fffffff (prio 0, ram): pc.ram @0000000000113000 KVM
         00000000b0000000-00000000bfffffff (prio 0, i/o): pcie-mmcfg-mmio
         00000000c0000000-00000000c0087fff (prio 0, ramd): 0000:0c:00.0 BAR 0 mmaps[0] KVM
         00000000c0088000-00000000c0088fff (prio 1, i/o): vfio-nvidia-bar0-88000-mirror-quirk
         00000000c0089000-00000000c0ffffff (prio 0, ramd): 0000:0c:00.0 BAR 0 mmaps[0] @0000000000089000 KVM
         00000000c1200000-00000000c120008f (prio 0, i/o): msix-table
         00000000c1200800-00000000c1200807 (prio 0, i/o): msix-pba
         00000000c1400000-00000000c140001f (prio 0, i/o): msix-table
         00000000c1400800-00000000c1400807 (prio 0, i/o): msix-pba
         00000000c1600000-00000000c160001f (prio 0, i/o): msix-table
         00000000c1600800-00000000c1600807 (prio 0, i/o): msix-pba
         00000000c1800000-00000000c180008f (prio 0, i/o): msix-table
         00000000c1800800-00000000c1800807 (prio 0, i/o): msix-pba
         00000000c1a00000-00000000c1afdfff (prio 0, ramd): 0000:0e:00.3 BAR 0 mmaps[0] KVM
         00000000c1afe000-00000000c1afe07f (prio 0, i/o): msix-table
         00000000c1afe080-00000000c1afffff (prio 0, ramd): 0000:0e:00.3 BAR 0 mmaps[0] @00000000000fe080
         00000000c1e00000-00000000c1e0009f (prio 0, i/o): shpc-mmio
         00000000c2000000-00000000c2000fff (prio 0, ramd): 0000:0c:00.3 BAR 0 mmaps[0] KVM
         00000000c2200000-00000000c2203fff (prio 0, ramd): 0000:0c:00.1 BAR 0 mmaps[0] KVM
         00000000c2300000-00000000c230003f (prio 0, i/o): capabilities
         00000000c2300040-00000000c230043f (prio 0, i/o): operational
         00000000c2300440-00000000c230044f (prio 0, i/o): usb3 port #1
         00000000c2300450-00000000c230045f (prio 0, i/o): usb3 port #2
         00000000c2300460-00000000c230046f (prio 0, i/o): usb3 port #3
         00000000c2300470-00000000c230047f (prio 0, i/o): usb3 port #4
         00000000c2300480-00000000c230048f (prio 0, i/o): usb2 port #1
         00000000c2300490-00000000c230049f (prio 0, i/o): usb2 port #2
         00000000c23004a0-00000000c23004af (prio 0, i/o): usb2 port #3
         00000000c23004b0-00000000c23004bf (prio 0, i/o): usb2 port #4
         00000000c2301000-00000000c230121f (prio 0, i/o): runtime
         00000000c2302000-00000000c230281f (prio 0, i/o): doorbell
         00000000c2303000-00000000c23030ff (prio 0, i/o): msix-table
         00000000c2303800-00000000c2303807 (prio 0, i/o): msix-pba
         00000000c2400000-00000000c240011f (prio 0, i/o): msix-table
         00000000c2400800-00000000c2400807 (prio 0, i/o): msix-pba
         00000000c2580000-00000000c2581fff (prio 0, i/o): intel-hda
         00000000c2582000-00000000c2583fff (prio 0, i/o): intel-hda
         00000000c2584000-00000000c2584fff (prio 1, i/o): ahci
         00000000c2585000-00000000c258500f (prio 0, i/o): msix-table
         00000000c2585800-00000000c2585807 (prio 0, i/o): msix-pba
         00000000c2586000-00000000c258600f (prio 0, i/o): msix-table
         00000000c2586800-00000000c2586807 (prio 0, i/o): msix-pba
         00000000c2587000-00000000c258700f (prio 0, i/o): msix-table
         00000000c2587800-00000000c2587807 (prio 0, i/o): msix-pba
         00000000c2588000-00000000c258800f (prio 0, i/o): msix-table
         00000000c2588800-00000000c2588807 (prio 0, i/o): msix-pba
         00000000c2589000-00000000c258900f (prio 0, i/o): msix-table
         00000000c2589800-00000000c2589807 (prio 0, i/o): msix-pba
         00000000c258a000-00000000c258a00f (prio 0, i/o): msix-table
         00000000c258a800-00000000c258a807 (prio 0, i/o): msix-pba
         00000000c258b000-00000000c258b00f (prio 0, i/o): msix-table
         00000000c258b800-00000000c258b807 (prio 0, i/o): msix-pba
         00000000c258c000-00000000c258c00f (prio 0, i/o): msix-table
         00000000c258c800-00000000c258c807 (prio 0, i/o): msix-pba
         00000000c258d000-00000000c258d00f (prio 0, i/o): msix-table
         00000000c258d800-00000000c258d807 (prio 0, i/o): msix-pba
         00000000c258e000-00000000c258e00f (prio 0, i/o): msix-table
         00000000c258e800-00000000c258e807 (prio 0, i/o): msix-pba
         00000000c258f000-00000000c258f00f (prio 0, i/o): msix-table
         00000000c258f800-00000000c258f807 (prio 0, i/o): msix-pba
         00000000c2590000-00000000c259000f (prio 0, i/o): msix-table
         00000000c2590800-00000000c2590807 (prio 0, i/o): msix-pba
         00000000c2591000-00000000c259100f (prio 0, i/o): msix-table
         00000000c2591800-00000000c2591807 (prio 0, i/o): msix-pba
         00000000fec00000-00000000fec00fff (prio 0, i/o): kvm-ioapic
         00000000fed1c000-00000000fed1ffff (prio 1, i/o): lpc-rcrb-mmio
         00000000fee00000-00000000feefffff (prio 4096, i/o): kvm-apic-msi
         00000000ffe00000-00000000ffe1ffff (prio 0, romd): system.flash1 KVM
         00000000ffe20000-00000000ffffffff (prio 0, romd): system.flash0 KVM
         0000000100000000-000000047fffffff (prio 0, ram): pc.ram @0000000080000000 KVM
         0000000800000000-000000080fffffff (prio 0, ramd): 0000:0c:00.0 BAR 1 mmaps[0] KVM
         0000000810000000-0000000811ffffff (prio 0, ramd): 0000:0c:00.0 BAR 3 mmaps[0] KVM
         0000000812000000-0000000812000fff (prio 0, i/o): virtio-pci-common-virtio-net
         0000000812001000-0000000812001fff (prio 0, i/o): virtio-pci-isr-virtio-net
         0000000812002000-0000000812002fff (prio 0, i/o): virtio-pci-device-virtio-net
         0000000812003000-0000000812003fff (prio 0, i/o): virtio-pci-notify-virtio-net
         0000000812100000-000000081213ffff (prio 0, ramd): 0000:0c:00.2 BAR 0 mmaps[0] KVM
         0000000812140000-000000081214ffff (prio 0, ramd): 0000:0c:00.2 BAR 3 mmaps[0] KVM
         0000000812200000-0000000812200fff (prio 0, i/o): virtio-pci-common-virtio-blk
         0000000812201000-0000000812201fff (prio 0, i/o): virtio-pci-isr-virtio-blk
         0000000812202000-0000000812202fff (prio 0, i/o): virtio-pci-device-virtio-blk
         0000000812203000-0000000812203fff (prio 0, i/o): virtio-pci-notify-virtio-blk
         0000000812300000-0000000812300fff (prio 0, i/o): virtio-pci-common-virtio-input
         0000000812301000-0000000812301fff (prio 0, i/o): virtio-pci-isr-virtio-input
         0000000812302000-0000000812302fff (prio 0, i/o): virtio-pci-device-virtio-input
         0000000812303000-0000000812303fff (prio 0, i/o): virtio-pci-notify-virtio-input
         0000000812400000-0000000812400fff (prio 0, i/o): virtio-pci-common-virtio-input
         0000000812401000-0000000812401fff (prio 0, i/o): virtio-pci-isr-virtio-input
         0000000812402000-0000000812402fff (prio 0, i/o): virtio-pci-device-virtio-input
         0000000812403000-0000000812403fff (prio 0, i/o): virtio-pci-notify-virtio-input
         0000000812500000-0000000812500fff (prio 0, i/o): virtio-pci-common-virtio-blk
         0000000812501000-0000000812501fff (prio 0, i/o): virtio-pci-isr-virtio-blk
         0000000812502000-0000000812502fff (prio 0, i/o): virtio-pci-device-virtio-blk
         0000000812503000-0000000812503fff (prio 0, i/o): virtio-pci-notify-virtio-blk

       FlatView #2
        AS \"KVM-SMRAM\", root: mem-container-smram
        Root memory region: mem-container-smram
         0000000000000000-00000000000bffff (prio 0, ram): pc.ram KVM
         00000000000c0000-00000000000dffff (prio 1, rom): pc.rom KVM
         00000000000e0000-00000000000fffff (prio 1, rom): isa-bios KVM
         0000000000100000-0000000000102fff (prio 0, ram): pc.ram @0000000000100000 KVM
         0000000000103000-0000000000103fff (prio 0, ram): synic-0-msg-page KVM
         0000000000104000-0000000000104fff (prio 0, ram): synic-1-msg-page KVM
         0000000000105000-0000000000105fff (prio 0, ram): synic-2-msg-page KVM
         0000000000106000-0000000000106fff (prio 0, ram): synic-3-msg-page KVM
         0000000000107000-0000000000107fff (prio 0, ram): synic-4-msg-page KVM
         0000000000108000-0000000000108fff (prio 0, ram): synic-5-msg-page KVM
         0000000000109000-0000000000109fff (prio 0, ram): synic-6-msg-page KVM
         000000000010a000-000000000010afff (prio 0, ram): synic-7-msg-page KVM
         000000000010b000-000000000010bfff (prio 0, ram): synic-8-msg-page KVM
         000000000010c000-000000000010cfff (prio 0, ram): synic-9-msg-page KVM
         000000000010d000-000000000010dfff (prio 0, ram): synic-10-msg-page KVM
         000000000010e000-000000000010efff (prio 0, ram): synic-11-msg-page KVM
         000000000010f000-000000000010ffff (prio 0, ram): synic-12-msg-page KVM
         0000000000110000-0000000000110fff (prio 0, ram): synic-13-msg-page KVM
         0000000000111000-0000000000111fff (prio 0, ram): synic-14-msg-page KVM
         0000000000112000-0000000000112fff (prio 0, ram): synic-15-msg-page KVM
         0000000000113000-000000007fffffff (prio 0, ram): pc.ram @0000000000113000 KVM
         00000000b0000000-00000000bfffffff (prio 0, i/o): pcie-mmcfg-mmio
         00000000c0000000-00000000c0087fff (prio 0, ramd): 0000:0c:00.0 BAR 0 mmaps[0] KVM
         00000000c0088000-00000000c0088fff (prio 1, i/o): vfio-nvidia-bar0-88000-mirror-quirk
         00000000c0089000-00000000c0ffffff (prio 0, ramd): 0000:0c:00.0 BAR 0 mmaps[0] @0000000000089000 KVM
         00000000c1200000-00000000c120008f (prio 0, i/o): msix-table
         00000000c1200800-00000000c1200807 (prio 0, i/o): msix-pba
         00000000c1400000-00000000c140001f (prio 0, i/o): msix-table
         00000000c1400800-00000000c1400807 (prio 0, i/o): msix-pba
         00000000c1600000-00000000c160001f (prio 0, i/o): msix-table
         00000000c1600800-00000000c1600807 (prio 0, i/o): msix-pba
         00000000c1800000-00000000c180008f (prio 0, i/o): msix-table
         00000000c1800800-00000000c1800807 (prio 0, i/o): msix-pba
         00000000c1a00000-00000000c1afdfff (prio 0, ramd): 0000:0e:00.3 BAR 0 mmaps[0] KVM
         00000000c1afe000-00000000c1afe07f (prio 0, i/o): msix-table
         00000000c1afe080-00000000c1afffff (prio 0, ramd): 0000:0e:00.3 BAR 0 mmaps[0] @00000000000fe080
         00000000c1e00000-00000000c1e0009f (prio 0, i/o): shpc-mmio
         00000000c2000000-00000000c2000fff (prio 0, ramd): 0000:0c:00.3 BAR 0 mmaps[0] KVM
         00000000c2200000-00000000c2203fff (prio 0, ramd): 0000:0c:00.1 BAR 0 mmaps[0] KVM
         00000000c2300000-00000000c230003f (prio 0, i/o): capabilities
         00000000c2300040-00000000c230043f (prio 0, i/o): operational
         00000000c2300440-00000000c230044f (prio 0, i/o): usb3 port #1
         00000000c2300450-00000000c230045f (prio 0, i/o): usb3 port #2
         00000000c2300460-00000000c230046f (prio 0, i/o): usb3 port #3
         00000000c2300470-00000000c230047f (prio 0, i/o): usb3 port #4
         00000000c2300480-00000000c230048f (prio 0, i/o): usb2 port #1
         00000000c2300490-00000000c230049f (prio 0, i/o): usb2 port #2
         00000000c23004a0-00000000c23004af (prio 0, i/o): usb2 port #3
         00000000c23004b0-00000000c23004bf (prio 0, i/o): usb2 port #4
         00000000c2301000-00000000c230121f (prio 0, i/o): runtime
         00000000c2302000-00000000c230281f (prio 0, i/o): doorbell
         00000000c2303000-00000000c23030ff (prio 0, i/o): msix-table
         00000000c2303800-00000000c2303807 (prio 0, i/o): msix-pba
         00000000c2400000-00000000c240011f (prio 0, i/o): msix-table
         00000000c2400800-00000000c2400807 (prio 0, i/o): msix-pba
         00000000c2580000-00000000c2581fff (prio 0, i/o): intel-hda
         00000000c2582000-00000000c2583fff (prio 0, i/o): intel-hda
         00000000c2584000-00000000c2584fff (prio 1, i/o): ahci
         00000000c2585000-00000000c258500f (prio 0, i/o): msix-table
         00000000c2585800-00000000c2585807 (prio 0, i/o): msix-pba
         00000000c2586000-00000000c258600f (prio 0, i/o): msix-table
         00000000c2586800-00000000c2586807 (prio 0, i/o): msix-pba
         00000000c2587000-00000000c258700f (prio 0, i/o): msix-table
         00000000c2587800-00000000c2587807 (prio 0, i/o): msix-pba
         00000000c2588000-00000000c258800f (prio 0, i/o): msix-table
         00000000c2588800-00000000c2588807 (prio 0, i/o): msix-pba
         00000000c2589000-00000000c258900f (prio 0, i/o): msix-table
         00000000c2589800-00000000c2589807 (prio 0, i/o): msix-pba
         00000000c258a000-00000000c258a00f (prio 0, i/o): msix-table
         00000000c258a800-00000000c258a807 (prio 0, i/o): msix-pba
         00000000c258b000-00000000c258b00f (prio 0, i/o): msix-table
         00000000c258b800-00000000c258b807 (prio 0, i/o): msix-pba
         00000000c258c000-00000000c258c00f (prio 0, i/o): msix-table
         00000000c258c800-00000000c258c807 (prio 0, i/o): msix-pba
         00000000c258d000-00000000c258d00f (prio 0, i/o): msix-table
         00000000c258d800-00000000c258d807 (prio 0, i/o): msix-pba
         00000000c258e000-00000000c258e00f (prio 0, i/o): msix-table
         00000000c258e800-00000000c258e807 (prio 0, i/o): msix-pba
         00000000c258f000-00000000c258f00f (prio 0, i/o): msix-table
         00000000c258f800-00000000c258f807 (prio 0, i/o): msix-pba
         00000000c2590000-00000000c259000f (prio 0, i/o): msix-table
         00000000c2590800-00000000c2590807 (prio 0, i/o): msix-pba
         00000000c2591000-00000000c259100f (prio 0, i/o): msix-table
         00000000c2591800-00000000c2591807 (prio 0, i/o): msix-pba
         00000000fec00000-00000000fec00fff (prio 0, i/o): kvm-ioapic
         00000000fed1c000-00000000fed1ffff (prio 1, i/o): lpc-rcrb-mmio
         00000000fee00000-00000000feefffff (prio 4096, i/o): kvm-apic-msi
         00000000ffe00000-00000000ffe1ffff (prio 0, romd): system.flash1 KVM
         00000000ffe20000-00000000ffffffff (prio 0, romd): system.flash0 KVM
         0000000100000000-000000047fffffff (prio 0, ram): pc.ram @0000000080000000 KVM
         0000000800000000-000000080fffffff (prio 0, ramd): 0000:0c:00.0 BAR 1 mmaps[0] KVM
         0000000810000000-0000000811ffffff (prio 0, ramd): 0000:0c:00.0 BAR 3 mmaps[0] KVM
         0000000812000000-0000000812000fff (prio 0, i/o): virtio-pci-common-virtio-net
         0000000812001000-0000000812001fff (prio 0, i/o): virtio-pci-isr-virtio-net
         0000000812002000-0000000812002fff (prio 0, i/o): virtio-pci-device-virtio-net
         0000000812003000-0000000812003fff (prio 0, i/o): virtio-pci-notify-virtio-net
         0000000812100000-000000081213ffff (prio 0, ramd): 0000:0c:00.2 BAR 0 mmaps[0] KVM
         0000000812140000-000000081214ffff (prio 0, ramd): 0000:0c:00.2 BAR 3 mmaps[0] KVM
         0000000812200000-0000000812200fff (prio 0, i/o): virtio-pci-common-virtio-blk
         0000000812201000-0000000812201fff (prio 0, i/o): virtio-pci-isr-virtio-blk
         0000000812202000-0000000812202fff (prio 0, i/o): virtio-pci-device-virtio-blk
         0000000812203000-0000000812203fff (prio 0, i/o): virtio-pci-notify-virtio-blk
         0000000812300000-0000000812300fff (prio 0, i/o): virtio-pci-common-virtio-input
         0000000812301000-0000000812301fff (prio 0, i/o): virtio-pci-isr-virtio-input
         0000000812302000-0000000812302fff (prio 0, i/o): virtio-pci-device-virtio-input
         0000000812303000-0000000812303fff (prio 0, i/o): virtio-pci-notify-virtio-input
         0000000812400000-0000000812400fff (prio 0, i/o): virtio-pci-common-virtio-input
         0000000812401000-0000000812401fff (prio 0, i/o): virtio-pci-isr-virtio-input
         0000000812402000-0000000812402fff (prio 0, i/o): virtio-pci-device-virtio-input
         0000000812403000-0000000812403fff (prio 0, i/o): virtio-pci-notify-virtio-input
         0000000812500000-0000000812500fff (prio 0, i/o): virtio-pci-common-virtio-blk
         0000000812501000-0000000812501fff (prio 0, i/o): virtio-pci-isr-virtio-blk
         0000000812502000-0000000812502fff (prio 0, i/o): virtio-pci-device-virtio-blk
         0000000812503000-0000000812503fff (prio 0, i/o): virtio-pci-notify-virtio-blk"#;

        let mappings = qmp_parse_mtree(mtreestr);

        assert_eq!(mappings.len(), 4);

        assert_eq!(mappings[0].range_start, 0);
        assert_eq!(mappings[0].range_end, 0xc0000);
        assert_eq!(mappings[0].remap_start, 0);

        assert_eq!(mappings[1].range_start, 0x100000);
        assert_eq!(mappings[1].range_end, 0x103000);
        assert_eq!(mappings[1].remap_start, 0x100000);

        assert_eq!(mappings[2].range_start, 0x113000);
        assert_eq!(mappings[2].range_end, 0x80000000);
        assert_eq!(mappings[2].remap_start, 0x113000);

        assert_eq!(mappings[3].range_start, 0x100000000);
        assert_eq!(mappings[3].range_end, 0x480000000);
        assert_eq!(mappings[3].remap_start, 0x80000000);
    }
}
