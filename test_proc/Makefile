ifneq ($(KERNELRELEASE),)
obj-m	:= proc_test01.o
else

KERNELDIR ?= /usr/src/linux-headers-`uname -r`/
PWD       := $(shell pwd)

default:
	$(MAKE)  -C $(KERNELDIR) M=$(PWD) modules INCDIR=$(INCLUDE) modules  
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.symvers modules.order
