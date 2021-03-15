#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/wait.h>
#include <linux/list.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/completion.h>

static wait_queue_head_t head;
static struct task_struct *oldpro;
static struct completion g_compl;

static char buf[] ="来自内核的访问\n";
static char buf1[32];
 
void son_fun(void)
{
	volatile size_t a = 0;
	atomic_t b;
	printk("%s, in the kthread function. pid:%d, ap:%p, ab:%p\n", __func__, current->pid, &a, &b);
}

int custom_fun(void *argc)
{
	if(argc)
	{
		printk("wake up main proc\n");
		wake_up_process((struct task_struct*)argc);
	}
	else{
		int ret;
		//complete(&g_compl);//	一个一个的唤醒等待队列中的睡眠的进程，如果唤醒的时遇到高优先级进程，则在唤醒后，不再继续唤醒其他进程。
		// complete_all，可以唤醒等待队列中的所有睡眠的进程，但也是一个一个的唤醒。
		ret = completion_done(&g_compl); // 返回1表示等待队列里没有进程在等待唤醒，为0表示有。
		printk("complete, done:%d, c_done:%d \n", g_compl.done, ret);
		complete(&g_compl);//	一个一个的唤醒等待队列中的睡眠的进程，如果唤醒的时遇到高优先级进程，则在唤醒后，不再继续唤醒其他进程。
	}
//	printk("%s, after wkup oldpro, current thread state:%ld, old thread state:%ld\n", __func__, current->state, oldpro->state);
}

void init_wakeup(void)
{
	char namefrm[] = "__wakeup.c%s";
	ssize_t time_out;
	struct task_struct *result, *result1;

	wait_queue_t data;
	result = kthread_create_on_node(custom_fun, NULL, -1, namefrm);
	printk("%s, kthread create, pid:%d\n", __func__, result->pid);
	wake_up_process(result);

	init_completion(&g_compl);
	init_waitqueue_entry(&data, result); // 一会要在新创建的子线程里唤醒当前的进程
	add_wait_queue(&g_compl.wait, &data);
	wait_for_completion(&g_compl);

	printk("%s, wake_up new kthread\n", __func__);
}

int __init test_init(void)
{
    	struct file *fp;
    	mm_segment_t fs;
    	loff_t pos;
    	int ret;
	printk("%s, init pid:%d\n", __func__, current->pid);
    
    	init_wakeup();
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
