/*!
This example shows how to use the qemu connector in conjunction
with a specific OS layer. This example does not use the `Inventory` feature of memflow
but hard-wires the connector instance with the OS layer directly.

The example is an adaption of the memflow core process list example:
https://github.com/memflow/memflow/blob/next/memflow/examples/process_list.rs

# Remarks:
The most flexible and recommended way to use memflow is to go through the inventory.
The inventory allows the user to swap out connectors and os layers at runtime.
For more information about the Inventory see the ps_inventory.rs example in this repository
or check out the documentation at:
https://docs.rs/memflow/0.1.5/memflow/connector/inventory/index.html
*/
use std::env::args;

use log::info;

use memflow::prelude::v1::*;
use memflow_win32::prelude::v1::*;

fn main() {
    env_logger::init_from_env(env_logger::Env::new().default_filter_or("info"));

    let connector_args = if let Some(arg) = args().nth(1) {
        str::parse(arg.as_ref()).expect("unable to parse command line arguments")
    } else {
        Default::default()
    };

    let connector = memflow_qemu::create_connector(&connector_args)
        .expect("unable to initialize qemu connector");

    let mut os = Win32Kernel::builder(connector)
        .build_default_caches()
        .build()
        .expect("unable to create win32 instance with qemu connector");

    let process_list = os.process_info_list().expect("unable to read process list");

    info!(
        "{:>5} {:>10} {:>10} {:<}",
        "PID", "SYS ARCH", "PROC ARCH", "NAME"
    );

    for p in process_list {
        info!(
            "{:>5} {:^10} {:^10} {}",
            p.pid, p.sys_arch, p.proc_arch, p.name
        );
    }
}
