# Crédits et références / Credits and references

- **NyBy** : module d'horloge, menu GBA, optimisation RTC, kit et installateur de cette distribution / RTC module, GBA menu, clock optimization, package and installer.
- **Skeli et l'équipe Unbound / Skeli and the Unbound team** : jeu Unbound / Unbound game. [Publication du projet / Project thread](https://www.pokecommunity.com/threads/pok%C3%A9mon-unbound-completed.382178/).
- **Antony** : traduction pour la variante FR / translation for the French variant. [Publication v2.1.179 / v2.1.179 release](https://github.com/AntonyKervazoCanut/gba_translator/releases/tag/v2.1.179).
- **davidgfnet et contributeurs / and contributors** : SuperFW. [Projet / Project](https://github.com/davidgfnet/superfw), [documentation des patchs / patch documentation](https://superfw.davidgf.net/docs/usermanual/patches/).
- **byuu** : spécification BPS, publiée dans le domaine public / public-domain BPS specification. [Spécification conservée avec Floating IPS / Specification preserved with Floating IPS](https://github.com/Sir-Walrus/Flips/blob/master/bps_spec.md). Les lecteurs BPS inclus sont des implémentations indépendantes / the bundled BPS readers are independent implementations.

Ce kit est un ajout indépendant. Il ne contient pas le jeu, la traduction, le firmware ou une sauvegarde personnelle. Les liens servent à identifier les projets d'origine ; ce kit ne télécharge aucun de leurs fichiers. Les composants d'origine conservent leurs crédits et conditions propres. Deux patchs RTC sont proposés pour les fichiers Unbound déjà préparés : EN après FireRed USA v1.0 + Unbound, FR après FireRed USA v1.0 + Unbound + traduction FR. Les patchs Unbound et de traduction ne sont pas inclus dans ce kit.

This is an independent add-on. It contains no game ROM, translation package, firmware, or personal save. Links identify the original projects; this package downloads none of their files. Original components retain their own credits and terms. RTC patches target already prepared Unbound files: EN after FireRed USA v1.0 + Unbound, FR after FireRed USA v1.0 + Unbound + the French translation. The Unbound and translation patches are not bundled here.

## Licence et méthode / Licensing and development

Code original NyBy : [MIT](LICENSE), [portée / scope](LICENSING-FR-EN.md).

Le projet a été principalement « vibecodé » avec Codex sous la direction de NyBy, qui ne sait pas écrire lui-même le code nécessaire. Il avait besoin d’une horloge pour jouer à Unbound sur sa SuperCard sans RTC et partage librement l’outil pour d’autres joueurs. Le C compilé ARM/Thumb et les fonctions écrites en assembleur limitent le travail et la mémoire pendant la partie ; le petit menu ne s’exécute qu’au démarrage. **Stable 1.1 FR a été validée par NyBy sur GBA le 21/10/2026 ; la version EN reste validée sur PC uniquement.**

This project was mainly vibe-coded with Codex under NyBy’s direction; he cannot write the required code himself. He needed a clock for Unbound on his SuperCard without an RTC and freely shares the tool for other players. Compiled ARM/Thumb C and assembly functions keep runtime work and memory small; the minimal menu only runs at startup. **NyBy accepted Stable 1.1 FR on real GBA on 2026-10-21; EN remains PC validated only.**


Stable 1.1 utilise un réglage manuel indépendant de l’heure SuperFW. / Stable 1.1 uses manual startup time independently of the SuperFW clock.
