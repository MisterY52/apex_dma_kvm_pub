#!/bin/bash

cd memflow_lib/
if cargo build --release --workspace ; then
    cd ../
    make
else
    echo "Error while building memflow libraries"
fi
