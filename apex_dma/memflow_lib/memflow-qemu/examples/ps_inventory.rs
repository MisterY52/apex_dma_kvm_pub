/*!
This example shows how to use the qemu connector in conjunction
with a specific OS layer. This example uses the `Inventory` feature of memflow
to create the connector itself and the os instance.

The example is an adaption of the memflow core process list example:
https://github.com/memflow/memflow/blob/next/memflow/examples/process_list.rs

# Remarks:
To run this example you must have the `qemu` connector and `win32` plugin installed on your system.
Make sure they can be found in one of the following locations:

~/.local/lib/memflow/
/usr/lib/memflow/

or in any other path found in the official memflow documentation.
*/
use std::env::args;

use log::info;

use memflow::prelude::v1::*;

fn main() {
    env_logger::init_from_env(env_logger::Env::new().default_filter_or("info"));

    let connector_args = args()
        .nth(1)
        .map(|arg| str::parse(arg.as_ref()).expect("unable to parse command line arguments"));

    let inventory = Inventory::scan();
    let connector = inventory
        .create_connector("qemu", None, connector_args.as_ref())
        .expect("unable to initialize qemu connector");
    let mut os = inventory
        .create_os("win32", Some(connector), None)
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
