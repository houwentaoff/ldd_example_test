# 如何使用xilinx petalinux快速构建自己的工程

## copy dts 文件
1. dts文件路径是固定的,使用脚本`cp_dts.sh`copy出. 用户自定义的dts文件在`system-user,dtsi`里面自行添加.
2. dts文件放置到`KDIR/arch/arm64/boot/xx`中,即可使用`gen_dtb.sh`进行编译`dtb`文件

## copy linux相关文件在外部重新编译
1. 全文搜索linux关键文件, tar打包copy出
2. 使用`gen_linux.sh`或者`gen_dtb.sh` 重新编译内核和`dtb`文件

## 生成tftpboot使用的image.ub文件
* 这里使用了`its`文件,里面描述了`dtb`的路径和`rootfs`的路径,目前为`ramdisk` 
* 使用`gen_image_buildroot.ub.sh` 利用`buildroot`构建的`rootfs`

## 使用新rootfs/linux编译生成的镜像

1. `tftpboot image.ub`
