# memflow-qemu

The qemu connector implements a memflow plugin interface for Qemu on top of the Process Filesystem on Linux.

## Compilation

### Installing the library

The recommended way to install memflow connectors is using [memflowup](https://github.com/memflow/memflowup#memflow-setup-tool).

### Development builds

To compile the connector as dynamic library to be used with the memflow plugin system use the following command:

```
cargo build --release --all-features
```

The plugin can then be found in the `target/release/` directory and has to be copied to one of [memflows default search paths](https://github.com/memflow/memflow/blob/main/memflow/src/plugins/mod.rs#L379).

### Linking the crate statically in a rust project

To use the connector in a rust project just include it in your Cargo.toml

```
memflow-qemu = "^0.2.0-beta"
```

## Arguments

The `target` argument specifies the name of the qemu virtual machine (specified with -name when starting qemu).

The following additional arguments can be used when loading the connector:

- `map_base` - overrides the default VM memory base (optional)
- `map_size` - overrides the default VM memory size (optional)

## Permissions

The `qemu` connector requires access to the qemu process via the linux procfs. This means any process which loads this connector requires to have at least ptrace permissions set.

To set ptrace permissions on a binary simply use:
```bash
sudo setcap 'CAP_SYS_PTRACE=ep' [filename]
```

Alternatively you can just run the binary via `sudo`.

## Memory Mappings

The connector supports dynamic acquisition of the qemu memory mappings by utilizing the [qemu qmp protocol](https://qemu.readthedocs.io/en/latest/interop/qemu-qmp-ref.html).

To enable qmp on a virtual machine simply add this to the qemu command line:
```
-qmp unix:/tmp/qmp-my-vm.sock,server,nowait
```

Alternatively a tcp server can be exposed:
```
-qmp tcp:localhost:12345,server,nowait
```

Or via libvirt:
```xml
<domain xmlns:qemu="http://libvirt.org/schemas/domain/qemu/1.0" type="kvm">

...

  </devices>
  <qemu:commandline>
    <qemu:arg value="-qmp"/>
    <qemu:arg value="unix:/tmp/qmp-my-vm.sock,server,nowait"/>
  </qemu:commandline>
</domain>
```

Please refer to the qemu [qmp manual](https://wiki.qemu.org/Documentation/QMP) for more information about how to configure this feature.

In case qmp is not active or could not be fetched, the connector falls back to hard-coded mapping tables for specific qemu machine types.

## Running Examples

Analog to the examples found in the main memflow repository examples can be run via:

```bash
RUST_SETPTRACE=1 cargo run --example read_phys --release
RUST_SETPTRACE=1 cargo run --example ps_win32 --release
RUST_SETPTRACE=1 cargo run --example ps_inventory --release
```

For more information about `RUST_SETPTRACE` and how to run examples see the [running-examples](https://github.com/memflow/memflow#running-examples) section in the main memflow repository.

## License

Licensed under MIT License, see [LICENSE](LICENSE).

### Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted for inclusion in the work by you, shall be licensed as above, without any additional terms or conditions.
