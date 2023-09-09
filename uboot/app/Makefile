default:all

#CROSS_COMPILE:= /opt/rk3399/linux-sdk/prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
CROSS_COMPILE:= aarch64-none-elf-
CC 			= 	$(CROSS_COMPILE)gcc
LD 			= 	$(CROSS_COMPILE)ld
OBJDUMP 	= 	$(CROSS_COMPILE)objdump
OBJCOPY 	= 	$(CROSS_COMPILE)objcopy
STRIP 		= 	$(CROSS_COMPILE)strip
READELF 	= 	$(CROSS_COMPILE)readelf
RANLIB      =   $(CROSS_COMPILE)ranlib
AS          =   $(CROSS_COMPILE)as

VPATH               +=  boot/armv8/

all:hello.elf hello.bin

OBJ_DIR 			?= obj

A_BOOT              ?=  vector.o start.o
AOBJS_BOOT          ?=  $(addprefix $(OBJ_DIR)/, $(A_BOOT))


AFLAGS              += $(DEBUG_FLAGS) $(INC_PATH) $(AFLAGS_EXTRA) --MD $@.d
CFLAGS 				?= -g -O0


$(OBJ_DIR):
	mkdir -p $@

#opt as不支持宏 include操作 还有debug选项
$(OBJ_DIR)/%.o: %.S $(OBJ_DIR) | $(OOPREREQS)
	$(call quiet,AS) $(AS) $(AFLAGS)  -o $@ $< 
#debug
$(OBJ_DIR)/%.o: %.s $(OBJ_DIR) | $(OOPREREQS)
	$(call quiet,CC) $(CC) -c $(CFLAGS)  -o $@ $< 

hello.elf:$(OBJ_DIR)/hello.o $(AOBJS_BOOT)
	-@#$(CC) -Ttext=0x800000 $< -o $@   -static -Wl,-Map=$@.map
	-@#$(CC) -T app.ld $< -o $@   -static -Wl,-Map=$@.map
	-@#$(CC) -T appgcc.ld $< -o $@   -static -Wl,-Map=$@.map
	$(CC) -T bare-metal.ld $^ -o $@   -static -Wl,-Map=$@.map
	$(OBJDUMP) -S -d $@ > $@.asm

hello.bin:hello.elf
	$(OBJCOPY) -O binary -R .note -R .comment -S --gap-fill=0xff  $^  $@
	$(READELF) -h $^
	-@du -hs $@

$(OBJ_DIR)/%.o:%.c $(OBJ_DIR)
	$(CC) -g -c -O0 $(CFLAGS) $< -o $@

install:
	install -v hello.bin ~/a

clean:
	rm *.o hello *.elf *.map *.asm hello.bin obj *.d -rf
