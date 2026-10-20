# NyBy RTC v1.5-beta3 — revue technique / engineering review

20 octobre 2026 / 20 October 2026

Cette revue séparée a été effectuée par un agent Codex intervenant comme ingénieur système GBA / ARM7TDMI. Elle porte sur les sources, les binaires et les outils PC ; elle ne constitue pas une certification matérielle indépendante.

This separate review was performed by a Codex agent acting as a GBA / ARM7TDMI systems engineer. It covers source code, binaries and PC tools; it is not independent hardware certification.

## Français

**Avis favorable pour les livrables PC audités. Aucun défaut bloquant trouvé. Un essai de cette compilation sur GBA/SuperCard reste nécessaire en FR et EN.** Les incidents historiques de capture et d'affichage des Options ne sont pas déclarés corrigés.

Les sources livrées ont été recompilées séparément avec Zig 0.15.2, depuis les deux bases Unbound propres acceptées. Les ROM obtenues et les BPS correspondent exactement à la distribution. Les trois installateurs HTML ont également été régénérés à l'octet depuis leurs sources. L'archive communautaire inspectée contient les patchs, sources, profils et documents, sans ROM, sauvegarde, image BIOS/firmware ou données PC privées détectées.

Les dix ZIP personnels déjà produits par la campagne d'installation ont été ouverts et contrôlés séparément : ROM, cache et profil exacts ; chaque sauvegarde incluse est une copie inchangée d'une fixture synthétique. Cela ne valide pas le contenu d'une partie réelle et ne répare pas une sauvegarde.

Le code intégré conserve les améliorations revues : vérification complète du secteur par huit lectures de 512 octets avec tampon isolé ; interface à deux pages avec transferts BIOS et aucune nouvelle animation ; moteur ARM à origine fixe utilisant le rapport exact `4389/262144` seconde par VBlank. Les fractions de seconde ne sont pas perdues entre lectures. Le format de réglage sur quatre octets est inchangé et mémorise le dernier réglage manuel à la minute.

Les profils réels indiquent PatchEngine et DirectSaving activés, RTC SuperFW, IGM et cheats désactivés. Les caches contiennent zéro opération RTC ; leur zone réservée ne recouvre pas le payload du mod. Aucun import de l'heure SuperFW n'est compilé ou livré.

Cette revue n'a exécuté aucun jeu, processeur ARM/GBA ou BIOS, ni accédé à une SD. Les campagnes antérieures de formules RTC, rendu C et vérificateur sont reliées à cette compilation par les empreintes des sources et du code. Les tests de navigateur et les contrôles natifs PC restent distincts d'un essai sur console. Aucun FPS, pourcentage CPU ou pic complet de pile matériel n'est revendiqué.

La console éteinte ne compte pas le temps. Des interruptions VBlank manquées peuvent faire retarder le compteur ; la tolérance de l'oscillateur réel n'a pas été mesurée. Ces limites sont distinctes d'une dérive d'arrondi du programme.

## English

**Favorable assessment of the audited PC deliverables. No blocking defect found. This build still requires real GBA/SuperCard testing in both FR and EN.** Historical capture freezes and Options display problems are not claimed to be fixed.

The delivered sources were rebuilt separately with Zig 0.15.2 against both accepted clean Unbound bases. Resulting ROMs and BPS patches match the distribution exactly. All three HTML installers were also reproduced byte for byte from their sources. The inspected community archive contains patches, sources, profiles and documentation, with no ROM, save, BIOS/firmware image or detected private PC data.

The ten personal ZIPs already produced by the installation campaign were opened and checked separately: exact ROMs, caches and profiles; every included save is an unchanged synthetic fixture. This does not validate a real game's save data or repair a save.

The integrated code preserves the reviewed improvements: full-sector verification through eight 512-byte reads with an isolated buffer; a double-page interface using BIOS bulk transfers without new animations; and an ARM engine using the exact `4389/262144` seconds-per-VBlank ratio from a fixed anchor. Fractional seconds are retained between reads. The four-byte settings format is unchanged and remembers the last manual setting with minute precision.

Actual profiles enable PatchEngine and DirectSaving while disabling SuperFW RTC, IGM and cheats. Caches contain zero RTC operations, and their reserved region does not overlap the mod payload. No SuperFW clock importer is compiled or included.

This review executed no game, ARM/GBA CPU or BIOS and accessed no SD card. Earlier RTC mathematics, production C rendering and verification tests are linked to this build through source and code fingerprints. Browser and native PC tests remain distinct from console testing. No hardware FPS, CPU percentage or complete stack high-water measurement is claimed.

Powered-off time is not tracked. Missed VBlank interrupts can make the counter fall behind; real oscillator tolerance was not measured. These limitations are separate from cumulative arithmetic rounding.

## Ressources vérifiées / verified resources

| Élément / item | Résultat / result |
|---|---:|
| Payload FR | 5 002 octets / bytes |
| Payload EN | 4 982 octets / bytes |
| Moteur RTC / RTC engine, pool inclus / including literal pool | 652 octets / bytes |
| Nouvelle RAM permanente / new permanent RAM | 0 octet / bytes |
| Cadre propre du démarrage / startup own stack frame | 112 octets / bytes |
| Chemin de dessin / drawing path | 332 octets / bytes |
| Chemin de vérification / verification path | 664 octets / bytes |

Les valeurs de pile excluent les pilotes, le BIOS et les interruptions. / Stack figures exclude drivers, BIOS and interrupts.

## Empreintes des composants / component fingerprints

SHA-256 des ROM personnelles générées, non incluses dans la distribution / SHA-256 of generated personal ROMs, not included in the distribution:

```text
FR  d7057b66baf15015f8e1de55403e0de577c72d0b857018288ccd3f176465bbed
EN  c683579f95b7990e1d8d8c86445ac80fce969f1b5c8fd583000850126a239a5e
```

SHA-256 des HTML / HTML SHA-256:

```text
INSTALLER-FR-EN.html et/and INSTALLER-FR.html
ae2ebd7c0ae602688b1cffc7d43a2244def793f2bbaacab631b3f3f068cdbada

INSTALLER-EN.html
6618abaa81f3ff28e05382d3431f74a7ebb87ba2c354a9e47af66897124c55fc
```

Le manifeste de la distribution identifie les autres fichiers et la version finale des documents. Ce rapport porte sur les composants aux empreintes ci-dessus ; ajouter le rapport et préciser les crédits ne change pas ces composants.

The distribution manifest identifies the remaining files and final documentation revisions. This report covers the components fingerprinted above; adding the report and clarifying credits does not change those components.
