#!/bin/bash
# linuxrc就是用busybox编译安装时产生的可执行文件busybox
# ram0
# ramdisk.gz是将busybox生成的_install目录及后添加的文件系统相关文件经xfs挂载后生成的，详见init.sh
sudo qemu-system-x86_64 \
	-smp 2 \
	-m 1024M \
    	-kernel ./linux-3.10.0-1127.el7.x86_64_qdma_xfs/arch/x86_64/boot/bzImage \
        -nographic \
	-append "root=/dev/ram0 rw rootfstype=xfs console=ttyS0 init=/linuxrc" \
	-initrd ./linux-3.10.0-1127.el7.x86_64_qdma_xfs/ramdisk.gz 
	#-net nic,vlan=0
