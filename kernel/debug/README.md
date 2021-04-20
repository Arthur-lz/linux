### 调试
```shell
qemu-system-x86 -kernel ./bzImage -m 1024M -smp 2 -initrd ./ramfs.gz \
		-append "root=/dev/ram0 rw rootfstype=xfs console=ttyS0 init=/linuxrc" \
		-nographic \
		-gdb tcp::9000
```

* 执行上面的脚本启动kernel后，就可以在本机上打开gdb来调试qemu中的kernel了
```shell
gdb vmlinux
(gdb) target remote localhost:9000 // gdb连接到虚拟机qemu中的内核
(gdb) b some_kernel_function       // 找到任何一个内核函数，设置断点, 如vfs_write
```

> 当然可以在gdb中做的，都可以在ddd中做，而且更方便。

* 物理机上的linux如何与qemu中的linux内核进行通信？
> tcp/ip? 

> debugfs?
