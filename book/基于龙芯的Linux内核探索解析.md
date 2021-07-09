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
					|---------kernel_thread(kernel_init, NULL, CLONE_FS); // 创建1号进程kernel_init
					|---------numa_default_policy();
					|---------kernel_thread(kthreadd, NULL, CLONE_FS | CLONE_FILES);// 创建2号进程kthreadd
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
```c
// arch/mips/kernel/setup.c
setup_arch()
	|---------cpu_probe();	// 探测CPU类型, 依据32位PRID寄存器（高8位保留，次高8位存公司ID，第三个8位存处理器ID，最后一个8位存修订号）
			|---CaseA: cpu_probe_legacy(c, cpu); 		// 无公司ID时用 legacy
			|---CaseB: cpu_probe_loongson(c, cpu);		// 龙芯3A2000后有公司ID，用loonsgon
			|---CaseC: others
	|---------prom_init(); 					// P Rom , 指BIOS芯片，通过BIOS传递来的信息来初始化
			|---prom_init_cmdline(); 		// 处理fw_arg0（参数个数）, fw_arg1（字符串数组）
			|---prom_init_env(); 			// 用fw_arg2初始化环境变量
			|---loongson_pch->early_config(); 	// 龙芯芯片组（ls2h, ls7a, rs780）相关初始化
				\---ls2h_early_config()/ls7a_early_config();/rs780_early_config();
			|---prom_init_memory(); / prom_init_numa_memory(); 	// 内存分配图初始化（根据是否为numa来选择这两个函数之一）
			|---prom_init_uart_base();				// 初始化CPU串口的基地址
			|---register_smp_ops(&loongson3_smp_ops);		// 初始化龙芯多核支持的SMP操作函数集
			|---board_ebase_setup = mips_ebase_setup;
			\---board_nmi_handler_setup = mips_nmi_setup;
	|---------arch_mem_init(cmdline_p);
			|---plat_mem_setup();		// 设置系统控制台（如果配置了VGA控制台，那么系统控制台就是vga_con，否则是哑控制台）
			|---bootmem_init();		// 建立boot_mem_map内存映射图
			|---device_tree_init();		// 校验解析并初始化设备树 
			|---sparse_init();		// 稀疏型内存模型初始化 
			\---plat_swiotlb_setup();	// DMA地址-物理地址的软件中转站SWIOTLB初始化
	|---------plat_smp_setup();
			\---mp_ops->smp_setup();
				\---loongson3_smp_setup();
	|---------prefill_possible_map();
	|---------cpu_cache_init();
			|---r4k_cache_init();
			\---setup_protection_map();
	\---------paging_init();			// 初始化各内存页面管理区(zone)
```

> prom_init_memory()完成内存分布图初始化。以numa版本的内存分布图初始化函数prom_init_numa_memory()来说，它首先初始化NUMA节点的距离矩陈，然后逐个解析内存分布图并将最终结果保存于loongson_memmap中，最后建立逻辑CPU和节点的映射关系（即CPU拓扑图）。非numa版本是一个简化版本只解析节点0的内存映射

> NUMA节点距离矩陈用于描述跨节点访问内存的代价。在linux系统里可以使用numactl --hardware来查看节点距离矩陈

> CPU拓扑图和节点距离矩陈是紧密相关的，节点距离矩陈只描述了各节点之间的距离，并没有描述各个核（逻辑CPU）之间的距离。因此有了CPU拓扑图，就知道哪个核属于哪个节点，哪个节点包括哪些核，这样才能知道任意两个核之间的距离

> boot_mem_map是BootMem内存分布图，它主要是给BootMem内存分配器使用，只包含系统内存

> loongson_memmap是BIOS内存分布图，记录了包括NUMA节点和多种内存类型在内的更多信息

> 如果启动时配置了initrd/initramfs，那么bootmem_init()还有一个功能就是通过init_initrd()处理initrd/initramfs的起始地址、结束地址和设备节点，再通过finalize_initrd()将initrd/initramfs所在的内存段设置为保留。Linux5.4已经淘汰BootMem而全面使用MemBlock

> 内存模型指的是物理地址空间分布的模型，Linux内核支持3种内存模型：平坦型、非连续型、稀疏型内存模型

> 包括龙芯在内的现代体系结构大多采用了比较自由的稀疏型内存模型

> SWIOTLB，这是一种DMA API。龙芯3号的访存能力是48位，但是由于芯片组或者设备本身的限制，设备的访存能力往往没有这么大。比如龙芯的顶级I/O总线（HT总线）位宽只有40位，一部分PCI设备的访存能力只有32位，而ISA/LPC设备的访存能力甚至只有24位。为了让任意设备能够对任意内存地址发起DMA访问，就必须在硬件上设置一个“DMA地址－物理地址”的翻译表，或者由内核在设备可访问的地址范围内预先准备一块内存做中转站。许多X86处理器在硬件上提供翻译表（IOMMU）；龙芯没有IOMMU，于是提供了软件中转站，也就是SWIOTLB

> plat_swiotlb_setup()初始化SWIOTLB元数据并注册DMA API操作集loonson_dma_map_ops, DMA API与芯片组相关

> LS2H芯片组：物理地址转DMA地址使用loongson_ls2h_phys_to_dma()；DMA地址转物理地址使用loongson_ls2h_dma_to_phys()

> LS7A：物理地址转DMA地址使用loongson_ls7a_phys_to_dma(); DMA地址转物理地址使用loongson_ls7a_dma_to_phys()

> RS780E: 物理地址转DMA地址使用loongson_rs780_phys_to_dma(); DMA地址转物理地址使用loongson_rs780_dma_to_phys()

> plat_smp_setup()

```c
struct plat_smp_ops loongson3_smp_ops = {
	.send_ipi_single = loongson3_send_ipi_single, 	// 用于核间通讯
	.send_ipi_mask = loongson3_send_ipi_mask,
	.smp_setup = loongson3_smp_setup,		// 在主核上执行，用于启动辅核
	.prepare_cpus = loongson3_prepare_cpus,		// 在主核上执行, 用于启动辅核
	.boot_secondary = loongson3_boot_secondary,	// 在主核上执行, 用于启动辅核
	.init_secondary = loongson3_init_secondary,	// 在辅核上执行
	.smp_finish = loongson3_smp_finish,		// 在辅核上执行
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_disable = loongson3_cpu_disable,		// 用于CPU热插拔
	.cpu_die = loongson3_cpu_die,			// 用于CPU热插拔
#endif
};
```

> smp_setup()->loongson3_smp_setup()建立CPU逻辑编号与物理编号对应关系；初始化IPI寄存器的地址和操作函数；确定主核的封装编号和核编号

> 为什么会有CPU逻辑编号和物理编号？这与保留掩码是否为0有关，当所有核都投入使用时保留掩码为0；当一部分核不使用时，保留掩码非0，这意味着投入使用的核不一定从0开始也不一定编号连续，但是Linux内核的很多数据结构和核心代码假定逻辑CPU的编号从0开始并且连续，所以需要区分物理编号和逻辑编号。内核提供了两个数组来管理：__cpu_number_map[]（用于物理编号到逻辑编号的映射）, __cpu_logical_map[]（用于逻辑编号到物理编号的映射）

> IPI（处理器间中断，Inter-Processor Interrupt）是核间通信的机制。每个CPU核对应有8个IPI寄存器（IPI_Status, Enable, Set, Clear, Mailbox0~3），前4个寄存器是32位，分别表示32种IPI，Mailbox寄存器64位，用于传递中断以外的更多信息。

> IPI_Status是状态寄存器（只读），某一位为1表示收到了某种IPI

> IPI_Enable是使能寄存器，往某一位写1表示允许某种IPI

> IPI_Set是触发寄存器，往某一位写1表示触发某种IPI

> IPI_Clear是清除寄存器，往某一位写1表示清除某种IPI

> prefill_possible_map()建立合理的逻辑CPU的possible值，它依据在启动参数nr_cpus和编译配置NR_CPUS中取最小值。获取到合理的possible值后，会调用set_cpu_possible()来更新cpu_possible_mask，最后将possible值赋给全局变量nr_cpu_ids

> paging_init()，初始化各个内存页面管理区。在初始化每个zone时，会调用init_page_count()将每个页帧的初始引用计数设置为1。因为此时此刻内存还处于BootMem管理器控制，这些页帧尚未转交到伙伴系统（内存页帧管理器），不是自由页帧，不可以被伙伴系统的页帧分配函数分配

#### 各种地址的定义
* 虚拟地址
> 也叫逻辑地址或程序地址，是从CPU角度看到的地址，也是写在代码里的那个地址。引入虚拟地址的主要原因是支持多任务。因为有了虚拟地址及其地址空间的隔离，多个任务才能使用同样的程序地址并发地跑在同一个CPU上（实际每个任务占用的是不同的物理内存）。

* 线性地址
> 在x86里，表示虚拟地址经过段转换得到的地址，实模式下等于物理地址，保护模式下需要经过页转换才能得到物理地址。在龙芯里，不需要经过页转换的虚拟地址即为线性地址，如32位地址空间里的KSEG0、KSEG1地址以及64位地址空间里的XKPHYS地址

* 物理地址
> 表示虚拟地址或线性地址（x86）经过页转换得到的地址。龙芯的线性地址不需要页转换，直接去掉类型前缀就可以得到物理地址。值得注意的是，此处的物理地址是“初始物理地址”，是CPU核的地址总线发出的地址；其要经过各级交叉开关的转换才能得到“最终物理地址”，即内存条上的地址

* 总线地址
> 也叫DMA地址，是从设备角度看到的地址，在没有IOMMU时等于物理地址，在有IOMMU时经过简单转换可以得到物理地址

#### 页面管理区zone 
* ZONE_DMA
> 包括所有物理地址小于16MB的页面

> 设置这个区的目的：为ISA/LPC等DMA能力只有24位地址的设备服务

* ZONE_DMA32
> 包括所有ZONE_DMA区之外的物理地址小于4GB的页面

> 设置这个区的目的：是为DMA能力只有32位地址的PCI设备服务

* ZONE_HIGHMEM
> 设置这个区的目的：是为物理地址超过线性地址表达能力的内存服务

> 对于32位的MIPS内核，线性地址表达能力只有512MB，因此512MB以外的页面被放置到ZONE_HIGHMEM区

> 对于64位的MIPS内核，物理地址暂时还没有超过线性地址表达能力，因此通常不设置ZONE_HIGHMEM区 

* ZONE_NORMAL
> 该区包含了前面几个区以外的所有页面

#### MIPS的虚拟地址分成3类
* 不缓存并且不分页的，这种虚拟地址既不需要Cache也不需要TLB

* 缓存但不分页的，这类地址需要Cache但不需要TLB

* 缓存并且分页的，这类地址既需要Cache也需要TLB

#### MIPS的虚拟地址空间划分
* 32位地址空间

|起始地址|空间名称|缓存吗？|分页吗？|
|:-|:-|:-|:-|
|0xC000 0000|KSEG2(1GB)|缓存|分页|
|0xA000 0000|KSEG1(512MB)|不缓存|不分页|
|0x8000 0000|KSEG0(512MB)|缓存|不分页|
|0x0000 0000|USEG(2GB)|缓存|分页|

* 64位地址空间
> 下图表中给出的是64位地址空间划分的最大能力，不是当前状态

|起始地址|空间名称|缓存吗？|分页吗？|权限|
|:-|:-|:-|:-|:-|
|0xFFFF FFFF C000 0000|CKSEG2(1GB)||||
|0xFFFF FFFF A000 0000|CKSEG1(512MB)||||
|0xFFFF FFFF 8000 0000|CKSEG0(512MB)||||
|0xC000 0000 0000 0000|XKSEG(4EB-2GB)|缓存|分页|只有内核可访问|
|0x8000 0000 0000 0000|XKPHYS(4EB)|是否缓存由地址的59～61位决定|不分页||
|0x4000 0000 0000 0000|XSSEG(4EB)||||
|0x0000 0000 0000 0000|XUSEG(4EB)|||用户态唯一可访问的地址段；内核当然也可以|

### 2.2.4 重要函数：trap_init()
* trap_init()的主要工作是CPU异常的初始化，大致可分为
> 准备工作

> 每CPU配置

> 建立异常向量表

```c
trap_init()
	|-----check_wait(); 		// 设置cpu_wait函数指针
	|-----ebase = CAC_BASE; / ebase = CKSEG0ADDR(ebase_pa);
	|-----if (board_ebase_setup) board_ebase_setup();
	|-----per_cpu_trap_init(true);
		|-----configure_status(); 			// 配置Status寄存器
		|-----configure_hwrena();			// 配置HWREna寄存器
		|-----configure_exception_vector();		// 配置异常向量的入口地址（主要是EBase寄存器）
		|-----if (!is_boot_cpu) cpu_cache_init();
		|-----tlb_init(); 	// 内核使用微汇编器动态生成TLB异常处理函数；因为TLB异常调用频率高，所以性能要求也非常高，所以。。
			\---build_tlb_refill_handler(); // 设置第一层次的向量0（TLB重填异常）、向量1（XTLB重填异常）
		\-----TLBMISS_HANDLER_SETUP();
			\---TLBMISS_HANDLER_SETUP_PGD(swapper_pg_dir);
	|------set_handler(0x180, &except_vec3_generic, 0x80); /* 设置向量3的处理函数，即except_vec3_generic(), 这个函数根据异常号查询第二层次的异常向量表exception_handlers[32]数组，如果匹配，则调用相应的处理函数*/
	|------for (i = 0; i <= 31; i++) set_except_vector(i, handle_reserved);
	|------set_except_vector(EXCCODE_INT, using_rollback_handler()?rollback_handle_init:handle_int);
	|-----/* 用set_except_vector()设置其他异常向量，方法同上 */
	|------board_nmi_handler_setup();
		\---mips_nmi_setup();
			\---memcpy(base, &except_vec_nmi, 0x80);
	|------board_cache_error_setup();
		\---r4k_cache_setup();
			\---set_uncached_handler(0x100, &except_vec2_generic, 0x80);
	\------local_flush_icache_range(ebase, ebase + vec_size); // 刷新EBase地址开始的一段内存，保证动态生成的代码被真正写入了内存，并且保证I-Cache里的内容不是过期的代码
```

> MIPS指令集中定义了WAIT指令，用作暂停流水线，降低空闲时CPU功耗。类似于x86的HLT指令。龙芯3A1000以前的CPU没有实现WAIT指令；龙芯3A2000以后的CPU实现了真正的WAIT指令，此时cpu_wait被设置成标准的r4k_wait()；其他龙芯处理器保持为空

> EBase寄存器，重定位寄存器：协处理器0中的EBase寄存器。EBase寄存器中的地址默认为CKSEG0（0xffff ffff 8000 0000）

> 龙芯处理器异常分类

|异常类型|异常入口（BEV=0）|异常入口（BEV=1）|
|:-|:-|:-|
|硬复位、软复位、NMI|0xffff ffff bfc0 0000|0xffff ffff bfc0 0000|
|TLB重填|0xffff ffff 8000 0000|0xffff ffff bfc0 0200|
|XTLB重填|0xffff ffff 8000 0080|0xffff ffff bfc0 0200|
|Cache错误|0xffff ffff a000 0100|0xffff ffff bfc0 0300|
|其他通用异常（包括陷阱、断点、系统调用、保留指令、地址错误、中断等）|0xffff ffff 8000 0180|0xffff ffff bfc0 0200|
|EJTAG调试（ProbeEn =0）|0xffff ffff bfc0 0480|0xffff ffff bfc0 0480|
|EJTAG调试（ProbeEn =1）|0xffff ffff ff20 0200|0xffff ffff ff20 0200|

> BEV是协处理器0中Status寄存器的BEV位，处理器刚上电时BEV＝1，但在内核刚刚开始的时候，在kernel_entry入口开始就把BEV清0了, 所以对于上表只需关心BEV=0的情况

> 硬复位、软复位和NMI是非常紧急或致命的异常，它们的异常处理程序入口地址永远是0xffff ffff bfc0 0000（BEV=0或BEV=1都是），这个地址也是所有MIPS处理器上电以后PC的初始值; 也就是说这几个异常的处理一直在BIOS里

> TLB重填和XTLB重填是调用频率非常高的异常，因此设置了专门的异常入口

> Cache错误发生时，cache本身不可用，因此cache错误的入口默认在CKSEG1段偏移为0x100的地方

> 其他各种通用异常共享同一个中断入口，默认在CKSEG0段偏移为0x180的地方

> 龙芯处理器在0xffff ffff bfc0 0000开始的1MB空间实际上不是内存空间，而是映射到BIOS的ROM芯片。因此当BEV＝1时，实际上所有异常均在BIOS里面处理，当操作系统启动过程中清BEV以后，TLB重填、XTLB重填、Cache错误和其他通用异常才会转由内核进行处理，其余几种异常依旧在BIOS里进行

#### per_cpu_trap_init()进行每CPU配置, 顾名思义，每CPU配置就是每个逻辑CPU都需要执行一次的配置
* config_status()执行的结果是：
> 协处理器0可用, 其他协处理器不可用

> 32个双精度浮点寄存器可用

> 64位核心地址空间段、管理地址空间段、用户地址空间段均被启用

> 异常向量入口地址使用运行时向量（BEV＝0），因为异常向量表马上就要建立起来了

#### 建立异常向量表（set_handler函数开始到trap_init结束）
* 异常向量有两个层次
> 第一层次：向量0（TLB重填异常），向量1（XTLB重填异常），向量2（高速缓存错误），向量3（其他通用异常）；这一层次一共有4个向量，每个向量有不同的入口地址，由硬件负责分发；

> 第二层次：向量3实际上是一个共享入口，必须做一个分发；这个层次共32个向量，它们的入口地址相同但有不同的编号，由软件负责分发

### 2.2.5 重要函数： init_IRQ()
```c
// arch/mips/kernel/irq.c

init_IRQ()
	\-----arch_init_irq();
		\---mach_init_irq();
			|-----irqchip_init();
				|---mips_cpu_irq_init();
					\---__mips_cpu_irq_init();
				\---CaseA: ls2h_irq_of_init();
				    CaseB: ls7a_irq_of_init();
				    CaseC: i8259_of_init();
				    	\---__init_i8259_irqs();
			|-----loongson_pch->init_irq();
				\---CaseA: ls2h_init_irq();
				    CaseB: ls7a_init_irq();
				    CaseC: rs780_init_irq();
			|-----irq_set_chip_and_handler(LOONGSON_UART_IRQ, &loongson_irq_chip, handle_percpu_irq);
			|-----irq_set_chip_and_handler(LOONGSON_BRIDGE_IRQ, &loongson_irq_chip, handle_percpu_irq);
			\---set_c0_status(STATUSF_IP2 | STATUSF_IP3 | STATUSF_IP6);
```

#### 龙芯电脑的中断传递路径
![avatar](./pic/loogson_interrupt_transmit.jpeg)

* 从最核心的CPU核来看，所有的MIPS处理器在这个层面都有8个中断源（IP0～IP7）

* IP7固定作为时钟中断，其中断源为MIPS内部时钟

* CPU中断路由有32个输入（中断源），4个输出INT0～INT3（对应IP2～IP5）

* 32个输入中常用的分为5大类：
> 来自于CPU内部LPC中断（如CPU内置串口和其他ISA/LPC设备）

> 来自于CPU内部PCI中断（目前的PCI/PCIE设备均连接在桥片上的PCIE控制器上，内部PCI控制器未使用）

> 来自于HT0中断（HT0用于芯片互联）

> 来自于HT1中断（可连接外部中断控制器）

> 来自于SYS中断（可连接外部中断控制器）

* LS2H、LS7A芯片组使用APIC中断控制器

* RS780+SB700芯片组使用I8259中断控制器

* MSI中断：消息中断

* 龙芯总共定义了256个中断号（IRQ），LS2H、LS7A和RS780这3类机型的中断号分配如下
> LS2H芯片组：

|中断号|中断|说明|
|:-|:-|:-|
|IRQ0～15|APIC的LPC中断|低速设备外部中断，级联在IRQ77下面|
|IRQ16～55|MSI中断||
|IRQ56～63|MIPS经经典的CPU中断|包括内部时钟中断|
|IRQ64～159|APIC的常规中断|高速设备外部中断|
|IRQ160～255|MSI中断||

> LS7A芯片组：

|中断号|中断|说明|
|:-|:-|:-|
|IRQ0～15|APIC的LPC中断|低速设备外部中断，级联在IRQ83下面|
|IRQ16～55|MSI中断||
|IRQ56～63|MIPS经经典的CPU中断|包括内部时钟中断|
|IRQ64～127|APIC的常规中断|高速设备外部中断|
|IRQ128～255|MSI中断||

> RS780芯片组：

|中断号|中断|说明|
|:-|:-|:-|
|IRQ0～15|I8259中断|外部中断|
|IRQ16～55|MSI中断||
|IRQ56～63|MIPS经经典的CPU中断|包括内部时钟中断|
|IRQ64～255|MSI中断||

* 龙芯推荐的中断路由配置中，内部LPC中断（包括CPU串口）路由到0号核的IP2（即INT0）

* 中断路由配置时只允许0号核直接处理中断，这就有可能导致0号核的中断负担过重; 为了实现中断负载均衡，可以通过处理器间中断来完成外部中断的软件转发，因此需要一个IRQ->IPI_OFFSET的映射表

* IRQ相关的数据结构
```c
struct irq_desc { // 用来描述IRQ
	struct irq_data		irq_data;
	irq_flow_handler_t	handle_irq; /* 高层中断处理函数，它针对IRQ，与之对应的还有一个针对底层设备的irqaction.handler处理函数；高层中断处理函数可能会被设置成handle_percpu_irq()、handle_level_irq(), handle_edge_irq()*/
	struct irqaction	*action;    /* irqaction组成一个链表，当某个IRQ被触发，handle_irq()就会被调用，然后handle_irq()会遍历irqaction链表，逐个调用action里的handler()*/
	...
	const char		*name;
};

struct irq_data {
	unsigned int		irq;
	struct irq_chip		*chip;
	...
	struct irq_common_data	*common;
};

struct irq_chip {
	const char		*name;
	unsigned int	(*irq_startup)(struct irq_data *data);
	void 		(*irq_shutdown)(struct irq_data *data);
	void 		(*irq_enable)(struct irq_data *data);
	void 		(*irq_disable)(struct irq_data *data);
	void 		(*irq_ack)(struct irq_data *data);
	void 		(*irq_mask)(struct irq_data *data);
	void 		(*irq_unmask)(struct irq_data *data);
	void 		(*irq_eoi)(struct irq_data *data);
	int		(*irq_set_affinity)(struct irq_data *data, const struct cpumask *dest, bool force);
	...
};

struct irq_common_data {
	cpumask_var_t		affinity;
	...

};

struct irqaction {
	irq_handler_t		handler; /* 底层中断处理函数，它针对具体的设备；它被称为中断服务例程ISR（Interrupt Service Rountine）；底层中断处理函数需要到具体注册中断时才能设置 */
	struct irqaction	*next;
	struct task_struct	*thread;
	irq_handler_t		thread_fn;
	...
};
```

### 2.2.6 重要函数：time_init()
> 该函数用于计时系统初始化，它与体系结构相关

```c
time_init()
	|-----plat_time_init();
		\---setup_hpet_timer(); 	// 建立HPET的ClockEvent
	|-----mips_clockevent_init();
		\---r4k_clockevent_init();	// 建立MIPS的ClockEvent
	\-----init_mips_clocksource();
		\---init_r4k_clocksource();	// 建立MIPS的ClockSource

/* 问题
 * 1. HPET的ClockSource为何不建立？
 *    答：ClockEvent是不能运行时切换的，只能在初始化时决定，因此这里HPET, MIPS的ClockEvent都进行了初始化;
 *        ClockSource是可以运行时切换的，因此这里只初始化了MIPS的，而HPET的ClockSource被延后了
 *
 * 2. MIPS时钟源和HPET时钟源都初始化了，在运行时选择哪一个？
 *    答：
 *        根据时钟源的评分值来决定选择哪一个时钟源。每个时钟源都有一个rating值，它表示时钟源的评分值，它的取值范围是1～500，分越高越会被选用。
 * 	  因为只有0号核能收到外部中断，那么也只有0号核能使用HPET时钟源当ClockEvent，其他核使用MIPS当ClockEvent;
 */
```

#### 背景知识
* TimeKeeping, 时间维护、或称为计时；计时最基本的功能是：获取当前时间（Time Of Day，TOD）；进程调度、软件定时、电源管理、性能采样统计、网络时间协议（NTP）都建立在TOD概念之上

* 计时功能的实现依赖硬件上的时钟源，硬件上的时钟源分成两类：周期性时钟源和无节拍时钟源
> 周期性时钟源，也叫固定节拍时钟源。这类硬件时钟源周期性地发射时钟中断，作为计时的主要依据; 周期的单位是HZ；SMP系统中，周期性时钟源由0号CPU维护

> 无节拍时钟源，它的硬件时钟源根据需要选择性发射时钟中断，以此作为计时的主要依据。SMP系统中，无节拍时钟源由各个CPU轮流维护

* ClockEvent时钟源
> 它是基于中断的时钟源

> 是本地的，每个逻辑CPU都有一个

* ClockSource时钟源 
> 它由CPU主动读取它的计数器值，是一般意义上的时钟源

> 它是全局的，所有逻辑CPU共用一个

* 内部时钟源
> 每个核都有一个，在每个核上都能产生中断

> 例如在龙芯CPU上，CP0的Count/Compare这一对寄存器就是一种内部时钟源（一般称为MIPS时钟源），Count寄存器是一个计数器，随着时间的流逝而增长，其增长频率与CPU主频成正比关系；当Count寄存器的值增长到与Compare寄存器的值相同时，就产生一次时钟中断

> MIPS内部时钟源可以作为ClockEvent, 或ClockSource；不过因为ClockSource是全局的，所以在多核环境下需要保证每个核的Count寄存器是同步的

* 外部时钟源
> 在CPU外部，如：PIT（可编程间隔定时器）、HPET（高精度事件定时器）

> 外部时钟源的中断不一定能路由到任意一个CPU核

> 外部时钟源的中断与CPU频率无关

> 可作为ClockEvent或ClockSource，不过因为ClockEvent基于中断，所以如果在中断路由上有所限制，那么只有在单核时才能作为ClockEvent

#### 计时的基本方式
* ClockEvent时钟源每隔固定长度时间给系统发送一个时钟中断，每次中断称为一个节拍（tick），记录节拍的总数就可以得到粗粒度的当前时间x（毫秒级）；通过亚节拍修正，即，在x加上一个ClockSource计算得到的时间偏移量，就可以得到微秒级的当前时间

* 由于一些限制因素（CPU热恋插拔、动态调频、中断路由），单纯使用内部时钟或外部时钟源的计时方法很难做到精确，所以龙芯采用了混合时钟源计时方法

### 2.2.7 1号进程：kernel_init()
* 内核在sched_init()完成后化身为0号进程，在rest_init()中创建1号进程、2号进程，然后就化身为空闲进程，休息去了。后续的系统启动步骤，由1号进程接管，1号进程的执行函数是kernel_init()

```c
// init/main.c

kernel_init()
	|-----kernel_init_freeable();
		|-----smp_prepare_cpus(setup_max_cpus);
			|-----init_new_context(current, &init_mm);	// 初始化1号进程地址空间, 因为1号进程现在还是内核线程，所以它共享0号进程地址空间init_mm
			|-----current_thread_info()->cpu = 0;		// 现在投入运行的只有0号核
			|-----mp_ops->prepare_cpus(max_cpus);
				\---loongson3_prepare_cpus();		// 将cpu_present_mask设置成与cpu_possible_mask相同 
			|-----set_cpu_sibling_map(0);			// 设置0号CPU的线程映射图
			\---set_cpu_core_map(0);			// 设置0号逻辑CPU的核映射图
		|-----workqueue_init();			// 创建初始的工作者线程(kworker)
		|-----do_pre_smp_initcalls();		// 执行所有用early_initcall()定义的initcall函数
		|-----smp_init();		  	// 启动多核 
			|-----idle_threads_init();	// 给每个辅核创建0号进程
			|-----for_each_present_cpu(cpu) cpu_up(cpu);	// 循环把每个present状态的CPU都启动起来
				\---do_cpu_up(cpu);
					\---_cpu_up(cpu);
						|-----idle = idle_thread_get(cpu);
						\---cpuhp_up_callbacks(cpu, st, CPUHP_BRINGUP_CPU);
							\---bringup_cpu(cpu);
								\---__cpu_up(cpu, idle);
									|-----mp_ops->boot_seondary(cpu, tidle);
										\---loongson3_boot_secondary(cpu, tidle);
									\---synchronise_count_master(cpu);
			\---smp_cpus_done(setup_max_cpus);
		|-----sched_init(smp);
			|-----sched_init_numa();
			\---sched_init_domains(cpu_active_mask);
		\---do_basic_setup();
			|-----driver_init();
				|-----devtmpfs_init();
				|-----devices_init();
				|-----buses_init();
				|-----classes_init();
				|-----firmware_init();
				|-----hypervisor_init();
				|-----of_core_init();
				|-----platform_bus_init();
				|-----cpu_dev_init();
				|-----memory_dev_init();
				\---container_dev_init();
			\---do_initcalls();
				\---for (level = 0; level < 8; level++) do_initcall_level(level);
	|-----numa_default_policy();
	|-----CaseA: run_init_process(ramdisk_execute_command);
	|-----CaseB: run_init_process(execute_command);
	|-----CaseC: run_init_process("/sbin/init");
	|-----CaseD: run_init_process("/etc/init");
	|-----CaseE: run_init_process("/bin/init");
	|-----CaseF: run_init_process("/bin/sh");
```

> 1号进程的生命周期分成两个阶段：内核态阶段和用户态阶段

> 1号进程刚创建出来的时候是一个内核线程，它的工作是接管0号进程的内核启动过程；当这一阶段完成，它就装载用户态的init程序，变身为一个用户进程，成为所有其他用户态进程的鼻祖；

> 1号进程在内核态阶段主要执行kernel_init_freeable()

> 到现在为止，整个系统依然只有一个CPU在运行，但1号进程会负责把所有逻辑CPU都启动

#### 单处理器阶段
* kernel_init_freeable()的第一步：准备工作
* kernel_init_freeable()的第二步：smp_prepare_cpus() 

* kernel_init_freeable()的第三步：workqueue_init()
* kernel_init_freeable()的第四步：do_pre_smp_initcalls()
> 该函数用于执行所有用early_initcall()定义的initcall函数 

> Linux内核初始化段一览表 

|段名|标记方法|功能描述|
|:-|:-|:-|
|.init.text|__init|初始化代码|
|.init.data|__initdata|初始化数据|
|.init.setup|early_param/__setup/__setup_param|启动参数|
|.initcallearly.init|early_initcall|早期initcall函数|
|.initcall0.init|pure_initcall|第0级initcall函数|
|.initcall1.init/.initcall1s.init|core_initcall/core_initcall_sync|第1级initcall函数|
|.initcall2.init/.initcall2s.init|postcore_initcall/postcore_initcall_sync|第2级initcall函数|
|.initcall3.init/.initcall3s.init|arch_initcall/arch_initcall_sync|第3级initcall函数|
|.initcall4.init/.initcall4s.init|subsys_initcall/subsys_initcall_sync|第4级initcall函数|
|.initcall5.init/.initcall5s.init|fs_initcall/fs_initcall_sync|第5级initcall函数|
|.initcallrootfs.init|rootfs_initcall|Rootfs级initcall函数|
|.initcall6.init/.initcall6s.init|device_initcall/device_initcall_sync|第6级initcall函数|
|.initcall7.init/.initcall7s.init|late_initcall/late_initcall_sync|第7级initcall函数|

> 这些初始化段按表中的顺序放在相邻的内存中，所有初始化段的起始地址是__init_begin, 结束地址是__init_end;启动参数段的起始地址是__setup_start, 结束地址是__setup_end；所有initcall函数的起始地址是__initcall_start, 结束地址是__initcall_end 

> rootfs_initcall用于建立初始化内存盘（initrd/initramfs）中的根文件系统

> module_init()定义的函数分两个应用场景：此类函数被直接编译到内核，那么它等价于device_initcall；如果此类函数被编译到模块里，则在内核启动完成后，由1号进程间接调用

* kernel_init_freeable()的第五步：smp_init()
> 龙芯的主核是通过Mailbox寄存器来启动辅核的。自BIOS或Bootloader把控制权交给内核以后，主核一始执行内核代码，而辅核一直在轮询各自的Mailbox寄存器。loongson3_boot_secondary()通过Mailbox寄存器来传递辅核启动需要的信息。启动信息包括：入口地址，初始的SP和GP。辅核的入口地址是smp_bootstrap;SP和GP的计算方法跟主核一样，都是用0号进程的thread_info算出来的，只不过主核自己算，而辅核是主核算好然后直接通过Mailbox传递给辅核。

> 龙芯的每个核有4个Mailbox寄存器，Mailbox0用于传递入口地址，Mailbox1用于传递SP，Mailbox2用于传递GP（辅核自己的0号进程的thread_info），Mailbox3暂时未用

* kernel_init_freeable()的第六步：sched_init_smp()
> sched_init_numa()，根据NUMA结构建立节点间的拓扑信息

> sched_init_domains()，建立节点内的拓扑信息

> 到这里为止，所有的辅核都已经启动，kernel_init_freeable正式进入多处理器并行状态

#### 多核处理器阶段
* do_basic_setup->driver_init(), 早期Linux使用devfs来创建/dev目录下的设备结节，但从Linux-2.6.13版本开始采用udev后，从此绝大部分设备节点都是在运行时候动态创建的。但是有一些基本设备节点非常重要，必须在内核启动的过程中创建，比如/dev/console, /dev/zero, /dev/null等

* do_basic_setup->do_initcalls(), 用于处理Early后面的8个级别的initcall；对应每个级别会调用do_initcall_level->initcall
> 重要的initcall函数

|函数名|级别|功能描述|
|:-|:-|:-|
|loongson_cu2_setup()|early_initcall|注册协处理器2的异常处理函数|
|cpu_stop_init()|early_initcall|创建停机迁移内核线程（migration）|
|spawn_ksoftirqd()|early_initcall|创建软中断内核线程（ksoftirqd）|
|init_hpet_clocksource()|core_initcall|注册HPEI的ClockSource|
|pm_init()|core_initcall|电源管理初始化|
|cpuidle_init()|core_initcall|CPUIdle核心初始化|
|cpufreq_core_init()|core_initcall|CPUFreq核心初始化|
|init_per_zone_wmark_min()|core_initcall|计算每个内存管理区的水位线和保留量|
|isa_bus_init()|postcore_initcall|ISA总线驱动初始化|
|pci_driver_init()|postcore_initcall|PCI总线驱动初始化|
|loongson3_platform_init()|arch_initcall|执行龙芯3号的platform_controller_hub中的pch_arch_initcall()函数指针|
|loongson3_clock_init()|arch_initcall|初始化龙芯3号CPU的频率表|
|loongson_cpufreq_init()|arch_initcall|注册龙芯2号和龙芯3号的CPUFreq设备|
|loongson_pm_init()|arch_initcall|注册龙芯2号和龙芯3号的平台睡眠操作函数集|
|pcibios_init()|arch_initcall|注册PCI控制器|
|pcibios_init()|subsys_initcall|枚举所有PCI设备|
|alsa_sound_init()|subsys_initcall|声卡驱动核心初始化|
|md_init()|subsys_initcall|MD（RAID与LVM）初始化|
|usb_init()|subsys_initcall|USB驱动核心初始化|
|init_pipe_fs()|fs_initcall|PIPE（管道）文件系统初始化|
|inet_init()|fs_initcall|因特网协议族初始化|
|cfg80211_init()|fs_initcall|CFG80211（WIFI）核心初始化|
|populate_rootfs()|rootfs_initcall|建立initrd/initramfs中的根文件系统|
|loongson3_device_init()|device_initcall|执行龙芯3号的platform_controller_hub中的pch_device_initcall()函数指针|
|pty_init()|device_initcall|伪终端设备（PTY）驱动初始化|
|kswapd_init()|module_init|创建kswapd内核线程|
|cpufreq_init()|module_init|龙芯CPU的CPUFreq驱动初始化|
|loongson_hwmon_init()|module_init|龙芯CPU温度传感器驱动初始化|
|lemote3a_laptop_init()|module_init|Leote（龙梦）笔记本电脑平台驱动初始化|
|ahci_init()|module_init|AHCI（SATA控制器）驱动初始化|
|ohci_hcd_mod_init()|module_init|注册USB1.0主机驱动（OHCI）|
|ehci_hcd_init()|module_init|注册USB2.0主机驱动（EHCI）|
|xhi_hcd_init()|module_init|注册USB3.0主机驱动（EHCI）|
|usb_stor_init()|module_init|USB存储设备（U盘）驱动初始化|
|drm_core_init()|module_init|DRM（显卡驱动）核心初始化|
|radeon_init()|module_init|AMD Radeon显卡驱动初始化|

> 当各个initcall函数执行完毕，1号进程的内核态阶段也即将结束。接下来它将先通过numa_default_policy()将1号进程自己的NUMA内存分配策略改成MPOL_DEFAULT；然后通过run_init_process()->do_execve()来装入用户态的init程序，变身为普通进程。一旦用户态的init程序装入并开始执行，内核的启动过程就结束了

## 2.3  内核启动过程：辅核视角
* 辅核主要完成本地CPU相关的初始化，之后执行自己的0号进程进入idle循环，之后辅核就能进行任务调度了
* 辅核的启动也包括第一入口和第二入口

### 2.3.1 第一入口：smp_bootstrap
* 主核的入口地址是BIOS或BootLoader给出的
* 辅核的入口则是主核在执行smp_init()时传递的
```c
// arch/mips/kernel/head.S

NESTED(smp_bootstrap, 16, sp)
	smp_slave_setup			// 类似主核的kernel_entry_setup，对于龙芯来说完全一样
	setup_c0_status_sec		// 类似于主核的setup_c0_status_pri，只是增加了对BEV的清0，因为主核里已经建立好异常向量表，辅核直接用即可
	j	start_secondary		// 跳转到第二入口
	END(smp_bootstrap)
```

### 2.3.2 第二入口：start_secondary()
* 功能上类似主核的start_kernel()
```c
// arch/mips/kernel/smp.c

start_secodary()
	|-----cpu_probe();
	|-----per_cpu_trap_init(false);		// 辅核这里传的参数是false，主核是true, 这个值的不同会导致在per_cpu_trap_init中是否执行cpu_cache_init
		|-----configure_status();
		|-----configure_hwrena();
		|-----configure_exception_vector();
		|-----if (!is_boot_cpu) cpu_cache_init();
		|-----tlb_init();
			\---build_tlb_refill_handler();
		\---TLBmiSs_HANDlER_SETUP();
			\---TLBMISS_HANDLER_SETUP_PGD(swapper_pg_dir);
	|-----mips_clockevent_init();		// ClockEvent不是全局的，所以每个核都需要注册 
	|-----mp_ops->init_secondary();
		\---loongson3_init_secondary();	// SMP启动中mp_ops的第一个辅核步骤
	|-----calibrate_delay();		// 计算loops_per_jiffy（它的用途是确定delay()的时候每个节拍需要多少空操作指令）
	|-----notify_cpu_starting(cpu);
	|-----synchronise_count_slave(cpu);	// 与主核进行Count寄存器同步
	|-----set_cpu_online(cpu, true);	// 辅核把自己设置成online
	|-----set_cpu_sibling_map(cpu);		// 设置线程映射图
	|-----set_cpu_core_map(cpu);		// 设置核映射图
	|-----mp_ops->smp_finish();
		\---loongson3_smp_finish();	// SMP启动中mp_ops的第二辅核个步骤
	\---cpu_startup_entry(CPUHP_AP_ONLINE_IDLE);	// 辅核开始执行0号进程 
		\---while (1) do_idle();
			\---cpuidle_idle_call();
				\---default_idle_call();
					\---arch_cpu_idle();
						\---cpu_wait();
```

> SMP启动中mp_ops的第一个辅核步骤mp_ops->init_secondary()->loongson3_init_secondary()首先设置Status寄存器的IP位；然后把所有possible核的IPI_Enable寄存器的所有位置1，这意味着允许任意核上任意类型的IPI中断。接下来，计算辅核自己的封装编号与核编号。最后Count寄存器同步

> SMP启动中mp_ops的第二个辅核步骤mp_ops->smp_finish()->loongson3_smp_finish()

```c
static void loongson3_smp_finish(void)
{
	int cpu = smp_processor_id();
	write_c0_compare(read_c0_count() + mips_hpt_frequency / HZ);	// 在1个节拍后产生一次中断；mip_hpt_frequency是Count寄存器的增长频率，通常是CPU主频的一半
	local_irq_enable();		// 开中断，只有开了中断才有可能处理时钟
	loongson3_ipi_write64(0, (void*)(ipi_mailbox_buf[cpu_logical_map(cpu)] + 0x0)); 	/* 清空mailbox寄存器 */
}
```

# 第3章 异常与中断解析
* 异常/中断指的是出现了正常执行流程被打断的事件。这些个事件如果来自CPU内部，就叫异常；如果来自CPU外部，就叫中断。一般来说，异常是同步的，需要立即处理。而中断是异步的，通常会在一定程度上延迟处理
> 在MIPS处理器的分类中，中断被算作异常的一种。

* 异常、中断有什么用？
> 中断是为了CPU和外设可以并行工作而引入的一种机制

> 有了中断，CPU就可以一心一意地作自己的工作而不必轮询设备状态，直到中断发生再去为设备服务

> 正常情况下CPU是顺序执行，而异常可以让CPU交错执行，有了交错执行，CPU就拥有了并发特征，CPU有了并发特征，单核CPU上才可以运行多任务的可能

* 在Linux内核里，上下文用于描述当前的软硬件状态和环境
> 上下文分为进程上下文、中断上下文

> 中断上下文又细分为：不可屏蔽中断（NMI）上下文、硬中断上下文、软中断上下文

> 进程上下文细分为：可抢占进程上下文、不可抢占进程上下文

> CPU在处理不可屏蔽中断时，处于不可屏蔽中断上下文；CPU在处理硬件中断时，处于硬件中断上下文；CPU在处理软中断时，处于软中断上下文；其他时候（包括用户进程、内核线程、异常处理）CPU处于进程上下文

* 各个上下文的特点
> 硬中断上下文：不可被硬中断、不可被软中断、不可睡眠、不可抢占

> 软中断上下文：可以被硬中断，不可被软中断、不可睡眠、不可抢占

> 进程上下文：可以被硬中断、可以被软中断、可以睡眠，是否可以被抢占取决于有没有关抢占

* 内核提供以下宏来判断当前上下文状态
> in_irq()，是否处于硬中断上下文

> in_softirq()，是否处于软中断上下文

> in_nmi()，是否处于不可屏蔽中断上下文

> in_interrupt()，是否处于硬中断、软中断、nmi等中断上下文

> in_atiomic()，是否处于原子上下文

> in_task()，是否处于进程上下文

## 3.1 寄存器操作
* 进程在用户态运行时使用用户栈，进入内核态以后使用内核栈

* 从用户态切换到内核态时，需要将除K0/K1外的所有寄存器保存到内核栈

* 在保存寄存器上下文前，SP指向的是用户栈，因此首要的事情是切换SP
> 将旧SP值（用户栈指针）保存到K0

> 首先使用get_saved_sp来获取当前进程在当前CPU上的内核栈指针并放入K1

> 将K1的值减去PT_SIZE（完整寄存器上下文大小）并写入SP，此时SP已经指向内核栈

* 内核态切换回用户态时，只需要恢复寄存器上下文中的SP就可以（？）

* 异常处理时发生的上下文切换只涉及一个进程，是同一个进程在“正常执行流”与“异常执行流”之间的切换

## 3.2 异常处理解析
### 3.2.1 复位异常和NMI
* 复位包括硬复位和软复位
> 硬复位异常在冷启动时发生，也就是说硬复位就是第一次开机上电

> 软复位异常在热启动时发生，也就是说在开机状态下执行重启操作将触发软复位

* NMI表示发生了非常严重的错误，通过CPU上专门的NMI引脚触发，因此不可屏蔽
> NMI直接跳到内核进行处理。内核的NMI处理入口是 except_vec_nmi()，定义在arch/mips/kernel/genex.S中, 最终调用nmi_exception_handler()执行关机或重启

* 复位异常和NMI有相同的入口地址0xffff ffff bfc0 0000
> 它们都由BIOS第一时间处理

* BIOS如何区分复位异常和NMI异常？
> 通过协处理器0中的Status状态寄存器，这3种异常发生时，硬件会根据情况设置Status寄存器的BEV、SR、NMI位
```
1. 硬复位：BEV=1, SR=0, NMI=0
2. 软复位：BEV=1, SR=1, NMI=0
3. NMI：BEV=1, SR=0, NMI=1
```

### 3.2.2 缓存错误异常
* Cache错误一旦发生，意味着Cache已经不可靠，接下来所有的取指令和内存访问都必须以非缓存的方式进行
> 内核本身存放的段可以被设置成以非缓存方式工作，但因为有动态生成的代码和数据不在内核所在的段上，所以对于Cache错误，实际上软件是无能为力。因此出现此种异常最终会调用panic()进入死机状态

> 龙芯3号处理器对Cache有纠错功能，但有些错误比较严重也会出现无法纠错的情况，那同样会死机

> 死机是种策略（由内核主动调用panic()函数实现），由上面对Cache错误的分析可知

### 3.2.3 TLB/XTLB异常
* CPU的内存管理单元叫MMU，而MMU包括TLB。TLB负责从虚拟地址到物理地址的转换，是内存中页表的一个子集
* 虚拟页和物理页的页内偏移相等，所以转换虚拟地址到物理地址只需要转换虚拟页号和物理页号
* 虚拟页号
> 多级页表可将虚拟页号拆分成PGD, PUD, PMD, PTE，这是四级页表

* 物理页号

* 64位MIPS处理器大多使用三级页表(PGD, PMD, PTE)
* 龙芯的PTE
```
--------------------------------------------------------------------------------------------
|   ___________________________________     ___________________________________________    |
|  |__VPN2__|__ASID__|__PageMask_|__G__|   |__PFN-0__|__Flags-0__|__PFN-1__|__Flags-1__|   |
|                                                                                          |
--------------------------------------------------------------------------------------------

/* VPN2，地址相邻的两个页面被放置在同一个页表项里，虚拟页号的高位部分叫VPN2，一对虚拟页面自然对应两个物理页面，即一对虚拟地址对应两个物理地址。因此一个VPN2对应PFN-0和PFN-1两个物理页号。
 * 
 * 物理页号是全局唯一的
 * 虚拟页号不是全局唯一的，因为每个进程都有自己的页表，为了防止虚拟地址冲突，TLB表项里加入了ASID来让VPN2+ASID保证唯一性，ASID约等于进程ID
 * 
 * G是全局标志位，G=1表示不需要匹配ASID。内核自己的页表一般会设置G=1
 *
 * PageMask是页面大小
 * Flags-0/1是页面属性标志，C是Cache属性，D表示可写位（D为1表示页面可写），V是有效位（V为1表示该TLB项有效）
 *
 * 龙芯的每个CPU核都有64项全相联的VTLB（V代表页面大小可变），全相联代表任意一个虚拟地址要以通过TLB中的任意一项来映射
 */
```

* 当TLB里面没有合适的条目可以翻译当前虚拟地址的时候，就会发生TLB异常
* TLB异常总共分4种：
> TLB/XTLB重填异常，意味着TLB中没有对应项

> TLB加载无效异常，表示读请求，TLB中有对应项，但对应项无效

> TLB存储无效异常，表示写请求，TLB中有对应项，但对应项无效

> TLB修改异常，写请求，TLB中有对应项，对应项有效，但只读

> 注：TLB/XTLB有专门的入口向量（向量01），其他几种异常使用通用入口向量（向量3）

> TLB重填异常用于32位模式，XTLB用于64位模式

#### 内核微汇编器原理
* TLB异常的处理函数不是静态编写的，而是在启动过程中用内核自带的微汇编器生成的动态代码 
* 龙芯TLB异常处理函数是靠什么生成的？如下这四个函数
> TLB/XTLB重填异常，build_r4000_tlb_refill_handler()

> TLB加载无效异常，build_r4000_tlb_load_handler()

> TLB存储无效异常，build_r4000_tlb_store_handler()

> TLB修改异常，build_r4000_tlb_modify_handler()

#### 龙芯TLB动态代码
* 以龙芯3号为例讲的龙芯TLB动态代码
> 已知的配置：龙芯3号可寻址范围是48位；

> 龙芯CPU虚拟地址的结构，即龙芯内核代码中的三级页表PGD只有47位，所以PGD地址范围小于CPU寻址能力, 所以需要其他代码来处理超出PGD范围的地址，这些地址被龙芯内核定义为非法地址，这些非法地址的处理会通过tlb_do_page_fault_0转移到do_page_fault()函数，最终结果是给应用程序发送“段错误”信号，终止其运行

* 虚拟地址分解
```
          -----------------------------------------------------------------
          |保留（17位）|PGD（11位）|PMD（11位）|PTE（11位）|Offset（14位）|  
	  -----------------------------------------------------------------
	               |           |           |           |<-PAGE_SHIFT->|
		       |           |           |<---PMD_SHIFT------------>|
		       |           |<---------PGD_SHIFT------------------>|
		       |<-------------整个PGDIR覆盖的地址范围------------>|
```

* 除TLB重填异常外，所有的TLB异常处理都分为快速路径、慢速路径
> 快速路径下，与给的虚拟地址对应的TLB项要么无效，要么有效无权限，但对应的页表项存在且有效有权限；因此，异常处理函数会访问相应的页表项并将其填到TLB中

> 慢速路径下，页表里面也不存在有效对应项（或者有效无权），因些需要通过tlb_do_page_fault_0（读操作触发的异常）或tlb_do_page_fault_1（写操作触发的异常）标号处的代码调用do_page_fault()进行缺页异常处理，建立有效页表项（或者处理权限问题）后再进行TLB装填

* 优化TLB重填异常
> 一个是在硬件层面上优化，一个是在软件层面上优化，这里我只关心软件层面的，软件上是如何优化的呢？高度精简。

> 软件精简举例，TLB重填异常处理只有快速路径，没有慢速路径，这并不是表示出现TLB重填异常时一定存在有效的页表项且权限正确，而是在发生TLB重填异常就把页表项装到TLB，不管页表项是有效是否有权限。如果有效有权则后面的执行一切正常；如果填到TLB的页表项是无效的，那么触发TLB重填异常的指令将再次触发TLB无效异常，在新的异常里面再进入慢速路径来建立有效页表项；如果填到TLB的页表项是无权限的，那么触发重填异常的指令会再次触发TLB修改异常，在新的异常里再进入慢速路径来处理权限问题

### 3.2.4 其他通用异常
* 通用异常有统一的入口except_vec3_generic，定义在arch/mips/kernel/genex.S中
> CAUSE寄存器的第2～6位标识了异常种类（异常编码）

> except_vec3_generic中先取出异常编码，以其为索引在exception_handler[]数组中找到正确的处理函数，然后跳转到处理函数

> exception_handler数组就是第二层次异常向量

* MIPS第二层次异常向量表

|异常编码|助记符|异常名称|处理函数|备注|由微汇编器动态生成|
|:-|:-|:-|:-|:-|:-|
|0|Int|中断|handle_init()|龙芯3A2000开始使用rollback_handle_init()|no, 单独定义|
|1|Mod|TLB修改异常|handle_tlbm()||yes|
|2|TLBL|TLB无效异常（读）|handle_tlbl()||yes|
|3|TLBS|TLB无效异常（写）|handle_tlbs()||yes|
|4|AdEL|地址错误异常（读）|handle_adel()|||
|5|AdES|地址错误异常（写）|handle_ades()|||
|6|IBE|总线错误异常（指令）|handle_ibe()|||
|7|DBE|总线错误异常（数据）|handle_dbe()|||
|8|Sys|系统调用异常|handle_sys()||no, 单独定义|
|9|Bp|断点异常|handle_bp()|||
|10|RI|保留指令异常|handle_ri()|龙芯3号系列使用handle_ri_rdhwr_tlbp()||
|11|CpU|协处理器不可用异常|handle_cpu()|||
|12|Ov|算术溢出异常|handle_ov()|||
|13|Tr|陷阱异常|handle_tr()|||
|14|MSAFPE|MSA向量浮点异常|handle_msa_fpe()|龙芯3A4000新增||
|15|FPE|浮点异常|handle_fpe()|||
|16|FTLB|FTLB异常|handle_ftlb()|龙芯3A2000新增||
|19|TLBRI|TLB抗读异常|tlb_do_page_fault_0|龙芯3A2000新增||
|20|TLBXI|TLB抗执行异常|tlb_do_page_fault_0|龙芯3A2000新增||
|21|MSA|MSA向量模块异常|handle_msa()|龙芯3A4000新增||
|22|MDMX|MDMX向量模块异常|handle_mdmx()|龙芯不支持||
|23|WATCH|观察点异常|handle_watch()|龙芯不支持||
|24|MCheck|机器检查异常|handle_mcheck()|龙芯不支持||
|25|Thread|SMT线程异常|handle_mt()|龙芯不支持||
|26|DSP|DSP模块异常|handle_dsp()|龙芯3A2000新增||
|27|GE|客户机退出异常|handle_guest_exit()|龙芯3A4000新增||
|30|CacheError|高速缓存错误异常|不属于通用异常|||

* 以系统调用异常为例说明
> Glibc中的INLINE_SYSCALL_CALL宏会根据系统调用参数个数展开成__INLINE_SYSCALL0()~__INLINE_SYSCALL7()中的一个，之后再进一步展开成INLINE_SYSCALL()宏，直至INTERNAL_SYSCALL宏，再展开成internal_syscall0()~internal_syscall7()中的一个

> 系统调用的总入口是handle_sys()

> 为了处理整数的符号扩展问题，从Linux-2.6.29引入了“系统调用包装器”并从Linux-3.10版全面启用。如今，系统调用函数使用专门的包装器宏来定义

```c
#define SYSCALL_DEFINE0(name)	\
	SYSCALL_METADATA(_##name, 0);	\
	asmlinkage long sys_##sname(void)

// 使用包装器的系统调用sys_fork()定义如下
SYSCALL_DEFINE0(fork);

// 实际结果相当于如下定义
asmlinkage unsigned long sys_fork(void);
```
## 3.3 中断处理解析
























