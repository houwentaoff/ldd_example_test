OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(main)
SECTIONS
{
 . = 0x00800000;
 . = ALIGN(8);
 .text :
 {
  *(.__image_copy_start)
  *(.text*)
 }
 . = ALIGN(8);
 .rodata : { *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*))) }
 . = ALIGN(8);
 .data : {
  *(.data*)
 }
 . = ALIGN(8);
 . = .;
 . = ALIGN(8);
 .u_boot_list : {
  KEEP(*(SORT(.u_boot_list*)));
 }
 . = ALIGN(8);
 .efi_runtime : {
                __efi_runtime_start = .;
  *(efi_runtime_text)
  *(efi_runtime_data)
                __efi_runtime_stop = .;
 }
 .efi_runtime_rel : {
                __efi_runtime_rel_start = .;
  *(.relaefi_runtime_text)
  *(.relaefi_runtime_data)
                __efi_runtime_rel_stop = .;
 }
 . = ALIGN(8);
 .image_copy_end :
 {
  *(.__image_copy_end)
 }
 . = ALIGN(8);
 .rel_dyn_start :
 {
  *(.__rel_dyn_start)
 }
 .rela.dyn : {
  *(.rela*)
 }
 .rel_dyn_end :
 {
  *(.__rel_dyn_end)
 }
 .preinit_array :
 {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
 }

 .init_array :
 {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);
 }

 .fini_array :
 {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);
 }

 _end = .;
 . = ALIGN(8);
 .bss_start : {
  KEEP(*(.__bss_start));
 }
 .bss : {
  *(.bss*)
   . = ALIGN(8);
 }
 .bss_end : {
  KEEP(*(.__bss_end));
 }
 /DISCARD/ : { *(.dynsym) }
 /DISCARD/ : { *(.dynstr*) }
 /DISCARD/ : { *(.dynamic*) }
 /DISCARD/ : { *(.plt*) }
 /DISCARD/ : { *(.interp*) }
 /DISCARD/ : { *(.gnu*) }
}
