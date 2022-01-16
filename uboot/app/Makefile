default:all

CROSS_COMPILE:= /opt/rk3399/linux-sdk/prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
OBJDUMP=$(CROSS_COMPILE)objdump
OBJCOPY=$(CROSS_COMPILE)objcopy
STRIP=$(CROSS_COMPILE)strip
READELF=$(CROSS_COMPILE)readelf

all:hello.elf hello.bin


hello.elf:hello.o
	#$(CC) -Ttext=0x800000 $< -o $@   -static -Wl,-Map=$@.map
	#$(CC) -T app.ld $< -o $@   -static -Wl,-Map=$@.map
	$(CC) -T appgcc.ld $< -o $@   -static -Wl,-Map=$@.map
	$(OBJDUMP) -S -d $@ > $@.asm

hello.bin:hello.elf
	$(OBJCOPY) -O binary -R .note -R .comment -S --gap-fill=0xff  $^  $@
	$(READELF) -h $^

%.o:%.c
	$(CC) -g -c -O0 $(CFLAGS) $< -o $@

clean:
	rm *.o hello *.elf *.map *.asm hello.bin -rf
