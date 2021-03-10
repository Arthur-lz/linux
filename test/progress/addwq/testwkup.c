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

static wait_queue_head_t head;
static struct task_struct *oldpro;

static char buf[] ="来自内核的访问\n";
static char buf1[32];
 
int custom_fun(void *argc)
{
	printk("%s, in the kthread function. pid:%d\n", __func__, current->pid);
}

void init_wakeup(void)
{
	char namefrm1[] = "add_wait_queue1";
	char namefrm2[] = "add_wait_queue2";
	ssize_t time_out;
	struct task_struct *result1, *result2, *result3;
	wait_queue_t data1, data2, data3, *curr, *next;

	result1 = kthread_create_on_node(custom_fun, NULL, -1, namefrm1);
	result2 = kthread_create_on_node(custom_fun, NULL, -1, namefrm2);
	printk("%s, kthread create, pid:%d\n", __func__, result1->pid);
	printk("%s, kthread create, pid:%d\n", __func__, result2->pid);

	init_waitqueue_head(&head);
	init_waitqueue_entry(&data1, result1); //
	add_wait_queue(&head, &data1); // 将进程加到等待队列头；add_wait_queue_exclusive是将进程加到等待队列尾，且设置进程为高优先级
	init_waitqueue_entry(&data2, result2);
	add_wait_queue(&head, &data2);

	printk("before wakeup\n");
	list_for_each_entry_safe(curr, next, &(head.task_list), task_list){
		printk("@@@@@pid: %d, state: %d\n", 
				((struct task_struct *)(curr->private))->pid,
				((struct task_struct *)(curr->private))->state
				);

	}
	__wake_up(&head, TASK_ALL, 0, NULL);
	printk("after wakeup\n");

	list_for_each_entry_safe(curr, next, &(head.task_list), task_list){
		printk("@@@@@pid: %d, state: %d\n", 
				((struct task_struct *)(curr->private))->pid,
				((struct task_struct *)(curr->private))->state
				);

	}
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
