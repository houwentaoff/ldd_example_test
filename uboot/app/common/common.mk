XXX_VPATH     ?= xxx/a xxx/b
RTOS_DIR              = rtos
THIRD_LIB 			  = libs

VPATH                +=  $(ROOT)/boot/armv8  $(addprefix $(ROOT)/, $(XXX_VPATH))

all:$(UNIT).elf $(UNIT).bin

mkobj:
	-@mkdir -p obj

OBJ_DIR 			 = obj
OOPREREQS 			+= mkobj $(ALOBJS:.o=.h) $(ALOBJS:.o=.c)

A_BOOT              ?= start.o vector.o 
C_BOOT 				?= 

XXX_OBJS     ?= 
IFLAG_XXX    ?= -Ixxx/a
IFLAG_RTOS          ?= -I$(RTOS_DIR)/common_smp/inc   -I$(RTOS_DIR)/ports_smp/cortex_a55_smp/gnu/inc/ 
IFLAG_BOOT 			?= -Iboot/armv8/ -Iboot/armv8/common -Iboot/common

AOBJS_BOOT          ?=  $(A_BOOT)  
COBJS_BOOT          ?=  $(C_BOOT)  

IFLAGS      		=  $(subst -I, -I$(ROOT), $(IFLAG_BOOT) $(IFLAG_XX) $(IFLAG_RTOS) $(IFLAG_EXTRA))

AFLAGS              += $(DEBUG_FLAGS) $(IFLAGS) $(AFLAG_EXTRA) --MD $@.d
CFLAGS  		    += $(IFLAGS) $(DEBUG_FLAGS) $(CFLAG_EXTRA) -O0 -DCORTEXA -D$(target)

OBJS_ALL 			=$(addprefix $(OBJ_DIR)/, $(AOBJS_BOOT) $(AOBJS) $(XX_OBJS) $(COBJS_BOOT) $(COBJS))


