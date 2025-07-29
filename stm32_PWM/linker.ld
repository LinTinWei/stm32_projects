ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);  /* 堆疊頂端 */

SECTIONS
{
  .text :
  {
    KEEP(*(.isr_vector))
    *(.text*)
    *(.rodata*)
    _etext = .;
  } > FLASH

  /* .data 從 FLASH 複製到 RAM */
  .data : AT (ADDR(.text) + SIZEOF(.text))
  {
    _sidata = LOADADDR(.data);
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM

  /* .bss 直接放 RAM 並清零 */
  .bss :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM
}
