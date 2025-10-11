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
/*
=======================================================================

CONFIRMATION MENU

=======================================================================
*/


#include "ui_local.h"


#define ART_CONFIRM_FRAME	"menu/art/cut_frame_2"

#define ID_CONFIRM_NO		10
#define ID_CONFIRM_YES		11


typedef struct {
	menuframework_s menu;

	menutext_s		no;
	menutext_s		yes;

	int				slashX;
	const char *	question;
	void			(*draw)( void );
	void			(*action)( qboolean result );
	
        int style;
        const char **lines;
        int frameX;
        int frameY;
        int frameWidth;
        int frameHeight;
        int contentTop;
        int lineHeight;
} confirmMenu_t;


static confirmMenu_t	s_confirm;


/*
=================
ConfirmMenu_Event
=================
*/
static void ConfirmMenu_Event( void* ptr, int event ) {
	qboolean	result;

	if( event != QM_ACTIVATED ) {
		return;
	}

	UI_PopMenu();

	if( ((menucommon_s*)ptr)->id == ID_CONFIRM_NO ) {
		result = qfalse;
	}
	else {
		result = qtrue;
	}

	if( s_confirm.action ) {
		s_confirm.action( result );
	}
}


/*
=================
ConfirmMenu_Key
=================
*/
static sfxHandle_t ConfirmMenu_Key( int key ) {
	switch ( key ) {
	case K_KP_LEFTARROW:
	case K_LEFTARROW:
	case K_KP_RIGHTARROW:
	case K_RIGHTARROW:
		key = K_TAB;
		break;

	case 'n':
	case 'N':
		ConfirmMenu_Event( &s_confirm.no, QM_ACTIVATED );
		break;

	case 'y':
	case 'Y':
		ConfirmMenu_Event( &s_confirm.yes, QM_ACTIVATED );
		break;
	}

	return Menu_DefaultKey( &s_confirm.menu, key );
}


/*
=================
MessaheMenu_Draw
=================
*/
static void MessageMenu_Draw( void ) {
        int frameX;
        int frameY;
        int frameWidth;
        int frameHeight;
        int y;
        int lineHeight;
        int i;

        frameX = s_confirm.frameX ? s_confirm.frameX : 142;
        frameY = s_confirm.frameY ? s_confirm.frameY : 66;
        frameWidth = s_confirm.frameWidth ? s_confirm.frameWidth : 359;
        frameHeight = s_confirm.frameHeight ? s_confirm.frameHeight : 270;
        lineHeight = s_confirm.lineHeight ? s_confirm.lineHeight : 18;

        {
                const int extraTop = 10;
                const int extraBottom = 10;
                int topExtension = extraTop;
                int drawFrameY;
                int drawFrameHeight;

                if ( frameY < topExtension ) {
                        topExtension = frameY;
                }

                drawFrameY = frameY - topExtension;
                drawFrameHeight = frameHeight + topExtension + extraBottom;

                UI_DrawNamedPic( frameX, drawFrameY, frameWidth, drawFrameHeight, ART_CONFIRM_FRAME );
        }

        // keep the logical frame metrics unchanged so text/button placement stays put

        if ( s_confirm.contentTop ) {
                y = s_confirm.contentTop;
        } else {
                y = frameY + 30 - ( lineHeight / 2 ) + 14;
                if ( y < frameY + 16 ) {
                        y = frameY + 16;
                }
        }

        if ( s_confirm.lines ) {
                for ( i = 0; s_confirm.lines[i]; i++ ) {
                        UI_DrawProportionalString( SCREEN_WIDTH / 2, y, s_confirm.lines[i], s_confirm.style, text_color_normal );
                        y += lineHeight;
                }
        }

        Menu_Draw( &s_confirm.menu );

        if( s_confirm.draw ) {
                s_confirm.draw();
        }
}

/*
=================
ConfirmMenu_Draw
=================
*/
static void ConfirmMenu_Draw( void ) {
	UI_DrawNamedPic( 142, 118, 359, 256, ART_CONFIRM_FRAME );

	// BAGPUSS CHANGED TO TEXT_COLOR_NORMAL
//	UI_DrawProportionalString( 320, 204, s_confirm.question, s_confirm.style, color_red );
//	UI_DrawProportionalString( s_confirm.slashX, 265, "/", UI_LEFT|UI_INVERSE, color_red );
	UI_DrawProportionalString( 320, 204, s_confirm.question, s_confirm.style, text_color_normal );
	UI_DrawProportionalString( s_confirm.slashX, 265, "/", UI_LEFT|UI_INVERSE, text_color_normal );
	// END

	Menu_Draw( &s_confirm.menu );

	if( s_confirm.draw ) {
		s_confirm.draw();
	}
}


/*
=================
ConfirmMenu_Cache
=================
*/
void ConfirmMenu_Cache( void ) {
	trap_R_RegisterShaderNoMip( ART_CONFIRM_FRAME );
}


/*
=================
UI_ConfirmMenu_Stlye
=================
*/
void UI_ConfirmMenu_Style( const char *question, int style, void (*draw)( void ), void (*action)( qboolean result ) ) {
	uiClientState_t	cstate;
	int	n1, n2, n3;
	int	l1, l2, l3;

	// zero set all our globals
	memset( &s_confirm, 0, sizeof(s_confirm) );

	ConfirmMenu_Cache();

	n1 = UI_ProportionalStringWidth( "YES/NO" );
	n2 = UI_ProportionalStringWidth( "YES" ) + PROP_GAP_WIDTH;
	n3 = UI_ProportionalStringWidth( "/" )  + PROP_GAP_WIDTH;
	l1 = 320 - ( n1 / 2 );
	l2 = l1 + n2;
	l3 = l2 + n3;
	s_confirm.slashX = l2;

	s_confirm.question = question;
	s_confirm.draw = draw;
	s_confirm.action = action;
	s_confirm.style = style;

	s_confirm.menu.draw       = ConfirmMenu_Draw;
	s_confirm.menu.key        = ConfirmMenu_Key;
	s_confirm.menu.wrapAround = qtrue;
// STONELANCE
	s_confirm.menu.transparent = qtrue;
// END

	trap_GetClientState( &cstate );
	if ( cstate.connState >= CA_CONNECTED ) {
		s_confirm.menu.fullscreen = qfalse;
	}
	else {
		s_confirm.menu.fullscreen = qtrue;
	}

	s_confirm.yes.generic.type		= MTYPE_PTEXT;      
	s_confirm.yes.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS; 
	s_confirm.yes.generic.callback	= ConfirmMenu_Event;
	s_confirm.yes.generic.id		= ID_CONFIRM_YES;
	s_confirm.yes.generic.x			= l1;
	s_confirm.yes.generic.y			= 264;
	s_confirm.yes.string			= "YES";
// BAGPUSS
//	s_confirm.yes.color				= color_red;
	s_confirm.yes.color				= text_color_normal;
// END
	s_confirm.yes.style				= UI_LEFT;

	s_confirm.no.generic.type		= MTYPE_PTEXT;      
	s_confirm.no.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS; 
	s_confirm.no.generic.callback	= ConfirmMenu_Event;
	s_confirm.no.generic.id			= ID_CONFIRM_NO;
	s_confirm.no.generic.x		    = l3;
	s_confirm.no.generic.y		    = 264;
	s_confirm.no.string				= "NO";
// BAGPUSS
//	s_confirm.no.color			    = color_red;
	s_confirm.no.color			    = text_color_normal;
// END
	s_confirm.no.style			    = UI_LEFT;

	Menu_AddItem( &s_confirm.menu,	&s_confirm.yes );             
	Menu_AddItem( &s_confirm.menu,	&s_confirm.no );

	UI_PushMenu( &s_confirm.menu );

	Menu_SetCursorToItem( &s_confirm.menu, &s_confirm.no );
}

/*
=================
UI_ConfirmMenu
=================
*/
void UI_ConfirmMenu( const char *question, void (*draw)( void ), void (*action)( qboolean result ) ) {
	UI_ConfirmMenu_Style(question, UI_CENTER|UI_INVERSE, draw, action);
}

/*
=================
UI_Message
hacked over from Confirm stuff
=================
*/
void UI_Message( const char **lines ) {
	uiClientState_t	cstate;
	int	n1;
	int	l1;
	float	sizeScale;
	int	lineCount;
	int	maxWidth;
	int	lineHeight;
	int	frameWidth;
	int	frameHeight;
	int	frameX;
	int	frameY;
	int	contentTop;
	int	buttonY;

	// zero set all our globals
	memset( &s_confirm, 0, sizeof( s_confirm ) );

	ConfirmMenu_Cache();

	n1 = UI_ProportionalStringWidth( "OK" );
	l1 = 320 - ( n1 / 2 );

	s_confirm.lines = lines;
	s_confirm.style = UI_CENTER|UI_INVERSE|UI_SMALLFONT;

	s_confirm.menu.draw	= MessageMenu_Draw;
	s_confirm.menu.key	= ConfirmMenu_Key;
	s_confirm.menu.wrapAround = qtrue;
	s_confirm.menu.transparent = qtrue;

	trap_GetClientState( &cstate );
	if ( cstate.connState >= CA_CONNECTED ) {
		s_confirm.menu.fullscreen = qfalse;
	} else {
		s_confirm.menu.fullscreen = qtrue;
	}

	sizeScale = UI_ProportionalSizeScale( s_confirm.style );
	lineCount = 0;
	maxWidth = 0;

	if ( lines ) {
		while ( lines[lineCount] ) {
			int width = UI_ProportionalStringWidth( lines[lineCount] ) * sizeScale;
			if ( width > maxWidth ) {
				maxWidth = width;
			}
			lineCount++;
		}
	}

	lineHeight = (int)( 22 * sizeScale );
	if ( lineHeight < 18 ) {
		lineHeight = 18;
	}

        frameY = 66;
	{
		const int minFrameWidth = 420;
		const int frameSidePadding = 180;
		const int minFrameHeight = 300;
                const int frameTopPadding = 96;
                const int frameBottomPadding = 132;
                int contentHeight = lineCount * lineHeight;

		frameWidth = maxWidth + frameSidePadding;
		if ( frameWidth < minFrameWidth ) {
			frameWidth = minFrameWidth;
		}
		if ( frameWidth > SCREEN_WIDTH - 40 ) {
			frameWidth = SCREEN_WIDTH - 40;
		}

		frameHeight = frameTopPadding + contentHeight + frameBottomPadding;
		if ( frameHeight < minFrameHeight ) {
			frameHeight = minFrameHeight;
		}
		if ( frameHeight > SCREEN_HEIGHT - 80 ) {
			frameHeight = SCREEN_HEIGHT - 80;
		}

		frameX = (SCREEN_WIDTH - frameWidth) / 2;
                contentTop = frameY + frameTopPadding - ( lineHeight / 2 ) - 40 + 14;
                if ( contentTop < frameY + 16 ) {
                        contentTop = frameY + 16;
                }
                buttonY = frameY + frameHeight - 68 - 14;
        }

	if ( buttonY > SCREEN_HEIGHT - 48 ) {
		buttonY = SCREEN_HEIGHT - 48;
	}

	s_confirm.frameX = frameX;
	s_confirm.frameY = frameY;
	s_confirm.frameWidth = frameWidth;
	s_confirm.frameHeight = frameHeight;
	s_confirm.contentTop = contentTop;
	s_confirm.lineHeight = lineHeight;

	s_confirm.yes.generic.type		= MTYPE_PTEXT;
	s_confirm.yes.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_confirm.yes.generic.callback	= ConfirmMenu_Event;
	s_confirm.yes.generic.id		= ID_CONFIRM_YES;
	s_confirm.yes.generic.x		= l1;
	s_confirm.yes.generic.y		= buttonY;
	s_confirm.yes.string		= "OK";
	s_confirm.yes.color		= text_color_normal;
	s_confirm.yes.style		= UI_LEFT;

	Menu_AddItem( &s_confirm.menu,	&s_confirm.yes );

	UI_PushMenu( &s_confirm.menu );

	Menu_SetCursorToItem( &s_confirm.menu, &s_confirm.yes );
}
