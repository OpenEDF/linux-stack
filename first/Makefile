# --------------------------------------------------------------------------
# Berif: Linux Kernel Module Build Makefile
# 
# --------------------------------------------------------------------------
#KERNELDIR ?= /lib/modules/$(shell uname -r)/build
KERNELDIR ?= /home/macro/linux/linux-5.10
PWD := $(shell pwd)

# --------------------------------------------------------------------------
build:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean 

.PHONY: build clean

# --------------------------------------------------------------------------
