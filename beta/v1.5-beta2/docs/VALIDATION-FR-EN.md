# Validation v1.5-beta2

## Français

La publication utilise les correctifs du candidat C1 validé le 19/10/2026. Une comparaison exhaustive des deux ROM, FR et EN, établit que seules les lettres du numéro C1/B2 changent : deux octets en données constantes. Les instructions, adresses, alignements, hooks et le format du réglage sont identiques. Voir [la preuve](C1-TO-BETA2-PROVENANCE.json).

### Contrôles finaux réellement exécutés sur bêta2

- Compilation ARM7TDMI, Zig 0.15.2, `-Oz`, sans libc ; entrées Unbound contrôlées par SHA-256.
- Recompilation des sources livrées : ROM, BPS et caches identiques aux sorties HTML.
- 29 contrôles moteur d'installation, 11 parcours Chrome hors ligne, 10 ZIP vérifiés indépendamment ; aucun appel HTTP. Sources conservées, copie `.sav` exacte, refus des entrées incompatibles et des sauvegardes vides, refus d'écrasement par le programme Python secondaire.
- mGBA FR/EN sur les fichiers issus du HTML : confirmation, correction directe des cinq champs, données absentes/invalides, appui START maintenu, affichage statique 240 × 160, erreur de mémorisation, passage du Nouvel An et progression de l'horloge.
- Sauvegarde native et redémarrage à froid sur les sorties HTML non modifiées ; conservation du réglage et des autres données. Les tests utilisent exclusivement des sauvegardes synthétiques.

[Rapport final](AUTOMATED-TESTS.json). Les essais de transfert injectent le timestamp dans des copies de ROM avant le démarrage d'Unbound ; ils ne démarrent pas le firmware réel. Les essais natifs de sauvegarde utilisent aussi des ROM HTML sans modification et des commandes du jeu.

### Campagne C1 réutilisée par équivalence

Le [laboratoire](lab/README.md) contient les preuves de défauts, la correction des réessais, les audits mémoire/ABI, les 24 redémarrages et 8 sauvegardes natives, ainsi que les 6 048 mesures de timers. Le coût de 1 035 cycles/date-heure et 585 cycles/heure seule inclut le trampoline ; les mesures sont isolées, à WAITCNT 0x4000, IRQ/DMA/son désactivés. Les 868 octets de pile observés au démarrage excluent les pilotes natifs/SD.

L'émulation conserve 16 777 216 Hz, 280 896 cycles/image, 240 × 160 et environ 59,7275 Hz ; frameskip et optimisation des boucles inactives désactivés. La cadence callback2 est distincte des images visuellement différentes et de la vitesse du PC. Les scènes mesurées sont une pièce du début, une direction et le menu ; elles ne démontrent pas les performances dans tous les combats/cartes. L'instrumentation est limitée à des ROM privées et son surcoût est inclus.

Les contrôles exhaustifs déjà acquis sur le moteur inchangé couvrent 36 525 dates et 86 400 secondes d'une journée ; ils ne sont pas annoncés comme une nouvelle campagne matériel. Les [rapports historiques](../../v1.5-beta1/docs/AUTOMATED-TESTS.json) et leurs fichiers sources de calendrier sont conservés dans la version précédente.

### Limites

Le cœur mGBA est épinglé à `0.11-212-7a12d6d` et son SHA-256 est enregistré. Il utilise un BIOS HLE et un modèle de cycles ; aucun contrôleur SD/SuperCard physique n'est reproduit. Le trajet complet SuperFW/BIOS et le matériel restent à tester. Les pilotes de certains audits de mémoire/fautes sont remplacés par des doubles ; cela est indiqué dans leurs rapports.

Les quatre octets du réglage appartiennent à un secteur de 4 Kio contenant aussi des données du jeu. Vérifier et réessayer ne rend pas l'écriture atomique. Une coupure peut perdre la copie RAM ; B pour continuer ne répare pas les octets déjà partiellement écrits. Le CRC6 ne protège pas toute la sauvegarde. Le défaut historique des choix dans les paramètres reste ouvert.

## English

Beta2 uses the C1 fixes validated on 19 October. An exhaustive FR/EN ROM comparison shows only two display-version bytes differ. Executable instructions, addresses, alignment, hooks and record format are identical. See the [equivalence proof](C1-TO-BETA2-PROVENANCE.json).

Final beta2 checks actually executed: ARM7TDMI compilation with Zig 0.15.2; byte-identical shipped-source rebuilds; 29 engine checks, 11 offline Chrome workflows and 10 independent ZIP checks with no HTTP requests; FR/EN mGBA startup, direct field editing, invalid/missing data, held START, static 240 × 160 rendering, write errors, New Year rollover, native save and cold resume using actual HTML output. Only synthetic saves are used. [Final report](AUTOMATED-TESTS.json).

The [C1 laboratory evidence](lab/README.md) is carried forward by code equivalence, not presented as newly repeated beta2 measurements. It includes fault injection/retry recovery, ABI/memory audits, 24 cold starts, 8 native saves and 6,048 timer samples. At WAITCNT 0x4000, 1,035 date/time cycles and 585 time-only cycles include the trampoline, with IRQ/DMA/audio disabled for isolation. The observed 868-byte startup stack excludes native/SD drivers.

Guest settings are 16,777,216 Hz, 280,896 cycles/frame, 240 × 160 and approximately 59.7275 Hz, with frame skipping and idle-loop removal disabled. Callback2 cadence is distinct from changed image count and host speed. Tested scenes are an early room, directional input and pause menu, not every battle/map. Private instrumentation overhead is included. Previously completed exhaustive calendar/day checks remain applicable to the unchanged engine; historical evidence is retained with its original version identifiers.

mGBA `0.11-212-7a12d6d` is pinned by hash, uses HLE BIOS and a cycle model, and does not emulate the physical SD/SuperCard controller. Timestamp handoff is simulated before Unbound startup; some driver calls are stubbed in memory/fault audits, as documented. Full firmware/BIOS and hardware testing remain pending.

The four-byte record shares a 4 KiB sector with game data. Verification and retries do not make writes atomic. Power loss can discard the retained RAM buffer; B to continue does not repair an already partial write. CRC6 is not whole-save protection. The historical native Options choices issue remains open.
