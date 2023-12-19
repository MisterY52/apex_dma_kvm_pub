#!/bin/bash

cd memflow_lib/memflow-win32-ffi/
if cargo build --release ; then
    cd ../memflow-qemu-procfs

    if cargo build --release --all-features ; then
        cd ../../
        make
    else
        echo "Error while building memflow-qemu-procfs"
    fi

else
    echo "Error while building memflow-win32-ffi"
fi
