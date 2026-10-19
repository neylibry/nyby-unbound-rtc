# Sources / Rebuilding

Les sources originales FR sont dans `rtc-manuel/`, EN dans `rtc-manuel-en/`. Le moteur C/assembleur cible ARM7TDMI ; la construction utilise Zig 0.15.2, Python 3.10+ et aucune libc. Les versions de ROM exactes sont décrites dans `../release.json`.

Original FR source is in `rtc-manuel/`, EN in `rtc-manuel-en/`. ARM7TDMI C/assembly; builds use Zig 0.15.2, Python 3.10+ and no libc. Exact ROM inputs are in `../release.json`.

Exemple depuis la racine du kit / Example from the package root:

```text
python sources/rtc-manuel/build.py --rom /path/to/clean-unbound-FR.gba --zig /path/to/zig --out build-output
python sources/rebuild-cache.py --rom /path/to/clean-unbound-FR.gba --build sources/rtc-manuel/build-output --template "SD/fr/Unbound FR 179 RTC NyBy v1.5 BETA2.patch" --output "sources/rtc-manuel/build-output/Unbound FR 179 RTC NyBy v1.5 BETA2.patch"
```

Pour EN, remplacer `rtc-manuel` par `rtc-manuel-en`, utiliser la ROM EN et le cache EN. Les sorties doivent rester dans un sous-dossier du dossier source. Aucun remplacement du fichier d'entrée. Le cache fourni conserve 19 opérations de compatibilité et ajoute une seule sonde RTC dans notre zone ; l'outil vérifie les destinations et l'espace DirectSaving.

For EN, use `rtc-manuel-en`, the EN input and EN cache. Outputs must remain in a subdirectory of the source folder. The input is preserved. The supplied cache retains 19 compatibility operations and adds one RTC probe in our own area; the tool checks destinations and DirectSaving space.

```text
python sources/installer/rebuild.py
```

Cette commande reconstruit le HTML depuis le template, le moteur JavaScript et `payload.json`. Si vous modifiez le code GBA, mettez aussi à jour les BPS, caches, empreintes du manifeste et données du payload avant de reconstruire le HTML. Compiler seulement une ROM ne met pas l'installateur à jour.

This rebuilds HTML from the template, JavaScript core and `payload.json`. After changing GBA code, also update BPS files, caches, manifest hashes and payload data before rebuilding HTML. Compiling a ROM alone does not update the installer.

`superfw_import.S` lit une seule fois les registres réservés transmis par SuperFW ; `rtc_fast.S` fait progresser l'heure à partir du compteur vidéo existant. `ui.c`, `banner.h` et `manual.h` ne dessinent qu'au démarrage. Aucun timer/IRQ supplémentaire ou RAM permanente. Les ROM, outils tiers et sauvegardes ne sont pas fournis.

`superfw_import.S` reads SuperFW's reserved registers once; `rtc_fast.S` advances time from the existing video counter. `ui.c`, `banner.h` and `manual.h` draw only at startup. No additional timer/IRQ or permanent RAM. ROMs, third-party tools and saves are not supplied.

Les essais d'installation reproductibles sont dans `tests/installer.cjs`. Installer Playwright séparément, puis placer `FR.gba`, `EN.gba` et des sauvegardes natives de test `FR.sav`, `EN.sav` dans `<dossier-prive>/inputs/`. Utiliser uniquement des copies ; les ROM doivent être les deux sources sans RTC. Lancer `node tests/installer.cjs <racine-du-kit> <dossier-prive>`. `CHROME_PATH` peut désigner Chrome ; sinon le navigateur Playwright installé est utilisé. Les ZIP personnels restent dans le dossier privé. Le rapport ne remplace pas un test matériel.

Reproducible installer checks are in `tests/installer.cjs`. Install Playwright separately, then put clean `FR.gba`, `EN.gba` and native test saves `FR.sav`, `EN.sav` in `<private-folder>/inputs/`. Use copies only. Run `node tests/installer.cjs <package-root> <private-folder>`. Set `CHROME_PATH` for Chrome or use the installed Playwright browser. Personal ZIPs stay in the private folder. These checks do not replace hardware testing.
