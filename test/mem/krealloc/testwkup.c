#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>

struct page *page_g = NULL;
#define new_size 26 
#define alloc_page_num 0

static int __init test_init(void);
static void __exit test_exit(void);

void init_get_vm_area(void)
{
	page_g = alloc_pages(GFP_KERNEL, alloc_page_num);
	if(!page_g){
		printk("alloc pages err\n");
		return;
	}else{
		char * c = (char*)page_g;
		int i;
		char x;
		*c = 'a';
		for(i = 0;i<new_size;i++){
			x = *c;
			c++;
			*c = x + 1;
			printk("%c", *c);
		}
		printk("page addr: %p\n", page_g);
		return ;
		char * addr = __krealloc(page_g, new_size, GFP_KERNEL);
		for(i=0; i < 26; i++, addr++){
			printk("%c", *addr);
		}

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
		__free_pages(page_g, alloc_page_num);
		printk(KERN_INFO "free vm area\n");
	}
    	printk("test exit\n");
}
 
module_init(test_init);
module_exit(test_exit);
 
MODULE_LICENSE("GPL"); 
