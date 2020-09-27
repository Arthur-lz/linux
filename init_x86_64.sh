#!/bin/fish
#初始化busybox生成的文件系统
#
# 
sudo rm -rf rootfs
sudo rm -rf tmpfs
sudo rm -rf ramdisk*
sudo mkdir rootfs
sudo cp ../busybox-1.27.1/_install_x86_64/*  rootfs/ -raf
sudo mkdir -p rootfs/root/
sudo mkdir -p rootfs/var/
# etc 
sudo mkdir -p rootfs/etc/init.d/
sudo touch rootfs/etc/init.d/rcS
sudo echo "mkdir -p proc"\n\
"mkdir -p /tmp"\n\
"mkdir -p /sys"\n\
"mkdir -p /mnt"\n\
"/bin/mount -a"\n\
"mkdir -p /dev/pts"\n\
"mount -t devpts devpts /dev/pts"\n\
"echo /sbin/mdev > /proc/sys/kernel/hotplug"\n\
"mdev -s" > rootfs/etc/init.d/rcS
sudo chmod +x rootfs/etc/init.d/rcS

sudo touch rootfs/etc/fstab
sudo echo "proc /proc proc defaults 0 0"\n\
"tmpfs /tmp tmpfs defaults 0 0"\n\
"sysfs /sys sysfs defaults 0 0"\n\
"tmpfs /dev tmpfs defaults 0 0"\n\
"debugfs /sys/kernel/debug debugfs defaults 0 0" > rootfs/etc/fstab
sudo touch rootfs/etc/inittab
sudo echo "::sysinit:/etc/init.d/rcS"\n\
"::respawn:-/bin/sh"\n\
"::askfirst:-/bin/sh"\n\
"::ctrlaltdel:/bin/umount -a -r" > rootfs/etc/inittab
# 因为我使用的是动态编译的busybox，所以根文件系统中需要加入编译busybox所需要的动态库
# 一个是ld-linux-x86-64.so.2, 它需要放在lib64/目录下
# 另一个是libc.so.6，它需要放在usr/lib/目录下
sudo mkdir -p rootfs/lib64
sudo cp /usr/lib64/ld-linux-x86-64.so.2 rootfs/lib64/
sudo rm rootfs/lib64/*.a
sudo strip rootfs/lib64/*
sudo mkdir -p rootfs/usr/lib/
sudo cp /usr/lib/libc.so.6 rootfs/usr/lib/
sudo mkdir -p rootfs/dev/
sudo mknod rootfs/dev/tty1 c 4 1
sudo mknod rootfs/dev/tty2 c 4 2
sudo mknod rootfs/dev/tty3 c 4 3
sudo mknod rootfs/dev/tty4 c 4 4
sudo mknod rootfs/dev/console c 5 1
sudo mknod rootfs/dev/null c 1 3
sudo dd if=/dev/zero of=ramdisk bs=1M count=32
sudo mkfs.ext4 -F ramdisk
sudo mkdir -p tmpfs
sudo mount -t ext4 ramdisk ./tmpfs/  -o loop
sudo cp -raf rootfs/*  tmpfs/
sudo umount tmpfs
sudo gzip --best -c ramdisk > ramdisk.gz
