
#riscv64-buildroot-linux-gnu-objcopy -O binary -R .note -R .comment -S vmlinux linux.bin
echo 'use tftp linux.bin'
mkimage  -f buildroot.its image.ub
