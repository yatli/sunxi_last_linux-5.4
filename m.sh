export PATH=/home/yatli/riscv64-glibc-gcc-thead_20200702/bin:$PATH

make -j8 CROSS_COMPILE=riscv64-unknown-linux-gnu- ARCH=riscv
make CROSS_COMPILE=riscv64-unknown-linux-gnu- ARCH=riscv INSTALL_MOD_PATH=out/rootfs/ modules_install
make CROSS_COMPILE=riscv64-unknown-linux-gnu- ARCH=riscv INSTALL_PATH=out/boot/ zinstall
cp arch/riscv/boot/dts/sunxi/board.dtb out/boot/
