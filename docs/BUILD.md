# Reproduce and test

## Requirements

- Python 3.10+; standard library for build/patching scripts.
- **Zig 0.15.2**, the compiler version used for the released bytes.
- Your own exact prepared Unbound input; no ROM or upstream game/translation patch is supplied.
- Node.js 22+ for installer unit tests. Playwright and a local Chrome are optional for browser tests.

ROM inputs are both 33,554,432 bytes:

| Variant | Input SHA-256 |
|---|---|
| FR Antony 2.1.179 | `55bd66a6cd6a54b36b1c90b750541dfd0d621466bfb1ddee1e222f6207ad4164` |
| EN 2.1.1.1 | `7aa25bbf568f7cfcf6ee1cf2e9e6ff637350b3d0705c2375cabb6baa7d9739f7` |

Both start from English FireRed USA v1.0 → Unbound 2.1.1.1; FR additionally needs the Antony translation before RTC. Do not supply a previously RTC-patched ROM.

## Build runtime and BPS

From the repository root, substitute your local paths:

```sh
python sources/rtc-manuel/build.py --rom /path/Unbound-FR-clean.gba --zig /path/zig --out build
python sources/rtc-manuel-en/build.py --rom /path/Unbound-EN-clean.gba --zig /path/zig --out build
```

PowerShell example:

```powershell
python sources/rtc-manuel/build.py --rom 'D:\roms\Unbound-FR-clean.gba' --zig 'C:\tools\zig\zig.exe' --out build
```

Output folders are relative to the source directory and must stay within it. Do not set `--out` to the source directory itself. Optional `ZIG_GLOBAL_CACHE_DIR`, `ZIG_LOCAL_CACHE_DIR` and TEMP/TMP may point to private build directories. Do not use `python -O`: source safeguards use assertions.

The five compiled files are `clock.c`, `ui.c`, `verify.c`, `lowlevel.S`, `rtc_fast.S`. C uses freestanding ARM7TDMI Thumb, `-Oz`, no libc, stack protector, unwind support or dynamic allocation. The linker produces the ARM/Thumb ELF; the script extracts allocated sections into the reserved ROM hole and writes signature-checked veneers.

Outputs: personal `.gba`, distributable `.bps`, `clock.elf`, objects, `payload.bin`, `.su` stack reports and `build.json` with symbols, changed ranges, source/target fingerprints and source-file hashes. **Never commit generated ROMs or saves.**

Expected released targets:

| Variant | Payload | Target ROM SHA-256 |
|---|---:|---|
| FR | 4,968 bytes | `12f11cfe5dd61063d0bea22cdb8dacb99740c73c89d513720ca6aecbad095a00` |
| EN | 4,948 bytes | `cb31a81a5271fe0f140f6ed0267a6e1a89cb087a028c50efa344cb23f29cce07` |

Exact BPS fingerprints are in `release.json`. The build date remains 2026-10-20; publication and owner acceptance are 2026-10-21. Changing the footer changes ROM/BPS hashes even if the clock is unchanged.

## Cache and offline installer

`build.py` builds the ROM patch, **not** a new SuperFW cache. This release ships a pinned 800-byte cache and five-setting per-game profile under `SD/fr` and `SD/en`. Firmware changes need separate cache/prologue/memory validation. Do not silently regenerate a different cache for a release intended to match the accepted configuration.

```sh
python sources/installer/rebuild.py
```

This regenerates all three standalone HTML files from the template, core JS and embedded payload. With modified runtime patches, first update `release.json`, `outils-python/release.json`, BPS copies, target/source/cache hashes and `sources/installer/payload.json`; regenerate HTML afterward. Rebuilding HTML alone does not compile the GBA code or update embedded patches automatically.

The page uses Web Crypto SHA-256, a bounds-checked BPS reader and a small uncompressed ZIP writer. CSP disallows network connections. Game language is selected by the prepared ROM fingerprint, independently of the page locale. A generated ZIP is personal because it contains the user's ROM.

## Tests without a ROM

```sh
python tests/check_release.py
node tests/test_installer.cjs
python tests/run_native.py --zig /path/zig
```

The native test compiles an arithmetic model and the real `clock.c` for the host CPU. It exercises calendar vectors, reciprocal divisions, counter rollover, fractional phases, CRC records and a roughly 72-hour frame trace. It is not ARM execution, a GBA emulator or a bus/flashcart timing simulation.

During development, `python tests/check_release.py --skip-manifest` permits a stale release inventory but still checks internal consistency. A changed target intentionally fails released target-hash tests until its new release metadata is reviewed. Do not remove input guards just to make a modified ROM pass.

## Browser/ROM integration checks

With the two private clean inputs in a directory as `FR.gba` and `EN.gba`, set `NYBY_TEST_INPUTS` to that directory. Tests generate synthetic 128-KiB save fixtures; no personal save is needed. Install Playwright separately if needed (`npm install --no-save playwright`). A local Chrome is used; set `NYBY_BROWSER` to an executable path if necessary.

```sh
export NYBY_TEST_INPUTS=/private/clean-inputs
node tests/browser.cjs
python tests/verify_downloads.py
```

```powershell
$env:NYBY_TEST_INPUTS = 'D:\private\clean-inputs'
node tests/browser.cjs
python tests/verify_downloads.py
```

`NYBY_TEST_OUTPUT` optionally selects a private output directory; otherwise tests use ignored `build-tests/installer`. The test runs offline and checks real file-input/download workflows, both game languages, optional save copying, old-output reuse, rejection paths and a narrow viewport. The Python checker independently inspects the downloaded ROM, save, cache, profile and report bytes and tests the Python patcher, including its refusal to overwrite. Test the real flashcart before claiming hardware compatibility.
