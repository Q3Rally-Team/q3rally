#ifndef G_PROFILE_H
#define G_PROFILE_H

struct profile_stats_s;
struct profile_rank_s;
struct ladderProfileSnapshot_s;

struct gclient_s;
struct gentity_s;

void G_Profile_Init( void );
void G_Profile_Shutdown( void );
void G_Profile_FlushIfDirty( void );
qboolean G_Profile_IsDirty( void );
void G_Profile_TrackClientSpawn( struct gclient_s *client );
void G_Profile_UpdateClientFrame( struct gentity_s *ent );
void G_Profile_RecordDamage( struct gclient_s *attacker, struct gclient_s *victim, int damage );
void G_Profile_AddScore( int delta );
void G_Profile_RecordKill( struct gclient_s *attacker, struct gclient_s *victim );
void G_Profile_RecordDeath( struct gclient_s *victim );
void G_Profile_RecordFlagCapture( struct gclient_s *client );
void G_Profile_RecordFlagAssist( struct gclient_s *client );
void G_Profile_RecordLapComplete( struct gclient_s *client, qboolean isLeader, qboolean allowRankProgress );
void G_Profile_RecordRacePlacement( struct gclient_s *client, int position );
void G_Profile_RecordRaceTime( struct gclient_s *client, int totalRaceMs );
void G_Profile_RecordSprintTime( struct gclient_s *client, int totalMs );
void G_Profile_RecordSurvivalTime( struct gclient_s *client, int survivalMs );
void G_Profile_RecordEliminationRound( struct gclient_s *client, int roundsLasted );
void G_Profile_RecordZoneHold( struct gclient_s *client, int zoneHoldMs );
void G_Profile_RecordCtfCapture( struct gclient_s *client );
void G_Profile_RecordWin( struct gclient_s *client );
void G_Profile_RecordLoss( struct gclient_s *client );
void G_Profile_RecordBestLap( struct gclient_s *client, int lapTime );
void G_Profile_RecordExcellent( struct gclient_s *client );
void G_Profile_RecordImpressive( struct gclient_s *client );
void G_Profile_RecordAccuracy( struct gclient_s *client, int accuracyPercent );
void G_Profile_RecordPerfect( struct gclient_s *client );
void G_Profile_RecordDominationCapture( struct gclient_s *client );
/* Public parse helpers – allow g_main.c to read profile JSON fields */
int      G_Profile_ParseIntPublic( const char *buffer, const char *key, int defaultValue );
double   G_Profile_ParseDoublePublic( const char *buffer, const char *key, double defaultValue );
void     G_Profile_ParseStringPublic( const char *buffer, const char *key, char *out, int outSize, const char *defaultValue );

qboolean G_Profile_GetRank( const struct profile_stats_s *stats, struct profile_rank_s *outRank );
int G_Profile_GetPlayerScore( void );
qboolean G_Profile_GetLadderSnapshot( struct ladderProfileSnapshot_s *outSnapshot,
                                      int *outSnapshotRevision,
                                      int *outSnapshotEpoch );

/* Gibt die UUID des aktiven Profils zurück.
 * Schreibt in out (muss mindestens PROFILE_MAX_UUID Bytes groß sein).
 * Gibt qtrue zurück wenn eine valide UUID verfügbar ist, sonst qfalse. */
qboolean G_Profile_GetUUID( char *out, int outSize );

#endif /* G_PROFILE_H */
