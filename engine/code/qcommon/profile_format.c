#include "profile_format.h"

/* Small bounded JSON walker shared by game and UI. Unlike the old field
 * searches, format detection only reads direct object members. */
static qboolean PF_EqualBytes( const char *a, const char *b, int length ) {
    int i;
    for ( i = 0; i < length; i++ ) {
        if ( a[i] != b[i] ) return qfalse;
        if ( !a[i] ) return qtrue;
    }
    return qtrue;
}

static const char *PF_Whitespace( const char *p ) {
    while ( *p == ' ' || *p == '\n' || *p == '\r' || *p == '\t' ) p++;
    return p;
}

static const char *PF_String( const char *p ) {
    int i;
    if ( *p++ != '"' ) return NULL;
    while ( *p && *p != '"' ) {
        if ( (unsigned char)*p < 32 ) return NULL;
        if ( *p++ == '\\' ) {
            if ( !*p ) return NULL;
            if ( *p == 'u' ) {
                p++;
                for ( i = 0; i < 4; i++, p++ ) {
                    if ( !*p || !strchr( "0123456789abcdefABCDEF", *p ) ) return NULL;
                }
            } else {
                if ( !strchr( "\"\\/bfnrt", *p ) ) return NULL;
                p++;
            }
        }
    }
    return *p == '"' ? p + 1 : NULL;
}

static const char *PF_Value( const char *p, int depth ) {
    char close;
    qboolean object;
    if ( depth > 16 ) return NULL;
    p = PF_Whitespace( p );
    if ( *p == '"' ) return PF_String( p );
    if ( *p == '{' || *p == '[' ) {
        object = *p == '{'; close = object ? '}' : ']';
        p = PF_Whitespace( p + 1 );
        if ( *p == close ) return p + 1;
        for ( ;; ) {
            if ( object ) {
                p = PF_String( p ); if ( !p ) return NULL;
                p = PF_Whitespace( p ); if ( *p++ != ':' ) return NULL;
            }
            p = PF_Value( p, depth + 1 ); if ( !p ) return NULL;
            p = PF_Whitespace( p );
            if ( *p == close ) return p + 1;
            if ( *p++ != ',' ) return NULL;
            p = PF_Whitespace( p );
        }
    }
    if ( PF_EqualBytes( p, "true", 4 ) || PF_EqualBytes( p, "null", 4 ) ) return p + 4;
    if ( PF_EqualBytes( p, "false", 5 ) ) return p + 5;
    if ( *p == '-' ) p++;
    if ( *p == '0' ) p++;
    else {
        if ( *p < '1' || *p > '9' ) return NULL;
        while ( *p >= '0' && *p <= '9' ) p++;
    }
    if ( *p == '.' ) {
        p++; if ( *p < '0' || *p > '9' ) return NULL;
        while ( *p >= '0' && *p <= '9' ) p++;
    }
    if ( *p == 'e' || *p == 'E' ) {
        p++; if ( *p == '+' || *p == '-' ) p++;
        if ( *p < '0' || *p > '9' ) return NULL;
        while ( *p >= '0' && *p <= '9' ) p++;
    }
    return p;
}

static const char *PF_Member( const char *object, const char *key ) {
    const char *p, *end, *found = NULL;
    int length = strlen( key );
    if ( !object || *object != '{' ) return NULL;
    p = PF_Whitespace( object + 1 );
    while ( *p && *p != '}' ) {
        end = PF_String( p ); if ( !end ) return NULL;
        if ( end - p == length + 2 && PF_EqualBytes( p + 1, key, length ) ) {
            const char *value = PF_Whitespace( end );
            if ( found || *value != ':' ) return "!"; /* Present but ambiguous, never legacy. */
            found = PF_Whitespace( value + 1 );
        }
        p = PF_Whitespace( end ); if ( *p++ != ':' ) return NULL;
        p = PF_Value( p, 0 ); if ( !p ) return NULL;
        p = PF_Whitespace( p ); if ( *p != ',' ) break;
        p = PF_Whitespace( p + 1 );
    }
    return found;
}

static qboolean PF_Equals( const char *value, const char *literal ) {
    const char *end;
    if ( !value ) return qfalse;
    end = PF_Value( value, 0 );
    return end && end - value == strlen( literal ) && PF_EqualBytes( value, literal, end - value );
}

int Profile_FormatClassify( const char *json ) {
    const char *root, *end, *version, *identity, *info, *progress, *link, *value;
    if ( !json || strlen( json ) >= PROFILE_JSON_MAX ) return -1;
    root = PF_Whitespace( json );
    end = PF_Value( root, 0 );
    if ( *root != '{' || !end || *PF_Whitespace( end ) ) return -1;
    version = PF_Member( root, "profileVersion" );
    if ( !version || PF_Equals( version, "1" ) ) {
        if ( PF_Member( root, "identity" ) || PF_Member( root, "webLink" ) ) return -1;
        value = PF_Member( root, "uuid" );
        if ( !value ) value = PF_Member( root, "name" );
        return value && *value == '"' ? 0 : -1;
    }
    if ( !PF_Equals( version, "2" ) ) return -1;
    identity = PF_Member( root, "identity" );
    info = PF_Member( identity, "info" );
    progress = PF_Member( root, "progress" );
    link = PF_Member( root, "webLink" );
    value = PF_Member( identity, "uuid" );
    if ( !value || *value != '"' || !info || *info != '{' ) return -1;
    value = PF_Member( identity, "name" );
    if ( !value || *value != '"' ) return -1;
    value = PF_Member( progress, "stats" );
    if ( !value || *value != '{' ) return -1;
    if ( !PF_Equals( PF_Member( progress, "epoch" ), "\"" PROFILE_PROGRESS_EPOCH "\"" )
        || !PF_Equals( PF_Member( progress, "authority" ), "\"local\"" )
        || !PF_Equals( PF_Member( link, "state" ), "\"unlinked\"" )
        || !PF_Equals( PF_Member( link, "accountId" ), "null" )
        || !PF_Equals( PF_Member( link, "profileId" ), "null" ) ) return -1;
    return PROFILE_FILE_VERSION;
}

static qboolean PF_Escape( char *out, int capacity, const char *text ) {
    const char *hex = "0123456789abcdef";
    int n = 0;
    unsigned char c;
    while ( (c = (unsigned char)*text++) != 0 ) {
        if ( c < 32 ) {
            if ( n + 6 >= capacity ) return qfalse;
            out[n++]='\\'; out[n++]='u'; out[n++]='0'; out[n++]='0';
            out[n++]=hex[c >> 4]; out[n++]=hex[c & 15];
        } else {
            if ( n + 2 >= capacity ) return qfalse;
            if ( c == '"' || c == '\\' ) out[n++]='\\';
            out[n++]=c;
        }
    }
    out[n]=0;
    return qtrue;
}

int Profile_FormatPrefix( char *out, int capacity, const char *uuid,
    const char *name, const profile_info_t *info, const char *favorites ) {
    char id[PROFILE_MAX_UUID*6], display[PROFILE_MAX_NAME*6];
    char gender[PROFILE_MAX_GENDER*6], birth[PROFILE_MAX_BIRTHDATE*6];
    char avatar[PROFILE_MAX_AVATAR*6], country[PROFILE_MAX_COUNTRY*6];
    const char *end;
    int n, used = 0;
    if ( !out || capacity <= 0 || !uuid || !name || !info || !favorites ) return -1;
    end = PF_Value( favorites, 0 );
    if ( *PF_Whitespace(favorites) != '[' || !end || *PF_Whitespace(end) ) return -1;
    if ( !PF_Escape(id,sizeof(id),uuid) || !PF_Escape(display,sizeof(display),name)
        || !PF_Escape(gender,sizeof(gender),info->gender) || !PF_Escape(birth,sizeof(birth),info->birthDate)
        || !PF_Escape(avatar,sizeof(avatar),info->avatar) || !PF_Escape(country,sizeof(country),info->country) ) return -1;
    n = Com_sprintf(out,capacity,"{\n\t\"profileVersion\": 2,\n\t\"identity\": {\n\t\"uuid\": \"%s\",\n\t\"name\": \"%s\",\n\t\"info\": {\n",id,display);
    if ( n < 0 || n >= capacity ) return -1;
    used += n;
    n = Com_sprintf(out+used,capacity-used,"\t\t\"gender\": \"%s\",\n\t\t\"birthDate\": \"%s\",\n\t\t\"avatar\": \"%s\",\n\t\t\"country\": \"%s\",\n\t\t\"favoriteCars\": %s\n\t}\n\t},\n",gender,birth,avatar,country,favorites);
    if ( n < 0 || used+n >= capacity ) return -1;
    used += n;
    n = Com_sprintf(out+used,capacity-used,"\t\"webLink\": {\"state\": \"unlinked\", \"accountId\": null, \"profileId\": null},\n\t\"progress\": {\n\t\"epoch\": \"" PROFILE_PROGRESS_EPOCH "\",\n\t\"authority\": \"local\",\n\t\"currentRank\": %d,\n\t\"highestRank\": %d,\n\t\"stats\": {\n",info->currentRank,info->highestRank);
    if ( n < 0 || used+n >= capacity ) return -1;
    return used+n;
}

qboolean Profile_FormatVehicles( const char *json, char *out, int capacity ) {
    const char *stats, *value, *end;
    if ( capacity < 3 || Profile_FormatClassify(json) != PROFILE_FILE_VERSION ) return qfalse;
    stats = PF_Member(PF_Member(PF_Whitespace(json),"progress"),"stats");
    value = PF_Member(stats,"vehicles");
    if ( !value ) { Q_strncpyz(out,"[]",capacity); return qtrue; }
    end = PF_Value(value,0);
    if ( *value != '[' || !end || end-value >= capacity ) return qfalse;
    memcpy(out,value,end-value); out[end-value]=0;
    return qtrue;
}
