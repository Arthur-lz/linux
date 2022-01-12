* ARM Linux 内核的异常向量表在arch/arm64/kernel/entry.S文件
```asm
SYM_CODE_START(vectors)
        kernel_ventry   1, t, 64, sync          // Synchronous EL1t
        kernel_ventry   1, t, 64, irq           // IRQ EL1t
        kernel_ventry   1, t, 64, fiq           // FIQ EL1h
        kernel_ventry   1, t, 64, error         // Error EL1t

        kernel_ventry   1, h, 64, sync          // Synchronous EL1h
        kernel_ventry   1, h, 64, irq           // IRQ EL1h
        kernel_ventry   1, h, 64, fiq           // FIQ EL1h
        kernel_ventry   1, h, 64, error         // Error EL1h

        kernel_ventry   0, t, 64, sync          // Synchronous 64-bit EL0
        kernel_ventry   0, t, 64, irq           // IRQ 64-bit EL0
        kernel_ventry   0, t, 64, fiq           // FIQ 64-bit EL0
        kernel_ventry   0, t, 64, error         // Error 64-bit EL0

        kernel_ventry   0, t, 32, sync          // Synchronous 32-bit EL0
        kernel_ventry   0, t, 32, irq           // IRQ 32-bit EL0
        kernel_ventry   0, t, 32, fiq           // FIQ 32-bit EL0
        kernel_ventry   0, t, 32, error         // Error 32-bit EL0
SYM_CODE_END(vectors)


```
> 该表从上到下共有四组，系统调用则属于第三组和第四组

```
    第三组为64位应用程序在异常级别EL0生成的异常。例如64位用户态程序发生系统调用，处理器从异常级别EL0切换到异常级别EL1，并且使用aarch64执行状态处理异常；
    第四组为32位应用程序在异常级别EL0生成的异常。例如32位用户态程序发生系统调用，处理器从异常级别EL0切换到异常级别EL1，并且使用aarch32执行状态处理异常；

    两组中的sync项即为同步异常（包括系统调用）的入口。以64位应用程序的系统调用为例，
    通过sync入口程序跳转到el0_sync处（这是因为kernel_ventry宏会在跳转的目标label前加入el字符串和输入的异常级别），el0_sync入口代码在entry.S文件中

    发生异常的原因会被保存在ESR寄存器中，该寄存器中的值被移位后与各种异常情况的标号值相比较

    asmlinkage表示该C函数可以在汇编语言中被调用
```    
