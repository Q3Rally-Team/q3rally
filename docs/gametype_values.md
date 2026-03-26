# Q3Rally Gametype Values

This document lists the `g_gametype` integer values for all gametypes across versions.

## v0.7c (current)

As of v0.7c, all gametype enum entries have **explicit integer values** in `bg_public.h`.
This prevents accidental value shifts when new gametypes are added in future versions.

### Non-team gametypes (0–15)

| g_gametype | Name | Description |
|---|---|---|
| 0 | GT_RACING | Racing |
| 1 | GT_RACING_DM | Racing with weapons |
| 2 | GT_SINGLE_PLAYER | Single player / time trial |
| 3 | GT_DERBY | Demolition derby |
| 4 | GT_LCS | Last car standing |
| 5 | GT_ELIMINATION | Elimination race |
| 6 | GT_DEATHMATCH | Free-for-all deathmatch |
| 7 | GT_SPRINT | Point-to-point sprint race |
| 8–15 | *(reserved)* | Reserved for future non-team modes |

### Team gametypes (16+)

| g_gametype | Name | Description |
|---|---|---|
| 16 | GT_TEAM | Team deathmatch |
| 17 | GT_TEAM_RACING | Team racing |
| 18 | GT_TEAM_RACING_DM | Team racing with weapons |
| 19 | GT_CTF | Capture the flag |
| 20 | GT_CTF4 | 4-team capture the flag |
| 21 | GT_DOMINATION | Domination |
| 22 | GT_KOTH | King of the hill |
| 23+ | *(reserved)* | Reserved for future team modes |

---

## Migration table

If you are upgrading a server from a previous version, use this table to update
your `g_gametype` setting in your server config.

| Gametype | v0.6c | v0.7b (buggy) | v0.7c (fixed) | Config change needed? |
|---|---|---|---|---|
| GT_RACING | 0 | 0 | 0 | No |
| GT_RACING_DM | 1 | 1 | 1 | No |
| GT_SINGLE_PLAYER | 2 | 3 | 2 | No (v0.6c→v0.7c) |
| GT_DERBY | 3 | 4 | 3 | No (v0.6c→v0.7c) |
| GT_LCS | 4 | 5 | 4 | No (v0.6c→v0.7c) |
| GT_ELIMINATION | 5 | 6 | 5 | No (v0.6c→v0.7c) |
| GT_DEATHMATCH | 6 | 7 | 6 | No (v0.6c→v0.7c) |
| GT_SPRINT | *(new)* | 2 ⚠️ | 7 | Yes (v0.7b→v0.7c) |
| GT_TEAM | 7 | 8 | 16 | **Yes** |
| GT_TEAM_RACING | 8 | 9 | 17 | **Yes** |
| GT_TEAM_RACING_DM | 9 | 10 | 18 | **Yes** |
| GT_CTF | 10 | 11 | 19 | **Yes** |
| GT_CTF4 | 11 | 12 | 20 | **Yes** |
| GT_DOMINATION | 12 | 13 | 21 | **Yes** |
| GT_KOTH | *(new)* | 14 | 22 | Yes (v0.7b→v0.7c) |
