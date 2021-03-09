#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

static char buf[] ="来自内核的访问\n";
static char buf1[32];
 
int __init test_init(void)
{
    struct file *fp;
    mm_segment_t fs;
    loff_t pos;
    int ret;
    
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
    printk("test exit\n");
}
 
module_init(test_init);
module_exit(test_exit);
 
MODULE_LICENSE("GPL"); 
