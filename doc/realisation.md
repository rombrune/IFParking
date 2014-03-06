## Document de réalisation

### Avancement

Le programme rendu réalise l'ensemble des fonctionnalités comprises dans le cahier des charges. Bien qu'elles soient satisfaisantes dans le cadre d'une utilisation normale, on pourrait mesurer précisément les performances de l'application et travailler à les améliorer. Un autre axe d'amélioration serait d'améliorer la généricité, en permettant par exemple l'utilisation d'un nombre arbitraire de barrières d'entrée et de sortie.

### Retour sur l'architecture

Après le rendu du schéma d'architecture générale de l'application multitâche, nous avons apporté des modifications mineures :

- Nous avons précisé le contenu de la mémoire partagée. Elle contient :
  - L'état du parking (l'ensemble des voitures garées à leur place respective)
  - Le nombre de places libres
  - Le nombre de requêtes d'entrée en cours et les requêtes elles-mêmes.
- Nous avons supprimé le sémaphore à compte `placesLibres`, car cette donnée fait maintenant partie de la mémoire partagée.
- Nous avons mis en place un canal de communication anonyme entre les tâches `GestionClavier` et `Sortie`. En effet, le seul signal spécifié dans le schéma initial ne permettait pas la communication nécessaire d'information entre ces deux tâches.
- Nous avons précisé le mécanisme de destruction hiérarchique, qui n'apparaissaient pas clairement sur le schéma initial.
- L'implémentation étant réalisée en anglais, nous avons traduit le nom des tâches et des ressources en conséquence : `Entree` devient `Entrance`, `Sortie` devient `ExitGate`, etc.