SECTIONS
{
  ER1 :
  {
    KEEP(*(.module_info))
    KEEP(*(.module_init))
  }
  ER2 :
  {
    . = ALIGN(4);

    *(.text*)

    *(.rodata*)
  }

  GOT :
  {
    . = ALIGN(4);

    PROVIDE(__got_start = .);
    *(.got*)
    PROVIDE(__got_end = .);
  }

  RW1 :
  {
    PROVIDE(__rw_base = .);

    /* alignment after __rw_base!! */
    . = ALIGN(8);

    __data_start__ = .;
    *(.data*)
    __data_end__ = .;

    __bss_start__ = .;
    *(.bss*)
    *(COMMON)
    __bss_end__ = .;

    __end__ = .;
    PROVIDE(end = .);
    KEEP(*(.module_heap))
    __HeapLimit = .;

    . = ALIGN(4);
    PROVIDE(__rw_end = .);
  }
  STACK :
  {
    . = ALIGN(4);
    /* stack has to be last! */
    KEEP(*(.module_stack))
  }
}
