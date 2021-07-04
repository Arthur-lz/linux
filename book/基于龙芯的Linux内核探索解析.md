# 第１章基础知识
* 龙芯CPU基于MIPS架构

## 1.1 龙芯处理器简介
* 龙芯1号系列处理器与MIPS32 R2（MIPS R2的32位版本）兼容
* 龙芯2E、2F与MIPS III兼容
* 龙芯2G、2H、3A1000、3B1000、3B1500与MIPS64 R1（MIPS R1的64位版本）兼容
* 龙芯3A2000、3A3000与MIPS64 R2（MIPS R2的完整版）兼容
* 上述（MIPS R2 .xxx）内这些MIPS版本指的是体系结构，即指的是指令集ISA，不是具体的CPU设计
> MIPS公司早期自己生产处理器，现在只对设计授权，不自己生产CPU

* 龙芯处理器在指令集以外的部分的许多设计跟R4000相似（如时钟源、Cache、MMU、FPU的设计等）
> R4000又称r4k

### 1.1.1 龙芯3号功能特征
### 1.1.2 龙芯3号处理器核
### 1.1.3 龙芯电脑基本结构

## 1.2 Linux内核简介
### 1.2.1 Linux内核发展简史
### 1.2.2 Linux内核的开发模式
* 在代码仓库管理上，有主线仓库（mainline）、稳定仓库（stable）、未来仓库（linux-next）和子系统仓库（subsystem）4大类
* 绝大多数开发者贡献的代码首先要接受子系统仓库管理员的审核，才能进入某个特定的子系统仓库；在进入子系统仓库以后，未来仓库会进行二次审核；二次审核通过以后，将进入主线仓库（偶尔也有跳过未来仓库，从子系统仓库直接进入主线仓库的情况）。可以说，代码进入子系统仓库才仅仅处于Alpha状态；进入未来仓库才算达到Beta状态；如果进入了主线仓库，就相当于达到RC状态或者Final状态，算是被官方采纳了。

#### 4类代码仓库
* 一、主线仓库
> 管理员: Linus Torvalds

> Git仓库地址：git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.gt

* 二、稳定仓库
> 管理员：Greg Kroah-Hartman等

> git仓库地址：git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git

* 三、未来仓库
> 管理员：Stephen Rothwell

> git仓库地址：git://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git

* 四、子系统仓库　
> 子系统仓库为数众多，一般按体系结构（arch）、驱动类型（drivers）进行分类

> MIPS子系统管理员：Ralf Baechle

> MIPS子系统git仓库地址：git://git.kernel.org/pub/scm/linux/kernel/git/ralf/linux.git

> GPU子系统管理员David Airlie

> GPU子系统git仓库地址：git://people.freedesktop.org/~airlied/linux

* 内核根目录中的MAINTAINERS文件会列出所有现任管理员及其相关信息
> M开头的行：管理员姓名和电子邮箱

> L开头的行：开发交流的邮件列表

> S开头的行：维护状态

> F开头的行：主要源码路径

### 1.2.3 关于长期维护稳定版本
* 普通的stable版本的维护时间为3个月左右
> 当主线仓库中下一版本发布，上一版本的stable分支就不再升级

### 1.2.4 龙芯的内核版本选型
* 官方内核是“内部完全自洽”的
* 同一个稳定分支的官方内核，版本号第三位越高越稳定

## 1.3 如何高效阅读代码
### 1.3.1 找准入口点
### 1.3.2 理清主脉络
### 1.3.3 顾名思义看功能
### 1.3.4 理解补丁文件
#### 用diff和patch这种原始命令来生成补丁和应用补丁
##### 生成补丁
* 假设当前目录下有linux-4.4.1和linux-4.4.2两个子目录，分别是两个内核不同版本的源代码目录。那么可以使用diff命令来导出一个变更集到补丁文件kernel.patch
```
diff -Naurp linux-4.4.1 linux-4.4.2 > kernel.patch
```

##### 应用补丁
* 进入linux-4.4.1目录，用patch命令来应用补丁文件kernel.patch将linux-4.4.1版本的源码变更成跟linux-4.4.2版本一致
```
patch -p1 < kernel.patch
```

##### 反向应用补丁
* 进入linux-4.4.2目录，用patch命令来应用补丁文件kernel.patch将linux-4.4.2版本的源码恢复成跟linux-4.4.1版本一致
```
patch -Rp1 < kernel.patch
```

#### 使用Git命令来导出和应用补丁文件
* Git中一个版本称为一个commit（提交），用一个40位的16进制数的散列值来表示
##### 导出简单格式补丁
```
git diff commit1 commit2 > kernel.patch
```

##### 应用简单格式补丁
```
git apply kernel.patch
```

##### 导出正规格式补丁
```
git format-patch commit1..commit2 -o kernel_patch_dir
```

##### 应用正规格式补丁
```
git am kernel_patch_dir/*.patch
```

* 正规格式补丁说明
> 如果commit1和commit2相邻，就导出一个补丁；如果不相邻，就会导出一系列补丁。这些补丁保存在kernel_patch_dir目录中，按版本从旧到新的顺序，以001-xxx-yyy.patch、002-xxx-yyy.patch的格式逐个命名

> 正规格式补丁导出以后可以直接以电子邮件的形式发送出去　

> 应用正规格式补丁的同时会自动提交到代码库

> 一个正规格式补丁文件包括4部分内容：头部信息、描述信息、正文区和脚注区

## 1.4 如何开发健壮内核
### 1.4.1内核代码风格
* 内核代码风格规范
> 中文版Documentation/zh_CN/CodingStyle

> 英文版Documentation/CodingStyle

### 1.4.2 合理生成补丁
* 合理使用git，牢记以下原则
> Git记录必须全程有机可回溯

> 补丁质量与代码质量同样重要

> 日志信息与代码质量同样重要

> 每次提交必须是一个完整的功能单位

* 提交日志应当如实描述补丁正文所做的事情，重点描述“为什么”和“怎么做”

### 1.4.3 谨慎对待创新
* 少即是多
* 向开源社区提交补丁与驱动程序的具体方法
> 中文版Documentatio/zh_CN/SubmittingPatches, SubmittingDrivers

> 英文版Documentation/SubmittingPatches, SubmittingDrivers, SubmitChecklist

# 第2章 内核启动解析
* 所有电脑开机以后首先执行的都是？基本输入输出系统（BIOS, Basic Input/Output System）
> BIOS存放在？只读存储器（ROM），是固化在硬件里的软件，即固件。

> BIOS的入口地址固定为？0xbfc00000，对应的物理地址为？0x1fc00000

> 龙芯上的BIOS主要有PMON和昆仑固件两种；PMON是MIPS平台上传统BIOS；昆仑固件是基于UEFI规范的BIOS

> BIOS在初始化硬件之后，通过启动加载器（BootLoader）来加载操作系统内核，然后由内核全面接管整个电脑的控制权

* 启动参数主要包括
> 1.指定根文件系统的参数，如root=/dev/sda1

> 2.指定控制台设备的参数，如console=tty

> 3.指定init程序路径的参数，如init=/sbin/init

> 完整的内核启动参数列表可参数内核自带文档Documentation/admin-guide/kernel-parameters.txt

*龙芯电脑操及其操作系统启动过程总述
```

　 --------              --------              ----                     --------------------
　|上电开机|----------->|昆仑固件|----------->|Grub|------------------>|读取grub.cfg显示菜单|
   --------              --------              ----                     -------------------- 
       |                                                                          |
       |                                                                          V
       |                 --------               --------------------        -------------------------
       ---------------->|  PMON  |------------>|读取boot.cfg显示菜单|----->|Linux 内核vmlinux/vmlinuz|------------- 
                         --------               --------------------        -------------------------             |
                                                                                    |                             |
										    |                             |
										    V                             |
								         ------------------------------           |
									|初始化内存盘(initrd/initramfs)|	  |  
									 ------------------------------           |
									             |                            | 
										     |                            |
										     V                            |
								           ------------------------------         |
									  |  init进程（SysVinit/Systemd）|<--------
                                                                           ------------------------------
									                 |
											 |
											 V
							   ----------		 -----------------             ----------
							  | 图形模式 | <--------|   各种系统服务  |---------->| 命令模式 |
							   ----------		 -----------------             ----------
								|		                                    |
								|			                            |
								V			                            V  
					          ---------------------------	   			 ------------------------
						 |登录管理器(KDM/GDM/LightDM)|	   			| 登录提示符(getty&login)|
						  ---------------------------	   			 ------------------------
                                                               |                                                      |
							       |                                                      |
							       V                                                      V
						 -----------------------------    			     -------------------         
						| 桌面环境(KDE/GNOME/Mate/DDE)|   			    |命令解释器（Shell）|
						 -----------------------------    			     -------------------
						               |                           			      |
							       |                           			      |
							        ------------------------------------------------------
								        		     |
											     V
											 -----------
											|  启动完成 |
											 -----------
```  

## 2.1 内核源码目录结构
* 内核的启动过程实际上就是各个子系统的初始化过程
* 龙芯是多核处理器，在操作系统启动完成后，每个核都是对称的。但是在启动过程中，编号为0的核承担着更多的责任，它是第一个启动的核，在它启动完成之后，才会唤醒其他核
> 0号核也叫启动核或主核

* 在Linux内核中，比"核"更通用的说法是逻辑cpu，一个逻辑cpu就是一个执行任务的最小单位
> 在单核处理器中，逻辑cpu就是物理cpu

> 在多核单线程处理器中，逻辑cpu就是核

> 在单核多线程或者多核多线程处理器中，逻辑cpu就是线程

> 前面说的启动核或主核实际上指的就是启动逻辑cpu和非启动逻辑cpu

## 2.2 内核启动过程：主核视角
* vmlinux, vmlinuz说明
> vmlinux是编译内核产生的ELF可执行内核文件，它是非压缩版本的原始内核

> vmlinuz是将vmlinux压缩以后再加上一个新的ELF头而得到的压缩版本内核

> BIOS即可以启动压缩版本内核，也可以启动原始内核

> 如果启动的是压缩版本内核，在解压前真正的入口是arch/mips/boot/compressed/head.S中的start标号；压缩版本内核在start标号处开始执行时通过decompress_kernel()进行自解压，解压内容释放到内存里形成一个原始内核。解压完毕后，执行流程跳转到原始内核的kernel_entry入口

* 主核的入口点是编译内核时确定的
> 根据内核是否压缩，主核的入口点分成两种

```c
/* 1、未压缩版本的原始内核vmlinux对应的内核的初始入口位于arch/mips/kernel/head.S中的Kernel_entry
 * 
 * 2、压缩版本内核vmlinuz，在解压缩之前真正的入口是arch/mips/boot/compressed/head.S中的start标号
 */
```

### 2.2.1 第一入口：kernel_entry
```
# MIPS汇编写的第一入口kernel_entry

NESTED(kernel_entry, 16, sp)                    # 函数头, 函数名：kernel_entry，栈帧大小16字节，返回地址为SP寄存器的内容
	kernel_entry_setup			# CPU具体类型相关初始化
	setup_c0_status_pri			# 设置主核协处理器0中Status寄存器的初始状态
	PTR_LA t0,	0f		        # 宏指令PTR_LA用于将一个变量的地址加载到寄存器；在32位时宏展开为la, 在64位配置下宏展开为dla
	jr		t0

0:
	PTR_LA		t0, __bss_start
	LONG_S		zero, (t0)
	PTR_LA		t1, __bss_stop - LONGSIZE

1:
	PTR_ADDIU	t0, LONGSIZE
	LONG_S		zero, (t0)
	bne		t0, t1, 1b
	LONG_S		a0, fw_arg0        #保存寄存器a0的内容到内存变量fw_arg0中，作用：保存a1中命令行参数中包含的参数个数
	LONG_S		a1, fw_arg1        #命令行参数, commandline; a0~a3中保存的是BIOS或引导程序传递给内核的参数
	LONG_S		a2, fw_arg2        #龙芯3号以前是key=value；龙芯3号开始是一个指向BIOS中一片数据区的地址，数据区有丰富的数据结构，有丰富的接口信息(见arch/mips/include/asm/mach-loongson64/boot_param.h是UEFI的LEFI接口规范), 其中有cpu和内存分布图定义
	LONG_S		a3, fw_arg3
	MTC0		zero, CP0_CONTEXT
	PTR_LA		$28, init_thread_union #用init_thread_union的地址来初始化GP，GP也叫全局指针，用于访问全局数据，$28代表28号通用寄存器
	PTR_LI		sp, _THREAD_SIZE - 32 - PT_SIZE     # 宏指令PTR_LI用于将一个立即数加载到寄存器; 使用宏指令是为了兼容32,64位内核
	PTR_ADDU	sp, $28
	back_to_back_c0_hzard
	set_saved_sp 	sp, t0, t1
	PT_SUBU		sp, 4 * SZREG
	j		start_kernel		# 跳转到第二入口start_kernel
	END(kernel_entry)                  # 标识函数结尾
```

#### 重要步骤：kernel_entry_setup, setup_c0_status_pri, PTR_LI sp, _THREAD_SIZE - 32 - PT_SIZE说明
* kernel_entry_setup
```
// arch/mips/include/asm/mach-loongson64/kernel-entry-init.h

	.macro	kernel_entry_setup
#ifdef CONFIG_CPU_LOONGSON3
	.set	push
	.set	mips64
	/* 设置*/
	mfc0	t0, CP0_CONFIG3
	or 	t0, (0x1 << 7)
	mtc0	t0, CP0_CONFIG3
	/* 打开ELPA（Enable Large Physical Address, 48位地址空间）功能 */
	...
	/* 如果是龙芯3A2000或更新的处理器就打开SFB功能 */
	...
2:
	_ehb
	.set	pop
#endif
	.endm
```

* setup_c0_status_pri
> 用于设置主核（启动核，或称为启动逻辑cpu）协处理器0（协处理器0是个什么东西？）中Status寄存器的初始状态

> 龙芯协处理器0的Status寄存器各bit位定义

|31～28|27|26|25～24|23|22|21|20|19|18～16|15～8|7|6|5|4～3|2|1|0|
|:-|:-|:-|:-|:-|:-|:-|:-|:-|:-|:-|:-|:-|:-|:-|:-|:-|:-|
|CU3~CU0|0|FR|0|PX|BEV|0|SR|NMI|0|IM7~IM0|KX|SX|UX|KSU|ERL|EXL|IE|

> 源码
```
// arch/mips/kernel/head.S
```

* init_thread_union相关（PTR_LI sp, _THREAD_SIZE - 32 - PT_SIZE）
> 在Linux中，进程和线程都是运行的程序实体，区别是进程有独立的地址空间，线程共享进程的地址空间，也就是说线程是一种特殊的进程；在Linux中线程的容器不是进程，而是线程组。例如，一个运行中的多线程程序是一个线程组，里面包含多个线程；一个运行中的单线程程序也是一个线程组，里面包含一个线程。

> 在windows操作系统中，进程是运行的程序实体，而线程是进程中的独立执行路径，也就是说，进程是容器，线程是容器中的执行体 

> 内核本身也可以视为一个特殊的进程，它可以派生出很多共享地址空间的内核线程，因此这个拥有很多线程的内核又可以视为一个特殊的线程组

> 一些重要的数据结构

```c
union thread_union {
	struct task_struct task;
	struct thread_info thread_info; // 不同的体系结构可以选择使用task_struct或是thread_info中的一个, MIPS选择的是thread_info
	unsigned long stack[THREAD_SIZE/sizeof(long)];
};

struct task_struct init_task = {
	.state		= 0,
	.stack		= init_stack,
	.tasks		= LIST_HEAD_INIT(init_task.tasks),
	.comm		= INIT_TASK_COMM,
	.thread		= INIT_THREAD,
	...
};

#define INIT_TASK_DATA(align)			\
	. = ALIGN(align)			\
	__start_init_task = .;			\
	init_thread_union = .;			\
	init_stack = .;				\
	KEEP( *(.data..init_task))		\          // KEEP(后面的空格完全是为了适应markdown, 不加空格时文件内容显示的状态不对
	KEEP( *(.data..init_thread_info))	\
	. = __start_init_task + THREAD_SIZE;	\
	__end_init_task = .;
	

unsigned thread_union init_thread_union; 	// 0号进程的thread_union
struct thread_info init_thread_info __init_thread_info = INIT_THREAD_INFO(init_task); // ???
unsigned long init_stack[THREAD_SIZE / sizeof(unsigned long)];
unsigned long kernelsp[NR_CPUS]; // 存放栈指针？
```

> 每个进程用一个进程描述符struct task_struct表示

> 每一个进程都有一个和体系结构相关的线程信息描述符struct thread_info 

> 每一个进程都有一个内核栈，用于处理异常、中断或者系统调用

> Linux内核为每一个进程分配一个大小为THREAD_SIZE（一般为一页大小）的内存区，用thread_union结构把task_struct, thread_info, 内核栈三个东西放在一起保存

> 0号进程一开始就是内核自身，在完成启动、初始化之后，变成idle进程

> 对于多核或多处理器系统，每个逻辑cpu都有一个0号进程； 请问每个0号进程都相同吗？因为上面定义的全局的init_thread_union只有一个, init_thread_info也只有一个

> 0号进程的thread_union是init_thread_union; thread_info是init_thread_info;内核栈是init_stack

> 每个逻辑cpu在处于内核态时都有一个当前内核栈，其栈指针就是kernelsp[]数组

### 2.2.2 第二入口：start_kernel
* 原始版本内核: BIOS->kernel_entry->start_kernel

* 压缩版本内核：BIOS->start->kernel_entry->start_kernel

#### start_kernel代码树
```c
start_kernel()
	|---------smp_setup_processor_id();
	|---------cgroup_init_early();
	|---------local_irq_disable();  // 关中断
	|---------boot_cpu_init(); // 设置cpu的存在性状态（有四种possible, present, online, active）
			|-------------cpu = smp_processor_id();
			|-------------set_cpu_online(cpu, ture);  // 在线
			|-------------set_cpu_active(cpu, true);  // 在线并处于活动状态
			|-------------set_cpu_present(cpu, true); // 表示物理上确实存在
			|-------------set_cpu_possible(cpu, true);// 表示物理上可能存在
			\-------------__boot_cpu_id = cpu;
	|---------page_address_init();
	|---------setup_arch(&command_line); 		// 根据体系结构进行相关的初始化
	|---------setup_command_line(command_line);	// 建立内核命令行参数
	|---------setup_nr_cpu_ids();
	|---------setup_per_cpu_areas();		// 建立每CPU变量区
	|---------smp_prepare_boot_cpu();		// 体系结构相关函数，用于把0号逻辑CPU设置成possible、present状态
	|---------boot_cpu_hotplug_init();		// 将启动核，即当前核设置成online状态
	|---------build_all_zonelists(NULL, NULL);
	|---------page_alloc_init();
	|---------parse_early_param();
	|---------vfs_caches_init_early();
	|---------trap_init();				// 异常初始化, 与体系结构相关，非常重要
	|---------mm_init();				// 内存管理初始化
			|-------mem_init();		// 建立内存分布图（将BootMem/MemBlock内存分布图转换为伙伴系统内存分布图，对每个页帧调用set_page_count()设置引用计数为0
			|-------kmem_cache_init(); 	// 完成slab内存对象管理器初始化
			|-------kmemleak_init();	// 内核内存泄漏扫描器初始化
			\-------vmalloc_init();		// 非连续内存区管理器初始化
	|---------sched_init();		// 调度器初始化，完成后主核就可以进行任务调度了
			|-------for_each_possible_cpu(i) { // 循环初始化每个CPU的运行队列rq（用于进程组织和调度，包括cfs, rt, dl这3个子队列）
					rq = cpu_rq(i);
					init_cfs_rq(&rq->cfs);
					init_rt_rq(&rq->rt);
					init_dl_rq(&rq->dl);
					...
				}
			|-------set_load_weight(&init_task); // 设置init_task的负荷权重
			|-------init_idle(current, smp_processor_id());		// 将内核自己进程化，从现在开始内核也是一个“进程”了，即0号进程
			\-------init_sched_fair_class();
	|---------radix_tree_init();
	|---------workqueue_init_early();	// 工作队列初始化，第一部分创建7个系统级工作队列
	|---------rcu_init();			// 初始化RCU子系统
	|---------early_irq_init();		// 初始化中断描述符（irq_desc[NR_IRQS]数组, 包含中断号的芯片数据irq_data和中断处理程序irqaction）
	|---------init_IRQ();		// 这是真正的设置中断描述符中的数据，它与体系结构相关；early_irq_init是将数据设置成no_irq_chip, 处理程序设置成handle_bad_irq()
	|---------tick_init();
	|---------rcu_init_nohz();
	|---------init_timers();	// 基本定时器初始化
	|---------hrtimers_init();	// 高分辨率定时器初始化
	|---------softirq_init();	// 软中断初始化
	|---------timekeeping_init();	// 初始化各种时间变量，如jiffies, xtime
	|---------boot_init_stack_canary();
	|---------time_init();		// 与体系结构相关，进一步初始化计时系统
	|---------perf_event_init();	// PerfEvents性能剖析工具初始化
	|---------profile_init();	// OProfile性能剖析工具初始化
	|---------call_function_init();            // 这之前是第一阶段，单线程关中为阶段 ; 到这里为止，中断相关的初始化都已经完成，可以开中断了 
	|---------local_irq_enable();              // 第二阶段从这里开始
	|---------console_init();			// 控制台初始化
	|---------setup_per_cpu_pageset();
	|---------numa_policy_init();		// NUMA内存分配策略初始化
	|---------acpi_early_init();
	|---------sched_clock_init();
	|---------calibrate_delay();		// 计算loops_per_jiffy（每个时钟节拍对应的循环次数，这个值用于实现各种delay()函数）
	|---------pid_idr_init();
	|---------anon_vma_init();
	|---------thread_stack_cache_init();
	|---------fork_init();			// 初始化fork()函数用到的一些数据结构, 如创建名为“task_struct”的slab对象
	|---------proc_caches_init();
	|---------buffer_init();
	|---------key_init();
	|---------security_init();
	|---------vfs_caches_init();
	|---------pagecache_init();
	|---------signals_init();		// 与信号相关的数据结构初始化。
	|---------proc_root_init();
	|---------cpuset_init();
	|---------cgroup_init();		// CGroup相关数据结构初始化，并创建sysfs, procfs节点
	|---------acpi_subsystem_init();
	\---------arch_call_reset_init();         // 第二阶段到这里结束，开中断单线程阶段
			\-------------rest_init();      // 第三阶段从这里开始，开中断、多线程阶段
					|---------kernel_thread(kernel_init, NULL, CLONE_FS); // 创建1号进程kernel_init和2号进程kthreadd
					|---------numa_default_policy();
					|---------kernel_thread(kthreadd, NULL, CLONE_FS | CLONE_FILES);
					\---------cpu_startup_entry(CPUHP_ONLINE); 
							\--------while (1) do_idle(); // 内核现在主要工作是休息
									\-------cpuidle_idle_call();
											\-------default_idle_call();
												   \--------arch_cpu_idle();
												        	\--------cpu_wait();
```

* 大致可以将整个start_kernel()过程分为3个大的阶段：关中断单线程阶段、开中断单线程阶段、开中断多线程阶段

* 第一阶段：关中断单线程阶段 
> 为什么要关中断？启动初期中断处理的基础设施尚未准备好。关中断是一个体系结构相关的操作，对于MIPS就是将协处理器0中Status寄存器中的IE位清0

> kernel_entry将控制权移交给start_kernel时就是关中断状态的，这也解释了为什么start_kernel函数不是第一句就执行local_disable_irq来关中断

> boot_cpu_init()用于设置启动cpu的存在性状态，有四种存在性状态：possible, present, online, active

> present与possible的区别：这与cpu物理热插拔有关，默认这两个值相关，当物理上移除一个cpu时，present数目会减少一个

> present与online的区别：这与cpu的逻辑热插拔有关，在不改变硬件的情况下，可以对/sys/devices/system/cpu/cpuN/online写0来关闭一个cpu，写1打开一个cpu

> online表示这个cpu可以调度任务了

> active表示可以往这个cpu迁移任务了

> online与active的区别：在通过逻辑热插拔关闭一个cpu的过程中，被关闭的cpu首先必须退出active状态，然后才能退出online状态

> setup_command_line()用于建立命令行参数。内核命令行参数可以写在启动配置文件（boot.cfg或grub.cfg）中，由BIOS或启动器（Bootloader, 如grub）传递给内核；也可以在编译内核时指定为缺省参数。该函数会将各个来源的参数综合在一起，处理成最终状态

> 普通数组中每个元素在内存中是相邻的

> 每cpu数组在内存中不相邻，每CPU数组中的每个元素位于不同的Cache行

> 把每cpu数组设计成跨cache line的作用：修改某个CPU的元素不会让另一个CPU的元素因为同在一个cache line而被污染，因而可以更有效利用cache; 这是典型的空间换时间

> 时间变量jiffies记录了系统启动以来的经历的节拍数；xtime记录的时间可以精确到纳秒

> 对于龙芯平台来说，第一阶段除了关中断以外，还有一大特点是：显示器上没有任何输出信息。因为龙芯内核使用哑控制台作为初始控制台，没有从BIOS继承任何可以显示的控制台信息

* 第二阶段：开中断单线程阶段
> 现在已经是开中断，尽管现在是单线程，但一旦产生中断就会切换控制流

> 这一阶段除了按顺序执行代码流程外，还可能以交错方式执行中断处理程序代码

> console_init()，控制台初始化。在内核中，控制台基于“键盘 + 鼠标 + 显示器”的VTConsole, 基于串口的SericalConsole，基于IP网络的NetConsole三类。console_init()要完成上述三类控制台的初始化，该函数执行完成后显示器上依然没有输出信息，但是所有的准备工作都已经做好了，包括启动徽标已经在内存VTConsole的屏幕缓冲区中绘制好了，只要显卡初始化一完成，基于FrameBuffer的FBConsole就会代替DummyConsole，从而输出显示信息（将显示内容从VTConsole的屏幕缓冲区复制到显卡的帧缓存FrameBuffer）

> 信号之于进程，好比中断之于内核。信号用于打断当前的执行流程，去完成一些更重要的工作

> CGroup全称Control Group（控制组），是内核中一种控制资源分配的机制

> 第二阶段完成所有调度有关的子系统初始化，这样在下一阶段就可以创建内核线程了，可以以并发的方式继续内核启动了

> 因显卡还未初始化，所以第二阶段显示器上无输出

* 第三阶段：开中断多线程阶段
> 1号、2号进程实际上是两个内核线程

> 2号进程是除0、1、2号进程以外其他所有内核线程的祖先

> 1号进程和2号进程创建以后，内核自己的初始化工作就基本完成了，余下的工作交给1号进程和2号进程

> 内核初始化的最后一步是执行cpu_startup_entry，通过执行do_idle()函数让0号进程（内核）休息，（如果别的进程有事要做，就调度别的进程，反之意味着系统空闲，回到0号进程，而0号进程本身就是idle进程，内核本身）

> 在SMP系统上，1号进程会打开所有其他CPU／核，让后面的内核启动真正并行起来。包括显卡驱动在内的各种设备驱动都在1号进程里完成

> 在这一阶段，显示器上在部分时间是有输出信息的

### 2.2.3 重要函数：setup_arch()





