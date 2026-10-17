# Pending moderator approval — do not submit automatically

[r/PokemonUnbound rules](https://www.reddit.com/r/PokemonUnbound/) include a
restriction in rule 6 on software/website promotion. Ask moderators whether
this free, Unbound-specific hardware add-on may be shared. Disclose substantial
AI-generated code and documentation. No approval has been requested or granted.
If approved, use a relevant flair such as Discussion and the text below.

## Title

Manual RTC for Unbound on real GBA + SuperCard without an RTC chip (EN/FR, RC1)

## Body

I wanted to play Unbound on my real GBA and a SuperCard with no RTC chip, so
I developed this clock add-on with substantial AI coding assistance.
At every startup, enter the current date and time in a small GBA menu.
The clock then advances while playing and remembers the last setting entered.
It cannot count time while the console is off. Native time-based game rules
still apply; this is not a guarantee that all daily events reset after edits.

Supported: Unbound 2.1.1.1 EN and Antony FR 2.1.179 with SuperFW 0.21.
French gameplay and saving have been tried on my GBA and feel smooth with
the optimized code. English PC checks passed; English hardware feedback is
still needed. An intermittent Options display issue remains open: this is RC1.

Free patch, offline installer, EN/FR instructions and source:
https://github.com/neylibry/nyby-unbound-rtc/releases/tag/v1.1-rc1-community-r4

Prepare your own FireRed USA v1.0 -> Unbound 2.1.1.1 -> optional Antony French
translation -> NyBy RTC patch. The installer does only the final RTC step.
No ROMs or player saves are supplied. Original NyBy code is MIT-licensed;
the game, translation and firmware retain their own rights.

SuperFW: PatchEngine and DirectSaving ON; firmware RTC, IGM and cheats OFF.
Included caches must not be regenerated. Back up your save before installing.

Substantial parts of the code and documentation were generated with Codex (AI), directed by NyBy. NyBy defined the requirements and performed the reported GBA testing. Hardware feedback concerns the French build; English hardware testing is still pending.

Credits: Skeli and team for Unbound, Antony for the French translation,
davidgfnet for SuperFW; NyBy for this independent RTC add-on.
