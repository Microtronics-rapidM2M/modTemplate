SECTIONS
{
  ER1 :
  {
    KEEP(*(.module_info))
    KEEP(*(.module_init))
  }
  ER2 :
  {
    *(.text*)

    *(.rodata*)
  }

  GOT :
  {
    PROVIDE(__got_start = .);
    *(.got*)
    PROVIDE(__got_end = .);
  }

  RW1 :
  {
    PROVIDE(__rw_base = .);
    *(.data*)
    *(.bss*)
    *(COMMON)
    KEEP(*(.module_stack))
    PROVIDE(end = .);
  }
}
