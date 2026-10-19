/* NyBy, 2026-10-17. Original experimental code; MIT.
 * Experimental alpha: integrated in PC test ROMs, not installed on the SD.
 * r0: aligned address of our dedicated SuperFW RTC-probe gate.
 * r1: current Unbound VBlank frame counter.
 * Returns 1 if imported, 0 otherwise. Caller must be in System mode.
 * Success keeps UND_LR (seconds since 2000) and replaces UND_SP (firmware
 * speed setting) with the frame anchor used by our existing clock engine.
 * Failure leaves both banked registers unchanged. No RAM/IO/stack writes.
 */
.syntax unified
.cpu arm7tdmi
.section .text.nyby_import,"ax",%progbits
.balign 4
.thumb
.thumb_func
.global nyby_import_superfw_clock
.type nyby_import_superfw_clock,%function
nyby_import_superfw_clock:
    bx pc
    nop
.arm
    ldr r2,[r0]
    ldr r3,=0x47702001       /* SuperFW 0.21 RTC probe: movs r0,#1; bx lr */
    cmp r2,r3
    bne .Lreject
    mrs r2,cpsr
    and r3,r2,#0x1f
    cmp r3,#0x1f            /* require privileged System mode */
    bne .Lreject
    orr r3,r2,#0x80         /* mask IRQ while accessing banked registers */
    bic r3,r3,#0x1f
    orr r3,r3,#0x1b
    msr cpsr_c,r3
    cmp sp,#5              /* SuperFW RTC_SPEED_CNT == 6 */
    bhi .Lbank_reject
    ldr r3,=3155760000      /* first second of 2100; exclusive bound */
    cmp lr,r3
    bhs .Lbank_reject
    mov sp,r1
    mov r0,#1
.Lrestore:
    msr cpsr_c,r2
    bx lr
.Lbank_reject:
    mov r0,#0
    b .Lrestore
.Lreject:
    mov r0,#0
    bx lr
.size nyby_import_superfw_clock,.-nyby_import_superfw_clock
.ltorg

/* A future compatible cache will apply exactly one RTC probe operation
 * here. This word is data, never called, and not a game RTC entry point.
 */
.section .rodata.nyby_gate,"a",%progbits
.balign 4
.global nyby_superfw_gate
.type nyby_superfw_gate,%object
nyby_superfw_gate:
    .word 0x5942594e
.size nyby_superfw_gate,4
