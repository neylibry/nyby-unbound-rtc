# Texte français — pour une communauté qui autorise ce type de publication

**Une horloge pour Unbound sur vraie GBA sans puce RTC — NyBy v1.1RC1**

Ma SuperCard n'a pas de puce RTC. J'ai donc fait développer un petit ajout
qui demande la date et l'heure au démarrage, puis les fait avancer pendant
le jeu. Il garde le dernier réglage saisi pour faciliter la prochaine saisie.
Le temps console éteinte n'est pas compté : il faut remettre l'heure au lancement.

Le kit gratuit prend en charge Unbound 2.1.1.1 anglais et la traduction
française Antony 2.1.179, avec SuperFW 0.21. Il contient les patchs BPS,
un installateur hors ligne, les guides FR/EN et le code original sous MIT.
Il ne contient aucune ROM ni sauvegarde personnelle.

Téléchargement et instructions : https://github.com/neylibry/nyby-unbound-rtc/releases/tag/v1.1-rc1-community-r4

Ordre : FireRed USA v1.0 personnel → Unbound → traduction FR si souhaitée
→ patch RTC correspondant. L'installateur NyBy fait seulement la dernière étape.

J'ai testé la FR sur ma GBA : sauvegarde/reprise fonctionnelles et jeu fluide.
La version EN a passé les tests PC et attend un essai sur console. Un souci
intermittent d'affichage des choix dans les paramètres reste signalé ; c'est
une version candidate RC1. Les règles temporelles natives du jeu restent actives.

Des parties importantes du code et de la documentation ont été générées avec Codex (IA), sous la direction de NyBy. NyBy a défini le besoin et effectué les essais sur sa GBA. Les tests matériels rapportés concernent la FR ; l'EN reste à confirmer sur console.

Merci à Skeli et l'équipe Unbound, Antony pour la traduction et davidgfnet
pour SuperFW. Cet ajout est indépendant de leurs projets.
