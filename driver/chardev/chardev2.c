#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define test_NAME "chr_test_dev"
static dev_t dev;
static struct cdev *test_cdev;
static signed count = 1;

struct file_operations testops = {
	.owner = THIS_MODULE,
	.open = test_open,
	.release = test_release,
	.read = test_read,
	.write = test_write
};

static int test_open(struct inode *inode, struct file *file)
{
	int major = MAJOR(inode->i_rdev);
	int minor = MINOR(inode->i_rdev);

	printk("%s: major=%d, minor=%d\n", __func__, major, minor);

	return 0;
}

static int test_release(struct inode *indoe, struct file *file)
{
	return 0;
}

static ssize_t test_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	printk("%s enter\n", __func__);
	return 0;
}

static ssize_t test_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)
{
	printk("%s enter \n", __func__);
	return 0;
}



int cdev_test_setup(void)
{
	cdev_init();
	cdev_add();
}
/*dev_t用于定义设备号
 *长度32位，12为主设备号，20位为次设备号
 *使用宏MKDEV(int major, int minor)生成设备号，即设备号的初始化
 *使用宏MAJOR(dev_t dev)读出主设备号
 *使用宏MINOR(dev_t dev)读出次设备号
 */
dev_t dev_no;

static int __init cdev_test_init(void)
{
	int ret; 
	printk("Hello kernel for cdev!\n");
	//alloc_chrdev_region是系统自动分配主设备号，申请成功后会将设备号存到dev_no中
	ret = alloc_chrdev_region($dev_no, 1,1,"lz_dev");
	if (ret < 0) {
		printk("failed to allocate char device region");
		return ret;
	}

	test_cdev = cdev_alloc();
	if(!test_cdev){
		printk("cdev_alloc failed\n");
		goto unregister_chrdev;
	}

	ret = cdev_add(test_cdev, dev, count);
	if (ret) {
		printk("cdev_add failed\n");
		goto cdev_fail;
		
	}

	printk("succeeded register char device: %s\n", test_NAME);
	printk("Major = %d, Minor = %d\n", MAJOR(dev), MINOR(dev));

	return 0;

cdev_fail:
	cdev_del(test_cdev);
unregister_chrdev:
	unregister_chrdev_region(dev,count);

	return ret;
}

static void __exit cdev_test_exit(void)
{
	printk("now removing device\n");
	if(test_cdev)
		cdev_del(test_cdev);

	unregister_chrdev_region(dev, count);
	return 0;
}

module_init(cdev_test_init);
module_exit(cdev_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("arthur");
MODULE_DESCRIPTION("learn it again.");

