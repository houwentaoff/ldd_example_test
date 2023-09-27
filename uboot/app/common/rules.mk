#opt as 支持伪代码incbin -I的路径,gcc不支持,但gcc支持include 含有incbin的使用as
$(OBJ_DIR)/%.o: %.S | $(OOPREREQS)
	$(call quiet,AS) $(AS) $(AFLAGS) -o $@ $< 
#debug
$(OBJ_DIR)/%.o: %.s | $(OOPREREQS)
	$(call quiet,CC) $(CC) -c $(CFLAGS) -o $@ $< 

$(OBJ_DIR)/%.o:%.c | $(OOPREREQS)
	$(CC)  -c  $(CFLAGS) $< -o $@

$(UNIT).elf:$(OBJS_ALL) 
	$(CC) -T $(ROOT)/bare-metal.ld $^ $(LFLAGS) -o $@   -static -Wl,-Map=$@.map
	$(OBJDUMP) -S -d $@ > $@.asm

$(UNIT).bin:$(UNIT).elf
	$(OBJCOPY) -O binary -R .note -R .comment -S --gap-fill=0xff  $^  $@
	#-@sh $(ROOT)/gen_mmu.sh $@ $(ROOT)/mmu.json $<
	#-@cp `dirname $@`/mmu.bin $@  -rf
	$(READELF) -h $^

clean:
	rm *.o *.bin *.elf *.map *.asm  *.o.d obj -rf 
	##下面这一句必须加上, VPATH 会搜索所有.o .c .h 依赖, 若其他目录存在obj/*.o 则不会新编译.o 导致使用了其他目录的obj/*.o
	cd $(ROOT);rm `find ./ -name '*.o'` -rf

