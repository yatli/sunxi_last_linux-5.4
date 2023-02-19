#!/bin/bash

export PATH=/home/yatli/riscv64-glibc-gcc-thead_20200702/bin:$PATH

make CROSS_COMPILE=riscv64-unknown-linux-gnu- ARCH=riscv menuconfig

