BASEINCLUDE ?= /home/arthur/work/tools/runqemu/linux-4.0.5
oopstest-objs := oops_test.o

obj-m := oopstest.o
all :
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) modules;
clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;
