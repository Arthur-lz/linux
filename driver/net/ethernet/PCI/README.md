# PCI定义
PCI即Peripheral Component Interconnect，中文意思是“外围器件互联”，是由PCISIG (PCI Special Interest Group)推出的一种局部并行总线标准。PCI总线是由ISA(Industy Standard Architecture)总线发展而来的，ISA并行总线有8位和16位两种模式，时钟频率为8MHz，工作频率为33MHz/66MHz。是一种同步的独立于处理器的32位或64位局部总线。从结构上看，PCI是在CPU的供应商和原来的系统总线之间插入的一级总线，具体由一个桥接电路实现对这一层的管理，并实现上下之间的接口以协调数据的传送。从1992年创立规范到如今，PCI总线已成为了计算机的一种标准总线。已成为**局部总线的新标准，广泛用于当前高档微机、工作站，以及便携式微机。主要用于连接显示卡、网卡、声卡。PCI总线是32位同步复用总线。其地址和数据线引脚是AD31～AD0。PCI的工作频率为33MHz**。

### PCI总线结构
* PCI总线取代了早先的ISA总线。

|标题|内容|说明|
|:-|:-|:-|
|树型结构|||
|独立于CPU总线|可以和CPU总线并行操作||
|PCI总线上可以挂接PCI设备和PCI桥片，PCI总线上只允许有一个PCI主设备，其他的均为PCI 从设备，而且读写操作只能在主从设备之间进行，从设备之间的数据交换需要通过主设备中转||
|PCI优点|即插即用(Plug and Play）、中断共享||
|数据宽度|32bit、64bit||
|总线速度|33MHz、66MHz||
|地址总线与数据总线是分时复用|数据传输时，由一个PCI设备做发起者（主控，Initiator或Master），而另一个PCI设备做目标（从设备， Target或Slave）。总线上的所有时序的产生与控制，都由Master来发起。PCI总线在同一时刻只能供一对设备完成传输，这就要求有一个仲裁机构（Arbiter），来决定谁有权力拿到总线的主控权。||

### PCI总线特点
|标题|内容|说明|
|:-|:-|:-|
|数据宽度|32bit、64bit||
|总线速度/时钟频率|33MHz、66MHz||
|最大数据传输率|132MB/s(264MB/s)||
|支持64位寻址||
|供电|5V和3.3V||
|高速性|PCI局部总线以33MHz的时钟频率操作，采用32位数据总线，数据传输速率可高达132MB/s，远超过以往各种总线。而早在1995年6月推出的PCI总线规范2。l已定义了64位、66MHz的PCI总线标准。因此PCI总线完全可为未来的计算机提供更高的数据传送率。另外，PCI总线的主设备（Master）可与微机内存直接交换数据，而不必经过微机CPU中转，也提高了数据传送的效率|
|即插即用性|目前随着计算机技术的发展，微机中留给用户使用的硬件资源越来越少，也越来越含糊不清。在使用ISA板卡时，有两个问题需要解决：一是在同一台微机上使用多个不同厂家、不同型号的板卡时，板卡之间可能会有硬件资源上的冲突；二是板卡所占用的硬件资源可能会与系统硬件资源（如声卡、网卡等）相冲突。而PCI板卡的硬件资源则是由微机根据其各自的要求统一分配，决不会有任何的冲突问题。因此，作为PCI板卡的设计者，不必关心微机的哪些资源可用，哪些资源不可用，也不必关心板卡之间是否会有冲突。因此，即使不考虑PCI总线的高速性，单凭其即插即用性，就比ISA总线优越了许多|
|可靠性|PCI独立于处理器的结构，形成一种独特的中间缓冲器设计方式，将中央处理器子系统与外围设备分开。这样用户可以随意增添外围设备，以扩充电脑系统而不必担心在不同时钟频率下会导致性能的下降。与原先微机常用的ISA总线相比，PCI总线增加了奇偶校验错（PERR）、系统错（SERR）、从设备结束（STOP）等控制信号及超时处理等可靠性措施，使数据传输的可靠性大为增加|
|复杂性|PCI总线强大的功能大大增加了硬件设计和软件开发的实现难度。硬件上要采用大容量、高速度的CPLD或FPGA芯片来实现PCI总线复杂的功能。软件上则要根据所用的操作系统，用软件工具编写支持即插即用功能设备驱动程序|
|自动配置|**PCI总线规范规定PCI插卡可以自动配置。PCI定义了3种地址空间：存储器空间，输入输出空间和配置空间，每个PCI设备中都有256字节的配置空间用来存放自动配置信息，当PCI插卡插入系统，BIOS将根据读到的有关该卡的信息，结合系统的实际情况为插卡分配存储地址、中断和某些定时信息**|
|共享中断|PCI总线是采用低电平有效方式，多个中断可以共享一条中断线，而ISA总线是边沿触发方式|
|扩展性好|如果需要把许多设备连接到PCI总线上，而总线驱动能力不足时，可以采用多级PCI总线，这些总线上均可以并发工作，每个总线上均可挂接若干设备。因此PCI总线结构的扩展性是非常好的。由于PCI的设计是要辅助现有的扩展总线标准，因此与ISA，EISA及MCA总线完全兼容|
|多路复用|在PCI总线中为了优化设计采用了地址线和数据线共用一组物理线路，即多路复用。PCI接插件尺寸小，又采用了多路复用技术，减少了元件和管脚个数，提高了效率|
|严格规范|PCI总线对协议、时序、电气性能、机械性能等指标都有严格的规定，保证了PCI的可靠性和兼容性。由于PCI总线规范十分复杂，其接口的实现就有较高的技术难度|

### PCI配置空间-----实现自动配置的关键
PCI设备(PCI device)都有一个配置空间，大小为256字节，实际上是一组连续的寄存器，位于设备上。其中头部64字节是PCI标准规定的，格式如下

![alt text](https://raw.githubusercontent.com/Arthur-lz/linux/master/pic/pci1.png "")

剩余的部分是PCI设备自定义的。
PCI配置空间头部有6个BAR(Base Address Registers)，BAR记录了设备所需要的地址空间的类型(memory space或者I/O space)，基址以及其他属性。BAR的格式如下：

![alt text](https://raw.githubusercontent.com/Arthur-lz/linux/master/pic/pci2.png "")

可以看出，设备可以申请两类地址空间，memory space和I/O space，它们用BAR的最后一位区别开来。
说到地址空间，计算机系统中，除了我们常说的memory address(包括逻辑地址、虚拟地址(线性地址)、CPU地址(物理地址))，还有I/O address，这是为了访问I/O设备(主要是设备中的寄存器)而设立的，大部分体系结构中，memory address和I/O address都是分别编址的，且使用不同的寻址指令，构成了两套地址空间，也有少数体系结构将memory address和I/O address统一编址(如ARM)。
有两套地址空间并不意味着计算机系统中需要两套地址总线，实际上，memory address和I/O address是共用一套地址总线，通过控制总线上的信号区别当前地址总线上的地址是memory address还是I/O address。**北桥芯片(Northbridge，Intel称其Memory Controller Hub，MCH)负责地址的路由工作**，它内部有一张address map，记录了memory address，I/O address的映射信息，一个典型的address map如图：

![alt text](https://raw.githubusercontent.com/Arthur-lz/linux/master/pic/pci3.png "")

我们来看北桥是如何进行地址路由的。根据控制总线上的信号，北桥首先可以识别地址属于memory space还是I/O space，然后分别做处理。
比如若是memory space，则根据address map找出目标设备(DRAM或Memory Mapped I/O)，若是DRAM或VGA，则转换地址然后发送给内存控制器或VGA控制器，若是其它I/O设备，则发送给南桥。
若是I/O space，则发送给南桥(Southbridge，Intel称其I/O Controller Hub，ICH)，南桥负责解析出目标设备的bus, device, function号，并发送信息给它。
PCI设备会向计算机系统申请很多资源，比如memory space, I/O space, 中断请求号等，相当于在计算机系统中占位，使得计算机系统认识自己。
PCI设备可以通过两种方式将自己的I/O存储器(Registers/RAM/ROM)暴露给CPU：
在memory space申请地址空间，或者在I/O space申请地址空间。
这样，PCI设备的I/O存储器就分别被映射到CPU-relative memory space和CPU-relative I/O space，使得驱动以及操作系统得以正常访问PCI设备。对于没有独立I/O space的体系结构(如ARM)，memory space和I/O space是统一编址的，也就是说memory space与I/O space等价了，这时，即使PCI设备在BAR表明了要申请I/O space，实际上也是分配在memory space的，所以驱动无法使用I/O端口指令访问I/O，只能使用内存指令。在Windows驱动开发中，PCM_PARTIAL_RESOURCE_DESCRIPTOR记录了为PCI设备分配的硬件资源，可能有CmResourceTypePort, CmResourceTypeMemory等，后者表示一段memory地址空间，顾名思义，是通过memory space访问的，前者表示一段I/O地址空间，但其flag有CM_RESOURCE_PORT_MEMORY和CM_RESOURCE_PORT_IO两种，分别表示通过memory space访问以及通过I/O space访问，这就是PCI请求与实际分配的差异，在**x86下，CmResourceTypePort的flag都是CM_RESOURCE_PORT_IO，即表明PCI设备请求的是I/O地址空间，分配的也是I/O地址空间**，而在ARM或Alpha等下，flag是CM_RESOURCE_PORT_MEMORY，表明即使PCI请求的I/O地址空间，但分配在了memory space，我们需要通过memory space访问I/O设备(通过MmMapIoSpace映射物理地址空间到虚拟地址空间，当然，是内核的虚拟地址空间，这样驱动就可以正常访问设备了)。

为了为PCI设备分配CPU-relative space，计算机系统需要知道其所申请的地址空间的类型、基址等，这些信息记录在设备的BAR中，每个PCI配置空间拥有6个BAR，因此每个PCI设备最多能映射6段地址空间(实际很多设备用不了这么多)。**PCI配置空间的初始值是由厂商预设在设备中的，于是设备需要哪些地址空间都是其自己定的，可能造成不同的PCI设备所映射的地址空间冲突，因此在PCI设备枚举(也叫总线枚举，由BIOS或者OS在启动时完成)的过程中，会重新为其分配地址空间，然后写入PCI配置空间中**。

***通过memory space访问设备I/O的方式称为memory mapped I/O，即MMIO，这种情况下，CPU直接使用普通内存指令即可访问设备I/O。
通过I/O space访问设备I/O的方式称为port I/O，或者port mapped I/O，这种情况下CPU需要使用专门的I/O指令如IN/OUT访问I/O端口***。

**常见的MMIO例子有，VGA card将framebuffer映射到memory space**，NIC将自己的片上缓冲映射到memory space，实际上，***最典型的MMIO应该是DRAM，它将自己的存储空间映射到memory space，是占用CPU地址空间最多的“设备***”。

|重点|内容|应用|
|:-|:-|:-|
|MMIO|通过memory space访问设备I/O的方式称为memory mapped I/O|VAG显卡的fb、DRAM|
|arm使用MMIO|没有独立I/O space，只能使用内存指令访问I/O空间|Alpha与arm一样|
|北桥负责进行I/O空间和内存空间地址总线的路由|I/O空间和内存空间共用一套地址总线，经由北桥来路由|
|PCI设备枚举(总线枚举)|为防止厂家设置的PCI配置空间内在与其他设备冲突，由BIOS或者OS在启动时会重新为其分配地址空间，然后写入PCI配置空间中|
