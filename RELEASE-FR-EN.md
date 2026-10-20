## Français

**Stable 1.1 devient la version principale de NyBy RTC.** NyBy a validé la version française sur sa vraie GBA, SuperCard et SuperFW 0.21. L’anglais passe les contrôles PC et la reconstruction exacte ; son test matériel reste à effectuer.

Saisir la date et l’heure au démarrage, puis jouer : le calendrier d’Unbound avance pendant la session, sans puce ni pile RTC. La dernière saisie est mémorisée. Le temps ne peut pas avancer console éteinte.

### Changements techniques

- Réglage manuel indépendant de l’heure SuperFW ; émulation RTC firmware désactivée dans le profil fourni.
- Vérification complète d’un secteur de sauvegarde de 4 096 octets par huit lectures de 512 octets, dans une fonction isolée. Le tampon attendu reste intact pendant les nouvelles tentatives.
- Interface GBA 240×160 en tuiles 4 bpp : sélection par boutons, deux pages présentées pendant VBlank, sans écran blanc forcé au changement de champ ni animation.
- Moteur ARM/Thumb de **652 octets**, rapport exact **4389 / 262144** seconde par VBlank, ancre fixe conservant les fractions, calendrier 2000–2099. Aucune RAM permanente supplémentaire.
- Installateurs HTML hors ligne FR/EN : identification/vérification par SHA-256, copie optionnelle de la sauvegarde native sans changer ses octets, cache SuperFW de 800 octets et profil par jeu.
- Sources, mémoire, ABI, points d’injection, format de sauvegarde, reconstruction et tests reproductibles documentés.

Les ROM et patchs restent identiques à la compilation du **20 octobre 2026** validée par le propriétaire. Publication le **21 octobre 2026**. Les anciennes distributions bêta 1.5 sont retirées au profit de cette version.

### Installation et validation

Télécharger l’archive complète ou un HTML. Préparer sa propre ROM : **FireRed USA v1.0 anglais → Unbound 2.1.1.1 → traduction Antony 2.1.179 si FR → NyBy RTC**. L’installateur réalise uniquement la dernière étape. Sauvegarder ses fichiers avant la copie sur SD. Aucun jeu, BIOS, firmware ou fichier de partie n’est fourni.

Reconstruction exacte FR/EN, tests d’installation et ZIP hors ligne, copies de sauvegarde, analyse statique et **65 660 514 assertions mathématiques sur PC**. Les essais GBA sont ceux du propriétaire pour la variante FR ; aucune mesure globale de FPS/charge CPU ou de dérive réelle n’est revendiquée. La cause historique des incidents capture/Options n’a pas été démontrée.

Projet principalement **vibecodé avec Codex** par NyBy, qui avait besoin de cet outil sans savoir écrire lui-même le code. Code original sous **MIT**, libre à modifier et redistribuer avec conservation de la licence ; les droits des jeux, de la traduction et du firmware restent distincts.

[Guide français](https://github.com/neylibry/nyby-unbound-rtc/blob/main/README_FR.md) · [Architecture](https://github.com/neylibry/nyby-unbound-rtc/blob/main/docs/ARCHITECTURE.md) · [Tests et limites](https://github.com/neylibry/nyby-unbound-rtc/blob/main/docs/VALIDATION.md)

---

## English

**Stable 1.1 is now the main NyBy RTC release.** The French build was accepted by NyBy on his real GBA, SuperCard and SuperFW 0.21. English passes PC checks and exact source rebuilds; English hardware acceptance is still outstanding.

Enter the real date and time at startup. The runtime advances Unbound’s calendar during play and remembers the last entered setting. Powered-off time cannot be tracked. The supplied per-game profile disables SuperFW RTC emulation.

The release combines isolated full-sector verification (eight 512-byte reads, unchanged expected buffer on retry), a tiled interface without forced-white field changes, and a **652-byte ARM/Thumb RTC engine** using the exact **4389 / 262144** seconds-per-VBlank ratio. No additional permanent RAM is reserved. Full payloads: **4,968 bytes FR / 4,948 bytes EN**. Pinned SuperFW cache: **800 bytes**.

Offline HTML installers check source/output SHA-256, select the patch independently of the page language, and optionally copy a native 128-KiB save byte for byte. Installation order: **English FireRed USA v1.0 → Unbound 2.1.1.1 → Antony translation if French → NyBy RTC**. Only the final RTC step is included. No ROM, BIOS, firmware image or personal save is distributed.

The owner-tested ROM bytes remain the **20 October 2026 build**; published **21 October 2026**. Older 1.5 beta distributions are retired. Exact rebuilds, offline installer/download checks, static review and native mathematical tests are documented, including **65,660,514 PC assertions**. These do not measure whole-game FPS, wall-clock calibration or flashcart behavior. The original cause of earlier capture/Options incidents remains unproven.

The project was mainly vibe-coded by **NyBy with Codex**, to solve a practical player need without prior coding ability. Original code is **MIT**. Developers can find the memory map, hooks, ARM/Thumb ABI, clock arithmetic, save layout, build instructions and portable tests. Contributions and critical review are welcome.

[English guide](https://github.com/neylibry/nyby-unbound-rtc#readme) · [Architecture](https://github.com/neylibry/nyby-unbound-rtc/blob/main/docs/ARCHITECTURE.md) · [Build and tests](https://github.com/neylibry/nyby-unbound-rtc/blob/main/docs/BUILD.md)
