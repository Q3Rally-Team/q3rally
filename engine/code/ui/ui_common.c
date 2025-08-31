#include "ui_local.h"

int UI_FetchDemoList( char *buffer, int bufSize, const char **list, int maxList, const char *(*copyFunc)(const char *) ) {
    char    *demoname;
    int     i, j, len;
    int     protocol, protocolLegacy;
    char    extension[32];
    int     numDemos;

    protocolLegacy = trap_Cvar_VariableValue("com_legacyprotocol");
    protocol = trap_Cvar_VariableValue("com_protocol");

    if( !protocol ) {
        protocol = trap_Cvar_VariableValue("protocol");
    }
    if( protocolLegacy == protocol ) {
        protocolLegacy = 0;
    }

    Com_sprintf( extension, sizeof( extension ), ".%s%d", DEMOEXT, protocol );
    numDemos = trap_FS_GetFileList( "demos", extension, buffer, bufSize );

    demoname = buffer;
    i = 0;

    for( j = 0; j < 2; j++ ) {
        if( numDemos > maxList ) {
            numDemos = maxList;
        }

        for( ; i < numDemos; i++ ) {
            len = strlen( demoname );
            if( copyFunc ) {
                list[i] = copyFunc( demoname );
            } else {
                list[i] = demoname;
            }
            demoname += len + 1;
        }

        if( !j ) {
            if( protocolLegacy > 0 && numDemos < maxList ) {
                Com_sprintf( extension, sizeof( extension ), ".%s%d", DEMOEXT, protocolLegacy );
                numDemos += trap_FS_GetFileList( "demos", extension, demoname,
                                bufSize - ( demoname - buffer ) );
            } else {
                break;
            }
        }
    }

    return numDemos;
}

