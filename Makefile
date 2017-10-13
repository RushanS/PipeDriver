ifneq ($(KERNELRELEASE),)
obj-m := mypipe.o
else
KDIR := /opt/newkernel/linux

all:
	$(MAKE) -C $(KDIR) M=$$PWD
endif
