#ifndef RALLY_PLATE_PATH_H
#define RALLY_PLATE_PATH_H

/* Generated client plate images are local cosmetic data, not packaged assets.
 * Keep the pure-server exception limited to canonical player-slot filenames. */
static qboolean Rally_IsGeneratedPlateImage( const char *path, qboolean requireExtension ) {
	const char *number;
	int slot = 0;
	const char *prefix = "models/players/plates/player";
	if ( !path || Q_stricmpn( path, prefix, strlen( prefix ) ) ) {
		return qfalse;
	}
	number = path + strlen( prefix );
	if ( *number < '0' || *number > '9' ||
		 (*number == '0' && number[1] >= '0' && number[1] <= '9') ) {
		return qfalse;
	}
	while ( *number >= '0' && *number <= '9' ) {
		slot = slot * 10 + *number++ - '0';
		if ( slot >= MAX_CLIENTS ) {
			return qfalse;
		}
	}
	if ( *number == '_' ) {
		int i;
		number++;
		for ( i = 0; i < 8; i++, number++ ) {
			if ( !((*number >= '0' && *number <= '9') ||
				  (*number >= 'a' && *number <= 'f') ||
				  (*number >= 'A' && *number <= 'F')) ) {
				return qfalse;
			}
		}
	}
	return !Q_stricmp( number, ".tga" ) || (!requireExtension && !*number);
}

#endif
