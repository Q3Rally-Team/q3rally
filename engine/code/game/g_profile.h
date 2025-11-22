#ifndef G_PROFILE_H
#define G_PROFILE_H

#include "profile_shared.h"

struct gclient_s;
struct gentity_s;

void G_Profile_Init( void );
void G_Profile_Shutdown( void );
void G_Profile_TrackClientSpawn( struct gclient_s *client );
void G_Profile_UpdateClientFrame( struct gentity_s *ent );
void G_Profile_RecordDamage( struct gclient_s *attacker, struct gclient_s *victim, int damage );
void G_Profile_RecordKill( struct gclient_s *attacker, struct gclient_s *victim );
void G_Profile_RecordDeath( struct gclient_s *victim );
void G_Profile_RecordFlagCapture( struct gclient_s *client );
void G_Profile_RecordFlagAssist( struct gclient_s *client );
void G_Profile_RecordWin( struct gclient_s *client );
void G_Profile_RecordLoss( struct gclient_s *client );
void G_Profile_RecordBestLap( struct gclient_s *client, int lapTime );
void G_Profile_RecordExcellent( struct gclient_s *client );
void G_Profile_RecordImpressive( struct gclient_s *client );
void G_Profile_RecordAccuracy( struct gclient_s *client, int accuracyPercent );
void G_Profile_RecordPerfect( struct gclient_s *client );

#endif /* G_PROFILE_H */
