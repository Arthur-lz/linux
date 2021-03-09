#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/uio.h>
#include <linux/slab.h>
#include <linux/mm.h>

static char buf[] ="来自内核的访问\n";
static char buf1[32];
#define INVALIDFILE "vdb_control.file" 

ssize_t AES_QDMA(const struct iovec *iov, size_t count, volatile unsigned long *asyncd,                                                                                                             
		    const char*key, const size_t keysize);
int __init test_init(void)
{
    struct file *fp;
    mm_segment_t fs;
    loff_t pos;
    int ret;
//    ssize_t *a = kzalloc(sizeof(ssize_t), GFP_KERNEL);
 //   ssize_t b = 0;
   
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
    if(strstr("vdb_control.file", INVALIDFILE))
    printk(">>>ret=%d \n", ret);

/*
    *a = 1;
    AES_QDMA(NULL, 1, a, NULL, 0);
    *a = 0;
    AES_QDMA(NULL, 1, a, NULL, 0);
    kfree(a);
    AES_QDMA(NULL, 1, &b, NULL, 0);
    b = 1;
    AES_QDMA(NULL, 1, &b, NULL, 0);
*/

    return 0;
}
void __exit test_exit(void)
{
    printk("vir driver exit\n");
}

ssize_t AES_QDMA(const struct iovec *iov, size_t count, volatile unsigned long *asyncd,                                                                                                             
		    const char*key, const size_t keysize)
{
	return 0;
	if( !asyncd){
		printk("input param err!\n");
		return -1;
	}
//	printk("%s, i:%d\n", __func__, *asyncd);
	if(iov){
		int ilen = (int)iov->iov_len;
		//printk("%s, data size: %d\n", __func__, ilen);
		if(ilen > 0){
			char *buf = kzalloc(ilen, GFP_KERNEL);
			if(buf){
				if(!copy_from_user(buf, iov->iov_base, ilen)){
					int i;
					for(i = 0;i < ilen;i++){
						if(*asyncd){// en
							if(buf[i] == 'A'){
								buf[i] = 'a';
								break;
							}
						}
						else{
							if(buf[i] == 'a'){
								buf[i] = 'A';
								break;
							}
						}
					}
					if(!copy_to_user(iov->iov_base, buf, ilen))
						goto out;
					else 
						goto out;
				}
out:
				kfree(buf);

			}
		}	
	}
	return 0;

}
EXPORT_SYMBOL(AES_QDMA);

ssize_t AES_QDMAP(const struct page *p, size_t count,                                                                                                              
		    const char*key, const size_t keysize)
{
	if(p){
		char *c = page_address(p);
		int i;
		for(i = 0;i<10;i++){
			if(c[i] == 'a')
				c[i] = 'A';
		}

		printk("%s, data size: %d, encrypt: %s\n", __func__, count, c);
	}
	return 0;
}
EXPORT_SYMBOL(AES_QDMAP);
 
module_init(test_init);
module_exit(test_exit);
 
MODULE_LICENSE("GPL"); 
