#!/bin/sh
#set -ex

# NOTE: do not run as script

#mkdir rootfs/ rootfs/sbin rootfs/bin rootfs/lib rootfs/usr rootfs/usr/sbin rootfs/usr/bin rootfs/usr/lib rootfs/usr/include rootfs/etc rootfs/var rootfs/dev rootfs/proc rootfs/sys
#git clone --depth=1 --branch=v6.5 https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/
#cd linux
#cp ../linux_config .config
#make olddefconfig
#make -j $(nproc)
#make INSTALL_HDR_PATH=../rootfs/usr/ headers_install
#cd ..
#git clone --depth=1 git://uclibc-ng.org/git/uclibc-ng
cd uclibc-ng
cp ../libc_config .config
make olddefconfig
make -j $(nproc)
make PREFIX=../rootfs/ install
cd ..
git clone --depth=1 git://git.busybox.net/busybox
cd busybox
cp ../busybox_config .config
make olddefconfig
make -j $(nproc)


qemu-system-aarch64 -machine virt -cpu cortex-a53  -kernel linux/arch/arm64/boot/Image -smp 1 -m 1024 -initrd rootfs.cpio -display none -serial stdio -append "console=ttyAMA0"
