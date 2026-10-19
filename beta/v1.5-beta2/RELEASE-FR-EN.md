# NyBy RTC v1.5-beta2 — fiabilité de sauvegarde / save reliability

19/10/2026 · Unbound EN 2.1.1.1 / FR Antony 2.1.179 · SuperFW 0.21 · Préversion / Prerelease

## Français

Cette version corrige deux défauts du chemin d'enregistrement de l'heure. Elle conserve le moteur RTC de la bêta 1 et son fonctionnement : lecture du réglage manuel SuperFW, confirmation START, correction avec L, puis progression du temps pendant le jeu.

### Problème corrigé

Avec DirectSaving, les opérations du cache SuperFW remplacent deux vérificateurs Flash natifs par un retour de succès. L'appel SDK utilisé par le mod ne garantit donc plus, à lui seul, la vérification complète du secteur. La bêta 1 relisait ensuite uniquement les quatre octets du réglage RTC.

Conséquence reproduite : si ces quatre octets étaient corrects mais qu'un autre octet du même secteur était corrompu, la bêta 1 pouvait accepter l'écriture. Un second défaut intervenait au réessai : relire le secteur déjà endommagé faisait perdre la copie saine encore disponible en RAM.

### Modifications

- **Vérification exacte du secteur entier.** `verify_flash_sector()` relit les 4 096 octets en huit blocs alignés de 512 octets et les compare au contenu attendu. Le contrôle est indépendant des vérificateurs neutralisés par SuperFW et ne repose pas sur une empreinte probabiliste.
- **Réessai avec le tampon conservé.** L'état local `staged` garde les données d'origine du secteur dans le tampon natif. Une erreur transitoire peut ainsi être réparée sans adopter comme référence les données partiellement écrites.
- **Modification du réglage après erreur.** La réécriture reste imposée si le joueur change l'heure avant START ou revient à la valeur initialement enregistrée. Le mot RTC est actualisé dans le tampon conservé ; les autres octets restent inchangés.
- **Fichiers de distribution recalculés.** ROM personnelles de validation, BPS, caches SuperFW, empreintes de l'installateur HTML et sources portent le numéro bêta 2 et la date du 19 octobre 2026.

Le tampon à `0x02039A38` est déjà employé par les routines natives de sauvegarde des ROM exactes FR/EN étudiées. Le réglage occupe toujours les quatre octets `0x1FFF0–0x1FFF3` du `.sav` de 128 Kio. Le secteur 31 contient également des données du jeu : elles sont conservées et vérifiées, sans être considérées comme un espace sacrifiable.

### Ressources et performances

| Mesure | Bêta 1 | Bêta 2 |
| --- | ---: | ---: |
| Module complet FR | 5 804 octets | 5 880 octets |
| Module complet EN | 5 784 octets | 5 860 octets |
| Pile propre observée au démarrage | 356 octets | 868 octets |
| Allocation RAM permanente supplémentaire | 0 octet | 0 octet |

Le correctif coûte **76 octets de ROM et 512 octets de pile temporaire supplémentaires**, au démarrage. Le tampon natif de 4 Kio est réutilisé. Les 868 octets observés excluent la pile des pilotes natifs/SD et ne constituent pas une borne du pire cas matériel.

Le moteur rapide de 656 octets reste identique après relocation. Aucun travail de vérification supplémentaire n'est ajouté à la boucle du jeu ni aux lectures RTC. Dans le microbanc mGBA, à `WAITCNT=0x4000`, trampoline installé compris : **1 035 cycles** pour date et heure, **585 cycles** pour l'heure seule. Un appel complet par image représente environ **0,3685 %** du budget de 280 896 cycles. Il s'agit de coûts émulés isolés, pas d'une mesure globale de charge CPU sur SuperCard.

### Tests et provenance

**Campagne approfondie du 19 octobre, sur le candidat C1 :**

- FR/EN : 13 scénarios de référence et 15 scénarios corrigés par langue ; erreurs de lecture, programmation, vérification, écritures partielles, corruption hors réglage et réessais avec changement de valeur.
- Vérification du relais natif sur les 32 secteurs ; seul le mot RTC du secteur 31 est modifié par le hook. Contrôles de registres, pile, limites mémoire et canaris.
- CRC6 : 32 inversions d'un bit et 2 048 mots aléatoires par langue. Le CRC6 reste un détecteur limité, pas une protection de toute la sauvegarde.
- **24 redémarrages à froid et 8 sauvegardes natives** enchaînés, à partir du `.sav` précédent, par commandes du jeu. ROM inchangées, sauvegardes synthétiques, sans injection RAM ni savestate dans cette campagne.
- **6 048 échantillons de timers**, trois répétitions identiques, avec plusieurs dates et profils d'attente mémoire. IRQ/DMA/son désactivés pour isoler les fonctions.
- Cadence de callback2 mesurée sur 600 images par scène, à même date et tranche horaire : pièce au repos, direction dans la pièce et menu pause. C1 et bêta 1 ont le même nombre d'appels dans chaque fenêtre. Le surcoût de l'instrumentation est inclus ; ces scènes ne couvrent pas un combat chargé ni une grande ville.

**Vérifications sur les fichiers finaux bêta 2 :** compilation ARM7TDMI avec Zig 0.15.2 ; recompilation des sources livrées donnant des ROM, BPS et caches identiques aux sorties HTML ; 29 contrôles du moteur d'installation, 11 parcours Chrome hors ligne, 10 ZIP vérifiés séparément, zéro requête HTTP de l'installateur. Les essais mGBA FR/EN vérifient aussi le rendu 240 × 160, le réglage manuel, l'import simulé, les erreurs, le passage du Nouvel An, la sauvegarde native et la reprise à froid.

La provenance est vérifiée : entre C1 et bêta 2, seuls **deux octets de texte** du numéro affiché changent ; le code exécuté, dont les deux correctifs, est identique. Les mesures approfondies sont réutilisées sur cette preuve d'équivalence, sans prétendre avoir refait ces campagnes sous le nom bêta 2. Les contrôles finaux ci-dessus ont bien été exécutés sur la nouvelle compilation. Les tests exhaustifs du calendrier 2000–2099 et des 86 400 secondes d'une journée restent acquis pour le moteur inchangé.

Preuves : [validation et limites](docs/VALIDATION-FR-EN.md), [tests finaux](docs/AUTOMATED-TESTS.json), [équivalence C1 → bêta 2](docs/C1-TO-BETA2-PROVENANCE.json), [rapports du laboratoire](docs/lab/README.md).

### Installation et limites connues

Pour passer de bêta 1 à bêta 2, repartez d'Unbound EN ou FR **sans ancien patch RTC**, puis fournissez facultativement une copie du `.sav` natif de la même langue. Le format de sauvegarde n'a pas changé ; le HTML copie et renomme le fichier sans conversion. Conservez une copie de votre installation précédente.

**SuperFW : PatchEngine, RTC activée, DirectSaving activé, IGM et cheats désactivés. Utilisez les nouveaux caches BETA2.** La date/heure SuperFW reste manuelle et ne progresse pas console éteinte.

Le contrôle après écriture **ne rend pas l'opération atomique**. Une coupure peut endommager le secteur et perdre le tampon RAM. Les coupures du laboratoire sont des modèles de fautes, pas une simulation physique de carte SD. A réessaie avec le tampon conservé ; B permet de continuer mais ne répare pas une écriture partielle déjà réalisée. Le trajet complet SuperFW/BIOS et le contrôleur SD réel restent à valider sur matériel. Le signalement historique des choix parfois absents dans les paramètres du jeu reste ouvert.

Kit gratuit : HTML hors ligne, BPS FR/EN, caches, guides, sources originales sous MIT et rapports. Aucune ROM, sauvegarde, traduction complète ou firmware inclus. [Crédits et méthode de développement avec Codex](CREDITS-FR-EN.md).

## English

This release fixes two defects in startup clock-setting persistence. It retains beta 1's RTC engine and behavior: import the manually configured SuperFW timestamp, confirm with START, adjust with L, then advance time during play.

### Fixed behavior

With DirectSaving, SuperFW cache operations replace two native Flash verification routines with a successful return. The SDK call used by the mod therefore no longer guarantees full-sector verification. Beta 1 subsequently reread only the four RTC setting bytes.

Reproduced failure: beta 1 could accept a write when those four bytes were correct but another byte in the same sector was corrupted. A second defect affected retries: rereading the damaged sector discarded the intact copy still held in RAM.

- **Exact full-sector verification.** `verify_flash_sector()` rereads all 4,096 bytes in eight aligned 512-byte blocks and compares them with the expected buffer. It does not depend on SuperFW's bypassed native verification or a probabilistic hash.
- **Retained-buffer retry.** Local `staged` state preserves the original sector data, allowing a transient write failure to be repaired without adopting partially written data as the new reference.
- **Editing after an error.** Repair is still required when the player changes the setting before START or returns to the previously saved value. The RTC word is updated in the retained buffer; other bytes remain intact.
- **Rebuilt distribution.** BPS files, matching SuperFW caches, installer hashes and sources carry beta 2 branding and the 19 October 2026 build date.

The buffer at `0x02039A38` is already used by the native save routines in the exact supported FR/EN ROMs. The setting still occupies `.sav` bytes `0x1FFF0–0x1FFF3` in a 128 KiB save. Sector 31 also holds game data; it is preserved and verified, not treated as disposable storage.

### Resource cost

The complete module grows from **5,804 to 5,880 bytes FR** and **5,784 to 5,860 bytes EN**: **+76 ROM bytes**. Observed mod-owned startup stack grows from **356 to 868 bytes**, adding a temporary 512-byte readback buffer. There is no additional permanent RAM allocation; the existing 4 KiB native buffer is reused. These stack figures exclude native/SD drivers and are not a physical worst-case bound.

The 656-byte fast RTC block is unchanged after relocation. No additional verification runs in the game loop or on RTC reads. At `WAITCNT=0x4000`, the isolated mGBA microbenchmark, including the deployed trampoline, measures **1,035 cycles** for date/time and **585 cycles** for time only. One complete read per frame uses approximately **0.3685%** of a 280,896-cycle frame budget. These are emulated isolated costs, not total SuperCard CPU utilization.

### Validation and provenance

**In-depth 19 October campaign on C1:**

- FR/EN: 13 baseline and 15 corrected fault scenarios per language, including read/program/verification errors, partial writes, corruption outside the setting and retries after editing.
- All 32 native write-hook sectors checked, with only sector 31's RTC word changed by the hook; register, stack, bounds and canary audits.
- CRC6: 32 single-bit flips and 2,048 random words per language. CRC6 remains limited error detection, not whole-save protection.
- **24 cold starts and 8 native in-game saves** chained from the previous `.sav`, using controls and synthetic saves, without RAM injection or save states in that campaign.
- **6,048 timer samples**, three identical repetitions, multiple dates and wait-state profiles; IRQ/DMA/audio disabled to isolate function costs.
- Callback2 cadence sampled over 600 frames per scene at matched date/time ranges: idle room, directional input and pause menu. C1 and beta 1 have identical callback counts in every window. Instrumentation overhead is included; busy battles and dense maps are outside this scope.

**Final beta 2 artifacts:** ARM7TDMI compilation with Zig 0.15.2; shipped-source rebuilds reproduce the exact HTML-generated ROMs, BPS and caches; 29 installer-engine checks, 11 offline Chrome workflows and 10 independently verified ZIPs, with zero installer HTTP requests. FR/EN mGBA checks also cover the 240 × 160 layout, manual adjustment, simulated timestamp import, errors, New Year rollover, native save and cold resume.

Provenance is verified: C1 and beta 2 differ only in **two display-branding bytes**. Executed code, including both fixes, is identical. The engineering campaigns are carried forward on that equivalence proof, not represented as repeated beta 2 campaigns. The final-artifact checks were run on the new build. Exhaustive 2000–2099 calendar and 86,400-second day checks remain applicable to the unchanged RTC engine.

Evidence: [validation and limits](docs/VALIDATION-FR-EN.md), [final checks](docs/AUTOMATED-TESTS.json), [C1 → beta 2 equivalence](docs/C1-TO-BETA2-PROVENANCE.json), [laboratory reports](docs/lab/README.md).

### Upgrade and known limits

To upgrade, supply clean prepared Unbound EN or FR **without an older RTC patch**, optionally adding a copy of your same-language native `.sav`. The save format is unchanged; HTML copies and renames the file without conversion. Keep a backup of the previous setup.

**SuperFW: PatchEngine, RTC enabled, DirectSaving enabled, IGM and cheats disabled. Use the new BETA2 caches.** Its date/time setting is manual and does not track powered-off time.

Post-write verification **does not make writes atomic**. Power loss can damage the sector and discard the retained RAM buffer. Laboratory power-cut cases are fault models, not a physical SD simulation. A retries from the retained buffer; B continues but does not repair an already partial write. The complete SuperFW/BIOS path and real SD controller remain untested on hardware. The previously reported intermittent native Options choices issue remains open.

Free package: offline HTML, FR/EN BPS, matching caches, guides, original MIT source and evidence. No ROM, save, complete translation or firmware included. [Credits and Codex development method](CREDITS-FR-EN.md).
