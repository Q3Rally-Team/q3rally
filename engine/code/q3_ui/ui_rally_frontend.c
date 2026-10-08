/*
===========================================================================
Copyright (C) 2002-2026 Q3Rally Team
===========================================================================
*/

#include "ui_local.h"
#include "ui_rally_frontend.h"

static vec4_t frontendPanelColor   = UI_FRONTEND_COLOR_PANEL;
static vec4_t frontendCardColor    = UI_FRONTEND_COLOR_PANEL_ALT;
static vec4_t frontendFocusColor   = UI_FRONTEND_COLOR_FOCUS_BG;
static vec4_t frontendBorderColor  = UI_FRONTEND_COLOR_BORDER;
static vec4_t frontendAccentColor  = UI_FRONTEND_COLOR_ACCENT;
static vec4_t frontendTextColor    = UI_FRONTEND_COLOR_TEXT;
static vec4_t frontendMutedColor   = UI_FRONTEND_COLOR_MUTED;
static vec4_t frontendProgressColor = UI_FRONTEND_COLOR_PROGRESS;
static vec4_t frontendHeroOverlayColor = UI_FRONTEND_COLOR_HERO_OVERLAY;
static const char *frontendBackgroundNames[] = {
    "gfx/ui/q3rally_frontend_bg",
    "gfx/ui/q3rally_frontend_bg_alt",
    "gfx/ui/q3rally_frontend_bg_alt2",
    "gfx/ui/q3rally_frontend_bg_alt3"
};
static qhandle_t frontendBackgroundShaders[ARRAY_LEN( frontendBackgroundNames )];
static qboolean frontendBackgroundRegistered[ARRAY_LEN( frontendBackgroundNames )];
static qhandle_t frontendBackgroundShader;
static menuframework_s *frontendBackgroundMenu;
static int frontendBackgroundIndex = -1;
static qboolean frontendBackgroundKeep;

/* The backdrops are large; only the one actually shown gets loaded. */
static qhandle_t Frontend_BackgroundHandle( int index ) {
    if ( !frontendBackgroundRegistered[index] ) {
        frontendBackgroundRegistered[index] = qtrue;
        frontendBackgroundShaders[index] = trap_R_RegisterShaderNoMip(
            frontendBackgroundNames[index] );
    }
    return frontendBackgroundShaders[index];
}

/* The next frontend screen keeps the current backdrop instead of rolling a
 * new one (used for the hand-off from the loading screen to the main menu). */
void Frontend_KeepBackgroundForNextMenu( void ) {
    frontendBackgroundKeep = qtrue;
}

qhandle_t Frontend_BackgroundShader( void ) {
    int nextIndex;

    /* Pick once when a frontend menu becomes active. This keeps the image
     * stable while a screen is open, but gives each screen a fresh backdrop. */
    if ( frontendBackgroundIndex < 0 || uis.activemenu != frontendBackgroundMenu ) {
        frontendBackgroundMenu = uis.activemenu;

        if ( frontendBackgroundIndex >= 0 && frontendBackgroundKeep ) {
            frontendBackgroundKeep = qfalse;
        } else {
            nextIndex = UI_RandomInt( ARRAY_LEN( frontendBackgroundNames ) );
            if ( ARRAY_LEN( frontendBackgroundNames ) > 1 &&
                 nextIndex == frontendBackgroundIndex ) {
                nextIndex = ( nextIndex + 1 ) % ARRAY_LEN( frontendBackgroundNames );
            }
            frontendBackgroundIndex = nextIndex;
        }
        frontendBackgroundShader = Frontend_BackgroundHandle( frontendBackgroundIndex );
    }

    return frontendBackgroundShader ? frontendBackgroundShader : uis.menuBackShader;
}

/* Screen-filling backdrop. zoom > 1 crops towards the centre; 1.0 is the
 * framing every frontend screen uses. */
void Frontend_DrawBackgroundZoomed( const float *scrimColor, float zoom ) {
    float sw = uis.glconfig.vidWidth;
    float sh = uis.glconfig.vidHeight;
    float scale = ( sw / SCREEN_WIDTH > sh / SCREEN_HEIGHT ) ? sw / SCREEN_WIDTH : sh / SCREEN_HEIGHT;
    float w = SCREEN_WIDTH * scale;
    float h = SCREEN_HEIGHT * scale;
    float inset;

    if ( zoom < 1.0f ) {
        zoom = 1.0f;
    }
    inset = 0.5f * ( 1.0f - 1.0f / zoom );

    UI_SetColor( NULL );
    trap_R_DrawStretchPic( ( sw - w ) * 0.5f, ( sh - h ) * 0.5f, w, h,
                           inset, inset, 1.0f - inset, 1.0f - inset,
                           Frontend_BackgroundShader() );
    if ( scrimColor ) {
        UI_FillRect( -uis.bias, 0, SCREEN_WIDTH + uis.bias * 2,
                     SCREEN_HEIGHT, scrimColor );
    }
}

void Frontend_DrawBackground( const float *scrimColor ) {
    Frontend_DrawBackgroundZoomed( scrimColor, 1.0f );
}

/* The legacy UI small cell is 6x16, which visibly stretches this square-cell
 * atlas vertically. Frontend text uses square cells at every size instead. */
static int Frontend_TextHeight( int style ) {
    if ( style & UI_SMALLFONT ) {
        return UI_FRONTEND_SMALL_GLYPH_SIZE;
    }
    if ( style & UI_GIANTFONT ) {
        return GIANTCHAR_HEIGHT;
    }
    return BIGCHAR_HEIGHT;
}

static int Frontend_TextCellWidth( int style ) {
    return Frontend_TextHeight( style );
}

static int Frontend_TextAdvance( int ch, int style ) {
    int cellWidth;
    int tracking;

    cellWidth = Frontend_TextCellWidth( style );
    if ( ch == ' ' ) {
        return ( cellWidth + 1 ) / 2;
    }

    if ( style & UI_SMALLFONT ) {
        tracking = 2;
    } else if ( style & UI_GIANTFONT ) {
        tracking = 5;
    } else {
        tracking = 3;
    }

    return cellWidth - tracking;
}

static int Frontend_TextWidthRaw( const char *text, int style ) {
    const char *s;
    int cursorX;
    int visualWidth;

    if ( !text ) {
        return 0;
    }

    cursorX = 0;
    visualWidth = 0;
    for ( s = text; *s; s++ ) {
        int ch;
        int glyphRight;

        if ( Q_IsColorString( s ) ) {
            s++;
            continue;
        }

        ch = *s & 255;
        if ( ch != ' ' ) {
            glyphRight = cursorX + Frontend_TextCellWidth( style );
            if ( glyphRight > visualWidth ) {
                visualWidth = glyphRight;
            }
        }
        cursorX += Frontend_TextAdvance( ch, style );
    }

    if ( cursorX > visualWidth ) {
        visualWidth = cursorX;
    }
    return visualWidth;
}

static int Frontend_TextVisualWidthRaw( const char *text, int style ) {
    const char *s;
    int cursorX;
    int visualWidth;

    if ( !text ) {
        return 0;
    }

    cursorX = 0;
    visualWidth = 0;
    for ( s = text; *s; s++ ) {
        int ch;
        int glyphRight;

        if ( Q_IsColorString( s ) ) {
            s++;
            continue;
        }

        ch = *s & 255;
        if ( ch != ' ' ) {
            glyphRight = cursorX + Frontend_TextCellWidth( style );
            if ( glyphRight > visualWidth ) {
                visualWidth = glyphRight;
            }
        }
        cursorX += Frontend_TextAdvance( ch, style );
    }

    if ( cursorX > visualWidth ) {
        visualWidth = cursorX;
    }
    return visualWidth;
}

static void Frontend_DrawTextRaw( float x, int y, const char *text,
                                  int style, float scale,
                                  const float *color ) {
    const char *s;
    float charHeight;
    float cursorX;
    vec4_t drawColor;

    if ( !text || !text[0] ) {
        return;
    }

    charHeight = Frontend_TextHeight( style ) * scale;
    cursorX = x;
    Vector4Copy( color, drawColor );

    trap_R_SetColor( drawColor );
    for ( s = text; *s; s++ ) {
        int ch;
        float advance;
        float quadWidth;
        float ax;
        float ay;
        float aw;
        float ah;
        float frow;
        float fcol;

        if ( Q_IsColorString( s ) ) {
            s++;
            continue;
        }

        ch = *s & 255;
        advance = Frontend_TextAdvance( ch, style ) * scale;
        quadWidth = Frontend_TextCellWidth( style ) * scale;
        if ( ch == ' ' ) {
            cursorX += advance;
            continue;
        }

        ax = cursorX * uis.xscale + uis.bias;
        ay = y * uis.yscale;
        aw = quadWidth * uis.xscale;
        ah = charHeight * uis.yscale;
        frow = ( ch >> 4 ) * 0.0625f;
        fcol = ( ch & 15 ) * 0.0625f;
        trap_R_DrawStretchPic( ax, ay, aw, ah,
                               fcol, frow, fcol + 0.0625f,
                               frow + 0.0625f, uis.charset );
        cursorX += advance;
    }
    trap_R_SetColor( NULL );
}

int Frontend_TextWidth( const char *text, int style ) {
    return Frontend_TextWidthRaw( text, style );
}

int Frontend_TextVisualWidth( const char *text, int style ) {
    return Frontend_TextVisualWidthRaw( text, style );
}

void Frontend_DrawTextScaled( int x, int y, const char *text, int style,
                              float scale, const float *color ) {
    float width;
    int format;
    float drawX;
    vec4_t drawColor;
    vec4_t dropColor;

    if ( !text || !color || scale <= 0.0f ) {
        return;
    }

    Vector4Copy( color, drawColor );
    if ( style & UI_PULSE ) {
        vec4_t lowlight;

        lowlight[0] = drawColor[0] * 0.8f;
        lowlight[1] = drawColor[1] * 0.8f;
        lowlight[2] = drawColor[2] * 0.8f;
        lowlight[3] = drawColor[3] * 0.8f;
        UI_LerpColor( drawColor, lowlight, drawColor,
                      0.5f + 0.5f * sin( uis.realtime / PULSE_DIVISOR ) );
    }

    width = Frontend_TextWidthRaw( text, style ) * scale;
    format = style & UI_FORMATMASK;
    drawX = (float)x;
    if ( format == UI_CENTER ) {
        drawX -= width * 0.5f;
    } else if ( format == UI_RIGHT ) {
        drawX -= width;
    }

    if ( style & UI_DROPSHADOW ) {
        dropColor[0] = 0.0f;
        dropColor[1] = 0.0f;
        dropColor[2] = 0.0f;
        dropColor[3] = drawColor[3];
        Frontend_DrawTextRaw( drawX + 2.0f, y + 2, text, style,
                              scale, dropColor );
    }
    Frontend_DrawTextRaw( drawX, y, text, style, scale, drawColor );
}

void Frontend_DrawText( int x, int y, const char *text, int style,
                        const float *color ) {
    Frontend_DrawTextScaled( x, y, text, style, 1.0f, color );
}

/*
 * Copy text into out, shortened with "..." so it fits maxWidth.
 */
void Frontend_FitText( char *out, int outSize, const char *text, int maxWidth,
                       int style ) {
    int len;

    Q_strncpyz( out, text ? text : "", outSize );
    if ( Frontend_TextWidthRaw( out, style ) <= maxWidth ) {
        return;
    }

    len = strlen( out );
    while ( len > 0 ) {
        out[--len] = '\0';
        if ( len + 4 > outSize ) {
            continue;
        }
        Q_strcat( out, outSize, "..." );
        if ( Frontend_TextWidthRaw( out, style ) <= maxWidth ) {
            return;
        }
        out[len] = '\0';
    }
}

/*
 * Word-wrapped text. Draws at most maxLines lines; the last line is
 * shortened with "..." if text remains. Returns the number of lines drawn.
 */
int Frontend_DrawTextWrapped( int x, int y, int maxWidth, int lineHeight,
                              int maxLines, const char *text, int style,
                              const float *color ) {
    char line[256];
    char candidate[256];
    char word[128];
    const char *s;
    int lines = 0;
    int n;

    if ( !text || maxLines <= 0 ) {
        return 0;
    }

    line[0] = '\0';
    s = text;
    while ( *s ) {
        while ( *s == ' ' ) {
            s++;
        }
        if ( !*s ) {
            break;
        }
        for ( n = 0; s[n] && s[n] != ' ' && n < (int)sizeof( word ) - 1; n++ ) {
            word[n] = s[n];
        }
        word[n] = '\0';

        if ( line[0] ) {
            Com_sprintf( candidate, sizeof( candidate ), "%s %s", line, word );
        } else {
            Q_strncpyz( candidate, word, sizeof( candidate ) );
        }

        if ( line[0] && Frontend_TextWidthRaw( candidate, style ) > maxWidth ) {
            if ( lines == maxLines - 1 ) {
                /* last allowed line: show what fits, then "..." */
                Frontend_FitText( candidate, sizeof( candidate ), va( "%s %s", line, s ),
                                  maxWidth, style );
                Frontend_DrawText( x, y + lines * lineHeight, candidate, style, color );
                return lines + 1;
            }
            Frontend_DrawText( x, y + lines * lineHeight, line, style, color );
            lines++;
            Q_strncpyz( line, word, sizeof( line ) );
        } else {
            Q_strncpyz( line, candidate, sizeof( line ) );
        }
        s += n;
    }

    if ( line[0] ) {
        Frontend_FitText( candidate, sizeof( candidate ), line, maxWidth, style );
        Frontend_DrawText( x, y + lines * lineHeight, candidate, style, color );
        lines++;
    }
    return lines;
}

static void Frontend_ColorWithAlpha( vec4_t out, const float *baseColor,
                                     float alpha ) {
    out[0] = baseColor[0];
    out[1] = baseColor[1];
    out[2] = baseColor[2];
    out[3] = baseColor[3] * alpha;
}

void Frontend_DrawPanel( int x, int y, int width, int height,
                         float alpha, int style ) {
    vec4_t fillColor;
    vec4_t borderColor;
    vec4_t accentColor;

    Frontend_ColorWithAlpha( borderColor, frontendBorderColor, alpha );
    Frontend_ColorWithAlpha( accentColor, frontendAccentColor, alpha );

    if ( style == UI_FRONTEND_STYLE_FRAME ) {
        /* Frames are intentionally open: one signal line gives the surface
         * an edge without turning it into another boxed window. */
        UI_FillRect( x, y, width, UI_FRONTEND_PANEL_TOPBAR, accentColor );
        UI_FillRect( x, y + height - 1, width, 1, borderColor );
        return;
    }

    if ( style == UI_FRONTEND_STYLE_ACTIVE ) {
        Frontend_ColorWithAlpha( fillColor, frontendFocusColor, alpha );
    } else if ( style == UI_FRONTEND_STYLE_CARD ) {
        Frontend_ColorWithAlpha( fillColor, frontendCardColor, alpha );
    } else {
        Frontend_ColorWithAlpha( fillColor, frontendPanelColor, alpha );
    }

    UI_FillRect( x, y, width, height, fillColor );

    if ( style == UI_FRONTEND_STYLE_ACTIVE ) {
        UI_FillRect( x, y, 2, height, accentColor );
    } else if ( style == UI_FRONTEND_STYLE_CARD ) {
        /* Cards get one quiet hairline instead of a complete border. */
        borderColor[3] *= 0.65f;
        UI_FillRect( x, y, width, 1, borderColor );
    }
}

void Frontend_DrawCard( int x, int y, int width, int height,
                        float alpha, qboolean active ) {
    Frontend_DrawPanel( x, y, width, height, alpha,
                        active ? UI_FRONTEND_STYLE_ACTIVE : UI_FRONTEND_STYLE_CARD );
}

static qboolean Frontend_DrawButtonInternal( int x, int y, int width, int height,
                                             const char *label, float alpha,
                                             qboolean active, qboolean allowHover,
                                             int textAlign ) {
    vec4_t buttonColor;
    vec4_t borderColor;
    vec4_t accentColor;
    vec4_t textColor;
    qboolean hovered;
    qboolean highlighted;

    hovered = ( uis.cursorx >= x && uis.cursorx <= x + width &&
                uis.cursory >= y && uis.cursory <= y + height ) ? qtrue : qfalse;
    highlighted = ( active || ( allowHover && hovered ) ) ? qtrue : qfalse;

    Frontend_ColorWithAlpha( buttonColor, frontendFocusColor, alpha );
    Frontend_ColorWithAlpha( borderColor, frontendBorderColor, alpha * 0.70f );
    Frontend_ColorWithAlpha( accentColor, frontendAccentColor, alpha );

    /* Flat controls are transparent at rest. Interaction is communicated by
     * a soft wash and an underline, rather than a raised rectangle. */
    if ( highlighted ) {
        buttonColor[3] *= 0.58f;
        UI_FillRect( x, y, width, height, buttonColor );
    }
    UI_FillRect( x, y + height - 1, width, 1, borderColor );
    if ( highlighted ) {
        UI_FillRect( x, y + height - 2, width, 2, accentColor );
    }

    if ( highlighted ) {
        Frontend_ColorWithAlpha( textColor, frontendAccentColor, alpha );
    } else {
        Frontend_ColorWithAlpha( textColor, frontendTextColor, alpha );
    }

    Frontend_DrawText( x + ( textAlign == UI_CENTER ? width / 2 : UI_FRONTEND_SPACE_MD ),
                       y + ( height - Frontend_TextHeight( UI_SMALLFONT ) ) / 2,
                       label, textAlign | UI_SMALLFONT, textColor );

    return hovered;
}

qboolean Frontend_DrawButton( int x, int y, int width, int height,
                              const char *label, float alpha,
                              qboolean active, int textAlign ) {
    return Frontend_DrawButtonInternal( x, y, width, height, label, alpha,
                                        active, qtrue, textAlign );
}

qboolean Frontend_DrawButtonFocused( int x, int y, int width, int height,
                                     const char *label, float alpha,
                                     qboolean active, int textAlign ) {
    return Frontend_DrawButtonInternal( x, y, width, height, label, alpha,
                                        active, qfalse, textAlign );
}

qboolean Frontend_DrawNavButton( int x, int y, int width, int height,
                                 const char *label, float alpha,
                                 qboolean active, int textAlign ) {
    vec4_t textColor;
    qboolean hovered;
    qboolean highlighted;

    hovered = ( uis.cursorx >= x && uis.cursorx <= x + width &&
                uis.cursory >= y && uis.cursory <= y + height ) ? qtrue : qfalse;
    highlighted = ( active || hovered ) ? qtrue : qfalse;

    /* Navigation stays visually quiet until it is selected. */
    if ( highlighted ) {
        Frontend_ColorWithAlpha( textColor, frontendFocusColor, alpha * 0.55f );
        UI_FillRect( x, y, width, height, textColor );
        Frontend_ColorWithAlpha( textColor, frontendAccentColor, alpha );
        UI_FillRect( x, y + height - 2, width, 2, textColor );
    }

    if ( highlighted ) {
        Frontend_ColorWithAlpha( textColor, frontendAccentColor, alpha );
    } else {
        Frontend_ColorWithAlpha( textColor, frontendMutedColor, alpha );
    }

    Frontend_DrawText( x + ( textAlign == UI_CENTER ? width / 2 : UI_FRONTEND_SPACE_MD ),
                       y + ( height - Frontend_TextHeight( UI_SMALLFONT ) ) / 2,
                       label, textAlign | UI_SMALLFONT, textColor );
    return hovered;
}

void Frontend_DrawStatusChip( int x, int y, const char *label,
                              const float *statusColor, float alpha ) {
    vec4_t dotColor;
    vec4_t textColor;

    Frontend_ColorWithAlpha( dotColor, statusColor, alpha );
    Frontend_ColorWithAlpha( textColor, frontendAccentColor, alpha );
    UI_FillRect( x, y + 3, UI_FRONTEND_STATUS_DOT, UI_FRONTEND_STATUS_DOT, dotColor );
    Frontend_DrawText( x + UI_FRONTEND_STATUS_DOT + UI_FRONTEND_SPACE_SM, y,
                       label, UI_LEFT | UI_SMALLFONT, textColor );
}

void Frontend_DrawSidebar( int x, int y, int width, int height,
                           const char *title, float alpha ) {
    vec4_t titleColor;

    Frontend_DrawPanel( x, y, width, height, alpha, UI_FRONTEND_STYLE_SURFACE );
    Frontend_ColorWithAlpha( titleColor, frontendMutedColor, alpha );
    Frontend_DrawText( x + UI_FRONTEND_SPACE_LG, y + 44, title,
                       UI_LEFT | UI_SMALLFONT, titleColor );
}

void Frontend_DrawVehicleHero( int x, int y, int width, int height,
                               playerInfo_t *playerInfo, int realtime,
                               float alpha ) {
    vec4_t overlayColor;

    Frontend_ColorWithAlpha( overlayColor, frontendHeroOverlayColor, alpha );
    UI_FillRect( x, y, width, height, overlayColor );
    Frontend_DrawPanel( x, y, width, height, alpha, UI_FRONTEND_STYLE_FRAME );
    UI_DrawPlayer( x, y, width, height, playerInfo, realtime );
}

void Frontend_DrawProgress( int x, int y, int width, int height,
                            float progress, float alpha ) {
    vec4_t trackColor;
    vec4_t fillColor;
    vec4_t borderColor;
    int fillWidth;

    if ( progress < 0.0f ) {
        progress = 0.0f;
    } else if ( progress > 1.0f ) {
        progress = 1.0f;
    }

    Frontend_ColorWithAlpha( trackColor, frontendProgressColor, alpha );
    Frontend_ColorWithAlpha( fillColor, frontendAccentColor, alpha );
    Frontend_ColorWithAlpha( borderColor, frontendBorderColor, alpha );

    UI_FillRect( x, y, width, height, trackColor );
    fillWidth = (int)( width * progress );
    if ( fillWidth > 0 ) {
        UI_FillRect( x, y, fillWidth, height, fillColor );
        UI_FillRect( x, y, fillWidth, UI_FRONTEND_PANEL_TOPBAR, borderColor );
    }
    UI_DrawRect( x, y, width, height, borderColor );
}

/* Aspect of the gfx/ui/q3rally_frontend_bg* images (about 4:3). */
#define FRONTEND_BACKGROUND_ASPECT 1.3333f

/*
 * The large right-hand panel of frontend screens: the backdrop tinted with
 * the hero overlay colour (cropped to the panel instead of stretched), the
 * overlay on top and an open frame.
 */
void Frontend_DrawHeroSurface( int x, int y, int width, int height, float alpha ) {
    vec4_t overlayColor;
    float  px = x, py = y, pw = width, ph = height;
    float  s0 = 0.0f, t0 = 0.0f, s1 = 1.0f, t1 = 1.0f;
    float  panelAspect;

    if ( width <= 0 || height <= 0 ) {
        return;
    }

    panelAspect = (float)width / (float)height;
    if ( panelAspect < FRONTEND_BACKGROUND_ASPECT ) {
        float visible = panelAspect / FRONTEND_BACKGROUND_ASPECT;
        s0 = 0.5f - visible * 0.5f;
        s1 = 0.5f + visible * 0.5f;
    } else {
        float visible = FRONTEND_BACKGROUND_ASPECT / panelAspect;
        t0 = 0.5f - visible * 0.5f;
        t1 = 0.5f + visible * 0.5f;
    }

    Frontend_ColorWithAlpha( overlayColor, frontendHeroOverlayColor, alpha );

    UI_SetColor( overlayColor );
    UI_AdjustFrom640( &px, &py, &pw, &ph );
    trap_R_DrawStretchPic( px, py, pw, ph, s0, t0, s1, t1, Frontend_BackgroundShader() );
    UI_SetColor( NULL );

    UI_FillRect( x, y, width, height, overlayColor );
    Frontend_DrawPanel( x, y, width, height, alpha, UI_FRONTEND_STYLE_FRAME );
}
