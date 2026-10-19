# Laboratoire du 19 octobre 2026 / 19 October laboratory

Ces rapports sont ceux de la campagne bêta1/C1, conservés avec leurs numéros et SHA-256 d'origine. Ils ne sont pas présentés comme une nouvelle campagne complète sous le numéro bêta2. La [preuve d'équivalence](../C1-TO-BETA2-PROVENANCE.json) montre que C1 et bêta2 ne diffèrent que par deux octets du numéro affiché. Les [essais finaux bêta2](../AUTOMATED-TESTS.json) sont séparés.

These reports retain their original beta1/C1 identities and ROM hashes. They are not represented as a repeated full beta2 campaign. The [equivalence proof](../C1-TO-BETA2-PROVENANCE.json) shows only two display-branding bytes differ. [Final beta2 checks](../AUTOMATED-TESTS.json) are recorded separately.

| Rapport / Report | Contenu / Contents |
| --- | --- |
| [summary.json](summary.json) | Synthèse globale C1 / C1 overview |
| [timing-summary.json](timing-summary.json), [timing-samples.json](timing-samples.json) | 6 048 échantillons timers ; coûts isolés / 6,048 isolated timer samples |
| [c1-memory-summary.json](c1-memory-summary.json) | Pile, ABI, canaris, équivalence du moteur / Stack, ABI, canaries, runtime equivalence |
| [save-fault-summary.json](save-fault-summary.json), [save-fault-results.json](save-fault-results.json) | Défauts reproduits, réessais et coupures modélisées / Reproduced faults, retries and modeled power cuts |
| [save-fault-mgba.json](save-fault-mgba.json) | Erreurs et reprise mGBA / mGBA errors and recovery |
| [native-stress-c1.json](native-stress-c1.json) | 24 démarrages à froid, 8 sauvegardes / 24 cold starts, 8 native saves |
| [gameplay-results.json](gameplay-results.json) | Cadence callback2, trois scènes ; pas les FPS du PC / Callback2 cadence across three scenes, not host FPS |
| [gba-target-profile.json](gba-target-profile.json), [local-core-profile.json](local-core-profile.json) | Paramètres GBA et cœur local / GBA target and local core |
| [strict-profile-probe.json](strict-profile-probe.json) | Contrôle des cycles et images PPU / Cycle and PPU calibration |
| [mgba-primary-sources.json](mgba-primary-sources.json) | Sources officielles épinglées / Pinned primary sources |
| [PROVENANCE.json](PROVENANCE.json) | Empreintes et remplacement des chemins privés / Hashes and local-path redaction |

Le BIOS est HLE ; le contrôleur SD/SuperCard et le trajet SuperFW physique ne sont pas simulés. Les piles des pilotes natifs sont exclues des mesures propres au mod. Le modèle de coupure illustre un risque et ne garantit pas l'atomicité. / BIOS is HLE; physical SD/SuperCard and firmware execution are not simulated. Native-driver stack use is excluded from mod-only figures. Power-cut models illustrate risk and do not guarantee atomic writes.
