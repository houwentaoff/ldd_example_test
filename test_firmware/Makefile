ifneq ($(KERNELRELEASE),)
obj-m	:= test_fw.o
else

KERNELDIR ?= /home/work/worke/github/buildroot-2018.02.2/output/build/linux-4.15 #/usr/src/linux-headers-`uname -r`/
PWD       := $(shell pwd)
CROSS_COMPILE := /home/work/worke/github/buildroot-2018.02.2/output/host/bin/x86_64-buildroot-linux-gnu-
default:
	$(MAKE)  -C $(KERNELDIR) CROSS_COMPILE=$(CROSS_COMPILE)  M=$(PWD) modules  
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers


