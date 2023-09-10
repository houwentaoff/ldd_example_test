UNIT = x

default:all

target     ?= abc
ROOT       ?=  ./

include $(ROOT)/common/tools.mk
include $(ROOT)/common/common.mk

IFLAG_EXTRA    		+= 
CFLAG_EXTRA    		+= 
AFLAG_EXTRA    		+= 
DEBUG_FLAGS 		= -g
LFLAGS 			= -L$(ROOT)/$(THIRD_LIB) 

COBJS               +=  hello.o #$(UNIT).o
AOBJS 				+= 

include $(ROOT)/common/rules.mk
#hello.elf:$(OBJ_DIR)/hello.o $(AOBJS_BOOT)
	#-@#$(CC) -Ttext=0x800000 $< -o $@   -static -Wl,-Map=$@.map
	#-@#$(CC) -T app.ld $< -o $@   -static -Wl,-Map=$@.map
	#-@#$(CC) -T appgcc.ld $< -o $@   -static -Wl,-Map=$@.map
	#$(CC) -T bare-metal.ld $^ -o $@   -static -Wl,-Map=$@.map
	#$(OBJDUMP) -S -d $@ > $@.asm

#hello.bin:hello.elf
	#$(OBJCOPY) -O binary -R .note -R .comment -S --gap-fill=0xff  $^  $@
	#$(READELF) -h $^
	#-@du -hs $@
