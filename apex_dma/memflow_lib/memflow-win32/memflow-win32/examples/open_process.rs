/*!
This example shows how to use a dynamically loaded connector in conjunction
with memflow-win32. This example uses the `Inventory` feature of memflow
but hard-wires the connector instance into the memflow-win32 OS layer.

The example showcases how to retrieve extended process info data,
opening the process and getting a list of all modules.

# Usage:
```bash
cargo run --release --example open_process -- -vv -c kvm -p "explorer.exe"
```
*/
use clap::*;
use log::{info, Level};

use memflow::prelude::v1::*;
use memflow_win32::prelude::v1::*;

pub fn main() -> Result<()> {
    let matches = parse_args();
    let (chain, process_name) = extract_args(&matches)?;
    let process_name = process_name.unwrap_or("explorer.exe");

    // create inventory + connector
    let inventory = Inventory::scan();
    let connector = inventory.builder().connector_chain(chain).build()?;

    let mut os = Win32Kernel::builder(connector)
        .build_default_caches()
        .build()
        .expect("unable to initialize memflow-win32");

    // display the extended process info for the process
    let process_info = os.process_info_by_name(process_name)?;
    let process_info_ext = os.process_info_from_base_info(process_info.clone())?;
    info!("{:?}", process_info_ext);

    // create a new process instance
    let mut process = os
        .into_process_by_info(process_info)
        .expect("unable to open process");

    // retrieve all modules
    let module_list = process.module_list().expect("unable to read module list");

    info!("{:>5} {:>10} {:^32} {:<}", "ADDR", "BASE", "NAME", "PATH");

    for m in module_list {
        info!("{:>5} {:^16} {:^32} {}", m.address, m.base, m.name, m.path);
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
        .arg(Arg::new("process").short('p').action(ArgAction::Set))
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
        matches.get_one::<String>("process").map(String::as_str),
    ))
}
