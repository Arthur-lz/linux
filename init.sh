#!/bin/fish
#初始化busybox生成的文件系统
#


mkdir etc
mkdir dev
mkdir mnt
mkdir -p etc/init.d/
touch etc/init.d/rcS
echo "mkdir -p proc"\n\
"mkdir -p /tmp"\n\
"mkdir -p /sys"\n\
"mkdir -p /mnt"\n\
"/bin/mount -a"\n\
"mkdir -p /dev/pts"\n\
"mount -t devpts devpts /dev/pts"\n\
"echo /sbin/mdev > /proc/sys/kernel/hotplug"\n\
"mdev -s" > etc/init.d/rcS
chmod +x etc/init.d/rcS
touch etc/fstab
echo "proc /proc proc defaults 0 0"\n\
"tmpfs /tmp tmpfs defaults 0 0"\n\
"sysfs /sys sysfs defaults 0 0"\n\
"tmpfs /dev tmpfs defaults 0 0"\n\
"debugfs /sys/kernel/debug debugfs defaults 0 0" > etc/fstab
touch etc/inittab
echo "::sysinit:/etc/init.d/rcS"\n\
"::respawn:-/bin/sh"\n\
"::askfirst:-/bin/sh"\n\
"::ctrlaltdel:/bin/umount -a -r" > etc/inittab
cd dev
sudo mknod console c 5 1
sudo mknod null c 1 3

