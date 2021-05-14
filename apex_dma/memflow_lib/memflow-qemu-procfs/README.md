# memflow-qemu-procfs

This connector implements an interface for Qemu via the Process Filesystem on Linux.

## Compilation

### Installing the library

The `./install.sh` script will just compile and install the plugin.
The connector will be installed to `~/.local/lib/memflow` by default.
Additionally the `--system` flag can be specified which will install the connector in `/usr/lib/memflow` as well.

### Building the stand-alone connector for dynamic loading

The stand-alone connector of this library is feature-gated behind the `inventory` feature.
To compile a dynamic library for use with the connector inventory use the following command:

```
cargo build --release --all-features
```

### Using the crate in a rust project

To use the connector in a rust project just include it in your Cargo.toml

```
memflow-qemu-procfs = "0.1"
```

Make sure to not enable the `inventory` feature when importing multiple
connectors in a rust project without using the memflow connector inventory.
This might cause duplicated exports being generated in your project.

## Arguments

- `name` - the name of the virtual machine (default argument, optional)

## Permissions

The `qemu_procfs` connector requires access to the qemu process via the linux procfs. This means any process which loads this connector requires to have at least ptrace permissions set.

To set ptrace permissions on a binary simply use:
```bash
sudo setcap 'CAP_SYS_PTRACE=ep' [filename]
```

Alternatively you can just run the binary via `sudo`.

## License

Licensed under MIT License, see [LICENSE](LICENSE).

### Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted for inclusion in the work by you, shall be licensed as above, without any additional terms or conditions.
