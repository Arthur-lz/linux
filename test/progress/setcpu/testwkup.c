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
	int *g = (int*)argc;
	cpumask_var_t mask;
	alloc_cpumask_var(&mask, GFP_KERNEL);
	cpumask_set_cpu((*g), mask);
	printk("in son thread proc\n");
	if(argc&&oldpro)
	{
		int a= -1;
		a = set_cpus_allowed_ptr(oldpro, mask);
		printk("set cpu ret:%d\n", a);
	}
	else{
		// complete_all，可以唤醒等待队列中的所有睡眠的进程，但也是一个一个的唤醒。
		complete(&g_compl);//	一个一个的唤醒等待队列中的睡眠的进程，如果唤醒的时遇到高优先级进程，则在唤醒后，不再继续唤醒其他进程。
		do_exit(0);// 结束当前进程
		printk("complete, done:%d \n", g_compl.done);
	}
//	printk("%s, after wkup oldpro, current thread state:%ld, old thread state:%ld\n", __func__, current->state, oldpro->state);
}

void init_wakeup(void)
{
	ssize_t time_out;
	struct task_struct *result;

	wait_queue_t data;
	int i;
	struct thread_info *ti = current_thread_info();
	if(ti->cpu < 11)
		i = ti->cpu + 1;
	else
		i = ti->cpu - 1;
	result = kthread_create_on_node(custom_fun, &i, -1, "set_cpu_allowed_ptr");
	printk("%s, kthread create, pid:%d, old proc cpu is:%d\n", __func__, result->pid, ti->cpu);

	oldpro = current;
	wake_up_process(result);
	time_out = schedule_timeout_uninterruptible(100);
	printk("%s,old proc cpu is:%d\n", __func__, ti->cpu);
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
