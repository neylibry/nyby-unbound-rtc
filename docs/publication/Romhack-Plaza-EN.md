# Submission worksheet — not yet submitted

Suggested title: **NyBy Manual RTC for Pokémon Unbound**
Author: **NyBy**
Platform: **Game Boy Advance**
Version: **1.1RC1** (community packaging **r4**)
Release date: **2026-10-17**
Suggested type: **Romhack / addendum or improvement**, subject to moderator
confirmation. This is an add-on for an existing hack, not a full translation.
Base retail game: **Pokémon FireRed Version (USA), revision 0 / v1.0**.
Immediate patch inputs: **Unbound 2.1.1.1 EN**, or **Antony FR 2.1.179**.
Patch format: **BPS**.
Project: https://github.com/neylibry/nyby-unbound-rtc
Download: https://github.com/neylibry/nyby-unbound-rtc/releases/tag/v1.1-rc1-community-r4
Upload file: **NyBy-Unbound-RTC-v1.1RC1-FR-EN-Community-r4-2026-10-17.zip**
Screenshot: **APERCU-EN.png**, optionally **APERCU.png** (720 × 480,
integer 3× enlargement of the GBA's 240 × 160 display, same aspect ratio).

## Description to paste (no hashes in this field)

NyBy Manual RTC is an independent clock add-on for Pokémon Unbound on real
GBA hardware with a SuperCard running SuperFW 0.21 and no RTC chip.
Enter the current date and time at each boot, then the clock advances while
playing. The last entered setting is remembered. Powered-off time is not counted.
Native game calendar rules and restrictions continue to apply.

It supports Unbound 2.1.1.1 English and Antony French 2.1.179. Start from your
own FireRed USA v1.0, apply official Unbound, optionally the French translation,
then the appropriate NyBy RTC patch. This package only performs that last step.
The offline bilingual HTML installer selects the RTC patch by the prepared
file's fingerprint and can copy a matching native 128 KiB save without conversion.
There are no ROM files, complete translations, firmware or player saves included.

French hardware testing has confirmed gameplay and save/resume, with smooth
gameplay reported by NyBy. English has passed PC checks but still needs real-GBA
testing. An intermittent Options-menu display issue is still open. This remains
a release candidate. Other flashcarts and game versions are not validated.

Use PatchEngine and DirectSaving; disable SuperFW RTC, IGM and cheats.
Matching caches are included and must not be regenerated.
Original NyBy code is provided under MIT; third-party game/translation/firmware
rights remain separate. Official downloads are free.

Substantial parts of the code and documentation were generated with Codex (AI), directed by NyBy. NyBy defined the requirements and performed the reported GBA testing. Hardware feedback concerns the French build; English hardware testing is still pending.

Credits: NyBy (RTC); Skeli and team (Unbound); Antony (French translation);
davidgfnet (SuperFW). This is not an official Unbound release.

## Dedicated hash fields / technical attachment

Do not label the retail FireRed hash as the immediate input to the RTC patch.
Use the matching prepared-Unbound input below. `release.json` lists all accepted
inputs, update patches and their hashes.

| Input | SHA-256 | CRC32 |
|---|---|---|
| FireRed USA v1.0, provenance only; cannot be fed directly to RTC installer | 3d0c79f1627022e18765766f6cb5ea067f6b5bf7dca115552189ad65a5c3a8ac | DD88761C |
| Unbound 2.1.1.1 EN, immediate EN BPS input | 7aa25bbf568f7cfcf6ee1cf2e9e6ff637350b3d0705c2375cabb6baa7d9739f7 | 4B3D4957 |
| Unbound FR Antony 2.1.179 without NyBy RTC, immediate FR BPS input | 55bd66a6cd6a54b36b1c90b750541dfd0d621466bfb1ddee1e222f6207ad4164 | 51F81F4B |

If the form accepts only one patch input, ask for the correct addendum/category
handling or separate EN/FR entries. Do not enter a misleading single base hash.
