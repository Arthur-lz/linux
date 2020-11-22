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
>>>>>>> f51522e3cc3b7a91e7c80066029c39d6f65b4a86
