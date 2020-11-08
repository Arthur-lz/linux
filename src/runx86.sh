#!/bin/fish
qemu-system-x86_64 -m 2048M -kernel ./linux-4.19.24/arch/x86/boot/bzImage -append "root=/dev/ram0 rw rootfstype=ext4 console=ttyS0 init=/linuxrc" -nographic --virtfs local,id=kmod_dev,path=$PWD/forshare,security_model=none,mount_tag=kmod_mount -initrd ./someone/ramdisk.gz
