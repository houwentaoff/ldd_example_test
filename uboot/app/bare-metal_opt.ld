ENTRY(tom_start64) 
SECTIONS
{
    .vectors 0x80200000 :
    {
        __code_start = .;
        KEEP(*(StartUp))
        KEEP(*(EL1VECTORS EL2VECTORS EL3VECTORS InterruptHandlers))
    }
    .text           :
    {
        *(.text.exit .text.exit.*)
        *(.text.startup .text.startup.*)
        *(.text.hot .text.hot.*)
        *(SORT(.text.sorted.*))
        *(.text .stub .text.* .gnu.linkonce.t.*)
    } 
    .fini           :
    {
        KEEP (*(SORT_NONE(.fini)))
    } 
    .preinit_array    :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    }
    .init_array    :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
        PROVIDE_HIDDEN (__init_array_end = .);
    }
    .fini_array    :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
        KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
        PROVIDE_HIDDEN (__fini_array_end = .);
    }
    .data           :
    {
        __data_start = .;
        *(.data .data.* .gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
    }
    . = .;
    __bss_start = .;
    __bss_start__ = .;
    .bss            :
    {
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
   /* Align here to ensure that the .bss section occupies space up to
      _end.  Align after .bss to ensure correct alignment even if the
      .bss section disappears because there are no input sections.
      FIXME: Why do we need it? When there is no .bss section, we do not
      pad the .data section.  */
       . = ALIGN(. != 0 ? 64 / 8 : 1);
    }
    _bss_end__ = .; __bss_end__ = .;
    . = ALIGN(64 / 8);
    __end__ = .;
    .stack         0x80000 :
    {
        _stack = .;
        *(.stack)
    }
    .el1_stack (NOLOAD):
    {
        . = ALIGN(0x1000);
        __el1_stack_bottom = .;
        . = . + 8 * 0x10000;    /*512KB system stack*/
        __el1_stack_top = .;
    }
    .heap (NOLOAD):
    {             
        . = ALIGN(4096);
        __heap_start = .;
        __end__ = .;
        PROVIDE(end = .);
        . = . + 0x10000000;         /*256MB*/ 
        __heap_end = .;
    }             
 }
