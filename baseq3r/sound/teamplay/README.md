# Teamplay Announcer Assets

The client and cgame modules expect announcer cues for four-team Capture the Flag scoring at the following paths:

- `sound/teamplay/voc_green_scores.wav`
- `sound/teamplay/voc_yellow_scores.wav`
- `sound/teamplay/voc_green_leads.wav`
- `sound/teamplay/voc_yellow_leads.wav`

These files are not bundled with the source tree. Add the matching 22.05 kHz mono WAV assets to your `baseq3` directory before packaging a build so that the announcer cues play correctly when the green or yellow team scores or takes the lead in CTF4.
