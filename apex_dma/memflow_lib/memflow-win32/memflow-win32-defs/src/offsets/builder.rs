use std::convert::TryFrom;

#[cfg(feature = "symstore")]
use super::symstore::SymbolStore;

use super::offset_table::Win32OffsetFile;
use super::{Win32Offsets, Win32OffsetsArchitecture};

use crate::kernel::{Win32Guid, Win32Version};

use memflow::error::{Error, ErrorKind, ErrorOrigin, Result};

pub struct Win32OffsetBuilder<'a> {
    #[cfg(feature = "symstore")]
    symbol_store: Option<SymbolStore>,

    guid: Option<Win32Guid>,
    winver: Option<Win32Version>,
    arch: Option<Win32OffsetsArchitecture>,

    offset_list: Option<&'a [Win32OffsetFile]>,
}

impl<'a> Default for Win32OffsetBuilder<'a> {
    fn default() -> Self {
        Self {
            #[cfg(feature = "symstore")]
            symbol_store: Some(SymbolStore::default()),

            guid: None,
            winver: None,
            arch: None,

            offset_list: None,
        }
    }
}

impl<'a> Win32OffsetBuilder<'a> {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn build(self) -> Result<Win32Offsets> {
        if self.guid.is_none() && self.winver.is_none() {
            return Err(Error(ErrorOrigin::OsLayer, ErrorKind::Configuration)
                .log_error("building win32 offsets requires either a guid or winver"));
        }

        // try to build via symbol store
        if let Ok(offs) = self.build_with_symbol_store() {
            return Ok(offs);
        }

        // use static offset list
        if let Ok(offs) = self.build_with_offset_list() {
            return Ok(offs);
        }

        Err(Error(ErrorOrigin::OsLayer, ErrorKind::Configuration)
            .log_error("no valid offset configuration found while building win32"))
    }

    fn build_with_offset_list(&self) -> Result<Win32Offsets> {
        let offsets = self.offset_list.ok_or_else(|| {
            Error(ErrorOrigin::OsLayer, ErrorKind::Configuration)
                .log_error("no offset list supplied")
        })?;

        // Try matching exact guid
        if let Some(target_guid) = &self.guid {
            for offset in offsets.iter() {
                if let (Ok(file), Ok(guid)) = (
                    <&str>::try_from(&offset.header.pdb_file_name),
                    <&str>::try_from(&offset.header.pdb_guid),
                ) {
                    if target_guid.file_name == file && target_guid.guid == guid {
                        return Ok(Win32Offsets(offset.offsets));
                    }
                }
            }
        }

        let mut closest_match = None;
        let mut prev_build_number = 0;

        // Try matching the newest build from that version that is not actually newer
        if let (Some(winver), Some(arch)) = (&self.winver, self.arch) {
            for offset in offsets.iter() {
                if winver.major_version() == offset.header.nt_major_version
                    && winver.minor_version() == offset.header.nt_minor_version
                    && winver.build_number() >= offset.header.nt_build_number
                    && prev_build_number <= offset.header.nt_build_number
                    && arch == offset.header.arch
                {
                    prev_build_number = offset.header.nt_build_number;
                    closest_match = Some(Win32Offsets(offset.offsets));
                }
            }

            if prev_build_number != winver.build_number() {
                log::warn!(
                    "no exact build number ({}) found! Closest match: {}",
                    winver.build_number(),
                    prev_build_number
                );
            }
        }

        closest_match.ok_or_else(|| {
            Error(ErrorOrigin::OsLayer, ErrorKind::Configuration)
                .log_error("no valid offset configuration found while building win32")
        })
    }

    #[cfg(feature = "symstore")]
    fn build_with_symbol_store(&self) -> Result<Win32Offsets> {
        if let Some(store) = &self.symbol_store {
            if let Some(guid) = &self.guid {
                let pdb = store.load(guid)?;
                Win32Offsets::from_pdb_slice(&pdb[..])
            } else {
                Err(Error(ErrorOrigin::OsLayer, ErrorKind::Configuration)
                    .log_error("symbol store can only be used with a guid"))
            }
        } else {
            Err(Error(ErrorOrigin::OsLayer, ErrorKind::Configuration)
                .log_error("symbol store is disabled"))
        }
    }

    #[cfg(not(feature = "symstore"))]
    fn build_with_symbol_store(&self) -> Result<Win32Offsets> {
        Err(
            Error(ErrorOrigin::OsLayer, ErrorKind::UnsupportedOptionalFeature)
                .log_error("symbol store is deactivated via a compilation feature"),
        )
    }

    #[cfg(feature = "symstore")]
    pub fn symbol_store(mut self, symbol_store: SymbolStore) -> Self {
        self.symbol_store = Some(symbol_store);
        self
    }

    #[cfg(feature = "symstore")]
    pub fn no_symbol_store(mut self) -> Self {
        self.symbol_store = None;
        self
    }

    pub fn offset_list(mut self, offset_list: &'a [Win32OffsetFile]) -> Self {
        self.offset_list = Some(offset_list);
        self
    }

    pub fn guid(mut self, guid: Win32Guid) -> Self {
        self.guid = Some(guid);
        self
    }

    pub fn get_guid(&self) -> &Option<Win32Guid> {
        &self.guid
    }

    pub fn winver(mut self, winver: Win32Version) -> Self {
        self.winver = Some(winver);
        self
    }

    pub fn get_winver(&self) -> &Option<Win32Version> {
        &self.winver
    }

    pub fn arch(mut self, arch: Win32OffsetsArchitecture) -> Self {
        self.arch = Some(arch);
        self
    }

    pub fn get_arch(&self) -> &Option<Win32OffsetsArchitecture> {
        &self.arch
    }
}
