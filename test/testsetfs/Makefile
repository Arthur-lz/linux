obj-m :=testsetfs.o                        #要生成的模块名     
testsetfsm-objs:= testsetfs.o     #生成这个模块名所需要的目标文件

KDIR := /lib/modules/3.10.0-serica-001.x86_64/build
PWD := $(shell pwd)

default:  
	make -C $(KDIR) M=$(PWD) modules

clean:
	rm -rf .* *.o *.cmd *.ko *.mod.c .tmp_versions Module.symvers modules.order 
