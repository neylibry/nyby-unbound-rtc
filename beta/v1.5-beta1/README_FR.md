# NyBy RTC v1.5-beta1 — guide français

Cette bêta est destinée à une vraie GBA et une SuperCard sous SuperFW 0.21 sans puce RTC. Elle récupère le réglage de date/heure du firmware, puis conserve notre moteur optimisé pour faire avancer le temps pendant la partie. Au démarrage, un écran fixe affiche les valeurs : START lance le jeu, L permet de les corriger.

SuperFW ne dispose pas d'une heure réelle automatique sur ce matériel. Son réglage est manuel et le temps console éteinte ne peut pas être calculé. Une valeur ancienne mais plausible n'est pas détectable : remettez SuperFW à l'heure avant chaque session.

## Préparer la ROM

Les deux parcours partent de votre propre **FireRed USA v1.0 en anglais** :

- EN : FireRed USA → patch Unbound 2.1.1.1 → notre patch RTC EN.
- FR : FireRed USA → patch Unbound 2.1.1.1 → traduction Antony FR 2.1.179 → notre patch RTC FR.

Le kit et son HTML réalisent uniquement la dernière étape RTC. Ils ne téléchargent aucun jeu et n'incluent pas les patchs Unbound ou de traduction. L'empreinte SHA-256 reconnaît les deux versions Unbound déjà préparées et choisit le bon patch. La langue de l'interface HTML ne traduit pas le jeu.

Cette bêta accepte Unbound sans ancien patch NyBy et sa propre sortie déjà patchée. Pour venir de v1.1, repartez du fichier Unbound préparé sans RTC ; votre sauvegarde de même langue peut être copiée. Les anciennes ROM RTC sont refusées pour éviter d'empiler les versions.

## Installer

1. Conservez sur PC votre installation qui fonctionne et une copie du .sav.
2. Extrayez le kit, puis ouvrez `INSTALLER-FR-EN.html` dans un navigateur récent. Aucun Python, compte ou réseau nécessaire.
3. Choisissez Unbound EN ou FR préparé comme ci-dessus. Facultativement, ajoutez le .sav natif de 128 Kio de cette même langue, sans savestate.
4. Générez et enregistrez le ZIP personnel. La sortie est contrôlée par SHA-256 ; le .sav est copié à l'identique sous le nom de la nouvelle ROM dans `SAVEGAME`.
5. Extrayez sur PC et copiez les fichiers à la racine SD. Fusionnez `.superfw` et adaptez `SAVEGAME` si votre configuration utilise un autre dossier. Sauvegardez tout fichier du même nom avant remplacement, puis éjectez proprement la carte.
6. Dans SuperFW : **PatchEngine, RTC ACTIVÉE, DirectSaving activé, IGM et cheats désactivés**. Utilisez les caches BETA1 fournis ; ne les régénérez pas. Aucun flashage de firmware nécessaire.
7. Réglez la date/heure dans les réglages SuperFW avant le lancement. Vérifiez l'écran de confirmation ; START lance, L ouvre la correction. Données absentes/invalides : START ouvre le formulaire manuel.

La v1.1 demandait RTC SuperFW désactivée : **la v1.5 exige RTC activée avec son propre cache**. Ne mélangez pas les caches.

## Commandes et mémoire

- Gauche/droite : sélectionner jour, mois, année, heure ou minute.
- Haut/bas : diminuer/augmenter directement ; maintien de touche pour répéter.
- START : mémoriser le réglage et lancer le jeu.
- Échec de mémorisation : A réessaie, B continue sans prétendre avoir enregistré.

Les listes ont été supprimées. Le champ actif est jaune et souligné. L'écran est fixe au repos ; le dessin se fait à l'ouverture et sur action. Le dernier réglage accepté est conservé, à la minute ; ce n'est pas l'heure de fermeture du jeu. Le format de sauvegarde reste inchangé, avec quatre octets de réglage.

Le module complet occupe 5 804 octets en FR, 5 784 en EN. Le moteur assembleur en partie reste celui de v1.1. Aucun timer, IRQ ou RAM permanente supplémentaires.

## État de validation

Les essais repartent de la copie personnelle de FireRed, reconstruisent EN puis FR et passent la dernière étape par le HTML. Ils couvrent 29 contrôles du moteur, 11 parcours Chrome hors ligne et 10 ZIP vérifiés séparément. Les ROM et caches reconstruits depuis les sources publiques sont identiques aux sorties HTML.

mGBA vérifie les cinq champs, le secours manuel, la reprise et la sauvegarde natives, les redémarrages à froid, le passage du Nouvel An et les erreurs de mémorisation. Les 36 525 dates 2000–2099 et les 86 400 secondes d'une journée sont vérifiées dans le code machine FR/EN sous Unicorn. Aucune sauvegarde personnelle utilisée.

L'import SuperFW est testé par injection de registres dans des copies de ROM, avant le démarrage d'Unbound ; les essais natifs de secours et de sauvegarde utilisent aussi les sorties HTML sans modification. **Le trajet complet depuis SuperFW/BIOS et le matériel GBA ne sont pas encore validés pour v1.5.** Une date SuperFW périmée reste indétectable. Les restrictions natives du jeu sur les changements de date s'appliquent ; le défaut historique des choix dans les paramètres reste ouvert.

Preuves : [rapport complet](docs/AUTOMATED-TESTS.json). [Sources et compilation](sources/README-FR-EN.md). [Crédits et méthode avec Codex](CREDITS-FR-EN.md). Code original [MIT](LICENSE), droits tiers séparés. Le ZIP créé avec votre ROM est personnel ; ne le republiez pas.
