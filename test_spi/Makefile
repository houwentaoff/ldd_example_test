ifneq ($(KERNELRELEASE),)
obj-m	:= test_spiapb.o

test_spiapb-objs := spi2apb.o test_spi_drv.o
else
KERNELDIR     ?= /usr/src/linux-headers-`uname -r`#/work/32work/B9161/Source/Kernel/linux-3.10.x #/home/other/tq/kernel_imx/
#INCLUDE       := $(KERNELDIR)/drivers/usb/gadget
PWD       	  := $(shell pwd)
ARCH          := arm
CROSS_COMPILE := arm-linux- #/opt/Embedsky/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/tq-linaro-toolchain/bin/arm-linux-
#EXTRA_CFLAGS += -I$(INClUDE)
#export EXTRA_CFLAGS
default:
	#x86
	$(MAKE)  -C $(KERNELDIR) M=$(PWD) modules INCDIR=$(INCLUDE) modules 
	#arm other
#	$(MAKE)  -C $(KERNELDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) INCDIR=$(INCLUDE) modules  
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers *.a *.mod


