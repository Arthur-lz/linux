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
* ARM Vexpress 平台打印的内核内存空间布局图如下
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

* ARM64架构的Linux内核的内存分布图如下　
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
   2. 之后遍历zone的空闲区中指定迁移类型的空闲链表，从与order相等的空闲区开始寻找(current_order = order)，如果第一个等于order的空闲区中对应迁移类型的空闲链表没有空闲对象，则++current_order，进入下一次循环，找更大的空闲区对应迁移类型的空闲链表，如果空闲链表里有空闲对象，那么就把这个空闲块从链表上取下来，之后用expand来拆分内存块（当然，如果找到的内存区的order与目标相同就不拆分了，直接从链表上取下来返回，这在expand里会将找的这个页块拆开，结束拆分的条件是当前分配的页块的current_order不大于申请的order）
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
	unsigned int batchcount; // 当本地CPU缓冲池为空时，从共享缓冲池或slabs_partialslabs_free中获取的对象数目;由enable_cpucache计算
	unsigned int limit; // 当本地缓冲池中空闲对象个数大于limit时，主动释放batchcount个对象，便于内核回收或销毁slab;在enable_cpucache中计算
	unsigned int shared; // 多核系统中shared大于0, 在enable_cpucache()中初始化
	unsigned int size; // 对象长度，需要根据align调整
	...
	unsigned int flags; //对象分配掩码
	unsigned int num; // 一个slab中最多可以有的对象数

	unsinged int gfproder; // 一个slab中占用2^gfporder个页面
	...
	size_t colour; // 一个slab中有几个不同的cache line, 即，包含的cache line数量
	unsinged int colour_off; // 一上cache colour的长度
	...

	unsigned int freelist_size;
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

// struct kmem_cache_node定义在mm/slab.h中
struct kmem_cache_node {
	spinlock_t list_lock;
	struct list_head slabs_partial; //部分空闲， 链表中成员是slab
	struct list_head slabs_full;    //完全用尽
	struct list_head slabs_free;	//空闲　
	unsigned long free_objects; // 三个链表中所有空闲对象数目
	unsigned int free_limit; // slab中可容许的空闲对象数目最大阈值
	unsigned int colour_next;
	struct array_cache *shared; //多核CPU中，除了本地CPU的缓冲池外，其余CPU有一个共享的对象缓冲池
	...
};
 
```

* 关键函数
> kmem_cache_create/destroy, 创建自定义的slab描述符;kmalloc用于创建通用的缓存

> kmem_cache_alloc/free

> calculate_slab_order, 计算一个slab需要多少个物理页面

> cache_estimate, 计算了slab中可容纳多少个对象

> cache_line_size, 计算L1 cache line大小

> enable_cpucache, 计算shared, limit, batchcount

> alloc_kmem_cache_cpus, 分配每CPU对象缓冲池

* 新创建的slab描述符会添加到全局链表slab_caches中
* 一个slab最多可以包含KMALLOC_MAX_ORDER（25）个页面，2^25 = 32MB
> 见include/linux/slab.h

### 2.5.2 分配slab对象　






