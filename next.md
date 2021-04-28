2020年 09月 26日 星期六 20:34:07 CST
0.准备gcc编译不优化版本的linux4.0.1内核镜像，用于qemu调试时使用
1.Anjuta环境学习；替换调试工具，其默认使用系统自带的gdb，是否可换成gdb-mltiarch?或其他的如cgdb?
2.准备x86_64、x86根文件系统
3.将新环境连接开发板调通

---
* busybox 做x86_64根文件系统make时问题记录
1. 以静态方式make时, 会出现找不到crypt,m等静态库问题
2. 以动态方式make时，在qemu加载时会出现无法加载linuxrc即busybox问题
0. 使用4.19.27版本的内核可以顺利make，不过调试时出现无法进入源码问题，以及断点设置后无法跟踪运行到断点位置问题
   使用gdb, cgdb, gdb-mulitarch三个gdb，调试的结果相同。
   不过有一点可以肯定，使用anjuta是可以使用其他gdb来调试的，如gdb-multiarch, cgdb等
---

## cc1: 错误：代码模式 kernel 在 PIC 模式下不受支持
* 使用gcc10和gcc9, gcc8均会出现同样错误; 

---
---
## 入职前准备
> 因这家公司不让带电脑，所以，必须自己在公司电脑上重新安装开发环境，以及其他需要的资源，所以需要列一份清单
### 软件清单
* i3
* fish
* ohmyfish
> curl -L https://get.oh-my.fish > install
> fish install --path=~/.local/share/omf --config=~/.config/omf
* alacritty
* compton
* feh
* vim
* nodejs
* plug in vim setup
* 中文输入法, fcitx
* git
* ranger
* flameshot
* libreoffice
* pcmanfm
* w3m
* curl
* cgdb
* gdb-multiarch
* anjuta
### 系统配置
* i3的配置
* vim配置
* anjuta配置

## 面试时遇到的问题
### 中断工作流程 
### 线程切换与进程切换区别
### timer中断是什么中断？硬中断还是软中断?
### 进程调度
