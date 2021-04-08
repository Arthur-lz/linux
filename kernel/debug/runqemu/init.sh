#!/bin/fish
#初始化busybox生成的文件系统
#

#!/bin/bash
sudo rm -rf rootfs
sudo rm -rf tmpfs
sudo rm -rf ramdisk*
sudo mkdir rootfs
sudo cp _install/*  rootfs/ -raf
sudo mkdir -p rootfs/proc/
sudo mkdir -p rootfs/sys/
sudo mkdir -p rootfs/tmp/
sudo mkdir -p rootfs/root/
sudo mkdir -p rootfs/var/
sudo mkdir -p rootfs/mnt/
sudo cp etc rootfs/ -arf
sudo mkdir -p rootfs/lib64
#sudo cp -arf /lib64/* rootfs/lib64/
#sudo rm rootfs/lib64/*.a
#sudo strip rootfs/lib64/*
sudo mkdir -p rootfs/dev/
sudo mknod rootfs/dev/tty1 c 4 1
sudo mknod rootfs/dev/tty2 c 4 2
sudo mknod rootfs/dev/tty3 c 4 3
sudo mknod rootfs/dev/tty4 c 4 4
sudo mknod rootfs/dev/console c 5 1
sudo mknod rootfs/dev/null c 1 3
sudo dd if=/dev/zero of=ramdisk bs=1M count=32
sudo mkfs.xfs ramdisk
sudo mkdir -p tmpfs
sudo mount -t xfs ramdisk ./tmpfs/  -o loop
sudo cp -raf rootfs/*  tmpfs/
sudo umount tmpfs
sudo gzip --best -c ramdisk > ramdisk.gz
