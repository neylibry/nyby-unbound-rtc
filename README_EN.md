# Unbound EN + FR — NyBy Manual Clock v1.1RC1

**By NyBy · Released: 17 October 2026 · EN + FR community package**

## What does this mod do?

Pokémon Unbound uses a clock for day/night cycles and time- or calendar-based activities. A SuperCard without a hardware RTC cannot provide the cartridge clock the game expects, which can leave time-dependent features stalled or unavailable.

This mod adds a small **240 × 160** menu for the original GBA screen. Enter the current date and time at startup. The game then receives a clock that advances while you play. Your last entered setting is remembered to make the next adjustment easier.

It supplies usable date/time readings **without adding RTC hardware**. It cannot track time while the console is off: correct the setting at every launch. It remembers the last value you entered, not the time of your last save or shutdown. Sleep or an interrupted video counter may also pause the clock.

Native game rules and restrictions still apply. This mod does not add extra content or guarantee that every daily event resets after a manual date change.

## Compatibility and release status

- **Games: original English Pokémon Unbound 2.1.1.1, or the same game with Antony's French translation 2.1.179.**
- **Target hardware: a real GBA and a SuperCard running SuperFW 0.21**, without a hardware RTC.
- The game keeps its language. **The GBA clock menu is English for the EN ROM and French for the FR ROM.** The installer recognizes the prepared build you supply by its fingerprint. Plain FireRed, other translations and game versions are not accepted.
- “v1.1RC1” identifies the RTC mod version. Runtime clock reads are optimized in ARM7TDMI assembly; the required game build is unchanged.
- The earlier beta was tested on a real GBA for time setting, saving and resuming. **A user report on 17 October 2026 confirms smooth gameplay with the optimized version on a real GBA.** This is qualitative feedback, without measured FPS.
- **The intermittent disappearing Options choices issue remains open.** A hardware fix has not been confirmed. The release is still identified as RC1.
- The new **English variant has passed PC checks: exhaustive calendar, native saving, clock edits and two cold restarts. A real-GBA test of this variant is still pending.**
- Other flash carts, firmware and emulators have not been validated.

## Prepare the ROM in the correct order

**Both paths start with your own English-language Pokémon FireRed USA v1.0 ROM.** First apply the Unbound patch to obtain English Unbound 2.1.1.1. For French gameplay, apply the French translation next, before the RTC patch.

| Desired game language | Preparation order |
|---|---|
| English | FireRed USA v1.0 → Unbound 2.1.1.1 patch → **NyBy RTC EN patch** |
| French | FireRed USA v1.0 → Unbound 2.1.1.1 patch → Antony French 2.1.179 translation patch → **NyBy RTC FR patch** |

**The NyBy package handles only the final step.** It does not apply the Unbound or French translation patches. Complete those steps before selecting your file in this installer. Original project references are in `CREDITS-FR-EN.md`.

“Automatic recognition” checks the SHA-256 fingerprint of your **already prepared file** against supported builds and selects the matching RTC patch. It does not select your game language or translate English Unbound into French. The installer’s Français/English selector changes only its interface. Updates from our previous French RTC versions are also supported.

## Easy installation — no Python required

1. Fully extract the **community package** on your PC. Open `INSTALLER-FR-EN.html` in a modern browser. It works offline: no account, server, or software installation is required.
2. Select your already prepared Unbound file from the workflow above in step 1. Only the matching RTC patch is selected automatically; input size, SHA-256 and output are verified. An already patched EN or FR NyBy v1.1RC1 ROM is recognized and reused without applying another patch.
3. Optionally select this game's **native 128 KiB .sav file** in step 2 to keep your progress. Keep a separate backup on your PC. Save states are not native saves. Choose a save matching the language of your ROM. This does not convert French saves to English or vice versa. The tool copies the bytes without conversion; it cannot tell whether this is your newest save or whether its internal game data is corrupt.
4. Click **Prepare the SD ZIP**, then **Save your personal ZIP**. This new ZIP contains your ROM and optionally your save: **keep it for your personal use**. The original community package is the one to share; it contains no ROM or player save.
5. Extract your personal ZIP on your PC. Copy **its contents**, not its enclosing folder, to the SD card root. Merge `.superfw` without replacing settings for other games. Back up existing files with matching names on your PC before replacing them. Show hidden files if needed.
6. Safely eject the card. Select the new ROM in SuperFW and check the launch options below.

Processing happens inside your browser, without uploading files, downloading ROMs, or writing directly to your SD. The generated personal ZIP is about 32 MiB because it contains your ROM.

### Required SuperFW options

| Option | Value |
|---|---|
| Patching mode | **PatchEngine**, instead of the internal database |
| Saving | **DirectSaving enabled** |
| SuperFW RTC | **Disabled** |
| SuperFW in-game menu / IGM | **Disabled** |
| Cheats | **Disabled** |

**Do not run “Generate patches”: caches for this exact ROM are already supplied.** Installing this mod on a card already running SuperFW 0.21 does not require flashing firmware.

Expected French files (for the EN variant, use `Unbound EN 2111 RTC NyBy v1.1 RC1` as the base filename everywhere):

```text
Unbound FR 179 RTC NyBy v1.1 RC1.gba
Unbound FR 179 RTC NyBy v1.1 RC1.patch
.superfw/patches/Unbound FR 179 RTC NyBy v1.1 RC1.patch
.superfw/config/Unbound FR 179 RTC NyBy v1.1 RC1.config
SAVEGAME/Unbound FR 179 RTC NyBy v1.1 RC1.sav  (if you added a save)
```

`SAVEGAME` is the folder used in our test setup. If SuperFW is configured to use a different folder, put the .sav there. ROM and save must have exactly matching base filenames. Only remove an older installation after backing up its ROM, save and settings on your PC and checking that your game resumes correctly.

## Using the clock and quitting

| Button | Action |
|---|---|
| Left / right | Select day, month, year, hour, or minute |
| Up / down | Change the value; hold to repeat |
| A | Open / confirm the list |
| B, inside a list | Cancel the change |
| START | Remember the setting and start the game |

Use 24-hour time; seconds restart at zero. The game's original schedule is preserved: morning 05:00–07:59, daytime 08:00–16:59, evening 17:00–19:59, night 20:00–04:59. The Time Turner item can shift the game by twelve hours.

If Unbound asks whether you use GPemu, JohnGBA or NoGBA, answer **NO**, then press A: this setup uses a real GBA.

To quit, save from the game's menu, wait for its completion confirmation, then power off. The supplied profile uses DirectSaving.

## Troubleshooting

| Symptom | What to check |
|---|---|
| White screen before the clock menu | Select PatchEngine and check both .patch caches, their exact filenames, and the profile. The internal database caused a white screen during testing. |
| PatchEngine unavailable | Check that the external cache is present and recognized. Do not regenerate patches for this ROM. |
| New Game instead of Continue | Check the exact .sav filename and SuperFW's configured save directory. Preserve your original save. |
| ROM rejected | Check supported hashes in `release.json`. Do not force the patch onto another version. |
| Save rejected | Use a native 131072-byte .sav. Blank files and save states are rejected. |
| MEMOIRE NON ENREGISTREE | A retries; B continues without remembering the setting. Check the SD and DirectSaving before continuing important progress. |
| Missing Options choices | Still an open issue; keep IGM disabled. RC1 is not a confirmed fix for it. |

For a useful report, provide GBA/cart model, SuperFW version, game and mod versions, launch options, reproduction steps and observed behavior. Do not post your ROM or save in a public comment.

## Other methods and source code

The four BPS patches are in `patches/`: one for original English Unbound and three for the French inputs. `SD/en/` contains English caches and the profile; `SD/fr/` contains the French set. For manual installation, copy only the contents of the subfolder matching your ROM. An external BPS patcher can be used; retain the expected output filename and then copy the contents of `SD/en/` or `SD/fr/`, matching your ROM.

The Python 3.10+ method remains in `outils-python/`: run `Appliquer-patch-Windows.cmd` or `py -3 appliquer_patch.py "path\source.gba"`. It creates only a ROM in `outils-python/ROM-patchee/`; it does not include your save or copy SD assets.

See `docs/VERIFICATION-FR-EN.md` for tests and limitations. Module and installer sources are in `sources/`. Compiler and emulator tools are not bundled.

Credits: RTC module **NyBy**; Unbound **Skeli and team**; French translation **Antony**; SuperFW **davidgfnet**. Independent project, without official affiliation. See `CREDITS-FR-EN.md` for links and details.

## Licensing and development

Original NyBy code is [MIT-licensed](LICENSE); see the [scope](LICENSING-FR-EN.md).

Substantial parts of the code and documentation were generated with Codex (AI), directed by NyBy. NyBy defined the requirements and performed the reported GBA testing. Hardware feedback concerns the French build; English hardware testing is still pending.
