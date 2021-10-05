# usb 子系统初始化
* usbcore不是设备，是所有usb设备依赖的模块
```c
// drivers/usb/core/usb.c

subsys_initcall(usb_init); 		/* 表示usb_init是usb子系统的初始化函数*/

usb_init()

```	

> 类似的还有pci子系统, scsi子系统

> 基本在drivers/下第一层目录里都有一个类似的子系统

* device_bind_driver
> device&driver通过它结为秦晋之好

> bus, device, driver铁三角

```
/* 每当一个device诞生，它就会到bus的drivers链表去找自己的另一半
 * 每当一个device_driver诞生，它就会到bus的devices链表上去寻找它的那些设备;如果找到了，就调用device_bind_driver绑定上，如果没有找到就等着  
 */
```

* /proc/devices中显示了linux系统中当前已经分配出去的主设备号
> usb的主设备号是180

> 并不是所有的usb设备都用这个usb主设备号; 大多数usb设备都会与input子系统、video子系统关联，并不单单只是作为usb设备存在

> 如果usb设备没有关联其他子系统，那么就需要在对应驱动的probe函数中使用usb_register_dev来注册并获得主设备号USB_MAJOR

> 如果usb设备关联了其他子系统，则需要在对应驱动的probe函数中使用子系统相应的注册函数，此时USB_MAJOR就用不上了

> 比如，usb键盘关联了input子系统，驱动对应driver/hid/usbhid/usbkbd.c文件，在它的probe函数中就用input子系统的注册函数input_register_device来注册一个输入设备，那么此时的usb键盘决不再会使用USB_MAJOR这个主设备号，而是使用input子系统的主设备号13

> 准确的说，上面所说的usb设备应称为usb接口，因为只有一个usb接口才对应一个usb驱动.

```c
struct usb_interface {

	...
	enum usb_interface_condition condition;         /* 描绘usb接口的心情
							 * UNBOUND, 孤苦
							 * BINDING, 期望
							 * BOUND, 幸福
							 * UNBINDING 分开
							 * 人生又何尝不是如此？

							 */
	
};
```

* usb协议中规定，所有usb设备都必须支持挂起状态
> 支持挂起是为了节电，当设备在指定的时间内（大约3ms），如果没有发生总线传输，就要进入挂起状态;当它收到一个non-idle信号时，就会被唤醒

* pm_usage_cnt
> pm是电源管理; usage_cnt是使用计数

> 当它为0时，接口允许autosuspend

```
/* 什么是autosuspend?
 * 笔记本电脑，当合上笔记本电脑后，它会自动进入休眠，这就是autosuspend
 * 但不是每次都这样，只有当pm_usage_cnt为0时才会允许接口autosuspend
 */
```

* usb_interface_descriptor
> 什么是描述符？我们的生活就是一个不断遇到人，认识人的过程，有些人注定只是擦肩而过，有些人却深深的留在我们的心里，比如usb的描述符

> EEPROM, usb的四个描述符就放在这里。因为EEPROM可以按字节擦除，而flash只能按block, 所以修改少量数据EEPROM较合适，尽管它比flash贵

> __attribute__ ((packed))用来告诉编译器这个数据结构的元素都是1字节对齐的，不要再添加填充位了；如果不加这个属性给数据结构，那么编译器就会依据你的平台类型在数据结构的每个元素之间添加一定的填充位

```c
struct usb_interface_descriptor {
	...
	bNumEndpoints;		/*它不包含0号端点，即如果此值为1，那么算上0号端点，此接口共有两个端点*/
	iInterface;		/*这个是第五个描述符，除了usb设备的那四个描述符外还有这个，这个叫字符串描述符，它里面可以存一些长字符串，如产品序列号; 字符串描述符与其他描述符一样，也可以有多个，这里的iInterface就是这些字符串描述符的索引值，用以区分各个不同的字符串描述符; 字符串描述符可有可无，usb规范未说它必须有*/
} __attribute__ ((packed));


struct usb_host_interface {
	string;		/*用来保存从设备里取出的字符串描述符信息*/
	extra;		/* 额外的描述符；除了usb四大描述符和字符串描述符外，还有为一类设备定义的描述符，还有厂商为设备特别定义的描述符, extra指的就是它们
			 */
	extralen;	/* 额外描述符数量*/
	struct usb_host_endpoint *endpoint;
};

struct usb_host_endpoint {	/* 端点，usb数据传输的终点 */
	struct usb_endpoint_descriptor          desc;	/* 0号端点没有自己的端点描述符
							 */
	struct ep_device                *ep_dev;        /* For sysfs info 
							 * ls /sys/bus/usb/devices/usb1/ep_00/
							 * 这里查到的文件都是用ep_dev创建的
							 */


};

struct usb_endpoint_descriptor {
	__u8  bLength;
        __u8  bDescriptorType;

        __u8  bEndpointAddress;		/* bit0~3表示端点号;
					 * bit8表示方向
					 */
        __u8  bmAttributes;		/* bit1,bit0定义传输类型
					 */
        __le16 wMaxPacketSize;		/*端点一次可以处理的最大字节数*/
        __u8  bInterval;		/* usb是轮询式总线，这个值表达了端点的一个美好的期望，希望主机轮询自己的时间间隔，但实际上主机可能不批准;不同的传输类型，它也有不同的含义
					 */

        /* NOTE:  these two are _only_ in audio endpoints. */
        /* use USB_DT_ENDPOINT*_SIZE in bLength, not sizeof. */
        __u8  bRefresh;
        __u8  bSynchAddress;
	
} __attribute__ ((packed));
```

* usb_bus
> 什么是usb_bus，每一个主机控制器对应一个usb总线，每一个usb总线对应一个usb_bus结构变量，这个变量在主机控制器驱动程序中申请

> 在usb世界里，一条usb总线就是一棵树，一个usb设备就是一个叶子。为了记录这棵树上的每个叶子，每个总线有一个地址映射表devmap

```c
struct usb_bus {
	struct usb_devmap devmap; 	/*usb总线上的地址映射表，它表示了一条总线上usb设备的连接情况, 一条总线上可以接128个usb设备*/
};
```

* usb_device
```c
struct usb_device {
	int             devnum;
	char            devpath[16];	/* ls /sys/bus/usb/devices/
					 * 这下面有usb1, usb2它们表示计算机上有两条usb总线，即2个主机控制器
					 * 1-0:1.0是什么？1就是1号总线, 0指的就是devpath, 1.0中的1表示配置，0表示接口号
					 * 总的来说就是，1号总线0号端口的设备，使用的是1号配置，接口号为0
					 *
					 * 但devpatch不是端口号;上面的结论是在Root Hub没有级联情况下的情况。如果在root hub又接了别的hub, 然后一级一级连下去，devpath的作用就是用来表示这个多级的hub树
					 * 顶级设备的devpath就是其连接在root hub上的端口号，而次级设备就是其父hub的devpath后面加上其端口号，
					 *  如：4-0:1.0是一个hub的话，那么它下面的1号端口的设备就是4-0.1:1.0, 2号端口的设备就是4-0.2:1.0, 3号就是4-0.3:1.0
					 */

	enum usb_device_state   state; 	/* 
					 *
					 */

	struct usb_tt   *tt;		/* tt, 即transaction translator，可以把它想像成一块特殊的电路，是hub里的电路，确切的说是高速hub里的电路;usb有low, full, high三种速度，
					 * 以前的主机控制器只有两种接口，ohci和uhci, 这是在usb 1.0时代；在usb 1.1引入了ehci, 即high speed设备
					 * hub也有高速hub和低速hub，那么就产生了一个兼容问题，高速hub是否能够支持low, full速设备?一般来说是不支持的，于是有了一个叫tt的电路，
					 * 它就负责高速和low,full设备进行数据转换。于是，如果一个高速设备中有tt, 那么就可以连接low, full设备，
					 * 要不然low, full设备就没办法用, 只能通过ohci,uhci的hub口连接使用
					 *
					 */
        int             ttport;
	unsigned int toggle[2];		/* 数组中只有两个元素，分别对应IN端点和OUT端点。这个数组的存在是什么价值？ 
					 * 前面说过，要想和usb设备通信，创建一个urb, 为它赋值，交给usb core就可以了。实际上在usb cable里流淌的根本不是那么回事儿，我们提交的是urb, 
					 * usb cable里流淌的是一个个数据包packet。
					 * 所有的packet都从一个SYNC同步字段开始，SYNC是一个8位长的二进制串，只是用来同步的，它的最后两位标志了SYNC的结束和PID(Packet Id)的开始
					 * PID是一个8位的二进制串，前四位用来区分不同的packet类型，后面四位只是前面四位的反码，是校验用的；
					 * 共有四大类packet，分别是：Token, Data, Handshake, Special
					 * 主机和设备通过PID来判断送过来的packet是不是自己所需要的
					 * PID之后紧跟的是地址字段，每个packet都需要知道自己往哪里去。这个地址包括两部分：7位表示总线上连接的设备或接口地址，4位表示端点地址；
					 * 地址段再往后是11位的帧号（frame number), 值达到7FFH时归0, 这个帧号并不是每个packet都有，它只在每帧或微帧开始的SOF Token包中发送。
					 * 帧是相对于low, full速模式来说的，一帧就是1ms, 对于高速模式用的是微帧，一个微帧为125ms, 每帧或微帧会传输多个packet
					 * 帧号再往后是Data字段，它可以有0到1024字节不等。最后还有CRC校验字段收尾 
					 * Data类型的packet有四种类型，DATA0, DATA1, DATA2, MDATA.
					 * 其中DATA0, DATA1可以用来实现data toggle同步
					 * 对于指传输、控制传输、中断传输来说，数据包最开始都是被初始化为DATA0的，然后为了传输正确性，就传输一次DATA0，再传输一次DATA1。一旦哪次
					 * 打破了这种规律，主机就可以认为传输出错了。
					 * 对于等时传输来说，data toggle并不被支持。
					 * usb就是使用这种简单哲学来判断对与错。这也不简单啊... 
					 * toggle数组就是为了支持这种哲学应运而生的，它里面的每一位元素表示的就是每个端点当前发送或接收的数据包是DATA0还是DATA1
					 *
					 */
	struct usb_device *parent;	/* 如果是root hub则此值为null;
					 * 它就是给那些从root hub接出来的设备用的
					 */
	struct usb_bus *bus;		/* 设备所在的那条总线 */
        struct usb_host_endpoint ep0;	/*端点0，它很特殊，它独有的特殊待遇是在struct usb_device对象产生时它就要初始化*/

	struct device dev;

        struct usb_device_descriptor descriptor;	/*四大描述符中的第三个 */
        struct usb_host_bos *bos;
        struct usb_host_config *config;

	struct usb_host_config *actconfig;
        struct usb_host_endpoint *ep_in[16];	/* 除了端点0，一个设备在高速模式下最多只能再有15个IN端点和15个OUT端点，端点0对应的管道是message管道，能进能出，
						 * 所以这里的ep_in, ep_out数组都有16个值
						 */
        struct usb_host_endpoint *ep_out[16];

	char **rawdescriptors;			/* 这是一个字符指针数组，数组里每一项都指向一个使用GET_DESCRIPTOR请求去获得配置描述符时所得到的结果。
						 * 当使用GET_DESCRIPTOR去请求配置描述符时，设备返回的不仅仅只有配置描述符，它把该配置所包括的所有接口的接口描述符，
						 * 还有接口里端点的端点描述符全返回了
						 */

        unsigned short bus_mA;			/*此值在主机控制器驱动中设置；通常来说，计算机的usb端点可以提供500mA的电流*/
        u8 portnum;
        u8 level;				/*usb设备树的级联关系。root hub的level是0，下面一层就是1, 再下面一层就是2*/

        unsigned can_submit:1;
        unsigned persist_enabled:1;
        unsigned have_langid:1;			/* 用来判断string_langid是否有效*/
        unsigned authorized:1;
        unsigned authenticated:1;
        unsigned wusb:1;
        unsigned lpm_capable:1;
        unsigned usb2_hw_lpm_capable:1;
        unsigned usb2_hw_lpm_besl_capable:1;
        unsigned usb2_hw_lpm_enabled:1;
        unsigned usb2_hw_lpm_allowed:1;
        unsigned usb3_lpm_u1_enabled:1;
        unsigned usb3_lpm_u2_enabled:1;
        int string_langid;			/*usb设备中的字符串描述符使用的是UNICODE编码，可以支持多种语言，此字段用于指定使用哪种语言*/

        /* static strings from the device */
        char *product;
        char *manufacturer;
        char *serial;

	struct list_head filelist;

        int maxchild;				/*hub的端口数，可不包括上行端口*/

        u32 quirks;				/*毛病*/
        atomic_t urbnum;

        unsigned long active_duration;

#ifdef CONFIG_PM			/*电源管理相关*/
        unsigned long connect_time;

        unsigned do_remote_wakeup:1;
        unsigned reset_resume:1;
        unsigned port_is_suspended:1;
#endif
        struct wusb_dev *wusb_dev;
        int slot_id;
        enum usb_device_removable removable;
        struct usb2_lpm_parameters l1_params;
        struct usb3_lpm_parameters u1_params;
        struct usb3_lpm_parameters u2_params;
        unsigned lpm_disable_count;
};

enum usb_device_state {
        /* NOTATTACHED isn't in the USB spec, and this state acts
         * the same as ATTACHED ... but it's clearer this way.
         */
        USB_STATE_NOTATTACHED = 0,

        /* chapter 9 and authentication (wireless) device states */
        USB_STATE_ATTACHED,			/* 表示设备已经连接usb接口上了，是hub检测到设备时的初始状态*/
        USB_STATE_POWERED,                      /* wired 加电状态；usb设备的电源可以来自外部电源，协议中叫self-pwoered;也可以来自hub内部，称为bus-powered
						 * 尽管self-powered类型的设备在连接到usb接口前就已经上电了，但它们直到连接到usb接口后才能被看作是powered的
						 */
        USB_STATE_RECONNECTING,                 /* auth */
        USB_STATE_UNAUTHENTICATED,              /* auth */
        USB_STATE_DEFAULT,                      /* limited function 
						 * 在powered后，设备必须在收到一个复位(reset)信号并成功复位后，才能使用默认地址回应主机发过来的设备和配置描述符请求
						 */
        USB_STATE_ADDRESS,			/* 表示主机分配了一个唯一的地址给设备，此时设备可以使用默认管道响应主机的请求
						 */
        USB_STATE_CONFIGURED,                   /* most functions 
						 * 表示设备已经被主机配置过了，此时主机可以使用设备提供的所有功能
						 */

        USB_STATE_SUSPENDED			/* 挂起状态，为了省电，设备在指定的时间内如果没有发生总线传输，就要进入挂起状态，此时usb设备要自己维护地址、配置信息
						 */
	/* usb设备从生到死都要按照这么几个状态渡过，遵循这个过程
	 */
};
struct usb_device_descriptor {
	__u8  bLength;
        __u8  bDescriptorType;

        __le16 bcdUSB;
        __u8  bDeviceClass;
        __u8  bDeviceSubClass;
        __u8  bDeviceProtocol;
        __u8  bMaxPacketSize0;		/* 端点0一次可以处理的最大字节数，端点0的属性放到这里来了！！够特殊！
					 * 其他端点的该值是存放在端点描述符里的，而因为端点0没有自己的端点描述符，所以放到设备描述符里来了
					 * 在spec里规定了，端点0单次最大可以处理的字节数只能是8, 16, 32, 64中其一
					 * 如果一个设备工作在high模式，则只能是64
					 * 如果一个设备工作在low模式，则只能是8
					 */
        __le16 idVendor;
        __le16 idProduct;
        __le16 bcdDevice;		/*设备版本号*/
        __u8  iManufacturer;
        __u8  iProduct;
        __u8  iSerialNumber;
        __u8  bNumConfigurations;
	
} __attribute__ ((packed));

```

* struct usb_host_config
```c
struct usb_host_config {
	struct usb_config_descriptor desc;
	char *string;           /* 此字段保存了配置描述符中iConfiguration字段对应的字符串 */

        /* List of any Interface Association Descriptors in this
         * configuration. */
        struct usb_interface_assoc_descriptor *intf_assoc[USB_MAXIADS];

        /* the interfaces associated with this configuration,
         * stored in no particular order */
        struct usb_interface *interface[USB_MAXINTERFACES];			/* 配置所包含的接口。这个数组的顺序未必是按照配置里接口号的顺序，
										 * 所以，你要想得到某个接口号对应的usb_interface结构对象，
										 * 就必须要用drivers/usb/usb.c中定义的usb_ifnum_to_if函数
										 *
										 * 协议中规定，请求配置描述符时，配置里的所有接口描述符是按照顺序一个一个返回的。
										 * 那为什么这里又要使用上面的函数来取呢？
										 * 因为协议归协议，厂商归厂商，有些厂商就是要先返回接口1再返回接口0，所以就不得不用上面的函数
										 */

        /* Interface information available even when this is not the
         * active configuration */
        struct usb_interface_cache *intf_cache[USB_MAXINTERFACES];		/* usb接口的缓存
										 */

        unsigned char *extra;   /* Extra descriptors */
        int extralen;

};

struct usb_config_descriptor {
        __u8  bLength;
        __u8  bDescriptorType;

        __le16 wTotalLength;		/* 是使用GET_DESCRIPTOR请求从设备中获取配置描述符信息时，返回的数据长度，
					 * 也就是说，对包括配置描述符、接口描述符、端点描述符, class-或vendor-specific描述符在内的所有描述符进行统计
					 */
        __u8  bNumInterfaces;
        __u8  bConfigurationValue;	/*对于拥有多个配置的设备来说，可以用这个值作为参数，使用SET_CONFIGURATION请求来改变正在被使用的usb配置，bConfigurationValue就指明了将要激活哪个配置
					 * 设备虽然可以有多个配置，但同一时间却只有一个配置被激活。SET_CONFIGURATION请求也是标准的设备请求之一，专门用来设置设备的配置
					 */
        __u8  iConfiguration;
        __u8  bmAttributes;		/* bit6为1表示self-powered
					 * bit7必须为1
					 * bit5为1表示这个配置支持远程唤醒
					 */
        __u8  bMaxPower;		/* 设备正常运行时，从总线那里分到的最大电流值，以2mA为单位。设备可以使用这个字段向hub表明自己需要的电流，
					 * 但如果设备请求的电流过于多，超出了hub所能给的电流，hub会直接拒绝; usb_device中的bus_mA就表示hub所能给的最大电流
					 */
} __attribute__ ((packed));


struct usb_interface_cache {
        unsigned num_altsetting;        /* number of alternate settings */
        struct kref ref;                /* reference counter */

        /* variable-length array of alternate settings for this interface,
         * stored in no particular order */
        struct usb_host_interface altsetting[0];	/* 这是一个可变长数组，可以按需分配，你对设备说GET_DESCRIPTOR时，
							 * 内核就根据返回的每个接口可选配置的数目分配给intf_cache数组相应的空间
							 * 为什么要缓存这些东西？
							 * 为了在配置被取代之后仍然能够获取它的信息，就把日后可能会需要的信息放到了intf_cache数组的struct usb_interface_cache对象里
							 * 谁会需要？
							 * usbfs里面显示有系统中所有usb设备的可选配置和端点信息，它就是利用intf_cache这个数组里缓存的东西实现的
							 */
};

```

* usb_device_driver, usb设备驱动
* usb_driver, usb接口驱动
> 一般来说，编写usb驱动指的就是编写usb接口驱动

* usb_device
```
# 设备的状态
attatched->powered->default->address
设备进入default状态，hub同时也会获得设备真正的速度，根据这个速度, hub能够知道端点0一次可处理的最大数据包大小, 这是根据usb协议规定的，什么设备对应什么样的最大数据包尺寸

```

* usb_ctrlrequest 
```c
struct usb_ctrlrequest {
	__u8 bRequestType;	/* 只有一个字节，但内容很丰富，大道理往往都包含在这种小地方
				 * bit7，表示控制传输中DATA transaction阶段中的方向，当然如有DATA阶段的话
				 * bit5, bit6表示request type，是标准的class-specific，还是vendor-specific
				 * bit0~bit4, 表示这个请求针对的是设备、接口还是端点
				 * 
				 */
        __u8 bRequest;		/* 表示具体是哪个request
				 */
        __le16 wValue;		/* 它是request的参数，request不同它也不同
				 */
        __le16 wIndex;		/* 也是request参数，requesttype用来指明request针对的是设备上的某个接口或端点，而index用来指明是哪一个接口或端点
				 */
        __le16 wLength;		/* 控制传输中DATA transaction阶段的长度，如果这个值为0，就表示没有DATA transaction阶段, 此时requesttype上的方向也就没什么用了
				 */
} __attribute__ ((packed));

/* usb_ctrlrequest，它描述了主机通过控制传输发送给设备的请求 
 * 	主机向设备请求写信息必须得按照协议中规定好的格式，不然设备就不会明白主机是什么意思  
 * 这个结构中描述的request都是在SETUP包中发送的，setup 包是Token PID类型中的一种，下面详细介绍一下控制传输底层的packet
 * 控制传输最少要有两人阶段的transation：SETUP和STATUS，中间还有一个可有可无的DATA阶段
 * transaction可以称为事务，或会话, 含义是主机和设备之间的一次完整交流
 * USB的transaction可以包括一个Token包、一个Data包和一个Handshake包
 * Token, Data, Handshake都属于四种PID类型
 * Token包只包括：SYNC、PID、地址域、CRC，它并没有DATA字段, 它的名字很形像就是用来标记所在transaction里接下来动作的
 * 对于Out和Setup Token包，里面的地址域指明了接下来要接收的Data包的端点
 * 对于In Token包，地址域指明了接下来哪个端点要发送Data包
 * 只有主机才有权利发送Token包
 * 
 * 与Token包相比，Data包中没有了地址域，多个Data字段，这个Data字段对于低速设备最大为8字节，对于全速设备最多为1023字节，对于高速设备最大为1024字节, 
 * 它就是躲在Token包后面用来传输数据的
 * 
 * Handshake包的成分简单多了，除了一个SYNC外，只有一个PID，通过PID取不同的值来报告一个transaction的状态，比如数据已经成功接收等
 * 
 * 控制传输的SETUP transaction一般来说有三个阶段：主机向设备发送Setup Token包，然后发送Data0包，如果一切顺利设备回应ACK Handshake包表示没有问题，为什么加上一般？
 * 如果中间的那个Data0包由于某种不可知因素被损坏了，设备就什么都不回应，这时就成了两个阶段了。
 * 
 * SETUP transaction之后，接下来如果控制传输有DATA Transaction要传输的话，那就Data0、Data1交叉地发送数据包，这是为了实现data toggle
 *
 * 最后是STATUS transaction，向主机汇报前面SETUP和DATA阶段的结果，如果主机下达的命令已经完成了，或者主机下达的命令没有完成，或者设备正忙着没工夫理会主机的那些命令
 * 
 * 这样经过SETUP、DATA、STATUS这三个传输阶段，一个完整的控制传输就完成了 
 *
 * 一个请求都在SETUP包中发是有问题的，因为SETUP包本身并没有DATA字段，严格来说它们都是在SETUP transaction阶段里Setup包后的Data0中发送的
 * 
 * 
 */
```
> USB_REQ_SET_ADDRESS, Standard requests, for the bRequest field of a SETUP packet. 


* urb
```c
/* include/linux/usb.h 
 */

struct urb {
        /* private: usb core and host controller only fields in the urb */
        struct kref kref;               /* urb的引用计数，它决定了urb的生死存, 
					 * 为什么urb的生死要掌握在它的手里？
					 * 之前有说，主机和设备之间通过管道传递数据，管道的一端点是主机上的缓冲区，另一端是设备上的端点
					 * 管理之中流动的数据，在主机控制器和设备看来是一个个packets，在咱们程序员看来就是urb
					 * 因而端点之中就有一个叫urb的队列，不过这并不代表一个urb只能发给一个端点，它可能通过不同的管理发送给不同的端点，
					 * 那么这样一来，我们如何知道一个urb被多少个不同的端点使用？如何判断这个urb生命已经结束？引用计数
					 * 每多一个使用者，引用计数就加1，相反减少一个就减1
					 * 如果最后一个使用者也释放了这个urb，宣称不再使用它了，那这个urb的生命周期就走到了心头，会自动销毁
					 */
        void *hcpriv;                   /* private data for host controller */
        atomic_t use_count;             /* concurrent submissions counter 
					 * 先了解urb来完成一交完整USB通信都要经历哪些阶段
					 * 首先驱动程序发现自己有与usb设备通信的必要，于是创建一个urb，
					 * 并指定它的目的地是设备上的哪个端点，然后提交给usb core, 
					 * usb core 将它美化一下移交给主机控制器驱动程序HCD,
					 * HCD会出解析这个urb, 了解它的目的是什么，并与usb设备进行交流，在交流结束，urb的目的达到之后，
					 * HCD再把这个urb的所有权移交回驱动程序
					 * 
					 * 这里的use_count就是在usb core将urb移交给HCD，办理交接手续时，插上一脚，每当走到这一步，它的值就加1，
					 * 什么时候减1？
					 * 在HCD重新将urb所有权移交回驱动程序时
					 *
					 * 只要HCD拥有这个urb的所有权，那么此urb的use_count就不会为0
					 * 当它的值大于0，就说明当前有HCD正在处理它
					 * 它的值为0，只能说明当前没有HCD处理它而已，并不代表需要把urb销毁
					 * 
					 * 它究竟是来干嘛的？
					 * 还是要从那几个阶段说。urb驱动也创建了，该提交的也提交了，HCD正在处理着，可驱动反悔了，它不想再继续这次通信了，想将这个urb终止掉。
					 * 善解人意的usb core当然会给驱动提供这样的接口来满足这样的需求
					 * 这个需要被细分为两种方式，一种异步方式，一种同步方式
					 * 异步方式：
					 * usb_unlink_urb
					 *
					 * 同步方式：
					 * usb_kill_urb
					 * HCD将这次通信终止后，同样会将urb的控制权移交回驱动程序，那么驱动通过什么来判断这次通信已经终止了？
					 * 就是通过这里的use_count，驱动会在usb_kill_urb里一直等着这个值变为0
					 */
        atomic_t reject;                /* submissions will fail 
					 * 拒绝谁？  
					 */
        int unlinked;                   /* unlink error code */

        /* public: documented fields in the urb that can be used by drivers */
        struct list_head urb_list;      /* list head for use by the urb's
                                         * current owner 
					 * 每个端点都会有的urb队列，这个队列就是由这里的urb_list一个一个连接起来的
					 * HCD每收到一个urb, 就会将添加到这个urb指定的那个端点的urb队列里去
					 * 这个链表的头在哪里？
					 * 当然是在端点里，就是端点里的那个struct list_head结构体成员
					 */
        struct list_head anchor_list;   /* the URB may be anchored */
        struct usb_anchor *anchor;
        struct usb_device *dev;         /* (in) pointer to associated device */
        struct usb_host_endpoint *ep;   /* (internal) pointer to endpoint */
        unsigned int pipe;              /* (in) pipe information 
					 * urb到达端点之前，需要经过一个通往端点的管道，就是这个pipe
					 * 怎么表示一个管道？
					 * 管道有两端，一端是主机上的缓冲区，一端是设备上的端点。因为有两端，所以需要方向
					 * 因为端点有四种类型，所以与端点相生相依的管道也不会只有一种类型
					 * 那么，要确定一个管道至少要知道：管道两头的地址、方向和类型。
					 * 因为主机这端的是确定的，需要确定的只是另一端设备的地址和端点的地址，怎么将这些内容组合起来？
					 * 一个整型值加一些宏就可以了
					 * bit7用来表示方向
					 * bit8~14表示设备地址
					 * bit15~18表示端点号
					 * bit30~bit31表示管道类型
					 * 再看一下关于管道的宏
					 * include/linux/usb.h中定义
					 *
					 * 如何创建一个管道？
					 * 主机和设备的交流必须通过管道，必须创建一个管理给urb，它才知道路怎么走
					 * 端点有四种，对应的管理也有四种 
					 * usb_rcvctrlpipe
					 * usb_rcvaddr0pipe
					 * 
					 */
        unsigned int stream_id;         /* (in) stream ID */
        int status;                     /* (return) non-ISO status 
					 * URB_SHORT_NOT_OK，这个状态只对IN端点读取数据的urb有效，它的意思是：如果从一个IN端点那里读取了一个比较短的数据包，就可以认为是错误的, 
					 * 问题来了，这个短要到什么程度？在端点描述符里有个wMaxPacketSize，指明了端点一次能处理的最大字节数. 因为在PID的四种类型里（即包的四种类型里），
					 * 只有DATA包中有数据字段，其他类型Token、Handshake类型的PID是没有数据字段的。但DATA类型包中不只有数据字段，里面还有SYNC、PID、地址域、CRC等字段。
					 * 那么现在问题又来了，每个端点描述符里的wMaxPacketSize所表示的最大字节数都包括哪些部分？指的是整个包的大小吗？不是，它指的是DATA包中数据字段的长度，
					 * 俗称data payload
					 * 其他的数据字段是协议本身需要的，不算在wMaxPacketSize内.
					 * short与wMaxPacketSize有什么关系？
					 * 如果从IN端点那里收到了一个比wMaxPacketSize还短的包，同时也设备了URB_SHORT_NOT_OK这个标志，那么就可以认为传输出错了.
					 *
					 * 本来收到一个较短的包是意味着这次传输到此为止就结束了，可以认真想想，data payload的长度最大必须是wMaxPacketSize，这个规定是不可违背的，
					 * 但是如果端点想给你的数据不止这么多，怎么办？这就需要分成多个wMaxPacketSize大小的data payload来传输，
					 * 有时数据不会那么凑巧刚好能分成wMaxPacketSize的整数倍，这时，最后一个data payload的长度就会比wMaxPacketSize要小，
					 * 这种情况本来就意味着端点到这里为止已经传输完了它想传的数据，这次传输就该结束了，不过如果你设置了URB_SHORT_NOT_OK标志，
					 * HCD就会认为发生了错误。
					 * 
					 * URB_ISO_ASAP, 此标志只是为了方便等时传输使用。
					 * 等时传输和中断传输都是周期性传输。这个周期是端点对主机表达的一种期望，它希望主机间隔固定的时间来访问自己一次。
					 * URB_ISO_ASAP的意思是，告诉HCD什么时候不忙什么时候开始，不用指定开始的帧号了。
					 *
					 * URB_NO_TRANSFER_DMA_MAP, URB_NO_SETUP_DMA_MAP 
					 * DMA是什么？DMA就是外设。比如usb摄像头，和内存之间进行数据交换，不用通过cpu
					 * 一般来说，都是驱动里提供了kmalloc们分配的缓冲区，HCD进行一定的DMA映射处理。
					 * 为了分担HCD的压力，于是就有了上面这两个标志，告诉HCD不要自己做DMA映射了，驱动提供的urb里已经提供有DMA缓冲区地址。
					 * 驱动提供的缓冲区地址就是下面的transfer_buffer, transfer_dma
					 *
					 * URB_NO_FSBR，这是给UHCI用的
					 *
					 * URB_ZERO_PACKET，这个标志表示指的out传输必须使用一个short packet结束。
					 * 即，如果传输的数据总量大于wMaxPacketSize，则会将数据分成多个DATA包来传输，那么最后一个data payload的长度可能等于wMaxPacketSize或小于它，
					 * 当等于时，并且设置了标志URB_ZERO_PACKET，则需要再发送一个长度为0的数据包来结束这次传输；
					 * 当小于时，就直接结束
					 * 如果数据传输总量小于wMaxPacketSize，与小于时一样，直接结束
					 *
					 * URB_NO_INTERRUPT，这个标志用来告诉HCD，在URB完成后，不要请求一个硬件中断，这就意味着，你的完成函数可能不会在urb完成时被调用，
					 * 而是在之后的某个时间被调用，urb core会保证每个urb调用一次完成处理函数。
					 *
					 */
        unsigned int transfer_flags;    /* (in) URB_SHORT_NOT_OK | ...*/
        void *transfer_buffer;          /* (in) associated data buffer 
					 * 这是一个, 这三个成员是用来描述主机上的缓冲区
					 *
					 * transfer_buffer是一定会被创建的，因为不是所有的主机控制器都能够使用DMA，万一遇到这种情况，就用transfer_buffer。
					 */
        dma_addr_t transfer_dma;        /* (in) dma addr for transfer_buffer 
					 * 这是一个, 这三个成员是用来描述主机上的缓冲区
					 *
					 * transfer_dma是使用usb_buffer_alloc分配的dma缓冲区。
					 * HCD不会同时使用transfer_buffer和transfer_dma，只会选择其中之一。
					 * 如果urb自带了transfer_dma，就要同时设置URB_NO_TRANSFER_DMA_MAP来告诉HCD一声，不用HCD再费心做DMA映射了。
					 * 
					 */
        struct scatterlist *sg;         /* (in) scatter gather buffer list */
        int num_mapped_sgs;             /* (internal) mapped sg entries */
        int num_sgs;                    /* (in) number of entries in the sg list */
        u32 transfer_buffer_length;     /* (in) data buffer length 
					 * 这是一个, 这三个成员是用来描述主机上的缓冲区
					 *
					 * transfer_buffer_length指的就是transfer_buffer或transfer_dma的长度。
					 */
        u32 actual_length;              /* (return) actual transfer length 
					 * urb结束后，会用actual_length告诉你实际上传输了多少数据
					 */
        unsigned char *setup_packet;    /* (in) setup packet (control only) 
					 * 此缓冲区是控制传输专用的, 其保存的是一个struct usb_ctrlrequest结构体
					 * 如果进行的是控制传输，setup_packet是必须要设置的，设置它是为了防止主机控制器不支持使用DMA的情况
					 */
        dma_addr_t setup_dma;           /* (in) dma addr for setup_packet 
					 * 此缓冲区是控制传输专用的, 其保存的是一个struct usb_ctrlrequest结构体
					 * 如果你的urb设置了setup_dma，则必须同时设置标志URB_NO_SETUP_DMA_MAP来告诉HCD。
					 */
        int start_frame;                /* (modify) start frame (ISO) 
					 * 如果没有指定标志URB_ISO_ASAP，则必须设置start_frame来指定等时传输在哪个帧或哪个微帧开始。
					 * 如果指定了URB_ISO_ASAP标志，urb结束时会使用这个值返回实际的开始帧号。
					 */
        int number_of_packets;          /* (in) number of ISO packets 
					 * 指定下面的数组iso_frame_desc的大小, 也就是要传输多少了packet
					 */
        int interval;                   /* (modify) transfer interval
                                         * (INT/ISO) 
					 * 等时传输和中断传输专用
					 * 每一帧或微帧里最多有一次等时传输或中断传输，不能再多了。
					 *
					 * 高速高带宽等时端点每个微帧可以进行2到3次等时transaction
					 */
        int error_count;                /* (return) number of ISO errors */
        void *context;                  /* (in) context for completion 
					 * 驱动设置的为函数complete使用的。可以将驱动里描述自己设备的结构体放在context里面，在complete函数里再把它取出来使用
					 */
        usb_complete_t complete;        /* (in) completion routine 
					 * 指向完成处理函数的函数指针。传输成功完成时，或中间发生错误时就会调用它。
					 * 驱动可以在这里检测urb状态，并做一些处理。比如，可以释放这个urb，或者重新提交给HCD.
					 * 实际应用场景：
					 * 摄像头，你向HCD提交了一个等时传输的urb，从摄像头那里读取视频数据，传输完成时调用了你指定的这个完成处理函数，并在里面取出了urb里面获得的视频数据，
					 * 并进行解码等处理，然后怎么办？总不会这一个urb读取的数据就够了吧，所以需要获得更多的数据，那你也总不会再去创建、初始化一个等时urb吧，很明显刚刚的
					 * urb可以继续使用，只要将它再次提交给HCD就可以了。
					 *
					 * 等时传输与其他传输不一样，可以指定传输多少个packet,
					 * 每个packet使用struct usb_iso_packet_descriptor结构来描述
					 */
        struct usb_iso_packet_descriptor iso_frame_desc[0];	/* 每个结构体usb_iso_packet_descriptor都代表了一次等时传输
								 * 等时传输低层packet情况说明，不像控制传输最少要有SETUP和STATUS两个阶段的transaction，等时传输只有Isochronous transaction,
								 * 即等时transaction一个阶段，一次等时传输就是一次等时transaction过程。
								 * 等时transaction本身只有两个阶段，一个是主机向设备发送OUT Token包，然后发送一个Data包；
								 * 或者是，主机向设备发送In Token包，然后设备向主机发送一个Data包。
								 * 这里没有Handshake包，因为等时传输是不保证数据完全正确无误到达的，没有什么错误重传机制，也就不需要Handshake包来汇报。
								 * 对它来说实时性远比正确性要重要的多，摄像头每秒多传一帧或少传一帧没有本质区别，
								 * 如果等时传输延迟几秒，感觉就明显不同了。
								 * 
								 * 对于等时传输来说，在完成了number_of_packets次传输后，会去调用你的完成处理函数，在里面对数据进行处理，
								 * 而error_count成员记录了这么多次传输中发生错误的次数。
								 */
                                        /* (in) ISO ONLY */
};

struct kref {				/* 内核中就是使用这个来判断一个对象有没有用*/
        refcount_t refcount;
};

void usb_kill_urb(struct urb *urb)
{
        might_sleep();
        if (!(urb && urb->dev && urb->ep))
                return;
        atomic_inc(&urb->reject);

        usb_hcd_unlink_urb(urb, -ENOENT);		/* 异步的终止这次urb通信, 直接返回了，但并不代表urb的通信会立即终止 */
        wait_event(usb_kill_urb_queue, atomic_read(&urb->use_count) == 0);	/* 所以，睡一会儿，等着urb通信终止，判断条件就是use_count为0*/

        atomic_dec(&urb->reject);
};

struct usb_iso_packet_descriptor {
        unsigned int offset;		/* 表示transfer_buffer里的偏移位置 
					 */
        unsigned int length;            /* expected length, 预期这次等时传输Data包中数据的长度 */
        unsigned int actual_length;	/* urb结束时，每个usb_iso_packet_descriptor结构体成员actual_length表示各次等时传输实际传输的数据长度，而status记录了它们的状态 
					 */
        int status;
};

usb_start_wait_urb(struct * urb, int timeout, int * actual_length);/* timeout的单位是ms
								    */

wait_for_completion_timeout  /* 里的超时参数必须以jiffy为单位
			      * jiffy, 意思是瞬间，短暂的时间跨度。短暂到什么程度？Linux里它表示两次时钟中断的间隔，
			      * 时钟中断是由定时器周期性产生的
			      * 关于周期，内核中有一个十分形像的变量来描述，就是HZ，它是一个体系结构相关的常数。
			      * 内核中还提供了专门的计数器记录从系统引导开始所度过的jiffy值，每次时钟中断发生时，这个计数器就增加1
			      * 
			      * ms转jiffy可以使用函数msecs_to_jiffies();
			      */

/*
 * ~ 是按位取反
 * UL是无符号长整数
 * ULL 是64位的无符号长整型
 * <<,>> 在C里，往右移后，空出来的那些空缺，对于无符号整数得补0，对于有符号数，得补上符号位
 */

usb_interrupt_msg()
usb_bulk_msg()
usb_submit_urb()		/* 这个函数对于驱动来说非常重要，驱动想从设备那里获得信息就是靠它来实现的，只有在usb_submit_urb正确的提交了请求，才会有后面的一切
				 * 所以，在提交urb之前，一定要对其进行正确的初始化, urb元素的每一个值决定了整个usb驱动能否顺利运转
				 * 对于usb驱动来说，usb_submit_urb是异步的，只要它返回0就代表urb请求提交成功了，接下来的事就交给usb core和HCD就可以了，当它们处理完，
				 * 会调用你在驱动给传给urb的完成函数, 之后你就可以处理设备返回的数据了, 只有到了完成函数里，你才又一次得到urb控制权，
				 * 你可以检查urb是否出错了，需不需要释放或重新提交.
				 *
				 * 这里有另一个重要的问题，对于中断/等时传输，是怎么实现让主机控制器按一定周期去轮询端点的？因为前面在端点的Interval成员中指定了主机轮询的时间间隔，
				 * 而背后主机控制器是如何按照指定的Interval值定期实现轮询的？
				 * 事实上是这样的，当驱动第一次提交一个中断或等时urb请求时，HCD会根据Interval值来判断自己是否能够满足你的需求，
				 * 如果不能够安排足够的带宽来完成这种周期性的传输，HCD是不会批准此次urb请求的，如果HCD认为它可以满足，就会保留足够的带宽，
				 * 但这并不表示万事大吉了，HCD是保留了带宽，但同时也需要驱动确保在urb队列里总是有urb，不能为空，否则这个保留的带宽就会被取消掉
				 * 新问题来了，驱动如何保证urb队列里一直有urb？驱动需要在完成函数中重新初始化和提交刚刚完成的urb。
				 * 注意，不能在完成函数里修改Interval值。
				 * 
				 * usb core里还封装了usb_control_msg, usb_bulk_msg, usb_interrupt_msg这三个函数，它们是同步的, 会等待传输完成，用它们三个，你将不需要创建urb，不用初始化urb，
				 * 也不用调用usb_submit_urb。
				 * 
				 */

struct usb_hcd {		/* HCD片区的王中王-----主机控制器驱动 */
	/*
         * housekeeping
         */
        struct usb_bus          self;           /* hcd is-a bus 
						 * 一个主机控制器就会连出一条USB总线
						 * 对于写驱动的人来说，总线和主机控制器差不多是同一样东西，用不着分得那么清楚，可以简单的说它们都是用来描述主机控制器的
						 * 那为什么分成了两个结构usb_bus和usb_hcd？
						 * 首先，主机控制器是一个硬件设备，而且更多时候它是一个PCI设备，那么它就应该纳入PCI设备范畴之内，
						 * 也就是说，struct usb_hcd中就应当包括：struct device或者struct pci_dev这两者之一，
						 * 但是，在usb_hcd中没有这两个中的任何一个，这可怎么办呢？
						 * 看一下struct usb_bus就明白了，里面的第一个成员就是struct device
						 *
						 * 以UHCI为例来解释一下
						 * 首先它是一个pci设备，要使用pci_register_driver来注册一个struct pci_driver结构体uhci_pci_driver
						 * 在uhci_pci_driver里有函数probe，在这个probe函数里，它会调用usb_create_hcd来创建一个usb_hcd，并初始化usb_hcd.self，
						 * 还将这个self里的controller设定为描述主机控制器pci_dev里的struct device结构体，
						 * 从而将usb_hdc, usb_bus, pci_dev这三者联系到一起
						 *
						 * struct uhci_hcd是什么？它是uhci自己私有的一个结构体，每个具体的主机控制器都有这么一个类似的结构体。
						 * 每个主机控制器的这类私有数据结构体都藏在struct usb_hcd结构体成员hcd_priv变长数组里
						 *
						 * 对于具体的主机控制器驱动，它们的眼里只有struct usb_hcd
						 * 没有usb_create_hcd去创建usb_hcd，就不会有usb_bus的存在
						 * 对于linux设备模型来说，usb_bus会更亲近一些。你可以把struct usb_bus当作是嵌入到usb_hcd中的一个结构体，它将usb_hcd要完成的一部分工作进行了封装，
						 * 因为描述一个主机控制器太复杂了，于是就创建了一个usb_bus去专门应对设备模型、sysfs等。
						 *
						 * Greg它们描述上面这种usb_hcd和usb_bus的这种奇妙关系为HCD bus-glue layer，并致力于“flatten out it”
						 * 它们之间的发展方向是，usb_hcd越来越走向前台，而usb_bus越来越走向幕后
						 */
        struct kref             kref;           /* reference counter */

        const char              *product_desc;  /* product/vendor string 
						 * 主机控制器的产品描述字符串，对于UHCI它就是UHCI Host Controller; 对于EHCI它就是EHCI Host Controller.
						 *
						 */
        int                     speed;          /* Speed for this roothub.
                                                 * May be different from
                                                 * hcd->driver->flags & HCD_MASK
                                                 */
        char                    irq_descr[24];  /* driver + bus # 
						 * 这里保存的是ehci-hcd:usb1之类的字符串，也就是驱动的名称再加上总线的编号
						 */

        struct timer_list       rh_timer;       /* drives root-hub polling */
        struct urb              *status_urb;    /* the current status urb */
#ifdef CONFIG_PM
        struct work_struct      wakeup_work;    /* for remote wakeup */
#endif

        /*
         * hardware info/state
         */
        const struct hc_driver  *driver;        /* hw-specific hooks 
						 * 每个主机控制器驱动都有一个struct hcd_driver结构体
						 * 它与usb_driver, pci_driver一样，所有的xxx_driver都有一堆函数指针，具体的主机控制器就靠它们来实现
						 * hc_driver.hcd_priv_size的值是变长数组hcd_priv的长度
						 */

        /*
         * OTG and some Host controllers need software interaction with phys;
         * other external phys should be software-transparent
         */
        struct usb_phy          *usb_phy;
        struct phy              *phy;

        /* Flags that need to be manipulated atomically because they can
         * change while the host controller is running.  Always use
         * set_bit() or clear_bit() to change their values.
         */
        unsigned long           flags;
	#define HCD_FLAG_HW_ACCESSIBLE          0       /* at full power */
#define HCD_FLAG_POLL_RH                2       /* poll for rh status? */
#define HCD_FLAG_POLL_PENDING           3       /* status has changed? */
#define HCD_FLAG_WAKEUP_PENDING         4       /* root hub is resuming? */
#define HCD_FLAG_RH_RUNNING             5       /* root hub is running? */
#define HCD_FLAG_DEAD                   6       /* controller has died? */
#define HCD_FLAG_INTF_AUTHORIZED        7       /* authorize interfaces? */
#define HCD_FLAG_DEV_AUTHORIZED         8       /* authorize devices? */

        /* The flags can be tested using these macros; they are likely to
         * be slightly faster than test_bit().
         */
#define HCD_HW_ACCESSIBLE(hcd)  ((hcd)->flags & (1U << HCD_FLAG_HW_ACCESSIBLE))
#define HCD_POLL_RH(hcd)        ((hcd)->flags & (1U << HCD_FLAG_POLL_RH))
#define HCD_POLL_PENDING(hcd)   ((hcd)->flags & (1U << HCD_FLAG_POLL_PENDING))
#define HCD_WAKEUP_PENDING(hcd) ((hcd)->flags & (1U << HCD_FLAG_WAKEUP_PENDING))
#define HCD_RH_RUNNING(hcd)     ((hcd)->flags & (1U << HCD_FLAG_RH_RUNNING))
#define HCD_DEAD(hcd)           ((hcd)->flags & (1U << HCD_FLAG_DEAD))

        /*
         * Specifies if interfaces are authorized by default
         * or they require explicit user space authorization; this bit is
         * settable through /sys/class/usb_host/X/interface_authorized_default
         */
#define HCD_INTF_AUTHORIZED(hcd) \
        ((hcd)->flags & (1U << HCD_FLAG_INTF_AUTHORIZED))

        /*
         * Specifies if devices are authorized by default
         * or they require explicit user space authorization; this bit is
         * settable through /sys/class/usb_host/X/authorized_default
         */
#define HCD_DEV_AUTHORIZED(hcd) \
        ((hcd)->flags & (1U << HCD_FLAG_DEV_AUTHORIZED))

        /* Flags that get set only during HCD registration or removal. */
        unsigned                rh_registered:1;/* is root hub registered? */
        unsigned                rh_pollable:1;  /* may we poll the root hub? */
        unsigned                msix_enabled:1; /* driver has MSI-X enabled? */
        unsigned                msi_enabled:1;  /* driver has MSI enabled? */
        unsigned                remove_phy:1;   /* auto-remove USB phy */
	unsigned                uses_new_polling:1;
        unsigned                wireless:1;     /* Wireless USB HCD */
        unsigned                has_tt:1;       /* Integrated TT in root hub */
        unsigned                amd_resume_bug:1; /* AMD remote wakeup quirk */
        unsigned                can_do_streams:1; /* HC supports streams */
        unsigned                tpl_support:1; /* OTG & EH TPL support */
        unsigned                cant_recv_wakeups:1;
                        /* wakeup requests from downstream aren't received */

        unsigned int            irq;            /* irq allocated 
						 * 这与主机控制器的娘家PCI有关的
						 * 讲PCI，就需要了解“PCI配置寄存器”
						 */
        void __iomem            *regs;          /* device memory/io 
						 * 调用ioremap_nocache映射后的内存地址
						 */
        resource_size_t         rsrc_start;     /* memory/io resource start 
						 * 从PCI配置寄存器里读取来的主机控制器的I/O端口或内存的首地址
						 */
        resource_size_t         rsrc_len;       /* memory/io resource length 
						 * 从PCI配置寄存器里读取来的主机控制器的I/O端口或内存的首长度
						 */
        unsigned                power_budget;   /* in mA, 0 = no limit */

        struct giveback_urb_bh  high_prio_bh;
        struct giveback_urb_bh  low_prio_bh;
	struct mutex            *address0_mutex;
        struct mutex            *bandwidth_mutex;
        struct usb_hcd          *shared_hcd;
        struct usb_hcd          *primary_hcd;


#define HCD_BUFFER_POOLS        4
        struct dma_pool         *pool[HCD_BUFFER_POOLS];	/* 几个dma池, 因为其可以有4个dma池，所以就表示每个主机控制器可以有四个dma池
								 * 前面说过，主机控制器是可以进行dma传输的，回到struct urb，它有两个成员transfer_dma, setup_dma，
								 * 前面只是说可以使用usu_buffer_alloc分配好dma缓冲区给它们，然后再告诉HCD, urb已经有dma缓冲区了，
								 * 这样HCD就可以不再用进行复杂的DMA映射了。但是并没有提到urb获取的DMA缓冲区是从哪里来的，就是这里的pool池.
								 *
								 * 看一下这个pool是怎么创建出来的，它是在hcd_buffer_create中创建的
								 * 
								 * DMA池还有其它一些用途。
								 * 一般来说DMA映射获得的都是以页为单位的内存，urb不需要这么大的内存，如果需要比较小的DMA缓冲区，就离不开DMA池了。
								 * 看hcd_buffer_create内部是使用dma_pool_create来创建DMA池的
								 *
								 * 有一点需要注意，当主机控制器不支持DMA时，也可以使用hcd_buffer_alloc, hcd_buffer_free，但创建的不再是dma池子，
								 * 存取的也不是dma缓冲区。此时hcd_buffer_alloc取到的只是kmalloc分配的普通内存。size, 也可以不是pool_max中指定的那四个尺寸。
								 * pool_max数组中四项，是DMA缓冲区可以申请的缓冲区大小，但并不是说只可以取这四个值，如果你申请的size小于这四个中的某一个，
								 * 则会返回一个略大的那个，如果你申请的比所有这四个值都大，也是可以的。
								 */

        int                     state;				/* 主机控制器的状态 */
#       define  __ACTIVE                0x01
#       define  __SUSPEND               0x04
#       define  __TRANSIENT             0x80

#       define  HC_STATE_HALT           0
#       define  HC_STATE_RUNNING        (__ACTIVE)
#       define  HC_STATE_QUIESCING      (__SUSPEND|__TRANSIENT|__ACTIVE)
#       define  HC_STATE_RESUMING       (__SUSPEND|__TRANSIENT)
#       define  HC_STATE_SUSPENDED      (__SUSPEND)

#define HC_IS_RUNNING(state) ((state) & __ACTIVE)
#define HC_IS_SUSPENDED(state) ((state) & __SUSPEND)

        /* more shared queuing code would be good; it should support
         * smarter scheduling, handle transaction translators, etc;
         * input size of periodic table to an interrupt scheduler.
         * (ohci 32, uhci 1024, ehci 256/512/1024).
         */

        /* The HC driver's private data is stored at the end of
         * this structure.
         */
        unsigned long hcd_priv[0]
                        __attribute__ ((aligned(sizeof(s64))));


};

struct usb_bus {		/* 表示usb总线 */
	struct device *controller;      /* host/master side hardware 
					 * 
					 */
        struct device *sysdev;          /* as seen from firmware or bus */
        int busnum;                     /* Bus number (in order of reg) 
					 * 总线编号
					 * 计算机里应该可以有多个主机控制器吧，自然也就可以有多个usb总线，因为有多条usb总线，所以需要编号来方便确认。
					 */
        const char *bus_name;           /* stable id (PCI slot_name etc) 
					 * 在大多数情况下，主机控制器都是一个pci设备，那么bus_name应该就是在pci总线上标识usb主机控制器的名字，
					 * pci总线使用PCI ID来标识pci设备，所以bus_name里保存的就应该是主机控制器对应的pci id
					 */
        u8 uses_dma;                    /* Does the host controller use DMA? 
					 * 用来表示，这个主机控制器是否支持DMA
					 * 主机控制器的一项重要工作就是在内存和USB总线之间传输数据，这个过程可以使用DMA也可以不使用DMA
					 * 不使用DMA的方式即所谓的PIO方式
					 */
        u8 uses_pio_for_control;        /*
                                         * Does the host controller use PIO
                                         * for control transfers?
                                         */
        u8 otg_port;                    /* 0, or number of OTG/HNP port */
        unsigned is_b_host:1;           /* true during some HNP roleswitches */
        unsigned b_hnp_enable:1;        /* OTG: did A-Host enable HNP? */
        unsigned no_stop_on_short:1;    /*
                                         * Quirk: some controllers don't stop
                                         * the ep queue on a short transfer
                                         * with the URB_SHORT_NOT_OK flag set.
                                         */
        unsigned no_sg_constraint:1;    /* no sg constraint */
        unsigned sg_tablesize;          /* 0 or largest number of sg list entries */

        int devnum_next;                /* Next open device number in
                                         * round-robin allocation 
					 * 记录devmap表中下一个为0的位
					 * devmap表里面为1的位都是已经被这条USB总线上的设备占用了的
					 */
        struct mutex devnum_next_mutex; /* devnum_next mutex */

        struct usb_devmap devmap;       /* device address allocation map */
        struct usb_device *root_hub;    /* Root hub 
					 * USB主机控制器：USB总线：Root Hub的关系是1:1:1
					 * 它们三个是一一对应的
					 * 一个usb主机控制器有一个usb总线，一个usb总线有一个Root Hub
					 */
        struct usb_bus *hs_companion;   /* Companion EHCI bus, if any */

        int bandwidth_allocated;        /* on this bus: how much of the time
                                         * reserved for periodic (intr/iso)
                                         * requests is used, on average?
                                         * Units: microseconds/frame.
                                         * Limits: Full/low speed reserve 90%,
                                         * while high speed reserves 80%.
					 *
					 * 用来表示总线为中断传输和等时传输预留了多少带宽
					 * 协议中规定：
					 * 对于高速设备来说，最多可以预留80%的带宽
					 * 对于低速和全速设备来说会多一点，可以预留90%带宽
					 * 它的单位是ms，表示一帧或微帧内有多少ms可以预留给中断或等时传输使用
					 * 
                                         */
        int bandwidth_int_reqs;         /* number of Interrupt requests 
					 * 当前中断传输的数量
					 */
        int bandwidth_isoc_reqs;        /* number of Isoc. requests */

        unsigned resuming_ports;        /* bit array: resuming root-hub ports */

#if defined(CONFIG_USB_MON) || defined(CONFIG_USB_MON_MODULE)
        struct mon_bus *mon_bus;        /* non-null when associated 
					 * 在driver/usb/mon目录下的Kconfig中找一下USB_MON一项
					 * 如果定义了CONFIG_USB_MON，USB Mon就会编译进内核，即所谓的USB监视器
					 * 它是用来监视USB总线上的底层通信流
					 */
        int monitored;                  /* non-zero when monitored */
#endif
};

struct hc_driver {
        const char      *description;   /* "ehci-hcd" etc */
        const char      *product_desc;  /* product/vendor string */
        size_t          hcd_priv_size;  /* size of private data */

        /* irq handler */
        irqreturn_t     (*irq) (struct usb_hcd *hcd);
...
};

int usb_hcd_submit_urb (struct urb *urb, gfp_t mem_flags) {		/* 此函数的目标明确，就是把urb传递给合适的主机控制器驱动
									 * drivers/usb/core目录下的hcd打头的几个文件，严格来说都不能算是hcd驱动，它们只能算是hcd驱动的接口，
									 * 它们是主机控制器的接口层，用来连接具体的主机控制器驱动和usb core
									 *
									 * 信号量和完成量只能用在可以睡眠的环境，自旋锁只能用在不能休眠的环境
									 */

};

/* drivers/usb/core/hub.c 
 */

static struct usb_driver hub_driver = {
        .name =         "hub",
        .probe =        hub_probe,
        .disconnect =   hub_disconnect,
        .suspend =      hub_suspend,
        .resume =       hub_resume,
        .reset_resume = hub_reset_resume,
        .pre_reset =    hub_pre_reset,
        .post_reset =   hub_post_reset,
        .unlocked_ioctl = hub_ioctl,
        .id_table =     hub_id_table,
        .supports_autosuspend = 1,
};

hub_probe---->INIT_WORK(&hub->events, hub_event);
hub_event
|---------> port_event
	|-------> hub_port_connect_change
		|------> hub_port_connect
			|------> usb_port_init
			|------> usb_new_device
				|----> usb_enumerate_device
					|------>usb_get_configuration
						|------> usb_get_descriptor
						|------> dev->rawdescriptors[i] = buffer 	/* 这里从设备返回的数据是原始的（raw），所有数据：配置描述符、接口描述符、端点描述符都挤在一起，
												 * 所以需要把它们解析出来 
												 * buffer里保存的就是GET_DESCRIPTOR请求获得的数据
												 * 不出意外的话，buffer的前USB_DT_CONFIG_SIZE个字节对应的就是配置描述符
												 */
						|------> usb_parse_configuration		/* 把raw的buffer拆开解析 
											 	 * 通过GET_DESCRIPTOR请求得到的数据并不是无序的，只是它们都挤在一起放在字符串里，
											 	 * 一般来说，配置描述符后面跟的是第一个接口的接口描述符，接着是这个接口的端点描述符，
												 * 如果有class-specific和vendor-specific描述符的话，
											 	 * 会跟在对应的标准描述符后面，不管接口有多少个端点都是按照这个顺序排列的。
											 	 * 但有些厂商非要先返回第二个接口后再返回第一个接口，
												 * 但配置描述符后总归是先接口描述符之后是端点描述符，大的规律是不变的
											 	 * 
											 	 * 配置描述符后面跟的不一定就是接口描述符，还可能是class-specific和vendor-specific描述符。
											 	 */
							|----------> usb_parse_interface 
									|----------> usb_parse_endpoint /* 经过usb_parse_configuration, usb_parse_interface, 
													 * usb_parse_endpoint这三个函数的层层推进，
													 * 通过GET_DESCRIPTOR请求获得的那堆数据现在已经解析完成。到这里为止，
													 * 设备的配置已经了然于胸，
													 * 那问题来了，接下来该怎么走？
													 * 可以进入Configured状态了吗？事情没有这么简单，光是获得设备各个配置信息没用，
													 * 要进入Configured状态，你还得有选择、有目的、有步骤、有计划地配置设备，
													 * 那怎么才可以做到？这就不是usb core可以回答的问题了，usb core不会知道设备想采用哪种设置，
													 * 只有设备驱动自己才知道，所以接下来要做的是去在设备模型中寻找属于自己的驱动。
													 */
			|-----> hub_power_remaining
					
```

* usb_generic_driver, 对所有usb_device_type都来者不拒的“花心大萝卜”
```c
/* drivers/usb/core/generic.c
 */

struct usb_device_driver usb_generic_driver = {
        .name = "usb",
        .probe = generic_probe,
        .disconnect = generic_disconnect,
#ifdef  CONFIG_PM
        .suspend = generic_suspend,
        .resume = generic_resume,
#endif
        .supports_autosuspend = 1,
};

static int generic_probe(struct usb_device *udev)
{
        int err, c;

        /* Choose and set the configuration.  This registers the interfaces
         * with the driver core and lets interface drivers bind to them.
         */
        if (udev->authorized == 0)
                dev_err(&udev->dev, "Device is not authorized for usage\n");
        else {
                c = usb_choose_configuration(udev);		/* 这里接上前面usb_get_configuration“从设备中取出配置信息并解析完成后”，
								 * 由驱动接手，接着需要做的事是找到上面的取得的配置信息并执行配置 
								 */
                if (c >= 0) {
                        err = usb_set_configuration(udev, c);	/* 设置配置, 从此之后设备就可以进入到“Configurated”状态
								 * 什么时候会disable 端点0?
								 * 一种是设备要断开时，另一种是设备从Default状态进化到Address状态时。
								 * 在设备的生命线里，设置完设备地址，让设备进入到Address状态后，立马就调用了hub.c中的ep0_reinit()，
								 * 这个函数只对端点0调用了usb_disable_endpoint, 但是端点0还是要用的，所以，接着重新将ep0赋给ep_in[0]和ep_out[0]
								 *
								 * SET_CONFIGRATION请求是针对设备的标准请求，且不需要DATA transaction.
								 * 
								 * 设备自从有了Address，拿到了各种描述符，就在那儿看着usb_generic_driver忙活
								 *
								 * Address状态有点像你刚买了套房子，如果不装修，它对你来说的意义就是可以对别人说你的家庭住址在哪，而实际上你在房子里什么也干不了，
								 * Configured状态就像是你的房子已经装修好了
								 *
								 * 简单的来说，usb_set_configuration或者说usb_generic_driver忙活的就是为设备的接口和接口驱动匹配做前置准备工作
								 * 它会将准备好的接口送给设备模型，Linux设备模型会根据总线类型将接口添加到USB总线的设备链表里，
								 * 然后轮询USB总线的驱动链表，针对每个找到的驱动去调用USB总线的match函数usb_device_match，去为接口寻找与它匹配的驱动
								 * 此时，设备已经被设置成了接口了，所以在usb_device_match函数里该走接口的分支了
								 *
								 * 那现在又来问题了，这些接口的驱动是从哪里来的？
								 * 当然是insmod, modprobe了
								 * 
								 * 
								 */
                        if (err && err != -ENODEV) {
                                dev_err(&udev->dev, "can't set config #%d, error %d\n",
                                        c, err);
                                /* This need not be fatal.  The user can try to
                                 * set other configurations. */
                        }
                }
        }
        /* USB device state == configured ... usable */
        usb_notify_add_device(udev);

        return 0;
}


/* driver/usb/core/usb.c
 */

subsys_initcall(usb_init);

usb_init()
	|------> usb_register_device_driver(&usb_generic_driver, THIS_MODULE);	/* 所有的USB设备都是绑定到usb_generic_driver上的
										 */

/* 驱动的生命线是从USB子系统的初始化函数usb_register_device_driver开始的

 */
int usb_register_device_driver(struct usb_device_driver *new_udriver,                                                                                           
                struct module *owner)                                                                                                                           
{                                                                                                                                                               
        int retval = 0;                                                                                                                                         
                                                                                                                                                                
        if (usb_disabled())                                                                                                                                     
                return -ENODEV;                                                                                                                                 
                                                                                                                                                                
        new_udriver->drvwrap.for_devices = 1; 				/*这表示是设备的驱动，不是接口的哟, is_usb_device_driver会用其做判断*/                                                    
        new_udriver->drvwrap.driver.name = new_udriver->name;                                                                                                   
        new_udriver->drvwrap.driver.bus = &usb_bus_type;                                 
        new_udriver->drvwrap.driver.probe = usb_probe_device;           /*当usb_generic_driver与USB设备匹配成功后，就会调用这里指定的函数usb_probe_device*/                 
        new_udriver->drvwrap.driver.remove = usb_unbind_device;                          
        new_udriver->drvwrap.driver.owner = owner;                                       
                                                                                         
        retval = driver_register(&new_udriver->drvwrap.driver);       /* driver_register是设备模型的函数，它可以将dricer添加到USB设备总线的驱动链表里
								       * 这里会调用driver_attach，来找usb总线上的设备，会执行总上的match函数usb_device_match，
								       * 从而将usb设备的接口与这里注册的驱动连上
								       * 但这个是usb子系统初始化时就执行了，可能此时设备还没有接入系统，所以应还有其他入口
								       * 比如，设备接入后会触发usb总线上的match函数进入interface分支，从而让设备进入Configured状态
								       */                   
                                                                                         
        if (!retval)                                                                     
                pr_info("%s: registered new device driver %s\n",                         
                        usbcore_name, new_udriver->name);                                
        else                                                                             
                printk(KERN_ERR "%s: error %d registering device "                       
                        "       driver %s\n",                                            
                        usbcore_name, retval, new_udriver->name);                        
                                                                                         
        return retval;                                                                                                                                          
} 


static int usb_device_match(struct device *dev, struct device_driver *drv)
{
        /* devices and interfaces are handled separately */
        if (is_usb_device(dev)) {

                /* interface drivers never match devices */
                if (!is_usb_device_driver(drv))
                        return 0;

                /* TODO: Add real matching code */
                return 1;

        } else if (is_usb_interface(dev)) {
                struct usb_interface *intf;
                struct usb_driver *usb_drv;
                const struct usb_device_id *id;

                /* device drivers never match interfaces */
                if (is_usb_device_driver(drv))
                        return 0;

                intf = to_usb_interface(dev);
                usb_drv = to_usb_driver(drv);

                id = usb_match_id(intf, usb_drv->id_table);
                if (id)
                        return 1;

                id = usb_match_dynamic_id(intf, usb_drv);
                if (id)
                        return 1;
		/* 到这里为止，可以讲usb core的工作就已经完成了，之后就会到usb设备自己的驱动的probe函数做更进一步的处理了，什么样的处理？只有驱动知道了.
		 * 怎么到这里的？
		 */
        }

        return 0;
}

```
* usb_register()
> 它是个宏，用来向usb core注册一个usb设备驱动

```c


```

* hub.c 
```c
usb_init()
	|-----> usb_hub_init()			/* hub ，这个是root hub，因为是在usb子系统初始化时调用的*/
		|-----> hub_wq = alloc_workqueue("usb_hub_wq", WQ_FREEZABLE, 0);	/* 而root hub用workqueue来管理，而不是2.6版本时的内核线程khubd*/

/* 
 */
static struct usb_driver hub_driver = {		/* hub 的驱动 */
       .name =         "hub",
       .probe =        hub_probe,		/* */
       .disconnect =   hub_disconnect,
       .suspend =      hub_suspend,
       .resume =       hub_resume,
       .reset_resume = hub_reset_resume,
       .pre_reset =    hub_pre_reset,
       .post_reset =   hub_post_reset,
       .unlocked_ioctl = hub_ioctl,
       .id_table =     hub_id_table,
       .supports_autosuspend = 1,
; 

hub_probe()
	|-----> INIT_WORK(&hub->events, hub_event);
	|-----> hub_configure
		|-----> usb_fill_int_urb(hub->urb, hdev, pipe, *hub->buffer, maxp, hub_irq, hub, endpoint->bInterval);
			\___hub_irq
				|------>kick_hub_wq
					|-----> queue_work(hub_wq, &hub->events)
		|-----> hub_activate
			|-----> kick_hub_wq

port_event, hub_event
	|-----> usb_reset_device
		|-----> usb_reset_and_verify_device

core/generic.c----> generic_resume -----> usb_port_resume
core/sysfs.c-----> remove_store ------> usb_remove_device



usb_remove_device, usb_port_resume, usb_reset_and_verify_device
	hub_port_logical_disconnect
		|-----> kick_hub_wq

/*当一个usb设备插入usb接口之后究竟会发生什么 */
hub_port_connect_change
	|-----> hub_port_connect
		|-----> hub_port_init

/* 对于usb来说，只要调用device_add函数向设备模型核心层添加设备就够了，对于剩下的事情，设备模型核心层会去处理，这就是设备模型的优点，所以也叫统一的设备模型。
 * 就是说，不管是pci、usb还是scsi，总线驱动的工作就是申请并建立总线的数据结构，
 * 设备驱动的工作就是往这条总线上注册，调用driver_add，而设备的工作一样，就是调用device_add往总线上注册。
 * driver_add会在总线上寻找每一个设备，如果找到了自己支持的设备，并且该设备没有和别的驱动相互绑定，那么就绑定该设备。
 * 设备的做法也一样，device_add在总线上寻找每一个设备驱动，找到了合适的就绑定该设备驱动。最后，调用probe函数，将兵权交给设备驱动。整个过程就叫做usb设备初始化。
 */

```

* usb_port_init
```c
static int hub_set_address(struct usb_device *udev, int devnum)
{
        int retval;
        struct usb_hcd *hcd = bus_to_hcd(udev->bus);

        /*
         * The host controller will choose the device address,
         * instead of the core having chosen it earlier
         */
        if (!hcd->driver->address_device && devnum <= 1)
                return -EINVAL;
        if (udev->state == USB_STATE_ADDRESS)
                return 0;
        if (udev->state != USB_STATE_DEFAULT)
                return -EINVAL;
        if (hcd->driver->address_device)
                retval = hcd->driver->address_device(hcd, udev);
        else
                retval = usb_control_msg(udev, usb_sndaddr0pipe(),		/* 设置设备地址, 只有设置了真正的地址后，硬件上才能真正通过这个地址进行通信*/
                                USB_REQ_SET_ADDRESS, 0, devnum, 0,
                                NULL, 0, USB_CTRL_SET_TIMEOUT);
        if (retval == 0) {
                update_devnum(udev, devnum);
                /* Device now using proper address. */
                usb_set_device_state(udev, USB_STATE_ADDRESS);			/* 设备地址设备成功后，设置设备的状态为Address, 这个状态表示有地址的状态*/
                usb_ep0_reinit(udev);						/* 为什么要调用reinit?
										 * 主机控制器会记录每一个端点的状态，在每一次设备的状态发生变化之后就需要更新主机控制器记录的端点状态。
										 * 具体来说，ep0是一个struct usb_host_endpoint，它的成员都是为主机控制器驱动使用。
										 * 它的成员struct list_head urb_list用来保存针对该端点的urb请求，即它保存的是urb请求组成的一个urb请求队列，
										 * 也许这个urb请求队列里没有urb请求，但如果有urb请求，那么在改变了设备地址后，
										 * 就需要将urb_list在设备地址改变前的所有urb请求清除掉, 而这就是ep0_reinit要通知主机控制器做的事，
										 * 由主机控制器具体清除这些urb请求。
										 *
										 * 当一个设备没有设置地址时，它使用默认地址，即地址0,而当我们设置完地址后，这个地址发生变化，
										 * 所以主机控制器必须知道这件事, 进一步的说其实是让主机控制器驱动知道这件事，因为与从设备通信的是hcd.
										 * usb_disable_endpoint()最终会调用主机控制器提供的函数来让主机控制器清除urb_list里的urb请求，并更新hcd
										 *
										 * 这里是因为设备的地址改变了要执行ep0_reinit，如果设备的包大小改变了，也需要执行ep0_reinit，这些都是设备的状态变了
										 */
        }
        return retval;
}

void usb_detect_quirks(struct usb_device *udev)					/*2.6.31里时，usb_detect_quirks是放在usb_new_deivces中的*/
{
        udev->quirks = __usb_detect_quirks(udev, usb_quirk_list);		/* usb_quirk_list是usb黑名单
										 * 本质上来说，usb_detect_quirks的作用就是看目标设备是否在这张usb黑名单里
										 * 如果在，则判断是具体哪种问题
										 */

        /*
         * Pixart-based mice would trigger remote wakeup issue on AMD
         * Yangtze chipset, so set them as RESET_RESUME flag.
         */
        if (usb_amd_resume_quirk(udev))
                udev->quirks |= __usb_detect_quirks(udev,			/* 将发现的quirk写到usb_device.quirks */
                                usb_amd_resume_quirk_list);

        if (udev->quirks)
                dev_dbg(&udev->dev, "USB quirks for this device: %x\n",
                        udev->quirks);

#ifdef CONFIG_USB_DEFAULT_PERSIST
        if (!(udev->quirks & USB_QUIRK_RESET))
                udev->persist_enabled = 1;
#else
        /* Hubs are automatically enabled for USB-PERSIST */
        if (udev->descriptor.bDeviceClass == USB_CLASS_HUB)
                udev->persist_enabled = 1;
#endif  /* CONFIG_USB_DEFAULT_PERSIST */
}


```

* usb_qualifier_descriptor
```c
/* USB_DT_DEVICE_QUALIFIER: Device Qualifier descriptor 
 * 不同的USB版本兼容性问题，比如一个高速设备接到了一个旧的hub上，总不能说用不了了吧，所以，如今的高速设备是可以以高速方式工作也可以不以高速方式工作，即可以调节，接在高速hub上就以高速方式工作，
 * 如果不是高速hub，则按全速方式工作。
 * 如何实现呢？首先，在高速和全速下有不同配置的高速设备必须具有一个usb_qualifier_descriptor描述符，它干什么用呢？它描述了一个高速设备在进行速度切换时需要的信息。比如，一个设备当前工作在高速状态，
 * 那么usb_qualifier_descriptor里保存就是全速信息，如果设备工作在全速状态，那么usb_qualifier_descriptor中保存的就是高速信息。
 *
 * 全速设备没有usb_qualifier_descriptor, 只有具有高速工作能力的设备才有
 */
struct usb_qualifier_descriptor {
        __u8  bLength;
        __u8  bDescriptorType;

        __le16 bcdUSB;
        __u8  bDeviceClass;
        __u8  bDeviceSubClass;
        __u8  bDeviceProtocol;
        __u8  bMaxPacketSize0;
        __u8  bNumConfigurations;
        __u8  bRESERVED;
} __attribute__ ((packed));

```

* usb_new_device
```c
device_add()		/* usb_new_deivce会调用device_add(), 
			 * 它是设备模型里提供的函数，从作用上来说，这个函数一执行，系统里就真正有这个设备了，/sysfs下面能看到这个设备了，
			 * 它会去遍历注册到USB总线上的所有驱动程序，如果找到合适的，就去调用这个驱动程序的probe函数，对于U盘就会调用storge_probe，对于hub就会调用hub_probe,
			 * 传递给xx_probe()函数的参数，是此前获得的struct usb_interface指针和一个struct usb_device_id指针。
			 * usb_device_id正是它在USB总线上寻找合适驱动的依据，
			 * 换句话说，每个驱动程序都会把自己支持的设备定义在一张表里，表中的每一项就是一个struct usb_device_id,
			 * 然后，当我们获得了一个具体的设备，我们就把设备的实际信息与这张表进行对比，如果找到匹配的，就认为该驱动支持该设备，从而最终会调用该驱动的probe函数。
			 * 从此，该设备就被传递到了设备驱动。到这里为止，hub驱动也完成了它最重要的一项工作。
			 */
```

* hub_power_remaining, 八大函数中的最后一个
```c
/* 执行完usb_new_device，正常情况下，hub驱动还在计算为新接进来的设备可以分多大的电流，即hub_power_remaining
 * 从软件的角度来说，hub是不会强行对新接进来的设备采取什么强制措施，最多是打印出调试信息，进行警告。而设备如果真的遇到了供电问题，它自然会出现异常，它也许不工作，这当然由设备的具体驱动负责处理。
 *
 * hub_port_connect_change函数讲的是hub驱动遇到端口连接发生变化时如何工作的
 */
```

* hub_activate-----> kick_hub_wq

* hub_irq
```c
/* 有两条道可以调用hub_events
 * 1、主动去读hub端口状态
 * 2、正常工作的hub驱动在发生中断时会读hub端口状态，中断服务的函数为hub_irq，即每当端口有变化事件发生，hub_irq就会被调用，hub_irq会调用kick_hub_wq，触发hub的event_list，于是再次调用hub_evnet()
 *
 *  热插拔的实现路径
 * hub_probe()
 *       |-----> INIT_WORK(&hub->events, hub_event);
 *       |-----> hub_configure
 *       	|-----> usb_fill_int_urb(hub->urb, hdev, pipe, *hub->buffer, maxp, hub_irq, hub, endpoint->bInterval);
 *       		\___hub端口有中断发生了，主机控制器驱动调用上面传入的urb完成函数hub_irq
 *       			|------>kick_hub_wq
 *					|-----> hub_event()
 */
```

* 电源管理，先天下之睡而睡，后天下之醒而醒
> APM

> ACPI，它的状态一共分五种:S1, S2, S3, S4, S5

```
S4就是STD
S3就是STR
在Linux中，S1被叫做Sandby, S3为STR, S5是shutdown; Linux中说挂起，主要说的就是S1, S3, S4, 在/sys/pwoer/state有三种freeze mem disk

/* 配置描述符中的bmAttributes就是标志着设备是否支持Remote Wakeup
 * Remote Wakeup指的是设备可以发一个信号，把自己唤醒，当然实际上唤醒的是总线，或者说最后的反应是唤醒主机。最简单的例子就是USB键盘，你半天不碰计算机可能大家都睡觉了，可是突然你按了一下键盘，
 * 可能就把大家唤醒了，因为你实际上是发送了一个硬件信号
 * 用命令lsusb -v看一下设备的bmAttributes属性，看看它里面有没有显示Remote Wakeup，如果有，则表示支持
 * Remote Wakeup是设备的特性，不是每个设备都具备。
 *
 * 当一个设备是在醒着的状态时，即设备醒来后需要把Wakeup功能关掉，因为只有在要睡觉的时候才需要打开Wakeup功能，就好比我们起床以后把闹钟关掉，而在晚上睡觉前定好闹钟是一个道理
 * 设置Wakeup用SetFeature，关掉Wakeup用ClearFeature.
 */
```

> STD, 挂起到磁盘; 与windows里的休眠对应。

> STR, 挂起到内存; 与windows里的Standby对应。

* David Brownell, 大卫-布劳内尔
* Alan stern, 艾仑-斯特恩
