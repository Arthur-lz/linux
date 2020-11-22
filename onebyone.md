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

### 下面四个中哪一个不是表示数组char A[10]的A[1]的地址？
* s+1
* s++
* &s[0]+1
* &s[1]

### float x; x = 10/4;  问x=?
* 2.5
* 2
* 3
* 2.0

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
