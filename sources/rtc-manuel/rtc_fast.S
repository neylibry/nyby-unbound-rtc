/* NyBy RTC: fixed-cost ARM7TDMI runtime, Thumb-1 ABI entry points.
 * No division runtime, calendar loops, private RAM, timer or new IRQ hook.
 * Existing UNDEF SP/LR retain exactly the v1.0 anchor representation.
 * Reciprocal constants are exhaustively checked over their input domains.
 */
.syntax unified
.cpu arm7tdmi
.section .text.rtc_fast,"ax",%progbits

/* Build small constants in the instruction stream instead of fetching a
 * separate literal from the slow cartridge bus. Same 8 bytes, faster here. */
.macro load16 reg,value
    mov \reg,#((\value)&0xff00)
    orr \reg,\reg,#((\value)&0xff)
.endm

.balign 4
.thumb
.thumb_func
.global rtc_datetime
.type rtc_datetime,%function
rtc_datetime:
    bx pc
    nop
.arm
    push {r4-r8,lr}
    mov r4,r0
    bl rtc_now_fast
    bl rtc_split_day_fast
    mov r5,r0                  /* days since 2000, 0..36524 */
    mov r6,r1                  /* seconds within day */

    load16 r1,22967
    mul r2,r5,r1
    mov r2,r2,lsr #25          /* complete 1461-day cycles */
    mov r7,r2,lsl #2
    load16 r1,1461
    mul r3,r1,r2
    sub r8,r5,r3               /* day within leap-first four-year cycle */
    subs r0,r8,#1
    movmi r0,#0
    load16 r1,1437
    mul r2,r0,r1
    mov r2,r2,lsr #19          /* year within cycle, 0..3 */
    add r7,r7,r2               /* actual year minus 2000 */
    load16 r1,365
    mul r3,r1,r2
    sub r8,r8,r3
    cmp r2,#0
    subne r8,r8,#1             /* zero-based day within actual year */

    mov r12,#103               /* BCD = n + 6*floor(n/10), n <= 99 */
    mul r3,r7,r12
    mov r3,r3,lsr #10
    add r3,r3,r3,lsl #1
    add r3,r7,r3,lsl #1
    strb r3,[r4,#0]

    /* March-based month arithmetic: no twelve-month search/table reads. */
    mov r2,#59
    tst r7,#3
    addeq r2,r2,#1
    cmp r8,r2
    subhs r8,r8,r2
    addlo r8,r8,#256
    addlo r8,r8,#50
    add r0,r8,r8,lsl #2
    add r0,r0,#2
    load16 r1,857
    mul r2,r0,r1
    mov r2,r2,lsr #17          /* month from March, 0..11 */
    mov r1,#153
    mul r0,r2,r1
    add r0,r0,#2
    load16 r1,1639
    mul r3,r0,r1
    mov r3,r3,lsr #13
    sub r0,r8,r3
    add r0,r0,#1               /* day of month */
    mul r3,r0,r12
    mov r3,r3,lsr #10
    add r3,r3,r3,lsl #1
    add r3,r0,r3,lsl #1
    strb r3,[r4,#2]
    cmp r2,#10
    addlo r2,r2,#3
    subhs r2,r2,#9
    mul r3,r2,r12
    mov r3,r3,lsr #10
    add r3,r3,r3,lsl #1
    add r3,r2,r3,lsl #1
    strb r3,[r4,#1]

    add r0,r5,#6               /* 2000-01-01 was Saturday */
    load16 r1,18725
    mul r2,r0,r1
    mov r2,r2,lsr #17
    rsb r2,r2,r2,lsl #3
    sub r0,r0,r2
    strb r0,[r4,#3]
    mov r0,r6
    bl rtc_hms_fast
    strb r0,[r4,#4]
    strb r1,[r4,#5]
    strb r2,[r4,#6]
    mov r0,#0x40
    strb r0,[r4,#7]
    mov r0,#1
    pop {r4-r8,lr}
    bx lr
.size rtc_datetime,.-rtc_datetime
.ltorg

.balign 4
.thumb
.thumb_func
.global rtc_time
.type rtc_time,%function
rtc_time:
    bx pc
    nop
.arm
    push {r4,lr}
    mov r4,r0
    bl rtc_now_fast
    bl rtc_split_day_fast
    mov r0,r1
    bl rtc_hms_fast
    strb r0,[r4,#4]
    strb r1,[r4,#5]
    strb r2,[r4,#6]
    mov r0,#1
    pop {r4,lr}
    bx lr
.size rtc_time,.-rtc_time

/* Return clamped absolute seconds in r0. Caller-saved registers only. */
.arm
.type rtc_now_fast,%function
rtc_now_fast:
    mrs r2,cpsr
    bic r3,r2,#0x1f
    orr r3,r3,#0x9b           /* UND + IRQ mask; all other CPSR bits preserved */
    msr cpsr_c,r3
    mov r0,lr
    mov r1,sp
    msr cpsr_c,r2
    ldr r2,=0x03003114
    ldr r2,[r2]
    sub r2,r2,r1
    mov r3,#0x1100
    orr r3,r3,#0x25
    umull r1,r12,r2,r3
    mov r1,r1,lsr #18
    orr r1,r1,r12,lsl #14      /* 280896/2^24 = 4389/2^18 exactly */
    ldr r2,=3155759999
    cmp r0,r2
    movhi r0,r2
    add r0,r0,r1
    cmp r0,r2
    movhi r0,r2
    bx lr
.size rtc_now_fast,.-rtc_now_fast

/* Return r0=days, r1=seconds-within-day; exact for all uint32 inputs. */
.type rtc_split_day_fast,%function
rtc_split_day_fast:
    ldr r2,=0xc22e4507
    umull r1,r3,r0,r2
    mov r3,r3,lsr #16
    mov r2,#0x15000
    orr r2,r2,#0x180
    mul r1,r2,r3
    sub r1,r0,r1
    mov r0,r3
    bx lr
.size rtc_split_day_fast,.-rtc_split_day_fast

/* r0=seconds-within-day -> BCD hour/minute/second in r0/r1/r2. */
.type rtc_hms_fast,%function
rtc_hms_fast:
    load16 r1,37283
    mul r12,r0,r1
    mov r12,r12,lsr #27
    mov r1,#3600             /* exact ARM immediate: no ROM data load */
    mul r3,r1,r12
    sub r2,r0,r3
    load16 r1,2185
    mul r3,r2,r1
    mov r1,r3,lsr #17
    mov r0,#60
    mul r3,r1,r0
    sub r2,r2,r3
    mov r0,#103
    mul r3,r2,r0
    mov r3,r3,lsr #10
    add r3,r3,r3,lsl #1
    add r2,r2,r3,lsl #1
    mul r3,r1,r0
    mov r3,r3,lsr #10
    add r3,r3,r3,lsl #1
    add r1,r1,r3,lsl #1
    mul r3,r12,r0
    mov r3,r3,lsr #10
    add r3,r3,r3,lsl #1
    add r0,r12,r3,lsl #1
    bx lr
.size rtc_hms_fast,.-rtc_hms_fast
.ltorg
