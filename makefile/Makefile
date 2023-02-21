KERNELDIR := /home/macro/imx6ull_mini/linux-imx-rel_imx_4.1.15_2.1.0_ga_alientek
CURRENT_PATH := $(shell pwd)
ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-
obj-m := chrdevbase.o
KO_FILE = chrdevbase.ko

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
send:
	scp $(KO_FILE) root@192.168.2.100:macro_drv/ 
