/* STM32H723ZGTx — 1MB Flash, 564KB RAM
 *
 * RAM regions:
 *   DTCM     (128KB @ 0x20000000) — CPU-only, fastest, stack lives here
 *   AXI SRAM (320KB @ 0x24000000) — general purpose, DMA accessible, heap here
 */

MEMORY
{
    FLASH    (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K
    DTCMRAM  (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
    RAM      (xrw) : ORIGIN = 0x24000000, LENGTH = 320K  /* AXI SRAM D1 */
    RAM_D2   (xrw) : ORIGIN = 0x30000000, LENGTH = 48K
    RAM_D3   (xrw) : ORIGIN = 0x38000000, LENGTH = 16K
}

/* Stack at top of DTCM (fastest memory for CPU) */
_estack = ORIGIN(DTCMRAM) + LENGTH(DTCMRAM);

_Min_Heap_Size  = 0x400;   /* 1KB C heap (FreeRTOS uses its own heap) */
_Min_Stack_Size = 0x800;   /* 2KB MSP stack */

SECTIONS
{
    /* Vector table must be first in flash */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.glue_7)
        *(.glue_7t)
        KEEP(*(.init))
        KEEP(*(.fini))
        . = ALIGN(4);
        _etext = .;
    } >FLASH

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >FLASH

    .preinit_array :
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } >FLASH

    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
    } >FLASH

    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } >FLASH

    /* Initialized data: loaded from flash, copied to AXI SRAM at startup */
    _sidata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT> FLASH

    /* Zero-initialized data: lives in AXI SRAM (DMA-accessible, FreeRTOS heap here) */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } >RAM

    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE(end = .);
        PROVIDE(_end = .);
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } >RAM

    /**
     * .noinit — NOT zeroed by startup code.
     * Used for crash records that must survive a soft reset.
     * Placed at the very end of AXI SRAM so normal data never overlaps it.
     */
    .noinit (NOLOAD) :
    {
        . = ALIGN(4);
        KEEP(*(.noinit))
        . = ALIGN(4);
    } >RAM

    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }
}

