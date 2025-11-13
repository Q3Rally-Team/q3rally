/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2021 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

q3rally source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with q3rally; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//

#include "g_local.h"
#include "g_profile.h"

extern vmCvar_t g_dominationScoreInterval;
extern vmCvar_t g_dominationCaptureDelay;

// Q3Rally Code Start

typedef struct domination_sigil_s
{
  gentity_t       *entity;
  sigilStatus_t   status;
} domination_sigil_t;

// Q3Rally Code END

typedef struct teamgame_s {
	float			last_flag_capture;
	int				last_capture_team;
	flagStatus_t	flagStatus[TEAM_NUM_TEAMS];	// CTF
	int				flagTakenTime[TEAM_NUM_TEAMS];
	int				obeliskAttackedTime[TEAM_NUM_TEAMS];
// Q3Rally Code Start
	domination_sigil_t     sigil[MAX_SIGILS];
	int				numSigils;
// Q3Rally Code END
} teamgame_t;

teamgame_t teamgame;

gentity_t	*neutralObelisk;

void Team_SetFlagStatus( int team, flagStatus_t status );
// Q3Rally Code Start
void Team_SetSigilStatus( int sigilNum, sigilStatus_t status );
void Init_Sigils( void );
// Q3Rally Code END

void Team_InitGame( void ) {
	int i;

	memset(&teamgame, 0, sizeof teamgame);

	switch( g_gametype.integer ) {
	case GT_CTF:
		teamgame.flagStatus[TEAM_RED] = -1; // Invalid to force update
		Team_SetFlagStatus( TEAM_RED, FLAG_ATBASE );
		teamgame.flagStatus[TEAM_BLUE] = -1; // Invalid to force update
		Team_SetFlagStatus( TEAM_BLUE, FLAG_ATBASE );
		break;

	case GT_CTF4:
		teamgame.flagStatus[TEAM_RED] = -1; // Invalid to force update
		Team_SetFlagStatus( TEAM_RED, FLAG_ATBASE );
		teamgame.flagStatus[TEAM_BLUE] = -1; // Invalid to force update
		Team_SetFlagStatus( TEAM_BLUE, FLAG_ATBASE );
		teamgame.flagStatus[TEAM_GREEN] = -1; // Invalid to force update
		Team_SetFlagStatus( TEAM_GREEN, FLAG_ATBASE );
		teamgame.flagStatus[TEAM_YELLOW] = -1; // Invalid to force update
		Team_SetFlagStatus( TEAM_YELLOW, FLAG_ATBASE );
		break;
        		
// Q3Rally Code Start
		
	case GT_DOMINATION:
	  Init_Sigils();
	  for ( i = 0; i < teamgame.numSigils; i++ ) {
	    teamgame.sigil[i].status = -1; // Invalid to force update
	    Team_SetSigilStatus( i, SIGIL_ISWHITE );
	  }
	  for ( ; i < MAX_SIGILS; i++ ) {
	    teamgame.sigil[i].status = -1; // Invalid to force update
	    Team_SetSigilStatus( i, SIGIL_NONE );
	  }
	  break;
	  
// Q3Rally Code END

#ifdef MISSIONPACK
	case GT_1FCTF:
		teamgame.flagStatus[TEAM_FREE] = -1; // Invalid to force update
		Team_SetFlagStatus( TEAM_FREE, FLAG_ATBASE );
		break;
#endif



	default:
		break;
	}
}

// Q3Rally Code Start
void Team_EndGame( void ) {
	// stop adding score when intermission starts
	if ( g_gametype.integer == GT_DOMINATION ) {
		int i;
		for ( i = 0; i < teamgame.numSigils; i++ ) {
			if( teamgame.sigil[i].entity ) {
				teamgame.sigil[i].entity->nextthink = 0;
			}
		}
	}
}
// Q3Rally Code END

int OtherTeam(int team) {
	if (team==TEAM_RED)
		return TEAM_BLUE;
	else if (team==TEAM_BLUE)
		return TEAM_RED;
	return team;
}

const char *TeamName(int team)  {
	if (team==TEAM_RED)
		return "RED";
	else if (team==TEAM_BLUE)
		return "BLUE";
// STONELANCE
	else if (team==TEAM_GREEN)
		return "GREEN";
	else if (team==TEAM_YELLOW)
		return "YELLOW";
// END
	else if (team==TEAM_SPECTATOR)
		return "SPECTATOR";
	return "FREE";
}

const char *OtherTeamName(int team) {
	if (team==TEAM_RED)
		return "BLUE";
	else if (team==TEAM_BLUE)
		return "RED";
	else if (team==TEAM_SPECTATOR)
		return "SPECTATOR";
	return "FREE";
}

const char *TeamColorString(int team) {
	if (team==TEAM_RED)
		return S_COLOR_RED;
	else if (team==TEAM_BLUE)
		return S_COLOR_BLUE;
// STONELANCE
	else if (team==TEAM_GREEN)
		return S_COLOR_GREEN;
	else if (team==TEAM_YELLOW)
		return S_COLOR_YELLOW;
// END
	else if (team==TEAM_SPECTATOR)
		return S_COLOR_YELLOW;
	return S_COLOR_WHITE;
}

// NULL for everyone
static __attribute__ ((format (printf, 2, 3))) void QDECL PrintMsg( gentity_t *ent, const char *fmt, ... ) {
	char		msg[1024];
	va_list		argptr;
	char		*p;
	
	va_start (argptr,fmt);
	if (Q_vsnprintf (msg, sizeof(msg), fmt, argptr) >= sizeof(msg)) {
		G_Error ( "PrintMsg overrun" );
	}
	va_end (argptr);

	// double quotes are bad
	while ((p = strchr(msg, '"')) != NULL)
		*p = '\'';

	trap_SendServerCommand ( ( (ent == NULL) ? -1 : ent-g_entities ), va("print \"%s\"", msg ));
}

/*
==============
AddTeamScore

 used for gametype > GT_TEAM
 for gametype GT_TEAM the level.teamScores is updated in AddScore in g_combat.c
==============
*/
void AddTeamScore(vec3_t origin, int team, int score) {
	gentity_t	*te;

	te = G_TempEntity(origin, EV_GLOBAL_TEAM_SOUND );
	te->r.svFlags |= SVF_BROADCAST;

        if ( g_gametype.integer == GT_CTF || g_gametype.integer == GT_CTF4 ) {
                int previousScore = level.teamScores[team];
                int newScore = previousScore + score;

                if ( g_gametype.integer == GT_CTF4 ) {
                        int maxOtherBefore = 0;
                        qboolean haveOpponent = qfalse;
                        int i;

                        for ( i = TEAM_RED; i < TEAM_NUM_TEAMS; ++i ) {
                                if ( i == TEAM_FREE || i == TEAM_SPECTATOR || i == team ) {
                                        continue;
                                }

                                if ( !haveOpponent || level.teamScores[i] > maxOtherBefore ) {
                                        maxOtherBefore = level.teamScores[i];
                                        haveOpponent = qtrue;
                                }
                        }

                        if ( !haveOpponent ) {
                                maxOtherBefore = 0;
                        }

                        {
                                qboolean wasLeading = haveOpponent ? (previousScore > maxOtherBefore) : qfalse;
                                qboolean wasTiedForLead = haveOpponent ? (previousScore == maxOtherBefore) : qfalse;
                                int scoreEvent;
                                char statusBuffer[64] = { 0 };
                                const char *statusSuffix = "";

                                if (team == TEAM_RED) {
                                        scoreEvent = GTS_REDTEAM_SCORED;
                                } else if (team == TEAM_BLUE) {
                                        scoreEvent = GTS_BLUETEAM_SCORED;
                                } else if (team == TEAM_GREEN) {
                                        scoreEvent = GTS_GREENTEAM_SCORED;
                                } else if (team == TEAM_YELLOW) {
                                        scoreEvent = GTS_YELLOWTEAM_SCORED;
                                } else {
                                        scoreEvent = GTS_TEAMS_ARE_TIED;
                                }

                                if (haveOpponent && newScore == maxOtherBefore) {
                                        te->s.eventParm = GTS_TEAMS_ARE_TIED;
                                } else if (haveOpponent && newScore > maxOtherBefore && !wasLeading) {
                                        if (team == TEAM_RED) {
                                                te->s.eventParm = GTS_REDTEAM_TOOK_LEAD;
                                        } else if (team == TEAM_BLUE) {
                                                te->s.eventParm = GTS_BLUETEAM_TOOK_LEAD;
                                        } else if (team == TEAM_GREEN) {
                                                te->s.eventParm = GTS_GREENTEAM_TOOK_LEAD;
                                        } else if (team == TEAM_YELLOW) {
                                                te->s.eventParm = GTS_YELLOWTEAM_TOOK_LEAD;
                                        } else {
                                                te->s.eventParm = scoreEvent;
                                        }
                                } else {
                                        te->s.eventParm = scoreEvent;
                                }

                                level.teamScores[team] = newScore;

                                if (haveOpponent && newScore > maxOtherBefore && !wasLeading) {
                                        Com_sprintf(statusBuffer, sizeof(statusBuffer), " %stakes the lead!^7", TeamColorString(team));
                                        statusSuffix = statusBuffer;
                                } else if (haveOpponent && newScore == maxOtherBefore && !wasTiedForLead) {
                                        Q_strncpyz(statusBuffer, " ^7tie for the lead.", sizeof(statusBuffer));
                                        statusSuffix = statusBuffer;
                                }

                                CenterPrint_All( va("%s%s^7 scores! ^7(%i)%s", TeamColorString(team), TeamName(team), newScore, statusSuffix) );
                                PrintMsg( NULL, "%s%s^7 scores! ^7(%i)%s\n", TeamColorString(team), TeamName(team), newScore, statusSuffix );
                        }

                        return;
                }

                {
                        int other_team_score;
                        int other_team = (team == TEAM_RED) ? TEAM_BLUE : TEAM_RED;
                        other_team_score = level.teamScores[other_team];

                        if ( newScore == other_team_score ) {
                                te->s.eventParm = GTS_TEAMS_ARE_TIED;
                        }
                        else if ( previousScore <= other_team_score && newScore > other_team_score ) {
                                if (team == TEAM_RED) {
                                        te->s.eventParm = GTS_REDTEAM_TOOK_LEAD;
                                } else {
                                        te->s.eventParm = GTS_BLUETEAM_TOOK_LEAD;
                                }
                        }
                        else {
                                if (team == TEAM_RED) {
                                        te->s.eventParm = GTS_REDTEAM_SCORED;
                                } else {
                                        te->s.eventParm = GTS_BLUETEAM_SCORED;
                                }
                        }

                        level.teamScores[team] = newScore;
                        return;
                }
        }

        level.teamScores[ team ] += score;
}

/*
==============
OnSameTeam
==============
*/
qboolean OnSameTeam( gentity_t *ent1, gentity_t *ent2 ) {
	if ( !ent1->client || !ent2->client ) {
		return qfalse;
	}

	if ( g_gametype.integer < GT_TEAM ) {
		return qfalse;
	}

	if ( ent1->client->sess.sessionTeam == ent2->client->sess.sessionTeam ) {
		return qtrue;
	}

	return qfalse;
}

static char ctfFlagStatusRemap[] = { '0', '1', '*', '*', '2' };
static char oneFlagStatusRemap[] = { '0', '1', '2', '3', '4' };

                                           // Q3Rally Code Start
/*
========================
Init_Sigils
========================
*/
void Init_Sigils( void ) {
    gentity_t     *point = NULL;
    teamgame.numSigils = 0;
    for (point = g_entities; point < &g_entities[level.num_entities] ;
    point++)
    {
        if (!point->inuse)
            continue;
            
        if (!Q_stricmp(point->classname, "team_domination_sigil")) {
            teamgame.sigil[teamgame.numSigils].entity = point;
            teamgame.numSigils++;
            if ( teamgame.numSigils == MAX_SIGILS )
                return;
            }
        }
    }
/*    
===============================
Team_SetSigilStatus
===============================
*/
void Team_SetSigilStatus( int sigilNum, sigilStatus_t status ) {
        qboolean modified = qfalse;

        // This can happen if there are too many sigils on the map.
        if ( sigilNum >= MAX_SIGILS ) {
                return;
        }

                // update only the sigil modified
                if( teamgame.sigil[sigilNum].status != status )
                {
                    teamgame.sigil[sigilNum].status = status;
                    modified = qtrue;
                }
    
    
    if( modified ) {
            char st[MAX_SIGILS+1];
            int i;
            
            //send all sigils' status to the configstring
            for ( i = 0; i < teamgame.numSigils; i++ ) {
                st[i] = '0' + teamgame.sigil[i].status;
            }
            st[teamgame.numSigils] = 0;
            
            trap_SetConfigstring( CS_SIGILSTATUS, st );
            
          }
        }
/*
==============================
ValidateSigilsInMap
==============================
*/

#define FRADIUS 800

void ValidateSigilsInMap( gentity_t *ent )
{
      vec3_t      start, end, temp, mins, maxs, tvec, offset = {FRADIUS, FRADIUS, FRADIUS};
      int         numEnts, i, touch[MAX_GENTITIES];
      gentity_t   *tent, *targ = NULL;
      float       vlen, closest_dist = FRADIUS;
      gitem_t     *item;

      // if 3rd sigil exists, this function doesn't need to run
      if (teamgame.numSigils != 2 || MAX_SIGILS < 3) {
          G_FreeEntity(ent);
          return;
      }
          
      VectorCopy(teamgame.sigil[0].entity->r.currentOrigin, start);
      VectorCopy(teamgame.sigil[1].entity->r.currentOrigin, end);
      VectorSubtract(start, end, temp);
      VectorScale(temp, 0.5, temp);
      VectorAdd(end, temp, temp);
      
      VectorCopy(temp, mins);
      VectorCopy(temp, maxs);
      VectorAdd(maxs, offset, maxs);
      VectorScale(offset, -1, offset);
      VectorAdd(mins, offset, mins);
      
      numEnts = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
 
      for ( i=0 ; i<numEnts ; i++ )
      {
          tent = &g_entities[touch[i]];
          
          if (!tent->item)
            continue;
            
          if (!(tent->item->giType == IT_HEALTH || tent->item->giType == IT_ARMOR || tent->item->giType == IT_WEAPON))
            continue;
            
          VectorSubtract(temp, tent->r.currentOrigin, tvec);
          vlen = VectorLength(tvec);
      
          if (vlen < closest_dist) {
              closest_dist = vlen;
              targ = tent;
          }
      }

      if (targ) {
          item = BG_FindItemForPowerup(PW_SIGILWHITE);
          targ->s.modelindex = item - bg_itemlist;
          targ->classname = item->classname;
          targ->item = item;
          targ->r.svFlags = SVF_BROADCAST;
          targ->s.powerups = PW_SIGILWHITE;
          targ->count = 0;
          teamgame.sigil[teamgame.numSigils].entity = targ;
          teamgame.numSigils++;
      }
      // kill the entity that does the spawn conversions
      G_FreeEntity(ent);
  }
  

  
// Q3Rally Code END

void Team_SetFlagStatus( int team, flagStatus_t status ) {
	qboolean modified = qfalse;

	if ( team >= TEAM_FREE && team < TEAM_NUM_TEAMS ) {
		if ( teamgame.flagStatus[team] != status ) {
			teamgame.flagStatus[team] = status;
			modified = qtrue;
		}
	}

	if( modified ) {
		char st[6];

		if( g_gametype.integer == GT_CTF ) {
			st[0] = ctfFlagStatusRemap[teamgame.flagStatus[TEAM_RED]];
			st[1] = ctfFlagStatusRemap[teamgame.flagStatus[TEAM_BLUE]];
			st[2] = 0;
		} else if ( g_gametype.integer == GT_CTF4 ) {
			st[0] = ctfFlagStatusRemap[teamgame.flagStatus[TEAM_RED]];
			st[1] = ctfFlagStatusRemap[teamgame.flagStatus[TEAM_BLUE]];
			st[2] = ctfFlagStatusRemap[teamgame.flagStatus[TEAM_GREEN]];
			st[3] = ctfFlagStatusRemap[teamgame.flagStatus[TEAM_YELLOW]];
			st[4] = 0;
		}
		else {		// GT_1FCTF
			st[0] = oneFlagStatusRemap[teamgame.flagStatus[TEAM_FREE]];
			st[1] = 0;
		}

		trap_SetConfigstring( CS_FLAGSTATUS, st );
	}
}

void Team_CheckDroppedItem( gentity_t *dropped ) {
	if( dropped->item->giTag == PW_REDFLAG ) {
		Team_SetFlagStatus( TEAM_RED, FLAG_DROPPED );
	}
	else if( dropped->item->giTag == PW_BLUEFLAG ) {
		Team_SetFlagStatus( TEAM_BLUE, FLAG_DROPPED );
	}
	else if( dropped->item->giTag == PW_GREENFLAG ) {
		Team_SetFlagStatus( TEAM_GREEN, FLAG_DROPPED );
	}
	else if( dropped->item->giTag == PW_YELLOWFLAG ) {
		Team_SetFlagStatus( TEAM_YELLOW, FLAG_DROPPED );
	}
	else if( dropped->item->giTag == PW_NEUTRALFLAG ) {
		Team_SetFlagStatus( TEAM_FREE, FLAG_DROPPED );
	}
}

/*
================
Team_ForceGesture
================
*/
void Team_ForceGesture(int team) {
	int i;
	gentity_t *ent;

	for (i = 0; i < MAX_CLIENTS; i++) {
		ent = &g_entities[i];
		if (!ent->inuse)
			continue;
		if (!ent->client)
			continue;
		if (ent->client->sess.sessionTeam != team)
			continue;
		//
		ent->flags |= FL_FORCE_GESTURE;
	}
}

/*
================
Team_FragBonuses

Calculate the bonuses for flag defense, flag carrier defense, etc.
Note that bonuses are not cumulative.  You get one, they are in importance
order.
================
*/
void Team_FragBonuses(gentity_t *targ, gentity_t *inflictor, gentity_t *attacker)
{
	int i;
	gentity_t *ent;
	int flag_pw = 0, enemy_flag_pw = 0;
	int otherteam;
	int tokens;
	gentity_t *flag, *carrier = NULL;
	char *c;
	vec3_t v1, v2;
	int team;

	// no bonus for fragging yourself or team mates
	if (!targ->client || !attacker->client || targ == attacker || OnSameTeam(targ, attacker))
		return;

	team = targ->client->sess.sessionTeam;
	otherteam = OtherTeam(targ->client->sess.sessionTeam);
	if (otherteam < 0 && g_gametype.integer < GT_CTF4) // g_gametype is not 4 team
		return; // whoever died isn't on a team

	// same team, if the flag at base, check to he has the enemy flag
	if ( g_gametype.integer == GT_CTF4 ) {
		if (attacker->client->sess.sessionTeam == TEAM_RED) flag_pw = PW_REDFLAG;
		else if (attacker->client->sess.sessionTeam == TEAM_BLUE) flag_pw = PW_BLUEFLAG;
		else if (attacker->client->sess.sessionTeam == TEAM_GREEN) flag_pw = PW_GREENFLAG;
		else if (attacker->client->sess.sessionTeam == TEAM_YELLOW) flag_pw = PW_YELLOWFLAG;

		if ( targ->client->ps.powerups[PW_REDFLAG] && team != TEAM_RED ) enemy_flag_pw = PW_REDFLAG;
		else if ( targ->client->ps.powerups[PW_BLUEFLAG] && team != TEAM_BLUE ) enemy_flag_pw = PW_BLUEFLAG;
		else if ( targ->client->ps.powerups[PW_GREENFLAG] && team != TEAM_GREEN ) enemy_flag_pw = PW_GREENFLAG;
		else if ( targ->client->ps.powerups[PW_YELLOWFLAG] && team != TEAM_YELLOW ) enemy_flag_pw = PW_YELLOWFLAG;
	} else { // GT_CTF
		if (team == TEAM_RED) {
			flag_pw = PW_REDFLAG;
			enemy_flag_pw = PW_BLUEFLAG;
		} else {
			flag_pw = PW_BLUEFLAG;
			enemy_flag_pw = PW_REDFLAG;
		}
	}

#ifdef MISSIONPACK
	if (g_gametype.integer == GT_1FCTF) {
		flag_pw = PW_NEUTRALFLAG;
		enemy_flag_pw = PW_NEUTRALFLAG;
	} 
#endif

	// did the attacker frag the flag carrier?
	tokens = 0;
#ifdef MISSIONPACK
	if( g_gametype.integer == GT_HARVESTER ) {
		tokens = targ->client->ps.generic1;
	}
#endif
	if (targ->client->ps.powerups[enemy_flag_pw]) {
		attacker->client->pers.teamState.lastfraggedcarrier = level.time;
		AddScore(attacker, targ->r.currentOrigin, CTF_FRAG_CARRIER_BONUS);
		attacker->client->pers.teamState.fragcarrier++;
		PrintMsg(NULL, "%s" S_COLOR_WHITE " fragged %s's flag carrier!\n",
			attacker->client->pers.netname, TeamName(team));

		// the target had the flag, clear the hurt carrier
		// field on all players
		for (i = 0; i < g_maxclients.integer; i++) {
			ent = g_entities + i;
			if (ent->inuse && ent->client)
				ent->client->pers.teamState.lasthurtcarrier = 0;
		}
		return;
	}

	// did the attacker frag a head carrier? other->client->ps.generic1
	if (tokens) {
		attacker->client->pers.teamState.lastfraggedcarrier = level.time;
		AddScore(attacker, targ->r.currentOrigin, CTF_FRAG_CARRIER_BONUS * tokens * tokens);
		attacker->client->pers.teamState.fragcarrier++;
		PrintMsg(NULL, "%s" S_COLOR_WHITE " fragged %s's skull carrier!\n",
			attacker->client->pers.netname, TeamName(team));

		// the target had the flag, clear the hurt carrier
		// field on the other team
		for (i = 0; i < g_maxclients.integer; i++) {
			ent = g_entities + i;
			if (ent->inuse && ent->client->sess.sessionTeam == otherteam)
				ent->client->pers.teamState.lasthurtcarrier = 0;
		}
		return;
	}

	if (targ->client->pers.teamState.lasthurtcarrier &&
		level.time - targ->client->pers.teamState.lasthurtcarrier < CTF_CARRIER_DANGER_PROTECT_TIMEOUT &&
		!attacker->client->ps.powerups[flag_pw]) {
		// attacker is on the same team as the flag carrier and
		// fragged a guy who hurt our flag carrier
		AddScore(attacker, targ->r.currentOrigin, CTF_CARRIER_DANGER_PROTECT_BONUS);

		attacker->client->pers.teamState.carrierdefense++;
		targ->client->pers.teamState.lasthurtcarrier = 0;

		attacker->client->ps.persistant[PERS_DEFEND_COUNT]++;
		// add the sprite over the player's head
		attacker->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
		attacker->client->ps.eFlags |= EF_AWARD_DEFEND;
		attacker->client->rewardTime = level.time + REWARD_SPRITE_TIME;

		return;
	}

	// flag and flag carrier area defense bonuses

	// we have to find the flag and carrier entities

#ifdef MISSIONPACK	
	if( g_gametype.integer == GT_OBELISK ) {
		// find the team obelisk
		switch (attacker->client->sess.sessionTeam) {
		case TEAM_RED:
			c = "team_redobelisk";
			break;
		case TEAM_BLUE:
			c = "team_blueobelisk";
			break;		
		default:
			return;
		}
		
	} else if (g_gametype.integer == GT_HARVESTER ) {
		// find the center obelisk
		c = "team_neutralobelisk";
	} else {
#endif
	// find the flag
	switch (attacker->client->sess.sessionTeam) {
	case TEAM_RED:
		c = "team_CTF_redflag";
		break;
	case TEAM_BLUE:
		c = "team_CTF_blueflag";
		break;
	case TEAM_GREEN:
		c = "team_CTF_greenflag";
		break;
	case TEAM_YELLOW:
		c = "team_CTF_yellowflag";
		break;
	default:
		return;
	}
	// find attacker's team's flag carrier
	for (i = 0; i < g_maxclients.integer; i++) {
		carrier = g_entities + i;
		if (carrier->inuse && carrier->client->ps.powerups[flag_pw])
			break;
		carrier = NULL;
	}
#ifdef MISSIONPACK
	}
#endif
	flag = NULL;
	while ((flag = G_Find (flag, FOFS(classname), c)) != NULL) {
		if (!(flag->flags & FL_DROPPED_ITEM))
			break;
	}

	if (!flag)
		return; // can't find attacker's flag

	// ok we have the attackers flag and a pointer to the carrier

	// check to see if we are defending the base's flag
	VectorSubtract(targ->r.currentOrigin, flag->r.currentOrigin, v1);
	VectorSubtract(attacker->r.currentOrigin, flag->r.currentOrigin, v2);

	if ( ( ( VectorLength(v1) < CTF_TARGET_PROTECT_RADIUS &&
		trap_InPVS(flag->r.currentOrigin, targ->r.currentOrigin ) ) ||
		( VectorLength(v2) < CTF_TARGET_PROTECT_RADIUS &&
		trap_InPVS(flag->r.currentOrigin, attacker->r.currentOrigin ) ) ) &&
		attacker->client->sess.sessionTeam != targ->client->sess.sessionTeam) {

		// we defended the base flag
		AddScore(attacker, targ->r.currentOrigin, CTF_FLAG_DEFENSE_BONUS);
		attacker->client->pers.teamState.basedefense++;

		attacker->client->ps.persistant[PERS_DEFEND_COUNT]++;
		// add the sprite over the player's head
		attacker->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
		attacker->client->ps.eFlags |= EF_AWARD_DEFEND;
		attacker->client->rewardTime = level.time + REWARD_SPRITE_TIME;

		return;
	}

	if (carrier && carrier != attacker) {
		VectorSubtract(targ->r.currentOrigin, carrier->r.currentOrigin, v1);
		VectorSubtract(attacker->r.currentOrigin, carrier->r.currentOrigin, v2);

		if ( ( ( VectorLength(v1) < CTF_ATTACKER_PROTECT_RADIUS &&
			trap_InPVS(carrier->r.currentOrigin, targ->r.currentOrigin ) ) ||
			( VectorLength(v2) < CTF_ATTACKER_PROTECT_RADIUS &&
				trap_InPVS(carrier->r.currentOrigin, attacker->r.currentOrigin ) ) ) &&
			attacker->client->sess.sessionTeam != targ->client->sess.sessionTeam) {
			AddScore(attacker, targ->r.currentOrigin, CTF_CARRIER_PROTECT_BONUS);
			attacker->client->pers.teamState.carrierdefense++;

			attacker->client->ps.persistant[PERS_DEFEND_COUNT]++;
			// add the sprite over the player's head
			attacker->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
			attacker->client->ps.eFlags |= EF_AWARD_DEFEND;
			attacker->client->rewardTime = level.time + REWARD_SPRITE_TIME;

			return;
		}
	}
}

/*
================
Team_CheckHurtCarrier

Check to see if attacker hurt the flag carrier.  Needed when handing out bonuses for assistance to flag
carrier defense.
================
*/
void Team_CheckHurtCarrier(gentity_t *targ, gentity_t *attacker)
{
	int flag_pw = 0;

	if (!targ->client || !attacker->client)
		return;

	if ( g_gametype.integer == GT_CTF4 ) {
		if ( targ->client->ps.powerups[PW_REDFLAG] && targ->client->sess.sessionTeam != TEAM_RED ) {
			flag_pw = PW_REDFLAG;
		} else if ( targ->client->ps.powerups[PW_BLUEFLAG] && targ->client->sess.sessionTeam != TEAM_BLUE ) {
			flag_pw = PW_BLUEFLAG;
		} else if ( targ->client->ps.powerups[PW_GREENFLAG] && targ->client->sess.sessionTeam != TEAM_GREEN ) {
			flag_pw = PW_GREENFLAG;
		} else if ( targ->client->ps.powerups[PW_YELLOWFLAG] && targ->client->sess.sessionTeam != TEAM_YELLOW ) {
			flag_pw = PW_YELLOWFLAG;
		}
	} else { // GT_CTF
		if (targ->client->sess.sessionTeam == TEAM_RED) {
			flag_pw = PW_BLUEFLAG;
		} else {
			flag_pw = PW_REDFLAG;
		}
	}

#ifdef MISSIONPACK
	if (g_gametype.integer == GT_1FCTF) {
		flag_pw = PW_NEUTRALFLAG;
	}
#endif

	// flags
	if ( flag_pw && targ->client->ps.powerups[flag_pw] &&
		targ->client->sess.sessionTeam != attacker->client->sess.sessionTeam)
		attacker->client->pers.teamState.lasthurtcarrier = level.time;

	// skulls
	if (targ->client->ps.generic1 &&
		targ->client->sess.sessionTeam != attacker->client->sess.sessionTeam)
		attacker->client->pers.teamState.lasthurtcarrier = level.time;
}


gentity_t *Team_ResetFlag( int team ) {
	char *c;
	gentity_t *ent, *rent = NULL;

	switch (team) {
	case TEAM_RED:
		c = "team_CTF_redflag";
		break;
	case TEAM_BLUE:
		c = "team_CTF_blueflag";
		break;
	case TEAM_GREEN:
		c = "team_CTF_greenflag";
		break;
	case TEAM_YELLOW:
		c = "team_CTF_yellowflag";
		break;
	case TEAM_FREE:
		c = "team_CTF_neutralflag";
		break;
	default:
		return NULL;
	}

	ent = NULL;
	while ((ent = G_Find (ent, FOFS(classname), c)) != NULL) {
		if (ent->flags & FL_DROPPED_ITEM)
			G_FreeEntity(ent);
		else {
			rent = ent;
			RespawnItem(ent);
		}
	}

	Team_SetFlagStatus( team, FLAG_ATBASE );

	return rent;
}

void Team_ResetFlags( void ) {
	if( g_gametype.integer == GT_CTF ) {
		Team_ResetFlag( TEAM_RED );
		Team_ResetFlag( TEAM_BLUE );
	} else if ( g_gametype.integer == GT_CTF4 ) {
		Team_ResetFlag( TEAM_RED );
		Team_ResetFlag( TEAM_BLUE );
		Team_ResetFlag( TEAM_GREEN );
		Team_ResetFlag( TEAM_YELLOW );
	}
#ifdef MISSIONPACK
	else if( g_gametype.integer == GT_1FCTF ) {
		Team_ResetFlag( TEAM_FREE );
	}
#endif
}

void Team_ReturnFlagSound( gentity_t *ent, int team ) {
	gentity_t	*te;

	if (ent == NULL) {
		G_Printf ("Warning:  NULL passed to Team_ReturnFlagSound\n");
		return;
	}

        te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_TEAM_SOUND );
        switch ( team ) {
        case TEAM_BLUE:
                te->s.eventParm = GTS_RED_RETURN;
                break;
        case TEAM_RED:
                te->s.eventParm = GTS_BLUE_RETURN;
                break;
        case TEAM_GREEN:
                te->s.eventParm = GTS_GREEN_RETURN;
                break;
        case TEAM_YELLOW:
                te->s.eventParm = GTS_YELLOW_RETURN;
                break;
        default:
                te->s.eventParm = GTS_BLUE_RETURN;
                break;
        }
        te->r.svFlags |= SVF_BROADCAST;
}

void Team_TakeFlagSound( gentity_t *ent, int team ) {
	gentity_t	*te;

	if (ent == NULL) {
		G_Printf ("Warning:  NULL passed to Team_TakeFlagSound\n");
		return;
	}

	// only play sound when the flag was at the base
	// or not picked up the last 10 seconds
	if ( teamgame.flagStatus[team] != FLAG_ATBASE ) {
		if ( teamgame.flagTakenTime[team] > level.time - 10000 ) {
			return;
		}
	}
	teamgame.flagTakenTime[team] = level.time;

        te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_TEAM_SOUND );
        switch ( team ) {
        case TEAM_BLUE:
                te->s.eventParm = GTS_RED_TAKEN;
                break;
        case TEAM_RED:
                te->s.eventParm = GTS_BLUE_TAKEN;
                break;
        case TEAM_GREEN:
                te->s.eventParm = GTS_GREEN_TAKEN;
                break;
        case TEAM_YELLOW:
                te->s.eventParm = GTS_YELLOW_TAKEN;
                break;
        default:
                te->s.eventParm = GTS_BLUE_TAKEN;
                break;
        }
        te->r.svFlags |= SVF_BROADCAST;
}

void Team_CaptureFlagSound( gentity_t *ent, int team ) {
	gentity_t	*te;

	if (ent == NULL) {
		G_Printf ("Warning:  NULL passed to Team_CaptureFlagSound\n");
		return;
	}

        te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_TEAM_SOUND );
        switch ( team ) {
        case TEAM_BLUE:
                te->s.eventParm = GTS_BLUE_CAPTURE;
                break;
        case TEAM_RED:
                te->s.eventParm = GTS_RED_CAPTURE;
                break;
        case TEAM_GREEN:
                te->s.eventParm = GTS_GREEN_CAPTURE;
                break;
        case TEAM_YELLOW:
                te->s.eventParm = GTS_YELLOW_CAPTURE;
                break;
        default:
                te->s.eventParm = GTS_RED_CAPTURE;
                break;
        }
        te->r.svFlags |= SVF_BROADCAST;
}

void Team_ReturnFlag( int team ) {
	Team_ReturnFlagSound(Team_ResetFlag(team), team);
	if( team == TEAM_FREE ) {
		PrintMsg(NULL, "The flag has returned!\n" );
	}
	else {
		PrintMsg(NULL, "The %s flag has returned!\n", TeamName(team));
	}
}

void Team_FreeEntity( gentity_t *ent ) {
	if( ent->item->giTag == PW_REDFLAG ) {
		Team_ReturnFlag( TEAM_RED );
	}
	else if( ent->item->giTag == PW_BLUEFLAG ) {
		Team_ReturnFlag( TEAM_BLUE );
	}
	else if( ent->item->giTag == PW_GREENFLAG ) {
		Team_ReturnFlag( TEAM_GREEN );
	}
	else if( ent->item->giTag == PW_YELLOWFLAG ) {
		Team_ReturnFlag( TEAM_YELLOW );
	}
	else if( ent->item->giTag == PW_NEUTRALFLAG ) {
		Team_ReturnFlag( TEAM_FREE );
	}
}

/*
==============
Team_DroppedFlagThink

Automatically set in Launch_Item if the item is one of the flags

Flags are unique in that if they are dropped, the base flag must be respawned when they time out
==============
*/
void Team_DroppedFlagThink(gentity_t *ent) {
	int		team = TEAM_FREE;

	if( ent->item->giTag == PW_REDFLAG ) {
		team = TEAM_RED;
	}
	else if( ent->item->giTag == PW_BLUEFLAG ) {
		team = TEAM_BLUE;
	}
	else if( ent->item->giTag == PW_GREENFLAG ) {
		team = TEAM_GREEN;
	}
	else if( ent->item->giTag == PW_YELLOWFLAG ) {
		team = TEAM_YELLOW;
	}
	else if( ent->item->giTag == PW_NEUTRALFLAG ) {
		team = TEAM_FREE;
	}

	Team_ReturnFlagSound( Team_ResetFlag( team ), team );
	// Reset Flag will delete this entity
}


/*
==============
Team_DroppedFlagThink
==============
*/
int Team_TouchOurFlag( gentity_t *ent, gentity_t *other, int team ) {
	int			i;
	gentity_t	*player;
	gclient_t	*cl = other->client;
	int			enemy_flag = 0;

	if ( g_gametype.integer == GT_CTF4 ) {
		if ( cl->ps.powerups[PW_REDFLAG] && cl->sess.sessionTeam != TEAM_RED) {
			enemy_flag = PW_REDFLAG;
		} else if ( cl->ps.powerups[PW_BLUEFLAG] && cl->sess.sessionTeam != TEAM_BLUE) {
			enemy_flag = PW_BLUEFLAG;
		} else if ( cl->ps.powerups[PW_GREENFLAG] && cl->sess.sessionTeam != TEAM_GREEN) {
			enemy_flag = PW_GREENFLAG;
		} else if ( cl->ps.powerups[PW_YELLOWFLAG] && cl->sess.sessionTeam != TEAM_YELLOW) {
			enemy_flag = PW_YELLOWFLAG;
		}
	}
#ifdef MISSIONPACK
	else if( g_gametype.integer == GT_1FCTF ) {
		enemy_flag = PW_NEUTRALFLAG;
	}
#endif
	else { // GT_CTF
		if (cl->sess.sessionTeam == TEAM_RED) {
			enemy_flag = PW_BLUEFLAG;
		} else {
			enemy_flag = PW_REDFLAG;
		}
	}


	if ( ent->flags & FL_DROPPED_ITEM ) {
		// hey, it's not home.  return it by teleporting it back
		PrintMsg( NULL, "%s" S_COLOR_WHITE " returned the %s flag!\n", 
			cl->pers.netname, TeamName(team));
		AddScore(other, ent->r.currentOrigin, CTF_RECOVERY_BONUS);
		other->client->pers.teamState.flagrecovery++;
		other->client->pers.teamState.lastreturnedflag = level.time;
		//ResetFlag will remove this entity!  We must return zero
		Team_ReturnFlagSound(Team_ResetFlag(team), team);
		return 0;
	}

	// the flag is at home base.  if the player has the enemy
	// flag, he's just won!
	if ( !enemy_flag || !cl->ps.powerups[enemy_flag] ) {
		return 0; // We don't have the flag
	}
#ifdef MISSIONPACK
	if( g_gametype.integer == GT_1FCTF ) {
		PrintMsg( NULL, "%s" S_COLOR_WHITE " captured the flag!\n", cl->pers.netname );
	}
	else {
#endif
	PrintMsg( NULL, "%s" S_COLOR_WHITE " captured the %s flag!\n", cl->pers.netname, TeamName(OtherTeam(team)));
#ifdef MISSIONPACK
	}
#endif

	cl->ps.powerups[enemy_flag] = 0;

	teamgame.last_flag_capture = level.time;
	teamgame.last_capture_team = team;

	// Increase the team's score
	AddTeamScore(ent->s.pos.trBase, other->client->sess.sessionTeam, 1);
	Team_ForceGesture(other->client->sess.sessionTeam);

	other->client->pers.teamState.captures++;
	// add the sprite over the player's head
	other->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
	other->client->ps.eFlags |= EF_AWARD_CAP;
	other->client->rewardTime = level.time + REWARD_SPRITE_TIME;
        other->client->ps.persistant[PERS_CAPTURES]++;
        G_Profile_RecordFlagCapture( other->client );

	// other gets another 10 frag bonus
	AddScore(other, ent->r.currentOrigin, CTF_CAPTURE_BONUS);

	Team_CaptureFlagSound( ent, team );

	// Ok, let's do the player loop, hand out the bonuses
	for (i = 0; i < g_maxclients.integer; i++) {
		player = &g_entities[i];

		// also make sure we don't award assist bonuses to the flag carrier himself.
		if (!player->inuse || player == other)
			continue;

		if (player->client->sess.sessionTeam !=
			cl->sess.sessionTeam) {
			player->client->pers.teamState.lasthurtcarrier = -5;
		} else if (player->client->sess.sessionTeam ==
			cl->sess.sessionTeam) {
#ifdef MISSIONPACK
			AddScore(player, ent->r.currentOrigin, CTF_TEAM_BONUS);
#endif
			// award extra points for capture assists
			if (player->client->pers.teamState.lastreturnedflag + 
				CTF_RETURN_FLAG_ASSIST_TIMEOUT > level.time) {
				AddScore (player, ent->r.currentOrigin, CTF_RETURN_FLAG_ASSIST_BONUS);
                                other->client->pers.teamState.assists++;

                                player->client->ps.persistant[PERS_ASSIST_COUNT]++;
                                G_Profile_RecordFlagAssist( player->client );
				// add the sprite over the player's head
				player->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
				player->client->ps.eFlags |= EF_AWARD_ASSIST;
				player->client->rewardTime = level.time + REWARD_SPRITE_TIME;

			} 
			if (player->client->pers.teamState.lastfraggedcarrier + 
				CTF_FRAG_CARRIER_ASSIST_TIMEOUT > level.time) {
				AddScore(player, ent->r.currentOrigin, CTF_FRAG_CARRIER_ASSIST_BONUS);
                                other->client->pers.teamState.assists++;
                                player->client->ps.persistant[PERS_ASSIST_COUNT]++;
                                G_Profile_RecordFlagAssist( player->client );
				// add the sprite over the player's head
				player->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
				player->client->ps.eFlags |= EF_AWARD_ASSIST;
				player->client->rewardTime = level.time + REWARD_SPRITE_TIME;
			}
		}
	}
	Team_ResetFlags();

	CalculateRanks();

	return 0; // Do not respawn this automatically
}

int Team_TouchEnemyFlag( gentity_t *ent, gentity_t *other, int team ) {
	gclient_t *cl = other->client;

#ifdef MISSIONPACK
	if( g_gametype.integer == GT_1FCTF ) {
		PrintMsg (NULL, "%s" S_COLOR_WHITE " got the flag!\n", other->client->pers.netname );

		cl->ps.powerups[PW_NEUTRALFLAG] = INT_MAX; // flags never expire

		if( team == TEAM_RED ) {
			Team_SetFlagStatus( TEAM_FREE, FLAG_TAKEN_RED );
		}
		else {
			Team_SetFlagStatus( TEAM_FREE, FLAG_TAKEN_BLUE );
		}
	}
	else{
#endif
		PrintMsg (NULL, "%s" S_COLOR_WHITE " got the %s flag!\n",
			other->client->pers.netname, TeamName(team));

		if (team == TEAM_RED)
			cl->ps.powerups[PW_REDFLAG] = INT_MAX; // flags never expire
		else if (team == TEAM_BLUE)
			cl->ps.powerups[PW_BLUEFLAG] = INT_MAX; // flags never expire
		else if (team == TEAM_GREEN)
			cl->ps.powerups[PW_GREENFLAG] = INT_MAX;
		else if (team == TEAM_YELLOW)
			cl->ps.powerups[PW_YELLOWFLAG] = INT_MAX;

		Team_SetFlagStatus( team, FLAG_TAKEN );
#ifdef MISSIONPACK
	}

	AddScore(other, ent->r.currentOrigin, CTF_FLAG_BONUS);
#endif
	cl->pers.teamState.flagsince = level.time;
	Team_TakeFlagSound( ent, team );

	return -1; // Do not respawn this automatically, but do delete it if it was FL_DROPPED
}

int Pickup_Team( gentity_t *ent, gentity_t *other ) {
	int team;
	gclient_t *cl = other->client;

#ifdef MISSIONPACK
	if( g_gametype.integer == GT_OBELISK ) {
		// there are no team items that can be picked up in obelisk
		G_FreeEntity( ent );
		return 0;
	}

	if( g_gametype.integer == GT_HARVESTER ) {
		// the only team items that can be picked up in harvester are the cubes
		if( ent->spawnflags != cl->sess.sessionTeam ) {
			cl->ps.generic1 += 1;
		}
		G_FreeEntity( ent );
		return 0;
	}
#endif
	// figure out what team this flag is
	if( strcmp(ent->classname, "team_CTF_redflag") == 0 ) {
		team = TEAM_RED;
	}
	else if( strcmp(ent->classname, "team_CTF_blueflag") == 0 ) {
		team = TEAM_BLUE;
	}
	else if( strcmp(ent->classname, "team_CTF_greenflag") == 0 ) {
		team = TEAM_GREEN;
	}
	else if( strcmp(ent->classname, "team_CTF_yellowflag") == 0 ) {
		team = TEAM_YELLOW;
	}
#ifdef MISSIONPACK
	else if( strcmp(ent->classname, "team_CTF_neutralflag") == 0  ) {
		team = TEAM_FREE;
	}
#endif
	else {
		PrintMsg ( other, "Don't know what team the flag is on.\n");
		return 0;
	}
#ifdef MISSIONPACK
	if( g_gametype.integer == GT_1FCTF ) {
		if( team == TEAM_FREE ) {
			return Team_TouchEnemyFlag( ent, other, cl->sess.sessionTeam );
		}
		if( team != cl->sess.sessionTeam) {
			return Team_TouchOurFlag( ent, other, cl->sess.sessionTeam );
		}
		return 0;
	}
#endif
	// GT_CTF
	if( team == cl->sess.sessionTeam) {
		return Team_TouchOurFlag( ent, other, team );
	}
	return Team_TouchEnemyFlag( ent, other, team );
                                       }

// Q3Rally Code Start

/*
===================
Sigil_Think
===================
*/
void Sigil_Think( gentity_t *ent ) {
  team_t team;

  switch( ent->s.powerups ) {
    case PW_SIGILRED :
      team = TEAM_RED;
      break;

    case PW_SIGILBLUE :
      team = TEAM_BLUE;
      break;

    case PW_SIGILGREEN :
      team = TEAM_GREEN;
      break;

    case PW_SIGILYELLOW :
      team = TEAM_YELLOW;
      break;

    default :
      team = TEAM_RED;
  }

  ent->count = 0;
  level.teamScores[team]++;
  ent->nextthink = level.time + g_dominationScoreInterval.integer;

  // refresh scoreboard
  CalculateRanks();
}


void CaptureSigil(gentity_t *ent, int sigilNum, sigilStatus_t status, powerup_t powerup) {
    Team_SetSigilStatus(sigilNum, status);
    ent->nextthink = level.time - (level.time % 4000) + 4000;
    ent->think = Sigil_Think;
    ent->s.powerups = powerup;
    ent->s.modelindex = ITEM_INDEX( BG_FindItemForPowerup( powerup ) );
    ent->count = 1;
}

/*
====================================
Sigil_Touch
====================================
*/
int Sigil_Touch( gentity_t *ent, gentity_t *other ) {
    gclient_t *cl = other->client;
    int sigilNum = 0;
    powerup_t powerup = PW_NONE;
    sigilStatus_t status = SIGIL_NONE;
    
    if (!cl)
        return 0;
        
    if    (ent->count && ent->nextthink < level.time + g_dominationCaptureDelay.integer)    // protect against overflows by not counting
        return 0;
        
    // find the index of the sigil reffered by ent
    while ( sigilNum < MAX_SIGILS && teamgame.sigil[sigilNum].entity != ent )
          sigilNum++;

    switch (cl->sess.sessionTeam) {
        case TEAM_RED:
            powerup = PW_SIGILRED;
            status = SIGIL_ISRED;
            break;
        case TEAM_BLUE:
            powerup = PW_SIGILBLUE;
            status = SIGIL_ISBLUE;
            break;
        case TEAM_GREEN:
            powerup = PW_SIGILGREEN;
            status = SIGIL_ISGREEN;
            break;
        case TEAM_YELLOW:
            powerup = PW_SIGILYELLOW;
            status = SIGIL_ISYELLOW;
            break;
        default:
            return 0;
    }

    if (ent->s.powerups != powerup) {
        CaptureSigil(ent, sigilNum, status, powerup);
    }

    return 0;
  }

// Q3Rally Code END
/*
===========
Team_GetLocation

Report a location for the player. Uses placed nearby target_location entities
============
*/
gentity_t *Team_GetLocation(gentity_t *ent)
{
	gentity_t		*eloc, *best;
	float			bestlen, len;
	vec3_t			origin;

	best = NULL;
	bestlen = 3*8192.0*8192.0;

	VectorCopy( ent->r.currentOrigin, origin );

	for (eloc = level.locationHead; eloc; eloc = eloc->nextTrain) {
		len = ( origin[0] - eloc->r.currentOrigin[0] ) * ( origin[0] - eloc->r.currentOrigin[0] )
			+ ( origin[1] - eloc->r.currentOrigin[1] ) * ( origin[1] - eloc->r.currentOrigin[1] )
			+ ( origin[2] - eloc->r.currentOrigin[2] ) * ( origin[2] - eloc->r.currentOrigin[2] );

		if ( len > bestlen ) {
			continue;
		}

		if ( !trap_InPVS( origin, eloc->r.currentOrigin ) ) {
			continue;
		}

		bestlen = len;
		best = eloc;
	}

	return best;
}


/*
===========
Team_GetLocation

Report a location for the player. Uses placed nearby target_location entities
============
*/
qboolean Team_GetLocationMsg(gentity_t *ent, char *loc, int loclen)
{
	gentity_t *best;

	best = Team_GetLocation( ent );
	
	if (!best)
		return qfalse;

	if (best->count) {
		if (best->count < 0)
			best->count = 0;
		if (best->count > 7)
			best->count = 7;
		Com_sprintf(loc, loclen, "%c%c%s" S_COLOR_WHITE, Q_COLOR_ESCAPE, best->count + '0', best->message );
	} else
		Com_sprintf(loc, loclen, "%s", best->message);

	return qtrue;
}


/*---------------------------------------------------------------------------*/

/*
================
SelectRandomDeathmatchSpawnPoint

go to a random point that doesn't telefrag
================
*/
#define	MAX_TEAM_SPAWN_POINTS	32
gentity_t *SelectRandomTeamSpawnPoint( int teamstate, team_t team ) {
	gentity_t	*spot;
	int			count;
	int			selection;
	gentity_t	*spots[MAX_TEAM_SPAWN_POINTS];
	char		*classname;

	if (teamstate == TEAM_BEGIN) {
		if (team == TEAM_RED)
			classname = "team_CTF_redplayer";
		else if (team == TEAM_BLUE)
			classname = "team_CTF_blueplayer";
		else if (team == TEAM_GREEN)
			classname = "team_CTF_greenplayer";
		else if (team == TEAM_YELLOW)
			classname = "team_CTF_yellowplayer";
		else
			return NULL;
	} else {
		if (team == TEAM_RED)
			classname = "team_CTF_redspawn";
		else if (team == TEAM_BLUE)
			classname = "team_CTF_bluespawn";
		else if (team == TEAM_GREEN)
			classname = "team_CTF_greenspawn";
		else if (team == TEAM_YELLOW)
			classname = "team_CTF_yellowspawn";
		else
			return NULL;
	}
	count = 0;

	spot = NULL;

	while ((spot = G_Find (spot, FOFS(classname), classname)) != NULL) {
		if ( SpotWouldTelefrag( spot ) ) {
			continue;
		}
		spots[ count ] = spot;
		if (++count == MAX_TEAM_SPAWN_POINTS)
			break;
	}

	if ( !count ) {	// no spots that won't telefrag
		return G_Find( NULL, FOFS(classname), classname);
	}

	selection = rand() % count;
	return spots[ selection ];
}


/*
===========
SelectCTFSpawnPoint

============
*/
gentity_t *SelectCTFSpawnPoint ( team_t team, int teamstate, vec3_t origin, vec3_t angles, qboolean isbot ) {
	gentity_t	*spot;

	spot = SelectRandomTeamSpawnPoint ( teamstate, team );

	if (!spot) {
		return SelectSpawnPoint( vec3_origin, origin, angles, isbot );
	}

	VectorCopy (spot->s.origin, origin);
	origin[2] += 9;
	VectorCopy (spot->s.angles, angles);

	return spot;
}

/*---------------------------------------------------------------------------*/

static int QDECL SortClients( const void *a, const void *b ) {
	return *(int *)a - *(int *)b;
}


/*
==================
TeamplayLocationsMessage

Format:
	clientNum location health armor weapon powerups

==================
*/
void TeamplayInfoMessage( gentity_t *ent ) {
	char		entry[1024];
	char		string[8192];
	int			stringlength;
	int			i, j;
	gentity_t	*player;
	int			cnt;
	int			h, a;
	int			clients[TEAM_MAXOVERLAY];
	int			team;

	if ( ! ent->client->pers.teamInfo )
		return;

	// send team info to spectator for team of followed client
	if (ent->client->sess.sessionTeam == TEAM_SPECTATOR) {
		if ( ent->client->sess.spectatorState != SPECTATOR_FOLLOW
			|| ent->client->sess.spectatorClient < 0 ) {
			return;
		}
		team = g_entities[ ent->client->sess.spectatorClient ].client->sess.sessionTeam;
	} else {
		team = ent->client->sess.sessionTeam;
	}


// STONELANCE
//	if (team != TEAM_RED && team != TEAM_BLUE) {
	if (team != TEAM_RED && team != TEAM_BLUE && team != TEAM_GREEN && team != TEAM_YELLOW) {
// END
		return;
	}

	// figure out what client should be on the display
	// we are limited to 8, but we want to use the top eight players
	// but in client order (so they don't keep changing position on the overlay)
	for (i = 0, cnt = 0; i < g_maxclients.integer && cnt < TEAM_MAXOVERLAY; i++) {
		player = g_entities + level.sortedClients[i];
		if (player->inuse && player->client->sess.sessionTeam == team ) {
			clients[cnt++] = level.sortedClients[i];
		}
	}

	// We have the top eight players, sort them by clientNum
	qsort( clients, cnt, sizeof( clients[0] ), SortClients );

	// send the latest information on all clients
	string[0] = 0;
	stringlength = 0;

	for (i = 0, cnt = 0; i < g_maxclients.integer && cnt < TEAM_MAXOVERLAY; i++) {
		player = g_entities + i;
		if (player->inuse && player->client->sess.sessionTeam == team ) {

			h = player->client->ps.stats[STAT_HEALTH];
			a = player->client->ps.stats[STAT_ARMOR];
			if (h < 0) h = 0;
			if (a < 0) a = 0;

			Com_sprintf (entry, sizeof(entry),
				" %i %i %i %i %i %i", 
//				level.sortedClients[i], player->client->pers.teamState.location, h, a, 
				i, player->client->pers.teamState.location, h, a, 
				player->client->ps.weapon, player->s.powerups);
			j = strlen(entry);
			if (stringlength + j >= sizeof(string))
				break;
			strcpy (string + stringlength, entry);
			stringlength += j;
			cnt++;
		}
	}

	trap_SendServerCommand( ent-g_entities, va("tinfo %i %s", cnt, string) );
}

void CheckTeamStatus(void) {
	int i;
	gentity_t *loc, *ent;

	if (level.time - level.lastTeamLocationTime > TEAM_LOCATION_UPDATE_TIME) {

		level.lastTeamLocationTime = level.time;

		for (i = 0; i < g_maxclients.integer; i++) {
			ent = g_entities + i;

			if ( ent->client->pers.connected != CON_CONNECTED ) {
				continue;
			}

// STONELANCE
//			if (ent->inuse && (ent->client->sess.sessionTeam == TEAM_RED ||	ent->client->sess.sessionTeam == TEAM_BLUE)) {
			if (ent->inuse && (ent->client->sess.sessionTeam == TEAM_RED
				|| ent->client->sess.sessionTeam == TEAM_BLUE
				|| ent->client->sess.sessionTeam == TEAM_GREEN
				|| ent->client->sess.sessionTeam == TEAM_YELLOW)) {
// END
				loc = Team_GetLocation( ent );
				if (loc)
					ent->client->pers.teamState.location = loc->health;
				else
					ent->client->pers.teamState.location = 0;
			}
		}

		for (i = 0; i < g_maxclients.integer; i++) {
			ent = g_entities + i;

			if ( ent->client->pers.connected != CON_CONNECTED ) {
				continue;
			}

			if (ent->inuse) {
				TeamplayInfoMessage( ent );
			}
		}
	}
}

/*-----------------------------------------------------------------*/

/*QUAKED team_CTF_redplayer (1 0 0) (-16 -16 -16) (16 16 32)
Only in CTF games.  Red players spawn here at game start.
*/
void SP_team_CTF_redplayer( gentity_t *ent ) {
	(void)ent;
}


/*QUAKED team_CTF_blueplayer (0 0 1) (-16 -16 -16) (16 16 32)
Only in CTF games.  Blue players spawn here at game start.
*/
void SP_team_CTF_blueplayer( gentity_t *ent ) {
	(void)ent;
}

/*QUAKED team_CTF_redspawn (1 0 0) (-16 -16 -24) (16 16 32)
potential spawning position for red team in CTF games.
Targets will be fired when someone spawns in on them.
*/
void SP_team_CTF_redspawn(gentity_t *ent) {
	(void)ent;
}

/*QUAKED team_CTF_bluespawn (0 0 1) (-16 -16 -24) (16 16 32)
potential spawning position for blue team in CTF games.
Targets will be fired when someone spawns in on them.
*/
void SP_team_CTF_bluespawn(gentity_t *ent) {
	(void)ent;
}

/*QUAKED team_CTF_greenplayer (0 1 0) (-16 -16 -16) (16 16 32)
Only in CTF games.  Green players spawn here at game start.
*/
void SP_team_CTF_greenplayer( gentity_t *ent ) {
	(void)ent;
}

/*QUAKED team_CTF_yellowplayer (1 1 0) (-16 -16 -16) (16 16 32)
Only in CTF games.  Yellow players spawn here at game start.
*/
void SP_team_CTF_yellowplayer( gentity_t *ent ) {
	(void)ent;
}

/*QUAKED team_CTF_greenspawn (0 1 0) (-16 -16 -24) (16 16 32)
potential spawning position for green team in CTF games.
Targets will be fired when someone spawns in on them.
*/
void SP_team_CTF_greenspawn(gentity_t *ent) {
	(void)ent;
}

/*QUAKED team_CTF_yellowspawn (1 1 0) (-16 -16 -24) (16 16 32)
potential spawning position for yellow team in CTF games.
Targets will be fired when someone spawns in on them.
*/
void SP_team_CTF_yellowspawn(gentity_t *ent) {
	(void)ent;
}

/*QUAKED team_CTF_redflag (1 0 0) (-16 -16 -16) (16 16 16)
Red flag
*/
void SP_team_CTF_redflag( gentity_t *ent ) {
	gitem_t *it = BG_FindItemForPowerup( PW_REDFLAG );
	if (!it) {
		ent->think = G_FreeEntity;
		ent->nextthink = level.time + FRAMETIME;
		G_Printf( "Couldn't find item for team_CTF_redflag\n" );
		return;
	}
	G_SpawnItem( ent, it );
}

/*QUAKED team_CTF_blueflag (0 0 1) (-16 -16 -16) (16 16 16)
Blue flag
*/
void SP_team_CTF_blueflag( gentity_t *ent ) {
	gitem_t *it = BG_FindItemForPowerup( PW_BLUEFLAG );
	if (!it) {
		ent->think = G_FreeEntity;
		ent->nextthink = level.time + FRAMETIME;
		G_Printf( "Couldn't find item for team_CTF_blueflag\n" );
		return;
	}
	G_SpawnItem( ent, it );
}

/*QUAKED team_CTF_greenflag (0 1 0) (-16 -16 -16) (16 16 16)
Green flag
*/
void SP_team_CTF_greenflag( gentity_t *ent ) {
	gitem_t *it = BG_FindItemForPowerup( PW_GREENFLAG );
	if (!it) {
		ent->think = G_FreeEntity;
		ent->nextthink = level.time + FRAMETIME;
		G_Printf( "Couldn't find item for team_CTF_greenflag\n" );
		return;
	}
	G_SpawnItem( ent, it );
}

/*QUAKED team_CTF_yellowflag (1 1 0) (-16 -16 -16) (16 16 16)
Yellow flag
*/
void SP_team_CTF_yellowflag( gentity_t *ent ) {
	gitem_t *it = BG_FindItemForPowerup( PW_YELLOWFLAG );
	if (!it) {
		ent->think = G_FreeEntity;
		ent->nextthink = level.time + FRAMETIME;
		G_Printf( "Couldn't find item for team_CTF_yellowflag\n" );
		return;
	}
	G_SpawnItem( ent, it );
}




#ifdef MISSIONPACK
/*
================
Obelisks
================
*/

static void ObeliskRegen( gentity_t *self ) {
	self->nextthink = level.time + g_obeliskRegenPeriod.integer * 1000;
	if( self->health >= g_obeliskHealth.integer ) {
		return;
	}

	G_AddEvent( self, EV_POWERUP_REGEN, 0 );
	self->health += g_obeliskRegenAmount.integer;
	if ( self->health > g_obeliskHealth.integer ) {
		self->health = g_obeliskHealth.integer;
	}

	self->activator->s.modelindex2 = self->health * 0xff / g_obeliskHealth.integer;
	self->activator->s.frame = 0;
}


static void ObeliskRespawn( gentity_t *self ) {
	self->takedamage = qtrue;
	self->health = g_obeliskHealth.integer;

	self->think = ObeliskRegen;
	self->nextthink = level.time + g_obeliskRegenPeriod.integer * 1000;

	self->activator->s.frame = 0;
}


static void ObeliskDie( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod ) {
	int			otherTeam;

	otherTeam = OtherTeam( self->spawnflags );
	AddTeamScore(self->s.pos.trBase, otherTeam, 1);
	Team_ForceGesture(otherTeam);

	CalculateRanks();

	self->takedamage = qfalse;
	self->think = ObeliskRespawn;
	self->nextthink = level.time + g_obeliskRespawnDelay.integer * 1000;

	self->activator->s.modelindex2 = 0xff;
	self->activator->s.frame = 2;

	G_AddEvent( self->activator, EV_OBELISKEXPLODE, 0 );

	AddScore(attacker, self->r.currentOrigin, CTF_CAPTURE_BONUS);

	// add the sprite over the player's head
	attacker->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
	attacker->client->ps.eFlags |= EF_AWARD_CAP;
	attacker->client->rewardTime = level.time + REWARD_SPRITE_TIME;
    attacker->client->ps.persistant[PERS_CAPTURES]++;
    G_Profile_RecordFlagCapture( attacker->client );

	teamgame.redObeliskAttackedTime = 0;
	teamgame.blueObeliskAttackedTime = 0;
}


static void ObeliskTouch( gentity_t *self, gentity_t *other, trace_t *trace ) {
	int			tokens;

	if ( !other->client ) {
		return;
	}

	if ( OtherTeam(other->client->sess.sessionTeam) != self->spawnflags ) {
		return;
	}

	tokens = other->client->ps.generic1;
	if( tokens <= 0 ) {
		return;
	}

	PrintMsg(NULL, "%s" S_COLOR_WHITE " brought in %i %s.\n",
					other->client->pers.netname, tokens, ( tokens == 1 ) ? "skull" : "skulls" );

	AddTeamScore(self->s.pos.trBase, other->client->sess.sessionTeam, tokens);
	Team_ForceGesture(other->client->sess.sessionTeam);

	AddScore(other, self->r.currentOrigin, CTF_CAPTURE_BONUS*tokens);

	// add the sprite over the player's head
	other->client->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
	other->client->ps.eFlags |= EF_AWARD_CAP;
	other->client->rewardTime = level.time + REWARD_SPRITE_TIME;
    other->client->ps.persistant[PERS_CAPTURES] += tokens;
    G_Profile_RecordFlagCapture( other->client );
	
	other->client->ps.generic1 = 0;
	CalculateRanks();

	Team_CaptureFlagSound( self, self->spawnflags );
}

static void ObeliskPain( gentity_t *self, gentity_t *attacker, int damage ) {
	int actualDamage = damage / 10;
	if (actualDamage <= 0) {
		actualDamage = 1;
	}
	self->activator->s.modelindex2 = self->health * 0xff / g_obeliskHealth.integer;
	if (!self->activator->s.frame) {
		G_AddEvent(self, EV_OBELISKPAIN, 0);
	}
	self->activator->s.frame = 1;
	AddScore(attacker, self->r.currentOrigin, actualDamage);
}

// spawn invisible damagable obelisk entity / harvester base trigger.
gentity_t *SpawnObelisk( vec3_t origin, vec3_t mins, vec3_t maxs, int team ) {
	gentity_t	*ent;

	ent = G_Spawn();

	VectorCopy( origin, ent->s.origin );
	VectorCopy( origin, ent->s.pos.trBase );
	VectorCopy( origin, ent->r.currentOrigin );

	VectorCopy( mins, ent->r.mins );
	VectorCopy( maxs, ent->r.maxs );

	ent->s.eType = ET_GENERAL;
	ent->flags = FL_NO_KNOCKBACK;

	if( g_gametype.integer == GT_OBELISK ) {
		ent->r.contents = CONTENTS_SOLID;
		ent->takedamage = qtrue;
		ent->health = g_obeliskHealth.integer;
		ent->die = ObeliskDie;
		ent->pain = ObeliskPain;
		ent->think = ObeliskRegen;
		ent->nextthink = level.time + g_obeliskRegenPeriod.integer * 1000;
	}
	if( g_gametype.integer == GT_HARVESTER ) {
		ent->r.contents = CONTENTS_TRIGGER;
		ent->touch = ObeliskTouch;
	}

	G_SetOrigin( ent, ent->s.origin );

	ent->spawnflags = team;

	trap_LinkEntity( ent );

	return ent;
}

// setup entity for team base model / obelisk model.
void ObeliskInit( gentity_t *ent ) {
	trace_t tr;
	vec3_t dest;

	ent->s.eType = ET_TEAM;

	VectorSet( ent->r.mins, -15, -15, 0 );
	VectorSet( ent->r.maxs, 15, 15, 87 );

	if ( ent->spawnflags & 1 ) {
		// suspended
		G_SetOrigin( ent, ent->s.origin );
	} else {
		// mappers like to put them exactly on the floor, but being coplanar
		// will sometimes show up as starting in solid, so lif it up one pixel
		ent->s.origin[2] += 1;

		// drop to floor
		VectorSet( dest, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] - 4096 );
		trap_Trace( &tr, ent->s.origin, ent->r.mins, ent->r.maxs, dest, ent->s.number, MASK_SOLID );
		if ( tr.startsolid ) {
			ent->s.origin[2] -= 1;
			G_Printf( "SpawnObelisk: %s startsolid at %s\n", ent->classname, vtos(ent->s.origin) );

			ent->s.groundEntityNum = ENTITYNUM_NONE;
			G_SetOrigin( ent, ent->s.origin );
		}
		else {
			// allow to ride movers
			ent->s.groundEntityNum = tr.entityNum;
			G_SetOrigin( ent, tr.endpos );
		}
	}
}

/*QUAKED team_redobelisk (1 0 0) (-16 -16 0) (16 16 8)
*/
void SP_team_redobelisk( gentity_t *ent ) {
	gentity_t *obelisk;

	if ( g_gametype.integer <= GT_TEAM ) {
		G_FreeEntity(ent);
		return;
	}
	ObeliskInit( ent );
	if ( g_gametype.integer == GT_OBELISK ) {
		obelisk = SpawnObelisk( ent->s.origin, ent->r.mins, ent->r.maxs, TEAM_RED );
		obelisk->activator = ent;
		// initial obelisk health value
		ent->s.modelindex2 = 0xff;
		ent->s.frame = 0;
	}
	if ( g_gametype.integer == GT_HARVESTER ) {
		obelisk = SpawnObelisk( ent->s.origin, ent->r.mins, ent->r.maxs, TEAM_RED );
		obelisk->activator = ent;
	}
	ent->s.modelindex = TEAM_RED;
	trap_LinkEntity(ent);
}

/*QUAKED team_blueobelisk (0 0 1) (-16 -16 0) (16 16 88)
*/
void SP_team_blueobelisk( gentity_t *ent ) {
	gentity_t *obelisk;

	if ( g_gametype.integer <= GT_TEAM ) {
		G_FreeEntity(ent);
		return;
	}
	ObeliskInit( ent );
	if ( g_gametype.integer == GT_OBELISK ) {
		obelisk = SpawnObelisk( ent->s.origin, ent->r.mins, ent->r.maxs, TEAM_BLUE );
		obelisk->activator = ent;
		// initial obelisk health value
		ent->s.modelindex2 = 0xff;
		ent->s.frame = 0;
	}
	if ( g_gametype.integer == GT_HARVESTER ) {
		obelisk = SpawnObelisk( ent->s.origin, ent->r.mins, ent->r.maxs, TEAM_BLUE );
		obelisk->activator = ent;
	}
	ent->s.modelindex = TEAM_BLUE;
	trap_LinkEntity(ent);
}

/*QUAKED team_neutralobelisk (0 0 1) (-16 -16 0) (16 16 88)
*/
void SP_team_neutralobelisk( gentity_t *ent ) {
	if ( g_gametype.integer != GT_1FCTF && g_gametype.integer != GT_HARVESTER ) {
		G_FreeEntity(ent);
		return;
	}
	ObeliskInit( ent );
	if ( g_gametype.integer == GT_HARVESTER) {
		neutralObelisk = SpawnObelisk( ent->s.origin, ent->r.mins, ent->r.maxs, TEAM_FREE );
		neutralObelisk->activator = ent;
	}
	ent->s.modelindex = TEAM_FREE;
	trap_LinkEntity(ent);
}


/*
================
CheckObeliskAttack
================
*/
qboolean CheckObeliskAttack( gentity_t *obelisk, gentity_t *attacker ) {
	gentity_t	*te;

	// if this really is an obelisk
	if( obelisk->die != ObeliskDie ) {
		return qfalse;
	}

	// if the attacker is a client
	if( !attacker->client ) {
		return qfalse;
	}

	// if the obelisk is on the same team as the attacker then don't hurt it
	if( obelisk->spawnflags == attacker->client->sess.sessionTeam ) {
		return qtrue;
	}

	// obelisk may be hurt

	// if not played any sounds recently
	if ((obelisk->spawnflags == TEAM_RED &&
		teamgame.redObeliskAttackedTime < level.time - OVERLOAD_ATTACK_BASE_SOUND_TIME) ||
		(obelisk->spawnflags == TEAM_BLUE &&
		teamgame.blueObeliskAttackedTime < level.time - OVERLOAD_ATTACK_BASE_SOUND_TIME) ) {

		// tell which obelisk is under attack
		te = G_TempEntity( obelisk->s.pos.trBase, EV_GLOBAL_TEAM_SOUND );
		if( obelisk->spawnflags == TEAM_RED ) {
			te->s.eventParm = GTS_REDOBELISK_ATTACKED;
			teamgame.redObeliskAttackedTime = level.time;
		}
		else {
			te->s.eventParm = GTS_BLUEOBELISK_ATTACKED;
			teamgame.blueObeliskAttackedTime = level.time;
		}
		te->r.svFlags |= SVF_BROADCAST;
	}

	return qfalse;
}
#endif
