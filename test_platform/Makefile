ifneq ($(KERNELRELEASE),)
#proc-fs-objs := main.o
obj-m	:= test_platform1.o
else
ifeq ($(ARCH), )
KERNELDIR ?= /usr/src/linux-headers-`uname -r`/
else
KERNELDIR ?= /home/work/worke/github/buildroot-2018.02.2/output/build/linux-4.15/  #/usr/src/linux-headers-`uname -r`/
endif
PWD       := $(shell pwd)

default:
	$(MAKE)  -C $(KERNELDIR) M=$(PWD) modules INCDIR=$(INCLUDE) modules  
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers


