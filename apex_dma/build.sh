#!/bin/bash

cd memflow_lib/memflow-ffi/
if cargo build --release ; then
    cd ../../
    make
else
    echo "Error while building memflow-ffi"
fi
