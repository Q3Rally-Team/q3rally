# Bot-Bestandsaufnahme – 24. Oktober 2026

Untersuchter Stand: `35067eb0` (`New AAS Files`), lokaler Checkout. Umfang: alle 17 benannten Spielmodi aus `gametype_t`, einschließlich Single Player, Sprint, Derby, LCS, Elimination, Teamvarianten und des absichtlich botfreien Ghost Race. Die reservierten Nummern 9–15 sind keine implementierten Modi. Auskommentierte beziehungsweise bedingt kompilierte Quake-/Missionpack-Altmodi wie Tournament, One Flag CTF, Obelisk und Harvester werden nicht als zusätzliche Q3Rally-Modi gezählt.

Das ist eine Code- und Datenbestandsaufnahme mit ausgeführten bestehenden Hilfsfunktionstests. Es wurden keine neuen Match-Benchmarks, Fahrversuche oder CPU-Profile durchgeführt. „Belegt“ bezeichnet einen nachverfolgten Codepfad; beobachtbare Häufigkeit und Ausmaß im Spiel bleiben gegebenenfalls zu messen. Externe Pakete und Daten im Nutzer-Homepath können die tatsächlich geladenen Routen verändern.

## Einschätzung

Ein kompletter Neubau der Bot-Infrastruktur ist derzeit nicht gerechtfertigt. Es gibt verwertbare Wahrnehmung, Waffenwissen, Charakterdaten, Navigation, Routendatenformate und modusspezifische Ansätze. Allerdings treffen diese Bausteine an mehreren Stellen nicht korrekt zusammen. Höherer Skill oder aggressivere Zahlen beheben diese Anschlussfehler nicht.

Empfehlung: zuerst die konkreten Funktionsfehler reparieren und reproduzierbar messen. Anschließend einen gemeinsamen Fahrzeugregler samt Recovery sowie eine direkte, modusspezifische Aufgabenplanung einführen. Die bestehenden Systeme schrittweise an diese beiden Schichten anschließen.

| ID | Modus | Aktive Grundlage und Hauptproblem | Richtung |
|---|---|---|---|
| 0 | Racing | Map/Ghost/Checkpoint; unterschiedlich ausgestattete Fahrpfade, Routendaten fehlen meist | Routen behalten, Fahren/Recovery vereinheitlichen |
| 1 | Racing-DM | Gegnerfindung ausdrücklich deaktiviert; Rennknoten ohne laufende Kampfsteuerung | Rennen und Waffen gleichzeitig planen |
| 2 | Single Player / Time Trial | Allgemeiner Rennbot plus alte Arena-Botverwaltung; reiner Rennmodus nicht durchgängig geschützt | Solo-/Trainingszweck klären, sauberen Rennpfad verwenden |
| 3 | Demolition Derby | Fahrzeug-Roaming plus alte Kampfbewegung; Rammer zählt nicht als Kampfstärke | Eigene Rammplanung und passende Risikobewertung |
| 4 | Last Car Standing | LCS-Risikoheuristik plus Arena-Kampf; Fluchtabsicht kann wieder zum Gegner führen | Überlebensplanung und echte Fluchtziele |
| 5 | Elimination | Allgemeiner Rennbot; keine situationsabhängige KO-Strategie, Kampf-Gates inkonsistent | Runden-/Ausscheidungsdruck im Racing-Planer |
| 6 | Deathmatch | Gegner/Waffen/Versorgung/Kampfphasen; Lenkabsicht und Feuergeometrie inkonsistent | Gemeinsamer Fahrzeugregler und Kampfplanung |
| 7 | Sprint | Rennbot; Checkpoint-Fallback kennt separates Ziel nach letztem CP nicht | Start-/Zielgeometrie für offene Strecken reparieren |
| 8 | Ghost Race | Bots absichtlich serverseitig gesperrt; aufgezeichneter Ghost ist kein KI-Fahrer | Botqualität nicht anwendbar, UI-Gates angleichen |
| 16 | Team Deathmatch | DM plus funktionierende Follow-Gruppen; grobe, träge und slotabhängige Organisation | Räumliche Teamaufträge und Mensch-Unterstützung |
| 17 | Team Racing | Einzelracer mit Teamwertung; keine eigene Team-Rennplanung | Teaminterne Rücksicht und Zeitoptimierung |
| 18 | Team Racing-DM | Renn-/Kampfzustände konkurrieren; Waffenlogik anders als in Racing-DM | Gemeinsamer Race/Combat-Planer mit Teamregeln |
| 19 | CTF | Angriff/Eskorte/Rückweg; Rückholziel falsch und Objective-Vorrang lückenhaft | Weltzustand und Auftragsprioritäten reparieren |
| 20 | CTF4 | Vierteam-Ansätze von Zielausführung/Setup abgekoppelt; Botfüllung nur Rot/Blau | Gesamte Vierteam-Kette anschließen |
| 21 | Domination | Sigilzugriff passt nicht zur Botlib/Daten; keine verbindliche Punktverteilung | Objective-Registrierung und Teamzuweisung erneuern |
| 22 | King of the Hill | Anzeigenposition als Fahrziel; keine ausdrückliche Halteaufgabe in echter Zone | Geprüfte Bodenpunkte und Zone-Halten |
| 23 | Autoball | Rollen/Turbo/lineare Prognose; grobe Steuerung und lokale Teamentscheidungen | Ballprognose und Teamplanung erneuern |

Quellen für Vollständigkeit und Kennungen: [aktives Enum](D:/Q3RallyDev/q3rally_tryout/engine/code/game/bg_public.h:158), [zentrale Modusklassifikation](D:/Q3RallyDev/q3rally_tryout/engine/code/game/bg_misc.c:2318).

## Gemeinsame Architektur

Der zentrale Ablauf lautet:

`BotAIStartFrame → BotAI → BotDeathmatchAI → Team-AI + aktiver Zustandsknoten → BotUpdateInput → Usercommand → Fahrzeugphysik`.

Der Name `BotDeathmatchAI` ist also der gemeinsame Einstieg, auch für Renn- und Teammodi. Die Zustandsmaschine enthält unter anderem Zielsuche, Itemsuche, Kampf, Verfolgung, Rückzug und Rennen. Autoball wird innerhalb der allgemeinen Zielsuche vorzeitig abgewickelt.

Quellen: [AI-Einstieg](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_main.c:1079), [gemeinsame Zustandsmaschine](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:6330), [Autoball-Einhängung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:3200).

Die Entscheidung läuft standardmäßig alle 100 ms, also ungefähr mit 10 Hz. Usercommands und Blickwinkel werden jedes Serverframe aktualisiert. Das ist eine wichtige Trennung: Eine höhere Denkrate kann Reaktionen verbessern, beseitigt aber keine falschen Ziele oder wirkungslosen Lenkbefehle. Für den späteren Umbau sollten strategische Entscheidungen und Fahrzeugregelung getrennte Takte erhalten.

Alle Bots benötigen derzeit initialisiertes AAS, auch die direkt fahrenden Racing-/Autoball-Pfade. Ohne AAS scheitert bereits das Setup beziehungsweise die Frame-Verarbeitung. AAS bleibt für die klassische Navigation nützlich; für eine eigenständig navigierende Arena-KI ist diese harte Abhängigkeit unnötig. Quellen: [Setup-Gate](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_main.c:1294), [Frame-Gate](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_main.c:1616).

### Weitere gemeinsame Anschlussfehler

Die Botlib verwendet in `be_ai_goal.c` noch ein eigenes altes Gametype-Enum mit `GT_TEAM=3`. Das Spiel übergibt unverändert die neuen Nummern, bei denen Teammodi erst ab 16 beginnen. Dadurch behandeln die Itemfilter Derby, LCS, Elimination, Deathmatch und Sprint fälschlich als Teammodus. Bei entsprechend mit `notteam`/`notfree` markierten Items stimmen Botziele und tatsächlich verfügbare Items nicht überein. Die Wirkung ist mapabhängig; ohne solche Markierungen entsteht daraus kein sichtbarer Unterschied. Quellen: [altes Botlib-Enum](D:/Q3RallyDev/q3rally_tryout/engine/code/botlib/be_ai_goal.c:92), [Itemfilter](D:/Q3RallyDev/q3rally_tryout/engine/code/botlib/be_ai_goal.c:879), [unveränderte Übergabe](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_main.c:1743).

Außerdem benutzt die KI an mehreren Übergängen weiterhin feste Listen „reiner Rennmodi“, obwohl `BG_GametypeIsNonDMRace` bereits existiert. Single Player und Elimination ohne Waffen fehlen in diesen KI-Listen. So können Gegnerkampf oder Rückzug noch vor dem Einstieg in den Rennknoten übernehmen. Die zentrale Klassifikation selbst ist sinnvoll und getestet; ihre tatsächliche Verwendung ist noch unvollständig. Quelle: [Gegnerprüfung vor Rennziel](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:3233).

Die Teamleader-Prüfung kontrolliert lediglich, ob der gespeicherte Name noch einem Client gehört. Sie prüft nicht, ob dieser Client weiterhin im eigenen Team ist. Ein Teamwechsel des Leaders kann deshalb die automatische Neuorganisation blockieren. Das ist ein gemeinsames Risiko für die über diesen Leader organisierten Teammodi. Quelle: [Leader-Validierung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_team.c:73).

## Deathmatch und gemeinsame Fahrzeugprobleme

### Belegt: Kampfphasen lenken über den falschen Eingang

`BotRallyCombatMove` besitzt Anfahrt, Feuerphase, Absetzen und Wenden. Seine Lenkfunktion setzt ausschließlich Links-/Rechtsbewegung (`rightmove`). Normale Bots erhalten jedoch keinen eigenen `cg_controlMode` und verwenden den genullten Standard `CT_MOUSE`. In diesem Modus steuert die Physik über die Differenz von Karosserie- und Steuerwinkel; `rightmove` wird erst im anderen Steuerungsmodus verwendet.

Die Kampfphasen übertragen ihr gewünschtes Fahr-Yaw nicht in die Steuerwinkel. Anschließend richtet `BotAimAtEnemy` diese wieder zum Gegner aus. Damit erreicht insbesondere die beabsichtigte Absetzlenkung die normale Bot-Fahrphysik nicht korrekt. Gas-/Rückwärtsbefehle kommen weiterhin an.

Quellen: [Kampf-Lenkfunktion](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:3534), [Physik-Eingänge](D:/Q3RallyDev/q3rally_tryout/engine/code/game/bg_wheel_forces.c:621), [Client-Initialisierung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_client.c:1349), [Steuerungsmodus-Enum](D:/Q3RallyDev/q3rally_tryout/engine/code/qcommon/q_shared.h:1752), [Steuerungsmodus aus Userinfo](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_client.c:1202), [Aim nach Bewegung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:3613).

### Belegt: aktive Kampfbewegung umgeht Hindernisnavigation

Der neue Kampfcontroller ignoriert die Travel-Flags, führt keine Hindernistraces oder AAS-Bewegung aus und gibt einen vollständig genullten `moveresult` zurück. Die nachfolgende Blockierungsprüfung sieht deshalb kein `blocked`-Signal. Es gibt eine einfache Stillstandserkennung mit Wechsel zur Absetzphase, aber keinen geplanten freien Rückweg oder gemeinsame Recovery.

Außerhalb des Kampfes wird wieder über AAS navigiert. Zusätzliche Fahrzeug-Hindernisvermeidung in der allgemeinen Zielsuche ist explizit auf Derby/LCS beschränkt. Damit ändert ein sichtbarer Gegner das Fahrverfahren erheblich; das betrifft auch Kämpfe in den Teammodi.

Quellen: [Kampfcontroller](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:3548), [Blockierungsbehandlung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:5545), [Sonderbehandlung Derby/LCS](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:3385).

### Belegt: Schussprüfung und tatsächliche Waffenrichtung weichen ab

`BotCheckAttack` prüft Sichtfeld und Schusstrace mit den Bot-Steuer-/Blickwinkeln. `FireWeapon` verwendet dagegen die wirkliche Fahrzeugausrichtung. Die Fahrzeugphysik unterscheidet diese Größen ausdrücklich. Ein gedanklich bereits ausgerichteter Bot kann deshalb Feuer freigeben, obwohl seine Fahrzeugnase und damit die Waffe noch anders ausgerichtet sind. Wie viele Fehlschüsse daraus entstehen, wurde hier nicht gemessen.

Quellen: [Feuerfreigabe](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:4724), [tatsächliches Feuern](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_weapon.c:1596), [Steuerwinkel](D:/Q3RallyDev/q3rally_tryout/engine/code/game/bg_pmove.c:2419), [Karosseriewinkel](D:/Q3RallyDev/q3rally_tryout/engine/code/game/bg_pmove.c:2643).

Weitere Qualitätsgrenzen: Gegnerwahl nimmt den ersten geeigneten sichtbaren Client statt alle Kandidaten zu bewerten. Kampfzeiten und Distanzen sind weitgehend fest und nicht an Waffe, Tempo oder Wendekreis angepasst. Der neue Controller umgeht die bisherige Attack-Skill-Bewegungslogik. Projektilvorhalt verwendet teilweise die alte AAS-Spielerbewegungsprognose statt einer Fahrzeugprognose.

Erhaltenswert sind Waffen-/Itemgewichte, Inventar, Sicht- und Teamabfragen, Reaktionszeit und Präzisionsparameter sowie Versorgung und Rückzug als grundsätzliche Fähigkeiten.

## Racing und Racing-DM

Rennführung wird in dieser Reihenfolge gewählt: Map-`bot_path_node`, geladener Ghost, Checkpoint-Kurve. Das sind drei unterschiedlich ausgestattete Implementierungen innerhalb desselben großen Rennknotens.

Die Datenprüfung der 30 losen BSPs und der vorhandenen PK3s ergab Map-Pfade nur für `Race_Bot_Test` mit 16 Knoten und `q3r_valley` mit 27 Einträgen, effektiv 26 eindeutigen Reihenfolgen. Beide verwenden ausschließlich `pathId=0`. Im Checkout einschließlich ignorierter Dateien und Pakete wurden keine `.ghost`-Dateien gefunden. Damit greift auf den meisten vorhandenen Rennstrecken die Checkpoint-Kurve. Zusätzliche Installations-/Homepath-Daten sind hier ausdrücklich nicht erfasst.

**Belegte Fehler und Unterschiede:**

- Ghost-Recovery berechnet `routeDistanceFromCenter` in einer versehentlich neu deklarierten lokalen Variablen. Der spätere Recovery-Abschluss und Export lesen die äußere, unverändert null gebliebene Variable. Eine Rückkehr kann daher bei niedriger Kollisionszahl bereits abseits der Linie als abgeschlossen gelten. Quellen: [äußere Variable](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:4345), [innere Variable](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:4763), [Recovery-Abschluss](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:5166).
- Der Map-Pfad kehrt vor der großen Ghost-Recovery zurück. Seine eigenen Stuck-/Reverse-Zustandsfelder werden nicht zu einer entsprechenden Verhaltenslogik verbunden. Die allgemeine Hindernishilfe existiert, eine gleichwertige Recovery fehlt. Quelle: [Map-Pfad-Rückkehr](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:4617).
- Die Duplikatprüfung der Knotenreihenfolge verwendet `uniqueCount`, bevor dieser gesetzt wurde. Valley enthält tatsächlich zweimal `order=15`; ein Punkt verschwindet anschließend beim Komprimieren. Quellen: [Validierung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_spawn.c:235), [Valley-Daten](D:/Q3RallyDev/q3rally_tryout/baseq3r/maps/q3r_valley.map:13184).
- Der Map-Export schreibt Entfernung zum vorausliegenden Zielpunkt als Linienabweichung. Der Ghost-Export schreibt wegen des genannten Variablenfehlers null. Die vorhandene KPI-Auswertung ist deshalb derzeit keine verlässliche Messung des Spurhaltens. Quelle: [Map-Export](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:4624).
- `GT_RACING_DM` beendet `BotFindEnemy` ausdrücklich mit `qfalse`. Im Rennknoten fehlen darüber hinaus laufende Gegnerwahl, Waffenwahl und Angriff. Der Team-Racing-DM-Pfad hat diese frühe Ausnahme nicht und ist dadurch zusätzlich inkonsistent. Quelle: [deaktivierte Gegnersuche](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:3997).

**Grenzen bei Ausbau:** Map-Pfade haben keinen expliziten zyklischen Abschluss; Lookahead endet am letzten Knoten und die nächste Suche bleibt bei Hint ±48. Die aktuellen kurzen Pfade beweisen deshalb noch keinen Rundenhänger; längere Rundkurse benötigen aber eine echte Wrap-Logik. Außerdem werden Routen-IDs beim Laden verdichtet, während die KI Arraypositionen als Basis/aggressiv/sicher interpretiert. Das wird bei fehlenden IDs falsch, ist mit den aktuellen reinen Linie-0-Daten aber noch nicht ausgelöst.

Geschwindigkeiten entstehen aus Heuristiken und Aufzeichnungen, ohne gemeinsames Bremsprofil aus Kurvenradius, Fahrzeugzustand und verfügbarer Haftung. Überholen und sichere seitliche Versätze sind im Ghost-Zweig deutlich weiter als im Map-Zweig. Racing-Skill ist nicht eigenständig kalibriert. Eine Tankstrategie ist nicht angeschlossen; Bots bekommen derzeit 90 % ihres Verbrauchs erstattet ([Verbrauchsausgleich](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_active.c:2076)).

Map-Format, Ghost-Import, Checkpointbindung und sichere Reset-Prüfungen behalten. Alle Routenquellen sollten künftig nur Geometrie, Fortschritt und Tempovorschläge an denselben Fahrzeugregler liefern.

## Weitere Rennmodi

### Sprint – separates Ziel wird im Fallback nicht angefahren

Sprint ist korrekt als reines Rennen klassifiziert und sperrt Kampf. Der Bot benutzt aber denselben Rennknoten wie ein Rundkurs. Im Checkpoint-Fallback fehlt die Unterstützung der offenen Start-/Zielstruktur:

- Nach dem letzten Checkpoint steigt der erwartete Index auf `numCheckpoints+1`. Das separate `rally_finish` erhält genau diese Nummer.
- Der Bot sucht ausschließlich nach Entities mit Classname `rally_checkpoint`. Er findet das Finish daher nicht und kehrt ohne neues Fahrziel zurück.
- Auf dem ersten Abschnitt wird außerdem der letzte Streckencheckpoint als vorheriger Punkt verwendet, statt des eigentlichen Starts.

Beide vorhandenen Sprint-BSPs, `q3r_a_to_b_test` und `q3r_canyonrush`, besitzen je sechs Checkpoints, separaten Start/Finish und keine enthaltenen Map-/Ghost-Botrouten. Das ist daher eine relevante Lücke im hier verwendeten Fallback. Restgeschwindigkeit kann trotzdem eine Zielüberquerung ermöglichen; der Code führt den Bot nach dem letzten CP nicht mehr gezielt dorthin.

Quellen: [Finish-Nummer](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_rally_mapents.c:854), [Checkpointfortschritt](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_rally_mapents.c:946), [Bot-Suche](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:5287), [vorheriger Punkt](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:4372).

Empfehlung: Eine Route ausdrücklich als offen oder geschlossen kennzeichnen. Start und Finish gehören als eigene Endpunkte in die Geometrie; der Fahrzeugregler bleibt derselbe wie beim Racing.

### Single Player / Time Trial – gemischte Bedeutung

GT2 ist als reines Zeitrennen klassifiziert, verwendet eigene Time-Trial-Rundenzahl und braucht keine Ready-Abfrage. Gleichzeitig sind die alte Arena-Botliste, automatische Botspawns, Turnierauswertung und Podium noch vorhanden. Im normalen Host-Modusmenü wird GT2 nicht als eigener Eintrag angeboten.

Für die Bots ist der Widerspruch konkret: Die Pure-Race-Kampfsperren enthalten GT2 nicht. Beim Einstieg über Zielsuche kann ein Bot deshalb in Gegnerkampf oder Rückzug wechseln, obwohl die zentrale Modusklassifikation ein waffenloses Rennen ausweist. Eine Arena-Botliste kann einem nominellen Solo-Zeitfahren außerdem automatisch Gegner hinzufügen.

Quellen: [Time-Trial-Parameter](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_main.c:1604), [Arena-Botspawns](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_bot.c:1023), [Host-Modusliste](D:/Q3RallyDev/q3rally_tryout/engine/code/q3_ui/ui_rally_startserver.c:433).

Empfehlung: Festlegen, ob GT2 wirklich solo sein soll oder ein Einzelspieler-Rennen mit Gegnern. Danach Botzulassung, Arena-Vorgaben und zentrale Renn-Kampfsperre konsistent machen. Eine eigene dritte Fahr-KI ist dafür unnötig.

### Elimination – normaler Rennbot ohne KO-Planung

Der tatsächliche KO-Ablauf ist rundenbasiert: Bei N Teilnehmern sind N−1 Runden vorgesehen. Wenn alle bis auf einen eine Runde beendet haben, scheidet der letzte Fahrer dieser Runde aus. Die Countdown-Cvars `g_eliminationStartDelay`, `g_eliminationInterval` und `g_eliminationWarning` werden registriert und exportiert, steuern diesen Ablauf aber nicht. Einige Konfigurationen beschreiben noch einen Countdown.

Quellen: [Ausscheidung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_rally_mapents.c:263), [Rundenanzahl](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_rally_racetools.c:441).

Die KI erfasst verbleibende Fahrer und Ausscheidungsrisiko im Inventar. Das beeinflusst Turbo-/Haste-Pickup-Gewichte, aber der aktive Rennknoten verwendet diese Informationen nicht für Tempowahl, Risikobereitschaft oder Überholen und plant dort keine entsprechenden Itemumwege. Es existiert daher noch kein eigener taktischer KO-Bot.

Ohne Waffen fehlt Elimination ebenfalls in den alten Pure-Race-Kampfsperren. Mit `g_eliminationWeapons=1` gibt es eine Machinegun mit 200 Schuss, während die Itemregeln weiterhin sämtliche Waffen-/Munitionspickups blockieren und Combat-Supply ausgenommen bleibt. Eine endliche Startausrüstung kann Absicht sein; der Mutator ist damit keine allgemeine Freigabe von Waffenversorgung.

Quellen: [KO-Inventar](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:2404), [Pickup-Gewichte](D:/Q3RallyDev/q3rally_tryout/baseq3r/botfiles/fuzi.c:573), [Elimination-Itemregeln](D:/Q3RallyDev/q3rally_tryout/engine/code/game/bg_misc.c:1442), [Startwaffe](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_client.c:1776).

Empfehlung: Rennkern behalten. Darauf eine kleine Risikoentscheidung setzen: drohender letzter Platz rechtfertigt andere Manöver als eine sichere Position. Vorher Waffenmutator und Modusklassifikation vereinheitlichen.

### Team Racing und Team Racing-DM

Team Racing wertet den Durchschnitt der Teamzeiten; noch nicht fertige Fahrer gehen mit aktueller Laufzeit ein. Der Botplaner hat jedoch keinen eigenen Team-Racing-Fall. Jeder Bot setzt unabhängig `LTG_WINRACE`. Es gibt keine Entscheidung anhand des Teamdurchschnitts, kein gezieltes Platzmachen und keine entsprechende Aufgabenzuweisung. Die Verkehrsprognose behandelt Teammitglieder nicht mit einer eigenen Teamregel.

Quellen: [Teamzeit und Bonus](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_rally_mapents.c:480), [Team-AI-Switch](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_team.c:2399), [allgemeines Rennziel](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:1784).

In Team Racing-DM gibt es zusätzlich 500 ms Teamzeitbonus pro Frag und eine Machinegun mit 50 Schuss. Anders als bei GT1 ist Gegnerfindung erlaubt. Damit kann die allgemeine Zielsuche in Kampf wechseln; nach Einstieg in den Rennknoten werden Gegner-/Waffenwahl und Angriff jedoch nicht laufend weitergeführt. Combat-Supply ist für beide Team-Rennmodi ausgeschaltet. Es fehlt eine Abwägung des möglichen Zeitbonus gegen die beim Kämpfen verlorene Fahrzeit.

Empfehlung: Rennfahrt dauerhaft aktiv lassen und Waffen parallel bedienen. Danach zunächst risikoarmes Verhalten gegenüber Teamkollegen, später teamzeitbezogene Unterstützung und Rollen. Für reines Team Racing reicht ein gemeinsamer Rennplaner mit Teamregeln; Team Racing-DM benötigt zusätzlich den Race/Combat-Planer.

### Ghost Race – absichtlich ohne KI-Gegner

GT8 ist der einzige ausdrücklich botfreie Modus. AddBot, Bot-ClientConnect, Mindestspieler-Auffüllung und Host-UI prüfen die zentrale Bot-Erlaubnis. Der aufgezeichnete Ghost ist Wiedergabe, kein denkender Bot. Das ist eine beabsichtigte Abgrenzung und kein Qualitätsdefizit.

Eine kleine UI-Lücke bleibt: Das Ingame-Menü prüft beim Add-/Remove-Bots-Eintrag nicht die zentrale Modusfreigabe. Mit aktiviertem `bot_enable` kann es die Aktion anbieten, die der Server anschließend ablehnt. In der normalen Ghost-Konfiguration ist `bot_enable` bereits aus.

Quellen: [Bot-Erlaubnis](D:/Q3RallyDev/q3rally_tryout/engine/code/game/bg_misc.c:2361), [serverseitiges AddBot-Gate](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_bot.c:586), [Ingame-Menü](D:/Q3RallyDev/q3rally_tryout/engine/code/q3_ui/ui_ingame.c:294).

## Demolition Derby und Last Car Standing

Beide sind Arenamodi und ausdrücklich keine Rennen. Sie betreten den Rennknoten nicht. Die zahlreichen LCS-spezifischen Ghost-/Linien-/Recovery-Abfragen innerhalb dieses Rennknotens sind deshalb für reguläre LCS-Bots unerreichbar. Tatsächlich aktiv sind Zielsuche, allgemeine Arena-Kampfzustände und ein gemeinsamer Roaming-Fallback, der bei fehlenden Itemzielen zum nächsten lebenden Gegner fährt.

Quellen: [Arena-Einordnung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:3063), [Rennknoten-Gate](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:4175), [Gegner-Roaming](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:3082).

### Derby – Rammer passt nicht zur Kampfentscheidung

Derby erhält `WP_DERBY_RAM`, ist vom neuen Vierphasen-Kampfregler ausgeschlossen und landet bei der älteren Kampfbewegung. Der wichtigste Logikfehler liegt davor: `BotAggression` bewertet Schusswaffen, Munition und Quad, aber den Rammer nicht. Ein normal ausgestatteter Derby-Bot ohne Quad kommt damit auf eine zu niedrige Aggression, will zurückweichen und nicht verfolgen. Scheitert das Rückzugsziel, kann er wieder im erzwungenen Kampf landen. Das ist ein Widerspruch in der Entscheidungslogik eines Rammspiels.

Quellen: [Derby-Ausrüstung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_client.c:1805), [Aggressionsbewertung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:2899), [ausgenommener Kampfcontroller](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:3256).

Der fahrende Roaming-Fallback hilft gegen Stillstand, schickt sein beabsichtigtes Rammziel aber durch dieselbe Vermeidung wie andere Autos: Ein langsamer/vorausstehender Gegner im zentralen Trace kann Bremsen beziehungsweise Rückwärtsfahrt auslösen. Die KI unterscheidet dort nicht „gewolltes Kontaktziel“ von „Hindernis“. Das kann beabsichtigte Angriffe abbrechen; die Häufigkeit ist noch zu messen. Quellen: [Rammziel-Fahrt](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:3126), [Body-Vermeidung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:4218), [Bremsreaktion](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:4315).

Weitere Strategie fehlt: Die KI plant weder Trefferzone noch Anfahrenergie, obwohl der Schaden Richtung, Masse und Impuls berücksichtigt. Sie berücksichtigt den No-Ram-Timeout nicht, obwohl ausbleibender Kontakt zur Selbstzerstörung führen kann. Rammkontakte liefern Fuel/NOS, aber eine aktive Derby-Turboplanung ist nicht angeschlossen.

Empfehlung: Rammer als echte Kampffähigkeit bewerten und eine eigene Rammentscheidung über den gemeinsamen Fahrzeugregler setzen: Anfahrt, gewollter Kontakt, Absetzen und neuer Anlauf. Kein umfassender Neubau von Navigation und Wahrnehmung nötig.

### LCS – Überlebensabsicht ohne passende Fluchtplanung

LCS hat bereits eigene Gesundheits-/Gegnerzahl-/Risikometriken und versucht riskante Kampfaufnahmen zu vermeiden. Der aktive Kampf benutzt den neuen Fahrzeug-Kampfregler und erbt dessen Lenk-/Hindernisprobleme.

Ein belegter Strategiewiderspruch: Wird der Gegner bei geringer Gesundheit bewusst ignoriert und findet die Zielsuche kein Itemziel, fällt LCS auf denselben Roaming-Fallback wie Derby zurück. Dieser fährt zum nächsten Gegner. Die Entscheidung „Kampf vermeiden“ erhält also kein sicheres Fluchtziel. Quellen: [Risikosperre](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:211), [Roaming bei fehlendem Ziel](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:3262).

Die feste Eintrittsschwelle von 62 HP berücksichtigt außerdem das Skill-Handicap nicht. Skill 1 bis unter 2 setzt MaxHealth auf 50. Ein auf seine normale volle Gesundheit zurückgefallener Bot liegt damit bereits unter der LCS-Kampfsperre, auch ohne Schaden. MaxHealth-normalisierte Gesundheit würde taktische Gefahr von absichtlicher Schwierigkeitsabsenkung trennen. Quellen: [absolute Schwelle](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:224), [Skill-Handicap](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_bot.c:667).

Empfehlung: Eigene Überlebensplanung behalten und ausbauen: fahrbare sichere Regionen, Gegneranfahrten, Versorgung, verbleibende Fahrer und Zeitpunkt eines Angriffs. Ausweichen und Angreifen müssen jeweils konkrete Fahrziele produzieren. Die unerreichbaren LCS-Verbesserungen im Rennknoten sollten in diesen aktiven Pfad überführt werden.

## Team Deathmatch

TDM ist mehr als eine Teamkennzeichnung: Teamprüfungen, direkte Friendly-Fire-Vermeidung und ausführbare Follow-/Help-Aufträge existieren. Automatische Gruppenbildung ist aber grob: Bei ein oder zwei Teammitgliedern gibt es keine Gruppe; bei drei bis zehn werden kleine Gruppen anhand der Client-Slot-Reihenfolge gebildet. Über zehn greift dieser Gruppenplan ebenfalls nicht. Nähe, Ausstattung, Gesundheit und aktuelle Kämpfe beeinflussen die Zuordnung nicht.

Quellen: [Gruppenbildung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_team.c:1057), [ausführbare Folgeaufträge](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_cmd.c:474).

Ein menschlicher Mitspieler wird automatisch als Leader gewählt, wenn er nicht als ungeeignet markiert ist. Dann übernimmt kein Bot die automatische Auftragsverteilung. Menschen ohne manuelle Befehle bekommen deshalb keine gleichwertige automatische Mannschaftsorganisation. Ohne Menschen wird per Chat und Wartezeiten ein Leader gewählt; regelmäßige TDM-Neuplanung erfolgt nur ungefähr alle 125 Sekunden oder nach begrenzten Änderungsereignissen. Quellen: [Mensch als Leader](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_team.c:2321), [Auftragsrhythmus](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_team.c:2393).

Kampf und Rückzug bleiben weitgehend Einzelentscheidungen. Gemeinsame Gegnerauswahl, lokale Überzahl, abgestimmter Rückzug und Itemreservierung sind nicht integriert. Der direkte Schusstrace schützt Mitspieler; Splash-Schaden auf Mitspieler ist noch als `FIXME` offen, relevant bei aktiviertem Friendly Fire. Quelle: [Schussprüfung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:4737).

Empfehlung: Funktionierende Folge-/Hilfeaufgaben behalten. Die Gruppenzuweisung durch einen kleinen Teamkoordinator ersetzen, der auf Nähe, Ausrüstung und Ereignisse reagiert und Menschen automatisch unterstützt. Der höhere individuelle Bot-Skill ersetzt diese fehlende Teamplanung nicht.

## CTF und CTF4

Normales CTF hat einen umfangreichen Bestand an Angriff, Verteidigung, Flaggenrückweg, Eskorte und Teamleader-Befehlen. Mehrere Entscheidungen und Statusaktualisierungen stammen aber noch aus der alten Zweiteam-/Chat-Architektur.

**Belegt in normalem CTF:** Die Ausführung von `LTG_RETURNFLAG` setzt für Rot die blaue Basisflagge und für Blau die rote Basisflagge als Ziel. Das ist kein dynamisches Rückholziel für die eigene fallengelassene Flagge oder deren Träger. Die Entscheidung bei sichtbarem feindlichem Flagträger enthält zusätzlich noch eine `FIXME`-Lücke. Quellen: [Rückholziel](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:1929), [Flagträger-Entscheidung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:839).

Versorgung kann Objective-Vorrang überschreiben: Die kleine erlaubte Itemreichweite für Flagträger wird bei Munitionsbedarf wieder auf mindestens 650 erhöht. Auch im Kampf erfolgt die nahe Versorgungssuche vor einem Flagträgerausschluss, den erst der spätere Langzeitversorgungshelfer besitzt. Das ermöglicht große Umwege auf dem Heimweg. Quelle: [Itemreichweiten](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:1146).

**CTF4 ist an zentralen Stellen funktional nicht angeschlossen:**

- `gametype` übernimmt unverändert `g_gametype`. Trotzdem liegt das Laden der grünen/gelben Flaggen innerhalb von `if (gametype == GT_CTF)`. Dieser innere CTF4-Zweig ist unerreichbar; bei echtem CTF4 werden auch Rot/Blau dort nicht initialisiert. Quelle: [Flaggeninitialisierung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:6536).
- Die persönliche CTF-Zielwahl und die eigentliche Flaggen-LTG-Ausführung sind ebenfalls nur für `GT_CTF` freigegeben. Quellen: [Zielwahl](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:1795), [Zielausführung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:1868).
- `BotCTF4Orders` ruft eine Basistravel-Sortierung auf, die im normalen Build für CTF4 kein Basisziel setzt und null Teammitglieder zurückliefert. Der Befehlsgeber beendet sich dann vor der Verteilung. Quelle: [Sortierung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_team.c:127), [CTF4-Aufruf](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_team.c:980).
- Die ausgelieferten Bot-Itemdefinitionen kennen nur rote, blaue und neutrale Flagge, keine grüne/gelbe. Die Befehlsparser sind ebenfalls überwiegend auf die alten Modi beschränkt. Quelle: [Bot-Items](D:/Q3RallyDev/q3rally_tryout/baseq3r/botfiles/items.c:717).
- Auch `bot_minplayers` berücksichtigt in sämtlichen Teammodi nur Rot und Blau. Grün/Gelb werden in CTF4 nicht automatisch aufgefüllt. Der Wert gilt dort pro Team, mit einer Kapazitätsgrenze auf Grundlage von zwei Teams. Das CTF4-Preset mit Wert 12 und 16 Slots versucht daher sieben Rot- und sieben Blau-Spieler zu erreichen, nicht zwölf Teilnehmer über vier Teams zu verteilen. Manuell hinzugefügte grüne/gelbe Bots bleiben möglich. Quelle: [Bot-Auffüllung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_bot.c:436).

Die Vierteam-Zielbewertung, Score-Druck und vorgesehene Rollen sind brauchbare Ansätze. Vor taktischem Tuning muss jedoch die gesamte Kette aus Weltzustand, Teamauftrag, individuellem Ziel und Fahrbarkeit funktionieren. Eine bloße Erweiterung einzelner `if`-Abfragen genügt dafür nicht.

## Domination

Der Sigilhelfer hat mehrere voneinander unabhängige Fehler ([Zielbewertung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:352)):

1. Er sucht `team_domination_sigil` über `BotGetLevelItemGoal`. Diese API vergleicht den Anzeigenamen, nicht den Classname. Die bereitgestellte Bot-`items.c` besitzt außerdem überhaupt keine Sigildefinitionen. Mit diesen Daten werden die gewünschten Ziele so nicht gefunden.
2. Der beste Kandidatenindex wird gespeichert und anschließend erneut an die Iterator-API übergeben. Diese beginnt **nach** dem übergebenen Item. Zurück kommt das nächste Sigil oder gar keines, nicht der ausgewählte Kandidat. Quelle: [Iterator-Semantik](D:/Q3RallyDev/q3rally_tryout/engine/code/botlib/be_ai_goal.c:859).
3. `bestScore=999999` bei `Statusgewicht + Distanz²` wirkt als unbeabsichtigte Entfernungsschranke: neutrale Punkte ab etwa 1000 Units und feindliche ab etwa 949 Units können nicht gewinnen. Große Arenen sind dadurch selbst nach einer Datenkorrektur problematisch.
4. Selbst mit funktionierendem Zugriff werden Statusreihenfolge und Botlib-Itemreihenfolge implizit gleichgesetzt, statt Sigils über eine stabile Entity-/Objective-ID zuzuordnen.

Die Teamverteilung rechnet zwar Verteidiger, Eroberer und Backcapper aus, versendet aber generische Befehle ohne konkrete Punktzuweisung. Der aktive persönliche Seek-Pfad wählt anschließend selbst neu und setzt `ordered=false`. Es fehlen Reservierungen und eine Entscheidung, welche Punkte eine Mannschaft halten soll. Quellen: [Teamverteilung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_team.c:2240), [persönliche Neubewertung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:1678).

Empfehlung: Sigils direkt als Objectives registrieren; pro Mannschaft konkrete Punktaufträge nach Fahrzeit, Besitz, Bedrohung und bereits zugewiesenen Autos vergeben. Für dieses überschaubare Entscheidungssystem lohnt sich ein Neuaufbau der Planung.

## King of the Hill

Die KI verwendet die Position aus `CS_KOTHSTATUS` direkt als AAS-/Fahrziel. Diese Position ist jedoch die Anzeigenposition: XY-Mitte und Z = Trigger-Oberkante plus `indicatorHeight`. Sie ist kein geprüfter Punkt auf der befahrbaren Fläche in der Capture-Zone. Der AAS-Helfer sucht bei Fehlschlag lediglich zehn Units nach oben. Ob die konkrete Position eine gültige befahrbare Area trifft, hängt daher von der Map ab.

Quellen: [Anzeigenposition](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_team.c:1760), [Verwendung als Botziel](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:1749), [Area-Helfer](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmq3.c:1859), [Zielausführung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:2190).

KOTH verwendet außerdem das generische `LTG_DEFENDKEYAREA` und allgemeine Zielnavigation. Eine ausdrückliche Aufgabe, innerhalb der tatsächlichen Capture-Zone anzuhalten und zu bleiben, fehlt. Kämpfe können bereits vor der Zielneubewertung übernehmen. Die vorgesehenen Hold-/Contest-/Rotate-Befehle sind keine verbindliche räumliche Aufgabenverteilung.

Quellen: [allgemeines Verteidigen](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_dmnet.c:1586), [KOTH-Teamaufteilung](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_team.c:2173).

Die bestehenden [AAS-Mapnotizen](D:/Q3RallyDev/q3rally_tryout/docs/maps/aas_proxy.md:52) beschreiben bereits Bots, die auf Mesa King Pickups anfahren, den Hügel aber meiden. Das sind frühere dokumentierte Beobachtungen, kein neuer Lauf dieses Audits. Der Code enthält konkrete Zielwahlprobleme, die vor einer erneuten AAS-Überarbeitung behoben werden sollten.

Empfehlung: eigene Aufgabe „Zone erreichen und halten“, mehrere geprüfte Bodenpunkte innerhalb der tatsächlichen Triggergrenzen, begrenztes Verfolgen außerhalb und verteilte Positionen auf dem Hügel. Contest-/Capture-Zustand und verbleibende Haltezeit müssen bei Versorgung und Kampf mitentscheiden.

## Autoball

Autoball hat eine eigenständige, kleine KI. Sie wählt Angreifer, Verteidiger und Support, berücksichtigt menschliche Teammitglieder, fährt hinter den Ball, nutzt Turbo und besitzt eine einfache Rückwärtsbefreiung. Ein Bonus für den bisherigen Angreifer verhindert einen Teil der Rollenwechsel. Diese Grundlagen sind sinnvoll.

Wesentliche Grenzen und belegte Anschlussprobleme:

- Trotz direkter Steuerung bleibt das allgemeine AAS-Gate bestehen. Arenen ohne gültige AAS bekommen dadurch keine funktionierende Bot-KI. Für die Testarena liegen hier Generator und Arena-Metadaten, aber keine lose fertige BSP/AAS vor; über externe Installationen trifft dieser Bericht keine Aussage.
- Ballprognose ist lineare XY-Fortschreibung bis maximal eine Sekunde. Höhe, Schwerkraft, Wandkontakte und Bounces fehlen. Torgefahr wird im Wesentlichen über Entfernung bewertet, nicht über Torflugbahn oder Zeit bis zum Einschlag.
- Alle Supporter berechnen denselben Stellplatz und bei entsprechendem Mangel denselben nächsten Turbopickup. Bei unmittelbarer Torgefahr schaltet zusätzlich der Verteidiger lokal auf Angriff, während der normale Angreifer erhalten bleibt. Die Zielkonvergenz ist belegt; sichtbare Kollisionen und Doppelangriffe müssen gemessen werden.
- Angreiferwahl berücksichtigt Entfernung und Annäherungsseite, aber keine erreichbare Ankunftszeit mit Orientierung, Tempo und Bremsweg. Ein untätiger Mensch kann deshalb trotzdem die Angreiferrolle belegen.
- Fahrsteuerung ist grob: keine geplante Ballkontaktgeschwindigkeit; kurzer schmaler Hindernistrace; eine Sekunde Rückwärtsfahrt als wesentliche Recovery. Kein gemeinsamer Teamplan für Pässe, abgefangene Schüsse, Rotation oder Mehrball-Aufteilung.
- Bei `g_autoballWeapons 1` werden Waffen erlaubt, aber der Autoball-Earlyreturn umgeht die normale Gegner-/Kampf-/Itemsuche. Die eigene KI bietet dafür keinen Ersatz.

Quellen: [Rollen](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_autoball.c:150), [Fahren](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_autoball.c:240), [Ballprognose](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_autoball.c:304), [Torgefahr und Aufgaben](D:/Q3RallyDev/q3rally_tryout/engine/code/game/ai_autoball.c:418).

Die Spielphysik und Matchverwaltung müssen dafür nicht ersetzt werden. Für Spielstatistiken existiert sogar bereits eine einfache ballistische Torprognose, die die KI nicht nutzt ([Torprognose](D:/Q3RallyDev/q3rally_tryout/engine/code/game/g_autoball.c:806)). Sie ist ein möglicher Ausgangspunkt, keine vollständige Kontaktphysikprognose. Ballmodell, erreichbare Treffpunkte und Team-Commitments sollten gemeinsam geplant werden.

## Moduswahl und vorhandene Serverkonfigurationen

Ein Teil der scheinbaren Botprobleme kann schon durch falsch gestartete Modi entstehen. Einige ältere Rotationen stimmen nicht mit den aktuellen Kennungen überein:

- Die Mixed-Combat-Rotation startet die Blöcke mit Elimination-Parametern als `g_gametype 6`, also Deathmatch auf Rennstrecken. Ein weiterer Block startet `17`, also Team Racing auf DM-Arenen; der Abschluss nutzt `5`, also Elimination auf der LCS-Map. Quelle: [Mixed-Rotation](D:/Q3RallyDev/q3rally_tryout/baseq3r/server_mixed_combat_rotation.cfg:32).
- Die ausdrücklich als Sprint bezeichnete Nebenrotation setzt `g_gametype 2`, also Single Player statt Sprint 7. Quelle: [Sprint-Rotation](D:/Q3RallyDev/q3rally_tryout/baseq3r/server_racing_modes_rotation.cfg:62).

Die einzelnen aktuellen `q3r_*.cfg`-Presets haben überwiegend die passenden Moduskennungen. Für einen Benchmark müssen dennoch tatsächliche Modusnummer, Map-Entities, AAS und laufende Mutatoren protokolliert werden. Sonst wird beispielsweise Rennbot-Verhalten auf einer Arena beurteilt, obwohl ursprünglich Team Deathmatch gemeint war. Der Audit hat diese Konfigurationen nicht verändert.

## Messung und Teststand

Ausgeführt und bestanden:

- `test_ai_dmnet_route_fallback_behavior`
- `test_bot_path_spawn_helpers_behavior`
- `test_missing_entity_data_regression`

Nach Erweiterung auf alle Modi zusätzlich bestanden:

- `test_gametype_classification`
- die sieben Testfunktionen aus `test_gametype_cli.py`, einschließlich Abgleich aller 17 Kennungen mit dem Engine-Enum
- `test_rally_state_machine_behaviour`

Insgesamt wurden damit zwölf bestehende Testfunktionen erfolgreich ausgeführt.

Die vorhandenen Python-Testfunktionen wurden direkt mit `runpy` aufgerufen und kompilierten ihre C-Tests mit WSL/GCC. `pytest` war dort nicht installiert. Es gab lediglich Warnungen zur doppelten Definition von `Q3_LITTLE_ENDIAN`.

Diese Tests bestätigen Routenauswahl, Spawn-/Filter-/Komprimierungshelfer und den Umgang mit fehlenden BSP-Entitydaten. Sie prüfen weder die oben gefundenen Integrationsfehler noch das Spielverhalten. Tests für KOTH-Regeln oder CTF4-Scoring sind ebenfalls keine Bot-Verhaltenstests.

Der vorhandene DMNet-Export und [KPI-Auswerter](D:/Q3RallyDev/q3rally_tryout/tools/ai_dmnet_kpi.py:1) bieten einen Anfang. Vor Einsatz müssen die Abweichungsmessung korrigiert und mehrere Bots getrennt ausgewertet werden: Der Auswerter hält derzeit nur einen globalen vorherigen Überholzustand und ignoriert die Client-ID. Außerdem bedeutet `collision_rate` hier Anteil der Ticks mit vorhergesagtem Risiko, nicht echte Kollisionshäufigkeit.

Für die nächste Ausbaustufe braucht es feste Szenarien und Seeds, definierte Bot-/Fahrzeugkonfigurationen und wiederholte Läufe. Vorschlag für Messgrößen:

| Bereich | Messen |
|---|---|
| Gemeinsames Fahren | Fahrzeit zum Ziel, Stillstandszeit, tatsächliche Kontakte, erfolgreiche Befreiungen, Resets |
| Racing | abgeschlossene Runden, Checkpointfehler, Rundenzeitstreuung, Spurabweichung, Überholerfolg |
| Sprint | Start-/Endsegment, Zielerreichung nach letztem CP, benötigte Recovery |
| Elimination | Überleben pro KO-Runde, Reaktion auf letzten Platz, unnötige Risiken aus sicherer Position |
| Team Racing / Team Racing-DM | Teamdurchschnitt, gegenseitige Behinderung, Kampf-Zeitverlust gegenüber Fragbonus |
| Derby | gültige Rammkontakte, Zeit ohne Kontakt, Trefferenergie/-zone, verhinderte eigene Angriffe |
| LCS | Überlebenszeit, gewählte Kämpfe, sichere Fluchtziele, Verhalten relativ zu MaxHealth |
| Deathmatch | Treffer pro Schuss, Zeit bis zum wirksamen Angriff, Richtungsfehler bei Feuerfreigabe, unnütze Zielwechsel |
| Team Deathmatch | gemeinsame Angriffe, Partnerabstand, Neuorganisation nach Tod/Trennung, Unterstützung von Menschen |
| CTF/CTF4 | Rückhol- und Capture-Erfolg, Heimwegumwege, Eskorte, Zeit ohne sinnvolle Objective-Aufgabe |
| Domination | Punktabdeckung, unnötige Überbesetzung, Capture-Abbrüche, gehaltene Mehrheit |
| KOTH | Zeit in der Zone, Ankunftszeit, Capture-/Contest-Erfolg, unnötige Ausfahrten |
| Autoball | erreichbare Ballkontakte, Saves, Eigentore, Doppel-Commitments, Recovery und Teamabstände |

Für CPU-Performance anschließend getrennt Botzeit pro Serverframe und Kosten mit steigender Botzahl erfassen. Der aktuelle Audit erlaubt keine Aussage über einen CPU-Flaschenhals.

## Umsetzung in sinnvoller Reihenfolge

1. **Funktion herstellen:** Botlib-Modusnummern und zentrale Renn-Kampfsperren, Sprint-Start/Finish, CTF4-Kette einschließlich Botfüllung, Domination-Objectivezugriff, CTF-Rückholziel, KOTH-Boden-/Zonenziel, Kampf-Lenkung und Feuergeometrie, Racing-DM-Gegnersuche sowie Derby-Rammerbewertung. Dazu kleine gezielte Regressionstests für die tatsächlichen Fehler. Ghost-Recovery, Leader-Gültigkeit und Telemetrie korrigieren; die Testkonfigurationen auf aktuelle Kennungen bringen.
2. **Fahrkern vereinheitlichen:** ein definierter Ausgang für gewünschte Fahrtrichtung beziehungsweise Zielpunkt, Zielgeschwindigkeit, Bremsen und Rückwärtsfahrt. Karosserie, Lenkwunsch und Aim-Wunsch ausdrücklich auseinanderhalten. Alle Modi erhalten dieselbe Fahrzeugbreitenprüfung, Hindernisreaktion und Recovery; Routenquellen bleiben austauschbar.
3. **Aufgaben direkt zuweisen:** gemeinsamer Weltzustand und konkrete Teamaufträge mit Objective-ID, Position und Verbindlichkeitsdauer. Chat ist Ausgabe dieser Aufträge. Team-DM, Teamrennen, CTF/CTF4, Domination, KOTH und Autoball bekommen passende Strategien und Prioritätsregeln für Kampf und Versorgung. Derby erhält Kontaktplanung, LCS echte Flucht-/Überlebensziele und Elimination eine KO-Risikobewertung.
4. **Spielstärke ausbauen:** Racing-Bremsprofil/Verkehr, waffenabhängige Kampfanflüge, echte Autoball-Intercepts, menschliche Mitspieler und abgestufte taktische Schwierigkeit. AAS dort ergänzen oder ersetzen, wo befahrbare Übergänge und Fahrzeugwenderadien es nachweislich erfordern.

Der erste praktische Meilenstein sollte ein fester Lauf pro Modus sein, in dem Bots ihr eigentliches Ziel zuverlässig erreichen und verfolgen. Danach lässt sich sinnvoll bewerten, wie schnell, gefährlich und koordiniert sie werden sollen.
