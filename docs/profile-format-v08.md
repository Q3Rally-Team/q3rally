# Lokale Profilstruktur für v0.8

Implementiert am 2026-11-02. Web-Anmeldung, Verknüpfung und Synchronisierung
bleiben v0.9-Arbeit. Kein neuer Dienst wird für v0.8 vorausgesetzt.

## Dateiformat 2

Profile liegen weiterhin unter `profiles/<Dateiname>.json`. Der Dateiname und
`profile_active` bleiben nutzbar. Inhalt:

- `profileVersion: 2` kennzeichnet das Dateiformat, unabhängig von statsRevision.
- `identity`: vorhandene UUID und Anzeigename, persönliche Angaben und vier
  Fahrzeugfavoriten. Die UUID wird nicht durch eine Web-Account-ID ersetzt.
- `progress`: `epoch: v0.8-local-1`, `authority: local`, aktueller/höchster Rang
  und Statistikblock einschließlich Fahrzeughistorie. Dies sind lokale Werte,
  kein vorgetäuschter zentral bestätigter Profilcache.
- `webLink`: `state: unlinked`, `accountId: null`, `profileId: null`. Es werden
  weder Accountdaten erfunden noch Zugangsdaten in die Profildatei geschrieben.

Ein vollständiges Beispiel liegt in `contracts/examples/local-profile-v2.json`,
das separate Dateischema in `contracts/local-profile-v2.schema.json`. Der
profile-sync-v1-Entwurf beschreibt erst für v0.9 vorgesehene Nachrichten.

## Umstellung bestehender Profile

Beim ersten Laden eines alten Profils bleiben UUID, Anzeigename, Geschlecht,
Geburtsdatum, Avatar, Land und Fahrzeugfavoriten erhalten. Punkte, Ränge,
Statistiken und alte Fahrzeugnutzungszeiten starten neu. Damit gehen auch
fortschrittsabhängige Rangfreischaltungen auf ihren Anfangszustand zurück.
Die bisherige native Punkteberechnung selbst wurde nicht verändert.

Das Menü schreibt die Umstellung, sobald Identität und Statistikblock gemeinsam
mit gültiger UUID geladen werden. Das Spiel markiert eine geladene Legacy-
Identität zum Speichern. Der Reset findet nicht bei jedem Laden eines bereits
umgestellten Profils statt. Bei fehlender/ungültiger UUID greift weiterhin die
vorhandene UUID-Wiederherstellung beziehungsweise Generierung.

Menüänderungen und laufendes Spiel verwenden denselben Header und dieselbe
Formaterkennung. Menüänderungen bewahren die Fahrzeughistorie. Auf Spielseite
bleiben Anzeigename und neu erreichte Ränge beim Speichern erhalten.

Unbekannte Versionen, andere Epochen, bereits zentral verwaltete/verknüpfte
Dateien, beschädigte oder zu große Dateien werden abgewiesen und nicht durch
v0.8 überschrieben. Die Größenbegrenzung beträgt 32768 Bytes einschließlich
Abschlussnull. Auch bei einer Erweiterung der Statistikfelder wird vor einem
Schreibzugriff geprüft, ob der vollständige JSON-Inhalt hineinpasst.
Es gibt keine neue Backup-/Cloud-Sicherung. Wer alte Punktestände archivieren
möchte, kopiert den bisherigen Profilordner vor dem ersten Öffnen mit v0.8.
Die automatisierten Tests benutzen ausschließlich synthetische Profile.

Alte Spielversionen müssen eigene Profilverzeichnisse verwenden. Eine alte
Version kann die neue Datei sonst wieder in ihr altes Format umschreiben.
Die getrennten Versionsverzeichnisse sind Teil des geplanten Launchers.

## Benutzerführung

Profilwähler und Abschluss der Profilerstellung zeigen den nicht verknüpften
Webstatus mit Hinweis auf v0.9. Die Profilerstellung endet nach Name und
persönlichen Angaben. Der automatische Offline-Key-Registrierungsdialog wird
nicht mehr angeboten; er ist keine Web-Anmeldung. Die v0.9-Verknüpfung muss
später über die Website bestätigt werden.

## Prüfung

- Produktionstests rufen die echten Menü-Lese-/Schreibfunktionen und die
  gemeinsame Formatimplementierung mit isoliertem VFS auf: Identität,
  einmaliger Reset, aktuelle/negative Punkte, Fahrzeughistorie, mehrdeutige
  Version, unbekannte/verknüpfte Dateien und Puffergrenzen sowie alle Statistikfelder und lange Favoriten. Sechs Tests bestehen
  unter Windows und Linux.
- Native Spielmodule und QVMs bauen unter Windows und Linux.
- Linux/WSLg-Laufzeittest mit altem Profil: GL1 und GL2, Country01,
  Renderer-Neustart, Profile/UUID/Land und neues Dateiformat bestehen.
- Ein zweiter Lauf mit bereits umgestelltem Profil prüft den Erhalt neuen
  Fortschritts. Ergebnisse unter `output/profile-structure-v08/`.
- macOS wurde für diese Änderung nicht erneut gebaut oder auf Hardware geprüft.

Der frühere Dedicated-only-/Backend-Prototyp wurde aus Engine und Diensten
entfernt. Das vorherige Backend-Verhalten ist wiederhergestellt; die lokale
Profilstruktur bleibt erhalten. Es wurde nichts produktiv hochgeladen oder
zurückgesetzt. Historische Prototypdateien liegen nur unter output/retired-ladder-prototype.
