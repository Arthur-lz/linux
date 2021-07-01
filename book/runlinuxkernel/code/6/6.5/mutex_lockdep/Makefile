BASEINCLUDE ?= /home/arthur/work/tools/runqemu/linux-4.0.5
mutexlk-objs := mutex_lockdep.o

obj-m := mutexlk.o
all :
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) modules;
clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;
