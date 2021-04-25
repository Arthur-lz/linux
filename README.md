# linux内核学习

## 建立实验环境
### 安装qemu
* 操作系统是manjaro, x86, 64bit cpu
* 直接使用sudo pacman -S qemu安装的话安装失败，没有可执行文件生成，所以采用下面的源码方式安装，实际操作后得知可以安装成功


|步骤|内容|说明|
|:-----|:-----|:-----|
|确定安装目录并进入|cd yourqemupath|我的安装在/opt/中, cd /opt|
|下载qemu源文件|wget https://download.qemu.org/qemu-5.1.0.tar.xz|文件不大，但编译后要10G+磁盘空间|
|解压qemu-5.1.0.tar.xz|tar -xJvf qemu-5.1.0.tar.xz||
|进入qemu-5.1.0目录| cd qemu-5.1.0||
|执行.configure||我的电脑是manjaro, 没有安装pkg-config，需要安装, 安装后再次执行./configure成功生成Makefile|
|make -j4||要执行一段时间，我的电脑大概半小时左右|
|sudo make install|由于是在/opt/目录下，另外还需要操作/usr/local/bin/ 所以需要管理员权限||
|测试安装是否成功|查一下版本号看看，qemu-system-arm -version|QEMU emulator version 5.1.0 Copyright (c) 2003-2020 Fabrice Bellard and the QEMU Project developers|

### 跑一下linux内核试一下qemu(以qemu-system-arm为例)

#### 实验步骤如下
|操作|说明|补充|
|:-|:-|:-|
|首先准备一个busybox|我用的版本是1.27.1版本||
|进入busybox文件夹| cd busybox||
|echo -e "ARCH=arm\nCROSS_COMPILE=arm-linux-gnuebai-" > Makefile|写入配置参数到Makefile|方便执行make|
|make menuconfig|Busybox Settings --->Build Options--->[\*]Build BusyBox as a static binary(no shared libs)|不使用动态库，这样可执行文件busybox里包含了所有的代码||
|make|||
|make install|这步执行成功后，会在busybox根目录下生成一个\_install文件夹||
|将busybox生成的\_install文件夹复制到linux4.0.1内核文件夹下|||
|初始化\_install|将init.sh复制到busybox生成的\_install文件夹下执行|在当前页面文件列表中|
|进入linux4.0.1内核文件夹|cd linux-4.0.1||
|make vexpress_defconfig|这里我以vexpress开发板为例||
|make menuconfig|General setup--->[\*]Initial RAM filesystem and RAM disk(initramfs/initrd)support(\_install) Initrams source file(s)
||Boot options -->()Default kernel command string||
||Kernel Features-->Memory split(3G/1G user/kernel split)
||[\*]High Memory Support||
|make -j4|你的核心多少个就写几个|这一步执行成功后会在boot下生成zImage，以及boot/dts/下生成dtb|
|运行qemu|qemu-system-arm -M vexpress-a9 -smp 4 -m 256M -kernel arch/arm/boot/zImage -append "rdinit=/linuxrc console=ttyAMA0 loglevel=8" -dtb arch/arm/boot/dtb/vexpress-v2p-ca9.dtb -nographic|注：可以使用qemu-system-arm -machine help来查看qemu支持的设备|
* 踩过的坑先记一下先
> yylloc重定义

> #include gcc_header(__GNUC__)没有此定义include/linux/compiler-gcc7.h
```
提示上面这个错误是因为我第一次make时使用的交叉工具链是7.5版本的。我之后选择的工具链是gcc-linaro-5.5.0-2017.10-rc1-i686_arm-linux-gnueabi;
linux4.0.1支持的最大版本arm交叉工具链是5.5，所以如果你安装的是高版本的，就需要再安装一个5.5的或低一些版本的交叉工具链
```
> 不使用linux4.10版本

## CPU内存模型
### CPU可寻址范围不是指CPU字长，是地址总线长度，如8086 CPU字长16位，但其地址总线长度为20位，可寻址是1M, 而非64k
### CPU有三大总线：地址总线、数据总线、控制总线
### 微内核缺点：每一个部分通信都需要IPC，性能低。
### MESI缓存一致性协议, 保证缓存(L1, L2, L3)一致性, 但首先要支持缓存一致性, 有了它就不用锁总线, 锁缓存就可以了
### lock前缀终极结论:  带锁缓存行的指令在锁释放时会把StoreBuffer刷入共享存储
### 如果没有缓存一致性cache-cohe(很古老的x86 cpu), 则为了保持缓存一致只能锁总线 
### Load Load
### Load Store
### Store Load, 会引起CPU指令排序错误
### Store Store, 因为x86不允许CPU直接修改缓存行，所以StoreStore是空操作, 因为对于一个核心来说，写操作都是FIFO的, 写操作不会重排序. 
### C的Volatile, 编译器不会优化

### 系统调用
    是编译器将用户程序中的read/write等系统调用编译成80软中断，操作系统再根据不同的中断号调用内核中的实现的系统调用的 

### mmap
    通过read, write系统调用来实现文件的访问，
    read, 首先需要在用户间建立缓存A，之后在read时将A传给在内核创建的缓存B，内核从磁盘读取到文件内容后将数据写到B，之后再从B拷贝到用户空间的缓存A，这是两次拷贝，A->B, B->A；
    write的过程与read类似。

    常规文件操作为了提高读写效率和保护磁盘，使用了页缓存机制。这样造成读文件时需要先将文件页从磁盘拷贝到页缓存中，由于页缓存处在内核空间，不能被用户进程直接寻址，所以还需要将页缓存中数据页再次拷贝到内存对应的用户空间中。这样，通过了两次数据拷贝过程，才能完成进程对文件内容的获取任务。写操作也是一样，待写入的buffer在内核空间不能直接访问，必须要先拷贝至内核空间对应的主存，再写回磁盘中（延迟写回），也是需要两次数据拷贝。

    而使用mmap操作文件中，创建新的虚拟内存区域和建立文件磁盘地址和虚拟内存区域映射这两步，没有任何文件拷贝操作。而之后访问数据时发现内存中并无数据而发起的缺页异常过程，可以通过已经建立好的映射关系，只使用一次数据拷贝，就从磁盘中将数据传入内存的用户空间中，供进程使用。

    常规文件操作需要从磁盘到页缓存再到用户主存的两次数据拷贝。
    而mmap操作文件，只需要从磁盘到用户主存的一次数据拷贝过程。说白了，mmap的关键点是实现了用户空间和内核空间的数据直接交互而省去了空间不同数据多次拷贝的繁琐过程。因此mmap效率更高。

#### 多路复用(select版)
     用户层需要告诉系统调用select具体文件描述符集合，需要用户态与内核态数据拷贝

#### 多路复用(epoll版)
     在内核开一块空间，即将用户进程中的一段虚拟地址直接mmap到内核，将文件描述符等数据直接写到映射到内核的空间，这样内核与用户空间不再需要系统调用来回传数据，减少了拷贝和系统调用从面提高了性能

#### kafka为什么快? 
     使用系统调用sendfile减少系统调用（read, write)，即实现了零拷贝。

