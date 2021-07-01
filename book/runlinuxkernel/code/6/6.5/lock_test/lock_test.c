#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

static DEFINE_SPINLOCK(hack_spinA);
static DEFINE_SPINLOCK(hack_spinB);

void hack_spinAB(void)
{
	printk("hack_lockdep: A->B\n");
	spin_lock(&hack_spinA);
	spin_lock(&hack_spinB);
}

void hack_spinBA(void)
{
	printk("hack_lockdep: B->A\n");
	spin_lock(&hack_spinB);
}

static int __init my_test_init(void)
{
	printk("locked module init\n");
	hack_spinAB();
	hack_spinBA();
	return 0;
}

static void __exit my_test_exit(void)
{
	printk("bye\n");
}

MODULE_LICENSE("GPL");
module_init(my_test_init);
module_exit(my_test_exit);
