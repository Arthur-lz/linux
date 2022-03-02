### 6.7.3 USB
* USB是将内核的热插拔能力提供给大量用户的第一种总线
* USB并不绑定到某种特定的处理器或体系结构 
#### 2. 驱动程序的管理 
* 内核中按两个层次实现usb总线系统
> 1. 宿主机适配器的驱动程序必须是可用的.该适配器必须为USB链提供连接选项，并承担与终端设备的电子通信.适配器自身必须连接到另一个系统总线（当前，有3种不同宿主机适配器类型，分别称之为OHCI, EHCI, 和UHCI,这些涵盖了市售的所有控制器类型)

> 2. 设备驱动程序与各个USB设备通信，并将设备的功能导出到内核的其他部分，进而到用户空间。这些驱动程序与宿主机控制器通过一种标准化接口交互，因而控制器类型与USB驱动程序是不相关的。任何其他方法显然都是不切实际的，因为需要为每个USB设备开发与宿主机控制器相关的驱动程序

# 第12章 网络 
## 12.1 互联的计算机
* 1. 如何建立物理连接？使用哪一种线缆？
* 2. 如何处理传输错误？
* 3. 如何识别网络中的每一台计算机？
* 4. 如果两台计算机通过其他计算机连接，那么二者之间的数据交换如何进行？如何查找最佳路由？
## 12.2 ISO/OSI 和TCP/IP参考模型
* 每层只能与紧邻的层通信
### 各层执行的任务如下
* 主机到网络层，负责将信息从一台计算机传输到远程计算机，并将数据流划分为定长的帧，以便在传输错误时重传数据块
> 各个计算机通过全球唯一的MAC地址标记每一个网卡，进而标记每台计算机

* 网络层（或称IP层）
> 网络层的任务是找到一条线路，使得计算机可以彼此通信

> 网络层也负责将传输的数据划分为特定长度的分组

> 网络层还分配网络中唯一的地址，以便计算机可以彼此通信

> 因特网中，网络层借助IP协议实现

> IP使用一定格式的地址来寻址计算机，这些地址由正式注册的权威机构分配，或者在私有范围内自由选择

* 传输层
> 任务是在两个建立了链路的计算机上，控制应用程序之间的数据传输

> 在因特网中，TCP, UDP

> 传输层的另一个任务是可以提供一种可靠的连接，使得通过该连接的数据按给定的顺序到达

* 应用层
> 它负责传输实际的内容 

### 网络的层状结构在内核中反映为如下事实
> 不同的层次由分离的代码实现

> 不同层次的代码之间通过明确定义的接口来交换数据或转发命令

## 12.3 通过套接字通信 
* 网卡的动作方式与普通的块设备和字符设备完全不同，使得UNIX箴言“万物皆文件”不再完全适用
> 这样说的原因是，所有层使用了许多不同的通信协议，这样就需要设置许多选项，无法在打开设备文件时完成这些设置，因此在/dev下没有与物理网卡对应的项

* 套接字现在用于定义和建立网络连接，以便可以用操作inode的普通方法来访问网络

### 12.3.1 创建套接字
> TCP用于流

> UPD用于数据报服务

* 因特网地址由IP和端口号唯一定义
> 关键数据结构struct sockaddr_in

* cpu存储数值有两种惯例，即小端序和大端序
* 网络字节序， 它等价大端序

### 12.3.2 使用套接字
* 服务器不会主动与另一个程序建立连接，服务器只会被动等待，直到收到连接请求，建立一个被动连接需要以下三个库函数
> 1. bind将套接字绑定到一个地址

> 2. listen通知套接字被动地等待客户端连接请求的到来。该函数创建一个等待队列，将所有希望建立连接的远程进程放在该队列上

> 3. accept接受等待队列上第一个客户端的连接请求，在队列为空时，该函数将阻塞，直到有一个想要连接的客户端到来

* 一个四元组(IP:port, IP:port)用来唯一标识一个连接

* 在服务器调用fork复制自身来处理某个连接之后，在内核中注册了两个套接字对
> 尽管两个服务器进程的套接字具有相同的IP地址：端口号组合，但二者对应的四元组是不同的

> 内核在分配输入和输出TCP/IP分组时，必须注意到四元组的所有4个分量，才能确保正确 

> 该任务称为多路复用

* netstat工具可以显示并检查系统上所有TCP/IP连接的状态 
```sh
netstat -an
激活Internet连接 (服务器和已建立连接的)
Proto Recv-Q Send-Q Local Address           Foreign Address         State
tcp        0      0 127.0.0.1:17473         0.0.0.0:*               LISTEN
tcp        0      0 127.0.0.1:4769          0.0.0.0:*               LISTEN
tcp        0      0 127.0.0.1:5345          0.0.0.0:*               LISTEN
tcp        0      0 0.0.0.0:111             0.0.0.0:*               LISTEN
...
```
### 12.3.3 数据报套接字
* TCP协议比较像电话网络
* UDP协议比较像邮政业务

## 12.4 网络实现的分层模型
* 传输的基本单位是以太网帧
> 网卡以帧为单位发送数据 

* 各协议层的数据划分为首部和数据两部分
* 以太网帧中通过TCP/IP传输HTTP数据举例
```
	|<--------------------------------------------------------->|
	|Mac首部|IP首部|TCP首部|HTTP首部|         HTML数据          |
	        |<---------以太网帧的净荷-------------------------->|
		       |<------IP净荷------------------------------>|
		               |<---------TCP净荷------------------>|
			                |<-----HTTP净荷------------>|
```
## 12.5 网络命名空间
* 记住这：
> 网络子系统实现的所有全局函数，都需要一个网络命名空间作参数，而网络子系统的所有全局属性，只能通过命名空间迂回访问
## 12.6 套接字缓冲区
* struct sk_buff
> 套接字缓冲区sk_buff用于在网络实现的各个层次之间交换数据，而无须来回复制分组数据

> sk_buff结构是网络子系统的基石之一

### 12.6.1 使用套接字缓冲区管理数据 
* 套接字缓冲区的基本思想是：通过操作指针来增删协议首部

### 12.6.2 管理套接字缓冲区数据 
## 12.7 网络访问层
* 该层主要负责在计算机之间传输信息，与网卡的设备驱动程序直接协作

* 以太网帧是用来解释如何在“电线”上表示数据

### 12.7.1 网络设备的表示 
#### 内核如何跟踪可用的网络设备？
* 每个命名空间中有如下3个机制可用
> 1. 所有的网络设备都保存在一个单链表中, 表头为dev_base

> 2. 按设备名散列，dev_get_by_name()根据设备名在该散列表上查找网络设备

> 3. 按接口索引散列, dev_get_by_index()根据给定的接口索引查找net_device实例

* struct net_device结构包含了与特定网络设备相关的所有信息
```c
struct net_device {
...
	unsigned	mtu;	// 接口MTU值，maximum transfer unit, 最大传输单位，它用于指定一个传输帧的最大长度
	tx_timeout		// 用来解决分组传输失败的问题
...
};
```

### 12.7.2 接收分组
* 分组到达内核的时间是不可预测的
> 所有现代的设备驱动程序都使用中断来通知内核有分组到达

* 几乎所有的网卡都支持DMA模式，能够自行将数据传输到物理内存，但这些数据应需要解释和处理
#### 1. 传统方法
#### 2. 对高速接口的支持
* 如果设备不支持高速率传输，那么前面的传统方法可以很好的将分组从网卡传输到内核的高层
> 每次一个以太网帧到达时，都使用一个IRQ来通知内核

> 对于低速网卡来说，在下一个分组到达之间，IRQ的处理通常已经结束, 这样各个分组之间不会重叠

* 由于下一个分组也通过IRQ通知内核，如果前一个分组的IRQ尚未处理完成，则会导致问题，高速网卡通常就是这样
> 现代以太网卡动作速度高达10000Mbit/s, 如果使用传统方式来驱动此类设备，将造成“中断风暴”

> 为解决这个问题NAPI使用了“IRQ和轮询组合方式”

* NAPI网卡情况如下
> 1. 第一个分组将触发网卡发出IRQ, 为防止后面的分组触发更多的IRQ，驱动程序会关闭该网卡的Rx IRQ，同时交该网卡放到一个轮询表上

> 2. 只要网卡上还有分组需要处理，内核就一直对轮询表上的设备进行轮询

> 3. 重新启用Rx中断

* 只有网卡满足如下两个条件时，才能实现NAPI方法
> 1. 设备必须能够保留多个接收的分组，例如保存到DMA环形缓冲区

> 2. 设备必须能禁用用于分组接收的IRQ

* 支持NAPI的设备必须提供一个poll函数
> netif_napi_add来设置poll, 使用它来注册poll，表明设备可以且必须使用NAPI处理

> poll函数是用来在IRQ禁用期间用来轮询设备

> netif_napi_add的参数weight指定了设备权重，权重不能超过该设备可以在Rx缓冲区中存储的分组数目

> 假设需要处理的分组数大于weight, 因一次轮询最多处理weight个分组，所以一次轮询不够的话，内核会将该设备移动到轮询表末尾，在链表中所有其他设备都处理后，继续轮询该设备

### 12.7.3 发送分组
* 当信息从计算机发送出去时，必须注意哪些事？
> 分组的路由是最重要的

> 路由只能由高层协议决定，网卡的驱动决定不了

* net/core/dev.c中的dev_queue_xmit用于将分组放到发出分组的队列上, 在放到等待队列上一定时间后，分组将会被发出
> 这是通过特定于网卡的函数hard_start_xmit完成的，在每个net_device结构中以函数指针形式出现，由网卡驱动程序实现

## 12.8 网络层
> 该层不仅负责发送和接收数据，还负责在彼此不直接连接的系统间转发和路由分组

> 查找最佳路由并选择适当的网络设备来发送分组，涉及对地址族处理，这是该层与网卡松散关联的原因

### 12.8.1 IPv4
### 12.8.2 接收分组
> 判断该分组的目的地是本地系统还是远程计算机，根据对分组目的地的判断，需要将分组转发到更高层，或转发到网络上的另一台计算机

### 12.8.3 交付到本地传输层
> 内核在一个独立的缓存中管理原本属于一个分组的各个分片，该缓存称为分片缓存

> 在分片缓存中，属于同一分组的各个分片保存在一个独立的等待队列中，直到该分组的所有分片都到达

> 在分组的所有分片都进入缓存后，ip_frag_reasm将各个分片重新组合起来

### 12.8.4 分组转发
* 分组的目标地址可分为以下两类：
> 1. 目标计算机在某个本地网络中，发送计算机与该网络有连接

> 2. 目标计算机在地理上属于远程计算机，不连接到本地网络，只能通过网关访问

* ip_route_input函数能够找到通向目标地址的路由

### 12.8.5 发送分组
* 内核中比较常用的通过互联网层发送数据的函数ip_queue_xmit
* 分组长度不大于传输介质MTU，则不需要分片
* ARP协议负责将IP地址转换成硬件地址

### 12.8.6 netfilter
* 1. 扩展网络功能
* 2. 调用挂钩函数
* 3. 扫描挂钩表
* 4. 激活挂钩函数

### 12.8.7 Ipv6
* 采用全新分组格式, 128位的IP地址

## 12.9 传输层
### 12.9.1 UDP
### 12.9.2 TCP
#### 1. TCP首部
#### 2. 接收TCP数据 
#### 3. 三次握手
* 在可以使用TCP链路前，必须在客户端和主机之间建立连接

* 建立TCP连接的过程需要交换3个TCP分组，因而称为三次握手
> 1. 客户端通过向服务器发送SYN来发出连接请求。客户端的套接字状态由CLOSED变为SYN_SENT

> 2. 服务器在一个监听套接字上接收到连接请求，并返回SYN和ACK。服务器套接字的状态由LISTEN变为SYN_RECV

> 3. 客户端套接字接收到SYN/ACK分组后，切换到ESTABLISHED状态，表明连接已经建立。一个ACK分组被发送到服务器

> 4. 服务器收到ACK分组，也切换到ESTABLIHED状态。这就完成了两端的连接建立工作，可以开始数据交换了

* 网络连接数据可以用tcpdump, wireshark之类的工具捕获
#### 4. 被动建立连接
#### 5. 主动建立连接
#### 6. 分组传输
* TCP没有提供显式的重传请求机制
> 换句话说，接收方不能请求发送方重传丢失的分组。如果在一定的超时时间内发送方没有收到确认，则发送丢失的部分数据是发送方的责任

#### 8. 发送分组
> tcp分组的发送由高层调用tcp_sendmsg开始

> 发送必须遵守下列需求

* 1. 接收方等待队列上必须有足够的空间可用于该数据
* 2. 必须实现防止连接拥塞的ECN机制
* 3. 必须检测某一方出现失效的情况，以免通信出现停顿
* 4. TCP慢启动机制要求在通信开始时，逐渐增大分组长度
* 5. 发送但未确认的分组，必须在一定的超时时间间隔之后反复重传，直到接收方最终确认
> 重传是TCP连接进行可靠传输的关键要素，它的工作机制如下

```
1) tcp_snd_test检查目前是否可以发送数据。接收方过载导致的分组积压，可能使得现在无法发送数据
2) tcp_transmit_skb使用地址族相关的af_specific->queue_xmit函数，将数据转发到网络层
3) update_send_head处理对统计量的更新。更重要的是，它会初始化所发送tcp信息段的重传定时器。不必对每个tcp分组都这样做，该机制只用于已经确认的数据区之后的第一个分组
4) inet_csk_reset_xmit_timer负责重置重传定时器。该定时器是未确认分组重发的基础，是tcp传输的一种保证。
内核使用的超时函数是tcp_write_timer，如果没有收到ack，该函数会调用tcp_retransmit_timer函数

在重传数据时，必须注意以下问题：
1. 连接在此期间可能已经关闭。在这种情况下，保存的分组和定时器将从内核内存中删除
2. 如果重传尝试的次数超过了sysctl_tcp_retries2变量指定的限制，则放弃重传

如上操作后，在收到ACK之后，删除相应分组的重传定时器
```

## 12.10 应用层
## 12.11 内核内部的网络通信
## 12.12 小结 


# 附录D 系统启动    
## 启动阶段分为以下3个部分
* 1. 内核载入物理内存，并创建最小化的运行时环境
* 2. 转移到内核中（平台相关）的机器码，并初始化基本的系统功能，初始化代码是特定于系统的，用汇编语言编写
* 3. 转移到初始化代码中平台无关的部分，用C语言编写，并完成所有子系统的初始化，最后切换到正常动作模式

## D.1 IA-32 系统上与体系结构相关的设置
* 1) 启动装载器(LILO、GRUP等)将内核载入内存，之后切换控制流跳转到内存的适当位置，来调用arch/x86/boot/header.S中的汇编语言函数setup（该函数总是位于目标文件的同一位置）

* 2) setup汇编函数执行以下内容
> (1) 检查内核是否加载到内存中正确的位置

> (2) 它确定系统内存的大小

> (3) 初始化显卡

> (4) 将内核映像移动到内存中的某个位置

> (5) 将CPU切换到保护模式

* 3) startup_32（arch/x86/boot/compressed/head_32.S）
> (1) 创建一个临时的内存栈

> (2) 用0字节填充未初始化的内核数据，相关的区域位于_edata和_end之间。在内核链接时，会根据内核二进制文件，自动对这些常数设置正确的值

> (3) 调用arch/x86/boot/compressed/misc_32.c中的C语言函数decompress_kernel。该函数将解压缩内核。

* 4) startup_32（arch/x86/kernel/head_32.S）这是特定于处理器初始化过程的最后一部分，它负责以下内容
> (1) 激活分页模式，设置一个最终的内核栈

> (2) 用0字节填充__bss_start, __bss_end之间的.bss段

> (3) 初始化中断描述符表（但所有中断处理程序都设置为ignore_init空例程，实际的处理程序在后面安装）

> (4) 检测处理器类型

## D.2 高层初始化(start_kernel)
```c
start_kernel
	|------->显示版本信息
	|------->特定于体系结构的、内存管理的高层设置
	|------->计算命令行参数
	|------->初始化大多数子系统的核心数据结构
	|------->确定处理器和系统错误
	|------->启动idle进程和init线程

/*****************************************************************/

start_kernel
	|------->setup_arch(&command_line);
		|------>确定内核在内存中的位置
		|------>parse_early_param, 解析命令行参数中内存相关内容，如物理内存总长度、BIOS内存区位置等
		|------>setup_memory检测高、低端内存区域中物理内存页数目;初始化bootmem分配器
		|------>paging_init设置内核参考页表，该页表用于映射物理内存、管理vmalloc区域;新页表的启用是通过：将页表地址写入cpu的CR3寄存器实现
	|------->build_all_zonelists(NULL);内存管理子系统初始化	
```

### 初始化中枢的数据结构和缓存
* start_kernel的大多数实质性任务是初始化内核子系统
```c
asmlinkage void __init start_kernel(void)     	/*函数定义前加宏asmlinkage ,表示这些函数通过堆栈而不是通过寄存器传递参数。 */
{
	...
	trap_init();  			/* 设置异常处理程序*/
	rcu_init();
	init_IRQ();  			/* 设置中断处理程序*/
	pidhash_init();			/* 分配散列表*/
	sched_init();
	init_timers();
	hrttimers_init();
	softirq_init();			/* 注册用于普通和高优先级tasklet的软中断队列*/
	timekeeping_init();
	time_init();			/* 从硬件时钟读取系统时间*/
	profile_init();
	...
	early_boot_irqs_on();
	local_irq_enable();
	...
	console_init();
	...
	mem_init();			/* 停用bootmem分配器*/
	kmem_cache_init();
	...
	calibrate_delay();		/* 计算每个jiffy期间可以执行多少个空循环;内核需要使用该值来估算一些轮询和忙等待任务所需要的时间*/
	pidmap_init();
	pgtable_cache_init();
	...
	thread_stack_cache_init();                                                                                                                                        
        cred_init();
        fork_init();
        proc_caches_init();
        uts_ns_init();
        buffer_init();			/* 创建buffer_head缓存，并计算max_buffer_heads值，这样缓存头所占用的缓存绝不会超过ZONE_NORMAL的10%*/
        key_init();
        security_init();
        dbg_late_init();
        vfs_caches_init();		/* 创建虚拟文件系统所需要的数据结构缓存*/
        pagecache_init();
        signals_init();
        seq_file_init();
        proc_root_init();		/* 初始化proc文件系统的inode缓存，在内核中注册proc文件系统，并生成核心的文件系统项，如/proc/meminfo, /proc/version, /proc/uptime*/
        nsfs_init();
        cpuset_init();
        cgroup_init();
        taskstats_init_early();
        delayacct_init();	

	...
}

void __init trap_init(void)
{
	set_trap_gate(0, &divide_error);
	set_intr_gate(1, &debug);
	set_intr_gate(2, &nmi);
	set_system_gate(4, &overflow);
	set_system_gate(5, &bounds);
	set_trap_gate(6, &invalid_op);
	set_trap_gate(7, &device_not_available);
	set_trap_gate(8, GDT_ENTRY_DOUBLEFAULT_TSS);
	set_trap_gate(9, &coprocessor_segment_overrun);
	set_trap_gate(10, &invalid_TSS);
	set_trap_gate(11, &segment_not_present);
	set_trap_gate(12, &stack_segment);
	set_trap_gate(13, &general_protection);
	set_intr_gate(14, &page_fault);
	set_trap_gate(15, &spurious_interrupt_bug);
	set_trap_gate(16, &coprocessor_error);
	set_trap_gate(17, &alignment_check);
	set_trap_gate(19, &simd_coprocessor_error);
	...
	set_system_gate(SYSCALL_VECTOR, &system_call);	
	...

}
```
### 查找已知的系统错误
> 软件不是唯一会出现bug的东西，处理器实现也会发生错误，芯片也可能无法像预期的那样工作，幸运的时，大多数错误情形都可以规避补救。但在规避之前，内核必须知道特定的处理器是否确实有bug，这可以使用体系结构相关的check_bugs函数确定

```c
/* 例如, IA-32系统上该函数代码如下*/
static void __init check_bugs(void)
{
	identify_boot_cpu();
	check_config();
	check_fpu();
	check_hlt();
	check_popad();
	init_utsname()->machine[1] = '0' + (boot_cpu_data.x86 > 6 ? 6 : boot_cpu_data.x86);
	alternative_instructions();
}
```

### idle和init线程
> start_kernel最后一个操作有如下两个步骤
 
* 1. rest_init启动一个新线程，并调用用户空间初始化程序/sbin/init

* 2. 第一个内核线程变为idle线程，在系统无事可做时调用

#### rest_init的实现
```c
/*init/main.c*/
static void rest_init(void)
{
	kernel_thread(kernel_init, NULL, CLONE_FS | CLONE_SIGHAND);
	pid = kernel_thread(kthreadd, NULL, CLONE_FS | CLONE_FILES);
	kthreadd_task = find_task_by_pid(pid);
	unlock_kernel();
	schedule();
	cpu_idle();
}
```

#### init线程代码流程
```c
init()
	|------>注册为孤儿子进程收割者, 在kernel_init()中设置的
	|------>SMP初始化（到这里为止，内核在多处理器系统上只使用了几个CPU中的一个，现在需要激活其他CPU，通过smp_prepare_cpus, do_pre_smp_initcalls, smp_init三步实现）
		|--------->smp_prepare_cpus确保激活剩余cpu，但这些cpu还未关联到内核的调度机制，所以暂时还是不可用的
		|--------->do_pre_smp_initcalls初始化迁移队列
		|--------->smp_init在内核中启用剩余cpu，使之可用
	|------>do_basic_setup, 开始驱动程序和子系统的一般初始化工作
		|---------->init_workqueues创建事件工作队列
		|---------->usermodehelper_init创建khelper工作队列
		|---------->driver_init调用通用驱动程序模型的数据结构
		|---------->init_irq_proc在proc文件系统中注册关于IRQ的信息
		|---------->do_initcalls负责调用驱动程序相关初始化函数。因为内核是定制配置的，所以必须提供一种机制，来明确需要调用哪些函数及调用顺序，这称为initcall机制
	|------>prepare_namespace
	|------>init_post
		|---------->free_initmem用于释放__init_begin和__init_end之间的内核内存
		|---------->执行用户空间初始化程序init
```		
#### 删除初始化数据
* 用于初始化的函数和数据通常只在内核启动时需要，决不会再次调用。为明确表明这一点，内核定义了__init属性
```c
#define __init 		__attribute__ ((__section__ (".init.text"))) __cold
#define __initdata 	__attribute__ ((__section__ (".init.data"))) __cold

```
> 链接器会将标记为__init的函数和__initdata数据放写入到二进制文件的特定段中

#### 用户空间初始化
* init线程的最后一个工作是调用init_post，该函数会启动一个程序在用户空间继续初始化，以便向用户提供一个可工作的系统。在unix/linux系统中，传统上将该任务委托给/sbin/init
```c
/* init/main.c*/
static int noinline init_post(void)			
{
	if (execute_command) {
		run_init_process(execute_command);
		printk();
	}
	run_init_process("/sbin/init");			/*ubuntu20.04, kernel 5.4版本里，它是指向/lib/systemd/systemd的软链接*/
	run_init_process("/ect/init");			/*ubuntu20.04, kernel 5.4版本里, 这是个目录*/
	run_init_process("/bin/init");			/*ubuntu20.04, kernel 5.4版本里, 没有此文件*/
	run_init_process("/bin/sh");			/*ubuntu20.04, kernel 5.4版本里, 它是指向/bin/dash的软链接*/
}


static const char *argv_init[MAX_INIT_ARGS+2] = { "init", NULL, };
const char *envp_init[MAX_INIT_ENVS+2] = { "HOME=/", "TERM=linux", NULL, };

static void run_init_process(char *init_filename)			/*run_init_process会设置一个最低限度的环境，以便init过程运行*/
{
	argv_init[0] = init_filename;
	kernel_execve(init_filename, argv_init, envp_init);
}
```

# 附录E ELF二进制格式
## E 1.1 ELF头
```
Magic：   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
类别:                              ELF64
数据:                              2 补码，小端序 (little endian)
Version:                           1 (current)
OS/ABI:                            UNIX - System V
ABI 版本:                          0
类型:                              DYN (共享目标文件)
系统架构:                          Advanced Micro Devices X86-64
版本:                              0x1
入口点地址：               0x5cd0
程序头起点：          64 (bytes into file)
Start of section headers:          127896 (bytes into file)
标志：             0x0
Size of this header:               64 (bytes)
Size of program headers:           56 (bytes)
Number of program headers:         13
Size of section headers:           64 (bytes)
Number of section headers:         30
Section header string table index: 29
```

* 在文件起始处，有四个字节标识，在7f之后的45 4c 46它们是ELF的ASCII码值

## E 1.2 程序头表
```
~> readelf -l /bin/dash

Elf 文件类型为 DYN (共享目标文件)
Entry point 0x5cd0
There are 13 program headers, starting at offset 64

程序头：
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  PHDR           0x0000000000000040 0x0000000000000040 0x0000000000000040
                 0x00000000000002d8 0x00000000000002d8  R      0x8
  INTERP         0x0000000000000318 0x0000000000000318 0x0000000000000318
                 0x000000000000001c 0x000000000000001c  R      0x1
      [Requesting program interpreter: /lib64/ld-linux-x86-64.so.2]
  LOAD           0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000004360 0x0000000000004360  R      0x1000
  LOAD           0x0000000000005000 0x0000000000005000 0x0000000000005000
                 0x00000000000127f5 0x00000000000127f5  R E    0x1000
  LOAD           0x0000000000018000 0x0000000000018000 0x0000000000018000
                 0x00000000000058f0 0x00000000000058f0  R      0x1000
  LOAD           0x000000000001df30 0x000000000001ef30 0x000000000001ef30
                 0x0000000000001310 0x0000000000003f40  RW     0x1000
  DYNAMIC        0x000000000001eb08 0x000000000001fb08 0x000000000001fb08
                 0x00000000000001f0 0x00000000000001f0  RW     0x8
  NOTE           0x0000000000000338 0x0000000000000338 0x0000000000000338
                 0x0000000000000020 0x0000000000000020  R      0x8
  NOTE           0x0000000000000358 0x0000000000000358 0x0000000000000358
                 0x0000000000000044 0x0000000000000044  R      0x4
  GNU_PROPERTY   0x0000000000000338 0x0000000000000338 0x0000000000000338
                 0x0000000000000020 0x0000000000000020  R      0x8
  GNU_EH_FRAME   0x0000000000019f44 0x0000000000019f44 0x0000000000019f44
                 0x0000000000000804 0x0000000000000804  R      0x4
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     0x10
  GNU_RELRO      0x000000000001df30 0x000000000001ef30 0x000000000001ef30
                 0x00000000000010d0 0x00000000000010d0  R      0x1
```
* PHDR
> 保存程序头表

* INTERP
> 在程序已经从可执行文件映射到内存后，指定必须调用的解释器

> 解释器并不是说二进制文件的内容必须由另一个程序解释

> 解释器指的是这样的程序：通过链接其他库，来解决未定义的引用

> 动态链接器/lib64/ld-linux-x86-64.so.2用于在虚拟地址空间中插入程序运行所需要的动态库

* LOAD
> 表示一个需要从二进制文件映射到虚拟地址空间的段，其中保存了常量数据、程序的目标代码

* DYNAMIC
> 保存了由动态链接器ld-linux-x86-64.so所需要的信息

### 段是可以重叠的
> ELF标准允许这种行为


## E 1.3 节
```
~> readelf -S /bin/dash
There are 30 section headers, starting at offset 0x1f398:

节头：
  [号] 名称              类型             地址              偏移量
       大小              全体大小          旗标   链接   信息   对齐
  [ 0]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0     0     0
  [ 1] .interp           PROGBITS         0000000000000318  00000318
       000000000000001c  0000000000000000   A       0     0     1
  [ 2] .note.gnu.propert NOTE             0000000000000338  00000338
       0000000000000020  0000000000000000   A       0     0     8
  [ 3] .note.gnu.build-i NOTE             0000000000000358  00000358
       0000000000000024  0000000000000000   A       0     0     4
  [ 4] .note.ABI-tag     NOTE             000000000000037c  0000037c
       0000000000000020  0000000000000000   A       0     0     4
  [ 5] .gnu.hash         GNU_HASH         00000000000003a0  000003a0
       00000000000002d8  0000000000000000   A       6     0     8
  [ 6] .dynsym           DYNSYM           0000000000000678  00000678
       0000000000001158  0000000000000018   A       7     1     8
  [ 7] .dynstr           STRTAB           00000000000017d0  000017d0
       00000000000006b6  0000000000000000   A       0     0     1
  [ 8] .gnu.version      VERSYM           0000000000001e86  00001e86
       0000000000000172  0000000000000002   A       6     0     2
  [ 9] .gnu.version_r    VERNEED          0000000000001ff8  00001ff8
       0000000000000070  0000000000000000   A       7     1     8
  [10] .rela.dyn         RELA             0000000000002068  00002068
       0000000000001b00  0000000000000018   A       6     0     8
  [11] .rela.plt         RELA             0000000000003b68  00003b68
       00000000000007f8  0000000000000018  AI       6    25     8
  [12] .init             PROGBITS         0000000000005000  00005000
       000000000000001b  0000000000000000  AX       0     0     4
  [13] .plt              PROGBITS         0000000000005020  00005020
       0000000000000560  0000000000000010  AX       0     0     16
  [14] .plt.got          PROGBITS         0000000000005580  00005580
       0000000000000020  0000000000000010  AX       0     0     16
  [15] .plt.sec          PROGBITS         00000000000055a0  000055a0
       0000000000000550  0000000000000010  AX       0     0     16
  [16] .text             PROGBITS         0000000000005af0  00005af0
       0000000000011cf5  0000000000000000  AX       0     0     16
  [17] .fini             PROGBITS         00000000000177e8  000177e8
       000000000000000d  0000000000000000  AX       0     0     4
  [18] .rodata           PROGBITS         0000000000018000  00018000
       0000000000001f42  0000000000000000   A       0     0     32
  [19] .eh_frame_hdr     PROGBITS         0000000000019f44  00019f44
       0000000000000804  0000000000000000   A       0     0     4
  [20] .eh_frame         PROGBITS         000000000001a748  0001a748
       00000000000031a8  0000000000000000   A       0     0     8
  [21] .init_array       INIT_ARRAY       000000000001ef30  0001df30
       0000000000000008  0000000000000008  WA       0     0     8
  [22] .fini_array       FINI_ARRAY       000000000001ef38  0001df38
       0000000000000008  0000000000000008  WA       0     0     8
  [23] .data.rel.ro      PROGBITS         000000000001ef40  0001df40
       0000000000000bc8  0000000000000000  WA       0     0     32
  [24] .dynamic          DYNAMIC          000000000001fb08  0001eb08
       00000000000001f0  0000000000000010  WA       7     0     8
  [25] .got              PROGBITS         000000000001fcf8  0001ecf8
       00000000000002f0  0000000000000008  WA       0     0     8
  [26] .data             PROGBITS         0000000000020000  0001f000
       0000000000000240  0000000000000000  WA       0     0     32
  [27] .bss              NOBITS           0000000000020240  0001f240
       0000000000002c30  0000000000000000  WA       0     0     32
  [28] .gnu_debuglink    PROGBITS         0000000000000000  0001f240
       0000000000000034  0000000000000000           0     0     4
  [29] .shstrtab         STRTAB           0000000000000000  0001f274
       000000000000011d  0000000000000000           0     0     1

```
* starting at offset 0x1f398
> 偏移量0x1f398指的是相对于二进制文件的

* 节信息不会复制到进程的虚拟地址空间
> 尽管如此，节信息在二进制文件中还是需要的

* 节信息中的类型信息为PROGBITS的是程序必须解释的信息
> 例如，二进制代码

* 29个节中一些有具体意义的节
> .interp保存了解释器的文件名

> .data保存了初始化过的数据, 普通程序数据，在程序运行时可修改

> .rodata保存了只读数据

> .init和.fini保存了进程初始化和结束的代码。这两个节通常由编译器自动添加, 无需应用程序员关注

> .hash是一个散列表，用于快速访问符号表

## E 1.4 符号表
> 符号表保存了ELF格式的程序实现或使用的所有变量和函数

* 如果程序引用了一个自身未定义的符号，则称为未定义符号
> 此类引用必须在静态链接期间用其他目标模块或库解决，或在加载时通过动态链接（ld-linux.so）解决

* 以下是执行readelf -s /bin/dash的部分结果
```
Symbol table '.dynsym' contains 185 entries:
Num:    Value          Size Type    Bind   Vis      Ndx Name
97: 00000000000200d0     8 OBJECT  GLOBAL DEFAULT   26 out1
98: 0000000000022d30     4 OBJECT  GLOBAL DEFAULT   27 tokpushback
99: 00000000000200c8     8 OBJECT  GLOBAL DEFAULT   26 out2
...
133: 0000000000020240     8 OBJECT  GLOBAL DEFAULT   27 __environ@GLIBC_2.2.5 (2)
134: 00000000000209c8     4 OBJECT  GLOBAL DEFAULT   27 loopnest
135: 0000000000022e58     8 OBJECT  GLOBAL DEFAULT   27 funcblock
136: 0000000000022d80     8 OBJECT  GLOBAL DEFAULT   27 redirlist
137: 0000000000022d58     8 OBJECT  GLOBAL DEFAULT   27 heredoclist
138: 0000000000022abc     4 OBJECT  GLOBAL DEFAULT   27 rootpid
139: 0000000000022d70     8 OBJECT  GLOBAL DEFAULT   27 heredoc
140: 0000000000019d20   258 OBJECT  GLOBAL DEFAULT   18 dqsyntax
141: 00000000000209cc     4 OBJECT  GLOBAL DEFAULT   27 back_exitstatus
142: 00000000000209c0     4 OBJECT  GLOBAL DEFAULT   27 exception
143: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND __cxa_finalize@GLIBC_2.2.5 (2)
```

> 上面的143行中__cxa_finalize@GLIBC_2.2.5表示函数__cxa_finalize要求Glibc版本最低是2.2.5

> out1, out2函数符号已经移到虚拟地址空间中的固定位置(在文件加载时，对应的代码将映射到这些位置) 

* ELF是如何实现符号表机制的？	

## E 2 内核中的数据结构
* ELF是一个处理器和体系结构无关的格式
> 内核定义了统一的数据结构，在所有体系结构上具有相同的位宽

```c
/* elf.h */
/* 以32位为例介绍 */
typedef __u32 Elf32_Addr;
typedef __u16 Elf32_Half;
typedef __u32 Elf32_Off;
typedef __s32 Elf32_Sword;
typedef __u32 Elf32_Word;
```

## E 2.2 头部
* 1. ELF头
> 在32位体系结构上，ELF头如下定义

```c
/* elf.h */
typedef struct elf32_hdr {
	unsigned char e_ident[EI_NIDENT];	/* ELF魔数
						 * e_ident共16字节，即EI_NIDENT=16
						 * 1) 前四个字节是0x7F和字母E、L、F的ASCII码值
						 * 2) EI_CLASS=4, 标识文件类型
						 * 3) EI_DATA=5, 字节序
						 * 4) EI_VERSION=6,
						 * 5) 从EI_PAD=7开始，ELF并没有的标识部分剩余的字节用0填充 
						 */
	Elf32_Half	e_type;
	Elf32_Half	e_machine;		/* 文件所需的体系结构*/
 	Elf32_Word      e_version;
        Elf32_Addr      e_entry;  		/* 文件在虚拟内存中的入口点 
						 * 它是程序已经加载并映射到内存后开始执行的位置  
						 */
        Elf32_Off       e_phoff;		/* 程序头表在文件中的偏移量 */
        Elf32_Off       e_shoff;		/* 节头表在文件中的偏移量 */
        Elf32_Word      e_flags;
        Elf32_Half      e_ehsize;
        Elf32_Half      e_phentsize;
        Elf32_Half      e_phnum;
        Elf32_Half      e_shentsize;
        Elf32_Half      e_shnum;
        Elf32_Half      e_shstrndx;
	...
} ELf32_Ehdr;

/* 对应的64位结构elf64_hdr不再介绍 */

```

* 2. 程序头表
> 通过数组实现，每个数组项的数据结构如下定义（它分成32位版本和64位版本）
```c
/* elf.h */
/* 在32位体系结构上，如下定义 */
typedef struct elf32_phdr {
	Elf32_Word    p_type;
   	Elf32_Off     p_offset; 	/* 段在二进制文件中的偏移量*/
   	Elf32_Addr    p_vaddr;		/* 段的数据映射到虚拟地址空间的位置 */
   	Elf32_Addr    p_paddr;		/* 不支持虚拟寻址的系统使用paddr*/
   	Elf32_Word    p_filesz;
   	Elf32_Word    p_memsz;
   	Elf32_Word    p_flags;
   	Elf32_Word    p_align;	
} Elf32_Phdr;

/* 对应的64位体系结构elf64_phdr不再介绍 */
```

* 3. 节头表
> 通过数组实现，每个数组项由下列结构定义

> 每个数组项即为一节的信息

```c
typedef struct {
	Elf32_Word    sh_name;
        Elf32_Word    sh_type;		/* 节的类型: 
					 * SH_NULL
					 * SH_PROGBITS保存程序相关信息
					 * SH_SYMTAB保存一个符号表
					 * SH_STRTAB字符串表
					 * SH_RELA和SHT_RELA保存重定位信息
					 * SH_HASH散列表，其作用是更快速的查找符号表中的项
					 */
        Elf32_Word    sh_flags;
        Elf32_Addr    sh_addr;		/* 节映射到虚拟地址空间的位置*/
        Elf32_Off     sh_offset;	/* 节在二进制文件中的开始位置*/
        Elf32_Word    sh_size;		/* 节的长度*/
        Elf32_Word    sh_link;		/* 另一个节的索引*/
        Elf32_Word    sh_info;		/* 其他节的信息*/
        Elf32_Word    sh_addralign;	/* 节的对齐值*/
        Elf32_Word    sh_entsize;	/* 如果节存放的是表，则为各个表项的长度*/
} Elf32_Shdr;
```
> ELF定义了若干固定名称的节，用于执行大多数目标文件所需要的标准任务

> 最重要的标准节如下所示

```
.bss	# 保存程序未初始化的数据节，在程序开始运行前填充0字节
.data	# 包含已经初始化的程序数据，例如，预行初始化的结构，其中在编译时填充了静态数据, 这些数据可以在程序运行期间更改
.rodata	# 保存了程序使用的只读数据
.dynamic和.dynstr	# 保存了动态信息
.interp		#保存了程序解释器的名称
.shstrtab	# 字符串表，节名称
.strtab		# 字符串表，主要包含符号表需要的各个字符串
.symtab		# 二进制文件符号表
.init和.fini	# 保存了程序初始化和结束时执行的机器指令；这两个节的内容通常是由编译器或其辅助工具自动创建的，主要是为程序建立一个适当的运行时环境
.text		# 保存了主要的机器指令

```

## E 2.3 字符串表
> 因为其格式非常动态，内核不能提供一个固定的数据结构，必须手工分析数据

## # 2.4 符号表
* 符号表保存了查找程序符号、为符号赋值、重定位符号所需要的全部信息
> 符号的主要任务是将一个字符串和一个值关联起来

> 例如，print符号表示printf函数在虚拟地址空间中的地址，该函数的机器代码就存放在该地址

```c
typedef struct elf32_sym {
	Elf32_Word    st_name;
	Elf32_Addr    st_value;
	Elf32_Word    st_size;		/* 指定对象长度*/
	unsigned char st_info;		/* 一个符号的确切用途由st_info定义
					 * 1)符号的绑定
					 * 	(1) 局部符号, 只在目标文件内部可见；如果一个程序的几个目标文件定义相同的局部符号，是可以的。局部符号不会在目标文件间彼此干扰
					 * 	(2) 全书符号，在定义的目标文件内部可见，也可以被其他目标文件引用。每个全局符号在一个程序内部只能定义一次。指向全局符号的未定义引用，将在重定位期间确定相关符号的位置。如果对全局符号的未定义引用无法解决，则拒绝程序执行或静态绑定
					 * 	(3) 弱符号，在整个程序内可见。但可以有多个定义。如果全局符号与弱符号名称相同，则优先处理全局符号。
					 * 2)符号的类型
					 * 	(1) STT_OBJECT, 表示符号关联到一个数据对象
					 *	(2) STT_FUNC，表示符号关联到一个函数或过程
					 * 	(3) STT_NOTYPE，符号类型未指定，用于未定义引用
					 */
	unsigned char st_other;
	Elf32_Half    st_shndx;		/* 保存一个节的索引, 符号将保存到该节*/
} Elf32_Sym;
```
> 可以使用readelf -s someelfile来查看程序的符号表中所有的符号

## E 2.5 重定位项
```
$readelf -r /bin/dash

重定位节 '.rela.dyn' at offset 0x2068 contains 288 entries:
  偏移量          信息           类型           符号值        符号名称 + 加数
00000001ef30  000000000008 R_X86_64_RELATIVE                    5db0
00000001ef38  000000000008 R_X86_64_RELATIVE                    5d70
00000001ef40  000000000008 R_X86_64_RELATIVE                    18ac8
00000001ef58  000000000008 R_X86_64_RELATIVE                    18ad6
00000001ef70  000000000008 R_X86_64_RELATIVE                    18ae3
00000001ef88  000000000008 R_X86_64_RELATIVE                    18af0

```
> 在程序运行或链接目标文件产生可执行文件时，如果某些机器代码引用了虚拟地址空间中位置尚不明确的函数或符号，则将使用Offset列的信息

> 函数或符号地址确定后，必须将其插入到指定的偏移量处，以便能够生成正确运行的可执行代码

* 重定位是将ELF文件中未定义符号关联到有效值的处理过程
> 就是将目标文件中未定义的引用替换为该进程的虚拟地址空间中适当的机器代码所在的地址

* 在每个目标文件中都有一个包含重定位项(标识需要进行重定位之处)的表

### 1. 数据结构
* 普通重定位
```c
typedef struct elf32_rel {		/*在该数据结构中虽然未定义"加数值r_addend"，但链接器根据该值应该在内存中出现的位置，将计算出的重定空位长度作为加数填入*/
	Elf32_Addr    r_offset;		/*应用重定位操作的位置*/
	Elf32_Word    r_info;		/*重定位类型和符号表索引*/
} Elf32_Rel;
```
* 添加常数重定位
```c
typedef struct elf32_rela {
	Elf32_Addr    r_offset;
  	Elf32_Word    r_info;
  	Elf32_Sword   r_addend;		/*在计算重定位值时，将根据不同的重定位类型，对该值进行不同的处理*/
} Elf32_Rela;
```

### 2. 重定位类型
> ELF标准支持很多种重定位类型

> 这些类型大部分用于生成动态链接库或与装载位置无关的代码

> Linux内核只对模块重定位感兴趣，与之相关的重定位类型主要有：相对重定位、绝对重定位

* 相对重定位
> 1. 相对重定位生成的重定位表项指向相对于PC(程序计数器，指令指针)指定的内存地址

> 2. 主要用于例程调用

> 3. 在IA-32上，其对应的类型为：R_386_PC32

> 4. 重定位计算结果：Result = S - P + A

```
A代表加数值
S是符号表中保存的符号值, 即add函数在内存的位置
P代表重新定位的位置偏移量, 即算出的数据写入到二进制文件中的位置偏移量
```

> 相对位移的例子

```
45: e8 fc ff ff ff call 46 <main+0x2c>		# e8是call的操作码, 0xfffffffc是传递给call的参数的值, 0xfffffffc对应的值为-4, 它就是加数值

##############################
# 如重定位表所示，重定位位置46是add函数调用
00000046 00000702 R_386_PC32 00000000 add
# 因为二进制文件的节在重定位之前就会移动到其在内存中的最终位置，add函数在内存的位置是已知的，如果add位于0x08048388，那么main函数应该在位置0x080483a2，这意味着定位结果应该写入到重定位位置0x80483ce

# 重定位结果使用相对重定位的公式计算：Result = S - P - A = 0x08048388 - 0x80483ce + (-4) = -74
# -74 对应于可执行文件test中的代码，可以使用objdump证实
80483cd: e8 b6 ff ff ff call 8048388 <add>
```

> 程序计数器总是指向当前执行语句之后的下一个语句，因而如果处理器在机器代码中从相对地址计算绝对跳转地址，会多出4字节.因此编译器必须从相对跳转地址扣除4字节，使得程序能够跳转到正确位置

* 绝对重定位
> 1.绝对重定位生成绝对地址，通常这种重定位项指向内存中在编译时就已知的数据，例如字符串常数 

> 2. 在IA-32上，其对应的类型为：R_386_32

> 3. 重定位计算结果：Result = S + A

> 4. 绝对重定位采用的方案与相对重定位的方案相同，但计算更简单，它只需要将目标符号的地址与加数值相加即可

## E 2.6 动态链接
* 内核对必须与库动态链接才能运行的ELF文件不感兴趣
* 模块中的所有引用都可以通过重定位解决
* 用户空间程序的动态链接则完全由用户空间中的ld.so完成
* .dynsym保存了有关符号表，包含了所有需要通过外部引用解决的符号
* .dynamic保存了一个数组，数组项为Elf32_Dyn类型，这些项提供了以下几个段落所描述的信息
```
Symbol table '.dynsym' contains 185 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND
     1: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND sigprocmask@GLIBC_2.2.5 (2)
     2: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND raise@GLIBC_2.2.5 (2)
     3: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND free@GLIBC_2.2.5 (2)
     4: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND strcasecmp@GLIBC_2.2.5 (2)
     5: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND abort@GLIBC_2.2.5 (2)
     6: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND __errno_location@GLIBC_2.2.5 (2)
     7: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterTMCloneTab
     8: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND _exit@GLIBC_2.2.5 (2)
     9: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND strcpy@GLIBC_2.2.5 (2)
...

# GLIBC_2.2.5表示必须至少使用GNU标准库的2.2.5版本才能解决这些引用 
```
> dynamic节中的数组项的数据类型在内核中定义如下(但根本没有使用，因为该信息在用户空间解释)

```c
<elf.h>
typdef struct dynamic {
	Elf32_Sword d_tag;	/* 用于区分各种指定信息类型的标记, 
				 *  DT_NEEDED,指定该程序执行所需要的一个动态库
				 * DT_STRTAB
				 * DT_SYMTAB
				 * DT_INIT
				 * DT_FINI
				 */ 
	union{
	  Elf32_Sword d_val;
	  Elf32_Addr  d_ptr;
	} d_un;

} Elf32_Dyn;

readelf --dynamic /usr/local/bin/vim

Dynamic section at offset 0x3b0e18 contains 47 entries:
  标记        类型                         名称/值
 0x0000000000000001 (NEEDED)             共享库：[libgtk-x11-2.0.so.0]
 0x0000000000000001 (NEEDED)             共享库：[libgdk-x11-2.0.so.0]
 0x0000000000000001 (NEEDED)             共享库：[libgdk_pixbuf-2.0.so.0]
 0x0000000000000001 (NEEDED)             共享库：[libgio-2.0.so.0]
 0x0000000000000001 (NEEDED)             共享库：[libpango-1.0.so.0]
 0x0000000000000001 (NEEDED)             共享库：[libgobject-2.0.so.0]
 0x0000000000000001 (NEEDED)             共享库：[libglib-2.0.so.0]
 0x0000000000000001 (NEEDED)             共享库：[libSM.so.6]
 0x0000000000000001 (NEEDED)             共享库：[libICE.so.6]
 0x0000000000000001 (NEEDED)             共享库：[libXt.so.6]
 0x0000000000000001 (NEEDED)             共享库：[libX11.so.6]
 0x0000000000000001 (NEEDED)             共享库：[libm.so.6]
 0x0000000000000001 (NEEDED)             共享库：[libtinfo.so.6]
 0x0000000000000001 (NEEDED)             共享库：[libselinux.so.1]
 0x0000000000000001 (NEEDED)             共享库：[libdl.so.2]
 0x0000000000000001 (NEEDED)             共享库：[liblua5.2.so.0]
 0x0000000000000001 (NEEDED)             共享库：[libperl.so.5.30]
 0x0000000000000001 (NEEDED)             共享库：[libpthread.so.0]
 0x0000000000000001 (NEEDED)             共享库：[libpython3.8.so.1.0]
 0x0000000000000001 (NEEDED)             共享库：[libruby-2.7.so.2.7]
 0x0000000000000001 (NEEDED)             共享库：[libc.so.6]
 0x000000000000000c (INIT)               0x83000	# 用于初始化的函数地址
 0x000000000000000d (FINI)               0x30c358	# 用于结束程序的函数地址
...

 0x0000000000000005 (STRTAB)             0x2eaa8	# 字符串表位置
 0x0000000000000006 (SYMTAB)             0xa580		# 符号表位置
```

# E 3 小结

# F 3.1
## kerneldoc
```c
/**  这里有两个**，以此类方式说明的函数将包含在API参考手册中
 *
 *
 */
```
* 可以使用make htmldocs来生成API参考手册
> 需要安装sudo apt-get install python3-sphinx

> 生成的API参考手册在Documentation/output/admin-guide/index.html
