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

static char buf[] ="来自内核的访问\n";
static char buf1[32];
 

static void irq_tasklet_action(unsigned long data)
{
	printk("%s, tasklet state is:%ld \n", __func__, tsklet.state);
	/* 说一下tasklet_struct成员变量state，其最后两位bit[1], bit[0]的含义：
	 * bit[1] == 1表示，当前tasklet正在执行, 只对SMP多核有效，其作用是防止多个cpu同时执行同一个tasklet
	 * bit[0] == 1表示，表示当前tasklet已经被调度，等待获得CPU资源执行
	 * */
}

static void irq_tasklet_action_hi(unsigned long data)
{
	printk("%s, tasklet1 state is:%ld \n", __func__, tsklet1.state);
}

void init_tsklet(void)
{
	char namefrm[] = "__wakeup.c%s";

	printk("%s, \n", __func__);
	tasklet_init(&tsklet, irq_tasklet_action, data);
	tasklet_schedule(&tsklet); //先设置tasklet.state的bit[0]=1，之后将软中断tsklet加到tasklet_vec链表尾

	printk(KERN_INFO "before disable, tsklet.count:%d\n", tsklet.count);
	tasklet_disable(&tsklet);// tasklet_disable使tasklet.count增加1，这会导致软中断进入睡眠状态，不会被调度，进而也不会得到CPU资源执行。
	if(atomic_read(&(tsklet.count)) != 0)
		printk("tsklet disable\n");
	printk(KERN_INFO "after disable, tsklet.count:%d\n", tsklet.count);

	tasklet_enable(&tsklet);// tasklet_disable使tasklet.count减1，如果count == 0，会唤醒软中断，可以被调度执行。
	if(atomic_read(&(tsklet.count)) == 0)
		printk("tsklet enable\n");
	printk(KERN_INFO "after enable, tsklet.count:%d\n", tsklet.count);

	tasklet_kill(&tsklet);
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
    printk("test exit\n");
}
 
module_init(test_init);
module_exit(test_exit);
 
MODULE_LICENSE("GPL"); 
