# Zentrales Profil – Vertragsentwurf

Die profile-sync-v1-Dateien beschreiben einen noch nicht implementierten
v0.9-Transportentwurf. local-profile-v2.schema.json beschreibt dagegen das
bereits implementierte lokale v0.8-Dateiformat.
Maßgeblich für die Fachregeln ist ../central-profile-model.md. Keine Dateien
enthalten echte Accountdaten, Schlüssel oder Hashes von Spielinhalten.
Die wiederholten a-/b-Digests sind ausschließlich Schema-Platzhalter.

- profile-sync-v1.schema.json: privater Snapshot, Offline-Fortschrittsstapel,
  Synchronisierungsergebnis und Fehlerantwort. Keine öffentliche Profilsicht.
- native-career-reference-v0.8.json: aus aktuellem nativen Quelltext gelesene
  Konstanten/Ränge mit Quellenhash; noch kein vollständiger Regelalgorithmus.
- examples/: ein Offline-Frag aus einem abgebrochenen Spielabschnitt ohne weitere
  Punktvergaben oder neue Achievement-
  Stufe: 100 → 102 Punkte. Der Beleg ist im Snapshot bereits enthalten.

Das Schema prüft ausschließlich Struktur und Typen. Es beweist keine
Authentifizierung, reale Spielausführung, Inhaltsberechtigung oder Plausibilität.
Einzigartige/geordnete Ordinals, Offset ≤ Dauer, Ereignistypen pro Modus,
Match-/Teilnehmer-Dubletten, Atomicität und Punktgleichheit sind separat zu prüfen.

Ereignisarten und Granularität sind vor Implementierung gegen den vollständigen
nativen Erfassungspfad zu prüfen; insbesondere Achievement-Auswertung und
Bonus-Rundung dürfen nicht aus einem neuen Aggregat frei rekonstruiert werden.
Statistik-/Modusfelder des Snapshots sind bewusst nur eine minimale Auswahl.
Weitere bestehende Felder müssen kontrolliert und versioniert ergänzt werden.
Größen-/Ratenlimits und das genaue kanonische Digest-Verfahren sind noch offen.
Berechtigungen/Garage verwenden einen getrennten Vertrag; garageRevision ist
hier nur ein Verweis und keine Freigabe zum Nutzen eines Assets.

Der Client darf keine eigenen Punkte, Ränge, Auszeichnungen oder Berechtigungen
in den Fortschrittsstapel einfügen. claimedSource ist eine Behauptung, keine
verifizierte Herkunft und keine automatische Berechtigung zum Online-Bonus.

second-device-progress.json startet wie das erste Gerät bei Revision 10. Sein
anderer Matchbeleg erhöht den bestätigten Stand von 102 auf 104, ohne den
älteren lokalen Stand zu überschreiben oder die 100 Basispunkte zu addieren.
Die Beispiele sind abgebrochene Abschnitte mit je einem Frag; eine reguläre
Niederlage oder ein Achievement würde zusätzliche native Regeln auslösen.


local-profile-v2.schema.json und examples/local-profile-v2.json gehören zur
v0.8-Dateistruktur, nicht zum v0.9-Sync-Protokoll. Die Datei enthält lokale
Identität, lokalen Fortschritt und ausschließlich eine unverbundene Web-Referenz.
Details zur einmaligen Umstellung: ../profile-format-v08.md.
