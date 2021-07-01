BASEINCLUDE ?= /home/arthur/work/tools/runqemu/linux-4.0.5
slub-objs := slub_test.o

obj-m := slub.o
all :
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) modules;
clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;
