#!/bin/bash

cargo build --release --all-features

# install connector to system dir
if [ ! -z "$1" ] && [ $1 = "--system" ]; then
    echo "installing connector system-wide in /usr/local/lib/memflow"
    if [[ ! -d /usr/local/lib/memflow ]]; then
        sudo mkdir /usr/local/lib/memflow
    fi
    sudo cp target/release/libmemflow_win32.so /usr/local/lib/memflow/libmemflow_win32.7.so
fi

# install connector in user dir
echo "installing connector for user in ~/.local/lib/memflow"
if [[ ! -d ~/.local/lib/memflow ]]; then
    mkdir -p ~/.local/lib/memflow
fi
cp target/release/libmemflow_win32.so ~/.local/lib/memflow/libmemflow_win32.7.so
