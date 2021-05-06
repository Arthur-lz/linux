# linux 开发一步一步来

### memmove
```c
void *memmove(void *dest, void *source, sizt_t n)
{
	if((dest == NULL)||(source == NULL))
		return NULL;

	int count = n;
	const char *s = source;
	char *d = dest;

	if(dest <= source){ // dest的高地址有可能与source重叠, 所以先将source低地址值复制给dest
		while(count--){
			*d++=*s++;	
		}	
	}else{ // dest的尾有可能与source重叠, 所以先将source高地址的数据复制给dest
		d+=n;
		s+=n;
		while(count--){
			*--d=*--s;
		}
	}
	return dest;
}
```
### memcpy
```c
```
---
### 下面四个中哪一个不是表示数组char A[10]的A[1]的地址？
* s+1
* s++
* &s[0]+1
* &s[1]
---
### float x; x = 10/4;  问x=?
* 2.5
* 2
* 3
* 2.0
<<<<<<< HEAD

### 查看当前系统cpu架构(smp, numa, mpp)
ls /sys/devices/system/node/     如果只有一个node0说明当前cpu架构为smp, 一般而言2~4个cpu适合smp架构。
lscpu 命令所列出的cpu信息也可以查看到cpu架构信息

* SMP （Symmetric Multiprocessing） , 对称多处理器. 顾名思义, 在SMP中所有的处理器都是对等的, 它们通过总线连接共享同一块物理内存，这也就导致了系统中所有资源(CPU、内存、I/O等)都是共享的，当我们打开服务器的背板盖，如果发现有多个cpu的槽位，但是却连接到同一个内存插槽的位置，那一般就是smp架构的服务器，日常中常见的pc啊，笔记本啊，手机还有一些老的服务器都是这个架构，其架构简单，但是拓展性能非常差

* NUMA （ Non-Uniform Memory Access），非均匀访问存储模型，这种模型的是为了解决smp扩容性很差而提出的技术方案，如果说smp 相当于多个cpu 连接一个内存池导致请求经常发生冲突的话，numa 就是将cpu的资源分开，以node 为单位进行切割，每个node 里有着独有的core ，memory 等资源，这也就导致了cpu在性能使用上的提升，但是同样存在问题就是2个node 之间的资源交互非常慢，当cpu增多的情况下，性能提升的幅度并不是很高。所以可以看到很多明明有很多core的服务器却只有2个node区。

* MPP (Massive Parallel Processing) ，这个其实可以理解为刀片服务器，每个刀扇里的都是一台独立的smp架构服务器，且每个刀扇之间均有高性能的网络设备进行交互，保证了smp服务器之间的数据传输性能。相比numa 来说更适合大规模的计算，唯一不足的是，当其中的smp 节点增多的情况下，与之对应的计算管理系统也需要相对应的提高。

#### 由于numa 架构经常会有内存分配不均匀的情况，常常需要手动干预，除了代码以外，还有linux命令进行cpu的绑定
* taskset  -cp 1,2  25718 #将进程ID 25718 绑定到cpu的第1和第2个core 里

### __alloc_pages_nodemask
* 首先调用first_zone_zonelist得到一个合适的zone来为接下来的内存分配做准备
* ->get_page_from_freelist, 先尝试从空闲列表分配内存；如果空闲列表分配不到内存再尝试进行慢速分配(alloc_pages_slowpath)
*   ->在get_page_from_freelist函数中遍历zonelist, 尝试分配2的order次方个连续物理页面 
*      ->分配内存前检查zone的水位值（有三种水位值，WMARK_MIN, WMARK_LOW, WMARK_HIGH）
*          WMARK_MIN, 表示空闲内存非常少，必须要调用kswapd以同步方式回收内存
*          WMARK_LOW, 表示空闲内存比较少，需要调用kswapd守护线程回内存
*          WMARK_HIGH, 表示空闲内存很多，kswapd可以进入sleep状态
*          因此，检查完zone水位值，就会有两路分支，如果水位值为WMARK_HIGH，则直接进入buffered_rmqueue来进行实际的内存分配操作，
*          否则，执行zone_reclaim来直接回收内存，当回收内存后满足所需内存数量，则进入buffered_rmqueue
*          ->buffered_rmqueue来实际分配一页
*            通常或大多数情况，包括内核源码也期望只分配一页，
*            在每一个zone中，有一个struct per_cpu_pageset __per_cpu *pageset结构，
*            pageset结构用于在每一个cpu上缓存一部分页面，当申请单个page时，尽可能从当前cpu的缓存中分配，
*            这样做的效率要高于从其他cpu共享的缓存中分配
*            介绍一下struct per_cpu_pages{
	     int count; // 链表中页面数量
	     int high;  // 最大页面数
	     int batch; // 一次性申请释放的页面数
	     struct list_head lists[MIGATE_PCPTYPES];
*            }
*            当count 大于 high时，表示当前cpu缓存的页面太多，需要释放batch个页面到zone中；
*            当list为空时，一次性申请batch个页面到list中
*            之后就可以从list取出一页做为本次申请的page的返回
*            接下来从list中删除这一页, 以及减小per_cpu_pages结构中的count值（即count--)
=======
---
### 在C/C++中，goto关键词只能函数内的局部跳转
---
### setjmp和longjmp
> 函数间的跳转需要使用setjmp和longjmp，这也是有些协程库基于setjmp和longjmp实现的原因
---
### LOAD为读操作，STORE为写操作
---
### 四种SMP架构的CPU内存一致性模型
|模型|说明|
|:-|:-|
|顺序一致性模型|（SC，Sequential Consistency，所有读取和所有写入都是有序的）|
|宽松一致性模型|（RC，Relaxed Consistency，允许某些可以重排序），ARM和POWER属于这类|
|弱一致性模型|（WC，Weak Consistency，读取和写入任意重新排序，仅受显式内存屏障限制）|
|完全存储排序|（TSO，Total Store Ordering），SPARC和x86属于这种类型，只有store load一种情况会发生重排序，其它情况和SC模型一样|
---
### x86-TSO
>x86-TSO是Intel推出的一种CPU内存一致性模型，特点是只有“Store Load”一种情况会重排序，也就是“Load”可以排（乱序）在“Store”的前面，因此不需要“Load Load”、“Store Store”和“Load Store”这三种屏障。

|类型|作用|
|:-----------------------|:-----------|
|Store Load屏障|确保“前者刷入内存”的数据对“后者加载数据”是可见的；|
|Load Load屏障|确保“前者装载数据”先于“后者装载指令”；|
|Store Store屏障|确保“前者数据”先于“后者数据”刷入内存，且“前者刷入内存的数据”对“后者是可见的”；|
|Load Store屏障|确保“前者装载数据”先于“后者刷新数据到内存”。|

### SMP对称多处理器结构
>多个CPU对称工作没有区别，无主次或从属关系，平等地访问内存、外设和一个操作系统，共享全部资源，如总线、内存和I/O系统等，因此也被称为一致存储器访问结构（UMA ： Uniform Memory Access）。

### NUMA处理器结构
>（Non-Uniform Memory Access，非统一内存访问），基本特征是将CPU分成多个模型，每个模型多个CPU组成，具有独立的本地内存和I/O槽口等

### MPP处理器结构
>（Massive Parallel Processing，海量并行处理结构），基本特征是由多个SMP服务器（每个SMP服务器称节点）通过节点互联网络连接而成，每个节点只访问自己的本地资源（内存、存储等），是一种完全无共享（Share Nothing）结构

### MESIF
>多核CPU缓存一致性协议

|协议|说明|
|:-|:-|
|M修改（Modified）|该Cache line（缓存行）有效，数据被修改（dirty）了，和主存中的数据不一致，数据只存在于本Cache中|
|E独占互斥（Exclusive）|该Cache line只被缓存在该CPU的缓存中，它是未被修改过的（clean），与主存中数据一致|
|S共享（Shared）|该Cache line有效，数据和内存中的数据一致，数据存在于很多Cache中|
|I无效（Invalid）|该Cache line无效，可能有其它CPU修改了该Cache line|
|F转发（Forward）|Intel提出来的，意思是一个CPU修改数据后，直接针修改的结果转发给其它CPU|
---
### 内存屏障（Memory Barrier）
>内存屏障，也叫内存栅栏（Memory Fence）。分编译器屏障（Compiler Barrier，也叫优化屏障）和CPU内存屏障，其中编译器屏障只对编译器有效，它们的定义如下表所示（限x86，其它架构并不相同）

```c
#define barrier() __asm__ __volatile__("":::"memory")
//编译器屏障，如果是GCC，则可用__sync_synchronize()替代

#define mb()  __asm__ __volatile__("mfence":::"memory")
//CPU内存屏障，还分读写内存屏障

#define rmb() __asm__ __volatile__("lfence":::"memory")
//CPU读（Load）内存屏障

#define wmb() __asm__ __volatile__("sfence":::"memory")
//CPU写（Store）内存屏障
```
* x86架构的CPU内存屏障代码可在内核源代码的arch/x86/include/asm/barrier.h中找到。

* 对于原子操作，需要使用CPU提供的“lock”指令

* 对于CPU乱序需使用CPU内存屏障

|代码顺序|编译器顺序|CPU顺序|
|:-|:-|:-|
|a=1;|a=1;|b=x;|
|b=x;|d=2;|c=z;|
|c=z;|b=x;|a=1;|
|d=2;|c=z;|d=2;|
---
### __user
使用时一般是这样的char __user *x; // 提醒x是一个指向用户空间的变量，但如果你是使用kmalloc为x分配的空间，那么x实际上使用的还是内核空间的地址。也就是说，__user只是用于提醒，并不代表x一定是用户空间的地址。

* 这里假设有如下定义
void fun(const char __user *a);// a是由应用层传入的用户空间数据，那么内核态中执行函数fun内的代码时，如果要操作变量a，则一般需要如下这样，
```c
void fun(const char __user *a, int count){
	if(IS_ERR(a))
		return;
	if(count <= 0)
		return;

	char * d = kzalloc(sizeof(char)*count, GFP_KERNEL);
	if(d){
		if(!copy_from_user(d, a, count)){
			printk(KERN_DEBUG "%s, input:%s\n", __func__, d);
			// 修改d中数据
			d[0] = 't';
			// 将新数据写回用户空间的变量a
			if(!copy_to_user((char *)a, d, count))
				printk(KERN_DEBUG "new d:%s\n", d);
			
		}
		kfree(d);
	}
}

```
---
### EXPORT_SYMBOL(x)
* 导出模块中的函数、变量，这样可以在其他模块或内核中使用

* 举例如下，
```c
// 在模块D1中定义函数funx
char funx(const char * a, const size_t b){
...
}
EXPORT_SYMBOL(funx);

// 在模块D2中调用funx
extern char funx(const char *a, const size_t b);

void somefunc(const char *a, const size_t b){

	char r = funx(a, b);
	
}
```
* 可以使用cat /proc/kallsyms 来查看内核所有可用的symbol

### 内核支持的系统调用，可以通过查看内核源码目录中的arch/x86/syscalls/syscall_64.tbl来查看
* 在用户应用程序中可以直接使用syscall号来调用系统调用，如
```c
ret = (int)syscall(350, &pid, &uid);
```

### kmalloc/ kzalloc等是从kmalloc_caches[]系统内核全局数组中分配的内存
* init/main.c----->start_kernel
* init/main.c--->mm_init
* mm/slab.c----->kmem_cache_init
* mm/slab_common.c--->create_kmalloc_caches
* mm/slab_common.c--->new_kmalloc_cache
* mm/slab_common.c---> create_kmalloc_cache 
* include/linux/slab.h---> kmem_cache_zalloc
* mm/slab.c----->kmem_cache_alloc
* mm/slao.c---->slab_alloc
* 相关的全局数组还有kmalloc_info, 以3.10.0内核源码来看，kmalloc_caches中最大的是64M, 最小的是8个字节.
* cat /proc/slabinfo
* 创建的所有cache都会挂在LIST_HEAD(slab_caches); 这个全局链表上.在cat /proc/slabinfo可以查看

### initramfs.img的作用
* 1.打包initramfs.img
```shell
dracut -f -v --hostonly -k '/lib/modules/version/' x.img
```
* 2.将initramfs.img解包
1)首先file initramfs.img看一下.img文件是否有early_cpio头，如果有则说明不是linux内核原生内核，是centos默认的内核，如果有，则这样解开.img
  (1)mkdir test, 将.img拷贝到test目录
  (2)在  test目录下建立两个文件夹，early_cpio, rootfs
  (3)进入early_cpio, 执行cpio -idmv < ../initramfs.img
  (4)进入rootfs, 执行/usr/lib/dracut/skipcpio  ../initramfs.img | zcat | cpio  -id
	       执行成功的话，在rootfs目录下就可以看到根文件系统

* 3.将.img解包后的文件再打包成.img
解包完成之后，我们可以修改initramfs rootfs的内容，完成一些定制，定制完成后，就需要压缩成内核可以识别的文件。我们将第一步解包后的两个文件夹再打包起来。
```shell
cd /tmp/early_cpio
find . -print0 | cpio --null -o -H newc --quiet >../early_cpio.img

cd /tmp/rootfs_cpio
find . | cpio -o -H newc | gzip > ../rootfs_cpio.img

cd /tmp
cat early_cpio.img rootfs_cpio.img > newInitramfs.img
```
这样newInitramfs.img就是新的.img

### 温习一下ssize_t, size_t
size_t是 unsigned long, -----64bit系统, 如果是32bit系统则是unsigned int;
ssize_t是long, --------64bit系统，如果是32bit系统则是int;

### 系统调用
1.POSIX
2.BSD
3.System V
信号与系统调用冲突时，怎么办？
通常不会，因为信号一般是在进程处理完一个系统调用，返回到用户态的时候。
1）System V Unix的解决方案是，中断系统调用，并让系统调用返回-EINTR，用户程序在收到系统调用返回-EINTR时，就知道是信号中断了前面执行的系统调用，用户程序需要重启系统调用，至到不再被信号中断。
2）BSD解决方案是，中断系统调用，之后去执行信号处理程序，在信号处理程序返回时主动重新调用系统调用，在这中间不需要用户应用程序处理任何事，即对用户程序来说是透明的。
Linux中通过SA_RESTART标志支持BSD方案，可以在安装信号处理程序时指定该标志。现在处理信号与系统调用冲突默认使用的是System V Unix。

在所有平台上，系统调用参数都是通过寄存器直接传递。
从用户态切到内核态，用户进程必须通过一个专用的机械指令（IA－32上是汇编指令INT $0x80，奔2及后续使用汇编指令sysenter, sysexit来快速进入和退出内核态)，来引起处理器或内核的关注，这需要C标准库协助。
IA-32平台上的汇编指令INT $0x80，系统调用编号由寄存器eax传递，参数由ebx, ecx, edx, esi和edi传递。
其他平台，如Alpha, PowerPC, AMD64使用了各自自定义的汇编指令来专门实现系统调用。
sys_call_table。
在上述根据各平台的机械指令进入到内核态后，接下来需要内核负责通过传递进来的系统调用编号来从sys_call_table表中定位到相应的系统调用处理函数指针。
系统调用返回值通常有如下约定：负值表示错误，正值和0表示成功结束。

当系统调用的参数超过五个或其操作的数据过大时，需要一个内存区来交换该数据，此内存区必须在用户空间中，这样用户应用程序才可以访问，此类内存区在内核中使用__user属性标记。
总的来说，系统调用是用户态切换到内核态采用的是同步方式。注：中断则为异步。

### 为什么make内核某个模块时，有的内核环境下会提示找不到crc32库的警告？

### crash, vmcore
yum查找不到debuginfo包是因为默认的debuginfo仓库是禁用的，打开即可：
路径：/etc/yum.repos.d/CentOS-Debug.repo
```shell
[base-debuginfo]
name=CentOS-7 - Debuginfo
baseurl=http://debuginfo.centos.org/7/$basearch/
gpgcheck=0
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-Debug-7
enabled=1
protect=1
priority=1
```
* 改成上面这样子会飞快的下载，protect=1, gpgcheck=0,priority=1

### cat /sys/block/sda/queue/rotational
其值为1，表示机械磁盘，为0表示SSD

### smp_processor_id()获取当前活动cpu id

### TASK_SIZE
```c
TASK_SIZE, arch/x86/include/asm/processor.h
#define TASK_SIZE               (test_thread_flag(TIF_ADDR32) ? IA32_PAGE_OFFSET : TASK_SIZE_MAX)

#define TASK_SIZE_MAX   ((1UL << 47) - PAGE_SIZE) 
```
### dracut
dracut创建内核使用的初始映像，用于预加载访问根文件系统所需的块设备模块（如IDE、SCSI或RAID），挂载根文件系统进入真实系统。

在引导时，内核将归档文件解压到RAM磁盘中，挂载并将其用作初始根文件系统。所有根设备的查找都发生在这个早期的用户空间中。

### 进程管理和调度
* 如果系统处于核心态并正在处理系统调用，那么系统中的其他进程是无法抢夺其CPU时间的，调试器也必须等到系统调用执行结束，才能选择另一个进程执行。
  在kernel 2.5开发其间，引入了内核抢占，该选项支持在紧急情况下切换到另一进程，甚至当前处于核心态执行系统调用也可以切换。
* 中断可以暂停处于用户态和核心态的进程，中断具有最高优先级，因为中断触发后需要尽快处理。

### 体系结构知识
* 内核中特定于体系结构的部分，包含定义和原型的头文件保存在/include/asm-arch/目录下;
* C、汇编保存在arch/下
* 对齐，指对齐到数据类型的字节长度可整除的字节地址。这里讲的是长度。
* 将数据类型对齐到其自身字节长度，称为自然对齐。
* PAGE_ALIGN(addr), 可以将任何地址对齐到页边界。
* arch/x86/include/asm/ptrace.h中定义的pt_regs用于保存CPU寄存器中的数据，在进程切换时需要。
  在进程因为系统调用、中断、或任何其他机制由用户态切换到内核态时，会将保存各寄存器值的pt_regs结构实例放到内核栈上。
  arch/x86/include/asm/processor.h中包含了thread_struct结构，其中包含所有其他寄存器和所有其他进程状态信息.

### 自洽
  简单地说就是按照自身的逻辑推演的话，自己可以证明自己至少不是矛盾或者错误的，这就是简单的自洽性。
  科学研究本身就是遵循自洽性的，建立于客观基础上，反之则建立于主观之上，最终归属不可证伪与证明，一个不能够满足自洽性的理论或者方法显然是不攻自破的。
  所谓自洽：“某个理论体系或者数学模型的内在逻辑一致，不含悖论”。
  转移到计算机软件，我对它的定义为“一个计算机软件，在各个模块，各个函数，各个功能之间对相同问题，没有不同的看法”。
  比如我们的软件中包含了供应商，和主要供应商，如果在采购时允许用户对同一个商品有多个供应商，可是在查询的时候，却仅仅为用户提供“按主要供应商查询的功能”，即是一个不自洽。
  软件自恰和需求无关，没有任何需求的情况下，一样可以检查系统的自洽性。
  自洽性是测试的一个基本工具。今天的软件自洽，但是随着新的需求的引入，一定会打破原来的自洽，引入新一轮的自洽过程，从而进入新一级的自洽。

### qgit
* 界面管理git工具，好用。

### ddd
>>> gdb的图形界面，拥有gdb一切功能。
>>> 也可以用ddd来查看内核转储文件，如/proc/kcore，或/var/crash/xxx/vmcore文件，ddd /usr/lib/debug/lib/modules/kernelversion/vmlinux /proc/kcore
* vmlinux是带有调试信息的内核镜像

### inode
* 硬链接，用ln创建，如: ln f1.txt f2.txt; 硬链接表示两个文件的inode号相同，即f1.txt与f2.txt的inode->i_ino相同。
* 软链接, 用ln -s创建，如: ln -s f1.txt f2.txt; 软链接表示f2.txt保存的数据是指向文件f1.txt的路径，两个文件的inode号不同。
          如果f1.txt被删除，则再访问f2.txt系统会报错找不到这样的文件.
	  ln 源文件 目标文件
* 可以用stat查看inode信息
* inode大小一般为128字节或256字节，inode总数在磁盘格式化时给定，一般是每1kb或2kb就设置一个inode;
* 硬盘格式化时，操作系统自动将硬盘分成两人个区域，一个是数据区用来存放文件数据； 另一个是inode区，用来存放inode所包含的信息。
* 硬盘最小存储单位叫扇区(sector)，每个扇区储存512字节。操作系统读取硬盘时，不会一个个扇区的读，效率太低，而是一次性连续读取多个扇区，限一次性读取一个块(block)。块的大小一般为4kb.
  块是文件存取的最小单位。

### 带宽、IOPS
  顺序读写看带宽，随机读写看的是IOPS；IOPS指处理I/O的数量。带宽指的是处理I/O的速率。

### RAID(Redundant Array of Independent Disks)
    独立磁盘冗余陈列，RADI技术将多个单独的物理磁盘以不同的方式组合成一个逻辑硬盘，从而达到提升存储容量、读写性能和数据安全性的目的。
    RAID0, 最少一块硬盘，无备份（即无容错，出现故障后无法恢复），性能最好，磁盘利用率100%;
    RAID1，最少两块硬盘，一个是业务盘存数据，一个是镜像盘，磁盘利用率50%;每次写操作，需要将数据同时写入镜像盘，读操作只从工作盘读。一旦工作盘出现故障立即转到镜像盘，由镜像盘读取数据。当更换故障工作盘后，数据可从镜像盘恢复。
    RAID3, 最少三块硬盘，一块用于存储专用的校验数据，录某块硬盘出现故障时，其他硬盘可以通过校验数据将有故障的硬盘数据重新恢复出来, 磁盘利用率N-1, 缺点：校验盘是性能瓶颈，每次读写关系到整组硬盘，且只完成一次I/O。
    RAID5, 校验数据分别存储在所有硬盘上，当有磁盘出来故障时，其他硬盘可以通过校验数据将故障硬盘的数据恢复出来, 磁盘利用率也是N-1.(缺点：异或校验影响性能)

### 进程、线程
* 进程是资源分配的最小单位；线程是程序执行的最小单位。
* 进程有自己的独立地址空间，线程没有。
* 并发：让一个CPU一心多用，同时运行多个进程，此种技术称为并发。实现并发技术的关键是“时间片轮转进程调度算法”，它的思想简单的说：在操作系统的管理下，所有正在运行的进程轮流使用CPU，每个进程允许占用CPU的时间非常短（比如说10毫秒），这样用户根本感觉不出来CPU是在轮流为多个进程服务，就好像所有的进程都在不间断地运行一样。但实际上，在任何一个时间内有且仅有一个进程占有CPU。
        简单的说，并发指的是在一段时间里，有几个程序都处于已启动运行到运行完毕之间，且这几个程序都是在同一个CPU上运行，但任意一个时间点上只有一个程序在CPU上运行。
	并发的关键点是你有处理多任务的能力，但不是同时处理。
* 并行：指的是多CPU（或多核心）时，多个不同的进程同时运行在不同的CPU（或核心）上。并行的关键是你有同时处理多个任务的能力。
* 多线程的执行方式和多进程是一样的，也是由操作系统在多个线程之间快速切换，让每个线程都短暂地交替运行，看起来就像同时执行一样。当然，真正同时执行多线程需要多核CPU才可能实现。

### cache, buffer
* CPU------cache------内存-------buffer--------磁盘
  cache: 高速缓存，用于减小cpu与内存性能差距提高性能, 一般针对的是读操作
  buffer: 用于降低内存与磁盘之间性能差距，一般针对的是写操作

### CPU 的高速缓存  
* L1, L2, L3, 三级缓存，L1最快并靠近CPU， L2次之，L3最慢，容量L1最小（一般256KB）, L2大一些，L3容量最大
  L1可再细分为L1d, L1i， 其中L1d为数据缓存，L1i为指令缓存.
* MESI, 依利诺斯协议
  M、E、S、I分别代表CPU高速缓存的四种状态，M为修改，E为独占，S为共享，I为无效
* 四种状态间切换规则
状态      事件           CPU行为
I         本地读操作     1、其他CPU上的缓存没有这份数据，则从内存中读取数据，并将Cache line变成E;
                         2、其他CPU上的缓存有这份数据，且状态为M，则将数据更新到内存，之后，本CPU缓存从内存中取数据，两个CPU上的cache line状态都变成S
			 3、其他CPU上的缓存有这份数据，且状态为E或S，则本CPU从内存取数据，所有有这份数据的cache line状态都变成S
          本地写操作     1、从内存取数据，在cache中修改，将cache line状态变为M;如果其他cache有这份数据，则其他cache line状态变为I

	  远程读操作     既然是I状态，别的CPU的操作与它无关
	  远程写操作     既然是I状态，别的核的操作与它无关

S         本地读操作     本地CPU从cache中取数据，状态不变
	  本地写操作     本地CPU在cache中修改数据，其他CPU的cache line状态变为I
	  远程读操作     其他CPU读这行数据，cache line状态不变
	  远程写操作     数据被其他CPU修改，本地CPU的cache中的数据不能再使用，状态变为I

M	  本地读操作     本地CPU从cache中取数据，状态不变
	  本地写操作     本地CPU修改cache中的数据，状态不变
	  远程读操作     本地CPU将这行数据写到内存中，使其他核可以使用到最新的数据，状态变为S
	  远程写操作     这行数据被本地CPU写到内存中，使其他核可以使用到最新的数据，但其他核会修改这行数据，状态变为I

E	  本地读操作     本地CPU从cache中取数据，状态不变
	  本地写操作     本地CPU在cache中修改数据，状态变为M
	  远程读操作     数据和其他核共用，状态变为S
	  远程写操作     数据被其他核修改，本地CPU的cache line状态变为I

### 终端快捷键
	Ctrl+Alt+T 打开终端
一些常用的终端快捷键:
	Ctrl+L 清空屏幕(功能相当于命令clear)
	Ctrl+U 剪切文本直到行的起始(可以用于清空行)
	Ctrl+K 剪切文本直到行的末尾
	Ctrl+Y 粘贴最近剪切的文本
	Ctrl+C 杀死当前进程(也可以用来清空当前行)
	Ctrl+D 退出当前Shell(功能相当于命令exit) 或者 删除当前的字符
	Ctrl+A 行首
	Ctrl+E 行尾
	Home/End 行首/行尾
	Ctrl+F 向前移动一个字符
	Ctrl+B 向后移动一个字符
	Ctrl+P 或 Ctrl+N 上下历史记录
	上下方向键 上下历史记录
	Ctrl+Shift+C 复制
	Ctrl+Shift+V 粘贴
	还有Tab补全,按住Ctrl键进行块选择.
	鼠标中键:粘贴(在gnome-terminal中使用"菜单键+P"也是可以粘贴的)

### https://segmentfault.com/     一个不错的网站	

### readpages，地址空间上的readpages函数用途是什么？预读？如下是预读的内核执行路径, 由下而上(如同Call trace一样)
   查找3.10.0kernel源码，只有两处调用了mmaping->a_ops->readpages，而实际调用readpages函数的只有后者
   一处是：fs/cachefiles/rdwr.c
   另一处是：mm/readahead.c, 如下是后者的调用路径，从源码可判断，readpages目前看只是用来做文件预读的
	address_space->readpages
	read_pages     <mm/readahead.c>
	__do_page_cache_readahead   <mm/readahead.c>
	ondemand_readahead    <mm/readahead.c>
	page_cache_async_readahead    <mm/readahead.c>
	do_generic_file_read     <mm/filemap.c>
	generic_file_aio_read    <mm/filemap.c>
	xfs_file_buffered_aio_read   <fs/xfs/xfs_file.c>

### ASSERT
* assert()是一个调试程序时经常使用的宏，在程序运行时它计算括号内的表达式，如果表达式为FALSE (0), 
	程序将报告错误，并终止执行。如果表达式不为0，则继续执行后面的语句，它的作用是终止程序以免导致严重后果，同时也便于查找错误。

* 每个assert只检验一个条件,因为同时检验多个条件时,如果断言失败,无法直观的判断是哪个条件失败

* 不能使用改变环境的语句,因为assert只在DEBUG个生效,如果这么做,会使用程序在真正运行时遇到问题
	错误: assert(i++ < 100)
	这是因为如果出错，比如在执行之前i=100,那么这条语句就不会执行，那么i++这条命令就没有执行。
	正确: assert(i < 100)；
	         i++;

* ASSERT只有在Debug版本中才有效，如果编译为Release版本则被忽略掉。（在C中，ASSERT是宏而不是函数），使用ASSERT“断言”容易在debug时输出程序错误所在。

* 而assert()的功能类似，它是ANSI C标准中规定的函数，它与ASSERT的一个重要区别是可以用在Release版本中
  使用assert的缺点是，频繁的调用会极大的影响程序的性能，增加额外的开销。

* 在调试结束后，可以通过在包含#include <assert.h>的语句之前插入 #define NDEBUG 来禁用assert调用，示例代码如下：
#include <stdio.h>
#define NDEBUG
#include <assert.h>

* 何时需要使用断言
1.可以在预计正常情况下程序不会到达的地方放置断言：ASSERT( FALSE );
2.断言可以用于检查传递给私有方法的参数。（对于公有方法，因为是提供给外部的接口，所以必须在方法中有相应的参数检验才能保证代码的健壮性）
3.使用断言测试方法执行的前置条件和后置条件
4.使用断言检查类的不变状态，确保任何情况下，某个变量的状态必须满足。（如age属性应大于0小于某个合适值）

* 什么地方不要使用断言
断言语句不是永远会执行，可以屏蔽也可以启用
因此：
1.不要使用断言作为公共方法的参数检查，公共方法的参数永远都要执行
2.断言语句不可以有任何边界效应，不要使用断言语句去修改变量和改变方法的返回值

### rpmdevtools，深踩一坑
* 第一步：下载并安装kernel-2.6.32-754.el6.src.rpm
* 第二步：yum install rpm-build
* 第三步：rpmbuild --target=x86_64 -bp SPEC/kernel.spec
  这一处会执行失败，卡在"gpg: WARNING: some OpenPGP programs can't handle a DSA key with this digest size"这里
  这是要“生成用于对模块签名的PGP密钥对”

  怀疑是环境问题，在安装了rpmdevtools后一切正常。
  其间，我测试了重新安装src.rpm, 重新下载后再安装测试问题依旧，最终确定是工具安装的不全，使用rpmbuild不能只安装一个rpm-build，必须安装rpmdevtools才可以，其中

  rpmdevtools工具参考：https://www.nixops.me/articles/howto-create-and-sign-rpm-package.html

### WARN
* WARN(条件, format..), 当条件为真时，用printk输出format指定格式的信息.

### /proc/sys/kernel/printk

* Printk 共有4个参数
```shell
cat /proc/sys/kernel/printk
```
7 4 1 7

该文件有四个数字值，它们根据日志记录消息的重要性，定义将其发送到何处。上面显示的4个数据分别对应：

控制台日志级别：优先级高于该值的消息将被打印至控制台

默认的消息日志级别：将用该优先级来打印没有优先级的消息

最低的控制台日志级别：控制台日志级别可被设置的最小值(最高优先级)

默认的控制台日志级别：控制台日志级别的缺省值

数值越小，优先级越高


（1）第一个参数 7表示小于7优先级消息才会被输出到控制台。

（2）第二个参数4 表示默认的printk消息优先级别，即printk(“hell world”);优先级为4， 由于4<7，故可以被打印到控制台。

（3）第三个参数1 表示可接收的最高优先级，当printk disable控制台输出时,设置第一个参数为1，但是，从内核等级来看，还有优先级0，这个是printk最高级优先级，一般用于内核严重消息打印。比如内存错误或者 watchdog reset.也可以设置第一个和第三个参数为0

（4）第四个参数7 默认控制台优先级，即第一个参数的默认优先级。

### flatpak
* 添加flathub仓库, flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
* 查看已经添加的软件仓库, flatpak remotes
* 删除软件仓库，例如 flathub，用这条命令：$flatpak remote-delete flathub
* 在flatpak沙盒中安装应用程序, flatpak install 仓库名 程序名, 如：flatpak install flathub 
* 运行flatpak 中的程序, flatpak run 程序名
* 查看已经安装在flatpak沙盒中的应用程序, flatpak list --app
* 删除flatpak中安装的应用程序, flatpak uninstall 程序名
* 查看可以在flatpak中安装的程序列表, flatpak remote-ls 
* 更新所有flatpak中的应用程序, flatpak update

### ls -d */ 只查看文件夹

### 一个进程的整个用户空间(用mm_struct结构描述)通常有若干离散的虚拟区间(用结构vm_area_struct描述)

### 总线地址、物理地址、逻辑地址、虚拟地址

### xfs文件系统备份及恢复
* 备份
xfsdump -f /bk.xfsdump /dev/sda1
* 恢复
xfsrestore -f /bk.xfsdump /dev/sda1
* 查看是否成功
xfsdump -I

### mce
 
### cscope -Rbq, ctags -R *

### 设置cpu0下线
```shell
echo 0 > /sys/devices/system/cpu/cpu0/online
```

### 文件描述符
0标准输入
1标准输出
2标准错误输出
所以，所有系统调用open分配的文件描述符从3开始

### 打开的文件必须与某个特定的进程关联

### file_oprations中的readv/writev用于实现向量的读取或写入。
    向量是种数据结构，用来提供一块非连续的内存区域，来存放读取的数据或写入的数据，
    该技术称为分散聚集，用于避免多次财用read/write降低性能。

### page_update
    用于检查缓存中的页中数据是否是最新的，如果不是则用mapping->a_ops->readpage更新

### 统计文件夹下（及其子目录）正规文件个数
    ls -Rl | grep "^-" | wc -l

### 内核中页是热的，指页已经被加载到CPU高速缓存    

### NR_CPUS宏
    它的值是内核支持的CPU个数，不是系统中实际存在的CPU个数
    单处理器时，其值为1
    SMP时，32位系统：2～32
           64位系统：2～64

### 对于用户空间的应用程序，物理内存中的空闲页是否连续都不影响其正常运行和使用内存，
    因其内存是通过页表映射到页帧的，所以在其看来内存总是连续的。	   

### 页
    1.不可移动页, 内核分配的大多数是这样的
    2.可回收页, 针对文件
    3.可移动页, 针对用户空间应用程序的页帧，因其是通过页表与页帧关联的，页帧移动后，只需要更新页表，应用程序不会注意到什么变化。
    反碎片 提出的上述页划分，而其中的可移动页是反碎片技术实现的基础。

### 伙伴系统分配函数关系
    alloc_page       get_zeroed_page                    _get_free_page          _get_dma_pages
        |                  |                                  |                        |
	 |                |                                    |                      |
	   |            |                                        |                  |
	    |          |                                            get_free_pages     
	      |      |                                                    |
	        |  |                                                    |
		 |                                                   |
		 |                                                 |
		  |                                             |
		    |                                        |
		       |                                  |
		          |                           |
			           alloc_pages
				        |
					|
				alloc_pages_nodes
					|
				__alloc_pages-------伙伴系统心脏

### 所有内核代码必须是并发安全的
  1.可重入, 程序必须允许同时执行
  2.线程安全，数据必须针对并行访问进行保护

### cat /proc/zoneinfo
  * 查看每核CPU上高速缓存


### 只有当slab系统启用后，kmalloc才可使用 

### shell脚本里的命令exec "$@" 
  $@表示的有传进脚本的参数
  exec 表示执行
  所以exec "$@"表示执行所有通过参数传进shell脚本的参数

### 软连接和硬链接的特点：

  软链接：

  1.软链接是存放另一个文件的路径的形式存在。
  2.软链接可以 跨文件系统 ，硬链接不可以。
  3.软链接可以对一个不存在的文件名进行链接，硬链接必须要有源文件。
  4.软链接可以对目录进行链接。

  硬链接：

  1. 硬链接，以文件副本的形式存在。但不占用实际空间。
  2. 不允许给目录创建硬链接。
  3. 硬链接只有在同一个文件系统中才能创建。
  4. 删除其中一个硬链接文件并不影响其他有相同 inode 号的文件。

