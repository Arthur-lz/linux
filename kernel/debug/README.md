### 调试
```shell
qemu-system-x86 -kernel ./bzImage -m 1024M -smp 2 -initrd ./ramfs.gz \
		-append "root=/dev/ram0 rw rootfstype=xfs console=ttyS0 init=/linuxrc" \
		-nographic \
		-gdb tcp::9000
```

* 执行上面的脚本启动kernel后，就可以在本机上打开gdb来调试qemu中的kernel了
