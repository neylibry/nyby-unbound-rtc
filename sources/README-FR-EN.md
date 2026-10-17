# Rebuilding / Reconstruction

The runtime C, assembly and linker sources are unchanged from the optimized RC1. Only the build script's default input path was made portable; pass a PC copy of the exact accepted French ROM explicitly. Zig 0.15.2 and Python 3.10+ are required and not bundled.

Les sources C, assembleur et linker du module sont inchangées. Seul le chemin d'entrée par défaut du script de compilation a été rendu portable. Fournir explicitement une copie PC de la VF exacte. Zig 0.15.2 et Python 3.10+ sont requis et non inclus.

```text
python build.py --rom "PATH-TO-EXACT-FRENCH-ROM.gba" --zig "PATH-TO-ZIG" --out build-output
```

Expected ROM SHA-256 / SHA-256 de la ROM attendue : 4aef499c90b35311695d42d98e89d3cca6388552a22563742abaff1a9c658898

The output directory is created inside sources/rtc-manuel. / Le dossier de sortie est créé dans sources/rtc-manuel.

To rebuild the self-contained browser installer after editing its source, run `python rebuild.py` from sources/installer. / Pour reconstruire l'installateur autonome après modification de ses sources, lancer `python rebuild.py` dans sources/installer.

English variant / Variante anglaise: run the same command inside sources/rtc-manuel-en with the original EN ROM. Expected English output SHA-256: ca4b2835e41daecdd2f80108270bb5d6337a4ad6f6373b2a5d554c662ab1a51f
