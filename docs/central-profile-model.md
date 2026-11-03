# Zentrales Profil und Synchronisierung

Stand: 2026-11-02. Architekturentwurf nach der Entscheidung des Nutzers;
noch keine implementierte Account-/Sync-API. Diese Datei ist für die weitere
Planung maßgeblich gegenüber dem früheren Dedicated-only-Ladder-Entwurf.

## Releaseumfang nach der letzten Nutzerentscheidung

**v0.8-Dateistruktur umgesetzt:** gemeinsames lokales Dateiformat 2 in Menü
und Spiel; Legacy-Identität bleibt, alter Fortschritt wird zurückgesetzt,
Webstatus bleibt unlinked. Details und Prüfstand: profile-format-v08.md.

**v0.8: ausschließlich die neue Profilstruktur als Grundlage.** Identität,
Fortschrittsdaten und die spätere Web-Verknüpfung werden sauber getrennt und
versioniert. Lokale Profile bleiben bis zur tatsächlichen zentralen Anbindung
benutzbar. Die Umstellung darf keinen nicht vorhandenen Login-/Sync-Dienst
voraussetzen oder einen lokalen Wert als zentral bestätigt ausgeben.

**v0.9: zentrale Anmeldung/Verknüpfung und die übrigen Profil-/Web-Funktionen.**
Die Anmeldung erfolgt auf der Website. Das Spiel erhält über eine bewusst
bestätigte Geräte-/Profilverknüpfung Zugang zum zentralen Webprofil; ein
Spielprofil oder seine UUID ist kein Web-Login. Zentraler Abruf, Offline-Sync,
Geräteverwaltung und Garage-/Arcade-Anbindung sind nicht Teil des v0.8-
Strukturumbaus. Die nachfolgenden Verträge beschreiben dieses Zielsystem,
keine zusätzliche v0.8-Pflicht. Die bereits vereinbarte Launcher-Arbeit wird
durch diese Begrenzung der Profilfunktionen nicht automatisch verschoben.

Alte Punkte, Statistiken und sonstiger Fortschritt müssen nicht migriert werden;
ein vollständiger Fortschritts-Reset ist zulässig. Vorhandene Profile sollen
möglichst erhalten bleiben: lokale Profil-ID/UUID, Name und persönliche
Darstellungsdaten werden vom zurücksetzbaren Fortschritt getrennt. Alte lokale
UUIDs werden nicht automatisch zu Web-Account-IDs. Eine spätere Verknüpfung
benötigt die Website-Anmeldung und ausdrückliche Bestätigung.

Ein Backup und eine gezielte Auswahl der Fortschrittsdaten bleiben sinnvoll;
„Flush“ ist keine Anweisung, jetzt vorhandene Profilidentitäten oder produktive
Daten zu löschen. Bis zum vereinbarten Wechsel erfolgt kein produktiver Reset.
Eine aufwendige Übernahme historischer Punktestände ist ausdrücklich kein
Blocker mehr.

## Verbindliche Richtung

Die Website beziehungsweise ihr Profildienst verwaltet das maßgebliche Profil.
Der native Client hält einen lokalen Cache und kann ohne Verbindung spielen.
Es gibt einen bestätigten nativen Karrierepunktestand, keinen zusätzlichen
nativen Ladder-Punktestand. Die native Ladder zeigt beziehungsweise wertet
diesen zentral bestätigten Fortschritt aus. Arcade-Bestenlisten bleiben davon
getrennt. Garage und kosmetische Berechtigungen gehören zur gemeinsamen Identität.

**Offline-Entscheidung des Nutzers:** reguläre Offline-Partien nach Basisprüfung
übernehmen. Offline-Fortschritt ist grundsätzlich gleichwertiger Karrierefortschritt;
die bestehenden nativen Bonus-/Modusregeln bleiben dabei erhalten. Eine Pflicht
zu überprüfbaren Replays ist nicht vorgesehen. Basisprüfung erkennt Fehler und
Dubletten, beweist aber keine unveränderte Spielausführung. Diese begrenzte
Manipulationssicherheit wird bewusst akzeptiert.

Vor v1.0: sichtbar gekennzeichneter Testbetrieb; Änderungen und angekündigte
Resets sind möglich. Ab v1.0 bleibt die Punkteberechnung unverändert. Neue
Profilränge, Achievements und kosmetische Belohnungen dürfen ergänzt werden,
ohne bestehende Punkte oder die vereinbarte Berechnung rückwirkend umzuschreiben.
Die Behandlung neuer punktwirksamer Achievement-Stufen ist vor dem Freeze
ausdrücklich zu entscheiden (siehe unten).

## Was am bisherigen Entwurf korrigiert werden muss

Der vorherige Prototyp wurde aus den aktiven Engine-/Dienstquellen entfernt;
eine lokale historische Kopie liegt unter output/retired-ladder-prototype.
Er ist **nicht zur Veröffentlichung freigegeben**. Seine Dedicated-only-Sperre, das Ablehnen aller
Offline-Keys und seine neue Matchscore-Formel sind nicht die Zielarchitektur.
Die damaligen Tests beweisen nur die Umsetzung dieses inzwischen überholten
Entwurfs. Sie sind keine fachliche Abnahme für das zentrale Profil.

Versions-/Periodenprüfung, Dublettenschutz und ein Backup vor Resets bleiben
sinnvoll. Sie müssen in das neue Modell übernommen werden. Die bestehenden
v0.8-Profiländerungen bleiben erhalten. Der frühere Backend-Stand ist wieder
aktiv; eine zentrale Anmeldung oder Synchronisierung wird damit nicht vorgetäuscht.

Ein konkreter Unterschied: `G_Profile_AddScore` vergibt bisher +2 für einen
Frag, -1 für einen Tod und -5 für einen Suizid. Der Prototyp würde im DM-Modus
stattdessen die Kill-Anzahl addieren. Das würde die Karriere verändern.
Die Referenz ist deshalb die bisherige native Karriereberechnung, nicht dieser
Backend-Fallback und nicht der reine Matchscore.

Der Referenzbestand mit Punktkonstanten und Rangschwellen liegt in
`contracts/native-career-reference-v0.8.json`. Zusätzlich gehören Modusregeln,
Ereignisreihenfolge und Rundung zum Vertrag. Insbesondere gilt der vorhandene
Online-Bonus von 10 Prozent nur für positive Vergaben und wird pro Vergabe mit
C-float32 und anschließender Ganzzahlkonvertierung berechnet. Einzelvergaben
dürfen nicht stillschweigend zu einer Summe zusammengezogen werden.

## Datenhoheit und Identität

| Daten | Maßgebliche Stelle | Verhalten des Clients |
|---|---|---|
| Account, verknüpfte Geräte, Zugang und Sperren | Identity-Dienst | Lokale Geräteanmeldung; keine Anmeldung nur per UUID |
| Bestätigte Karrierepunkte und Statistiken | Profildienst | Letzten bestätigten Stand zwischenspeichern |
| Rang und bestätigte Achievements | Versionierte zentrale Definitionen und Profildaten | Anzeigen; Offline-Vorschau klar als vorläufig markieren |
| Garage, erworbene und freigeschaltete Cosmetics | Gemeinsamer Berechtigungsdienst | Geprüfte Manifest-/Berechtigungsdaten cachen |
| Favorisierte Fahrzeuge, Name, Avatar, Land | Profil-Metadaten mit Revision | Bewusste Änderungen synchronisieren |
| Native Ladder | Lesesicht auf bestätigte native Profildaten/Ergebnisse | Kein eigener konkurrierender Karrierepunktestand |
| Arcade-Bestenlisten | Je Arcade-Spiel eigene Ergebniswertung | Keine automatische Umwandlung in native Karrierepunkte |
| Steuerung, Grafik, Audio, Demos, lokale Dateien | Gerät | Keine automatische geräteübergreifende Überschreibung |

`accountId` identifiziert den Web-Account, `profileId` das zugehörige kanonische
Spielprofil, `deviceId` eine widerrufbare Geräteverknüpfung. Zunächst ein natives
Profil pro Account; vorhandene lokale Profile können jeweils getrennt bleiben
oder bewusst verknüpft werden. Account und Profil werden nicht anhand gleicher
Namen zusammengeführt. Die vorhandene lokale UUID ist nur ein Legacy-Verweis,
kein Eigentumsnachweis und kein Zugangsschlüssel.

Verknüpfung: Client fordert einen kurzlebigen Einmalcode an, der Nutzer bestätigt
das Gerät im angemeldeten Web-Account, das Gerät erhält eine widerrufbare
Berechtigung für genau sein Profil. Öffentliche Server bekommen keine dauerhaften
Account-/Geräteschlüssel. Für Online-Teilnahme sind kurzlebige, zweckgebundene
Teilnahmenachweise vorzusehen. Öffentliche Profilantworten enthalten keine
Gerätekennungen, E-Mail-Adressen, Geburtstage oder Zugangsdaten.

## Drei Versionskennungen mit verschiedenen Aufgaben

- `careerEpoch`: Wertungsperiode des bestätigten Karrierefortschritts; ändert
  sich bei einem ausdrücklichen Reset. Alte Warteschlangen bleiben dieser alten
  Periode zugeordnet und werden nicht nachträglich umbenannt.
- `scoreRulesId`: unveränderliche Kennung der Berechnungsregeln. Sie ist nicht
  identisch mit der Spielversion; spätere kompatible Releases dürfen dieselben
  eingefrorenen Regeln verwenden.
- `profileRevision`: monoton steigende Revision des zentralen Profils. Sie
  entscheidet, ob ein Cache aktuell ist, nicht ob ältere Offline-Partien
  grundsätzlich noch synchronisiert werden dürfen.

Rang-, Achievement- und Asset-Definitionen haben zusätzliche eigene Versionen.
Eine neue Spielversion erzwingt damit nicht automatisch einen Reset.

## Lebenszyklus im Client

1. Online anmelden/verknüpfen und den bestätigten Profilsnapshot laden.
2. Snapshot und Synchronisierungsstatus atomar speichern. Gerätezugang und
   Spielprofile getrennt halten; private Dateien nicht ins Release aufnehmen.
3. Beim Spielen Ereignisse und matchbezogene Fakten dauerhaft in eine lokale
   Ausgangswarteschlange schreiben. Keine kumulierten Karrierewerte als
   vermeintlich maßgebliches Profil hochladen.
4. Bei fehlender Verbindung weiter spielen. Der letzte bestätigte Stand bleibt
   erhalten; neue Vergaben erscheinen als vorläufig, beispielsweise
   „102 Punkte · Fortschritt noch nicht synchronisiert“. Es entsteht kein
   zweites dauerhaftes Punktekonto.
5. Nach Wiederverbindung Einträge prüfen lassen. Angenommene Fakten werden mit
   den geltenden Regeln zentral verbucht; der Dienst liefert Belege und einen
   neuen Snapshot. Ablehnungen und ihre Gründe bleiben sichtbar.
6. Cache und Belege gemeinsam dauerhaft speichern. Erst danach bestätigte
   Warteschlangeneinträge entfernen. Ein Absturz oder Timeout darf keine
   Doppelvergabe und keinen stillen Verlust verursachen.

Antworten müssen vor jeder Cache-Änderung zum aktiven Account, Profil, zur
Anfrage und zur Epoche passen. Eine verspätete Antwort für ein inzwischen
gewechseltes Profil darf nicht in das neue Profil geschrieben werden.

Ein Journal muss auch Abstürze und abgebrochene Partien abbilden. Schon bisher
können Frag-/Statistikvergaben vor dem Matchende lokal gespeichert werden.
Solche Abschnitte werden als `abandoned` statt als regulär beendet abgeschlossen;
Gewinn-, Verlust- und Abschlussvergaben müssen dabei exakt den nativen Regeln
folgen. Ein Abbruch darf nicht pauschal alle vorher erspielten Vergaben löschen
oder einen zusätzlichen abgeschlossenen Sieg erzeugen. Die genaue Abbildung
gehört zum Referenzvergleich, nicht zu einer neuen Punkteformel.

Mit mehreren Geräten werden weder Summen überschrieben noch einfach die
höchsten lokalen Zähler übernommen. Beispiel: Gerät A und B starten bei
100 Punkten und erspielen unabhängig je einen Offline-Frag. Ohne weitere
Vergaben ergibt das zentral 104, nicht 102 oder 204. Die veraltete Basisrevision
des zweiten Geräts ist dabei kein Ablehnungsgrund.

Ein Gast kann ohne Account spielen. Das Verknüpfen darf seine bisherigen
Dateien nicht löschen. Ob ein vor der Verknüpfung erzeugtes Journal nachträglich
übernommen werden darf, ist eine gesonderte Migrationsentscheidung; es wird
nicht automatisch ein alter kumulierter Punktestand importiert.

## Aufnahme und Prüfung eines Fortschritts

Der Transportentwurf liegt in `contracts/profile-sync-v1.schema.json` mit
Beispielen unter `contracts/examples/`. Er ist keine bereits erreichbare API.

| Vorgang | Vorgesehener Vertrag |
|---|---|
| `GET /profile/v1/me` | Privater kanonischer Snapshot mit Revision und Epoche |
| `POST /profile/v1/progress` | Authentifizierter Stapel matchbezogener Ereignisse; Antwort mit Einzelbelegen und neuem Snapshot |
| `PATCH /profile/v1/me` | Ausschließlich erlaubte Metadaten, mit erwarteter Revision; Konflikt neu laden/auflösen |
| Geräteverknüpfung und Widerruf | Eigene Identity-Endpunkte; konkrete Authentifizierung ist vor Implementierung festzulegen |
| Öffentliche Profil-/Ladderansicht | Ausschließlich freigegebene Felder aus bestätigten Daten |

Eine Fortschrittsmeldung enthält Profil, Gerät, Epoche, Regelkennung,
Spiel-/Content-Version, Match-/Sitzungskennung, Ereignisreihenfolge und Fakten.
Punktbeträge, Ränge, Achievement-Auszeichnungen und Freischaltungen werden nicht
vom Client vorgegeben. Die zuständige Regelimplementierung vergibt sie.

Basisprüfung umfasst:

- Berechtigung für das betroffene Profil; kein Schreiben für beliebige UUIDs.
- Unterstützte Spiel-/Content-Version, passende Epoche und Regelkennung.
- Vollständige Struktur, Wertebereiche, Ereignistypen und Zeit-/Modusbezug;
  nachvollziehbare Dauer, Reihenfolge und Abschluss eines Matches.
- Plausibilität und Raten-/Größenbegrenzungen nach messbaren Spielgrenzen;
  keine ungeprüften frei erfundenen Grenzwerte als neue Spielregeln.
- Einmalige Verarbeitung derselben Matchteilnahme und Ereignisse, auch wenn
  Client und Dedicated Server dasselbe Match melden.

Ein Ereignisbeleg referenziert `(profileId, careerEpoch, matchId)`, den
kanonischen Inhaltsdigest und den Bewertungsstatus. Server und Client müssen
dafür dieselbe Matchkennung kennen; diese Übermittlung fehlt heute noch.
Gleiche Kennung/gleiche Fakten liefert denselben Beleg. Gleiche Kennung mit
geänderten Fakten ist ein Konflikt, keine zweite Wertung. Ein Client darf
ein serverseitig gemeldetes Online-Match nicht als neues Offline-Match umetikettieren.
Ohne vertrauenswürdigen Ausführungsnachweis bleibt der vereinbarte begrenzte
Manipulationsschutz bestehen.

Belege müssen dauerhaft abrufbar sein; ein verlorenes HTTP-Ergebnis wird durch
Wiederholung mit derselben Kennung aufgeklärt. Eine unvollständige Antwort
bestätigt nichts durch bloße Abwesenheit. Der Snapshot kann ältere Belege bereits
enthalten: ein Beleg ist niemals eine Aufforderung, seinen Punktbetrag zusätzlich
auf den gelieferten Snapshot zu addieren.

Fortschritt, Achievement-Vergaben, kosmetische Berechtigungen, Belege und
Profilrevision werden pro angenommenem Match in derselben zentralen Transaktion
verbucht. Ausschließlich eine Stelle entscheidet über bestätigten Fortschritt.
Eine rückwirkende fachliche Korrektur bekommt einen eigenen Verwaltungsbeleg;
ein älterer Cache wird nicht allein wegen niedrigerer Punkte verworfen.

## Offline- und Online-Ergebnisse ohne doppelte Wertung

Offline- und Listen-Server-Ergebnisse sind zulässige Quellen. Dedicated-only
ist keine Produktregel. Ein Gerät darf Ergebnisse für sein eigenes Profil
einreichen; ein berechtigter Gameserver darf seine authentifizierten Teilnehmer
melden. Beide Eingänge benutzen dieselbe zentrale Prüfung und Verarbeitung.
Ein bloßes `source=dedicated` darf nicht unkontrolliert Bonus oder Rechte erteilen.
Die Herkunftsprüfung für den bestehenden Online-Bonus ist noch konkret festzulegen.
Unbekannte Herkunft ist kein Grund, alle regulären Offline-Partien zu verbieten.

Die bisherige Punkteberechnung wird mit Referenzfällen übertragen, einschließlich
Strafen, Modi ohne Rangfortschritt, Platzierungen, Rundung und Achievement-Boni.
Ankunftsreihenfolge mehrerer Geräte darf dieselbe Achievement-Stufe nicht doppelt
vergeben. Welche Ereignisgranularität nötig ist, um insbesondere Bonus und
Achievement-Auswertung exakt zu reproduzieren, ist ein technischer Prüfpunkt.
Solange diese Gleichheit nicht nachgewiesen ist, ersetzt kein neuer Backend-
Algorithmus die bisherige Karriereberechnung.

## Garage und Achievements

Ein Asset-Download beweist keinen Besitz. Berechtigungen werden zentral erteilt,
versioniert und mit signierten Manifesten lokal geprüft. Einträge aus einem
lokal veränderten Profil geben keine neuen zentralen Nutzungsrechte frei.
Offline bleiben bereits bestätigte Berechtigungen gemäß Cache-Regeln nutzbar;
neu erspielte zentrale Belohnungen gelten bis zur Synchronisierung als vorläufig.
Widerruf und Offline-Cache-Gültigkeit benötigen eine ausdrückliche Produktregel.

Heute vergibt jede neue native Achievement-Stufe +8 Karrierepunkte. Die
zulässige Erweiterung nach v1.0 muss deshalb vor dem Einfrieren präzisiert werden:
Sollen neue native Stufen weiterhin nach der festen +8-Regel zählen dürfen,
oder sollen neue Achievements ausschließlich kosmetisch bleiben? Beides ist
technisch möglich; diese Detailentscheidung ist noch offen. Bestehende Vergaben
bleiben bis dahin erhalten. Neue Arcade-/Cosmetic-Achievements bekommen eigene
Kennungen und dürfen nicht automatisch die native Punktevergabe auslösen.
Als sichere Voreinstellung für neue, nicht von den eingefrorenen Regeln erfasste
Belohnungstypen schlage ich punktneutrale Freischaltungen vor. Das ist eine
Empfehlung, keine bereits getroffene Produktentscheidung.

## Reset und Migration

Ein Karriere-Reset ändert die Epoche und setzt nur die ausdrücklich benannten
Fortschrittsdaten zurück. Identität, Geräteverknüpfungen und unabhängig erworbene
Cosmetics bleiben erhalten. Der Umgang mit bisherigen rangabhängigen Freischaltungen
und alten lokalen Profilen wird vor dem Wechsel ausdrücklich festgelegt.
Der bisherige Reset-Prototyp darf nicht ungeprüft auf den späteren zentralen
Account-/Garage-Datenbestand angewendet werden.

Ein Client mit altem Cache erhält `epoch_mismatch`, lädt den neuen Snapshot und
legt alte vorgemerkte Einträge nachvollziehbar ins Archiv. Er benennt sie nicht
um und stellt zurückgesetzte Punkte nicht wieder her. Kein Reset wird aus einer
kleineren Profilrevision, einer anderen Geräteuhr oder einem Netzwerkfehler abgeleitet.

Vorhandene Profile werden zuerst gesichert und inventarisiert. Eine bestehende
UUID allein erlaubt keine Übernahme eines fremden Profils. Legacy-Zuordnung,
eine mögliche einmalige Übernahme und die Reichweite eines v0.8-Resets brauchen
einen ausdrücklichen Migrationsplan. Keine stille Zusammenführung oder Löschung.

## Architektur und Umsetzung in Schritten

Das Web-Arcade-Konzept plant einen gemeinsamen Identity-/Garage-Dienst mit
PostgreSQL. Das zentrale Profil wird an diese Identität angebunden. Die vorhandene
PHP-Ladder kann ihre Website-/Leseaufgaben behalten und bestätigte Daten als
Lesesicht erhalten. Ein notwendiger Übergangsadapter darf aber nicht nebenher
weiter einen zweiten maßgeblichen Punktestand aus lokalen Snapshots bilden.
Eine PHP-Übergangslösung ist möglich; Dienststandort und Authentifizierungsstack
sind noch nicht technisch entschieden. Ein vollständiger Website-Neubau ist
keine Voraussetzung für den ersten Sync-Prototyp.

1. **Vertrag und Referenz sichern:** dieses Modell, Transportentwurf und native
   Berechnungsreferenz; offene Migrations-/Berechtigungsregeln entscheiden.
2. **Getrennter Entwicklungs-PoC:** ein verknüpftes Profil, zwei Geräte,
   kanonischer Snapshot, dauerhafte Belege, Offline-Warteschlange; künstliche
   Ereignisse und vollständig isolierte Testdaten.
3. **Native Anbindung:** Fakten erfassen, gemeinsame Matchkennungen, sichere
   Cache-/Journaldateien und passende Client-Anzeige; Punktgleichheit prüfen.
4. **Ladder-Adapter und Migration:** eine einzige schreibende Instanz, alte
   Endpunkte kontrolliert ablösen, Backup/Reset/Übernahme getrennt testen.
5. **Launcher:** Versionsauswahl, getrennte Legacy-Verzeichnisse und Geräte-
   verknüpfung einbinden. Der Launcher selbst entscheidet keine Karrierepunkte.
6. **Spätere Web-Erweiterungen:** gemeinsame Garage, Skin Studio, Arcade-
   Belohnungen und deren separate Bestenlisten nach dem Web-Arcade-Plan.

Vorgesehene Abnahmeszenarien: zwei Geräte bei gleichem Startstand, gleichzeitige
Online-/Offline-Meldung desselben Matches, Timeout nach Commit, erneuter Upload,
veraltete Basisrevision, widerrufenes Gerät, falsches Profil, Reset während
Offline-Spiel, manipulierte Summen, Reihenfolge und Rundung der Punktvergaben,
einmalige Achievements und Berechtigungen sowie Backup/Wiederherstellung.

## Abgrenzung des Arbeitsstands

Das Dokument und der validierbare Transportentwurf definieren den nächsten
Entwicklungsschritt. Account-Verknüpfung, zentraler Punktestand, Offline-Sync,
signierte Berechtigungen und eine ersetzte native Anzeige sind noch nicht
implementiert. Der Dedicated-only-Prototyp wurde aus den aktiven Quellen entfernt;
keine produktiven Dienste oder Profile wurden verändert.


## Quellen für diesen Entwurf

- Nutzerentscheidung: zentrales Profil auf der Website; Offline-Partien nach
  Basisprüfung übernehmen, mit bewusst begrenztem Manipulationsschutz.
- Q3Rally_Web_Arcade_Zwischenstand_2026-11-02.docx: gemeinsame Identität/Garage,
  kosmetische Verknüpfung und getrennte Arcade-Bestenlisten; technische Abnahme
  dort noch offen. Die Dokumentinhalte werden als Projektkontext verwendet,
  nicht als Erlaubnis zur Umsetzung aller Web-Arcade-Funktionen in diesem Schritt.
- engine/code/game/g_profile.c und profile_shared.h: native Punktvergaben,
  Online-Bonus, Rangschwellen und bisheriges lokales Profil.
- engine/code/game/bg_achievements.c sowie
  engine/code/q3_ui/ui_rally_profiles.c: vorhandene Achievement-/Freischaltungsbasis.


Transportprüfung dieses Entwurfs: JSON Schema Draft 2020-12 ist gültig;
sechs Beispielnachrichten bestehen die Strukturprüfung, neun bewusst ungültige
Varianten werden verworfen. Negative Karrierepunktestände bleiben zulässig,
entsprechend der nativen Referenz. Der Quellenhash der extrahierten nativen
Referenz stimmt mit dem unveränderten Quelltext überein. Das ist keine
Laufzeit-/Authentifizierungsabnahme. Prüfprotokoll:
`output/profile-sync-design/validation.json`.
