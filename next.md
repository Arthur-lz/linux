2020年 09月 26日 星期六 20:34:07 CST
0.准备gcc编译不优化版本的linux4.0.1内核镜像，用于qemu调试时使用
1.Anjuta环境学习；替换调试工具，其默认使用系统自带的gdb，是否可换成gdb-mltiarch?或其他的如cgdb?
2.准备x86_64、x86根文件系统
3.将新环境连接开发板调通

---
* busybox 做x86_64根文件系统make时问题记录
1. 以静态方式make时, 会出现找不到crypt,m等静态库问题
2. 以动态方式make时，在qemu加载时会出现无法加载linuxrc即busybox问题
