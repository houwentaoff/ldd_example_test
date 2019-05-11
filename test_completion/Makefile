ifneq ($(KERNELRELEASE),)
obj-m	:= test_completion.o
else
KERNELDIR     ?= /home/work/worke/github/buildroot-2018.02.2/output/build/linux-4.15
#INCLUDE       := $(KERNELDIR)/drivers/usb/gadget
PWD       	  := $(shell pwd)
#ARCH          := arm
#CROSS_COMPILE :=arm-linux- #/opt/Embedsky/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/tq-linaro-toolchain/bin/arm-linux-
CROSS_COMPILE := /home/work/worke/github/buildroot-2018.02.2/output/host/bin/x86_64-buildroot-linux-gnu-  
default:
	$(MAKE)  -C $(KERNELDIR)  CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) INCDIR=$(INCLUDE) modules  
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers


