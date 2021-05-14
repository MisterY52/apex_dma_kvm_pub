#!/bin/bash

cargo build --release --all-features

# install connector to system dir
if [ ! -z "$1" ] && [ $1 = "--system" ]; then
    echo "installing connector system-wide in /usr/lib/memflow"
    if [[ ! -d /usr/lib/memflow ]]; then
        sudo mkdir /usr/lib/memflow
    fi
    sudo cp target/release/libmemflow_qemu_procfs.so /usr/lib/memflow
fi

# install connector in user dir
echo "installing connector for user in ~/.local/lib/memflow"
if [[ ! -d ~/.local/lib/memflow ]]; then
    mkdir -p ~/.local/lib/memflow
fi
cp target/release/libmemflow_qemu_procfs.so ~/.local/lib/memflow
