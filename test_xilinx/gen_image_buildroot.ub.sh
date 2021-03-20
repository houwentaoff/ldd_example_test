aarch64-none-elf-objcopy -O binary -R .note -R .comment -S vmlinux linux.bin
mkimage -f ../../bin/system-top-buildroot.its image.ub
echo "cp image.ub /tftpboot/a.ub"
cp image.ub /tftpboot/ -rf
