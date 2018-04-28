ifneq ($(KERNELRELEASE),)
obj-m := mirror.o
else

KERNELDIR     ?= /work/imx6/linux
ifndef KERNELDIR
	KERNELDIR=/lib/modules/$(shell uname -r)/build
endif

PWD       	  := $(shell pwd)
ARCH          := arm
CROSS_COMPILE := /work/imx6/fsl-linaro-toolchain-master/bin/arm-fsl-linux-gnueabi-

default:
	$(MAKE)  -C $(KERNELDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) INCDIR=$(INCLUDE) modules  
endif

clean:
	rm -rf *.o *.ko .tmp* .*cmd *.mod.c *.symvers
