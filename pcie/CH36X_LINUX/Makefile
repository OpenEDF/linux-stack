# This makefile for CH36x chips of WCH crop.

#Makefile for linux 2.6.x
all:	modules

install: modules
	mkdir -p /lib/modules/$(shell uname -r)/kernel/drivers/char/
	cp -f ./ch36x.ko /lib/modules/$(shell uname -r)/kernel/drivers/char/
	mkdir -p /lib/modules/$(shell uname -r)/misc/
	cp -f ./ch36x.ko /lib/modules/$(shell uname -r)/misc/
	depmod -a
	
	
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
