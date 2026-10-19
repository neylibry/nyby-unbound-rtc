# NyBy RTC v1.5-beta2 — English guide

This beta hardens write verification and retry recovery. It checks the complete 4 KiB sector and retains the original buffer after an error. To upgrade from beta1, start from Unbound without RTC, supply a copy of your same-language .sav and use the new BETA2 caches. See [technical release notes](RELEASE-FR-EN.md) for changes, provenance and limits.

This beta targets a real GBA and SuperCard running SuperFW 0.21 without an RTC chip. It imports the firmware's date/time setting, then uses our optimized clock engine during play. A static startup screen displays the values: START launches; L opens manual adjustment.

SuperFW does not provide an automatic real-world clock on this hardware. Its timestamp is manually configured and powered-off time cannot be calculated. A plausible but stale value cannot be detected: update SuperFW's setting before each session.

## Prepare the ROM

Both paths start from your own **English FireRed USA v1.0**:

- EN: FireRed USA → Unbound 2.1.1.1 patch → our EN RTC patch.
- FR: FireRed USA → Unbound 2.1.1.1 patch → Antony French 2.1.179 translation → our FR RTC patch.

The package and HTML handle only the final RTC step. They download no games and do not include Unbound or translation patches. SHA-256 identifies the two already prepared Unbound builds and selects the correct patch. The HTML interface language does not translate the game.

This beta accepts clean Unbound without an older NyBy patch and recognizes its own patched output. To move from v1.1, start again from prepared Unbound without RTC; a same-language save can be copied. Older RTC ROMs are rejected to avoid stacking versions.

## Install

1. Keep PC backups of your working installation and .sav.
2. Extract the package and open `INSTALLER-FR-EN.html` in a recent browser. No Python, account or network needed.
3. Select prepared EN or FR Unbound. Optionally supply the matching-language native 128 KiB .sav; no save states.
4. Generate and save the personal ZIP. Output SHA-256 is verified. The .sav is copied unchanged into `SAVEGAME` under the new ROM name.
5. Extract on your PC and copy the contents to the SD root. Merge `.superfw`; adjust `SAVEGAME` if your configuration uses another directory. Back up files with matching names before replacement and safely eject the card.
6. In SuperFW: **PatchEngine, RTC ENABLED, DirectSaving enabled, IGM and cheats disabled**. Use the supplied BETA2 caches; do not regenerate them. No firmware flashing required.
7. Set date/time in SuperFW settings before launching. Review the confirmation screen; START launches, L opens adjustment. Missing/invalid data: START opens the manual form.

v1.1 required SuperFW RTC disabled: **v1.5 needs RTC enabled and its own matching cache**. Do not mix caches.

## Controls and persistence

- Left/right: select day, month, year, hour or minute.
- Up/down: decrease/increase directly; hold to repeat.
- START: remember the setting and launch.
- Setting write failure: A retries; B continues without claiming it was saved.

Dropdown lists are removed. The active field is yellow and underlined. The idle screen stays static; drawing happens only on entry and input. The last accepted setting is remembered with minute precision; it is not the time at shutdown. The native save format is unchanged, with a four-byte setting.

The complete module is 5,880 bytes FR and 5,860 bytes EN. The in-game assembly engine is unchanged from v1.1. No additional timer, IRQ or permanent RAM.

## Validation status

Checks start from the personal FireRed backup, reconstruct EN then FR, and use the HTML for the final RTC step. They cover 29 engine checks, 11 offline Chrome workflows and 10 independently verified ZIPs. ROMs and caches rebuilt from the public sources are byte-identical to HTML output.

mGBA checks all five fields, manual fallback, native save/resume, cold starts, New Year rollover and setting write errors. All 36,525 dates in 2000–2099 and all 86,400 seconds of a day are checked in compiled FR/EN machine code under Unicorn. No personal save used.

SuperFW import is tested by injecting registers into ROM copies before Unbound startup. Native fallback and save checks also use unmodified HTML output ROMs. **The full SuperFW/BIOS path and real GBA remain untested for v1.5.** Stale SuperFW timestamps cannot be detected. Native restrictions on date changes still apply; the previously reported Options choices issue remains open.

Evidence: [complete report](docs/AUTOMATED-TESTS.json). [Sources and rebuilding](sources/README-FR-EN.md). [Credits and Codex development](CREDITS-FR-EN.md). Original code under [MIT](LICENSE); third-party rights are separate. ZIPs generated with your ROM are personal; do not redistribute them.
