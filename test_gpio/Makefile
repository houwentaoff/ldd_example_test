ifneq ($(KERNELRELEASE),)
#proc-fs-objs := main.o
obj-m	:= test_gpio.o
else

KERNELDIR ?= /usr/src/linux-headers-`uname -r`/
PWD       := $(shell pwd)

default:
	$(MAKE)  -C $(KERNELDIR) M=$(PWD) modules INCDIR=$(INCLUDE) modules  
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers


