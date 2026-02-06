use log::{debug, info};

use memflow::connector::MappedPhysicalMemory;
use memflow::derive::connector;
use memflow::error::*;
use memflow::mem::MemoryMap;
use memflow::plugins::ConnectorArgs;
use memflow::types::{umem, Address};
use memflow_kvm_ioctl::{AutoMunmap, VMHandle};
use std::sync::Arc;

pub type KVMConnector<'a> = MappedPhysicalMemory<&'a mut [u8], KVMMapData<&'a mut [u8]>>;

pub struct KVMMapData<T> {
    handle: Arc<AutoMunmap>,
    mappings: MemoryMap<T>,
    addr_mappings: MemoryMap<(Address, umem)>,
}

impl<'a> Clone for KVMMapData<&'a mut [u8]> {
    fn clone(&self) -> Self {
        unsafe { Self::from_addrmap_mut(self.handle.clone(), self.addr_mappings.clone()) }
    }
}

impl<T> AsRef<MemoryMap<T>> for KVMMapData<T> {
    fn as_ref(&self) -> &MemoryMap<T> {
        &self.mappings
    }
}

impl<'a> KVMMapData<&'a mut [u8]> {
    unsafe fn from_addrmap_mut(handle: Arc<AutoMunmap>, map: MemoryMap<(Address, umem)>) -> Self {
        Self {
            handle,
            mappings: map.clone().into_bufmap_mut(),
            addr_mappings: map,
        }
    }
}

/// Creates a new KVM Connector instance.
#[connector(name = "kvm")]
pub fn create_connector<'a>(
    args: &ConnectorArgs,
) -> Result<MappedPhysicalMemory<&'a mut [u8], KVMMapData<&'a mut [u8]>>> {
    const ERROR_UNABLE_TO_READ_MEMORY: &str = "Could not access the memflow device at /dev/memflow. Please make sure that you installed the dkms module properly and that it is loaded via `modprobe memflow`. Also ensure that you have read and write access to /dev/memflow. For further information check the readme at https://github.com/memflow/memflow-kvm";

    let pid = match &args.target {
        Some(pidstr) => Some(
            pidstr
                .parse::<i32>()
                .map_err(|_| Error(ErrorOrigin::Connector, ErrorKind::ArgValidation))?,
        ),
        None => None,
    };

    let vm = VMHandle::try_open(pid).map_err(|_| {
        Error(ErrorOrigin::Connector, ErrorKind::UnableToReadMemory)
            .log_error(ERROR_UNABLE_TO_READ_MEMORY)
    })?;
    let (pid, memslots) = vm.info(64).map_err(|_| {
        Error(ErrorOrigin::Connector, ErrorKind::UnableToReadMemory)
            .log_error(ERROR_UNABLE_TO_READ_MEMORY)
    })?;
    debug!("pid={} memslots.len()={}", pid, memslots.len());
    for slot in memslots.iter() {
        debug!(
            "{:x}-{:x} -> {:x}-{:x}",
            slot.base,
            slot.base + slot.map_size,
            slot.host_base,
            slot.host_base + slot.map_size
        );
    }
    let mapped_memslots = vm.map_vm(64).map_err(|e| {
        Error(ErrorOrigin::Connector, ErrorKind::UnableToMapFile).log_error(format!(
            "The mapped memory slots for the vm could not be read: {}",
            e
        ))
    })?;

    let mut mem_map = MemoryMap::new();

    info!("mmapped {} slots", mapped_memslots.len());
    for slot in mapped_memslots.iter() {
        debug!(
            "{:x}-{:x} -> {:x}-{:x}",
            slot.base,
            slot.base + slot.map_size,
            slot.host_base,
            slot.host_base + slot.map_size
        );
        mem_map.push_remap(
            slot.base.into(),
            slot.map_size as umem,
            slot.host_base.into(),
        );
    }

    let munmap = Arc::new(unsafe { AutoMunmap::new(mapped_memslots) });

    let map_data = unsafe { KVMMapData::from_addrmap_mut(munmap, mem_map) };

    let mem = MappedPhysicalMemory::with_info(map_data);

    Ok(mem)
}
