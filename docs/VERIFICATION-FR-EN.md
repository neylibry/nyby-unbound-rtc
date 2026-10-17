# Vérification et limites / Verification and limitations

## Français

La variante FR reste identique à la v1.1RC1 installée. La variante EN utilise le même moteur RTC optimisé, les mêmes hooks natifs vérifiés et un menu traduit en anglais. Son module occupe 4500 octets. Les tests machine, le calendrier exhaustif, la sauvegarde native et les deux redémarrages ont également été exécutés sur le binaire EN. L’essai EN sur vraie GBA reste à confirmer.

Tests antérieurs du binaire final : 1 930 contrôles de base et 291 065 contrôles exhaustifs, dont les 36 525 jours de 2000 à 2099, les 86 400 secondes d'une journée pour la lecture de l'heure, les limites d'écriture, les erreurs SD, la sauvegarde native et deux redémarrages. Les 192 affichages des paramètres comparés sur PC sont identiques à ceux de la VF sans module.

Mesures sous mGBA, WAITCNT 0x4000, date du 17/10/2026 à 17:30, 64 appels et coût de boucle inclus :

| Lecture | v1.0 | v1.1RC1 | Réduction |
|---|---:|---:|---:|
| Date et heure | 7 705 cycles/appel | 1 091 cycles/appel | 85,8 % |
| Heure seule | 7 775 cycles/appel | 641 cycles/appel | 91,8 % |

Il s'agit du coût de la routine sous émulation, pas d'un gain de FPS global mesuré sur GBA. Le module occupe 4 504 octets dans le remplissage libre ; la ROM reste à 32 Mio. Pas de RAM permanente supplémentaire, de timer réservé ou de nouveau service d'interruption. Le réglage persistant conserve ses quatre octets ; les sauvegardes natives bêta et v1.0 restent compatibles.

Sur matériel : la bêta a permis le réglage d'heure, la sauvegarde et la reprise. Le 17/10/2026, l'utilisateur ayant installé la version optimisée rapporte que le jeu est devenu complètement fluide sur sa GBA. Ce retour ne constitue pas une mesure de FPS ni une validation de tous les événements du jeu. Le défaut intermittent des choix de paramètres reste ouvert.

Les résultats de vérification du nouvel installateur se trouvent dans `INSTALLER-TESTS.json`. Ils couvrent notamment les quatre sources FR/EN, la réutilisation d'une ROM déjà patchée, la copie des sauvegardes, les entrées erronées et le ZIP produit, sans incorporer de ROM ou de sauvegarde aux preuves publiques.

## English

The FR variant remains identical to the installed v1.1RC1. The EN variant uses the same optimized RTC engine, verified identical native hooks, and an English menu. Its payload uses 4500 bytes. Machine checks, the exhaustive calendar, native saves and both cold restarts were also run against the EN binary. EN hardware testing remains pending.

Earlier final-binary checks: 1,930 basic checks and 291,065 exhaustive checks, including all 36,525 dates from 2000 through 2099, all 86,400 seconds for time-only reads, write boundaries, SD errors, native saves and two restarts. The 192 Options screen comparisons on PC match the French build without this module.

mGBA measurements, WAITCNT 0x4000, 17 October 2026 at 17:30, 64 calls including loop overhead: full date/time fell from 7,705 to 1,091 cycles per call (85.8%); time-only reads fell from 7,775 to 641 (91.8%). These are emulated routine costs, not measured overall GBA FPS gains.

The payload uses 4,504 bytes of existing ROM padding; ROM size remains 32 MiB. No additional permanent RAM, reserved timer, or new interrupt service is used. Persistent settings still occupy four bytes; native beta and v1.0 saves remain compatible.

Hardware feedback: the beta supported time setting, saving and resuming. On 17 October 2026, the user who installed the optimized version reported fully smooth gameplay on their GBA. This is not measured FPS or validation of every game event. The intermittent disappearing Options choices issue remains open.

See `INSTALLER-TESTS.json` for the new installer's checks, including four FR/EN inputs, already-patched ROM reuse, save copying, invalid inputs and generated ZIP contents. Public evidence contains no ROM or player save.
