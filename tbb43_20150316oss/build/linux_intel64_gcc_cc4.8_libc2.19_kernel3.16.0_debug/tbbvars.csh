#!/bin/csh
setenv TBBROOT "/home/lou/Documents/graphics-cis560/tbb43_20150316oss" #
setenv tbb_bin "/home/lou/Documents/graphics-cis560/tbb43_20150316oss/build/linux_intel64_gcc_cc4.8_libc2.19_kernel3.16.0_debug" #
if (! $?CPATH) then #
    setenv CPATH "${TBBROOT}/include" #
else #
    setenv CPATH "${TBBROOT}/include:$CPATH" #
endif #
if (! $?LIBRARY_PATH) then #
    setenv LIBRARY_PATH "${tbb_bin}" #
else #
    setenv LIBRARY_PATH "${tbb_bin}:$LIBRARY_PATH" #
endif #
if (! $?LD_LIBRARY_PATH) then #
    setenv LD_LIBRARY_PATH "${tbb_bin}" #
else #
    setenv LD_LIBRARY_PATH "${tbb_bin}:$LD_LIBRARY_PATH" #
endif #
 #
