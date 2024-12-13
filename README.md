# Plan Agricole Connecté

## Raspberry Pi

- Tous les fichiers du serveur flask sont dans le dossier `roc_proj`.

Ce dossier peut être envoyé sur la Raspberry Pi de la manière suivante :

```bash
scp -r roc_proj/ rpi-roc@<ip_addr>:/home/rpi-roc/
```

- Les paquets pip dont le projet serveur sont inscrits dans `rpi_deps/requirements.txt`

- Enfin, il est possible de démarrer le serveur dès le démarrage via le fichier `roc_proj.service` qui peut être placé sur la Pi via la commande suivante :

```bash
scp roc_proj.service rpi-roc@<ip_addr>:/etc/systemd/system/
```

- Activer le service depuis le terminal de la Pi :

```bash
sudo systemctl daemon-reload && sudo systemctl enable --now roc_proj.service
```

## Arduino

Le code des Arduino est situé sous le dossier `Arduino`.
