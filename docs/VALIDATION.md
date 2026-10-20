# Validation and evidence boundaries

## Real hardware acceptance

On **2026-10-21**, NyBy confirmed the installed French Stable 1.1 passed the requested tests and authorized publication as the main release. Target SHA-256:

```text
12f11cfe5dd61063d0bea22cdb8dacb99740c73c89d513720ca6aecbad095a00
```

Platform: real GBA, standard-proportion LCD, SuperCard, SuperFW 0.21. Requested protocol: manual date/time and resume; Options; battle/capture; save/restart; clock progression. The evidence is the owner's overall acceptance report, not captured telemetry or an independently observed timing session. [Acceptance record](HARDWARE-VALIDATION.json).

**English hardware testing is not claimed.** EN has PC installer, source rebuild and shared-code checks. Historical capture/Options scenarios pass the owner's reported FR acceptance test; the original root cause was not proven and no promise covers every save, cart or configuration.

## PC checks

The Stable 1.1 candidate campaign recorded 29 installer-engine checks, 14 Chrome offline workflows (including one private owner-save copy) and 10 separately inspected synthetic-data ZIPs. It verified source/target fingerprints, exact save copying, cache/profile bytes, HTML reproduction and the Python fallback's output/refusal to overwrite. New release-facing status/acceptance text changes only PC packaging, not the owner-tested GBA ROMs.

Preserved earlier evidence, linked by source and binary fingerprints:

| Campaign | Scope |
|---|---|
| RTC mathematics | 65,660,514 assertions; 328,725 calendar vectors covering every 2000–2099 day; 86,400 HMS inputs; reciprocal interval endpoints; fractional phases; about 72 nominal hours sampled each frame |
| Native C renderer | 24,866 FR/EN comparisons using PC video buffers and checked BIOS mocks |
| Production verifier | 4,180 native cases: all 4,096 one-byte corruption positions, eight read errors and 76 clean/retry cases |
| Separate Codex-agent review | Source/ABI/bounds review; independent FR/EN rebuilds; HTML/BPS/cache/profile and ZIP inspection |

These are functional/math/static tests, not CPU/bus/BIOS execution. The prior rendering comparison used its recorded footer marker; the later Stable 1.1 branding is connected by source provenance and compilation. Historical reports retain their original build labels and target hashes under `docs/evidence/history/` so they are not falsely relabeled as measurements of another artifact.

The stable release retains the exact owner-tested ROMs and BPS. Public-source rebuilds and final installer checks are recorded by the release tooling. All distributed files are inventoried in `MANIFEST.json`; downloadable asset hashes are in `SHA256SUMS.txt` on the release.

## Unmeasured or unproven

- No measured global FPS improvement or total CPU percentage.
- No complete hardware stack high-water measurement including SD drivers/BIOS/IRQ nesting.
- No absolute long-session wall-clock calibration, oscillator tolerance measurement or recovery of missed VBlank interrupts.
- No guarantee of power-failure atomicity, universal flashcart support or correctness of every possible game-save value.
- No independent hardware certification; the separate engineering reviewer was another Codex agent.

Keep these distinctions when contributing or describing a modified build. A clean arithmetic trace proves the formula's behavior for the supplied counts, not that a flashcart never suppresses an interrupt or that a save has no semantic defect.

## Final public-package checks — 2026-10-21

The delivered developer tools were exercised before publication: 17 ROM-free JS checks, 28 private-input engine checks, 13 real Chrome offline workflows with zero HTTP requests, and 10 independently inspected generated ZIPs. The Python fallback produced exact FR/EN targets and refused existing output paths. The portable native C tool reproduced all 65,660,514 assertions. Both public runtime source trees rebuilt the exact released ROMs, BPS and build reports with Zig 0.15.2. No runtime source or accepted ROM byte changed during publication preparation. See [machine-readable results](PUBLICATION-CHECKS.json) and the stable11-public reports under evidence/. No SD access or personal-save read was needed for this publication campaign.
