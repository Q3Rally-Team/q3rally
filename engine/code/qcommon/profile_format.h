#ifndef Q3R_PROFILE_FORMAT_H
#define Q3R_PROFILE_FORMAT_H

#include "../game/profile_shared.h"

#define PROFILE_FILE_VERSION 2
#define PROFILE_PROGRESS_EPOCH "v0.8-local-1"
#define PROFILE_JSON_MAX 32768

/* 0: legacy identity may be imported, 2: current, -1: do not modify. */
int Profile_FormatClassify( const char *json );
int Profile_FormatPrefix( char *out, int capacity, const char *uuid,
    const char *name, const profile_info_t *info, const char *favorites );
qboolean Profile_FormatVehicles( const char *json, char *out, int capacity );

#endif
