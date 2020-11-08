# linux内核文件的区别(vmlinux、zImage、bzImage、uImage、vmlinuz、initrd )
|内核|说明|
|:-|:-|
|vmlinux|  编译出来的最原始的内核文件，未压缩。
|zImage |  是vmlinux经过gzip压缩后的文件。
|bzImage| bz表示“big zImage”，不是用bzip2压缩的。两者的不同之处在于，zImage解压缩内核到低端内存(第一个640K)，bzImage解压缩内核到高端内存(1M以上)。如果内核比较小，那么采用zImage或bzImage都行，如果比较大应该用bzImage。
|uImage|   U-boot专用的映像文件，它是在zImage之前加上一个长度为0x40的tag。
|vmlinuz|  是bzImage/zImage文件的拷贝或指向bzImage/zImage的链接。
|initrd |  是“initial ramdisk”的简写。一般被用来临时的引导硬件到实际内核vmlinuz能够接管并继续引导的状态。


