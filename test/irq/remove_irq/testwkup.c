#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/wait.h>
#include <linux/list.h>
#include <linux/delay.h>

#include <linux/interrupt.h>

static struct tasklet_struct tsklet, tsklet1;
static unsigned long data = 0;
static int irq = 10;

static char buf[] ="来自内核的访问\n";
static char buf1[32];
 

static irqreturn_t irq_handler(int data, void * dev)
{
	printk("%s \n", __func__);
	return IRQ_NONE;
}


static struct irqaction act = {
	.irq = 10,
	.handler = irq_handler,
	.flags= IRQF_DISABLED,
	.name = "NEW_DEV_A",
	.dev_id = NULL
};

void init_tsklet(void)
{
	/* request_irq, 申请一个软中断，中断号是3, 中断类型是IRQF_DISABLED, 中断名称是NEW_DEV_A, 不对应具体设备
	 * 中断处理函数是irq_handler
	 * 中断号是全局数组irq_desc中元素的下标，此数组的大小是16640, 书上这么写的；实际查代码是下面这样子的：
	 * 3.10.0的内核中，中断号是保存在红黑树irq_desc_tree中, 其在内核源码kernel/irq/irqdesc.c 中定义
	 * */
	int r,t = 1;
	struct irqaction *a = &act;
	int ret = request_irq(a->irq, a->handler,a->flags, a->name, a->dev_id);
	int ret1 = irq_set_chip(irq, NULL);

	//r = irq_set_chip_data(irq, &cd);
	r = irq_set_irq_type(irq, t);
	printk("%s, ret:%d, ret1:%d, r:%d \n", __func__, ret, ret1, r);
	//disable_irq_nosync使中断的深度加1 
	//disable_irq_nosync(irq);
	//disable_irq(irq);
	// enable_irq使中断的深度减1，同时触发中断处理函数irq_handler执行
	//enable_irq(irq);
	printk("%s\n", __func__);

}

int __init test_init(void)
{
    	struct file *fp;
    	mm_segment_t fs;
    	loff_t pos;
    	int ret;
	printk("%s, init pid:%d\n", __func__, current->pid);
    
    	init_tsklet();
	return 0;

    	printk("KERNEL_DS=0x%llx USER_DS=0x%llx get_fs()=0x%llx\n", KERNEL_DS, USER_DS, get_fs());
    	fp =filp_open("/home/arthur/work/kernel_file",O_RDWR | O_CREAT,0644);
    	if (IS_ERR(fp)){
    	    printk("create file error\n");
    	    return -1;
    	}
    	fs =get_fs();
    	set_fs(KERNEL_DS);
    	pos =0;
    	printk("fp=%p, buf=%p get_fs()=0x%llx\n", fp, buf, get_fs());
    	ret = vfs_write(fp,buf, sizeof(buf), &pos);
    	printk("ret=%d\n", ret);
    	pos =0;
    	printk("fp=%p, buf1=%p\n", fp, buf1);
    	ret = vfs_read(fp,buf1, sizeof(buf), &pos);
    	printk("ret=%d Write contet=%s\n", ret, buf1);
    	filp_close(fp,NULL);
    	set_fs(fs);
    	return 0;
}
void __exit test_exit(void)
{
	remove_irq(irq, &act);
    	printk("test exit\n");
}
 
module_init(test_init);
module_exit(test_exit);
 
MODULE_LICENSE("GPL"); 
