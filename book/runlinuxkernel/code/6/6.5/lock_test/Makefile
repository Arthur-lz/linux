BASEINCLUDE ?= /home/arthur/work/tools/runqemu/linux-4.0.5
spinlk-objs := lock_test.o

obj-m := spinlk.o
all :
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) modules;
clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;
