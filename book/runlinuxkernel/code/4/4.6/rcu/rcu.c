#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/rcupdate.h>
#include <linux/kthread.h>
#include <linux/delay.h>

struct foo {
	int a;
	struct rcu_head rcu;
};

static struct foo *g_ptr;
static char bye = 0;

static void myrcu_reader_thread(void *data)  // 读者线程
{
	struct foo *p = NULL;
	while (!bye) {
		msleep(200);
		rcu_read_lock();
		p = rcu_dereference(g_ptr);
		if (p)
			printk("%s: read a=%d\n", __func__, p->a);
		rcu_read_unlock();
	}

}

static void myrcu_del(struct rcu_head *rh)
{
	struct foo *p = container_of(rh, struct foo, rcu);
	printk("%s: a=%d\n", __func__, p->a);
	kfree(p);
}

static void myrcu_writer_thread(void *p)
{
	struct foo *new, *old;
	int val = (unsigned long)p;

	while (!bye) {
		msleep(400);
		struct foo *new_ptr = kmalloc(sizeof (struct foo), GFP_KERNEL);
		old = g_ptr;
		printk("%s: write to new %d\n", __func__, val);
		*new_ptr = *old;
		new_ptr->a = val;
		rcu_assign_pointer(g_ptr, new_ptr);
		call_rcu(&old->rcu, myrcu_del);
		val++;
	}
}

static int __init my_test_init(void)
{
	struct task_struct *reader_thread, *writer_thread;
	int val = 5;
	
	printk("module init\n");
	g_ptr = kzalloc(sizeof (struct foo), GFP_KERNEL);

	reader_thread = kthread_run(myrcu_reader_thread, NULL, "rcu_reader");
	writer_thread = kthread_run(myrcu_writer_thread, NULL, "rcu_writer");
	return 0;
}

static void __exit my_test_exit(void)
{
	bye = 1;
	msleep(800); // 等待读写线程结束, 否则释放全局g_ptr后，可能读线程还要读存储在全局变量中保存的数据，而此时全局变量已经被释放，其物理地址已经被回收
	printk("bye\n");
	if (g_ptr)
		kfree(g_ptr);
}

MODULE_LICENSE("GPL");
module_init(my_test_init);
module_exit(my_test_exit);
