use std::prelude::v1::*;

use super::{Win32Kernel, Win32ModuleListInfo};

use crate::prelude::MmVadOffsetTable;

use std::fmt;

use memflow::mem::virt_translate::*;
use memflow::prelude::v1::{Result, *};

// those only required when compiling cglue code
#[cfg(feature = "plugins")]
use memflow::cglue;

use super::Win32VirtualTranslate;

/// Exit status of a win32 process
pub type Win32ExitStatus = i32;

/// Process has not exited yet
pub const EXIT_STATUS_STILL_ACTIVE: i32 = 259;

/// EPROCESS ImageFileName byte length
pub const IMAGE_FILE_NAME_LENGTH: usize = 15;

#[derive(Debug, Clone)]
#[cfg_attr(feature = "serde", derive(::serde::Serialize))]
pub struct Win32ProcessInfo {
    pub base_info: ProcessInfo,

    // general information from eprocess
    pub section_base: Address,
    pub ethread: Address,
    pub wow64: Address,

    // teb
    pub teb: Option<Address>,
    pub teb_wow64: Option<Address>,

    // peb
    pub peb_native: Option<Address>,
    pub peb_wow64: Option<Address>,

    // modules
    pub module_info_native: Option<Win32ModuleListInfo>,
    pub module_info_wow64: Option<Win32ModuleListInfo>,

    // memory
    pub vad_root: Address,
}

impl Win32ProcessInfo {
    pub fn wow64(&self) -> Address {
        self.wow64
    }

    pub fn peb(&self) -> Option<Address> {
        if let Some(peb) = self.peb_wow64 {
            Some(peb)
        } else {
            self.peb_native
        }
    }

    pub fn peb_native(&self) -> Option<Address> {
        self.peb_native
    }

    pub fn peb_wow64(&self) -> Option<Address> {
        self.peb_wow64
    }

    /// Return the module list information of process native architecture
    ///
    /// If the process is a wow64 process, module_info_wow64 is returned, otherwise, module_info_native is
    /// returned.
    pub fn module_info(&self) -> Option<Win32ModuleListInfo> {
        if !self.wow64.is_null() {
            self.module_info_wow64
        } else {
            self.module_info_native
        }
    }

    pub fn module_info_native(&self) -> Option<Win32ModuleListInfo> {
        self.module_info_native
    }

    pub fn module_info_wow64(&self) -> Option<Win32ModuleListInfo> {
        self.module_info_wow64
    }

    pub fn translator(&self) -> Win32VirtualTranslate {
        Win32VirtualTranslate::new(self.base_info.sys_arch, self.base_info.dtb1)
    }
}

#[cfg(feature = "plugins")]
cglue_impl_group!(Win32Process<T, V, D>, ProcessInstance, { VirtualTranslate });
#[cfg(feature = "plugins")]
cglue_impl_group!(Win32Process<T, V, D>, IntoProcessInstance, { VirtualTranslate });

pub struct Win32Process<T, V, D> {
    pub virt_mem: VirtualDma<T, V, D>,
    pub proc_info: Win32ProcessInfo,

    sysproc_dtb: D,
    offset_eproc_exit_status: usize,
    mmvad: MmVadOffsetTable,
}

// TODO: can be removed i think
impl<T: Clone, V: Clone, D: Clone> Clone for Win32Process<T, V, D> {
    fn clone(&self) -> Self {
        Self {
            virt_mem: self.virt_mem.clone(),
            proc_info: self.proc_info.clone(),
            sysproc_dtb: self.sysproc_dtb.clone(),
            offset_eproc_exit_status: self.offset_eproc_exit_status,
            mmvad: self.mmvad,
        }
    }
}

impl<T, V, D> AsMut<VirtualDma<T, V, D>> for Win32Process<T, V, D> {
    fn as_mut(&mut self) -> &mut VirtualDma<T, V, D> {
        &mut self.virt_mem
    }
}

impl<T: PhysicalMemory, V: VirtualTranslate2, D: VirtualTranslate3> MemoryView
    for Win32Process<T, V, D>
{
    fn read_raw_iter(&mut self, data: ReadRawMemOps) -> Result<()> {
        self.virt_mem.read_raw_iter(data)
    }

    fn write_raw_iter(&mut self, data: WriteRawMemOps) -> Result<()> {
        self.virt_mem.write_raw_iter(data)
    }

    fn metadata(&self) -> MemoryViewMetadata {
        self.virt_mem.metadata()
    }
}

impl<T: PhysicalMemory, V: VirtualTranslate2, D: VirtualTranslate3> VirtualTranslate
    for Win32Process<T, V, D>
{
    fn virt_to_phys_list(
        &mut self,
        addrs: &[VtopRange],
        out: VirtualTranslationCallback,
        out_fail: VirtualTranslationFailCallback,
    ) {
        self.virt_mem.virt_to_phys_list(addrs, out, out_fail)
    }
}

// TODO: implement VAD and rollback to the old bound!
//impl<T: MemoryView> Process for Win32Process<T> {

impl<T: PhysicalMemory, V: VirtualTranslate2> Process
    for Win32Process<T, V, Win32VirtualTranslate>
{
    /// Retrieves virtual address translator for the process (if applicable)
    //fn vat(&mut self) -> Option<&mut Self::VirtualTranslateType>;

    /// Retrieves the state of the process
    fn state(&mut self) -> ProcessState {
        if let Ok(exit_status) = self.virt_mem.read::<Win32ExitStatus>(
            self.proc_info.base_info.address + self.offset_eproc_exit_status,
        ) {
            if exit_status == EXIT_STATUS_STILL_ACTIVE {
                ProcessState::Alive
            } else {
                ProcessState::Dead(exit_status)
            }
        } else {
            ProcessState::Unknown
        }
    }

    /// Changes the dtb this process uses for memory translations
    ///
    /// # Remarks
    ///
    /// For memflow-win32 the second parameter should be set to `Address::invalid()`.
    fn set_dtb(&mut self, dtb1: Address, _dtb2: Address) -> Result<()> {
        self.proc_info.base_info.dtb1 = dtb1;
        self.proc_info.base_info.dtb2 = Address::invalid();
        self.virt_mem.set_translator(self.proc_info.translator());
        Ok(())
    }

    /// Walks the process' module list and calls the provided callback for each module
    fn module_address_list_callback(
        &mut self,
        target_arch: Option<&ArchitectureIdent>,
        mut callback: ModuleAddressCallback,
    ) -> memflow::error::Result<()> {
        let infos = [
            (
                self.proc_info.module_info_native,
                self.proc_info.base_info.sys_arch,
            ),
            (
                self.proc_info.module_info_wow64,
                self.proc_info.base_info.proc_arch,
            ),
        ];

        // Here we end up filtering out module_info_wow64 if it doesn't exist
        let iter = infos
            .iter()
            .filter(|(_, a)| {
                if let Some(ta) = target_arch {
                    a == ta
                } else {
                    true
                }
            })
            .cloned()
            .filter_map(|(info, arch)| info.zip(Some(arch)));

        self.module_address_list_with_infos_callback(iter, &mut callback)
            .map_err(From::from)
    }

    /// Retrieves a module by its structure address and architecture
    ///
    /// # Arguments
    /// * `address` - address where module's information resides in
    /// * `architecture` - architecture of the module. Should be either `ProcessInfo::proc_arch`, or `ProcessInfo::sys_arch`.
    fn module_by_address(
        &mut self,
        address: Address,
        architecture: ArchitectureIdent,
    ) -> memflow::error::Result<ModuleInfo> {
        let info = if architecture == self.proc_info.base_info.sys_arch {
            self.proc_info.module_info_native.as_mut()
        } else if architecture == self.proc_info.base_info.proc_arch {
            self.proc_info.module_info_wow64.as_mut()
        } else {
            None
        }
        .ok_or(Error(ErrorOrigin::OsLayer, ErrorKind::InvalidArchitecture))?;

        info.module_info_from_entry(
            address,
            self.proc_info.base_info.address,
            &mut self.virt_mem,
            architecture,
        )
        .map_err(From::from)
    }

    /// Retrieves address of the primary module structure of the process
    ///
    /// This will be the module of the executable that is being run, and whose name is stored in
    /// _EPROCESS::IMAGE_FILE_NAME
    fn primary_module_address(&mut self) -> memflow::error::Result<Address> {
        let mut ret = Err(Error(ErrorOrigin::OsLayer, ErrorKind::ModuleNotFound));
        let sptr = self as *mut Self;
        let callback = &mut |ModuleAddressInfo { address, arch }| {
            let s = unsafe { sptr.as_mut() }.unwrap();
            let info = if arch == s.proc_info.base_info.sys_arch {
                s.proc_info.module_info_native.as_mut()
            } else {
                s.proc_info.module_info_wow64.as_mut()
            }
            .unwrap();

            if let Ok((_, true)) = info
                .module_base_from_entry(address, &mut s.virt_mem, arch)
                .map(|b| (b, b == s.proc_info.section_base))
            {
                ret = Ok(address);
                false
            } else {
                true
            }
        };
        let proc_arch = self.proc_info.base_info.proc_arch;
        self.module_address_list_callback(Some(&proc_arch), callback.into())?;
        ret
    }

    fn module_import_list_callback(
        &mut self,
        info: &ModuleInfo,
        callback: ImportCallback,
    ) -> Result<()> {
        memflow::os::util::module_import_list_callback(&mut self.virt_mem, info, callback)
    }

    fn module_export_list_callback(
        &mut self,
        info: &ModuleInfo,
        callback: ExportCallback,
    ) -> Result<()> {
        memflow::os::util::module_export_list_callback(&mut self.virt_mem, info, callback)
    }

    fn module_section_list_callback(
        &mut self,
        info: &ModuleInfo,
        callback: SectionCallback,
    ) -> Result<()> {
        memflow::os::util::module_section_list_callback(&mut self.virt_mem, info, callback)
    }

    /// Retrieves the process info
    fn info(&self) -> &ProcessInfo {
        &self.proc_info.base_info
    }

    fn mapped_mem_range(
        &mut self,
        gap_size: imem,
        start: Address,
        end: Address,
        out: MemoryRangeCallback,
    ) {
        fn _walk_vad(
            mem: &mut impl MemoryView,
            vad_entry: Address,
            offsets: &MmVadOffsetTable,
            arch: ArchitectureObj,
            start: Address,
            end: Address,
            out: &mut MemoryRangeCallback,
        ) {
            if vad_entry.is_null() || start == end {
                return;
            }

            log::trace!("WALK VAD {vad_entry} {start} {end}");

            let _ = (move || {
                // Older versions of windows store starting/ending VPNs as address ranges without
                // the high parts, as opposed to frame numbers
                let pfn_mul = if offsets.starting_vpn_high == offsets.ending_vpn_high {
                    1
                } else {
                    0x1000
                };

                // TODO: handle starting/ending vpn high values

                let s = mem.read::<u32>(vad_entry + offsets.starting_vpn)? as umem;
                let s = Address::from(s * pfn_mul);
                let e = mem.read::<u32>(vad_entry + offsets.ending_vpn)? as umem;
                let e = Address::from(e * pfn_mul);

                let sl = mem.read::<u8>(vad_entry + offsets.starting_vpn_high)? as umem;
                let el = mem.read::<u8>(vad_entry + offsets.ending_vpn_high)? as umem;

                let fl = mem.read::<u32>(vad_entry + offsets.u)?;

                // Bits are as follows:
                // RXW (maybe)
                println!("FL {fl:b} | {}", offsets.protection_bit);

                let _r = fl & (0b1 << offsets.protection_bit);

                let fl = fl >> offsets.protection_bit;

                println!("FL {fl:b}");

                let fl = fl & !(!0u32 << 5);

                println!("S {s} E {e} | {sl:x} {el:x} | {fl:b} {fl}");

                if (s >= start && s < end) || (e <= end && e > start) {
                    let left = mem.read_addr_arch(arch, vad_entry + offsets.vad_node)?;
                    let right =
                        mem.read_addr_arch(arch, vad_entry + offsets.vad_node + arch.size_addr())?;

                    _walk_vad(mem, left, offsets, arch, start, s, out);

                    if !out.call(CTup3(
                        s,
                        e.to_umem() - s.to_umem() + pfn_mul,
                        Default::default(),
                    )) {
                        return Result::Ok(());
                    }

                    _walk_vad(mem, right, offsets, arch, e, end, out);
                }

                Result::Ok(())
            })();
        }

        /*let mut gap_remover = memflow::types::util::GapRemover::new(out, gap_size, start, end);

        // Temporarily load up the sysproc dtb into the memory view
        self.sysproc_dtb = self.virt_mem.set_translator(self.sysproc_dtb);

        let out = &mut |data| {
            gap_remover.push_range(data);
            true
        };

        let mut out = out.into();

        _walk_vad(
            &mut self.virt_mem,
            self.proc_info.vad_root,
            &self.mmvad,
            self.proc_info.base_info.sys_arch.into(),
            start,
            end,
            &mut out,
        );

        // Load back the original value
        self.sysproc_dtb = self.virt_mem.set_translator(self.sysproc_dtb);*/

        self.virt_mem.virt_page_map_range(gap_size, start, end, out)
    }
}

// TODO: replace the following impls with a dedicated builder
// TODO: add non cloneable thing
impl<T: PhysicalMemory, V: VirtualTranslate2> Win32Process<T, V, Win32VirtualTranslate> {
    pub fn with_kernel(kernel: Win32Kernel<T, V>, proc_info: Win32ProcessInfo) -> Self {
        let mut virt_mem = kernel.virt_mem;
        virt_mem.set_proc_arch(proc_info.base_info.proc_arch.into());
        let sysproc_dtb = virt_mem.set_translator(proc_info.translator());

        Self {
            virt_mem,
            proc_info,
            sysproc_dtb,
            mmvad: kernel.offsets.mm_vad(),
            offset_eproc_exit_status: kernel.offsets.eproc_exit_status(),
        }
    }

    /// Consumes this process, returning the underlying memory and vat objects
    pub fn into_inner(self) -> (T, V) {
        self.virt_mem.into_inner()
    }
}

impl<'a, T: PhysicalMemory, V: VirtualTranslate2>
    Win32Process<Fwd<&'a mut T>, Fwd<&'a mut V>, Win32VirtualTranslate>
{
    /// Constructs a new process by borrowing a kernel object.
    ///
    /// Internally this will create a `VirtualDma` object that also
    /// borrows the PhysicalMemory and Vat objects from the kernel.
    ///
    /// The resulting process object is NOT cloneable due to the mutable borrowing.
    ///
    /// When u need a cloneable Process u have to use the `::with_kernel` function
    /// which will move the kernel object.
    pub fn with_kernel_ref(kernel: &'a mut Win32Kernel<T, V>, proc_info: Win32ProcessInfo) -> Self {
        let sysproc_dtb = *kernel.virt_mem.translator();

        let (phys_mem, vat) = kernel.virt_mem.mem_vat_pair();
        let virt_mem = VirtualDma::with_vat(
            phys_mem.forward_mut(),
            proc_info.base_info.proc_arch,
            proc_info.translator(),
            vat.forward_mut(),
        );

        Self {
            virt_mem,
            proc_info,
            sysproc_dtb,
            mmvad: kernel.offsets.mm_vad(),
            offset_eproc_exit_status: kernel.offsets.eproc_exit_status(),
        }
    }
}

impl<T: PhysicalMemory, V: VirtualTranslate2, D: VirtualTranslate3> Win32Process<T, V, D> {
    fn module_address_list_with_infos_callback(
        &mut self,
        module_infos: impl Iterator<Item = (Win32ModuleListInfo, ArchitectureIdent)>,
        out: &mut ModuleAddressCallback,
    ) -> Result<()> {
        for (info, arch) in module_infos {
            let callback = &mut |address| out.call(ModuleAddressInfo { address, arch });
            info.module_entry_list_callback(self, arch, callback.into())?;
        }
        Ok(())
    }
}

impl<T, V, D> fmt::Debug for Win32Process<T, V, D> {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self.proc_info)
    }
}
