# v0.8 race feedback

Implemented on 2026-11-01; in-game verification pending.

- Touching a checkpoint or finish gate later than the required gate shows
  `CHECKPOINT MISSED` and `Return to checkpoint N`. Progress does not advance.
  Earlier/already-cleared gates do not produce this warning. Messages are
  limited to one per player every four seconds and suppressed before the race,
  after finishing, for bots, spectators and non-driving player states.
  A shared start/finish line does not warn while checkpoint 1 is expected and
  no checkpoint has been cleared since lap start: the initial crossing and
  lingering contact after completing a lap are not proof of a missed gate.
  Separate sprint finish gates still report missing checkpoint 1.
- Wrong-way warnings use horizontal travel direction rather than car heading.
  They require more than 40 game units/second of horizontal movement and the
  existing two-second sustained-direction delay. Intro, countdown, finished
  and non-driving states clear the pending warning. A new race or disabling
  the checkpoint arrow also clears its timer.
- Successful automatic recovery of an overturned car shows `CAR RECOVERED`.
  Rejected recovery attempts do not show a success message.
- Non-winning local finishes show `RACE FINISHED`. Existing winner messages,
  Ghost Race results and elimination-specific feedback remain in use.

The wrong-way heuristic still points toward the next gate, not a route tangent;
hairpins may therefore need a later route-aware improvement. Missing gates are
reported when a later gate is actually touched, not immediately at an arbitrary
off-track position. Existing checkpoint-arrow visibility controls still apply.

## Validation

Production-code C harnesses in `tests/test_race_feedback.py` and
`tests/test_race_camera_transitions.py` pass. They cover warning recipients,
cooldowns, countdown/finish/spectator/bot/dead guards, unchanged race progress,
low-speed/vertical-motion/heading thresholds, and finished-car transitions.
Native Windows game DLLs and QVMs build successfully; existing unrelated
compiler warnings remain. No final release package was built.

Manual checks:

1. Skip a gate and touch a later checkpoint; confirm the required number and
   that the missed checkpoint still needs to be passed. Test the finish line
   on both lap and sprint tracks, and repeated trigger contacts.
2. Stay still facing backward, drive toward the gate in reverse, then drive
   away for over two seconds. Check countdown, restart and after finishing.
3. Let an overturned car recover; check one success message after recovery.
4. Finish first and later in an offline/multiplayer race; check Ghost Race and
   elimination results for their existing specialized messages.
