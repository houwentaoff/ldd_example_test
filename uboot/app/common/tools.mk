CROSS_COMPILE:= aarch64-none-elf-
CC 			= 	$(CROSS_COMPILE)gcc
LD 			= 	$(CROSS_COMPILE)ld
OBJDUMP 	= 	$(CROSS_COMPILE)objdump
OBJCOPY 	= 	$(CROSS_COMPILE)objcopy
STRIP 		= 	$(CROSS_COMPILE)strip
READELF 	= 	$(CROSS_COMPILE)readelf
RANLIB      =   $(CROSS_COMPILE)ranlib
AS          =   $(CROSS_COMPILE)as

