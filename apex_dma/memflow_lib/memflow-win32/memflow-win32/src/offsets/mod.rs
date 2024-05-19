pub use memflow_win32_defs::offsets::*;

use crate::prelude::v1::*;

#[repr(align(16))]
struct Align16<T>(pub T);

#[cfg(feature = "embed_offsets")]
const WIN32_OFFSETS: Align16<
    [u8; include_bytes!(concat!(env!("OUT_DIR"), "/win32_offsets.bin")).len()],
> = Align16(*include_bytes!(concat!(
    env!("OUT_DIR"),
    "/win32_offsets.bin"
)));

pub fn offset_builder<'a>() -> Win32OffsetBuilder<'a> {
    let builder = Win32Offsets::builder();

    #[cfg(feature = "embed_offsets")]
    {
        // # Safety
        // Struct padding and alignment is compile-time guaranteed by the struct (see mod offset_table).
        let offsets = unsafe {
            core::slice::from_raw_parts(
                WIN32_OFFSETS.0.as_ptr() as *const Win32OffsetFile,
                WIN32_OFFSETS.0.len() / std::mem::size_of::<Win32OffsetFile>(),
            )
        };
        builder.offset_list(offsets)
    }
    #[cfg(not(feature = "embed_offsets"))]
    builder
}

pub fn offset_builder_with_kernel_info<'a>(
    kernel_info: &Win32KernelInfo,
) -> Win32OffsetBuilder<'a> {
    let builder = offset_builder();
    kernel_info.into_offset_builder(builder)
}
