# memflow's KVM connector

This is a connector for Linux kernel based virtual machines (KVMs), which by utilizing a kernel module directly maps all VM memory. This provides an effortless way to run memflow on all KVM based VMs (not just QEMU), and with highest performance.

## Setting up

This connector requires the [memflow module](https://github.com/memflow/memflow-kvm) to be present (to access `/dev/memflow` interface), and have appropriate permissions to access the interface.

For development purposes, it is possible to `chmod o+rw /dev/memflow` to gain access, but it is a security risk.

`create_connector` accepts a single, optional, argument - PID. This PID will be passed to the `memflow` module to select which VM monitor to target, or can be omitted to pick the first found one.
