ifneq ($(KERNELRELEASE),)
#proc-fs-objs := main.o
obj-m	:= test_key.o
else

KERNELDIR ?= /home/work/workf/tq/kernel_imx/ #/usr/src/linux-headers-`uname -r`/
PWD       := $(shell pwd)

ARCH          := arm
CROSS_COMPILE := arm-linux- #/opt/Embedsky/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/tq-linaro-toolchain/bin/arm-linux-

default:
	$(MAKE)  -C $(KERNELDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) INCDIR=$(INCLUDE) modules  
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers


