.syntax unified
.cpu arm7tdmi
.section .text.lowlevel,"ax",%progbits
.arm
.global anchor_get
.type anchor_get,%function
anchor_get:
    mrs r2,cpsr
    bic r3,r2,#0x1f
    orr r3,r3,#0x9b           /* UND + IRQ mask; all other CPSR bits preserved */
    msr cpsr_c,r3
    mov r0,lr
    mov r1,sp
    msr cpsr_c,r2
    bx lr
.global anchor_set
.type anchor_set,%function
anchor_set:
    mrs r2,cpsr
    bic r3,r2,#0x1f
    orr r3,r3,#0x9b           /* UND + IRQ mask; all other CPSR bits preserved */
    msr cpsr_c,r3
    mov lr,r0
    mov sp,r1
    msr cpsr_c,r2
    bx lr
.global frame_seconds
.type frame_seconds,%function
frame_seconds:
    ldr r1,=280896
    umull r2,r3,r0,r1
    mov r0,r2,lsr #24
    orr r0,r0,r3,lsl #8
    bx lr
.ltorg
.thumb
.thumb_func
.global original_init
.type original_init,%function
original_init:
    push {r4,lr}
    ldr r0,=0x030030F0
    movs r4,#0
    str r4,[r0,#0x20]
    ldr r3,=0x080004CD
    bx r3
.ltorg
.thumb_func
.global original_write_sector
.type original_write_sector,%function
original_write_sector:
    push {r4,lr}
    lsls r0,r0,#24
    lsrs r4,r0,#24
    movs r0,r4
    ldr r3,=0x080D99E1
    bx r3
.ltorg

/* BIOS CpuFastSet, only for aligned transfers of a multiple of 32 bytes.
 * r0=source address, r1=destination, r2=word count | (fill ? 1<<24 : 0).
 * BIOS preserves the callee-saved registers; caller-saved/flags may change.
 */
.section .text.video_fast_set,"ax",%progbits
.balign 2
.thumb
.thumb_func
.global video_fast_set
.type video_fast_set,%function
video_fast_set:
    swi 0x0c
    bx lr
.size video_fast_set,.-video_fast_set
