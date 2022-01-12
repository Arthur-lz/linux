* 问题出处
```
https://syzkaller.appspot.com/bug?id=5d4035ba820addf03b442b34767b363c728da7e3
```

* kexec 是一个 Linux 内核到内核的引导加载程序，可以帮助从第一个内核的上下文引导到第二个内核
* kexec 机制在内核以及用户空间中都有组件。内核提供了几个用于 kexec 重启功能的系统调用。名为 kexec-tools 的用户空间工具使用这些调用，并提供可执行文件来加载和引导“第二内核”

