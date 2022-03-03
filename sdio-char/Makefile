# --------------------------------------------------------------------------
# Berif: Linux Kernel Module Build Makefile
# 
# --------------------------------------------------------------------------
# Define kernel header that is the same as runing-os
KERNELDIR ?= /lib/modules/$(shell uname -r)/build

# Define the specified Linux kernel source code
#KERNELDIR ?= /home/macro/linux/linux-5.10
PWD := $(shell pwd)
APP := test
GCC = gcc

# Define cross-compiler toolchain
# CROSS_COMPILE=arm-linux-guneabi-
# ARCH=arm

# Define target soc arch
obj-m := driver/sdio.o

# App src file
app_src := app/main.c

# --------------------------------------------------------------------------
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

app:
	$(GCC) $(app_src) -o $(APP)

.PHONY: clean app
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean 
	rm -rf test
# --------------------------------------------------------------------------
