# --------------------------------------------------------------------------
# Berif: Linux Kernel Module Build Makefile
# 
# --------------------------------------------------------------------------
# Define kernel header that is the same as runing-os
KERNELDIR ?= /lib/modules/$(shell uname -r)/build

# Define the specified Linux kernel source code
#KERNELDIR ?= /home/macro/linux/linux-5.10
PWD := $(shell pwd)

# Define cross-compiler toolchain
# CROSS_COMPILE=arm-linux-guneabi-
# ARCH=arm

# Define target soc arch
obj-m := sdio.o
# --------------------------------------------------------------------------
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

.PHONY: clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean 

# --------------------------------------------------------------------------
