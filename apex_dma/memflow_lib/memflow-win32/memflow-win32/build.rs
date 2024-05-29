use std::{
    env,
    error::Error,
    fs::{self, File},
    io::{Read, Write},
    path::Path,
};

use memflow::dataview::PodMethods;
use std::convert::TryInto;

#[cfg(feature = "embed_offsets")]
use memflow_win32_defs::{
    kernel::*,
    offsets::{
        offset_table::{Win32OffsetFile, Win32OffsetHeader},
        *,
    },
};

#[repr(C, align(4))]
#[derive(Debug, ::serde::Serialize, ::serde::Deserialize)]
#[cfg(feature = "embed_offsets")]
pub struct Win32OffsetFileStart {
    header: Win32OffsetHeader,
}

#[cfg(feature = "embed_offsets")]
fn embed_offsets() -> Result<(), Box<dyn Error>> {
    let regenerate_offsets =
        match &*env::var("MEMFLOW_WIN32_REGENERATE_OFFSETS").unwrap_or_default() {
            "on" | "1" => 1,
            "force" => 2,
            _ => 0,
        };

    let out_dir = env::var("OUT_DIR")?;
    let dest_path = Path::new(&out_dir).join("win32_offsets.bin");
    let mut all_the_files = File::create(dest_path)?;

    // iterate offsets folder
    for f in fs::read_dir("./offsets")? {
        let f = f?;

        if !f.file_type()?.is_file() {
            continue;
        }

        let fp = f.path();

        let mut file = File::open(&fp)?;
        let mut tomlstr = String::new();
        file.read_to_string(&mut tomlstr)?;

        std::mem::drop(file);

        let offsets = match toml::from_str::<Win32OffsetFile>(&tomlstr) {
            x if regenerate_offsets == 2 || (regenerate_offsets == 1 && x.is_err()) => {
                let regenerate = || -> Result<Win32OffsetFile, Box<dyn Error>> {
                    let mut header = match toml::from_str::<Win32OffsetFileStart>(&tomlstr) {
                        Ok(header) => header,
                        _ => {
                            // Parse header from the filename. Example format:
                            // 10_0_18362_X64_0AFB69F5FD264D54673570E37B38A3181.toml
                            let fname = fp.file_name().and_then(|s| s.to_str()).unwrap_or_default();

                            let parts = fname.split_terminator('_').collect::<Vec<_>>();

                            if parts.len() != 5 {
                                return Err(format!(
                                        "Cannot regenerate offsets for {fname} - invalid filename format"
                                )
                                    .into());
                            }

                            let nt_major_version = str::parse(parts[0])?;
                            let nt_minor_version = str::parse(parts[1])?;
                            let nt_build_number = str::parse(parts[2])?;
                            let pdb_guid = parts[4];

                            if !pdb_guid.ends_with(".toml") {
                                return Err(format!("{fname} does not contain valid guid ({pdb_guid})").into());
                            }

                            Win32OffsetFileStart {
                                header: Win32OffsetHeader {
                                    pdb_file_name: "".into(),
                                    pdb_guid: pdb_guid.trim_end_matches(".toml").into(),
                                    arch: toml::from_str(&format!("\"{}\"", parts[3]))?,
                                    nt_major_version,
                                    nt_minor_version,
                                    nt_build_number,
                                },
                            }
                        }
                    }.header;

                    println!("{header:?}");

                    // Use pdb file name if available, backup if only file name is known.

                    let file_names1: [&str; 1] = [(&header.pdb_file_name).try_into()?];

                    let file_names2 = ["ntkrnlmp.pdb", "ntkrpamp.pdb"];

                    let file_names = if file_names1[0].is_empty() {
                        &file_names2[..]
                    } else {
                        &file_names1[..]
                    };

                    let guid = (&header.pdb_guid).try_into()?;

                    let (offsets, pdb_file_name) = file_names
                        .iter()
                        .map(|f| {
                            Win32Offsets::builder()
                                .symbol_store(SymbolStore::new())
                                .guid(Win32Guid::new(f, guid))
                                .build()
                                .map(|v| (v.0, f))
                        })
                        .find_map(Result::ok)
                        .ok_or("Failed to get symbols from the symbol store")?;

                    header.pdb_file_name = (*pdb_file_name).into();

                    println!("{header:?}");
                    println!("{offsets:?}");

                    Ok(Win32OffsetFile { header, offsets })
                };

                match regenerate() {
                    Ok(x) => {
                        let mut file = File::create(&fp)?;
                        file.write_all(toml::to_string(&x)?.as_bytes())?;

                        x
                    }
                    Err(e) => {
                        // Do not report failure if forced, but already contains correct configuration
                        if x.is_ok() {
                            x?
                        } else {
                            return Err(e);
                        }
                    }
                }
            }
            x => x?,
        };

        all_the_files.write_all(offsets.as_bytes())?;
    }

    Ok(())
}

#[cfg(not(feature = "embed_offsets"))]
fn embed_offsets() -> Result<(), Box<dyn Error>> {
    Ok(())
}

fn main() -> Result<(), Box<dyn Error>> {
    embed_offsets()?;
    Ok(())
}
