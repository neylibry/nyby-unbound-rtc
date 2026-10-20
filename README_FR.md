# NyBy RTC — Stable 1.1

Une horloge pour **Pokémon Unbound sur une vraie Game Boy Advance**, avec une **SuperCard sous SuperFW 0.21 sans puce RTC**.

[Télécharger la version stable](https://github.com/neylibry/nyby-unbound-rtc/releases/latest) · [English](README.md) · [Documentation développeur](docs/ARCHITECTURE.md) · [MIT](LICENSE)

Saisis la date et l'heure réelles au démarrage, puis appuie sur **START**. Le dernier réglage est prérempli à la session suivante. Le moteur ARM fait avancer le calendrier pendant la partie ; l'émulation RTC de SuperFW reste désactivée.

**NyBy a validé Stable 1.1 FR sur sa GBA**, suivant le protocole demandé : heure/reprise, Options, combat et capture, sauvegarde/redémarrage, progression de l'heure. La version EN est contrôlée sur PC et reconstruite depuis ses sources ; **elle n'est pas déclarée testée sur console**. [Portée des validations](docs/VALIDATION.md).

## Installation

1. Conserve sur PC une copie de ta sauvegarde native `.sav` et de l'installation qui fonctionne.
2. Prépare ta ROM personnelle dans l'ordre ci-dessous. Le kit fournit **uniquement la dernière étape RTC**.
3. Ouvre `INSTALLER-FR-EN.html` ou `INSTALLER-FR.html`, même hors ligne.
4. Choisis Unbound déjà préparé, sans ancien patch RTC. Tu peux ajouter le `.sav` natif de **128 Kio**, de la même langue : ses octets sont copiés à l'identique sous le nouveau nom.
5. Télécharge et extrais le ZIP personnel, puis copie son contenu à la racine SD en fusionnant `.superfw`. La sauvegarde va dans `SAVEGAME` ; adapte ce dossier si nécessaire. Préserve toute partie plus récente avant de remplacer un fichier.
6. Utilise le cache/profil fourni : **PatchEngine et DirectSaving activés ; RTC SuperFW, IGM et cheats désactivés**.
7. Sur GBA : **gauche/droite** choisit le champ, **haut/bas** change la valeur, **START** mémorise et lance le jeu. Choisis ensuite Continuer.

| Langue du jeu | Ordre des patchs |
|---|---|
| Anglais | **FireRed USA v1.0 en anglais** → Unbound 2.1.1.1 → NyBy RTC EN 1.1 |
| Français | **FireRed USA v1.0 en anglais** → Unbound 2.1.1.1 → traduction Antony FR 2.1.179 → NyBy RTC FR 1.1 |

La langue de la page ne traduit pas le jeu : l'empreinte du fichier Unbound détermine le patch. Le HTML vérifie les SHA-256, reconnaît sa propre sortie et refuse les anciennes ROM RTC. Un outil Python est fourni dans `outils-python/`.

Le kit ne contient aucune ROM, sauvegarde personnelle, image BIOS/firmware, patch Unbound ou patch de traduction. Le HTML n'envoie rien sur Internet et n'accède pas directement à la SD. **Le ZIP personnel généré contient ta ROM et éventuellement ta partie : ne le redistribue pas.**

## Fonctionnement et limites

L'heure avance pendant le jeu à partir du compteur VBlank natif. Le menu n'apparaît qu'au démarrage : 240 × 160, deux pages vidéo, sélection par boutons, aucune animation ni liste déroulante. Le moteur n'ajoute ni RAM permanente, ni timer, ni hook IRQ.

Le temps console éteinte n'est pas compté. La valeur mémorisée est le **dernier réglage saisi**, à la minute ; remets l'heure réelle à chaque session. De longues interruptions bloquées peuvent faire perdre des VBlank ; la tolérance du quartz n'a pas été mesurée. Les restrictions natives d'Unbound sur les changements de date restent applicables.

La copie `.sav` ne convertit, ne répare et ne valide pas les données de la partie. Les empreintes reconnues figurent dans [`release.json`](release.json). Les scénarios autrefois problématiques passent le test FR rapporté par NyBy ; cela ne démontre pas la cause antérieure ni la compatibilité de toute autre configuration.

## Pour modifier le projet

| Document | Sujet |
|---|---|
| [Architecture](docs/ARCHITECTURE.md) | Mémoire, points d'injection, ABI ARM/Thumb, ancre RTC, vidéo et firmware |
| [Sauvegarde](docs/SAVE-FORMAT.md) | Record de quatre octets, CRC6, secteur 31, vérification et erreurs |
| [Compilation](docs/BUILD.md) | Commandes Zig 0.15.2, empreintes, outils PC et génération HTML |
| [Validation](docs/VALIDATION.md) | Essais matériels rapportés, preuves PC et limites des mesures |
| [Contribution](CONTRIBUTING.md) | Workflow, invariants, parité FR/EN, portage et bugs |

Les documents techniques détaillés sont en anglais pour faciliter les contributions internationales. Le moteur est en assembleur ARM7TDMI ; le menu et la persistance sont en C freestanding compilé en Thumb. Le wrapper BIOS fait quatre octets Thumb. Python et JavaScript s'exécutent uniquement sur PC.

Payload complet : **4 968 octets FR**, **4 948 EN** ; moteur RTC commun : **652 octets**. Aucun ajout de RAM permanente. Cadre propre du démarrage : **112 octets** ; chemins du mod pour dessin/vérification : **332/664 octets**, hors pilotes/BIOS/IRQ. Ces chiffres ne sont pas des pics complets mesurés sur GBA ; aucun gain global de FPS ou de pourcentage CPU n'est annoncé.

## Méthode et licence

NyBy avait besoin de cet outil sans savoir écrire lui-même le code nécessaire. Le projet est principalement **vibecodé avec Codex**, sous sa direction et avec ses essais GBA. Il le partage librement pour les autres joueurs. Les optimisations sont fondées sur les binaires compilés et les tests documentés. La revue séparée par un agent Codex ne constitue pas une certification matérielle indépendante.

Code original NyBy sous **MIT** : modification et redistribution autorisées en conservant la notice. Les droits de Pokémon, Unbound, de la traduction et de SuperFW restent distincts. [Crédits](CREDITS-FR-EN.md) · [Portée de la licence](LICENSING-FR-EN.md).

Publication : **21/10/2026**. Le build GBA et son pied d'écran restent datés du **20/10/2026**, pour conserver les octets testés sur console.
