ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 20K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
  } > FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text*)
    *(.rodata*)
    *(.glue_7)
    *(.glue_7t)
    _etext = .;
  } > FLASH

  /* .data section in RAM, load from FLASH */
  .data : AT (_etext)
  {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM

  _sidata = LOADADDR(.data); /* FLASH address of .data */

  /* .bss section: zero-initialized variables */
  .bss :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
    _end = .;
  } > RAM

  /* Heap / Stack (if needed) can be added here */
}
