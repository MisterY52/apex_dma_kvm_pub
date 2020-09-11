#!/bin/sh
path="apex_dma/build"
if [ -d "$path" ]; then
	cd "$path" && ninja;
else
	cd vmread && meson "../$path" && cd "../$path" && ninja;
fi