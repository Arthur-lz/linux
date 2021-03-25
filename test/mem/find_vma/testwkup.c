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
