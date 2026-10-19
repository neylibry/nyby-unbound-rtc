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

Ce projet a été principalement « vibecodé » avec Codex sous la direction de NyBy, qui ne sait pas écrire lui-même le code nécessaire. Le besoin était concret : jouer à Unbound avec une horloge sur sa SuperCard sans RTC. Le moteur utilise du code ARM/Thumb compilé et une routine assembleur pour limiter le travail pendant la partie ; le petit écran ne s'exécute qu'au démarrage. Le code est partagé librement pour les autres joueurs. Les retours GBA historiques concernent la v1.1 FR ; la v1.5 bêta FR/EN attend son essai matériel.

This project was mainly vibe-coded with Codex, directed by NyBy, who cannot write the required code himself. He needed a clock for Unbound on his SuperCard without an RTC. Compiled ARM/Thumb code and an assembly runtime keep clock work small during play; the minimal UI only runs at startup. The code is freely shared for other players. Historical GBA feedback concerns v1.1 FR; v1.5 beta FR/EN still needs hardware testing.
