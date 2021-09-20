# upstream

```
这里将会记录提交第一个pacth到upstream的经历
```

## 目标1、kernel-usb方向提交一个patch

```
drivers/usb下
```

### 1.需要学习与usb相关的技能

#### 1) usb的工作原理
* (1). usb是主从设备，所有的通信全由主机控制器发起，usb设备是从设备只可以被轮询
* (2). usb设备驱动是对应接口的，不是设备，不是端点
* (3). 主机控制器与usb设备通信是通过pipe完成的，pipe通过端点地址获取
* (4). 主机控制器有uhci, ohci, ehci, xhci四种，它们是四种不同类型的芯片，对应的有四种不同的主机控制器驱动，这些驱动由主机控制器厂商提供，不需要由usb设备驱动开发人员维护
* (5). 在usb设备的E2PRom中保存着device, configure, interface, endpoint四类信息，可以通过lsusb -v查询
> device, configure, interface, endpoint对应着内核中的数据结构：

```c
usb_device_descriptor		/*每个usb设备只有一个这样的数据结构*/ 
usb_configure_descriptor 	/*configure是device描述符的子成员，它可以有多个*/
usb_interface_descriptor 	/*接口描述符是configure描述符的子成员，它可以有多个，每一个接口描述符对应一个usb设备驱动*/
usb_endpoint_descriptor 	/*每个接口描述符中可以有多个endpoint描述符，endpoint描述符定义了usb设备与主机控制器之间通信的具体细节，如pipe值即是用endpoint地址来计算的*/
// 它们定义在include/uapi/linux/usb/ch9.h
```

* (6). usb设备endpoint里会定义此设备与主机控制器间通信的类型，如果是中断类型，那么要注意usb这里定义的中断不是内核中硬件设备的中断，因为usb设备是主从设备，所以这里的中断不会让usb设备主动发起中断请求给主机控制器，与usb中断配套的在endpoint结构中还有一个成员变量bInterval，它用来定义主机控制器在中断类型时(其他类型呢？这里理解的可能不准确)轮询此usb设备的时间时隔
* (7). usb设备，如usb鼠标，它在是usb设备的同时，它也是一个输入设备，所以在主机控制器用urb与usb设备通信完成时调用的完成回调函数时，在完成函数中会处理鼠标usb设备写回的数据，并通过输入设备事件的方式通知给上层应用

> 详细的可以看一下usb鼠标的实现，在内核代码drivers/hid/usbhid/usbmouse.c

> 这里多说一句，usb的代码量是巨大的，从哪里入手很重要，由其是刚刚学习usb时，上面说了usb鼠标的usbmouse.c，其实这是一个只有200多行的文件，以它做为入门是相对简单容易的

* (8). 每个endpoint都只有一个方向，要么输入，要么输出
> 端点0是特殊端点，在主机控制器从usb设备E2PRom中取得所有的四类信息前，它需要有方法与usb设备交互，那么此时端点0的用途就是这个，它是双向的，那么主机控制器就可以通过0端点来取得usb的配置信息，同时也可以通过0端点来设备这些配置信息

* (9). 端点除了会定义方向，还会定义与主机控制器通信的方式，如中断、批量;bEndpointaddress长度一字节，低四位是定义方向，高四位是endpoint的id?
* (10). usb设备是热插拔类型设备，主机控制器的根集线器会定时轮询来判断集线器上的电压变化，将有usb设备接入时，主机控制器会通过根集线器发现电压升高，从而触发驱动匹配等一系列操作，为新插入的usb设备建立通信
