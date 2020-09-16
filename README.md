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

1.首先准备一个busybox，用它来编译一个最小的文件系统并配置如下
|||
|:-|:-|
|初始化脚本||

2.将busybox生成的_install文件复制到linux内核根目录下

3.准备linux4.0.1内核

* 踩过的坑先记一下先
> yylloc重定义

> #include gcc_header(__GNUC__)没有此定义include/linux/compiler-gcc7.h
```
提示上面这个错误是因为我第一次make时使用的交叉工具链是7.5版本的。我之后选择的工具链是gcc-linaro-5.5.0-2017.10-rc1-i686_arm-linux-gnueabi;
linux4.0.1支持的最大版本arm交叉工具链是5.5，所以如果你安装的是高版本的，就需要再安装一个5.5的或低一些版本的交叉工具链
```
> 不使用linux4.10版本
