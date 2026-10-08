/*
===========================================================================
Copyright (C) 2002-2026 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.
===========================================================================
*/

/*
 * Pure helpers for the online version check: version comparison and URL
 * validation. Header-only and free of engine state so the unit tests can
 * include it directly (tests/update_util_test.c).
 */

#ifndef CL_UPDATE_UTIL_H
#define CL_UPDATE_UTIL_H

#include <ctype.h>
#include <string.h>

#define CL_UPDATE_MAX_URL 256

/*
 * Skip a leading 'v'/'V' ("v0.7d" -> "0.7d").
 */
static ID_INLINE const char *CL_UpdateVersion_SkipPrefix( const char *s ) {
	if ( ( s[0] == 'v' || s[0] == 'V' ) && isalnum( (unsigned char)s[1] ) ) {
		return s + 1;
	}
	return s;
}

/*
 * End of the comparable part: build metadata after '+' ("v0.7d+20261004+abc")
 * and the ioq3 suffix after '_' ("v0.7d_IOQ3+r1234") are ignored.
 */
static ID_INLINE qboolean CL_UpdateVersion_IsEnd( char c ) {
	return ( c == '\0' || c == '+' || c == '_' || isspace( (unsigned char)c ) ) ? qtrue : qfalse;
}

/*
 * Compare one dotted segment list, e.g. "0.7d" or "rc1". Digit runs compare
 * numerically, letter runs case-insensitively; a missing numeric token counts
 * as 0 ("0.7" == "0.7.0"), a missing letter token sorts first ("0.7" < "0.7d").
 * Stops at stopChar or at the end of the version. Returns -1, 0 or 1.
 */
static ID_INLINE int CL_UpdateVersion_CompareRun( const char **pa, const char **pb, char stopChar ) {
	const char *a = *pa;
	const char *b = *pb;
	int result = 0;

	for ( ;; ) {
		qboolean endA, endB;

		while ( *a == '.' ) a++;
		while ( *b == '.' ) b++;

		endA = ( CL_UpdateVersion_IsEnd( *a ) || *a == stopChar ) ? qtrue : qfalse;
		endB = ( CL_UpdateVersion_IsEnd( *b ) || *b == stopChar ) ? qtrue : qfalse;

		if ( endA && endB ) {
			break;
		}

		if ( isdigit( (unsigned char)*a ) || isdigit( (unsigned char)*b ) ||
		     endA || endB ) {
			/* numeric token, or one side ran out */
			if ( endA && !isdigit( (unsigned char)*b ) ) { result = -1; break; }
			if ( endB && !isdigit( (unsigned char)*a ) ) { result = 1; break; }
			if ( !endA && !isdigit( (unsigned char)*a ) ) { result = -1; break; } /* letter < number */
			if ( !endB && !isdigit( (unsigned char)*b ) ) { result = 1; break; }
			{
				unsigned long na = 0, nb = 0;
				while ( isdigit( (unsigned char)*a ) ) { na = na * 10 + (unsigned long)( *a - '0' ); a++; }
				while ( isdigit( (unsigned char)*b ) ) { nb = nb * 10 + (unsigned long)( *b - '0' ); b++; }
				if ( na != nb ) { result = ( na < nb ) ? -1 : 1; break; }
			}
		} else {
			/* both sides start with a non-digit character */
			int ca = tolower( (unsigned char)*a );
			int cb = tolower( (unsigned char)*b );
			if ( ca != cb ) { result = ( ca < cb ) ? -1 : 1; break; }
			a++;
			b++;
		}
	}

	*pa = a;
	*pb = b;
	return result;
}

/*
 * Compare two Q3Rally version strings. Returns <0 if a is older than b,
 * 0 if equal and >0 if a is newer. A pre-release suffix after '-'
 * ("0.8-rc1") sorts before the release ("0.8").
 */
static ID_INLINE int CL_UpdateVersion_Compare( const char *a, const char *b ) {
	int result;
	qboolean preA, preB;

	if ( !a ) a = "";
	if ( !b ) b = "";

	while ( isspace( (unsigned char)*a ) ) a++;
	while ( isspace( (unsigned char)*b ) ) b++;

	a = CL_UpdateVersion_SkipPrefix( a );
	b = CL_UpdateVersion_SkipPrefix( b );

	result = CL_UpdateVersion_CompareRun( &a, &b, '-' );
	if ( result ) {
		return result;
	}

	/* skip to the pre-release part, if any */
	while ( !CL_UpdateVersion_IsEnd( *a ) && *a != '-' ) a++;
	while ( !CL_UpdateVersion_IsEnd( *b ) && *b != '-' ) b++;
	preA = ( *a == '-' ) ? qtrue : qfalse;
	preB = ( *b == '-' ) ? qtrue : qfalse;

	if ( preA != preB ) {
		return preA ? -1 : 1;
	}
	if ( !preA ) {
		return 0;
	}

	a++;
	b++;
	return CL_UpdateVersion_CompareRun( &a, &b, '\0' );
}

/*
 * A URL that is safe to hand to the system browser: http(s) only, no
 * whitespace, quotes or shell metacharacters (openURL builds a shell
 * command on Linux and macOS).
 */
static ID_INLINE qboolean CL_IsSafeURL( const char *url ) {
	const char *s;
	size_t len;

	if ( !url ) {
		return qfalse;
	}

	len = strlen( url );
	if ( len == 0 || len >= CL_UPDATE_MAX_URL ) {
		return qfalse;
	}

	if ( Q_strncmp( url, "https://", 8 ) && Q_strncmp( url, "http://", 7 ) ) {
		return qfalse;
	}

	for ( s = url; *s; s++ ) {
		unsigned char c = (unsigned char)*s;
		if ( isalnum( c ) ) {
			continue;
		}
		if ( !strchr( "-._~:/?#[]@!&()*+,;=%", c ) ) {
			return qfalse;
		}
	}
	return qtrue;
}

/*
 * Download URLs announced by the version file must point at the official
 * site, so a compromised or spoofed response cannot send players elsewhere.
 */
static ID_INLINE qboolean CL_IsOfficialDownloadURL( const char *url ) {
	if ( !CL_IsSafeURL( url ) ) {
		return qfalse;
	}
	return ( !Q_strncmp( url, "https://www.q3rally.com/", 24 ) ||
	         !Q_strncmp( url, "https://q3rally.com/", 20 ) ) ? qtrue : qfalse;
}

#endif /* CL_UPDATE_UTIL_H */
