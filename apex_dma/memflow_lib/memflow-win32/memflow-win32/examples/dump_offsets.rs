/*!
This example shows how to use a dynamically loaded connector in conjunction
with memflow-win32. This example uses the `Inventory` feature of memflow
but hard-wires the connector instance into the memflow-win32 OS layer.

The example then dumps all the found offsets into the specified `output` file.

# Usage:
```bash
cargo run --release --example dump_offsets -- -vv -c kvm --output file.toml
```
*/
use std::fs::File;
use std::io::Write;

use clap::*;
use log::{error, Level};

use memflow::prelude::v1::{Result, *};
use memflow_win32::prelude::v1::*;

pub fn main() -> Result<()> {
    let matches = parse_args();
    let (chain, output) = extract_args(&matches)?;

    // create inventory + connector
    let inventory = Inventory::scan();
    let connector = inventory.builder().connector_chain(chain).build()?;

    let os = Win32Kernel::builder(connector)
        .build_default_caches()
        .build()
        .unwrap();

    let winver = os.kernel_info.kernel_winver;

    if winver != (0, 0).into() {
        let guid = os.kernel_info.kernel_guid.unwrap_or_default();
        let offsets = Win32OffsetFile {
            header: Win32OffsetHeader {
                pdb_file_name: guid.file_name.as_str().into(),
                pdb_guid: guid.guid.as_str().into(),

                arch: os.kernel_info.os_info.arch.into(),

                nt_major_version: winver.major_version(),
                nt_minor_version: winver.minor_version(),
                nt_build_number: winver.build_number(),
            },
            offsets: os.offsets.into(),
        };

        // write offsets to file
        let offsetstr = toml::to_string_pretty(&offsets).unwrap();
        match output {
            Some(output) => {
                let mut file = File::create(output).unwrap();
                file.write_all(offsetstr.as_bytes()).unwrap();
            }
            None => println!("{offsetstr}"),
        }
    } else {
        error!("kernel version has to be valid in order to generate a offsets file");
    }

    Ok(())
}

fn parse_args() -> ArgMatches {
    Command::new("dump_offsets example")
        .version(crate_version!())
        .author(crate_authors!())
        .arg(Arg::new("verbose").short('v').action(ArgAction::Count))
        .arg(
            Arg::new("connector")
                .short('c')
                .action(ArgAction::Append)
                .required(true),
        )
        .arg(Arg::new("os").short('o').action(ArgAction::Append))
        .arg(Arg::new("output").long("output").action(ArgAction::Set))
        .get_matches()
}

fn extract_args(matches: &ArgMatches) -> Result<(ConnectorChain<'_>, Option<&str>)> {
    let log_level = match matches.get_count("verbose") {
        0 => Level::Error,
        1 => Level::Warn,
        2 => Level::Info,
        3 => Level::Debug,
        4 => Level::Trace,
        _ => Level::Trace,
    };
    simplelog::TermLogger::init(
        log_level.to_level_filter(),
        simplelog::Config::default(),
        simplelog::TerminalMode::Stdout,
        simplelog::ColorChoice::Auto,
    )
    .unwrap();

    let conn_iter = matches
        .indices_of("connector")
        .zip(matches.get_many::<String>("connector"))
        .map(|(a, b)| a.zip(b.map(String::as_str)))
        .into_iter()
        .flatten();

    let os_iter = matches
        .indices_of("os")
        .zip(matches.get_many::<String>("os"))
        .map(|(a, b)| a.zip(b.map(String::as_str)))
        .into_iter()
        .flatten();

    Ok((
        ConnectorChain::new(conn_iter, os_iter)?,
        matches.get_one::<String>("output").map(String::as_str),
    ))
}
