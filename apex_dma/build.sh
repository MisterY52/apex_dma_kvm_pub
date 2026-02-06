#!/bin/bash

cd memflow_lib/
if cargo build --release --workspace ; then
    cd ../
    make -C r5dumper
    mkdir -p build
    mkdir -p build/r5dumper
    cp r5dumper/r5dumper build/r5dumper/
    cp memflow_lib/target/release/libmemflow_kvm.so build/ 2>/dev/null || true
    cp memflow_lib/target/release/libmemflow_qemu.so build/ 2>/dev/null || true
    cp memflow_lib/target/release/libmemflow_win32.so build/ 2>/dev/null || true
    make
else
    echo "Error while building memflow libraries"
fi
