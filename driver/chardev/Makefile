BASELINUX ?= ~/linux/source/linux-4.0.1/
test-objs := chardev2.o

obj-m := testchrdev.o
all :
	$(MAKE) -C $(BASELINUX) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASELINUX) SUBDIRS=$(PWD) clean;
	RM *.ko;
