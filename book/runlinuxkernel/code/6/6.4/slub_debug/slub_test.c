#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

static char *buf;

static void create_slub_error(void)
{
	buf = kmalloc(32, GFP_KERNEL);
	if (buf)
		memset(buf, 0x55, 36);
}

static void create_slub_error2(void)
{
	buf = kmalloc(32, GFP_KERNEL);
	if (buf) {
		kfree(buf);
		printk("access free memory\n");
		memset(buf, 0x55, 36);
	
	}
}

static void create_slub_error3(void)
{
	buf = kmalloc(32, GFP_KERNEL); // 申请32bytes, slab会分配64字节，
	if (buf) {
		printk("access out memory\n");
		// BUG kmalloc-64 (Tainted: G           O   ): Redzone overwritten
		memset(buf, 0x88, 65);// 这里写了６５个字节，正好到Redzone的第一个字节
		// BUG kmalloc-64 (Tainted: G    B      O   ): Object padding overwritten
		buf[-1] = 0x88; // 这是另一个异常, 会写到Padding中; 
		kfree(buf);

		printk("access free memory\n");
		// BUG kmalloc-64 (Tainted: G    B      O   ): Object already free
		memset(buf, 0x99, 16);// 访问已经被释放的内存
	//	printk("double free memory\n");
		// BUG kmalloc-64 (Tainted: G    B      O   ): Object already free
	//	kfree(buf); // 重复释放内存
	}
}

static void create_kmemleak(void)
{
	buf = kmalloc(120, GFP_KERNEL);
	buf = vmalloc(4096);
}

static int __init my_test_init(void)
{
	printk("figo: my module init\n");
	//create_slub_error3();
	create_kmemleak();
	return 0;
}

static void __exit my_test_exit(void)
{
	printk("bye\n");
	kfree(buf);
}

MODULE_LICENSE("GPL");
module_init(my_test_init);
module_exit(my_test_exit);
