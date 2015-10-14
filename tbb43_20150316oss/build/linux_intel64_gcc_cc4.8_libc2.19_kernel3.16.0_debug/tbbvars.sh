#!/bin/bash
export TBBROOT="/home/lou/Documents/graphics-cis560/tbb43_20150316oss" #
tbb_bin="/home/lou/Documents/graphics-cis560/tbb43_20150316oss/build/linux_intel64_gcc_cc4.8_libc2.19_kernel3.16.0_debug" #
if [ -z "$CPATH" ]; then #
    export CPATH="${TBBROOT}/include" #
else #
    export CPATH="${TBBROOT}/include:$CPATH" #
fi #
if [ -z "$LIBRARY_PATH" ]; then #
    export LIBRARY_PATH="${tbb_bin}" #
else #
    export LIBRARY_PATH="${tbb_bin}:$LIBRARY_PATH" #
fi #
if [ -z "$LD_LIBRARY_PATH" ]; then #
    export LD_LIBRARY_PATH="${tbb_bin}" #
else #
    export LD_LIBRARY_PATH="${tbb_bin}:$LD_LIBRARY_PATH" #
fi #
 #
