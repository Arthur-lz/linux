# Linux网络驱动模板

### Linux网络设备驱动结构
* 网络协议接口层
> dev_queue_xmit()发送数据包；netif_rx()接收数据包

> sk_buf，套接字缓冲区
> 定义在include/linux/skbuff.h
* 网络设备接口层
> 结构体net_device
* 设备驱动功能层
> hard_sart_xmit()发送数据包；中断接收数据包(NAPI常用)
* 网络设备与媒体层
> 具体设备了，即物理层


### 网络设备驱动的set_config函数模板 
```c
static int xxx_config(struct net_device *dev, struct ifmap *map)
{
	if(netif_running(dev))// 不能设置一个正在运行状态的设备
		return -EBUSY;

	// 假设不允许改变I/O地址
	if(map->base_addr != dev->base_addr){
		printk(KERN_DEBUG "xxx: can't change I/O address\n");
		return -EOPNOTSUPP;
	}

	// 假设允许改变IRQ
	if(map->irq != dev->irq)
		dev->irq = map->irq;

	return 0;
}
// net_device_stats 定义在include/linux/netdevice.h中.
```

### 网络设备驱动程序的注册和注销模板

```c
static int xxx_register(void)
{
	...
	// 分配net_device结构体并对其成员赋值
	xxx_dev = alloc_netdev(sizeof(struct xxx_priv), "sn%d", xxx_init);
	if(xxx_dev == NULL)
	// 分配net_device失败
	
	//注册net_device结构体
	if((result = register_netdev(xxx_dev)))
	....
}

static void xxx_unregister(void)
{
	...
	// 注销net_device结构体
	unregister_netdev(xxx_dev);
	// 释放net_device结构体
	free_netdev(xxx_dev);
}
```

### 网络设备初始化模板
```c
void xxx_init(struct net_device *dev)
{
	// 设备的私有信息结构体
	struct xxx_priv *priv;

	/*xxx_hw_init完成与硬件相关的初始操作
	 * 1.探测xxx网络设备是否存在
	 * 2.探测设备的具体硬件配置
	 * 3.申请设备所需要的硬件资源
	 */
	xxx_hw_init();

	// 初始化以太网设备的公用成员
	ether_setup(dev);

	// 设置设备的成员函数指针
	dev->netdev_ops = &xxx_netdev_ops;
	dev->ethtool_ops = &xxx_ethtool_ops;
	dev->watchdog_timeo = timeout;

	// 取得私有信息，并初始化
	priv = netdev_priv(dev);
	...
}
```

### 网络设备打开和释放模板
```c
static int xxx_open(struct net_device *dev)
{
	// 申请端口、IRQ等
	ret = request_irq(dev->irq, &xxx_interrupt, 0, dev->name, dev);
	...
	netif_start_queue(dev);
	...
}

static int xxx_release(struct net_device *dev)
{
	//释放端口、IRQ
	free_irq(dev->irq, dev);
	...
	netif_stop_queue(dev);
	...
}
```

### 网络设备驱动程序的数据包发送模板
```c
int xxx_tx(struct sk_buff *skb, struct net_device *dev)
{
	int len;
	char *data, shortpkt[ETH_ZLEN];
	if(xxx_send_available(...))// 发送队列未满，可以发送
	{
		// 获得有效数据指针和长度
		data = skb->data;
		len = skb->len;
		if(len < ETH_ZLEN){// 如果帧长小于以太网帧最小长度，补0
			memset(shortpkt, 0, ETH_ZLEN);
			memcpy(shortpkt, skb->data, skb->len);
			len = ETH_ZLEN;
			data = shortpkt;

		}
		dev->trans_start = jiffies; // 记录发送时间戳
		if(avail){// 设置硬件寄存器，让硬件把数据包发送出去
			xxx_hw_tx(data, len, dev);
		}else{
			netif_stop_queue(dev);
			...
		}
	}
}
```

### 网络设备驱动程序的数据包发送超时函数模板
> 当数据传输超时时，意味着当前的发送操作失败或硬件已经陷入未知状态，此时，数据包发送超时处理函数xxx_tx_timeout()将被调用。
```c
void xxx_tx_timeout(struct net_device *dev)
{
	...
	netif_wake_queue(dev);// 唤醒上层向下层发数据包,定义于include/linux/detdevice.h
}
```

### 网络设备驱动的中断处理函数模板
```c
static void xxx_interrupt(int irq, void *dev_id)
{
	...
	switch(status &ISQ_EVENT_MASK){
	case ISQ_RECEIVER_EVENT:
		//获取数据包
		xxx_rx(dev);
		break;
		// 其他类型中断
	}
}

static void xxx_rx(struct xxx_device *dev)
{
	...
	length = get_rev_len(...);
	// 分配新的套接字缓冲区
	skb = dev_alloc_skb(length + 2);

	skb_reserve(skb, 2); // 对齐
	skb->dev = dev;

	//读取硬件上接收到的数据
	insw(ioaddr + RX_FRAME_PORT, skb_put(skb, length), length >> 1);
	if(length &1)
		skb->data[length - 1] = inw(ioaddr + RX_FRAME_PORT);

	// 获取上层协议类型
	skb->protocol = eth_type_trans(skb, dev);

	// 把数据包交给上层
	netif_rx(skb);

	//记录接收时间戳
	dev->last_rx = jiffies;
}
```

* 如果是NAPI兼容的设备驱动，则可以通过poll方式接收数据包，这时需要为netif_napi_add()提供xxx_poll()函数
```c
static int xxx_poll(struct napi_struct *napi, int budget)
{
	int npackets = 0;
	struct sk_buff *skb;
	struct xxx_priv *priv = container_of(napi, struct xxx_priv, napi);

	struct xxx_packet *pkt;

	while(npackets < budget && priv->rx_queue){
		// 从队列中取出数据包
		pkt = xxx_dequeue_buf(dev);

		// 接下来的处理和中断触发的数据包接收一致
		skb = dev_alloc_skb(length + 2);
		skb_reserve(skb, 2);
		memcpy(skb_put(skb, pkt->datalen), pkt->data, pkt->datalen);
		skb->dev = dev;
		skb->protocol = eth_type_trans(skb, dev);
		// 调用netif_receive_skb（而不是net_rx）将数据包发送给上层协议
		netif_receive_skb(skb);

		//更新数据统计
		priv->stats.rx_packets++;
		priv->stats.rx_bytes += pkt->datalen;
		xxx_release_buffer(pkt);
		npackets++;
	}
	if(npackets < budget){
		napi_complete(napi);
		xxx_enable_rx_int(...);//再次启动网络设备的接收中断
	}
	return npackets;
}

// 网络设备驱动的poll中断处理
static void xxx_interrupt(int irq, void *dev_id)
{
	switch (status &ISQ_EVENT_MASK){
	case ISQ_RECEIVER_EVENT:
		//获取数据包
		xxx_disable_rx_int(...);//禁止接收中断
		napi_schedule(&priv->napi);
		break;
		...
		// 其他中断
	case ISQ_TRANSMITTER_EVENT:
		dev->stats.tx_packets++; // 数据包发送成功，统计信息tx_packets加1
		netif_wake_queue(dev);// 通知上层协议
		if((status &(TX_OK | TX_LOST_CRS | TX_SQE_ERROR | TX_LATE_COL | TX_16_COL)) != TX_OK){// 读取硬件上的出错标志，根据不同情况，对net_device_stats的不同成员加1
			if((status &TX_OK) == 0)
				dev->stats.tx_errors++;
			if(status &TX_LOST_CRS)
				dev->stats.tx_carrier_errors++;
			if(status &TX_SQE_ERROR)
				dev->stats.tx_heartbeat_errors++;
			if(status &TX_LATE_COL)
				dev->stats.tx_window_errors++;
			if(status &TX_16_COL)
				dev->stats.tx_aborted_errors++;
		}
		break;
	case ISQ_RX_MISS_EVENT:
		dev->stats.rx_missed_errors += (status >> 6);
		break;
	case ISQ_TX_COL_EVENT:
		dev-stats.collisions += (status >> 6);
		break;

	}
}
```
### 定时检查连接状态模板
> 网络适配器电路可以检测出连路上是否有载波，载波反映了网络的连接是否正常。网络设备驱动可以通过函数netif_carrier_on()和off改变设备的连接状态，如果驱动检测到连接状态发生变化，也应该用这两个函数显式的通知内核。
```c
static void xxx_timer(unsigned long data)
{
	struct net_device *dev = (struct net_device*)data;
	u16 link;

	...
	if(!(dev->flags & IFF_UP))
		goto set_timer;

	// 获取物理上的连接状态
	if(link = xxx_chk_link(dev)){// xxx_chk_link用于读取网络适配器硬件的相关寄存器，以获得连接状态
		if(!(dev->flags &IFF_RUNNING)){
			netif_carrier_on(dev);
			dev-flags |= IFF_RUNNING;
			printk(KERN_DEBUG "%s: link up\n", dev->name);
		}
	}else{
		if(dev->flags &IFF_RUNNING){
			netif_carrier_off(dev);
			dev->flags &= ~IFF_RUNNING;
			printk(KERN_DEBUG "%s: link down\n", dev->name);
		}
	}
	set_timer:
	priv->timer.expires = jiffies + 1 * Hz;
	priv->timer.data = (unsigned long)dev;
	priv->timer.function = &xxx_timer;
	add_timer(&priv->timer);
}

// 设备打开时，首次调用定时函数，来获得连接状态信息, 之后由xxx_timer函数本身自己不断反复执行定时器来检查连接状态。
static int xxx_open(struct net_device *dev)
{
	struct xxx_priv *priv = netdev_priv(dev);

	...
	priv->timer.expires = jiffies + 3 * Hz;
	priv->timer.data = (unsigned long)dev;
	priv->timer.function = &xxx_timer;
	add_timer(&priv->timer);
}

```

### 网络设备驱动的get_stats()函数模板
```c
struct net_device_stats *xxx_stats(struct net_device *dev)
{
	...
	return &dev->stats;
}
```
