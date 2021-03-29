### 解释/proc/meminfo中各项含义
* Cached
 Page Cache里包括所有file-backed pages，统计在/proc/meminfo的”Cached”中。
 Cached不仅包括mapped，也包括unmapped的页面，当一个文件不再与进程关联之后，原来在page cache中的页面并不会立即回收，仍然被计入Cached，还留在LRU中，但是 Mapped 统计值会减小。【ummaped = (Cached – Mapped)】
 Cached包含tmpfs中的文件，POSIX/SysV shared memory，以及shared anonymous mmap。

 注：POSIX/SysV shared memory和shared anonymous mmap在内核中都是基于tmpfs实现的，参见：
 https://www.kernel.org/doc/Documentation/filesystems/tmpfs.txt
 有意思的是，Shared memory和tmpfs在不发生swap-out的时候属于”Cached”，而在swap-out/swap-in的过程中会被加进swap cache中、属于”SwapCached”，一旦进了”SwapCached”，就不再属于”Cached”了。”Cached”和”SwapCached”两个统计值是互不重叠的

* SwapCached
我们说过，匿名页(anonymous pages)要用到交换区，而shared memory和tmpfs虽然未统计在AnonPages里，但它们背后没有硬盘文件，所以也是需要交换区的。也就是说需要用到交换区的内存包括：”AnonPages”和”Shmem”，我们姑且把它们统称为匿名页好了。

交换区可以包括一个或多个交换区设备（裸盘、逻辑卷、文件都可以充当交换区设备），每一个交换区设备都对应自己的swap cache，可以把swap cache理解为交换区设备的”page cache”：page cache对应的是一个个文件，swap cache对应的是一个个交换区设备，kernel管理swap cache与管理page cache一样，用的都是radix-tree，的区别是：page cache与文件的对应关系在打开文件时就确定了，而一个匿名页只有在即将被swap-out的时候才决定它会被放到哪一个交换区设备，即匿名页与swap cache的对应关系在即将被swap-out时才确立。

并不是每一个匿名页都在swap cache中，只有以下情形之一的匿名页才在：
匿名页即将被swap-out时会先被放进swap cache，但通常只存在很短暂的时间，因为紧接着在pageout完成之后它就会从swap cache中删除，毕竟swap-out的目的就是为了腾出空闲内存；

【注：参见mm/vmscan.c: shrink_page_list()，它调用的add_to_swap()会把swap cache页面标记成dirty，然后它调用try_to_unmap()将页面对应的page table mapping都删除，再调用pageout()回写dirty page，最后try_to_free_swap()会把该页从swap cache中删除。】
曾经被swap-out现在又被swap-in的匿名页会在swap cache中，直到页面中的内容发生变化、或者原来用过的交换区空间被回收为止。

【注：当匿名页的内容发生变化时会删除对应的swap cache，代码参见mm/swapfile.c: reuse_swap_page()。】
/proc/meminfo中的SwapCached背后的含义是：系统中有多少匿名页曾经被swap-out、现在又被swap-in并且swap-in之后页面中的内容一直没发生变化。也就是说，如果这些匿名页需要被swap-out的话，是无需进行I/O write操作的。
“SwapCached”不属于”Cached”，两者没有交叉。

 “SwapCached”内存同时也在LRU中，还在”AnonPages”或”Shmem”中，它本身并不占用额外的内存。

  
* Buffers
  我的电脑上(Centos 7.8)一直是固定值1096K
  Buffers统计的是直接访问块设备时的缓冲区的总大小，有时候对文件系统元数据的操作也会用到buffers。这部分内存不好直接对应到某个用户进程，应该算作kernel占用。

* Bounce
有些老设备只能访问低端内存，比如16M以下的内存，当应用程序发出一个I/O 请求，DMA的目的地址却是高端内存时（比如在16M以上），内核将在低端内存中分配一个临时buffer作为跳转，把位于高端内存的缓存数据复制到此处。这种额外的数据拷贝被称为”bounce buffering”，会降低I/O 性能。大量分配的bounce buffers 也会占用额外的内存。 

* KernelStack
每一个用户线程都会分配一个kernel stack（内核栈），内核栈虽然属于线程，但用户态的代码不能访问，只有通过系统调用(syscall)、自陷(trap)或异常(exception)进入内核态的时候才会用到，也就是说内核栈是给kernel code使用的。在x86系统上Linux的内核栈大小是固定的8K或16K（可参阅我以前的文章： 内核栈溢出 http://ju.outofmemory.cn/entry/254937 ）。

Kernel stack（内核栈）是常驻内存的，既不包括在LRU lists里，也不包括在进程的RSS/PSS内存里，所以我们认为它是kernel消耗的内存。统计值是/proc/meminfo的KernelStack。 

* PageTables
Page Table用于将内存的虚拟地址翻译成物理地址，随着内存地址分配得越来越多，Page Table会增大，/proc/meminfo中的PageTables统计了Page Table所占用的内存大小。

注：请把Page Table与Page Frame（页帧）区分开，物理内存的最小单位是page frame，每个物理页对应一个描述符(struct page)，在内核的引导阶段就会分配好、保存在mem_map[]数组中，mem_map[]所占用的内存被统计在dmesg显示的reserved中，/proc/meminfo的MemTotal是不包含它们的。（在NUMA系统上可能会有多个mem_map数组，在node_data中或mem_section中）。
而Page Table的用途是翻译虚拟地址和物理地址，它是会动态变化的，要从MemTotal中消耗内存。 


* HardwareCorrupted
当系统检测到内存的硬件故障时，会把有问题的页面删除掉，不再使用，/proc/meminfo中的HardwareCorrupted统计了删除掉的内存页的总大小。相应的代码参见 mm/memory-failure.c: memory_failure ()。 

* SLAB
通过slab分配的内存被统计在以下三个值中：
SReclaimable: slab中可回收的部分。调用kmem_getpages()时加上SLAB_RECLAIM_ACCOUNT标记，表明是可回收的，计入SReclaimable，否则计入SUnreclaim。
SUnreclaim: slab中不可回收的部分。
Slab: slab中所有的内存，等于以上两者之和。 

*  DirectMap
/proc/meminfo中的DirectMap所统计的不是关于内存的使用，而是一个反映TLB效率的指标。TLB(Translation Lookaside Buffer)是位于CPU上的缓存，用于将内存的虚拟地址翻译成物理地址，由于TLB的大小有限，不能缓存的地址就需要访问内存里的page table来进行翻译，速度慢很多。为了尽可能地将地址放进TLB缓存，新的CPU硬件支持比4k更大的页面从而达到减少地址数量的目的， 比如2MB，4MB，甚至1GB的内存页，视不同的硬件而定。”DirectMap4k”表示映射为4kB的内存数量， “DirectMap2M”表示映射为2MB的内存数量，以此类推。所以DirectMap其实是一个反映TLB效率的指标。 

* VmallocUsed
通过vmalloc分配的内存都统计在/proc/meminfo的 VmallocUsed 值中，但是要注意这个值不止包括了分配的物理内存，还统计了VM_IOREMAP、VM_MAP等操作的值，譬如VM_IOREMAP是把IO地址映射到内核空间、并未消耗物理内存，所以我们要把它们排除在外。从物理内存分配的角度，我们只关心VM_ALLOC操作，这可以从/proc/vmallocinfo中的vmalloc记录看到：
注：/proc/vmallocinfo中能看到vmalloc来自哪个调用者(caller)，那是vmalloc()记录下来的，相应的源代码可见：
mm/vmalloc.c: vmalloc > 
      __vmalloc_node_flags > 
                __vmalloc_node > 
		      __vmalloc_node_range > 
		             __get_vm_area_node > 
			               setup_vmalloc_vm

通过vmalloc分配了多少内存，可以统计/proc/vmallocinfo中的vmalloc记录，例如：
一些driver以及网络模块和文件系统模块可能会调用vmalloc，加载内核模块(kernel module)时也会用到，可参见 kernel/module.c。 

*  Linux的内存都用到哪里去了？
尽管不可能较精确统计Linux系统的内存，但大体了解还是可以的。
kernel内存的统计方式应该比较明确，即
【Slab+ VmallocUsed + PageTables + KernelStack + Buffers + HardwareCorrupted + Bounce + X】

注1：VmallocUsed其实不是我们感兴趣的，因为它还包括了VM_IOREMAP等并未消耗物理内存的IO地址映射空间，我们只关心VM_ALLOC操作，（参见1.2节），所以实际上应该统计/proc/vmallocinfo中的vmalloc记录

注2：kernel module的内存被包含在VmallocUsed中，见1.3节。

注3：X表示直接通过alloc_pages/__get_free_page分配的内存，没有在/proc/meminfo中统计，不知道有多少，就像个黑洞。 

*  用户进程的内存主要有三种统计口径：
[1]围绕LRU进行统计
【(Active + Inactive + Unevictable) + (HugePages_Total * Hugepagesize)】

[2]围绕Page Cache进行统计
当SwapCached为0的时候，用户进程的内存总计如下：
【(Cached + AnonPages) + (HugePages_Total * Hugepagesize)】

当SwapCached不为0的时候，以上公式不成立，因为SwapCached可能会含有Shmem，而Shmem本来被含在Cached中，一旦swap-out就从Cached转移到了SwapCached，可是我们又不能把SwapCached加进上述公式中，因为SwapCached虽然不与Cached重叠却与AnonPages有重叠，它既可能含有Shared memory又可能含有Anonymous Pages。

[3]围绕RSS/PSS进行统计
把/proc/[1-n]/smaps 中的 Pss 累加起来就是所有用户进程占用的内存，但是还没有包括Page Cache中unmapped部分、以及HugePages，所以公式如下：
ΣPss + (Cached – mapped) + (HugePages_Total * Hugepagesize)

所以系统内存的使用情况可以用以下公式表示：
MemTotal = MemFree +【Slab+ VmallocUsed + PageTables + KernelStack + Buffers + HardwareCorrupted + Bounce + X】+【Active + Inactive + Unevictable + (HugePages_Total * Hugepagesize)】
MemTotal = MemFree +【Slab+ VmallocUsed + PageTables + KernelStack + Buffers + HardwareCorrupted + Bounce + X】+【Cached + AnonPages + (HugePages_Total * Hugepagesize)】
MemTotal = MemFree +【Slab+ VmallocUsed + PageTables + KernelStack + Buffers + HardwareCorrupted + Bounce + X】+【ΣPss + (Cached – mapped) + (HugePages_Total * Hugepagesize)】 



