## 实验步骤

1. 在linux内核源代码drivers目录下创建一个hello目录
2. 将hellokernel.c, Kconfig, Makefile三个文件放到hello目录下
3. 在linux内核drivers中的Kconfig,加上一句

   **source driver/hello/Kconfig**
4. 进入linux内核根目录, make menuconfig
5. 进入Device Drivers，最后一个就是刚刚增加的驱动hello_lz，
   进入里面，就可以选择hello_kernel了。选择之后，退出并保存，
   则在linux内核根目录的.config文件内就会自动增以下这段定义，

   **CONFIG_HELLO_KERNEL=y**
6. 在linux内核的drivers/目录下的Makefile中添加如下这一句，
   **obj-$(CONFIG_HELLO_KERNEL)      += hello/**

>   当CONFIG_HELLO_KERNEL 的值为y或m时，kbuild将会把hello 目录列入向下迭代的目标中，具体hello 目录下的文件是要作为模块编译还是链入内核，由hello目录下的Makefile文件的内容决定。


* hellokernel.c源文件如下
```c
#include <linux/kernel.h>
#include <linux/module.h>

static int __init helloinit(void)
{
	printf("hello, kernel , lz in\n");
	return 0;
}

static void __exit helloexit(void)
{
	printk("hello, kernel, good by.\n");
}

module_init(helloinit);
module_exit(helloexit);

MODULE_LICENSE("GPL");
```
* Kconfig文件内容如下
```c
menu "hello_lz"
	config HELLO_KERNEL
		bool "hello_kernel"
		default n
	help
	if you unselect it, you can do nothing.
endmenu
```

* Makefile
```
obj- $(CONFIG_HELLO_KERNEL) += hellokernel.o
```
