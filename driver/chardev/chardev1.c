#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>

dev_t dev_no;

static int __init cdev_test_init(void)
{
	int ret; 
	printk("Hello kernel for cdev!\n");
	dev_no = MKDEV(222,2);
	ret = register_chrdev_region(dev_no, 1, "lz_dev");

	return ret;
}

static int __exit cdev_test_exit(void)
{
	unregister_chrdev_region(dev_no, 1);
	return 0;
}

module_init(cdev_test_init);
module_exit(cdev_test_exit);

MODULE_LICENSE("GPL");

