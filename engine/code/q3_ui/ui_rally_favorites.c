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

#include "ui_local.h"

static int UI_Favorites_ParseIndex( const char *favorite ) {
    if ( !favorite || Q_strncmp( favorite, "favoritecar", 11 ) ) {
        return -1;
    }

    return atoi( favorite + 11 ) - 1;
}

/*
=================
GetValuesFromFavorite

=================
*/
qboolean GetValuesFromFavorite( const char *favorite, char *modelName, char *skinName, char *rimName, char *headName ) {
    char            buf[MAX_QPATH];
    char            buf2[MAX_QPATH];
    char            *slash;
    const profile_info_t *info;
    int             favoriteIndex;

    favoriteIndex = UI_Favorites_ParseIndex( favorite );
    info = UI_Profile_GetActiveInfo();

    if ( info && favoriteIndex >= 0 && favoriteIndex < PROFILE_MAX_FAVORITE_CARS ) {
        const profile_favorite_car_t *favoriteCar = &info->favoriteCars[favoriteIndex];

        if ( favoriteCar->model[0] && favoriteCar->skin[0] ) {
            if ( modelName ) {
                Q_strncpyz( modelName, favoriteCar->model, MAX_QPATH );
            }
            if ( skinName ) {
                Q_strncpyz( skinName, favoriteCar->skin, MAX_QPATH );
            }
            if ( rimName ) {
                Q_strncpyz( rimName, favoriteCar->rim, MAX_QPATH );
            }
            if ( headName ) {
                Q_strncpyz( headName, favoriteCar->head, MAX_QPATH );
            }

            return qfalse;
        }
    }

    trap_Cvar_VariableStringBuffer( favorite, buf, sizeof( buf ) );

    slash = strchr( buf, '/' );
    if ( slash ) {
        Q_strncpyz( buf2, slash + 1, sizeof( buf2 ) );
        *slash = 0;
        if ( modelName ) {
            strcpy( modelName, buf );
        }
    }
    else{
        modelName = NULL;
        return qtrue;
    }

    slash = strchr( buf2, '/' );
    if ( slash ) {
        Q_strncpyz( buf, slash + 1, sizeof( buf ) );
        *slash = 0;
        if ( skinName ) {
            strcpy( skinName, buf2 );
        }
    }
    else{
        skinName = NULL;
        return qtrue;
    }

    slash = strchr( buf, '/' );
    if ( slash ) {
        Q_strncpyz( buf2, slash + 1, sizeof( buf2 ) );
        *slash = 0;
        if ( rimName ) {
            strcpy( rimName, buf );
        }
    }
    else{
        rimName = NULL;
        return qtrue;
    }

    if (*buf2){
        if ( headName ) {
            strcpy( headName, buf2 );
        }
    }
    else{
        headName = NULL;
        return qtrue;
    }

    return qfalse;
}
