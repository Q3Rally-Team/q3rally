# Team-mode entity overflow notes

## Symptom
When running any team-based game type the server eventually aborts with `G_Spawn: no free entities`. The entity dump printed right before the abort shows hundreds of entries still tagged as `noclass`.

## Root cause
`CopyToBodyQue` clones the extra collision bounds (`frontBounds`/`rearBounds`) from the dying vehicle so that the corpse keeps its full collision hull. Body queue slots are recycled in a ring buffer, which means the helper entities from a previous corpse may still exist when the slot is reused. Prior to this fix the code simply overwrote the `body->frontBounds`/`rearBounds` pointers with new `G_Spawn()` results, leaving the old helpers orphaned in the world. The leak quickly exhausts the entity pool in small-team modes where deaths happen in quick succession.

## Fix
Each body-queue entry now owns two persistent helper entities that are created alongside the queue in `InitBodyQue`. When a corpse is copied into the slot we simply update and relink those helpers; when the corpse sinks we unlink them again instead of freeing. A shared `G_ResetExtraBBox` helper keeps every code path (body sinking, gibbing, etc.) consistent so the helpers are only unlinked and recycled. The entity count therefore stays flat regardless of death frequency and the scoreboard no longer breaks because we reuse live helpers instead of freeing memory out from under the queue.
