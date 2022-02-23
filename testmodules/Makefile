ifeq ($(KERNELRELEASE),) 
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
 	M=$(PWD) modules
 	PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules 

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install 

.PHONY: modules modules_install clean
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module* modules* *.mod

else
 obj-m :=kiwi_sdio.o 
endif
