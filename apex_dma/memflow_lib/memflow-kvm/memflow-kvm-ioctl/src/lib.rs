#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::fs::File;
use std::os::unix::io::{AsRawFd, FromRawFd, RawFd};

// Do not depend on entire libc just for this function
extern "C" {
    fn ioctl(fd: i32, request: __u64, ...) -> i32;
    fn munmap(addr: *mut std::os::raw::c_void, len: usize) -> i32;
}

use std::io::Result;

/// Structure assisting automatic memory map unmapping
///
/// It does not reference count on its own. Wrap it in Arc,
/// or Rc for it to take place
pub struct AutoMunmap {
    memslots: Vec<vm_memslot>,
}

impl AutoMunmap {
    /// Create automatic unmapping
    ///
    /// # Safety
    ///
    /// Drop implementation of this structure calls munmap on all mapped memory regions.
    /// vm_memslots have to be correct for the runnings process, or causes undefined bahaviour.
    pub unsafe fn new(memslots: Vec<vm_memslot>) -> Self {
        Self { memslots }
    }
}

impl Drop for AutoMunmap {
    fn drop(&mut self) {
        for slot in &self.memslots {
            unsafe {
                munmap(slot.host_base as _, slot.map_size as _);
            }
        }
    }
}

/// Handle to memflow's kernel module
///
/// This is a handle to `/dev/memflow`. It functions as a file, closes automatically when dropped
pub struct ModuleHandle {
    memflow: File,
}

impl ModuleHandle {
    pub fn try_open() -> Result<Self> {
        Ok(Self {
            memflow: File::open("/dev/memflow")?,
        })
    }
}

impl AsRawFd for ModuleHandle {
    fn as_raw_fd(&self) -> RawFd {
        self.memflow.as_raw_fd()
    }
}

/// Handle to a KVM VM
///
/// This is a handle to a single KVM instance. Allows access to basic memory layout information of the VM.
pub struct VMHandle {
    vm: File,
}

impl VMHandle {
    /// Open a KVM instance with a memflow handle
    ///
    /// This may be useful when multiple VMs are accessed to save syscalls, but in regular scenarios,
    /// use `try_open`.
    ///
    /// # Arguments
    ///
    /// * `memflow` - handle to the memflow driver
    /// * `pid` - optional process identifier. If it is `None`, or `Some(0)`, handle to any of the KVM
    /// VMs gets retrieved.
    pub fn try_open_handle(memflow: &ModuleHandle, pid: Option<i32>) -> Result<Self> {
        let vm_fd: RawFd = unsafe {
            ioctl(
                memflow.as_raw_fd(),
                IO_MEMFLOW_OPEN_VM as u64,
                pid.unwrap_or_default(),
            )
        };

        if vm_fd < 0 {
            Err(std::io::Error::last_os_error())
        } else {
            Ok(Self {
                vm: unsafe { File::from_raw_fd(vm_fd) },
            })
        }
    }

    /// Open a KVM instance
    ///
    /// # Arguments
    ///
    /// * `pid` - optional process identifier. If it is `None`, or `Some(0)`, handle to any of the KVM
    /// VMs gets retrieved. Otherwise, the specific PID gets targeted. And if the process does not exist,
    /// this function will fail.
    pub fn try_open(pid: Option<i32>) -> Result<Self> {
        Self::try_open_handle(&ModuleHandle::try_open()?, pid)
    }

    /// Retrieve info about the KVM instance
    ///
    /// Pulls info about the current VM handle. That means its PID, and the memory layout (in KVMs userspace).
    pub fn info(&self, slot_count: usize) -> Result<(i32, Vec<vm_memslot>)> {
        let mut vm_info = vm_info::default();
        let mut memslots = vec![Default::default(); slot_count];

        vm_info.slot_count = slot_count as u32;
        vm_info.slots = memslots.as_mut_ptr();

        let ret = unsafe { ioctl(self.vm.as_raw_fd(), IO_MEMFLOW_VM_INFO as u64, &mut vm_info) };

        if ret < 0 {
            Err(std::io::Error::last_os_error())
        } else {
            memslots.truncate(vm_info.slot_count as usize);
            #[allow(clippy::unnecessary_cast)]
            Ok((vm_info.userspace_pid as i32, memslots))
        }
    }

    /// Memory map the KVM instance
    ///
    /// Maps the memory of the KVM instance into local address space, and returns the mapped memory layout.
    ///
    /// The memory map is permanent (unless manually unmapped using libc). KVM has the possibility of changing
    /// the memory layout (in its process only), but this function can not account for it.
    pub fn map_vm(&self, slot_count: usize) -> Result<Vec<vm_memslot>> {
        let mut vm_info = vm_map_info::default();
        let mut memslots = vec![Default::default(); slot_count];

        vm_info.slot_count = slot_count as u32;
        vm_info.slots = memslots.as_mut_ptr();

        let ret = unsafe { ioctl(self.vm.as_raw_fd(), IO_MEMFLOW_MAP_VM as u64, &mut vm_info) };

        if ret < 0 {
            Err(std::io::Error::last_os_error())
        } else {
            memslots.truncate(vm_info.slot_count as usize);
            Ok(memslots)
        }
    }
}
