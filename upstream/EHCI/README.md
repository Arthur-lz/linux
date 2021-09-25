# ehci主机控制器
## driver/usb/core/hub.c
```c
hub_port_init	/* 一、在取设备描述符时，现有的方案是重试，有没有更好的方法？
                 * 二、如何重试的？
		 * 1. 重试有两重，外面重试GET_DESCRIPTOR_TRIES=2次，里面重试3次
		 * 2. 里面的重试直接发urb_control_msg来向ep0发读usb设备描述符请求, 里面的3次urb请求之间没有时间间隔，如果请求结果不符合要求直接重试
		 * 3. 外层的2次重试有时间间隔（100ms），且会重置hub对应此设备的端口
		 * 
		 * 三、上面这段代码只是通过读ep0来知道usb设备的MaxPocketSize, DescriptorType，如果这两项都满足要求，则会正式的取usb设备描述符两次，一次读8字节，一次读USB_DT_DEVICE_SIZE字节，
		 *     第二次等于是读出usb e2prom中保存的usb全部配置信息（device, configuration, interface, endpoint）
		 */


/* 有两处会调用到hub_port_init, 全部在hub.c中
 * 一处是：当新设备被ehci发现时, 
 *	port_event()
 *	|-------->hub_port_connect_change()	
 * 		|-------->hub_port_connect()
 *			  |-------->hub_port_init()
 *
 * 一处是：重置usb设备时, 
 *	port_event()/hub_event()
 *		|-------->usb_reset_device()
 *			  |-------->usb_reset_and_verify_device()
 *				    |--------->hub_port_init()
 */


/* 需要了解的数据结构 */
struct usb_device {	/*在《深入Linux内核架构》中此结构被解释为“USB设备树”*/
};
```
