#include <linux/kernel.h>
#include <linux/module.h>

static int __init helloinit(void)
{
	printf("hello, kernel , lz in\n");
	return 0;
}

static void __exit helloexit(void)
{
	printk("hello, kernel, good by.\n");
}

module_init(helloinit);
module_exit(helloexit);

MODULE_LICENSE("GPL");
