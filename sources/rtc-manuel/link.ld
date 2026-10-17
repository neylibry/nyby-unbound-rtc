ENTRY(rtc_boot)
SECTIONS {
 . = 0x0964C000;
 .text : { KEEP(*(.text.rtc_boot)) KEEP(*(.text*)) }
 .rodata : { *(.rodata*) }
 /DISCARD/ : { *(.ARM.exidx*) *(.ARM.extab*) *(.comment*) *(.note*) }
 .unexpected : { *(.data*) *(.bss*) *(COMMON) }
 ASSERT(SIZEOF(.unexpected)==0,"No private persistent RAM is allowed")
 ASSERT(. <= 0x09650000,"RTC payload exceeds reserved 16 KiB")
}
