# 第1章 处理器体系结构
* ARMv7指令集的处理器系列以Cortex命名，又细分成A、R和M系列
> A系列，针对大型嵌入式系统（如手机）

> R系列，针对实时性系统

> M系列，针对单片机

> 地址空间是32位的，最高支持4G虚拟内存

* ARMv8指令集支持64位指令集

## 1、请简述精简指令集RISC和复杂指令集CISC区别
* 指令集是CPU上的大量晶体管实现的
* RISC
> 编码固定长度

> 无条件码

> 指令个数较少

* CISC
> 编码长度是变长的，如，x86-64上指令长度为1～15字节

> 有条件码

> 指令较多，如，Intel的指令手册有1200多页用于说明指令

## 2、请简述数值0x12345678在大端序处理器的存储器中的存储方式
* 大端序、小端序指的是字节顺序
* 0x12345678是一个16进制数，两位16进制是一字节
> 大端序: 0x12 34 56 78

> 小端序: 0x78 56 34 12

* Cortex-A系列的处理器可以通过软件来配置大小端模式

* 字节序用在哪里呢？是CPU访问内存时，处理寄存器与内存字节顺序时使用的
> 即，寄存器中的值0x1245678，如果是大端序，则在虚拟内存中，其值为0x12345678，如果是小端序，则在虚拟内存中，其值为0x78563412

## 3、请简述在你所熟悉的处理器（比如双核Cortex-A9）中一条存储(器？)读写指令的执行全过程
* 流水线
* 过程概述如下
> 指令首先按顺序进入流水线前端（预取、译码），并采用乱序方式进行发射，然后乱序执行，最后按顺序提交结果，并将最终结果更新到LSQ（Load-Store Queue）

* LSQ是存储子系统的最高层，它的功能是接收来自CPU的存储器指令，并把这些指令发送到存储器子系统，之后处理存储器子系统返回的数据和消息

* 多处理器时，需要保证cache一致性, ARM cortex A9采用MESI协议来保证cache一致性 
> Cortex-A9内置一级缓存，外接二级缓存，这两级缓存的一致性由MESI协议来保证

* 寄存器重命名
> 它指的是，当一条指令写一个结果寄存器时，不直接写到这个结果寄存器，而是先写到一个中间寄存器过渡，当这条指令提交时再写到结果寄存器中

## 4、请简述内存屏障（memory barrier）产生原因
* 内存屏障是用来阻止内存乱序访问的
* 有两种内存乱序访问
> 编译器优化导致的内存乱序访问

> 运行时，多CPU之间交互引起的内存乱序访问

* 编译器导致的乱序访问可以使用内存屏障来避免
```c
#define barrier() __asm__ __volatile__ ("" ::: "memory")

// barrier 告诉编译器，不要因为性能优化将这些代码重排
```

* ARM的Cortex-A系列处理器实现弱一致性内存模型

* 弱一致性内存模型：
> 对同步变量的访问是顺序一致的

> 在所有之前的写操作完成之前，不能访问同步变量

> 在所有之前同步变量的访问完成之前，不能访问（读或写）数据

## 5、ARM有几条memory barrier指令？分别有什么区别？
* 从ARMv7指令集开始，ARM提供３条内存屏障指令:
> 数据存储屏障（DMB, Data Memory Barrier）

> 数据同步屏障（DSB, Data synchornization Barrier）

> 指令同步屏障（ISB, Instruction synchronization Barrier）

* DMB，当位于此指令前的所有内存访问均完成时，DMB指令才会完成
> 仅当所有在它前面的存储器访问操作都执行完毕后，才提交在它后面的存取访问操作指令

* DSB，任何指令都要等待DSB前面的存储访问完成
> 比DMB更严格一些，仅当所有在它前面的存储访问操作指令都执行完毕后，才执行它后面的指令

* ISB，最严格，它通常用来保证上下文切换
> 冲洗流水线和预取后，才会从cache或内存中预取ISB之后的指令（这是书中原话，感觉TMD的像是FP）

## 6、请简述cache的工作方式
## 7、cache的映射方式有全关联、直接映射、组相联３种方式，它们的区别是？为什么现代的处理器都使用组相联的cache映射方式？
* 直接映射
> 一组一行

> 会有冲突不命中问题，即cache颠簸

* 组相联
> 多组，每组有多行

* 全关联
> 只有一组，所有行在一组中

## 8、在一个32KB的4路组相联的cache中，其中cache line为32bytes，请画出这个cache的cache line, way和set示意图
* 首先计算出组的个数，每组有4个cache line，那么一个组的大小是4 * 32 = 2^7, 总大小是32KB=2^15，所以组的个数是2^15 / 2^7 = 2^8 = 256
> 这样需要８位用于组索引

> cache line大小是32字节，所以需要32=2^5，5位来定位块偏移

> tag 占32 - 8 - 5 = 19位

## 9、ARM9处理器的Data cache组织方式使用的是VIVT，即虚拟index虚拟tag, 而在Cortex-A7处理器中使用的是PIPT，即物理Index物理tag，PIPT比VIVI好在哪？
* cache可以设计成通过虚拟地址或物理地址来访问，这是在CPU设计时决定的

* cache分为三类，vivt, vipt, pipt
> vivt, 这是早期arm处理器采用的方式，它的特点是不用MMU翻译，直接使用虚拟地址来查找cache line，这种方式会导致高速缓存别名问题

> ARM11系列采用VIPT方式，这种方式也可能导致高缓别名问题

> ARM Cortex-A系列处理器的数据cache采用PIPT，它不会产生高速缓存别名问题

## 10、请画出在二级页表架构中虚拟地址到物理地址查询页表过程
### ARMv7-A架构处理器的二级页表
* 根据页的大小有四种情况：超级大段（16MB），段（1MB），大页面（64KB），页面（4KB），前两个只有一级页表，后面两个需要二级页表

* 以4KB页面大小为例说明二级页表查询过程
> 虚拟地址是32位的，这32位是怎么编码的呢？首先因页大小是4KB，所以需要有12位来定位页内的偏移，接下来这个页表的两级各有多少项，一级页表有4096项需要12位，那二级页表不用问一定是32 - 12 - 12 = 8位，即256项，这样目前为止已经明确虚拟地址32位各位的分布如下: 0～11 是页内字节偏移，12～19是二级页表索引，20～31是一级页表索引

> 一级页表的基址在mm_struct的pgd成员变量中

> 页表输入的是VPN，输出的是PPN，由PPN + 虚拟地址的后12位计算得到物理地址

> 如果是TLB命中，即，TLB中保存有页表项PTE，而PTE中有要找的PPN，那么不用查页表，直接用TLB输出的PTE来得到PPN，从而得到物理地址

### ARMv8-A架构处理器的四级页表
* 以页面大小是4KB为例说明，与二级页表类似，其64位中使用了48位用作页表编码
> 0～11位，用于页内偏移

> 12～20位，是第4级页表索引

> 21～29位，是3级页表索引

> 30～38位，是2级页表索引

> 39～47位，是1级页表索引

> 一级页表基址保存在mm.pgd中，当然也在寄存器TTBRx中

## 11、在多核处理器中，cache的一致性是如何实现的？请简述MESI协议的含义
* 出现高速缓存一致性问题的原因是：在多核上的dcache和内存可能有同一上数据的多个副本
> 如果CPU只有一个核，那么是不存在高速缓存一致性问题的

* MESI协议，M（修改Modified）、E（独占Eclusive）S（共享Share）I（无效Invalid）

* MESI协议定义
|状态|描述|
|:-|:-|
|M|这行数据有效，数据被修改，和内存中的数据不一致，数据只存在本cache中|
|E|这行数据有效，数据和内存中数据一致，数据只存在于本cache中|
|S|这行数据有效，数据和内存中数据一致，多个cache有这个数据的副本|
|I|这行数据无效|

* MESI状态说明
|当前状态|操作|响应|迁移状态|
|:-|:-|:-|:-|
|M|总线读|Flush该cache line到内存，以便其他CPU可以访问到最新的内容，状态变成S|S|
|M|总线写|Flush该cache line到内存，然后其他CPU修改cache line，因此本cache line执行清空数据操作，状态变成I|I|
|M|处理器读|本地处理器读该cache line，状态不变|M|
|M|处理器写|本地处理器写该cache line，状态不变|M|
|E|总线读|独占状态的cache line是干净的，因此状态变成S|S|
|E|总线写|数据被修改，该cache line不能再使用了，状态变成I|I|
|E|本地读|从该cache line中取数据，状态不变|E|
|E|本地写|修改该cache line数据，状态变成M|M|
|S|总线读|状态不变|S|
|S|总线写|数据被修改，该cache line数据不能使用了，状态变成I|I|
|S|本地读|状态不变|S|
|S|本地写|修改了该cache line的数据，状态变成M；其他核上共享的cache line的状态变成I|M|
|I|总线读|状态不变|I|
|I|总线写|状态不变|I|
|I|本地读|1、如果cache miss, 则从内存中取数据，cache line变成E；2、如果其他cache有这份数据，且状态为M，则将数据更新到内存，本cache再从内存中取数据，两个cache的状态都变成s；3、如果其他cache有这份数据，且状态是S或E，本cache从内存中取数据，这些cache line都变成S|E/S|
|I|本地写|1、如果cache miss，从内存中取数据，在cache中修改，状态变成M；2、如果其他cache有这份数据，且状态为M，则要先将数据更新到内存，其他cache line状态变成I，然后修改本cache line的内容|M|

## 12、cache在Linux内核中有哪些应用？
* 1.内核中常用的数据结构通常是和L1 cache对齐的
> mm_struct使用SLAB_HWCACHE_ALIGN标志位来创建slab缓存，见proc_caches_init()

* 2.一些常用的数据结构在定义时就约定以L1 cache对齐，如: struct zone, struct irqaction, softirq_vec[], irq_stat[, struct worker_pool
> 使用宏___cacheline_internodeaigned_in_smp, ____cacheline_aligned_in_smp来定义上述数据结构, 内核源码下：include/linux/cache.h

* cache与内存交换数据的最小单位是cache line
> 如果一个结构没有和cache line对齐，那么结构有可能会占用多个cache line

> 举例：假设结构C1和C2，缓存到L1 cache时没有按照cache line对齐，因此它们有可能同时占用了一条cache line，根据cache一致性协议，假设cpu0修改结构体C1时会导致cpu1的cache line失效。两个CPU原本没有共享访问，因为要共同访问同一个cache line，产生了事实上的共享。解决这个问题的一个方法是让结构体C1和C2按照cache line对齐，以空间换时间。

* 3.数据结构中频繁访问的成员可以单独占用一个cache line，或者相关的成员在cache line中彼此错开，以提高访问效率
> 例如，zone->lock, zone->lru_lock这两个频繁被访问的锁，可以让它们各自使用不同的cache line, 以提高获取锁的效率

> 还有，struct worker_pool中的nr_running成员也独占一个cache line，避免多cpu同时读写该成员时引发其他临近的成员“颠簸”

* 4. slab日着色区

* 5. 自旋锁的实现

## 13、简述ARM big.LITTLE架构，包括总线连接和cache管理
* ARM提出大小核概念
> 大核：针对性能优化过的处理器内核

> 小核：针对功耗优化过的处理器内核

* CCI-400模块（缓存一致性控制器）：用于管理大小核架构中缓存一致性相关的模块
> 它只支持两个CPU簇，而CCI-500可支持6个CPU簇(cluster)

> 大小核集群中的cache一致性管理

> 实现CPU间数据共享

> 可连接GPU, MMU, CPU, 外设, DMC

> 外设需要支持ACE协议才可连接到CCI-400上

* GIC-400：中断控制器
> 与CPU直接连接

* MMU-400：内存管理单元　

* DMC-400: 内存控制器
> 连接内存设备，如LP-DDR2/3或DDR内存设备

* NIC-400: 用于AMBA总线协议的连接，可以支持AXI、AHB和APB总线的连接
> 如用于连接DMA, LCD设备

* ACE协议（AMBA AXI Coherency Extension），是AXI4协议的扩展协议，它增加了很多特性用于支持硬件一致性
> 模块之间共享内存不需要软件参与，硬件直接管理和维护各个cache之间的一致性

## 14、cache coherency和memory consistency有什么区别？
* cache coherency，关注的是同一个数据在多个cache和内存中的一致性问题
> 解决高速缓存一致性的方法主要是总线监听协议，例如MESI协议

* memory consistency, 关注的是处理器系统对多个地址进行存储器访问序列的正确性 
> 这需要对内存访问模型的知识，如严格一致性内存模型、处理器一致性内存模型、弱一致性内存模型

> 现在处理器中广泛应用的是弱一致性内存模型，而内存屏障指令即属于这一模型

## 15、简述cache的write back有哪些策略
* write back（回写模式），在进行写操作时，数据直接写入当前cache, 而不会继续传递，当该cache line被替换出去时，被改写的数据才会更新到下一级cache或主存储器中

* write through（直写模式），进行写操作时，数据同时写入当前的cache、下一级cache或主存储器中

## 16、简述cache line的替换策略
* cache不命中时，意味着处理器需要从主存储器中获取数据，而且还需要将cache的某个cache line替换出去

* cache 的替换策略有：随机法（Random Policy）、先进先出法（FIFO）、最近最少使用算法（LRU）
> LRU：根据各块使用情况，总是选择最近最少使用的块来替换，符合局部性原理

> FIFO：选择最先调入的那个cache block进行替换，最先调入的块可能被多次命中，但是被优先替换，因而不符合局部性原理

> 随机：随机确定替换的cache block，命中率较低

* Cortex-A57处理器中，L1 cache 采用的LRU算法，L2 cache 采用的是随机算法

* Cortex-A72处理器中，L2 cache采用LRU或伪随机算法

## 17、多进程间频繁切换对TLB有什么影响？现代的处理器是如何面对这个问题的？
* TLB用于缓存页表项，输入vpn, 输出pte，而pte中有ppn
> tlb可以缓存的pte项是有限的，如果系统页大小是4KB, 而一个程序需要512页，那么对应的就需要512个页表项来缓存这个程序，如果tlb可存的pte个数小于512，则tlb不命中的机率就会高，反之，如果页的大小是1MB，则只需要2个pte就可以了，这样tlb命中率就高

## 18、简述NUMA架构特点
* 首先要知道什么是UMA，之后再说NUMA
> 现在大多数ARM采用的是UMA的内存架构，内存是统一结构和统一寻址

* UMA内存架构的系统有以下特点
> 所有硬件资源都是共享的，每个处理器都能访问到系统中的内存和外设资源

> 所有处理器都是平等的

> 统一寻址访问内存

> 处理器和内存通过内部的一条总线连接在一起

* NUMA架构特点
> 每个CPU集群有本地内存结点，访问本地的内存更快，访问别的CPU集群的内存结点速度慢

* ARM在2016年Cavium公司发布的ARMv8-A指令集的服务器芯片ThunderX2开始支持NUMA

## 19、ARM从Cortex系列开始性能有了质的飞跃，比如Cortex-A8/A15/A53/A72，说说Cortex系列在芯片设计方面做了哪些重大改进？
* 2005年发布的Cortex-A8
> 引入超标量技术，单时钟周期可并行发射两条指令

> 静态调度流水线和顺序执行

* 2007年发布的Cortex-A9
> 引入乱序执行和猜测执行

> 扩大L2 Cache容量

* 2010年发布的Cortex-A15
> 主频到2.5GHz, 支持8核，单个簇最多4核，有1TB物理地址空间，支持虚拟化

> 引入Micro-Ops，微操作概念，与x86的uops指令想法类似

* 2012年发布的Cortex-A53,A57
> A57是首款支持64位的ARM处理器内核

* 2015年发布的Cortex-A72
> 5发射, 并行执行8条微操作指令

## 开源指令集
* OpenRISC
> linux内核中已经包含这个体系结构

* RISC-V
> 还没有被集成到linux内核

> 是由伯克利大学设计的

> 最基础的指令集只包含40条指令

> 通过扩展可以支持64位和128位运算以及变长指令

> 2016年RISC-V基金会成立（成员有华为、Google、HP、甲骨文、西部数据等）

> 已经进入GCC/Binutils主线

# 第2章 内存管理
## 内核有属于内核自己的页表；每个进程有属于进程自己的页表；
## 虚拟内存和物理内存采用页表来建立映射关系
## 本章以ARM Vexpress平台为例讲述
* 不支持NUMA

## 用户态内存管理相关API
```c
void *malloc(size_t size);
void free(void *ptr);

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);

int getpagesize(void);

int mprotect(const void *addr, size_t len, int prot);

int mlock(const void *addr, size_t len);
int munlock(const void *addr, size_t len);

int madvise(void *addr, size_t length, int advice);
void *mremap(void *old_address, size_t old_size, size_t new_size, int flags, ...);
int remap_file_pages(void *addr, size_t size, int prot, ssize_t pgoff, int flags);
```

## 2.1 物理内存初始化
### 2.1.1 内存管理概述
* 用户层　
> 通过libc库封装的C语言函数malloc, mmap等

* 内核层
> 使用系统调用实现用户层libc封装的C语言函数(sys_brk, sys_mmp, sys_madvise)

> VMA管理

> 缺页中断

> 匿名页面

> page cache

> 页面回收

> slab分配器　

> 页表管理

> 伙伴系统/页面分配器

* 硬件层
> MMU

> TLB

> cache

> 物理内存（板载的）

### 2.1.2 内存大小
* 物理内存大小　
> ARM Linux中，各种设备相关属性采用DTS（device tree source）方式来描述

> 2011年ARM Linux社区开始全面支持DTS

> ARV Vexpress平台中，内存的定义在arch/arm/boot/dts/vexpress-v2p-ca9.dts中定义

```
	memory@60000000 {                                                                                                                        
		device_type = "memory";
	        reg = <0x60000000 0x40000000>;
	};
// 内存的起始地址是0x60000000
// 内存的大小是0x40000000 = 1GB = 2^30
```

> 内核在启动过程中，需要解析这些DTS文件，代码调用关系如下:

```c
start_kernel()->
		setup_arch()->
				setup_machine_fdt()->
							early_init_dt_scan_nodes()->
											early_init_dt_scan_memory()
															->memblock_add()

// 书中写的是会调用函数memblock_add添加到memblock子系统中，所以函数early_init_dt_add_memory_arch是arch/arm64/kernel/setup.c中定义的，所以可知Vexpress平台是64位的系统

// early_init_dt_add_memory_arch是在early_init_dt_scan_memory中调用的
	
// drviers/of/fdt.c

int __init early_init_dt_scan_memory
```

* 虚拟内存大小　

### 2.1.3 物理内存映射
* 在内核使用内存前，需要初始化内核的页表
#### 书中给的内核代码调用路径是
```c
start_kernel()->
		setup_arch()->
				paging_init()->
						prepare_page_table()
						map_lowmem()
						bootmem_init()

// paging_init()中是先调用prepare_page_table()，之后调用map_lowmem(), 见arch/arm/mm/mmu.c中函数paging_init()实现

// 因书中给出的是上述调用路径，所以paging_init()函数是在arch/arm/mm/mmu.c中实现;而不是在arch/arm64/mm/mmu.c中，因为这个里面的paging_init没有调用prepage_page_table()和map_lowmem()，这就与上一节2.1.2中最后一句说的会调用memblock_add()函数是冲突的，因为它是在arm64中才会调memblock_add
// 上述内核版本是3.10.0
// 下载了4.4.24版本的内核源码，目前截止到书中39页，与书中源码一致
```

* kernel的代码段起始地址是_stext
> 在内核版本4.4.24中函数map_lowmem中有_stext直接引用，但内核4.11.10及以后版本4.20.9都使用的是KERNEL_START

> ARM Vexpress, _stext值为0x60000000

* kernel的代码终止地址是__init_end
> 对应ARM Vexpress，__init_end值为0x60800000

### 2.1.4 zone初始化
* 页表初始化完成后，内核就可以对内存进行管理了，内核采用分区的方式来管理这些页面
> zone会经常被访问，因此这个数据结构要求以L1 cache对齐　

> zone结构中的ZONE_PADDING()是让zone->lock和zone->lru_lock分布在不同的cache line中

> 一个内存结点只有几个zone实例，所以才会用ZONE_PADDING()用空间换时间

* zone一般分为ZONE_DMA, ZONE_DMA32, ZONE_NORMAL, ZONE_HIGHMEM

* ARM Vexpress只有ZONE_NORMAL, ZONE_HIGHME
> ZONE的类型定义在include/linux/mmzone.h中, struct zone也在mmzone.h中定义

* zone的初始化集中在bootmem_init()中完成，其也是在paging_init中被调用的
> zone具体的初始化函数是在free_area_init_core中, 其调用路径为
```c
start_kernel()->
		setup_arch()->
				paging_init()->
						bootmem_init()->
								zone_sizes_init()->
											free_area_init_node()->
														free_area_init_core()
```

* struct zonelist, 定义在mmzone.h中
```c
struct zone {
	...						// 省略很多
	struct free_area	free_area[MAX_ORDER]; 	// free_area结构中包含有几种迁移类型的页面, ARM里有六种迁移类型 
	...						// 省略很多
};
struct zoneref {
	struct zone *zone;
	int zone_idx;
};
struct zonelist {
	struct zoneref _zonerefs[MAX_ZONES_PER_ZONELIST + 1];
};
```
> 伙伴系统从zonelist开始分配内存，zonelist中有一个zoneref数组，数组里有成员指向zone数据结构。zoneref数组的第一个成员指向的zone是页面分配器的第一个侯选者，其他成员是在第一个候选者分配失败之后才考虑的

> zonelist初始化路径如下
```c
start_kernel()->
		build_all_zonelists()->
		 			build_all_zonelists_init()->
									__build_all_zonelists()->
												build_zonelists()->
															build_zonelists_node()
								
// build_zonelists_node函数中从最高MAX_NR_ZONES的zone开始循环，初始化zonelist的_zonerefs数组
```

* mem_map全局变量，它是一个struct page数组，实现快速把虚拟地址映射到物理地址

### 2.1.5 空间划分
* 内核中使用宏PAGE_OFFSET来描述内核空间偏移量, 以地址总线位宽为32位来计算，虚拟地址空间是4GB时，用户空间和内核空间划分如下
> 当用户空间与内核空间按3G/1G划分时，内核空间大小是1GB, 那么PAGE_OFFSET就等于0xC0000000

> 当用户空间与内核空间按2G/2G划分时，此时内核空间大小是2GB，那么PAGE_OFFSET就等于0x80000000

* ARM Linux中有一个配置选项Memory split（它可用于高速用户和内核空间大小划分）, 在文件arch/arm/Kconfig

* 利用PAGE_OFFSET可以换算线性映射时物理地址和虚拟地址的转换关系
> 线性映射的物理地址 = 虚拟地址 - PAGE_OFFSET + PHYS_OFFSET

### 2.1.6 物理内存初始化
* 在内核启动时，内核知道物理内存DDR的大小，并计算出高端内存的起始地址和内核空间的内存布局后，物理内存页面接下来就要加入到伙伴系统中
> uboot或BIOS把物理内存的大小告诉linux内核

> 大部分物理内存页面都放在MIGRATE_MOVABLE链表中　

> 大部分物理内存页面初始化时存放在2的10次方链表中

* 物理页面是如何加入到伙伴系统的？
> 是以2的order次幂的方式加入的

```c
static unsigned long __init free_low_memory_core_early(void)
{
	...
	for_each_free_mem_range(i, NUMA_NO_NODE, &start, &end, NULL)
		count += _free_memory_core(start, end);
	...
}

static void __init __free_page_memory(unsigned long start, unsigned long end)
{
	int order;
	while (start < end) {
		...
		__free_pages_bootmem(pfn_to_page(start), order);
		...
	}
}

void __init __free_pages_bootmem(struct page *page, unsigned int order)
{
	...
	__free_pages(page, order); // 4.4.24版本，__free_pages_bootmem()->__free_pages_boot_core()->__free_pages()
}
```

> 物理页面加入到伙伴系统的代码调用路径(4.4.24版本内核)

|函数|.c|
|:-|:-|
|start_kernel   |init/main.c|
|mm_init	|init/main.c |
|mem_init        | arch/arm/mm/init.c |
|free_all_bootmem|mm/nobootmem.c|	
|free_low_memory_core_early|mm/nobootmem.c|
|__free_memory_core	|mm/nobootmem.c|
|__free_pages_memory|mm/nobootmem.c|
|__free_pages_bootmem|mm/page_alloc.c |
|__free_pages_boot_core|mm/page_alloc.c |
|__free_pages|mm/page_alloc.c|

## 2.2 页表的映射过程
* 多级页表命名

|页表名|kernel中的命名|
|:-|:-|
|一级页表（L0）|PGD|
|二级页表（L1）|PUD|
|三级页表（L2）|PMD|
|四级页表（L3）|PTE|

### 2.2.1 ARM32页表映射
* PGD页表基址通过init_mm结构的pgd成员来获取
> init_mm是mm_struct结构类型

#### ARM结构中一级页表PGD的偏移应该从第20位开始，为何头文件arch/arm/include/asm/pgtable-2level.h中的宏PGDIR_SHIFT是21，即从21位开始？
> start_kernel->setup_arch->paging_init->map_mem->__map_memblock->create_mapping->alloc_init_pud->alloc_init_pmd->alloc_init_pte->early_pte_alloc

```c
// arch/arm/mm/mmu.c 

static pte_t * __init early_pte_alloc(pmd_t *pmd, unsigned long addr, unsigned long prot)
{
	if (pmd_none(*pmd)) {
		pte_t *pte = early_alloc(PTE_HWTABLE_OFF + PTE_HWTABLE_SIZE);
		...
}
```
* 看上面的源码，early_alloc中传递的PTE_HWTABLE_OFF + PTE_HWTABLE_SIZE = 512 + 512

> 1、ARM32硬件结构中，PGD是从20位开始31位结束，PGD页表项数目是4096; PTE是从12位开始到19位结束，有256个PTE页表项；

> 2、Linux 内核中, PGD是从21位开始到31位结束，PGD页表项数目是2048; PTE是从12位到20位，共有512个PTE页表项；

> 对应1和2，Linux内核中少了一半PGD页表项，所以它采用双倍的PTE来补PGD少的那一半PGD页表项，即，在Linux内核中一个PGD表项对应512个PTE页表项

> 再看上面的代码，实际分配的是512 + 512个PTE页表项给一个PGD页表项，这么做是为什么呢？其中一半是给内核用的页表，后面的是给ARM硬件MMU用的

> 为什么不用一份，而是分别准备两组PTE？这是因为，ARM硬件页面表中没有提供对标志位PTE_DIRTY, PTE_YOUNG, PTE_PRESENT支持，所以需要使用内核页表来模拟上述3个标志位

* PTE_DIRYT, PTE_YOUNG, PTE_PRESENT用途
> PTE_DIRYT，CPU执行写操作时会设置该标志位，表示对应的页面被写过，为脏页

> PTE_YOUNG，CPU访问页面时会标记该标志。在页面换出时，如果该标志位置位，说明该页刚刚被访问过，根据LRU算法，此页不益被换出，同时清除该标志位

> PTE_PRESENT，页在内存中

### 2.2.2 ARM64页表映射
* 内核初始化阶段会对内核空间的页表进行一一映射
> 使用函数create_mapping()完成映射　

> start_kernel->setup_arch->paging_init->map_mem->__map_memblock->create_mapping

> __create_mapping->alloc_init_pud->alloc_init_pmd->alloc_init_pte

* 本节以4KB页面，48位地址，4级页表说明ARM64的地址映射过程
> 用户空间和内核空间内存分布图

|start|end|size|use|
|:-|:-|:-|:-|
|0x0000 0000 0000 0000|0x0000 ffff ffff ffff|256TB|用户|
|0xffff 0000 0000 0000|0xffff ffff ffff ffff|256TB|内核|

> VMALLOC_START = 0XFFFF 0000 0000 0000

* PGD页表基址通过init_mm结构的pgd来取，swapper_pg_dir全局变量指向PGD页表基址
```
// arch/arm64/kernel/vmlinux.lds.S
swapper_pg_dir = .;
. += SWAPPER_DIR_SIZE;

// arch/arm64/include/asm/page.h

#define SWAPPER_PGTABLE_LEVELS	(CONFIG_ARM64_PGTABLE_LEVELS - 1) // 因为这里讲的是4级页表，所以CONFIG_ARM64_PGTABLE_LEVELS = 4
#define SWAPPER_DIR_SIZE	(SWAPPER_PGTABLE_LEVELS * PAGE_SIZE)

// arch/arm64/include/asm/pgtable-hwdef.h
#define ARM64_HW_PGTABLE_LEVEL_SHIFT(n) ((PAGE_SHIFT - 3) * (4 - (n)) + 3) //  (4 - n) * (PAGE_SHIFT - 3) + 3

#define PAGE_SHIFT 	12 // 因为是4KB页面
#define PGDIR_SHIFT 	ARM64_HW_PGTABLE_LEVEL_SHIFT(0) 	// (4 - 0) * (12 - 3) + 3 = 39
#define PUD_SHIFT 	ARM64_HW_PGTABLE_LEVEL_SHIFT(1)  	// (4 - 1) * (12 - 3) + 3 = 30
#define PMD_SHIFT 	ARM64_HW_PGTABLE_LEVEL_SHIFT(2)		// (4 - 2) * (12 - 3) + 3 = 21
#define PTE_SHIFT	ARM64_HW_PGTABLE_LEVEL_SHIFT(3)		// (4 - 3) * (12 - 3) + 3 = 12

#define PTRS_PER_PTE    (1 << (PAGE_SHIFT - 3)) // 1 << 9 = 2^9 = 512
#define PTRS_PER_PGD	PTRS_PER_PTE
#define PTRS_PER_PUD	PTRS_PER_PTE
#define PTRS_PER_PMD	PTRS_PER_PTE

#define PGDIR_SIZE	1 << PGDIR_SHIFT 	// 1 << 39 // 512GB, 因为有512个PGD页表项，所以总计可映射的最大虚拟地址是：512 * 512GB = 256TB
#define PUD_SIZE	1 << PUD_SHIFT		// 1 << 30 // 1GB, 512个PGD, 512个PUD，一个PUD可映射1GB，所以可映射最大地址是:512 * 512 * 1GB = 256TB
#define PMD_SIZE	1 << PMD_SHIFT		// 1 << 21 // 2MB, 512PGD * 512PUD * 512PMD * 2MB = 256TB
#define PTE_SIZE	1 << PTE_SHIFT		// 1 << 12 // 4KB, 512PGD * 512PUD * 512PMD * 512PTE * 4KB = 256TB	

```

## 2.3 内核内存的布局图
* Linux 内核在启动时会打印出内核内存空间布局图
> start_kernel->mm_init->mem_init

### 2.3.1 ARM32内核内存布局图
* ARM Vexpress 平台打印的内核虚拟内存空间布局图如下
```
Virtual kernel memory layout:
	vector	: 0xffff0000	-	0xffff1000	(4KB)
	fixmap	: 0xffc00000	-	0xfff00000	(3072 KB)
	vmalloc : 0xf0000000	-	0xff000000	(240 MB)
	lowmem  : 0xc0000000 	-	0xef800000	(760 MB)
	pkmap	: 0xbfe00000 	-	0xc0000000	(2 MB)
	modules : 0xbf000000	-	0xbfe00000	(14 MB)
	 .text	: 0xc0008000	-	0xc0658750	(6466 KB)
	 .init  : 0xc0659000	-	0xc0782000	(1188 KB)
	 .data  : 0xc0782000	-	0xc07b1920	(191 KB)
	 .bss	: 0xc07b1920	-	0xc07db378	(167 KB)
```

* 内核编译完成后生成的System.map文件中有各部分地址(当然全是虚拟地址)
* 高端内存起始地址是在内存初始化时，在函数sanity_check_meminfo()中确定的，并将高端内存起始地址保存在全局变量high_memory中
* 内核通常把低于760MB的称为线性映射内存，高于760MB的称为高端内存

### 2.3.2 ARM64内核内存布局图
* ARM64处理器采用48位物理寻址; 虚拟地址同样最大支持48位寻址　
* Linux内核在大多数体系结构上把两个地址空间划分为用户空间和内核空间
> 用户空间：0x0000 0000 0000 0000	～	0x0000 ffff ffff ffff

> 内核空间：0xffff 0000 0000 0000	～	0xffff ffff ffff ffff

* 64位的Linux内核中没有高端内存概念，因为48位寻址空间已经足够大了

* ARM64架构的Linux内核的虚拟内存分布图如下　
```
Virtual kernel memory layout:
	vmalloc : 0xffff 0000 0000 0000		-	0xffff 7bff bfff 0000 	(126974 GB)
	vmemmap : 0xffff 7bff c000 0000		-	0xffff 7fff c000 0000	(4096 GB)
		: 0xffff 7bff c100 0000		-	0xffff 7bff c300 0000 	(32 MB)
	fixed	: 0xffff 7fff fabf e000		-	0xffff 7fff ac00 0000 	(8 KB)
	PCI I/O : 0xffff 7fff fae0 0000		-	0xffff 7fff fbe0 0000 	(16 MB)
	modules : 0xffff 7fff c000 0000		-	0xffff 8000 0000 0000 	(64 MB)
	memory	: 0xffff 8000 0000 0000		-	0xffff 8000 8000 0000	(2048 MB)
	 .init  : 0xffff 8000 0077 4000		-	0xffff 8000 008b c000	(1312 KB)
	 .text  : 0xffff 8000 0008 0000		-	0xffff 8000 0077 34e4	(7118 KB)
	 .data  : 0xffff 8000 008c 0000 	-	0xffff 8000 0091 f400	(381 KB)
```

## 2.4 分配物理页面
### 2.4.1 伙伴系统分配内存
```c
// include/linux/gfp.h
#define alloc_pages(gfp_mask, order) alloc_pages_node(numa_node_id(), gfp_mask, order)
alloc_pages->alloc_pages_node->__alloc_pages->__alloc_pages_nodemask->first_zones_zonelist
									get_page_from_freelist->for_each_zone_zonelist_nodemask
									__alloc_pages_slowpath
									
	get_page_from_freelist->cpuset_zone_allowed
				zone_watermark_ok
				buffer_rmqueue->__rmqueue->__rmqueue_smallest->expand
						zone_statistics
				prep_new_page->check_new_page

/* 1. 伙伴系统分配内存的第一步是要找到一个合适的zone
   2. 之后遍历zone的空闲区中指定迁移类型的空闲链表，从与order相等的空闲区开始寻找(current_order = order)，如果第一个等于order的空闲区中对应迁移类型的空闲链表没有空闲对象，则++current_order，进入下一次循环，找更大的空闲区对应迁移类型的空闲链表，如果空闲链表里有空闲对象，那么就把这个空闲块从链表上取下来，之后用expand来拆分内存块（当然，如果找到的内存区的order与目标相同就不拆分了，直接从链表上取下来返回; 这在expand里会将找的这个页块拆开，结束拆分的条件是当前分配的页块的current_order不大于申请的order）
   3. 到这里内存块已经分配成功，从__rmqueue返回struct page指针　
   4. 回到buffer_rmqueue后，需要调用zone_staticstics做统计
   5. 回到get_page_from_freelist后，调用prep_new_page来调用check_new_page对分配的内存块的struct page进行检查，合格后即可返回最终的struct page指针
 */
```
* 找到zone之后，在函数__rmqueue_smallest中找空闲链表时，首先找的是与申请的order相同的空闲区中的空闲链表，如果没有空闲对象，才会往大的空闲区中找
> 所以，一般来说找到的空闲链表都是大于申请的order的，所以需要expand进行切分，返回一个合适大小的page

* 什么样子的zone是合适的？（page = alloc_pages(GFP_KERNEL, order）
> 首先根据分配掩码找到：空闲链表的迁移类型、zone索引(用于从zonelist中定位是哪个zone)、使用numa_node_id()得到内存结点

> 之后根据内存结点找到zonelist

> 之后在for_each_zone_zonelist_nodemask中的指定的内存结点上遍历该内存结点下zonelist的_zonerefs数组中的所有zone（对于ARM32，只有NORMAL和HIGH），找与zone_idx相同的zone，到这里已经找到zone了

### 2.4.2 释放页面
* 核心函数__free_pages->__free_pages_ok->__free_one_page
			free_hot_cold_page

```c
static inline void __free_one_page(struct page *page, 
					unsigned long pfn,
					struct zone *zone, 
					unsigned int order,
					int migratetype)
{
	unsigned long page_idx;
	unsigned long combined_idx;
	unsigned long uninitialized_var(buddy_idx);
	struct page *buddy;
	int max_order = MAX_ORDER;

	page_idx = pfn & ((1 << max_order) - 1);

	while (order < max_order - 1) {
		buddy_idx = __find_buddy_index(page_idx, order);
		buddy = page + (buddy_idx - page_idx);
		if (!page_is_buddy(page, buddy, order))
			break;

		if (page_is_guard(buddy))
			clear_page_guard(zone, buddy, order, migratetype);
		else {
			list_del(&buddy->lru);
			zone->free_area[order].nr_free--;
			rmv_page_order(buddy);
		}
		combined_idx = buddy_idx & page_idx;
		page = page + (combined_idx - page_idx);
		page_idx = combined_idx;
		order++;
	}
}
```

* 函数__free_one_page是合并相邻伙伴的核心代码，举例如下
> 假设现在要释放一个内存块A，大小为2个page, 内存块的page的开始页帧号是0x8e010, order=1

> 1、首先计算得出page_idx = 0x8e010 & ((1 << 10) - 1) = 0x8e010 & 0x3ff = 0x10; // 取pfn的后10位

> 2、在第一次while循环中，计算buddy_idx = page_idx ^ (1 << order) = 0x10 ^ 2 = 0x12 // ^是异或, 0001 0000 ^ 0010 = 0001 0010 = 0x12

> 3、那么buddy就是内存块A的相邻内存块B了

> 4、接下来通过page_is_buddy函数判断内存块B是不是空闲内存块

> 5、如果发现内存块B也是空闲块，并且order等于1，那我们就找到一个伙伴，把它从空闲链表上取下来，以便和内存块A合并到高一阶的空闲链表中

> 6、这时combined_idx指向内存块A的起始地址。order++表示继续在附近寻找有没有可能合并的相邻内存块，这次要找的order=2

> 7、重复步骤2，查找附近有没有order=2的内存块

> 8、如果在0x14位置的内存块C不满足合并条件，例如内存块C不是空闲页面，或者C的order不是2。如果没有找到order=2的内存块，那么只能合并内存块A和B了，然后把这个内存块添加到空闲页表中

### 2.4.3 小结
## 2.5 slab分配器
### 2.5.1 创建slab描述符（有很事要做）
* 关键数据结构
```c
// struct kmem_cache是slab描述符, 在include/linux/slab_def.h中定义
struct kmem_cache {
	struct array_cache __percpu *cpu_cache; // 本地CPU对象缓冲池
	unsigned int batchcount; // 当本地CPU缓冲池为空时，从共享缓冲池或slabs_partial, slabs_free中获取的对象数目;由enable_cpucache计算
	unsigned int limit; // 当本地缓冲池中空闲对象个数大于limit时，主动释放batchcount个对象，便于内核回收或销毁slab;在enable_cpucache中计算
	unsigned int shared; // 多核系统中shared大于0, 在enable_cpucache()中初始化
	unsigned int size; // 对象长度，需要根据align调整
	...
	unsigned int flags; //对象分配掩码
	unsigned int num; // 一个slab中最多可以有的对象数

	unsinged int gfproder; // 一个slab中占用2^gfporder个页面
	...
	size_t colour; // 一个slab中有几个不同的cache line, 即，包含的cache line数量
	// colour 的值就是一个slab描述符中slab的个数(根据colour_next的取值推断)
	// 从着色区的功能上分析，colour是slab总数也是合理

	unsinged int colour_off; // 一个cache colour的长度, 等于L1 cache line大小
	// 着色区的长度 = colour_next * colour_off, 其中colour_next从0开始递增, 最大值colour-1
	...

	unsigned int freelist_size;
	// cache_init_objs调用set_free_obj初始化freelist中每个值为索引值
	// 有一点不明白的是，在free_block中回收缓冲对象时，slab_put_obj还是调用set_free_obj，值还是原值，而且在slab_get_obj中只是page->active++，freelist中保存的值未做任何变化，是不是多余调用slab_put_obj中的set_free_obj？
	...
	const char *name; // slab描述符名字
	...
	int object_size; // 实际申请的对象大小
	int align; // 对齐长度
	struct kmem_cache_node *node[MAX_NUMNODES]; // slab节点，在numa系统中，每个结点有一个kmem_cache_node; 它包含3个slab链表：部分空闲、完全用尽、空闲
};

struct array_cache { 
	unsinged int avail; // 对象缓冲池中可用的对象数　
	unsigned int limit; // 当本地缓冲池中空闲对象个数大于limit时，主动释放batchcount个对象，便于内核回收或销毁slab
	unsigned int batchcount; // 当本地CPU缓冲池为空时，从共享缓冲池或slabs_partialslabs_free中获取的对象数目
	unsigned int touched;
	void *entry[]; // 用于保存对象
};
// cpu_cache_get(kmem_cache*)取cpu本地slab缓冲池array_cache*

// struct kmem_cache_node定义在mm/slab.h中
struct kmem_cache_node {
	spinlock_t list_lock;
	struct list_head slabs_partial; //部分空闲， 链表中成员是slab
	struct list_head slabs_full;    //完全用尽
	struct list_head slabs_free;	//空闲　
	unsigned long free_objects; // 三个链表中所有空闲对象数目
	unsigned int free_limit; // slab中可容许的空闲对象数目最大阈值
	unsigned int colour_next; // slab结点中下一个slab应当包括的colour数目
	struct array_cache *shared; //多核CPU中，除了本地CPU的缓冲池外，其余CPU有一个共享的对象缓冲池
	...
};
 
```

* 关键函数
> kmem_cache_create/destroy, 创建自定义的slab描述符;kmalloc用于创建通用的缓存

> kmem_cache_alloc/free

> calculate_slab_order, 计算一个slab需要多少个物理页面

> cache_estimate, 计算了一个slab中可容纳多少个对象

> cache_line_size, 计算L1 cache line大小

> enable_cpucache, 计算shared, limit, batchcount

> alloc_kmem_cache_cpus, 分配每CPU对象缓冲池

* 新创建的slab描述符会添加到全局链表slab_caches中
* 一个slab最多可以包含KMALLOC_MAX_ORDER（25）个页面，2^25 = 32MB
> 见include/linux/slab.h

### 2.5.2 分配slab对象　
* 关键函数
> kmem_cache_alloc

> cache_alloc

> cache_alloc_refill
```
1.首先判断共享对象缓冲池n->shared中有没有空闲对象，有的话，尝试迁移batchcount个空闲对象到本地对象缓冲池, 使用transfer_objects实现
2.如果共享对象缓冲池没有空闲对象，那么查看slab结点中的slabs_partial, slabs_free链表
  1)如果slabs_partial或slabs_free链表不为空，说明有空闲对象，那么从链表中取出一个slab，通过slab_get_obj()取得对象地址，然后通过ac_put_obj把对象迁移到本地对象缓冲池中，最后把这个slab挂回合适的链表；这里是每次从slab里取一个对象，所以在cache_alloc_refill中是用一个循环来迁移batchcount个对象的

  2)如果slabs_partal, slabs_free链表都为空，说明整个slab节点都没有空闲对象，这时需要重新分配slab, 即需要使用函数cache_grow来分配slab 
```
> cache_grow, 分配slab, 首先分配所需页面（用伙伴系统来分配），之后调用alloc_slabmgmt, slab_map_pages

> alloc_slabmgmt, 设置page->s_mem, s_mem是slab中第一个对象的开始地址；还计算slab中的freelist

> slab_map_pages, 设置page->slab_cache, slab_cache就是当前操作的slab描述符, 只有slab的第一个页面的page结构中的slab_cache指向slab描述符

### 2.5.3 释放slab缓冲对象
* virt_to_pfn, pfn_to_page
* 在一个slab中，第一个页面的page结构中的page->slab_cache指向struct kmem_cache，即指向slab描述符
* __cache_free
* cache_flusharray
> 当本地对象缓冲池中的空闲对象ac-avail（可用空闲对象数）大于或等于ac->limit时，__cache_free会调用cache_flusharray来尝试回收空闲对象

> cache_flusharray在回收空闲对象时，先判断是否有共享对象缓冲池，如果有，就把本地对象缓冲池中的空闲对象复制batchcount个到共享对象缓冲池

> 如果共享对象缓冲池中的空闲对象数量等于limit，那么会调用free_block主动释放batchcount个空闲对象。如果slab没有活跃对象了（即page->active == 0），并且slab节点中所有空闲对象数目n->free_objects超过n->free_limit，那么调用slabs_destroy来销毁这个slab

* free_block

* ac_put_obj, 将对象释放到本地对象的缓冲池中

* page->active用于记录活跃slab对象个数，slab_get_obj函数分配一个slab对象时会增加page->active值，slab_put_obj函数释放一个slab对象时会减小page->active

### 2.5.4 kmalloc分配函数
* 系统启动时会调用create_kmalloc_caches()函数创建名为kmalloc-16, kmalloc-32, kmalloc-64....的slab描述符
* kmalloc_index函数, 用于方便查找使用哪个slab描述符来分配slab对象, 它根据不同的size与2的order次对比来选择不同的kmalloc-x描述符
> 源码中，最大可返回26，即kmalloc最大可分配64MB？书中写最大32MB

* mm/slab_common.c中定义的全局kmalloc_caches数组中保存着所有kmalloc描述符

* kmalloc函数分配物理内存给内核后，后续不需要更新页表？

### 2.5.5 小结
* 不论是slab自定义描述符分配缓冲对象还是使用kmalloc来分配缓冲对象，都是在分配物理内存，为什么vmalloc在使用页分配器分配完物理内存后，需要调用map_vm_area建立页表映射，而slab这些不需要？
> 因为slab对象太小了？每个对象最大SLAB_OBJ_MAX_NUM是2^7或2^15

> 还是因为slab系统架构本身即可?

> ?

* 一个slab没有明确的数据结构来描述，它成三段组，着色区 + freelist区 + obj区
> 着色区的值从0，按colour_off递增，保障各个不同的slab在cache line上不冲突

> obj就是slab对象, 一个slab中的obj个数等于kmem_cache.num　

* slab着色区的作用是让不同的slab可以命中到不同的cache line
> 通过colour_next * colour_off来实现, colour_next从0加到kmem_cache.colour，colour_off是cache line大小

* slab分配器通过使用本地CPU对象缓冲池可提升硬件和cache使用率
> 让一个对象运行在同一个CPU上，可以让对象使用同一个CPU的cache，有助于提高性能

> 访问per-cpu类型的本地对象缓冲池不需要使用自旋锁，因为不会有别的CPU来访问这些per-cpu类型的对象池，避免自旋锁争用

* 微小嵌入式系统使用slob分配器

* 大型系统使用slub分配器

* slab需要的物理内存在什么时候分配？
> 在创建slab描述符时，不会立即分配页面，要等到分配slab对象时，发现本地缓冲池和共享缓冲池都是空的，然后查询3大链表中也没有空闲对象，这时才分配一个slab所需要的页面，并且把这个slab挂入slabs_free链表

* slab系统有两种回收内存的方式
> 1.使用kmem_cache_free释放一个对象

> 2.注册定时器，定时调用cache_reap

## 2.6 vmalloc
* 本书中介绍的是以ARM32的内核路径为例说明vmalloc实现的; 
* 相关数据结构、宏定义、关键函数
```c
struct vm_struct
VMALLOC_START, VMALLOC_END
static struct rb_root vmap_area_root = RB_ROOT;
struct vmap_area // 用于将找到的hole插入到红黑树vmap_area_root中；并通过setup_vmalloc_vm设置vm_struct

__vmalloc_node_range->__get_vm_area_node->alloc_vmap_area->__insert_vmap_area
					  setup_vmalloc_vm
			__vmalloc_area_node->map_vm_area

pte_alloc_one_kernel
alloc_page->alloc_pages(gfp_mask, 0) // vmalloc的物理内存由伙伴系统负责(alloc_page)分配
map_vm_area // vmalloc的建立页表映射函数
```

### vmalloc的逻辑流
* __vmalloc_node_range->__get_vm_area_node->alloc_vmap_area->__insert_vmap_area
					    setup_vmalloc_vm
			__vmalloc_area_node->map_vm_area

> 1、alloc_vmap_area在vmalloc空间查找一块大小合适的且没被使用的空间hole，并将最终找到或分配的hole插入到vmap_area_root红黑树，函数返回vmap_area*

> 2、setup_vmalloc_vm把alloc_vmap_area返回的vmap_area中包含hole的信息填写到vm_struct，并返回vm_struct*

> 3、接下来__vmalloc_area_node函数给vmalloc分配物理页面，并将page指针保存到vm_struct.pages数组中，之后调用map_vm_area更新页表

## 2.7 VMA操作
* 关键数据结构　
```c
struct vm_area_struct {
	unsigned long vm_start;
	unsigned long vm_end;
	...
	struct rb_node vm_rb;
	...
	struct mm_struct *vm_mm;
	...

}; 
struct mm_struct {
	struct vm_area_struct *mmap; // 单链表来存当前进程中所有的vm_area_struct实例
	struct rb_root mm_rb; // 每个进程都有一个红黑树，每个vm_area_struct实例都会连到此红黑树上
	...
};
```

### 2.7.1 查找VMA
* 关键函数
> find_vma

### 2.7.2 插入VMA
#### 关键函数
* insert_vm_struct, 向VMA链表和红黑树插入一个新的VMA

* find_vma_links查找要插入的位置, 作为返回值的几个参数如下
> rb_parent指针指向要插入的节点的父节点

> pprev指针指向要插入的节点的父节点指向的VMA

* 书中125页描述“rb_link指向要插入节点指针本身的地址”，新的节点还没有插入，哪里来的rb_link？也就是说，红黑树上的某个没有子节点的节点P，它的左右子节点的指针P->rb_left和P->rb_right在rb_link_node中全是初始化为NULL的？
```c
// 我的新的理解如下，我的答案：
struct rb_node {
	...
	struct rb_node *rb_right;
	struct rb_node *rb_left;
};

struct rb_node ***rb_link;
struct rb_node **rb_parent;
1.假设新节点A
2.rb_parent是无疑问的，它是新节点A的父节点
  rb_parent中的两个成员rb_left和rb_right全指向NULL，但是因为rb_parent是已经存在的，即已经被初始化的有物理内存的，那么它的两个成员也是已经被初始化的，这两个成员rb_left, rb_right都是指针，它们各自占用的8字节地址空间已经被分配了，只不过指针的值是NULL

3.三级指针的值是存二级指针的地址，而二级指针的值是存一级指针的地址，所以
static nt find_vma_links(struct mm_struct *mm, unsigned long addr, unsigned long end, struct vm_area_struct **pprev,
			struct rb_node ***rb_link, struct rb_node **rb_parent)
int insert_vm_struct(struct mm_struct *mm, struct vm_area_struct *vma)
{
	struct vm_area_struct *prev;
	struct rb_node **rb_link, *rb_parent;

	if (!vma->vm_file) {
		BUG_ON(vma->anon_vma);
		vma->vm_pgoff = vma->vm_start >> PAGE_SHIFT;
	}
	if (find_vma_links(mm, vma->vm_start, vma->vm_end, &prev, &rb_link, &rb_parent)) // 关注三个指针prev, rb_link, rb_parent
		return -ENOMEM;
	/* 指针prev, rb_link, rb_parent的值全是NULL，因为没有初始化，但是指针的地址都已经分配了内存，所以可以取它们的地址作为参数传给find_vma_links函数
	 */
	...
	
}
find_vma_links(...,&rb_link,...) 
{
	struct ** __rb_link;
	...
		__rb_link = &__rb_parent->rb_right;// 或rb_left指针的地址，注意有&
	...
	*rb_link = __rb_link; // 这里是将二级值指针的值修改为指针rb_left或rb_right的地址
	...
}
```

* vma_link 添加到链表(__vma_link_list)和红黑树(__vma_link_rb)

#### 二级指针在Linux内核中主要有两种用法
* 作为函数形参
* 链表操作（如上面的红黑树操作；还有RCU）

### 2.7.3 合并VMA
* vma_merge函数实现将一个新的VMA和附近的VMA合并
> vma_merge的参数中的prev是新VMA的前继节点，start, end是新vma的起始和结束地址，所以，在使用vma_merge时可知，新的vma并未创建

> 最终合并在vma_adjust函数实现

* vma_merge()常见的可以合并的3种情况
> 1、新的VMA的起始地址和prev节点的结束地址重合

> 2、新的VMA的终止地址和next节点的开始地址重合

> 3、新的VMA的开始地址和prev节点的结束地址重合，且新的VMA的结束地址与next节点的开始地址重合


### 2.7.4 红黑树例子
* 见实验: book/runlinuxkernel/code/2/2.7.4
### 2.7.5 小结

## 2.8 malloc
### 2.8.1 brk实现
* malloc, brk是从进程虚拟地址空间堆上分配虚拟内存
> mm_struct.end_data是数据段的结束地址，brk申请分配的虚拟地址必须大于它

> mm_struct.brk记录动态分配区当前的底部

> brk系统调用的参数是新边界newbrk，它等于申请内存长度len, 加上mm_struct.brk, newbrk = len + mm_struct.brk

> 如果newbrk < mm_struct.brk，表示释放空间，调用do_munmap函数实现

* 关键数据结构、函数
> find_vma_intersection, 使用旧边界找当前进程中有没有一块已经存在的VMA满足当前申请要求，如果有则返回旧边界

> do_brk->get_unmapped_area // 判断虚拟内存是否有足够的空间
	  find_vma_links // 在红黑树中的精确定位插入新节点的节点相关信息
	  vma_merge // 是否可以合并vma
	  vma_link  // 如果不可以合并，则创建新vma，并将其添加到mm_struct.mmap链表和mm_struct.mm_rb红黑树

> 从do_brk返回目标vma的起始地址, 之后回到brk系统调用中，判断flags是否置位VM_LOCKED，如果是则需要调用mm_populate立即分配物理内存并建立页表映射

* 调用系统调用mlockall会置位VM_LOCKED

### 2.8.2 VM_LOCKED情况
* 指定标志位VM_LOCKED表示：需要立即为描述的这块进程地址空间的VMA分配物理页面并建立映射
* mm_populate->__mm_populate->__mlock_vma_pages_range->__get_user_pages
> __mlock_vma_pages_range调用__get_user_pages给VMA分配物理内存并建立映射

* __get_user_pages
> mm/gup.c

> cond_resched, 判断当前进程是否需要被调度，内核代码通常在while循环中添加cond_resched，用于优化系统的延迟

> follow_page_mask, 查看vma中的虚拟页面是否已经分配了物理内存，通过判断页表的方式来确认; follow_page_mask是follow_page的具体实现;mm/gup.c

* follow_page_mask
> 在mm/gup.c中实现

> 它通过检查页表PGD, PUD, PMD, PTE来检查各级页表是否有效，通过下列函数来检查

```c
pgd_offset, pud_offset, pmd_offset, follow_page_pte
```

> follow_page_pte->vm_normal_page

> vm_normal_page把页面分为normal page, special page

```
1. normal page举例，匿名页面、page cache和共享内存页面
2. special page，这些页面不希望参与内存管理的回收、合并，如:
   1) VM_IO: 为I/O设备映射内存
   2) VM_PFN_MAP: 纯PFN映射, vma->vm_flags中如果定义了这些标志位，则这是special mapping, 见vm_normal_page函数
   3) VM_MIXEDMAP: 固定映射
```

> 如果follow_page_mask没有返回struct page，那么__get_user_pages会调用faultin_page->handle_mm_fault来人为地触发一个缺页中断

> handle_mm_fault是缺页中断处理的核心函数

> 分配完页面后，pages指针数组指向这些page，最后调用flush_anon_page, flush_dcache_page来flush这些页面对应的cache

### 2.8.3 小结
* 处理器的MMU硬件单元处理最小单元是页
> 内核建立虚拟地址和物理地址映射以页为单位

> PAGE_ALIGN(addr)宏让地址addr按页大小对齐

* 每个进程都有一个自己的页表，一个mm_struct结构、一个管理VMA的红黑树和链表
> mm_struct.pgd指向页面基地址

> 进程本身的VMA会挂入属于自己的红黑树和链表

* malloc函数的逻辑流
```c
malloc()->
      brk()->
          find_vma_intersection()
	  do_brk()->
	           get_unmapped_area()
		   find_vma_links()
		   vma_merge()
		   分配一个新的vma
		   vma_link()把新的vma插入mm系统
	           无VM_LOCKED->返回brk新边界
	   	   置位VM_LOCKED->
	    		mm_populate()->
			  __mlock_vma_pages_range()->
					__get_user_pages()->
						find_extend_vma()
						follow_page_mask()
							N->faultin_page()->
 								  handle_mm_fault()->
								      page页面已经分配和映射->
  								      			返回brk新边界

/* malloc有三条逻辑流　　
 * 1.find_vma_intersection, 查找已经存在的vma逻辑流程
 *
 * 2.do_brk, 申请vma的逻辑流
 *   1)先用get_unmapped_area判断目前剩余的虚拟地址空间是否够大
 *   2)再用vma_merge判断能不能合并，如果可以合并就不分配新的vma，所以原则是新的虚拟地址能合并先合并到原有的vma，不能合并才分配新的vma给虚拟地址
 *   3)前面1)不满足条件就返回错误, 2)是满足条件就直接返回合并后的新vma，如果不能合并接下来就需要创建一个新的vma来管理这个虚拟地址
 *   4)有了新的vma，就需要把这添加到mm_struct的红黑树和vm_area_struct链表中
 *   5)如果mm->def_flags标志位VM_LOCKED没有置位，则直接返回新的brk边界
 *   6)如果有置位VM_LOCKED则在回到系统调用brk()后，需要调用mm_populate()为vma分配物理内存并建立映射
 *
 * 3.mm_populate, 需要立即分配物理页面并建立页表映射的逻辑流
 *   1)调用__mlock_vma_pages_range为vma分配物理内存并建立页表映射，它只是入口，具体的分配和建立映射是后面的__get_user_pages完成
 *   2)find_extend_vma, 查找vma，如果目标start地址小于vma->start，则扩展vma；如果没有找到，且start在gate_vma中，那用gate_vma;否则从__get_user_pages错误返回
 *   3)现在找到vma了，接下来判断它的页表，用follow_page_mask，如果一切正常则返回struct page，如果页表中没有对应这个新虚拟地址合适的物理页面，则手动触发页中断，用函数faultin_page调用缺页中断处理函数handle_mm_fault来分配物理页面并建立映射
 *   4)从mm_populate返回到brk()系统调用
 *   5)brk系统调用返回brk新边界
 */

// find_vma_intersection()查找是否已经存在vma
// get_unmapped_area()判断是否有足够的空间
// vma_merge()判断是否可以和附近的vma合并　
```

* malloc实现涉及内存管理中几个重要函数
> get_user_pages, 将用户空间的虚拟内存传到内核空间，内核为其分配物理内存并建立页表映射; 如果pte的内容为空或者页表L_PTE_PRESENT没置位，那么handle_mm_fault()分配内存并建立映射

> follow_page, 通过虚拟地址寻找相应的物理页面，返回normal mapping页面对应的struct page, 函数寻找确认的过程中会查页表, 但还有其他判断

> vm_normal_page, 过滤掉那些special page

## 2.9 mmap
* 私有映射和共享映射的区别？
* 下面strace捕捉某个进程的mmap调用，为什么第二次调用mmap没有出现地址重叠错误呢？
```c
mmap(0x20000000, 8192, PORT_READ | PORT_WRITE, MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0) = 0x20000000
...
mmap(0x20000000, 4096, PORT_READ | PORT_WRITE, MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0) = 0x20000000
/////////////////////////////////////////////////////
unsigned long mmap_region(struct file *file, unsigned long addr, unsigned long len, vm_flags_t vm_flags, unsigned long pgoff)
{
	...
	if (find_vma_links(mm, addr, addr + len, &prev, &rb_link, &rb_parent)) {
		if (do_munmap(mm, addr, len))
			return -ENOMEM;
		...
	}
	...
}
/* 回答：
 * 查看do_mmap_pgoff()->mmap_region()函数, 看上面的代码片段, 在mmap_region中调用了函数find_vma_links函数，它是用来查找当前进程地址空间中的VMA们的，它会检查当前要映射的区域和已经存在的vma们是否存在重叠，如果有重叠，它会返回-ENOMEM，这样在mmap_region函数中会调用do_munmap函数来把这段重叠的区域先销毁，然后重新映射，这就是上面问题中mmap第二次映射同样的地址没有出错的原因
 * 
 */
```

### 2.9.1 mmap概述
```c
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

int munmap(void *addr, size_t length);

// addr 一般设置为NULL, 让内核来分配地址 
// prot 设置映射区域的读写属性
// flags，以什么方式映射，如共享映射、私有映射
// fd, 文件映射时，它是打开的文件句柄 
// offset, 文件映射时，表示文件的偏移

prot可以取这些值：PROT_EXEC, PROT_WRITE, PROT_READ, PROT_NONE
flags可取值：MAP_PRVATE, MAP_SHARED, MAP_ANONYMOUS, MAP_FIXED, MAP_POPULATE
```
#### mmap映射类型
* 私有匿名映射，通常用于内存分配
> 当参数fd=-1, flags=MAP_PRIVATE | MAP_ANONYMOUS时，创建的mmap映射是私有匿名映射

> 私有匿名映射最常见的用途是glibc分配大块内存中，当需要分配的内存大于M_MMAP_THRESHOLD(128KB)时，glibc会默认使用mmap代替brk来分配内存

* 私有文件映射，通常用于加载动态库

* 共享匿名映射，通常用于进程间共享内存

* 共享文件映射，通常用于内存映射IO，进程间通信
> 共享文件映射通常有两个应用场景：读写文件、进程间通信

### 2.9.2 小结
* 问题：在一个播放程序中同时打开几十个不同的高清视频文件，发现播放有些卡，打开视频文件是用mmap函数实现的，请分析原因
> 使用mmap来创建文件映射时，由于只是创建了vma，而没有立即为vma分配物理内存和页表映射，所以当播放器真正读取文件时，产生了缺页中断才去读取文件内容到page cache中。这样每次播放器真正读取文件时，会频繁的发生缺页中断，然后从文件中读取磁盘内容到page cache中，导致磁盘读性能比较差，从而造成播放视频卡。

> 如何解决这个问题？有效提高流媒体服务I/O性能的方法是增大内核默认预读窗口，默认内核预读的大小是128KB, 可以通过shell命令blockdev --setra来修改

* mmap逻辑流
```c
mmap->sys_mmap_pgoff->do_mmap_pgoff->get_unmapped_area->mmap_region->vma_merge->N->分配一个新的VMA->三个分支
分支一、文件映射->获取文件结构->file->f_op->mmap()->把新vma插入mm系统中（红黑树和vma链表）
分支二、匿名映射->把新vma插入mm系统中
分支三、shmem映射->shmem_zero_setup()建立一个共享匿名映射->把新的vma插入到mm系统中
->有VM_LOCKED->mm_populate->...->返回addr
->无VM_LOCKED->返回addr
```
## 2.10 缺页中断处理
* 匿名页面、KSM页面、page cache页面、写时复制、私有映射、共享映射
* 缺页异常依赖CPU的体系结构
* ARM的MMU中有两个与存储访问失效相关的寄存器
> 失效状态寄存器（Data Fault Status Register, FSR）

> 失效地址寄存器（Data Fault Address Register, FAR）

* do_DataAbort()
```c
struct fsr_info {
	int (*fn)(unsigned long addr, unsigned int fsr, struct pt_regs *regs); // 修复失效状态的函数指针
	int sig; // 内核处理失败时，发送的信号类型
	int code;
	const char *name; // 失效状态的名称
};

static struct fsr_info fsr_info[] = {
...
{ do_page_fault,        SIGSEGV, SEGV_MAPERR,   "page translation fault"}, // 页面转换失效
...
{ do_page_fault,        SIGSEGV, SEGV_ACCERR,   "page permission fault"},  // 页面访问权限失效
...
};
// fsr_info与具体的CPU体系结构相关，上面的例子中的fsr_info数组定义在arch/arm/mm/fsr-2level.c
```

### 2.10.1 do_page_fault()
* do_page_fault是缺页中断处理的核心函数，它具体实现与CPU的体系结构相关
```c
do_page_fault->
	__do_page_fault->find_vma
	                 access_error
			 handle_mm_fault->重新建立物理页面与vma的映射关系->handle_pte_fault->(1),(2),(3)
				(1)pte_present()为0
				   页不在内存中，这是真正的缺页
				   A.pte内容为空
				     a.文件映射，调用vma.vm_ops.do_fault()
				     b.匿名映射，调用do_anonyous_page()
				   B.pte内容不为空，且present没置位，说明页被交换到了swap分区，调用do_swap_page加载页面

				(2)pte有物理页面，但页面原来是只读，现在需要写操作
				   调用do_wp_page()
				(3)pte_mkyoung(), 设置访问标志位, 表示页面刚刚被访问过

	__do_user_fault, 如果__do_page_fault返回出错，且进程处于用户模式，则会调用__do_user_fault
	__do_kernel_fault, 如果__do_page_fault返回出错，且处于内核模式，则执行__do_kernel_fault
```

### 2.10.2 匿名页面缺页中断
* do_anonyous_page, 匿名页面缺页中断核心函数
* 在linux内核中，未关联到文件的页面称为匿名页面

```c
static int do_anonymous_page(struct mm_struct *mm, struct vm_area_struct *vma, unsigned long address, pte_t *page_table, pmd_t *pmd, unsigned int flags)
{
	struct mem_cgroup *memcg;
	struct page *page;
	spinlock_t *ptl;
	pte_t entry;

	if (check_stack_guard_page(vma, address) < 0) // 判断当前vma是否需要添加一个guard page作为安全垫
		return VM_FAULT_SIGSEGV;

	// use the zero-page for reads
	if (!(flags & FAULT_FLAG_WRITE) && !mm_forbids_zeropage(mm)) { //分配的内存只需要只读，则使用一个全0填充的全局页面empty_zero_page
		entry = pte_mkspecial(pfn_pte(my_zero_pfn(address), vma->vm_page_prot)); // pfn_pte使用0页面生成一个PTE Entry, pte_mkspecial设置PTE entry的PTE_SPECIAL标志位
		page_table = pte_offset_map_lock(mm, pmd, address, &ptl); // 获取当前pte页表项
		if (!pte_none(*page_table)) // 判断取到的pte页表项是否为空
			goto unlock;
		goto setpte; // 页表项不为空，则跳到setpte
	}

	if (unlikely(anon_vma_prepare(vma))
		goto oom;
	page = alloc_zeroed_user_highpage_movable(vma, address); // 分配一个可写的匿名页面, 其利用伙伴系统来分配新页面
	if (!page)
		goto oom;

	__SetPageUptodate(page);

	if (mem_cgroup_try_charge(page, mm, GFP_KERNEL, &memcg))
		goto oom_free_page;
	entry = mk_pte(page, vma->vm_page_prot); // 生成一个PTE Entry
	if (vma->vm_flags & VM_WRITE)
		entry = pte_mkwrite(pte_mkdirty(entry));
	page_table = pte_offset_map_look(mm pmd, address, &ptl);
	if (!pte_none(＊page_table))
		goto release;
	inc_mm_counter_fast(mm, MM_ANONPAGES); // 增加匿名页面统计计数
	page_add_new_anon_rmap(page, vma, address); // 把匿名页面添加到反射映射机制中
	mem_cgroup_commit_charge(page, memcg, false);
	lru_cache_add_active_or_unevictable(page, vma); // 把匿名页面添加到LRU链表中
}

// empty_zero_page在系统启动时，由paging_init()分配一个页面作为零页面
```

### 2.10.3 文件映射缺页中断 
* vma->vm_ops->fault()
```c
static int do_fault(struct mm_struct *mm, struct vm_area_struct *vma, unsigned long address, pte_t *page_table, pmd_t *pmd, unsigned int flags,
		pte_t orig_pte)
{
	pgoff_t pgoff = (((address & PAGE_MASK) - vma->vm_start) >> PAGE_SHIFT) + vma->vm_pgoff;

	pte_unmap(page_table);
	if (!(flags & FAULT_FLAG_WRITE))
		return do_read_fault(mm, vma, address, pmd, pgoff, flags, orig_pte); // 只读异常
	if (!(vma->vm_flags & VM_SHARED))
		return do_cow_fault(mm, vma, address, pmd, pgoff, flags, orig_pte); // 写时复制异常

	return do_shared_fault(mm, vma, address, pmd, pgoff, flags, orig_pte);// 写缺页异常 
}
```
### 2.10.4 写时复制
* do_wp_page()
> 处理用户修改pte页表没有可写属性的页面，它新分配一个新页并复制旧页内容到新页

> 有两个大的分支，一个是需要创建新页的“写时复制”，另一个是“复用原页面”

* 哪些情况需要创建新页的写时复制Gotten？
> 不是normal mapping页面且不是可写的共享页面

> 是normal mapping且不是单身匿名页面

* 哪些情况是复用原来的页面Reuse？
> 是normal mapping页面, 是匿名页面且不是KSM，是单身匿名页面

> 是normal mapping页面，文件映射并且可写的共享页面

> 不是normal mapping页面，是可写共享页面

* Gotten: 写时复制
> 1、分配一个新页面new_page

> 2、设置new_page的pte页表项的Dirty/Write位

> 3、把new_page设置到原来的pte表项中

> 4、把new_page添加到rmap系统中

> 5、把new_page添加到活跃LRU链表中

> 6、释放oldpage

* Reuse: 复用原来页面
> 1、设置页面pte的Dirty/Write标志位

> 2、文件映射页面，设置PageDirty，调用系统回写机制回写一部分页面

### 2.10.5 小结
#### 缺页中断发生后，根据pte页表项中的PRESENT位、pte内容是否为空、是否文件映射等条件，相应处理函数如下
* 1、匿名页面缺页中断do_anonymous_page()
> (1)判断条件：pte页表项中PRESENT没有置位、pte内容为空且没有指定vma->vm_ops->fault()函数指针

> (2)应用场景：malloc()分配内存

* 2、文件映射缺页中断do_fault()
> (1)判断条件：pte页表项中的PRESENT没有置位，pte内容为空且指定了vma->vm_ops->fault()函数指针

> (2)应用场景：A、使用mmap读文件内容；B、动态库映射

* 3、swap缺页中断do_swap_page()
> 判断条件：pte页表项中的PRESENT没有置位且pte页表项内容不为空

* 4、写时复制COW缺页中断do_wp_page()
> 判断条件：pte页表项中的PRESENT置位且发生写错误缺页中断

> 应用场景：fork。父进程fork子进程，父子进程共享父进程的匿名页面，当父子中一方修改内容时，COW便会发生

* do_wp_page分两种情况处理
> reuse复用old_page：单身匿名页面和可写的共享页面

> gotten写时复制：非单身页面、只读或非共享的文件映射页面

## 2.11 Page引用计数
* 匿名页面与page cache页面有什么区别？

### 2.11.1 struct page数据结构
```c
// include/linux/mm_types.h
struct page {

};

// flags 标志位集合
// include/linux/page-flags.h
enum pageflags {

};
// 操作标志位的宏
#define TESTPAGEFLAG(uname, lname) static inline int Page##uname(const struct *page page) {return test_bit(PG_##lname, &page->flags);}
#define SETPAGEFLAG(uname, lname) static inline int SetPage##uname(const struct *page page) {return set_bit(PG_##lname, &page->flags);}
#define CLEARPAGEFLAG(uname, lname) static inline int ClearPage##uname(const struct *page page) {return clear_bit(PG_##lname, &page->flags);}
// flags除了存标志位外，还存放SECTION编号、NODE节点编号、ZONE编号和LAST_CPUPID等
```
### 2.11.2 _count和_mapcount的区别
* _count是内核中引用该页面的次数
> 加：get_page, 减：put_page

> 伙伴系统分配好的页面初始_count值为1

> _count == 0 说明页面已经释放了，如果put_page将_count减到0，则会立即调用__put_single_page()释放页面

* _count常规用法：
> 1、分配页面时_count引用计数会变成1

> 2、加入LRU链表时，page会被kswapd内核线程使用，因此_count加1

> 3、被映射到其他用户进程pte时，_count会加1

> 4、页面的private中有私有数据;PG_swapable页面：__add_to_swap_cache()人增加_count；PG_private页面：buffer_migrate_page()会增加_count;

> 5、内核对页面进行操作的关键路径上也会_count加1;如follow_page、get_user_pages

* _mapcount是页面被进程映射的个数, 也就是pte页表个数
> _mapcount主要用于RMAP反向映射机制

> _mapcount == -1表示没有pte映射到页面中

> _mapcount == 0表示只有父进程映射了页面；匿名页面刚分配时，_mapcount初始为0

> _mapcount > 0表示除了父进程外还有其他进程映射了这个页面

### 2.11.3 页面锁PG_Locked
* flags标志集合中的PG_Locked标志，内核常利用PG_Locked来设置一个页面锁
> 调用lock_page()函数申请页面锁, 如果页面锁被其他进程占用了，那么当前进程会睡眠等待

### 2.11.4 小结
* _count是page的命根子，因为当_count == 0时页面就会被释放
* _mapcount是page的幸福指数，因为它是进程映射页面的个数，被用的多说明更具有价值

## 2.12 反向映射RMAP
* struct page中的_mapcount记录有多少个用户PTE页表项映射了物理页面，不包括内核地址空间映射物理页面产生的PTE页表项
* 有的页面需要被迁移，有的页面长时间不使用需要交换到磁盘，在交换前，必须找出哪些进程使用这个页面，然后断开这些映射的PTE
* 一个物理页面可以被多个进程的虚拟内存映射
* 一个虚拟页面同时只能有一个物理页面与之映射
#### 反向映射的作用范围(我个人分析得出的结论)：
* 1.用户进程使用函数mmap建立的私有匿名映射、匿名共享映射
> 首先要明确有哪些操作会出现物理页面被多个虚拟页面映射的函数, 且是进程的PTE

> 其次，用什么数据结构来记录这种一个物理页面对多个虚拟页面(av, avc)

* 2.父进程创建子进程时

### 2.12.1 父进程分配匿名页面　
* 父进程分配什么样子的匿名页面？
> do_anonymous_page

* 关键数据结构 
```c
// av
struct anon_vma {
	struct anon_vma *root;
	struct rw_semaphore rwsem;
	atomic_t refcount;
	struct anon_vma *parent;
	struct rb_root rb_root;
};

// avc
struct anon_vma_chain {
	struct vm_area_struct *vma;
	struct anon_vma *anon_vma;
	struct list_head same_vma;
	struct rb_node rb;
	unsigned long rb_subtree_last;
};

struct rmap_walk_control {                                                                                                                       
    	void *arg;
      	int (*rmap_one)(struct page *page, struct vm_area_struct *vma,
                                         unsigned long addr, void *arg);
        int (*done)(struct page *page);
        struct anon_vma *(*anon_lock)(struct page *page);
        bool (*invalid_vma)(struct vm_area_struct *vma, void *arg);
};
// 上面的rmap_walk_control配合函数rmap_walk一起用，rmap_walk会遍历所有找到的avc并执行rmap_one
```

* 关键函数
> page_add_new_anon_rmap


### 2.12.2 父进程创建子进程
* 父进程通过fork调用创建子进程时，子进程会
> 复制父进程的进程地址空间VMA数据结构的内容作为自己的进程地址空间

> 复制父进程的pte页表项内容到子进程的页表中，实现父子进程共享页表

* 多个不同子进程中的虚拟页面会同时映射到同一个物理页面

* 多个不相干的进程的虚拟页面也可以通过ksm机制映射到同一个物理页面

### 2.12.3 子进程发生COW
* 子进程和父进程共享的匿名页面，在子进程vma发生cow时
```
产生缺页中断->handle_pte_fault->do_wp_page->分配一个新的匿名页面->__page_set_anon_rmap使用子进程的anon_vma来设置page->mapping
```

### 2.12.4 RMAP应用
* 典型应用
> 1.kswapd内核线程回收页面需要断开所有映射了该匿名页面的用户PTE页表项

> 2.页面迁移时，需要断开所有映射到匿名页面的用户PTE页表项

* 反向映射的核心函数try_to_unmap
> mm/rmap.c

* 内核中有三种页面需要unmap
> 1. KSM页面

> 2. 匿名页面

> 3. 文件映射页面

### 2.12.5 小结　

## 2.13 回收页面
### 2.13.1 LRU链表
* 内核中一共有五种LRU链表
> 不活跃匿名页面链表（LRU_INACTIVE_ANON）

> 活跃匿名页面链表（LRU_ACTIVE_ANON）

> 不活跃文件映射页面链表（LRU_INACTIVE_FILE）

> 活跃文件映射页面链表（LRU_ACTIVE_FILE）

> 不可回收页面链表（LRU_UNEVICTABLE）

* LRU链表这样分，是因为当内存紧张时总是优先换出page cache页面，而不是匿名页面
> 因为大多数情况下, page cache页面不需要回写磁盘

> 而匿名页面必须写入交换区才能被换出

* 每个zone都有一整套LRU链表
> zone结构中成员lruvec指向这些LRU链表

#### page_check_references()函数作用
* 1.如果有访问引用pte，那么
> 该页面是匿名页面，则加入活跃链表

> 最近第二次访问的page cache或shared page cache，则加入活跃链表

> 可执行文件的page cache，加入活跃链表

> 除上述三种情况外，继续留在不活跃链表，例如第一次访问的page cache

* 2.如果没有访问引用pte，则表示可以尝试回收它

#### ARM32 Linux内核实现了两套页表，一套给linux内核，一套给ARM硬件

### 2.13.2 kswapd内核线程
* 负责在内存不足时回收页面
* 系统启动时会在kswapd_try_to_sleep()函数中睡眠并让出CPU控制权。当系统内存紧张时，例如alloc_pages在低水位中无法分配出内存，这时分配内存函数会调用wakeup_kswapd()来唤醒kswapd内核线程。kswapd内核线程被唤醒后，调用balance_pgdat()来回收页面

### 2.13.3 balance_pgdat函数
### 2.13.4 shrink_zone函数
* 扫描zone中所有可回收页面

### 2.13.5 shrink_active_list函数
* 从活跃链表加入不活跃链表

### 2.13.6 shrink_inactive_list函数
* 把不活跃链表中的页面加入到活跃链表
* 从不活跃链表中回收页面　

### 2.13.7 跟踪LRU活动情况
### 2.13.8 Refault Distance算法
* T2 - T1
> 第二次读的时间减第一次踢出的时间= Refault Distance

### 2.13.9 小结
* Linux4.0内核的页面回收代码基于zone的LRU扫描策略

* 页面回收流程
> 1、从空闲页面添加到LRU链表（这里是说，页面回收前需要先分配哈，新分配的匿名页面需要加入到活跃LRU）

|1)匿名页面|比如do_anonymous_page()=>加入活跃LRU|
|:-|:-|
|2) page cache|文件读或者mmap读=>加入不活跃LRU|
|3) 共享内存||

> 2、从活跃链表加入不活跃链表shrink_active_list()

|1)page_referenced()返回该页是否被引用？并清除每个引用PTE的YOUNG bit|
|:-|
|2) 有被引用的可执行的page cache页面加入活跃LRU，其他都加入不活跃LRU|

> 3、从不活跃链表加入活跃链表shrink_inactive_list()

|1) 有被引用的匿名页面|
|:-|
|2) 有被引用的可执行的page cache|
|3) 访问2次的page cache|
|4) 分配swap空间不成功的匿名页面|
|5) try_to_unmap()/pageout()/try_to_release_page()失败的页面|

> 4、继续待在不活跃LRU

|1) 已经被别的进程加锁的页|
|:-|
|2) 正在回写的页|
|3) 没有被引用的页|
|4) 只访问了一次的page cache|
|5) 脏的page cache|

> 5、从不活跃LRU中回收页面shrink_inactive_list()

|1) try_to_unmap()通过反向映射来解除每个引用pte映射|
|:-|
|2) pageout()把脏页/匿名页面写入存储设备|
|3) 释放BH buffer|
|4) 释放页|


* kswapd内核线程何时会被唤醒？
> 答：分配内存时，当在zone的WMARK_LOW水位分配失败时，会支唤醒kswapd内核线程来回收页面

* LRU链表如何知道page的活动频繁程序？
> 答：LRU链表按照先进先出的逻辑，页面先进入LRU链表头，然后慢慢挪动到链表尾，有一个老化过程。另外，page中有PG_reference/PG_active标志位和页表的PTE_YOUNG位来实现第二次机会法

* kswapd按照什么原则来换出页面？
> 答：页面在活跃LRU链表，需要从链表头到链表尾的一个老化过程才能迁移到不活跃LRU链表。在不活跃LRU链表中又经过一个老化过程，首先被剔除那些脏页面或正在回写的页面，然后那些在不活跃LRU链表老化过程中没有被访问引用的页面是最佳的被换出候选者，具体看shrink_page_list()函数

* kswapd按照什么方向扫描zone？
> 答：从低zone到高zone，和分配页面的方向相反

* kswapd以什么标准退出扫描LRU？
> 答：判断当前内存节点是否处于“生态平衡”，详见pgdat_balanced()函数

* 手持设备（例如Android系统）没有swap分区，kswapd会扫描匿名页面LRU吗？
> 答：没有swap分区不会扫描匿名页面LRU链表，详见get_scan_count()函数

* swappiness的含义是什么？kswapd如何计算匿名页面和page cache之间的扫描比重？
> 答：swappiness用于设置swap分区写页面的活跃程序，详见get_scan_count()

* 当秕中充斥着大量只访问一次的文件访问时，kswapd如何来规避这种风暴？
> 答：page_check_reference()函数设计了一个简易的过滤那些短时间只访问一次的page cache的过滤器，详见page_check_reference()

* 在回收page cache时，对于dirty的page cache ，kswapd会马上回写吗？
> 答：不会，详见shrink_page_list()函数

* 内核中有哪些页面会被kswapd写到交换分区？
> 答：匿名页面，还有一种特殊情况，是利用shmem机制建立的文件映射，其实也是使用的匿名页面，在内存紧张时，这种页面也会被swap到交换分区

## 2.14 匿名页面生命周期
### 2.14.1 匿名页面的诞生
* 匿名页面的诞生
> page->_count = 1;

> page->_mapcount = 0;

> page->flags = PG_lru | PG_swapbacked;

> 加入活跃lru

> page->mapping = anon_vma;

### 2.14.2 匿名页面的使用
### 2.14.3 匿名页面的换出
* 扫描活跃链表，加入到不活跃链表add_to_swap
> page->_count=3

> page->_mapcount=0

> page->flags=PG_lru|PG_swapbacked|PG_swapcache|PG_dirty|PG_uptodate|PG_locked

* try_to_unmap()
> page->_count=2

> page->_mapcount=-1

> page->flags=PG_lru|PG_swapbacked|PG_swapcache|PG_dirty|PG_uptodate|PG_locked

* page_out()
> page->_count=2

> page->_mapcount=-1

> page->flags=PG_lru|PG_swapbacked|PG_swapcache|PG_uptodate|PG_reclaim|PG_writeback;

* 第x次扫描不活跃链表(x>=2, 假设在第x次扫描不活跃链表时页面写入swap分区完成)
> Block layer层的回调函数end_swap_bio_write()->end_page_writeback()完成：1清PG_writeback标志位、2唤醒等待在该页PG_writeback的线程，wake_up_page

> shrink_page_list()->__remove_mapping()：判断_count是否为2，并将_count置0、清PG_swapcache,PG_locked

> 最后把page加入free_page链表中，释放该页。到这里，该页的内容已经写入swap分区，它的物理页面已经释放

* 所谓的物理页面已经释放，其实指就是将该通过伙伴系统将该页回收到zone的free_area中free_list？my answer is right.

### 2.14.4 匿名页面的换入
* 匿名页面被换出到swap分区后，如果应用程序需要读写这个页面，缺页中断发生，因为pte中的present位为0显示该页面不在内存中，但pte表项不为空，说明该页在swap分区中，因此调用do_swap_page()函数重新读入该页内容

### 2.14.5 匿名页面的销毁
* 当用户进程关闭或退出时，内核会扫描这个用户进程所有的vma，并会清除这些vma上所有的映射，如果符合释放标准，相关页面会被释放

### 2.14.6 小结

## 2.15 页面迁移
* migrate_pages()函数
> mm/migrate.c

> 调用unmap_and_move一个页面一个页面的迁移, 返回MIGRATEPAGE_SUCCESS表示页面迁移成功

> MIGRATE_ASYNC, 它是枚举类型的迁移模式，它表示异步迁移

> PF_MEMALLOC, 当前进程的标志位，表示可能是在直接内存压缩内核路径上，不可以睡眠等待页面锁（不安全）

> 页面锁PG_locked

> wait_on_page_writeback()

> move_to_new_page()

> page_mapping(), 匿名页面、KSM页面、slab页面它返回NULL-------4.4.24内核源码

> migrage_page()

> remove_migration_ptes(), 利用反向映射机制找到映射旧页面的每一个pte，之后迁移页面的每一个pte

> rmap_walk会对每一个找到的avc执行rmap_one函数指令指向的函数

> set_pte_at()

* 为什么不可以在直接内存压缩内核路径上睡眠等待页面锁？
> 举例，在文件预读时，预读的所有页面都会加页面锁，并添加到LRU链表。等预读完成后，这些页面会标记PG_uptodate并释放页面锁，这个过程中块设备会把多个页面合并到一个BIO中。如果分配第2和第3个页面时发生内存短缺，内核会运行到直接内存压缩内核路径上，导致一个页面已经加了页面锁又去等待这个页面锁，产生死锁。因此，在直接压缩内存的内核路径上标记PF_MEMALLOC

* PF_MEMALLOC标志位一般是在直接内存压缩、直接内存回收和kswapd中设置，这些场景下也可能会有少量的内存分配行为，设置PF_MEMALLOC表示允许它们使用系统预留的内存，即不考虑水位值
> __perform_reclaim()

> __alloc_pages_direct_compact()

> kswapd()

## 2.16 内存规整（memory compaction）
* 内核使用的页面是不可迁移的，且实现迁移的难度和复杂度大，因此内核本身的物理页面不作迁移

* 用户进程使用的页面，是通过用户页表映射来访问，用户页表可以移动和修改映射关系，不会影响用户进程

* 内存规整基于页面迁移实现

### 2.16.1 内存规整实现
* 内存规整的一个重要应用场景：在分配大块内存时（order > 1），在WMARK_LOW低水位情况下分配失败，唤醒kswapd内核线程后依然无法分配出内存，这时使用__alloc_pages_direct_compact()来压缩内存尝试分配所需内存
> alloc_pages->__alloc_pages_nodemask->__alloc_pages_slowpath->alloc_pages_direct_compact

```c
static struct page * __alloc_pages_direct_compact(gfp_t gfp_mask, unsigned int order,
              			int alloc_flags, const struct alloc_context *ac,                                                                                		enum migrate_mode mode, int *contended_compaction,
                 		bool *deferred_compaction)
{
        unsigned long compact_result;
   	struct page *page;

        if (!order)
        	return NULL;
 
        current->flags |= PF_MEMALLOC;
  	compact_result = try_to_compact_pages(gfp_mask, order, alloc_flags, ac,
                                                 mode, contended_compaction);
    	current->flags &= ~PF_MEMALLOC;

  	switch (compact_result) {
    	case COMPACT_DEFERRED:
             	*deferred_compaction = true;
      	/* fall-through */
        case COMPACT_SKIPPED:
                 return NULL;
	default:
                 break;
     	}
	count_vm_event(COMPACTSTALL);
	// 上面执行完内存规整后，会调用get_page_from_freelist来尝试分配内存，如果分配成功则返回page
	page = get_page_from_freelist(gfp_mask, order, alloc_flags & ~ALLOC_NO_WATERMARKS, ac);
	...
}

unsigned long try_to_compact_pages(gfp_t gfp_mask, unsigned int order,int alloc_flags, const struct alloc_context *ac,
		enum migrate_mode mode, int *contended)
{
	for_each_zone_zonelist_nodemask(zone, z, ac->zonelist, ac->high_zoneidx,ac->nodemask) {
		int status;
		int zone_contended;

		if (compaction_deferred(zone, order))
			continue;
		status = compact_zone_order(zone, order, gfp_mask, mode,&zone_contended, alloc_flags,ac->classzone_idx);
		...
		if (zone_watermark_ok(zone, order, low_wmark_pages(zone),ac->classzone_idx, alloc_flags)) {
		}
	}
}

static unsigned long compact_zone_order(struct zone *zone, int order, gfp_t gfp_mask, enum migrate_mode mode, int *contended, int alloc_flags,
		int classzone_idx)
{
	unsigned long ret;
	struct compact_control cc = {
		.nr_freepages = 0,
		.nr_migratepages = 0,
		.gfp_mask = gfp_mask,
		.zone = zone,
		.mode = mode,
		.alloc_flags = alloc_flags,
		.classzone_idx = classzone_idx,
	};
	INIT_LIST_HEAD(&cc.freepages);
	INIT_LIST_HEAD(&cc.migratepages);

	ret = compact_zone(zone, &cc);
}
compact_zone->compaction_suitable
	      compact_finished
	      isolate_migratepages()函数用于扫描和查找合适迁移的页面，从zone的头部开始找
	      migrate_pages()迁移页的核心函数
```

* 不适合内存规整迁移的页面总结如下
> 必须在LRU链表中的页面，还在伙伴系统中的页面不适合

> 正在回写的页面不适合

> 标记有PG_unevictable页面不适合

> 没有定义mapping->a_ops->migratepage()方法的脏页面不适合

## 2.17 KSM
* KSM（Kernel SamePage Merging）,用于合并内容相同的页面
> KSM的出现是为了优化虚拟化中产生的冗余页面。因为虚拟化的实际应用中在同一台宿主机上会有许多相同的操作系统和应用程序，那么许多内存页面的内容有可能是相同的，因此它们可以被合并，从而释放内存供其他程序使用

> ksm允许合并同一个进程或不同进程之间的内容相同的匿名页面；

> ksm把内容相同的页面合并成只读的页面，从而释放出物理页面，当应用程序需要改变页面内容时，会发生写时复制(copy-on-write, COW)

### 2.17.1 KSM实现
* ksm在初始化时会创建一个ksmd内核线程

* KSM只会处理通过madvise系统调用显式指定的用户进程空间内存
> 用户程序想使用KSM，那么在分配内存时必须显式调用madvise(addr, length, MADV_MERGEABLE) 

> 用户想在KSM中取消一个用户进程地址空间的合并功能必须调用madvise(addr, length, MADV_UNMERGEABLE)
	
### 2.17.2 匿名页面和KSM页面的区别
* 使用宏PageAnon()和PageKsm()区分
* Ksm最早是为了KVM虚拟机设计的
* 一个典型的应用程序可以由以下五个内存组成
> 1.可执行文件的内存映射

> 2.程序分配使用的匿名页面

> 3.进程打开的文件映射

> 4.进程访问文件系统产生的cache

> 5.进程访问内核产生的内核buffer

## 2.18 Dirty COW内存漏洞
* 实验代码book/runlinuxkernel/code/2/2.18

## 2.19 总结内存管理数据结构和API
### 2.19.1 内存管理数据结构关系图
```
 ____         ________
|_MM_|       |__vaddr_|
   ^              ^            ___________
    \            /            |__pg_data__|
  (1)\      (2) /                   /
      \        /               (8) /
       \      /                   /
      __V___ V___            ____V____
     |___VMA_____|          |__zone___|
          ^		       ^
       (3)|               (7) /
          |                  /           
     _____V________________</     ___________
    /    page     \<_____________|	     |
    \_____________/      (9)     | mem_map[] |
        ^    ^                   |___________|
   (4) /      \ (6)
   ___V___   __V____		
  |_pfn___| |__pte__|
     ^
  (5)|
  ___V___
 |_paddr_|

```

* (1)用mm和虚拟地址vaddr找对应的vma
```c
find_vma
find_vma_prev
find_vma_intersection
```

* (2)用page和vma找虚拟地址vaddr
```c
mm/rmap.c

// 针对匿名页面
vma_address
=> pgoff = page->index;
=> vaddr = vma->vm_start + ((pgoff - vma->vm_pgoff) << PAGE_SHIFT);
```

* (3)用page找到所有映射的vma
```c
// 通过反向映射rmap系统来实现rmap_walk()
// 对于匿名页面来说：
=> 由page->mapping找到anon_vma数据结构
=> 遍历anon_vma->rb_root红黑树，取出avc数据结构
=> 每个avc数据结构中指向每个映射的vma
```

* 用vma和虚拟地址找相应的page
```c
include/linux/mm.h

follow_page
=> 由虚拟地址vaddr通过查询页表找出pte
=> 由pte找出页帧号pfn, 然后在mem_map[]找到相应的page
```

* (4)page和pfn之间互换
```c
include/asm-generic/memory_model.h
// 由page到pfn:
page_to_pfn()

// 由pfn到page
__pfn_to_page(pfn)

```

* (5)pfn和paddr之间互换
```c
arch/arm/include/asm/memory.h

// 由paddr到pfn
__phys_to_pfn(paddr)

// 由pfn到paddr
__pfn_to_phys(pfn)
```

* (6)page和pte之间互换
```c
// page到pte
=> 先由page到pfn
=> 然后由pfn到pte

// 由pte到page
pte_page(pte)
```

* (7)zone和page之间互换
```
// 由zone到page
zone数据结构有zone->start_pfn指向zone起始的页面，然后由pfn找到page数据结构

// page到zone
page_zone()函数返回page所属的zone
```

* (8)zone和pg_data互换
```
// pd_data到zone
pd_data_t->node_zones

// zone到pd_data
zone->zone_pgdat
```

### 2.19.2 内存管理中常用api
* 1.页表相关
> 查询页表

> 判断页表项的状态

> 修改页表

> page和pfn的关系

* 2.内存分配
> 分配和释放页面

> slab分配器

> vmalloc相关

* 3.VMA操作相关
* 4.页面相关
> PG_XXX标志位操作
```c
PageXXX()
SetPageXXX()
ClearPageXXX()
TestSetPageXXX()
TestClearPageXXX()
void lock_page(struct page *page);
int trylock_page(struct page *page);
void wait_on_page_bit(struct page *page, int bit_nr);
void wake_up_page(struct page *page, int bit);
static inline void wait_on_page_locked(struct page *page);
static inline void wait_on_page_writeback(struct page *page);
```

> page引用计数操作
```c
void get_page(struct page *page);
void put_page(struct page *page);
#define page_cache_get(page)	get_page(page)
#define page_cache_release(page)	put_page(page)
static inline int page_count(struct page *page);
static inline int page_mapcount(struct page *page);
static inline int page_mapped(struct page *page);
static inline int put_page_testzero(struct page *page);

```

> 匿名页面和KSM页面
```c
static inline int PageAnon(struct page *page);
static inline int PageKsm(struct page *page);
struct address_space *page_mapping(struct page *page);
void page_add_new_anon_rmap(struct page *page, struct vm_area_struct *vma, unsigned long address);
```

> 页面操作
```c
struct page *follow_page(struct vm_area_struct *vma, unsigned long address, unsigned int foll_flags);

struct page *vm_normal_page(struct vm_area_struct *vma, unsigned long addr, pte_t pte);

long get_user_pages(struct task_struct *tsk, struct mm_struct *mm, unsigned long start, unsigned long nr_pages, int write, int force, struct page **pages, struct vm_area_struct **vmas);
```

> 页面映射
```c
void create_mapping_late(phys_addr_t phys, unsigned long virt, phys_addr_t size, pgprot_t prot);

unsigned long do_mmap_pgoff(struct file *file, unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long pgoff, unsigned long *populate);

int remap_pfn_range(struct vm_area_struct *vma, unsigned long addr, unsigned long pfn, unsigned long size, pgprot_t prot);
```

> 缺页中断 
```c
int do_page_fault(unsigned long addr, unsigned int fsr, struct pt_regs *regs);
int handle_pte_fault(struct mm_struct *mm, struct vm_area_struct *vma, unsigned long address, pte_t *pte, pmd_t *pmd, unsigned int flags);
static int do_anonymous_page(struct mm_struct *mm, struct vm_area_struct *vma, unsigned long address, pte_t *page_table, pmd_t *pmd, unsigned int flags);

static int do_wp_page(struct mm_struct *mm, struct vm_area_struct *vma, unsigned long address, pte_t *page_table, pmd_t *pmd, spinlock_t *ptl, pte_t orig_pte);

```

> LRU和页面回收
```c
void lru_cache_add(struct page *page);

#define lru_to_page(_head) (list_entry((_head)->prev, struct page, lru))

bool zone_watermark_ok(struct zone *z, unsigned int order, unsigned long mark, int classzone_idx, int alloc_flags);
```
## 2.20 最新更新和展望
### 2.20.1 页面回收策略从zone迁移到node
### 2.20.2 OOM Killer改进
### 2.20.3 swap优化
### 2.20.4 展望

# 第3章 进程管理
## 3.1 进程的诞生
```c
int main(void)
{
	int i;
	for (i = 0; i < 2; i++) {
		fork();
		printf("-\n");
	}
	wait(NULL);
	wait(NULL);
	return 0;
}
// 问：上述代码输出几个-？
```
* 线程和进程的区别：进程拥有独立的资源空间，而线程共享进程的资源空间

* Linux内核并没有对线程有特别的调度算法或定义特别的数据结构来标识线程，线程和进程都使用相同的PCB（进程控制块，Processing control block）数据结构

* 内核里使用clone创建线程，其工作方式和创建进程fork方法类似，但区分哪些资源和父进程共享，哪些资源为线程独占

* fork系统调用是所有进程的孵化器（idle进程除外）

### 3.1.1 init进程
* linux内核在启动时会有一个init_task进程，它是系统中所有进程的鼻祖，称为0号进程或idle进程或swapper进程
> 当系统没有进程需要调度时，调度器就地支执行idle进程

> idle进程在内核启动时(start_kernel()函数)静态创建，所有核心的数据结构都是预先静态赋值

> init_task进程的task_struct数据实例通过INIT_TASK宏来赋值

```c
// include/linux/init_task.h

#define INIT_TASK(tsk)   \
{
	.state		= 0,	\
	.stack		= &init_thread_info,	\
	...
	.flags		= PF_KTHREAD,	\
	...
	.policy		= SCHED_NORMAL,		\
	...
	.mm		= NULL,	\
	.active_mm	= &init_mm,	\
	...
}
/* 1.stack指向thread_info
 * 2.通常内核栈大小是8KB（ARM32是8KB, 这与体系结构相关, ARM64是16KB）, 存放在内核映像文件的data段中，在编译链接时预先分配好，见arch/arm/kerenl/vmlinux.lds.S链接文件
 * 
 */
```
* ARM32处理器从汇编代码跳转到start_kernel()函数之前，设置了寄存器SP为指向8KB的内核栈顶部区域（需预留8字节空洞） 
```
// arch/arm/kernel/head-common.S
__mmap_switched:
	adr	r3, __mmap_switched_data
	...
	ARM(ldmia	r3!, {r4, r5, r6, r7, sp})
	...
	b	start_kernel
ENDPROC(__mmap_switched)
```

* current用于获取当前进程的task_struct实例，它利用了内核栈的特性
> 首先通过SP寄存器得到当前内核栈的地址，对齐后取到struct thread_info指针，再通过thread_info->task得到task_struct实例

```c
// arch/arm/include/asm/thread_info.h

struct thread_info {
	unsigned long	flags;
	int 	preempt_count;
	...
	struct task_struct *task;
	...
	__u32		cpu;
	...
	struct cpu_context_save cpu_context;
	...
};
```
### 3.1.2 fork
* 进程或线程是通过fork、vfork、clone等系统调用来建立的
> 在内核中这三个系统调用都是通过同一个函数do_fork()来实现的, 定义在kernel/fork.c

```c
long do_fork(unsigned long clone_flags, unsigned long stack_start, unsigned long stack_size, int __user *parent_tidptr, int __user *child_tidptr);

// clone_flags: 创建进程的标志位集合, 定义在include/uapi/linux/sched.h
// stack_start: 用户态栈的起始地址
// stack_size: 用户态栈的大小，通常设置0
// parent_tidptr, child_tidptr: 指向用户空间中地址的两个指针，分别指向父子进程的PID

// fork的实现：
	do_fork(SIGCHLD, 0, 0, NULL, NULL);
// vfork的实现：
	do_fork(CLONE_VFORK | CLONE_VM | SIGCHLD, 0, 0, NULL, NULL);
// clone的实现:
	do_fork(clone_flags, newsp, 0, parent_tidptr, child_tidptr);
// 内核线程：
	do_fork(flags | CLONE_VM | CLONE_UNTRACED, (unsigned long)fn, (unsigned long)arg, NULL, NULL);

```

* 关键路径
```c
fork->do_fork->copy_process->dup_task_struct->arch_dup_task_struct
         				      setup_thread_stack
         		     sched_fork->__sched_fork
         		                 set_task_cpu
         		     copy_thread
         		     copy_files, copy_fs, copy_io
         		     copy_mm->dup_mm->mm_init->mm_alloc_pgd->__pgd_alloc
         		                                             pud_alloc
         		         	     		             pmd_alloc
         		         				     pte_alloc_map
         		         				     set_pte_ext
         		         	      dup_mmap->anon_vma_fork
         		         	     	        __vma_link_rb
         		         		        copy_page_range->copy_pud_range->copy_pmd_range->copy_pte_range->copy_one_pte
         		     task_pt_regs
	       wake_up_new_task
         
// dup_mmap函数的主要作用是遍历父进程中所有的VMA，然后复制父进程VMA中对应的pte页表项到子进程相应的VMA对应的pte中，但只是复制pte页表项，并没有复制vma对应的页面内容					       

// copy_page_range函数复制父进程VMA的页表到子进程页表中
//
// wake_up_new_task准备唤醒新创建的进程，也就是把进程加入到调度器里接受调度运行

// init_struct_pid是init_task进程的默认配置，新进程需要重新分配pid实例（alloc_pid初始化pid结构，pid_nr获取新进程真正的pid号）

// vfork创建的子进程，首先要保证子进程先运行
```

* copy_mm函数
> 如果current->mm ==  NULL 则表示进程没有自己的运行空间，只是一个“寄人篱下”的线程或内核线程 

> 如果要创建一个和父进程共享内存空间的新进程，那么直接将新进程的mm指针指向父进程的mm即可

* dup_mm函数分配一个mm数据结构, 并从父进程中复制相关内容

* mm_struct的几个相关成员
> mm_rb

> mm_users, 表示在用户空间的用户个数

> mm_count表示内核中引用了该数据结构的个数

> mmap_sem, 用于保护进程地址空间的读写信号量 

> page_table_lock用于保护进程页表的spinlock锁

* 对于ARM体系结构，Linux内核栈顶存放着ARM的通用寄存器, 在代码中使用struct pt_regs表示
> 如果新进程不是内核线程，那么将父进程的寄存器值复制到子进程中, thread_info.cpu_context成员中保存着进程上下文相关的通用寄存器

```c
#define task_pt_regs(p) \
	((struct pt_regs*)(THREAD_START_SP + task_stack_page(p)) - 1)

struct pt_regs {
	unsigned long uregs[18];
};
```

### 3.1.3 小结

## 3.2 CFS 调度器
* 三种类型的进程：交互式进程、批处理进程、实时进程
> 交互式进程：与人机交互的进程，和鼠标、键盘、触摸屏等相关的应用，如vim, 它一直在睡眠等待用户召唤它，这类进程的特点是系统响应时间越快越好，否则用户就会抱怨系统卡

> 批处理进程：此类进程默默地工作和付出，可能会占用比较多的系统资源，例如编译代码

> 实时进程：有些应用对整体时延有严格要求，例如现在很火的VR设备，从头部转动到视频显示需要控制到19毫秒以内，否则会使人出现眩晕感；对于工业控制系统，不符合要求的时延可能会导致严重的事故

* O(N)调度器
> 发布于1992年

> 该调度器算法：从就绪队列中比较所有进程的优先级，然后选择一个最高优先级的进程作为下一个调度进程。每个进程有一个固定时间片，当进程时间片使用完之后，调度器会选择下一个调度进程，当所有进程都运行一遍后再重新分配时间片。这个调度器选择下一个调度进程前需要遍历整体就绪队列，花费O(n)时间

#### 目前linux内核实现了4种调度策略, 这样可以让不同的进程采用不同的调度策略
> 这些调度策略可以使用struct sched_class来定义调度类

> 这4种调度类通过next指针串联在一起，用户空间程序可以使用调度策略API（sched_setscheduler()函数）来设定用户进程的调度策略 

* deadline

* realtime

* CFS

* idle

```c
// include/uapi/linux/sched.h

#define SCHED_NORMAL	0
#define SCHED_FIFO	1
#define	SCHED_RR	2
#define	SCHED_BATCH	3
//      SCHED_ISO	4  保留但未实现
#define SCHED_IDLE	5
#define	SCHED_DEADLINE	6
```

### 3.2.1 权重计算
* 内核使用0～139表示进程的优先级，数值越低优先级越高 
> 0 ~ 99给实时进程使用

> 100 ~ 139给普通进程使用

> 用户空间有一个变量nice值（取值范围-20～19, 默认0）映射到普通进程的优先级(100 ~ 139)

> pcb中有3个成员用于描述进程优先级 

```c
struct task_struct {
	...
	int prio;
	int static_prio;
	int normal_prio;
	unsigned int rt_priority;
	...
	struct sched_entity se; // 调度实体
	...
};
/* static_prio: 它是静态优先级，在程序启动时分配，内核不存储nice值，使用static_prio代替。内核宏NICE_TO_PRIO()实现由nice转换成static_prio; 
 *  		它不会随时间改变，用户可通过系统调用nice、sched_setscheduler修改该值
 *
 * normal_prio: 普通进程时它与static_prio相等，实时进程时会根据rt_priority计算 
 * prio: 保存进程的动态优先级，是调度类考虑的优先级
 * rt_priority: 是实时进程优先级
 */
```

#### 调度实体权重
* 调度实体
```c
// include/linux/sched.h

struct sched_entity {
	struct load_weight 	load;
	...
	struct sched_avg	avg; //  多核CPU时有这个, 用于描述进程的负载, 我参考的内核版本(4.4.24)与书不同，书中此结构中有runnable_avg_sum
	// 书中用的应当是4.0.x版的，今天下载一版4.0.5看看, 有!
};
```

* 内核使用struct load_weight结构来记录调度实体的权重
```c

struct load_weight {
	unsigned long weight;
	u32 inv_weight;
};
```

* 为了计算方便内核约定nice值为0的权重值为1024，其他nice值对应的权重值通过查表prio_to_weight[40]来获取
> 另外还提供了一个表prio_to_wmult[40]

* prio_to_wmult[]表的计算公式如下: inv_weight = 2^32 / weight;
> 其中inv_weight是指权重被倒转了，作用是为后面计算方便, 内核提供函数set_load_weight()来查询这两个表，然后把值存在p->se.load结构中

> prio_to_wmult[]表预先做了除法，因此实际的vruntime计算只有乘法和移位运算vruntime = (delta_exec * nice_0_wieght * inv_weight) >> shift;

* CFS调度器的虚拟时间vruntime = delta_exec * nice_0_weight / weight
> vruntime表示进程虚拟的运行时间

> delta_exec表示实际运行时间

> nice_0_weight表示nice为0的权重值

> weight表示该进程的权重值

> CFS调度器总是选择虚拟时间跑得慢的进程 ; 这有什么意义?本质上不还是与O(n)一样选择优先级高的吗?

* calc_delta_fair()
* CFS调度器抛弃以前固定的时间片和固定的调度周期算法，采用进程权重值的比重来量化和计算实际时间片（运行时间）
> 引入虚拟时钟概念，每个进程的虚拟时间是实际运行时间相对nice值为0的权重的比例值

> nice值越小的进程，优先级高权重大，对应的虚拟时间比实际的时钟跑得慢，因此可以得到更多的运行时间

> nice值越大的进程，优先级低权重小，对应的虚拟时间比实际的时钟跑得快，因此得到的运行时间更少

* 权重是与优先级nice值一一对应的，只有40个，其值固定, 由表prio_to_weight[]直接查出

#### CPU的负载计算
* 一个调度实体的平均负载和以下3个因素相关
> 调度实体的权重值weight

> 调度实体的可运行状态下的总衰减累计时间runnable_avg_sum

> 调度实体在调度器中的总衰减累计时间runnable_avg_period

* runnable_avg_sum越接近runnable_avg_period，则平均负载越大，表示该调度实体一直在占用CPU

### 3.2.2 进程创建














