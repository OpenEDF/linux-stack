# This makefile for CH36x chips of WCH crop.

#Makefile for linux 2.6.x
ifneq ($(KERNELRELEASE), )
#call from kernel build system
obj-m := ch36x.o
else
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)
clean:
	rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions modules.order Module.symvers Module.markers built-in.o ch36x.ko *.ko.*
endif
