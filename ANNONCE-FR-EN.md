# Texte de présentation prêt à copier / Ready-to-copy release post

## Français

**NyBy — Horloge manuelle pour Unbound EN + FR / vraie GBA + SuperCard — v1.1RC1**

Votre SuperCard n'a pas de puce RTC et les activités d'Unbound liées au temps vous manquent ? Ce petit mod demande la date et l'heure au démarrage, puis fournit au jeu une horloge qui avance pendant la session. Le dernier réglage saisi est mémorisé pour faciliter la prochaine saisie. Il ne compte pas le temps console éteinte : il faut remettre l'heure au lancement.

Il prend en charge **Unbound 2.1.1.1 anglais original et la traduction française Antony 2.1.179, avec SuperFW 0.21**. Il conserve les règles du jeu, dont le cycle jour/nuit ; il ne garantit pas le rétablissement de chaque événement quotidien après un changement de date.

**Préparation de la ROM, dans cet ordre :**

- **EN :** FireRed USA v1.0 → patch Unbound 2.1.1.1 → patch RTC NyBy EN.
- **FR :** FireRed USA v1.0 → patch Unbound 2.1.1.1 → patch traduction FR Antony 2.1.179 → patch RTC NyBy FR.

Le kit NyBy intervient seulement à la dernière étape. Unbound et la traduction éventuelle doivent être appliqués auparavant. Le choix FR/EN de l’interface ne change pas la langue du jeu.

Le kit comprend un installateur bilingue hors ligne, sans Python : ouvrir le HTML, choisir sa propre ROM et éventuellement son .sav, puis générer un ZIP prêt à copier sur la SD. Les caches et le profil SuperFW sont inclus. Le kit public ne contient aucune ROM ni sauvegarde. L’installateur reconnaît par empreinte la version d’Unbound déjà préparée et sélectionne uniquement le patch RTC adapté. Le menu d’horloge correspond à la langue de cette version. La version anglaise a passé les tests PC de sauvegarde/reprise et du calendrier ; son essai sur vraie GBA reste à confirmer.

L'optimisation ARM7TDMI a fait l'objet de tests sur PC. Un utilisateur confirme maintenant une jouabilité fluide sur vraie GBA. Le problème intermittent d'affichage des choix dans les paramètres reste ouvert : cette édition conserve le statut RC1.

**Réglages : PatchEngine ; DirectSaving activé ; RTC SuperFW, IGM et cheats désactivés. Ne pas régénérer les patchs fournis.**

Guide FR/EN, sources, empreintes et limites inclus. Merci de joindre les versions et les étapes de reproduction à tout retour, sans publier de ROM ou de sauvegarde personnelle.

RTC : NyBy. Unbound : Skeli et son équipe. Traduction française : Antony. SuperFW : davidgfnet. Projet indépendant.

## English

**NyBy — Manual clock for English and French Unbound / real GBA + SuperCard — v1.1RC1**

Does your SuperCard lack an RTC chip, leaving you without Unbound's time-dependent activities? This small mod asks for the current date and time at startup, then supplies a clock that advances during play. It remembers the last value you entered for easier adjustment next time. Powered-off time is not counted: set the clock again whenever you launch the game.

It supports **original English Unbound 2.1.1.1 and Antony's French translation 2.1.179, running on SuperFW 0.21**. Native game rules, including day/night behavior, remain in place. The mod does not guarantee that every daily event resets after a manual date change.

**ROM preparation, in order:**

- **EN:** FireRed USA v1.0 → Unbound 2.1.1.1 patch → NyBy RTC EN patch.
- **FR:** FireRed USA v1.0 → Unbound 2.1.1.1 patch → Antony French 2.1.179 translation patch → NyBy RTC FR patch.

The NyBy package handles only the final step. Apply Unbound and the optional translation beforehand. Selecting FR/EN in the interface does not change the game’s language.

The package includes a bilingual offline installer, with no Python required: open the HTML, select your own ROM and optionally its .sav, then generate a ZIP ready to copy to your SD. SuperFW caches and the profile are included. The public package contains no ROM or player save. The installer checks the fingerprint of your already prepared Unbound build and selects only the matching RTC patch. The clock menu matches that build’s language. The English variant has passed PC calendar and save/resume checks; its real-GBA test remains pending.

The ARM7TDMI optimization has been tested on PC. A user now reports smooth gameplay on real GBA hardware. The intermittent disappearing Options choices issue remains open, so this release retains its RC1 status.

**Settings: PatchEngine; DirectSaving enabled; SuperFW RTC, IGM and cheats disabled. Do not regenerate the supplied patches.**

FR/EN documentation, source code, hashes and limitations are included. When reporting a problem, provide versions and reproduction steps without posting ROMs or personal saves.

RTC: NyBy. Unbound: Skeli and team. French translation: Antony. SuperFW: davidgfnet. Independent project.

## Licence et méthode / Licensing and development

Code original NyBy : [MIT](LICENSE), [portée / scope](LICENSING-FR-EN.md).

Des parties importantes du code et de la documentation ont été générées avec Codex (IA), sous la direction de NyBy. NyBy a défini le besoin et effectué les essais sur sa GBA. Les tests matériels rapportés concernent la FR ; l'EN reste à confirmer sur console.

Substantial parts of the code and documentation were generated with Codex (AI), directed by NyBy. NyBy defined the requirements and performed the reported GBA testing. Hardware feedback concerns the French build; English hardware testing is still pending.
