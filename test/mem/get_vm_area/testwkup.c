#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/wait.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>

static unsigned long data = 0;
static int irq = 3;

static char buf[] ="来自内核的访问\n";
static char buf1[32];

struct vm_struct *vm = NULL;// vm_struct 是内核虚拟区间描述符
 

void init_get_vm_area(void)
{
	vm = __get_vm_area(8096*4, VM_ALLOC, 0xc0001000, 0xdd000000);	
	/* __get_vm_area(size, flags, start, end), 查找一块从start开始，end结束的线性地址，并从该地址范围内创建size字节大小的内核虚拟区间.
	 * flags= VM_ALLOC, 表示使用vmalloc()来分配页
	 *
	 * */
	if(!vm){
		printk("get vm area err\n", __func__);
		return;
	}else{
		printk("vm size:%d, addr: 0x%lx\n", vm->size, (unsigned long)vm->addr);// size的值是36684 = 8096*4 + 4096得到的，加上的4096是保护页的大小。
		/* 在调用__get_vm_area分配内核虚拟地址时，内核会在原本大小的基础上再加一个保护页给返回值vm_struct
		 * */
	}
}

int __init test_init(void)
{
    	struct file *fp;
    	mm_segment_t fs;
    	loff_t pos;
    	int ret;
	printk("%s, init pid:%d\n", __func__, current->pid);
    
    	init_get_vm_area();
	return 0;

    	printk("KERNEL_DS=0x%llx USER_DS=0x%llx get_fs()=0x%llx\n", KERNEL_DS, USER_DS, get_fs());
    	fp =filp_open("/home/arthur/work/kernel_file",O_RDWR | O_CREAT,0644);
    	if (IS_ERR(fp)){
    	    printk("create file error\n");
    	    return -1;
    	}
    	fs =get_fs();
    	set_fs(KERNEL_DS);
    	pos =0;
    	printk("fp=%p, buf=%p get_fs()=0x%llx\n", fp, buf, get_fs());
    	ret = vfs_write(fp,buf, sizeof(buf), &pos);
    	printk("ret=%d\n", ret);
    	pos =0;
    	printk("fp=%p, buf1=%p\n", fp, buf1);
    	ret = vfs_read(fp,buf1, sizeof(buf), &pos);
    	printk("ret=%d Write contet=%s\n", ret, buf1);
    	filp_close(fp,NULL);
    	set_fs(fs);
    	return 0;
}
void __exit test_exit(void)
{
	if(vm){
		free_vm_area(vm);
		printk(KERN_INFO "free vm area\n");
	}
    	printk("test exit\n");
}
 
module_init(test_init);
module_exit(test_exit);
 
MODULE_LICENSE("GPL"); 
