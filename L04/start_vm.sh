#!/bin/sh

cd rootfs
find . | cpio -co > ../rootfs.cpio
cd ..

qemu-system-aarch64 \
	-machine virt \
	-cpu cortex-a53 \
	-smp 1 \
	-m 1024 \
	-kernel linux/arch/arm64/boot/Image \
	-initrd rootfs.cpio \
	-display none \
	-serial stdio \
	-no-reboot \
	-append "console=ttyAMA0 panic=-1"
