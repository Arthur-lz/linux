#include <linux/security.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define AREA_NR 4096*30
static int __init test_init(void);
static void __exit test_exit(void);

void init_get_vm_area(void)
{
	struct mm_struct *mm;
	unsigned long addr_start, addr_end;
	struct vm_area_struct *vma;
	unsigned long mm_start;

	mm = current->mm;
	mm_start = mm->mmap->vm_start; // 当前进程起始地址
	addr_start = mm_start + 1;
	addr_end = mm_start + 10;

	vma = find_vma_intersection(mm, addr_start, addr_end); //返回进程的第一个虚拟地址空间

	if(vma){
		printk("progress start:0x%lx, end:0x%lx\n", vma->vm_start, vma->vm_end);
	}
	addr_start = mm_start + AREA_NR;
	addr_end = mm_start + AREA_NR + 10;

	vma = find_vma_intersection(mm, addr_start, addr_end);

	if(vma){
		printk("progress start:0x%lx, end:0x%lx\n", vma->vm_start, vma->vm_end);
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
