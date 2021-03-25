#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>

void *page_g = NULL;
#define new_size 26 
#define alloc_page_num 8192

static int __init test_init(void);
static void __exit test_exit(void);

void init_get_vm_area(void)
{
	page_g = alloc_pages_exact(alloc_page_num, GFP_KERNEL); //申请的大小是字节为单位， 精确的申请内存大小，只申请满足条件的最小连续页面
	if(!page_g){
		printk("alloc pages err\n");
		return;
	}else{
		char * c = (char*)page_g;
		int i;
		char x;
		*c = 'a';
		for(i = 0;i<new_size;i++){
			printk("%c", *c);
			x = *c;
			c++;
			*c = x + 1;
		}
		printk("\npage addr: %p\n", page_g);
	}
}

int __init test_init(void)
{
    	init_get_vm_area();
	return 0;
}
void __exit test_exit(void)
{
	if(page_g){
		free_pages_exact(page_g, alloc_page_num);
		printk(KERN_INFO "free vm area\n");
	}
    	printk("test exit\n");
}
 
module_init(test_init);
module_exit(test_exit);
 
MODULE_LICENSE("GPL"); 
