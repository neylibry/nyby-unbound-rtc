# NyBy RTC — Stable 1.1

A manual-start clock for **Pokémon Unbound on a real Game Boy Advance**, designed for **SuperCard + SuperFW 0.21 without a hardware RTC**.

[Download the stable release](https://github.com/neylibry/nyby-unbound-rtc/releases/latest) · [Français](README_FR.md) · [Developer documentation](docs/ARCHITECTURE.md) · [MIT](LICENSE)

Enter the real date and time at startup, then press **START**. The last entered setting is prefilled next time. A small ARM runtime advances the calendar during play. SuperFW RTC emulation is disabled for this game.

**NyBy accepted the French Stable 1.1 build on his real GBA.** English passes PC and source-rebuild checks; English hardware validation has not been reported. [Validation scope](docs/VALIDATION.md).

## Install

1. Keep a PC backup of your native `.sav` and working installation.
2. Prepare your own ROM in the order below. This project supplies **only the final RTC patch**.
3. Open `INSTALLER-FR-EN.html`, `INSTALLER-FR.html` or `INSTALLER-EN.html` from the release, including offline.
4. Select clean prepared Unbound. Optionally select a matching-language native **128 KiB `.sav`**: its bytes are copied unchanged under the new filename.
5. Download and extract the personal ZIP, then copy its contents to the SD root, merging `.superfw`. Adjust `SAVEGAME` if configured differently. Preserve newer saves before replacing files.
6. Use the supplied cache/profile: **PatchEngine and DirectSaving enabled; SuperFW RTC, IGM and cheats disabled**.
7. On GBA: **left/right** selects a field, **up/down** changes it, **START** remembers the setting and launches the game. Select Continue to resume.

| Game language | Preparation order |
|---|---|
| English | **English FireRed USA v1.0** → Unbound 2.1.1.1 → NyBy RTC EN 1.1 |
| French | **English FireRed USA v1.0** → Unbound 2.1.1.1 → Antony FR 2.1.179 translation → NyBy RTC FR 1.1 |

The prepared ROM's SHA-256 selects the patch; the page language does not translate the game. The installer verifies the output, accepts its own current output without patching twice, and rejects older RTC builds. A Python fallback is in `outils-python/`.

No ROM, personal save, BIOS, firmware image, Unbound patch or translation patch is distributed. The HTML uploads nothing and does not access the SD directly. The ZIP it generates contains your ROM and possibly your save: **do not redistribute that personal ZIP**.

## Behavior and limits

- Time advances during powered-on play from the existing native VBlank counter.
- The remembered value is the **last manually entered setting**, with minute precision. Powered-off time is not tracked; update it each session.
- No new permanent RAM, timer or IRQ hook is allocated by the RTC runtime.
- The startup-only UI is 240 × 160, uses two video pages, buttons and no animations/dropdowns.

Long periods with VBlank interrupts blocked can lose counts; oscillator tolerance has not been measured. Native Unbound date-change restrictions still apply. Copying a save does not repair, convert or validate its game data. Only the input fingerprints in [`release.json`](release.json) are supported.

## Developer entry points

| Document | Contents |
|---|---|
| [Architecture](docs/ARCHITECTURE.md) | ROM/RAM/video maps, hooks, ARM/Thumb ABI, banked anchor and firmware boundaries |
| [Save format](docs/SAVE-FORMAT.md) | Four-byte record, CRC6, full-sector verification, retries and failure behavior |
| [Build and reproduce](docs/BUILD.md) | Zig 0.15.2 commands, expected hashes, tests, HTML generation and cache responsibilities |
| [Validation](docs/VALIDATION.md) | Owner hardware acceptance, PC evidence, provenance and unmeasured quantities |
| [Contributing](CONTRIBUTING.md) | Modification workflow, parity, invariants, porting and bug reports |
| [Changelog](CHANGELOG.md) | Stable 1.1 changes |

| Resource | Stable 1.1 |
|---|---:|
| Full payload, FR / EN | 4,968 / 4,948 bytes |
| ARM RTC engine including literal pool | 652 bytes |
| Additional permanent RAM | 0 bytes |
| Startup own stack frame | 112 bytes |
| Mod drawing / verification stack paths | 332 / 664 bytes |
| Settings record | 4 bytes |

Stack figures exclude native drivers, BIOS, IRQs and outer callers. They are static mod-side figures, not measured full hardware peaks. No whole-game FPS or CPU-percentage improvement is claimed.

Sources: [`sources/rtc-manuel/`](sources/rtc-manuel) (FR), [`sources/rtc-manuel-en/`](sources/rtc-manuel-en) (EN), [`sources/installer/`](sources/installer), [`tests/`](tests). The clock is handwritten ARM7TDMI assembly; startup and persistence use freestanding C compiled to Thumb. The BIOS wrapper is four bytes of Thumb assembly. Python and JavaScript run only on PC.

## Development and credits

NyBy needed this tool but could not write the required code himself. The project was mainly **vibe-coded with Codex**, with NyBy directing it and testing it on his GBA. It is shared freely because other players may need it too. Assembly is used where benefits can be established from compiled output and recorded tests. A separate Codex agent reviewed the integration; this is not third-party hardware certification.

Original NyBy code is **MIT**, including modification and redistribution with the notice preserved. Pokémon, Unbound, Antony's translation and SuperFW retain their own rights. [Credits](CREDITS-FR-EN.md) · [License scope](LICENSING-FR-EN.md).

Release: **2026-10-21**. The GBA build/footer remains **2026-10-20**, preserving the exact owner-tested ROM bytes.
