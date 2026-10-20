# Save record and write contract

The native save remains 131,072 bytes. This version uses exactly four bytes in the supported build's existing gap: sector **31**, relative **`0xFF0..0xFF3`**, file **`0x1FFF0..0x1FFF3`**. The aligned record is little-endian u32. Do not assume this gap exists in another ROM.

| Bits | Meaning |
|---|---|
| 0..25 | Whole minutes since 2000-01-01 00:00 |
| 26..31 | CRC6 of those 26 data bits |

Maximum minute value is `52,595,999`. Decode rejects raw zero/`0xFFFFFFFF`, out-of-range values and CRC mismatch, then returns `minutes * 60`. The encoded epoch includes a CRC and is not raw zero.

CRC6: initial `0x2D`, data MSB first, six-bit register, conditional polynomial XOR `0x27`, final XOR `0x15`. See `clock.c`. This detects some record errors; it is not authentication or validation of the entire game save.

## Meaning and write paths

The record remembers the **last manually entered setting**, to the minute, not the last computed shutdown time. The normal game-save hook reads the seconds word of `anchor_get()` without moving the anchor. Saving does not truncate active-session fractions. Changing this behavior needs an explicit backward-compatible save/UX design.

Startup: stage the entire original 4-KiB sector, change only the record word, program through the native entry, then compare all 4,096 bytes against the staged expected sector. On failure, retain that expected data for retry.

Normal game save: `rtc_write_sector` modifies `buffer + 0xFF0` only when sector is 31. Other sectors pass unchanged. Its relocated original prologue resumes the native writer. Native extension bytes before `0xFF0` and the other footer bytes are not interpreted as a new schema. Firmware/SD programming is not guaranteed power-failure atomic.

## Verification contract

The explicitly noinline verifier has `u32 readback[128]` (512 bytes). Eight reads at offsets `0x000..0xE00` compare the complete 4-KiB sector as aligned words. A read-status failure or mismatch returns failure immediately. The expected sector is read-only to this function.

Do not reuse the expected buffer for readback: that destroys evidence of the intended write and makes retries unreliable. Do not reduce verification to the four record bytes or native SDK success alone: that misses other sector corruption. Do not move the temporary array into unproven shared RAM/VRAM.

The editor exposes retry/continue on persistence failure. Continuing still initializes the session clock; it does not certify storage or falsely report a successful setting write.

## Installer behavior and diagnostics

The HTML accepts an optional `.sav` of exactly 128 KiB and rejects all-zero/all-`0xFF` files. It copies every byte unchanged into `SAVEGAME/<output ROM name>.sav` and records SHA-256 in the personal report. It does not check every native checksum, game value, language match or CFRU extension; it does not repair or convert a save.

Preserve newer data when copying to SD. Never attach ROMs, BIOS or personal saves publicly to bug reports. Native checksums do not authenticate every extension or establish semantic correctness. Generic FireRed assumptions are unsafe: these Unbound saves use signature `0x01121999`, rather than the generic FireRed signature. Use the exact build's schema when investigating a report.
