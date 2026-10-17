# Unbound EN + FR — Horloge manuelle NyBy v1.1RC1

**Par NyBy · Parution : 17 octobre 2026 · Édition communauté FR + EN**

## À quoi sert ce mod ?

Pokémon Unbound utilise une horloge pour ses cycles jour/nuit et ses activités liées à l'heure ou au calendrier. Une SuperCard sans puce RTC ne fournit pas l'horloge de la cartouche attendue par le jeu : les fonctions temporelles peuvent alors rester bloquées ou indisponibles.

Ce mod ajoute un petit menu **240 × 160**, adapté à l'écran d'origine de la GBA. À chaque démarrage, vous indiquez la date et l'heure réelles. Le jeu reçoit ensuite une horloge qui avance pendant que vous jouez. Le dernier réglage saisi est mémorisé pour faciliter la prochaine saisie.

Il apporte une solution pour jouer avec une date et une heure fonctionnelles **sans ajouter de matériel RTC**. Il ne peut pas compter le temps console éteinte : corrigez le réglage à chaque lancement. Il mémorise la dernière valeur entrée, et non l'heure de la dernière sauvegarde ou de l'extinction. Une veille ou une interruption du compteur vidéo peut aussi arrêter temporairement l'horloge.

Le jeu conserve ses propres règles et restrictions : ce mod ne débloque pas de contenu additionnel et ne garantit pas que chaque événement quotidien se réinitialise après un changement manuel de date.

## Compatibilité et état de la version

- **Jeux : Pokémon Unbound 2.1.1.1 EN original, ou cette même version avec la traduction française Antony 2.1.179.**
- **Matériel visé : vraie GBA et SuperCard sous SuperFW 0.21**, sans RTC matérielle.
- Le jeu conserve sa langue. **Le menu RTC est en anglais pour la ROM EN et en français pour la ROM FR.** L’installateur reconnaît par empreinte la version déjà patchée que vous lui fournissez. FireRed seul, les autres traductions et versions ne sont pas acceptés.
- « v1.1RC1 » est la version du mod RTC. L'optimisation des lectures d'horloge est écrite en assembleur ARM7TDMI ; elle ne change pas la ROM de base attendue.
- La bêta a été essayée sur GBA avec réglage d'heure, sauvegarde et reprise de partie. **Un retour utilisateur du 17/10/2026 confirme une jouabilité devenue fluide avec la version optimisée sur vraie GBA.** Il s'agit d'un constat utilisateur, sans mesure de FPS.
- **Le problème intermittent de choix invisibles dans les paramètres reste ouvert.** Sa disparition n'a pas été confirmée sur GBA. La version reste identifiée RC1.
- La nouvelle variante **anglaise a passé les tests PC : calendrier exhaustif, sauvegarde native, changement d’heure et deux redémarrages. Son essai sur vraie GBA reste à confirmer.**
- Compatibilité avec d'autres linkers, firmwares ou émulateurs : non validée.

## Préparer la ROM dans le bon ordre

**Les deux parcours partent de votre propre ROM américaine en anglais : Pokémon FireRed USA v1.0.** Le patch Unbound transforme d'abord cette ROM en Unbound 2.1.1.1 anglais. Pour jouer en français, appliquez ensuite la traduction, avant le patch d'horloge.

| Langue souhaitée dans le jeu | Ordre des étapes |
|---|---|
| Anglais | FireRed USA v1.0 → patch Unbound 2.1.1.1 → **patch RTC NyBy EN** |
| Français | FireRed USA v1.0 → patch Unbound 2.1.1.1 → patch traduction FR Antony 2.1.179 → **patch RTC NyBy FR** |

**Le kit NyBy intervient à la dernière étape uniquement.** Il n'applique pas le patch Unbound ni la traduction FR ; ces étapes doivent déjà être terminées lorsque vous choisissez votre fichier dans l'installateur. Les références aux projets d'origine sont dans `CREDITS-FR-EN.md`.

La « reconnaissance automatique » compare l'empreinte SHA-256 du **fichier déjà préparé** aux versions acceptées, puis choisit le patch RTC correspondant. Elle ne choisit pas votre langue de jeu et ne transforme pas Unbound EN en FR. Le sélecteur Français/English de l'installateur ne change que son interface. Les mises à jour depuis nos anciennes versions RTC FR restent également prises en charge.

## Installation simple — sans Python

1. Décompressez entièrement le **kit communauté** sur le PC. Ouvrez `INSTALLER-FR-EN.html` dans un navigateur récent. L'installateur fonctionne hors ligne ; aucun compte, serveur ni installation de logiciel n'est nécessaire.
2. Choisissez votre fichier Unbound déjà préparé selon le parcours ci-dessus dans l’étape 1. Seul le patch RTC correspondant est sélectionné automatiquement ; taille, SHA-256 et résultat sont vérifiés. Une ROM NyBy v1.1RC1 EN ou FR déjà patchée est reconnue et réutilisée sans appliquer le patch une seconde fois.
3. Pour conserver votre partie, choisissez facultativement son fichier natif **.sav de 128 Kio** dans l'étape 2. Gardez-en une copie de sécurité sur le PC. Un savestate n'est pas une sauvegarde native. Utilisez une partie correspondant à la langue de votre ROM. L’outil ne convertit pas une partie FR en EN ou inversement. Il copie les octets sans conversion ; il ne peut pas établir si la partie choisie est la plus récente ni si elle est exempte de corruption interne.
4. Cliquez sur **Préparer le ZIP pour la SD**, puis **Enregistrer le ZIP personnel**. Ce nouveau ZIP contient votre ROM et éventuellement votre sauvegarde : **gardez-le pour votre usage personnel**. Le kit communauté à partager est l'archive originale, qui ne contient ni ROM ni partie.
5. Décompressez le ZIP personnel sur le PC. Copiez **son contenu**, et non le dossier qui le contient, à la racine de la carte SD. Fusionnez `.superfw` ; ne remplacez pas les réglages des autres jeux. Si un fichier du même nom existe déjà, sauvegardez-le sur le PC avant de le remplacer. Vérifiez les fichiers cachés si nécessaire.
6. Éjectez proprement la carte, puis sélectionnez la nouvelle ROM dans SuperFW et vérifiez les options ci-dessous.

La génération se fait dans le navigateur, sans envoi de fichiers, téléchargement de ROM ni écriture directe sur la SD. Le ZIP personnel fait environ 32 Mio, car il contient votre ROM.

### Options SuperFW indispensables

| Option | Valeur |
|---|---|
| Mode de patch | **PatchEngine**, à la place de « BDD interne » |
| Sauvegarde | **DirectSaving activé** |
| RTC SuperFW | **Désactivée** |
| Menu en jeu / IGM | **Désactivé** |
| Cheats | **Désactivés** |

**Ne lancez pas « Générer les patchs » : les caches correspondant exactement à cette ROM sont fournis.** Aucun flash de firmware n'est nécessaire pour installer le mod sur une carte déjà sous SuperFW 0.21.

Arborescence française attendue (la variante EN utilise partout `Unbound EN 2111 RTC NyBy v1.1 RC1` comme nom de base) :

```text
Unbound FR 179 RTC NyBy v1.1 RC1.gba
Unbound FR 179 RTC NyBy v1.1 RC1.patch
.superfw/patches/Unbound FR 179 RTC NyBy v1.1 RC1.patch
.superfw/config/Unbound FR 179 RTC NyBy v1.1 RC1.config
SAVEGAME/Unbound FR 179 RTC NyBy v1.1 RC1.sav  (si partie ajoutée)
```

`SAVEGAME` correspond à la configuration des essais. Si SuperFW utilise un autre dossier de sauvegardes, placez le .sav dans ce dossier. La ROM et le .sav doivent porter exactement le même nom de base. Ne supprimez l'ancienne installation qu'après avoir conservé sa ROM, son .sav et ses réglages sur PC et vérifié la reprise de partie.

## Utiliser l'horloge et quitter le jeu

| Bouton | Action |
|---|---|
| Gauche / droite | Choisir jour, mois, année, heure ou minute |
| Haut / bas | Modifier la valeur ; maintien pour répéter |
| A | Ouvrir / confirmer la liste |
| B, dans une liste | Annuler le changement |
| START | Mémoriser et démarrer le jeu |

Réglez l'heure sur 24 h ; les secondes repartent de zéro. Le calendrier natif est conservé : matin 05:00–07:59, jour 08:00–16:59, soir 17:00–19:59 et nuit 20:00–04:59. L'objet Time Turner peut décaler le jeu de douze heures.

Si le jeu demande si vous utilisez GPemu, JohnGBA ou NoGBA, répondez **NON**, puis A : vous utilisez une vraie GBA.

Pour quitter : sauvegardez dans le menu du jeu, attendez la confirmation de fin, puis éteignez. Le profil fourni utilise DirectSaving.

## Dépannage

| Symptôme | Vérification |
|---|---|
| Écran blanc avant le menu d'heure | Choisir PatchEngine et vérifier les caches .patch, leurs noms et le profil. « BDD interne » a causé cet écran blanc pendant les essais. |
| PatchEngine indisponible | Vérifier que le cache externe est présent et reconnu ; ne pas régénérer les patchs de cette ROM. |
| « Nouvelle partie » au lieu de « Continuer » | Vérifier le nom exact du .sav et le dossier configuré dans SuperFW. Ne pas écraser la sauvegarde d'origine. |
| ROM refusée | Voir les sources et empreintes de `release.json`. Ne pas forcer l'application sur une autre version. |
| Fichier .sav refusé | Fournir un .sav natif de 131072 octets ; les fichiers entièrement vides et les savestates sont refusés. |
| MEMOIRE NON ENREGISTREE | A réessaie ; B continue sans mémoriser. Vérifier la carte et DirectSaving avant de poursuivre une partie importante. |
| Choix de paramètres invisibles | Signalement toujours ouvert ; IGM doit rester désactivé. Ne pas considérer la RC1 comme une correction confirmée de ce défaut. |

Pour un retour utile, indiquez : modèle GBA/linker, version SuperFW, version du jeu et du mod, options de lancement, étapes et résultat observé. N'envoyez pas votre ROM ou votre sauvegarde dans un commentaire public.

## Autres méthodes et sources

Les quatre patchs BPS sont dans `patches/` : un pour la ROM EN originale et trois pour les sources FR. `SD/en/` contient les caches et le profil EN ; `SD/fr/` ceux de la FR. Pour une installation manuelle, copier uniquement le contenu du sous-dossier correspondant à la ROM. Un outil BPS externe peut être utilisé, en conservant le nom de sortie attendu et en copiant ensuite le contenu de `SD/en/` ou `SD/fr/`, selon la ROM.

La méthode Python 3.10+ reste disponible dans `outils-python/` : lancer `Appliquer-patch-Windows.cmd` ou `py -3 appliquer_patch.py "chemin\source.gba"`. Elle crée uniquement une ROM dans `outils-python/ROM-patchee/`, sans inclure la sauvegarde ni copier les fichiers SD.

`docs/VERIFICATION-FR-EN.md` décrit les tests et leurs limites. Les sources du module et de l'installateur sont fournies dans `sources/`. Les outils de compilation et d'émulation ne sont pas inclus.

Crédits : module RTC **NyBy** ; Unbound **Skeli et son équipe** ; traduction FR **Antony** ; SuperFW **davidgfnet**. Projet indépendant, sans affiliation officielle. Liens et précisions dans `CREDITS-FR-EN.md`.
