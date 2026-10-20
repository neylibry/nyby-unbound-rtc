# Architecture and integration contract

This describes the exact Stable 1.1 implementation, not a generic patch for other ROMs. [Build](BUILD.md) · [Persistence](SAVE-FORMAT.md) · [Validation](VALIDATION.md).

## Source map and control flow

| File | Responsibility |
|---|---|
| `rtc_fast.S` | Thumb veneers; ARM clock sampling, day/calendar/BCD conversion |
| `lowlevel.S` | Banked anchor; relocated native prologues; Thumb BIOS wrapper |
| `clock.c` | Startup arithmetic, CRC6 codec, status stubs and native save hook |
| `ui.c` | Manual editor, staging/programming sequence, checked native reads and register restoration |
| `verify.c` | Explicitly noinline 512-byte readback buffer and full-sector comparison |
| `manual.h` | Field positions, selection, notices and partial redraw |
| `banner.h`, `font.h`, `banner_font.h` | Startup tiled rasterizer and fonts |
| `build_info.h`, `footer_gen.py`, `footer.h` | Language/metadata and generated 3 × 5 footer masks |
| `link.ld`, `build.py`, `patch_formats.py` | Placement guards, signature-checked hooks, compilation and BPS |

`sources/rtc-manuel/` targets FR Antony 2.1.179; `sources/rtc-manuel-en/` targets EN 2.1.1.1. Shared runtime sources must remain identical. `build_info.h` selects UI language; `build.py` pins input SHA and output metadata.

Boot sequence: relocated original initialization → save display/interrupt state → read/decode sector-31 setting → manual editor → necessary programming and exact readback verification → set fixed anchor → clean BG/palette and restore state → return to native game. During play, only native RTC calls execute the new clock helpers. There is no new per-frame task.

The editor polls VCOUNT. IME is disabled around drawing and enabled in a narrowly scoped native programming call. Do not substitute `VBlankIntrWait` or HALT without reviewing this interrupt contract.

## ROM placement and hooks

Normal CPU ROM address is `0x08000000 + file offset`.

| Region | File interval or start |
|---|---|
| RTC payload | `0x0164C000`, CPU address `0x0964C000` |
| Reserved blank hole | `[0x0164C000, 0x01650000)`, 16 KiB |
| Pinned DirectSaving reservation | `[0x015FDC00, 0x01649C00)` |
| Gap before RTC payload | 9,216 bytes |

The source hole must be all `0xFF`. The linker rejects `.data`, `.bss` and COMMON and enforces the reservation end. Preserve input hashes, original signatures and blank-hole checks.

| Hook | ROM file offset | Entry |
|---|---|---|
| Startup | `0x000004C4` | `rtc_boot` |
| Native sector writer | `0x000D99D8` | `rtc_write_sector` |
| RTC status | `0x00A0B1DC` | `rtc_status` |
| Success path | `0x00A0B2C4` | `rtc_success` |
| Date/time | `0x00A0B2D4` | `rtc_datetime` |
| Time only | `0x00A0B334` | `rtc_time` |
| Success path | `0x00A0B398` | `rtc_success` |

Each hook is an eight-byte Thumb LDR/BX veneer plus an odd Thumb pointer. Build reports contain before/after hashes. These offsets are specific to the accepted builds; they are not a discovery strategy for other versions.

## Fixed anchor and precision

The unsigned native VBlank counter is at `0x03003114` (`gMain + 0x24`); its native increment was inspected at `0x0800075C..0x08000761`. It counts serviced VBlank events, not completed gameplay callbacks.

| UND-mode banked register | Retained value |
|---|---|
| LR | Entered seconds since 2000-01-01 |
| SP | Frame count at the anchor |

**SP_und is data, not an exception stack.** Code that uses UND exceptions or this bank requires redesign. No memory access may use that SP while the temporary mode is active. `anchor_get`/`anchor_set` save CPSR, temporarily select UND with IRQ masked using `(CPSR & ~0x1F) | 0x9B`, access banked values, then restore CPSR. Other bits and the caller's state are preserved.

```text
delta = (current_frames - anchor_frames) modulo 2^32
elapsed = floor(delta * 4389 / 262144)
        = floor(delta * 280896 / 16777216)
now = min(anchor_seconds + elapsed, 3155759999)
```

UMULL produces the unsigned 64-bit product, extracted with LSR18/LSL14. Reads always refer to the same anchor; they do not repeatedly round and re-anchor. Integer quantization is less than one second relative to the duration of the counted nominal frames. This is not a wall-clock accuracy guarantee.

Unsigned subtraction handles rollover. A full counter cycle is about 832.28 days; a longer session is ambiguous. Long IRQ masking can lose VBlank counts. Powered-off/STOP time is not recovered and oscillator tolerance is unmeasured. The calendar range is 2000–2099, saturating at its end.

The reduced multiplier has two significant bytes instead of three, reducing local UMULL internal work on ARM7TDMI. It is not a measured whole-game speedup. Primary references: [ARM DDI0029G, §§6.6/6.20](https://ww1.microchip.com/downloads/en/DeviceDoc/DDI0029G_7TDMI_R3_trm.pdf), [Tonc timing](https://www.coranac.com/tonc/text/timers.htm).

## ABI

| Function | Contract |
|---|---|
| `rtc_datetime(RtcInfo*)` | Returns 1; writes year/month/day/weekday/hour/minute/second/status at offsets 0..7 |
| `rtc_time(RtcInfo*)` | Returns 1; writes only hour/minute/second at offsets 4..6 |
| `rtc_status(RtcInfo*)` | Writes status `0x40` at offset 7; returns 1 |
| `rtc_success()` | Returns 1 |
| `anchor_get()` | Seconds in r0, anchor frames in r1; matches the declared u64 return |
| `anchor_set(seconds, frames)` | Stores the banked values and restores CPSR |

Date/time bytes are BCD except weekday (Sunday zero, 0..6). Preserve untouched fields. Datetime saves/restores r4-r8/LR (24 bytes), time r4/LR (8 bytes). Thumb entry veneers switch to ARM through BX PC. r0-r3/r12 are caller-saved; do not introduce undeclared callee-saved clobbers or misaligned stack calls.

## Startup memory and stack

The existing 4-KiB staging range is `[0x02039A38, 0x0203AA38)`, used only before normal gameplay. It is not an in-game allocation.

| Mod stack quantity | Bytes |
|---|---:|
| `rtc_boot` own frame | 112 |
| Drawing: boot + screen + values + text | 332 |
| `verify_flash_sector` own frame | 536 |
| Verification: boot + verifier + checked-read wrapper | 664 |

These exclude native SD drivers, BIOS, interrupts and outer callers. Inlining the verifier's 512-byte array back into `rtc_boot` keeps that space alive throughout editing. Moving it to unproven shared memory is not an equivalent optimization.

## Video

BG0 uses 4bpp tiles at 240 × 160. Each page has 600 tiles × 32 bytes, beginning at tile 64. Ranges are half-open:

| Object | Address |
|---|---|
| Pixel page one | `[0x06000800, 0x06005300)` |
| Pixel page two | `[0x06008800, 0x0600D300)` |
| Tile map | `0x0600F800` |
| OBJ VRAM begins | `0x06010000` |

BG0CNT selects the visible page; char bases are 32 KiB apart. A text draw calculates the inactive page once. The footer masks are generated on PC, with supported alphabet and width checked.

Updates clear ten tile rows (y=40..119), redraw all five fields and present at VBlank. Both page histories matter: pixel writes OR color nibbles, so a missing clear can leave old pixels or combine palette indices. Narrower dirty regions need explicit page synchronization.

The BIOS wrapper is exactly `swi 0x0C; bx lr` (four Thumb bytes). CpuFastSet needs aligned 32-bit source/destination and a **word count multiple of eight**; fill is bit 24. Bulk sizes: full page/clone 19,200 bytes, variable region 9,600, exit BG cleanup 65,536 and palette cleanup 1,024. Fill reads an aligned zero word in ROM. OBJ VRAM is not cleared. [Primary BIOS wrapper reference](https://raw.githubusercontent.com/devkitPro/libtonc/master/asm/tonc_bios.s).

## Firmware boundary

The pinned 800-byte cache hash is in `release.json`. Decoded operations: seven WAITCNT, eight save, four IRQ, **zero RTC**. These are firmware operations, not new RTC IRQ hooks. DirectSaving and the RTC reservation are disjoint.

Checked native read: `0x081DEA99`; programming/verification: `0x081DEC95`; resumed native writer after relocated prologue: `0x080D99E1`. A signature check distinguishes the patched status-returning read from the original SDK void contract. Review these boundaries before changing the cache or firmware target.

This integration targets the named ROMs and SuperFW 0.21. Porting requires reevaluating hooks, memory, native prologues, save gaps and firmware behavior; simply recognizing the FireRed game code is insufficient.
