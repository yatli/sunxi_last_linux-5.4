export PATH=/home/yatli/riscv64-glibc-gcc-thead_20200702/bin:$PATH

make -j8 CROSS_COMPILE=riscv64-unknown-linux-gnu- ARCH=riscv dtbs
cp arch/riscv/boot/dts/sunxi/board.dtb out/boot/
