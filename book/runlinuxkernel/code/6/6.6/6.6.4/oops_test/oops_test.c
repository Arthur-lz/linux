#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

static DEFINE_SPINLOCK(hack_spinA);
static DEFINE_SPINLOCK(hack_spinB);

static void create_oops(void)
{
	*(int*)0 = 0;
}

static int __init my_test_init(void)
{
	printk("oops module init\n");
	create_oops();
	return 0;
}

static void __exit my_test_exit(void)
{
	printk("bye\n");
}

MODULE_LICENSE("GPL");
module_init(my_test_init);
module_exit(my_test_exit);
