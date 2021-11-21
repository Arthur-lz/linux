#include <linux/security.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/uaccess.h>


static int __init test_init(void);
static void __exit test_exit(void);

void init_get_vm_area(void)
{
	struct mm_struct *mm;
	unsigned long addr;
	struct vm_area_struct *vma;
	struct msg_msg *msg;
	char *a, *b;
	printk("before kmalloc, msg %p, a %p,\n &msg %lx,\n &a %lx, \n unsigned long msg %lx,\n (unsigned long)a %lx, \n(u l) b %lx,\n (u l)&msg %lx \n", 
			msg, a, &msg, &a, 
			(unsigned long)msg,
			(unsigned long)a,
			(unsigned long)b, 
			(unsigned long)&msg

			);

	a = kmalloc(32, GFP_KERNEL);
	b = kmalloc(32, GFP_KERNEL);
	a = "abc";
	printk("a pointer is %p, value is %s, &a %lx, \n (unsigned long)a %lx, \n (ul) b %lx\n", 
			a, a, &a, 
			(unsigned long)a,
			(unsigned long)b
			);
	msg = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (msg) {
		printk("msg pointer is %p, &msg is %lx, \n unsigned long msg %lx\n, (ul) &msg %lx", 
				msg, &msg, 
				(unsigned long)msg,
				(unsigned long)&msg
				);
		kfree(msg);
	}

	mm = current->mm;
	addr = mm->mmap->vm_next->vm_start + 1;

	printk("where is dest 0x%lx\n", addr);
	vma = find_vma(mm, addr);

	if(vma){
		printk("vma start:0x%lx, end:0x%lx\n", vma->vm_start, vma->vm_end);
	}
}

int __init test_init(void)
{
    	init_get_vm_area();
	return 0;
}
void __exit test_exit(void)
{
    	printk("test exit\n");
}
 
module_init(test_init);
module_exit(test_exit);
 
MODULE_LICENSE("GPL"); 
