### qemu 上跑x86_64内核，文件系统使用xfs

#### 环境
* 物理机centos 7, x86_64, kernel 3.10.0
 

#### 一、准备文件
* 1、下载busybox
>  make, make install后生成\_install文件夹

* 2、kernel
>   1)make menuconfig
>     (1)将xfs编译到zImage, 即将原来的M改成*
     
>   2)make bzImage

* 3、基于busybox生成的\_install来制作ramfs.gz  

* 4、下载qemu
>   wget https://download.qemu.org/qemu-5.1.0.tar.xz 
>   make , make install

#### 二、遇到的问题汇总
* qemu启动内核时出现的问题

* 0.qemu: could not load PC BIOS ‘bios-256k.bin’
> yum install seabios 

* 1.VFS: Cannot open root device ram0, unknown-block(0,0): error -19
  root=ram0
> 需要将内核中的文件系统xfs编译到zImage中

* 2.Failed to execute /linuxrc.
>  这个是busybox的问题，从busybox官网上下载一个binary二进制文件, 替换原来的，之后再重新生成ramdisk.gz文件即可
>  https://busybox.net/downloads/binaries/1.31.0-defconfig-multiarch-musl/busybox-x86_64


