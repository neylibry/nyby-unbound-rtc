# Contributing

Original code is MIT. Preserve the license and credits; rights to the game, translation and firmware remain separate. Read [Architecture](docs/ARCHITECTURE.md), [Save format](docs/SAVE-FORMAT.md) and [Build](docs/BUILD.md) before changing integration code.

## A useful modification workflow

1. Branch from `main`; keep personal ROMs, saves, compiler products and credentials outside commits.
2. State the trigger, proposed behavior and measurable reason for a change.
3. Modify shared runtime files in **both** FR and EN directories. Their language metadata/input hashes differ; core code should not accidentally diverge.
4. Run package/installer/native checks. Rebuild both variants with Zig 0.15.2 and compare code size, `.su` stack reports and changed ROM ranges.
5. Regenerate release metadata, embedded BPS/cache assets and HTML only when intentionally producing a new artifact. Do not claim old target hashes cover new code.
6. Describe actual hardware testing separately from PC checks. Report the GBA model, cart, firmware, PatchEngine/DirectSaving/RTC/IGM settings, source fingerprint and scenario.

## Invariants to preserve

- ARM7TDMI/Thumb-1 compatibility, correct entry-state pointers, native prologues, register ABI and call-stack alignment.
- Fixed-anchor fractions and unsigned-counter semantics; exact calendar/BCD bytes and bounded 2000–2099 output.
- No new persistent game RAM, timer or IRQ dependency without an explicit integration design.
- Only the proven four-byte sector-31 gap; unchanged expected 4-KiB buffer across retry; full-sector comparison and checked reads.
- Word/halfword video accesses, aligned BIOS transfer counts, coherent inactive-page history, cleanup without OBJ VRAM writes.
- Exact source SHA/signature/hole guards. An unchanged FireRed game code does not identify a supported Unbound build.
- Cache/native-entry compatibility; do not enable SuperFW RTC because the game needs time—this mod deliberately replaces that path.

A blanket assembly rewrite is not required. Keep C where compiled output is compact and auditable; introduce assembly when size, stack or local instruction benefits are established. Do not turn static instruction counts into unsupported whole-game FPS claims.

## Porting

Treat a different Unbound release, translation or flashcart as a new supported profile. Recheck injection offsets, blank ROM space, save layout, native functions, startup scratch lifetime, banked-register assumptions, cache reservations and actual hardware behavior. Dropping the SHA check is not a port.

## Reporting a problem

Use the issue template. Include the exact mod version, input/output SHA if available, platform/profile, actions, whether audio/buttons continued, and a minimal reproduction. Do not attach ROMs, BIOS or personal saves publicly. Distinguish a native `.sav` from an emulator save state. A report is valuable even without code; a reproducible trigger is more useful than a guessed cause.

The project was mainly built with Codex under NyBy's direction. Human review, reproducible fixes and additional real-hardware coverage are welcome. Clearly disclose the scope of any AI-generated changes and verify their binary behavior.
