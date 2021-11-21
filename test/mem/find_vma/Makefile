obj-m := testwk.o                        #要生成的模块名     

KDIR := /lib/modules/`uname -r`/build
PWD := $(shell pwd)

default:  
	make -C $(KDIR) M=$(PWD) modules

clean:
	rm -rf .* *.o *.cmd *.ko *.mod.c .tmp_versions Module.symvers modules.order 
