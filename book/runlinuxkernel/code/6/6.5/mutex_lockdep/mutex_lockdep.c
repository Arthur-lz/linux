#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/mutex.h>
#include <linux/delay.h>

static DEFINE_MUTEX(mutex_a);
static struct delayed_work delay_task;
static void lockdep_timefunc(unsigned long);
static DEFINE_TIMER(lockdep_timer, lockdep_timefunc, 0, 0);

static void lockdep_timefunc(unsigned long dummy)
{
	schedule_delayed_work(&delay_task, 10);
	mod_timer(&lockdep_timer, jiffies + msecs_to_jiffies(100));
}

static void lockdep_test_worker(struct work_struct *work)
{
	mutex_lock(&mutex_a);	
	mdelay(300);
	mutex_unlock(&mutex_a);
}

static int lockdep_thread(void *nothing)
{
	set_freezable();
	set_user_nice(current, 0);

	while (!kthread_should_stop()) {
		mdelay(500);

		mutex_lock(&mutex_a);
		cancel_delayed_work_sync(&delay_task);// 取消worker, 该函数会调用flush操作和等待所有的worker回调函数执行完成，然后才返回, 那么问题来了，在worker的回调函数lockdep_test_worker中第一句就是获取mutex，而它已经在lockdep_thread中取得了，这就产生了死锁, 而且它还在等待worker的回调函数完成后从cancel_delayed_work_sync返回来释放mutex，但永远也等不到
		mutex_unlock(&mutex_a);
	}
	return 0;
}

static int __init lockdep_test_init(void)
{
	struct task_struct *lock_thread;
	printk("lockdep module init\n");

	lock_thread = kthread_run(lockdep_thread, NULL, "lockdep_test");

	INIT_DELAYED_WORK(&delay_task, lockdep_test_worker);

	lockdep_timer.expires = jiffies + msecs_to_jiffies(500);
	add_timer(&lockdep_timer);
	return 0;
}

static void __exit lockdep_test_exit(void)
{
	printk("bye\n");
}

MODULE_LICENSE("GPL");
module_init(lockdep_test_init);
module_exit(lockdep_test_exit);

