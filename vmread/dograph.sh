#!/bin/bash

BUILD_DIR=$1

if [[ -z $(lsmod | grep vmread ) ]]; then
        echo loading vmread module
        if [[ ! -f $BUILD_DIR/vmread.ko ]]; then
                echo build vmread first!
                exit
        fi
        sudo insmod $BUILD_DIR/vmread.ko
fi

echo "Running external mode..."
sudo ./build/example | tail -n 5 > read_external
echo "Running kernel mapped mode..."
sudo ./build/kmod_example | tail -n 5 > read_kmod
gnuplot perf_plot.p
