# Changelog

## Stable 1.1 — 2026-10-21

- Main stable release following NyBy's French real-GBA acceptance; English remains PC validated.
- Manual date/time at startup, last-entered setting prefilled; SuperFW clock import removed and firmware RTC disabled in supplied profiles.
- Startup verification uses a noinline 512-byte buffer, eight reads and exact 4-KiB comparison; staged expected sector retained for retries.
- Compact tiled double-page UI, hoisted page calculation, build-time footer masks and four-byte Thumb BIOS bulk-transfer wrapper.
- Exact fixed-anchor ARM clock using `4389/262144` seconds per serviced VBlank; compact CPSR mode construction and 652-byte runtime.
- Current payload: 4,968 bytes FR / 4,948 bytes EN; no new permanent RAM.
- Offline FR/EN installers, exact-byte save copying, Python fallback, technical memory/ABI/save documentation and reproducible test tools.
- Experimental v1.5 beta distributions are retired from current repository files and GitHub beta releases/tags. Local historical archives are retained. This is release consolidation, not a claim that a higher-numbered game version was downgraded.

The ROM build/footer remains 2026-10-20 to preserve the owner's tested artifacts. Earlier capture/Options root causes were not proven; the owner reports the requested FR scenarios pass. No universal save repair, hardware timing calibration or global FPS figure is claimed.
