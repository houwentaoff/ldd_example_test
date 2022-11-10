# android环境 
## android helloworld app (c)

* 在frameworks下新建测试helloo目录
* `cd android/frameworks/helloo`
``` Android.mk 
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := hello1

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := hello.c

LOCAL_SHARED_LIBRARIES := \

LOCAL_C_INCLUDES := 

LOCAL_CFLAGS += -Werror -Wall
LOCAL_CLANG := true

include $(BUILD_EXECUTABLE)

```

* 使用mmm进行编译,如下。可在out/target/product/kona/system/bin/hello1 获取到编译后的hello1文件
```
mmm frameworks/helloo
/LINUX/android/frameworks/helloo
Restriction Checker not present, skipping..
device/qcom/kona/kona.mk:4: warning: TEMPORARY_DISABLE_PATH_RESTRICTIONS: export has been deprecated. It is a glob
hardware/qcom/media/conf_files/kona/kona.mk:28: warning: "Enabling codec2.0 SW only for non-generic odm build vari
============================================
PLATFORM_VERSION_CODENAME=REL
PLATFORM_VERSION=10
TARGET_PRODUCT=kona
TARGET_BUILD_VARIANT=userdebug
TARGET_BUILD_TYPE=release
TARGET_ARCH=arm64
TARGET_ARCH_VARIANT=armv8-a
TARGET_CPU_VARIANT=kryo300
TARGET_2ND_ARCH=arm
TARGET_2ND_ARCH_VARIANT=armv8-2a
TARGET_2ND_CPU_VARIANT=cortex-a75
HOST_ARCH=x86_64
HOST_2ND_ARCH=x86
HOST_OS=linux
HOST_OS_EXTRA=Linux-5.4.0-124-generic-x86_64-Ubuntu-20.04.3-LTS
HOST_CROSS_OS=windows
HOST_CROSS_ARCH=x86
HOST_CROSS_2ND_ARCH=x86_64
HOST_BUILD_TYPE=release
BUILD_ID=QKQ1.201222.002
OUT_DIR=out
============================================
[100% 5/5] Install: out/target/product/kona/system/bin/hello1

#### build completed successfully (7 seconds) ####
```

## 不依赖android环境,单独编译android linux driver
```Makefile
TEST:=AAA

TOOLCHAIN := $(SDK_DIR)/LINUX/android/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/

#SPI
ifeq ($(TEST),AAA)  
ANDROID_BUILD_TOP:=$(SDK_TOP)/LINUX/android/
ANDROID_PRODUCT_OUT:=$(SDK_TOP)/LINUX/android/out/target/product/kona
endif

#I2C
ifeq ($(TEST),BBBB)  
ANDROID_BUILD_TOP:=$(SDK_TOP)/LINUX/android/
ANDROID_PRODUCT_OUT:=$(SDK_TOP)/LINUX/android/out/target/product/kona
endif

#CROSS_ARCH:=ARCH=arm64 CROSS_COMPILE="$(TOOLCHAIN)/aarch64-linux-android-"
CROSS_ARCH:=ARCH=arm64 CROSS_COMPILE="aarch64-none-elf-"
ccflags-y += -I/home/testuser/code/spi 
ccflags-y += -g -I/home/testuser/code/i2c 

KDIR:=$(ANDROID_PRODUCT_OUT)/obj/KERNEL_OBJ/
PWD:=$(shell pwd)

ifeq ($(TEST), AAA)  
## ppcciiee-ggppiioo
obj-m := ppcciiee-ggppiioo.o
ppcciiee-ggppiioo-objs := ppcciiee_ggppiioo/gpio-bb-pcidrv.o ppcciiee_ggppiioo/gpio-bb-common.o

## gpio-client test
obj-m += ggppiioo-client.o
ggppiioo-client-objs := ppcciiee_ggppiioo/ggppiioo-client.o

endif

ifeq ($(TEST),BBBB) 
## ab spi2apb
obj-m := spi2apb.o
spi2apb-objs := spi/spi2apb.o 

## ab dsi
obj-m += ab_dsid.o
ab_dsid-objs := abdsid/ab-dsid.o 

endif

.PHONY: modules package clean
all:package
modules:
	-@#@if [ "$(ANDROID_BUILD_TOP)_yes" = "_yes" ]; then echo -e "You have to run \". build/envsetup.sh\" to init enviroment first. \nAnd then you have to run\"choosecombo\" to setup the project."&&exit 1; fi
	-@#@if [ ! -d $(KDIR) ]; then echo "Build kernle first."&&cd $(ANDROID_BUILD_TOP)&&make bootimage&&cd -; fi
	$(MAKE) $(CROSS_ARCH) -C $(KDIR) M=$(PWD) modules

package:modules
	-@mkdir -p ./package
	-@cp $(KDIR)/certs/signing_key.pem  ./
	-@cp $(KDIR)/certs/signing_key.x509  ./
	-@for ko in  `ls *.ko`; do \
		echo $(KDIR)/scripts/sign-file sha512 signing_key.pem signing_key.x509 $$ko; \
		$(KDIR)/scripts/sign-file sha512 signing_key.pem signing_key.x509 $$ko; \
	done

install:
	-@install -v *.ko ~/platform-tools

clean:
	-@rm -rf *.o abc/*.o spi/*.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers package

```

## 直接修改android 内核的.config文件

`cd ${ANDROID_SDK}/LINUX/android/out/target/product/kona/obj/kernel/msm-4.1`
`make ARCH=arm64  nconfig`
在内核编译目录使用`make ARCH=arm64  nconfig`配置完`.config`后 用对比工具进行对比进行替换源码目录中的kona-perf_defconfig文件
`${ANDROID_SDK}/LINUX/android/kernel/msm-4.19/arch/arm64/configs/vendor/kona-perf_defconfig`

## Android-SDK编译
+ `cd ${ANDROID_TOP}/LINUX/android`
+ `source build/envsetup.sh
+ `lunch test-userdebug`
+ `make bootimage -j8`
+ `m kernel bootimage dtboimage`

## android一些服务
. display显示服务，该服务会独占drm `rm /vendor/bin/hw/vendor.qti.hardware.display.composer-service` 
. 网络服务, 该服务会动态操作网卡`rm /system/bin/netd`

## buildroot在android上的使用
. `target.tar.gz`为`buildroot`目录下的rootfs打包文件. 需要上传到`/system`中解压
. super.img中需要修改生成如下 `/system/buildrootlib /system/buildrootlib64 /system/buildrootusr /system/buildroottmp`4个软连接
```
adb root
adb disable-verity
adb remount
adb shell
cd /system/
ln -s /system/target/lib /system/buildrootlib
ln -s /system/target/lib64 /system/buildrootlib64
ln -s /system/target/usr /system/buildrootusr
ln -s /system/target/tmp /system/buildroottmp
```

## 网络adb
. 保证android网络正常
. usb连接线所在PC执行`adb tcpip 5555`
. `adb connect android-target-ip`
. `adb shell`