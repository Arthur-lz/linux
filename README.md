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
