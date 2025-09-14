# Q3Rally Docker Readme

## English

This Docker image allows you to run **Q3Rally** easily without manual installation or compilation.

### Pull the latest image
```bash
docker pull registry.gitlab.com/manalejandro/q3rally-docker/q3rally-docker:latest
```

### Run the container (basic example)
```bash
docker run --rm -it registry.gitlab.com/manalejandro/q3rally-docker/q3rally-docker:latest
```

This will start Q3Rally inside the container.  
You can adapt the command with volume mounts or network options depending on whether you want to run a server or client.

### Run a dedicated server
Example (exposing port 27960 for Quake III servers):
```bash
docker run --rm -it -p 27960:27960/udp registry.gitlab.com/manalejandro/q3rally-docker/q3rally-docker:latest +set dedicated 1 +exec server.cfg
```

This starts Q3Rally as a dedicated server with configuration file `server.cfg`.

---

## Deutsch

Dieses Docker-Image ermöglicht es dir, **Q3Rally** einfach auszuführen, ohne es manuell installieren oder kompilieren zu müssen.

### Neueste Version herunterladen
```bash
docker pull registry.gitlab.com/manalejandro/q3rally-docker/q3rally-docker:latest
```

### Container starten (einfaches Beispiel)
```bash
docker run --rm -it registry.gitlab.com/manalejandro/q3rally-docker/q3rally-docker:latest
```

Damit startet Q3Rally im Container.  
Den Befehl kannst du anpassen, z. B. mit Volume-Mounts oder Netzwerkoptionen, je nachdem ob du einen Server oder Client betreiben möchtest.

### Dedizierten Server starten
Beispiel (Port 27960 für Quake III Server freigeben):
```bash
docker run --rm -it -p 27960:27960/udp registry.gitlab.com/manalejandro/q3rally-docker/q3rally-docker:latest +set dedicated 1 +exec server.cfg
```

Damit startet Q3Rally als dedizierter Server mit der Konfigurationsdatei `server.cfg`.
