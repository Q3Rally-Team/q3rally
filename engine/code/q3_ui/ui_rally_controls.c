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

typedef struct {
	char	*command;
	char	*label;
	int		id;
	int		anim;
	int		defaultbind1;
	int		defaultbind2;
	int		bind1;
	int		bind2;
} bind_t;

typedef struct
{
	char*	name;
	float	defaultvalue;
	float	value;	
} configcvar_t;

#define SAVE_NOOP		0
#define SAVE_YES		1
#define SAVE_NO			2
#define SAVE_CANCEL		3

// control sections
#define C_MOVEMENT		0
#define C_LOOKING		1
#define C_COMBAT		2
#define C_WEAPONS		3
#define C_MISC			4
#define C_DEVELOPER	5
#define C_MAX			6

#define ID_MOVEMENT		100
#define ID_LOOKING		101
#define ID_WEAPONS		102
#define ID_MISC			103
#define ID_DEVELOPER	104
#define ID_DEFAULTS		105
#define ID_BACK			106
#define ID_SAVEANDEXIT	107
#define ID_EXIT			108
#define ID_EXITCONFIRM_SAVE	109
#define ID_EXITCONFIRM_DISCARD	110
#define ID_EXITCONFIRM_CANCEL	111
#define ID_COMBAT		112

// bindable actions
#define ID_SHOWSCORES	0
#define ID_USEITEM		1
#define ID_ACCEL		2
#define ID_BRAKE		3
#define ID_HANDBRAKE	4
#define ID_TURBO		5
#define ID_MOVEUP		6	
#define ID_MOVEDOWN		7
#define ID_SHOWHUD2		8
#define ID_LEFT			9
#define ID_RIGHT		10	
#define ID_REARATTACK	11
#define ID_LOOKUP		12	
#define ID_LOOKDOWN		13
#define ID_MOUSELOOK	14
#define ID_CENTERVIEW	15
#define ID_ZOOMVIEW		16
#define ID_WEAPON1		17	
#define ID_WEAPON2		18	
#define ID_WEAPON3		19	
#define ID_WEAPON4		20	
#define ID_WEAPON5		21	
#define ID_WEAPON6		22	
#define ID_WEAPON7		23	
#define ID_WEAPON8		24	
#define ID_WEAPON9		25
#define ID_WEAPON10     26
#define ID_ATTACK		27
#define ID_ALT_ATTACK   28
#define ID_WEAPPREV		29
#define ID_WEAPNEXT		30
#define ID_HORN			31
#define ID_CHAT			32
#define ID_CHAT2		33
#define ID_CHAT3		34
#define ID_CHAT4		35
#define ID_DROP_REAR	36
#define ID_HEADLIGHT	37
#define ID_STARTDEMO	38
#define ID_STOPDEMO		39
#define ID_FREELOOK		40
#define ID_INVERTMOUSE	41
#define ID_ALWAYSRUN	42
#define ID_AUTOSWITCH	43
#define ID_MOUSESPEED	44
#define ID_JOYENABLE	45
#define ID_JOYTHRESHOLD	46
#define ID_SMOOTHMOUSE	47
#define ID_AUTODROP		48
#define ID_NEXTCAMERA   49
#define ID_DROPITEM             50
#define ID_JUKEBOX_PLAY         51
#define ID_JUKEBOX_NEXT         52
#define ID_JUKEBOX_PREV         53
#define ID_JUKEBOX_RESCAN       54
#define ID_JUKEBOX_SHUFFLE      55
#define ID_JUKEBOX_REPEAT       56
#define ID_MOVE_BPOINT_X_POS	57
#define ID_MOVE_BPOINT_X_NEG	58
#define ID_MOVE_BPOINT_Y_POS	59
#define ID_MOVE_BPOINT_Y_NEG	60
#define ID_MOVE_BPOINT_Z_POS	61
#define ID_MOVE_BPOINT_Z_NEG	62
#define ID_MOVE_BHANDLE_X_POS	63
#define ID_MOVE_BHANDLE_X_NEG	64
#define ID_MOVE_BHANDLE_Y_POS	65
#define ID_MOVE_BHANDLE_Y_NEG	66
#define ID_MOVE_BHANDLE_Z_POS	67
#define ID_MOVE_BHANDLE_Z_NEG	68
#define ID_PREV_BPOINT		69
#define ID_NEXT_BPOINT		70
#define ID_TOGGLE_BOT_PATHS	71
#define ID_SAVE_BPOINTS	72
#define ID_GEARUP		73
#define ID_GEARDOWN		74
#define ID_JOYANALOG	75
#define ID_INPUTMODE	76

#define CONTROLS_INPUT_KEYBOARD		0
#define CONTROLS_INPUT_CONTROLLER	1
#define CONTROLS_PROFILE_BINDINGS	128

#define ANIM_IDLE		0
#define ANIM_WALK		1
#define ANIM_BACK		2
#define ANIM_JUMP		3
#define ANIM_CROUCH		4
#define ANIM_STEPLEFT	5
#define ANIM_STEPRIGHT	6
#define ANIM_TURNLEFT	7
#define ANIM_TURNRIGHT	8
#define ANIM_LOOKUP		9
#define ANIM_LOOKDOWN	10
#define ANIM_WEAPON1	11
#define ANIM_WEAPON2	12
#define ANIM_WEAPON3	13
#define ANIM_WEAPON4	14
#define ANIM_WEAPON5	15
#define ANIM_WEAPON6	16
#define ANIM_WEAPON7	17
#define ANIM_WEAPON8	18
#define ANIM_WEAPON9	19
#define ANIM_WEAPON10	20
#define ANIM_ATTACK		21
#define ANIM_ALT_ATTACK 22
#define ANIM_REARATTACK	23
#define ANIM_DIE		24
#define ANIM_CHAT		25
#define ANIM_HEADLIGHT	26
#define ANIM_HORN		27
#define ANIM_DROPREAR	28
#define ANIM_TURBO		29
#define ANIM_STARTDEMO  30
#define ANIM_STOPDEMO   31
// END

typedef struct
{
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		player;
	menutext_s			movement;
	menutext_s			looking;
	menutext_s			combat;
	menutext_s			weapons;
	menutext_s			misc;
	menutext_s			developer;
	menutext_s			searchLabel;
	menufield_s		search;
    
	menuaction_s		accel;
	menuaction_s		brake;
	menuaction_s		handbrake;
	menuaction_s		turbo;
	menuaction_s		gearup;
	menuaction_s		geardown;

	menuaction_s		moveup;
	menuaction_s		movedown;
	menuaction_s		turnleft;
	menuaction_s		turnright;

	menuaction_s		machinegun;
	menuaction_s		chainsaw;
	menuaction_s		shotgun;
	menuaction_s		grenadelauncher;
	menuaction_s		rocketlauncher;
	menuaction_s		lightning;
	menuaction_s		railgun;
	menuaction_s		plasma;
	menuaction_s		bfg;
	menuaction_s        flamethrower;
	menuaction_s		attack;

	menuaction_s        alt_attack;
	menuaction_s		rearattack;
	menuaction_s		droprear;
	menuradiobutton_s	autodroprear;

	menuaction_s		prevweapon;
	menuaction_s		nextweapon;
	menuaction_s		lookup;
	menuaction_s		lookdown;
	menuaction_s		mouselook;
	menuradiobutton_s	freelook;
	menuaction_s		centerview;
	menuaction_s		zoomview;
	menuradiobutton_s	invertmouse;
	menuslider_s		sensitivity;
	menuradiobutton_s	smoothmouse;
	menuradiobutton_s	alwaysrun;
	menuaction_s		showscores;
	menuradiobutton_s	autoswitch;
	menuaction_s		useitem;
        menuaction_s            dropitem;
	playerInfo_t		playerinfo;
	qboolean			changesmade;
	menuaction_s		chat;
	menuaction_s		chat2;
	menuaction_s		chat3;
	menuaction_s		chat4;

	menuaction_s		headlight;
	menuaction_s		horn;
	menuaction_s		nextcamera;
	menuaction_s		jukeboxPlay;
	menuaction_s		jukeboxNext;
	menuaction_s		jukeboxPrev;
	menuaction_s		jukeboxRescan;
	menuaction_s		jukeboxShuffle;
	menuaction_s		jukeboxRepeat;
    menuaction_s        startdemo;
    menuaction_s        stopdemo;
	menuaction_s		moveBPointXPos;
	menuaction_s		moveBPointXNeg;
	menuaction_s		moveBPointYPos;
	menuaction_s		moveBPointYNeg;
	menuaction_s		moveBPointZPos;
	menuaction_s		moveBPointZNeg;
	menuaction_s		moveBHandleXPos;
	menuaction_s		moveBHandleXNeg;
	menuaction_s		moveBHandleYPos;
	menuaction_s		moveBHandleYNeg;
	menuaction_s		moveBHandleZPos;
	menuaction_s		moveBHandleZNeg;
	menuaction_s		prevBPoint;
	menuaction_s		nextBPoint;
	menuaction_s		toggleBotPaths;
	menuaction_s		saveBPoints;

	menulist_s			inputmode;
	menuradiobutton_s	joyenable;
	menuradiobutton_s	joyanalog;
	menuslider_s		joythreshold;
	int					section;
	qboolean			waitingforkey;
	char				playerModel[64];
	vec3_t				playerViewangles;
	vec3_t				playerMoveangles;
	int					playerLegs;
	int					playerTorso;
	weapon_t			playerWeapon;
	qboolean			playerChat;

	menutext_s			back;
} controls_t; 	

static controls_t s_controls;

typedef struct {
	menuframework_s	menu;
	menutext_s		save;
	menutext_s		discard;
	menutext_s		cancel;
} controlsExitConfirm_t;

static controlsExitConfirm_t s_controlsExitConfirm;
static int s_rebindConfirmTargetId = -1;
static int s_rebindConfirmKey = -1;
static char s_rebindConfirmQuestion[128];
static char s_controlsSearchText[64];
static menucommon_s* s_globalSearchControls[128];
static int s_globalSearchControlCount;
static int s_controlsProfileKeys[2][CONTROLS_PROFILE_BINDINGS];
static float s_controlsProfileJoyEnable[2];
static float s_controlsProfileJoyAnalog[2];
static float s_controlsProfileJoyThreshold[2];
static qboolean s_controlsProfileLoaded[2];
static int s_controlsActiveProfileMode;

static const char *s_controlsInputModes[] = {
	"Mouse/Keyboard",
	"Controller",
	0
};

// static vec4_t controls_binding_color  = {1.00, 0.43, 0.00, 1.00};

// Keep default controls conflict-free and aligned with common racing key layouts.
static bind_t g_bindings[] = 
{
	{"+scores",			  "show scores",		  ID_SHOWSCORES,	  ANIM_IDLE,		  K_TAB,			-1,		-1, -1},
	{"+button2",		  "use item",			  ID_USEITEM,		  ANIM_IDLE,		  K_ENTER,		    -1,		-1, -1},
        {"drop",                          "drop item",                   ID_DROPITEM,           ANIM_IDLE,              'g',                   -1,             -1, -1},
	{"+forward", 		  "accelerate",		      ID_ACCEL,		      ANIM_WALK,		  'w',		        K_UPARROW,		-1, -1},
	{"+back", 			  "brake",			      ID_BRAKE,		      ANIM_BACK,		  's',	            K_DOWNARROW,		-1, -1},
	{"+button14", 		"handbrake",		ID_HANDBRAKE,	ANIM_BACK,		K_SPACE,		K_CTRL,		-1, -1},
	{"+speed", 			"turbo",			ID_TURBO,		ANIM_TURBO,		K_SHIFT,		-1,		-1,	-1},
	{"gearUp",			"gear up",			ID_GEARUP,		ANIM_IDLE,		K_PGUP,			-1,		-1, -1},
	{"gearDown",		"gear down",		ID_GEARDOWN,	ANIM_IDLE,		K_PGDN,			-1,		-1, -1},
	{"+moveup",			"clutch / up",		ID_MOVEUP,		ANIM_JUMP,		'x',			-1,		-1, -1},
	{"+movedown",		"down",				ID_MOVEDOWN,	ANIM_CROUCH,	'c',			-1,		-1, -1},
	{"+hud", 			"show HUD",			ID_SHOWHUD2,	0,				'q',			-1,		-1, -1},
	{"+left", 			"turn left",		ID_LEFT,		ANIM_TURNLEFT,	'a',	K_LEFTARROW,		-1, -1},
	{"+right", 			"turn right",		ID_RIGHT,		ANIM_TURNRIGHT,	'd',	K_RIGHTARROW,		-1, -1},
	{"+button12", 		"rear attack",		ID_REARATTACK,	ANIM_REARATTACK, K_KP_INS,		-1,		-1, -1},
	{"+lookup", 		"look up",			ID_LOOKUP,		ANIM_LOOKUP,	-1,			-1,		-1, -1},
	{"+lookdown", 		"look down",		ID_LOOKDOWN,	ANIM_LOOKDOWN,	-1,			-1,		-1, -1},
	{"+mlook", 			"mouse look",		ID_MOUSELOOK,	ANIM_IDLE,		'/',			-1,		-1, -1},
	{"centerview", 		"center view",		ID_CENTERVIEW,	ANIM_IDLE,		K_END,			-1,		-1, -1},
	{"+zoom", 			"zoom view",		ID_ZOOMVIEW,	ANIM_IDLE,		K_MOUSE3,			-1,		-1, -1},
	{"weapon 1",		"chainsaw",			ID_WEAPON1,		ANIM_WEAPON1,	'1',			-1,		-1, -1},
	{"weapon 2",		"machinegun",		ID_WEAPON2,		ANIM_WEAPON2,	'2',			-1,		-1, -1},
	{"weapon 3",		"shotgun",			ID_WEAPON3,		ANIM_WEAPON3,	'3',			-1,		-1, -1},
	{"weapon 4",		"grenade launcher",	ID_WEAPON4,		ANIM_WEAPON4,	'4',			-1,		-1, -1},
	{"weapon 5",		"rocket launcher",	ID_WEAPON5,		ANIM_WEAPON5,	'5',			-1,		-1, -1},
	{"weapon 6",		"lightning",		ID_WEAPON6,		ANIM_WEAPON6,	'6',			-1,		-1, -1},
	{"weapon 7",		"railgun",			ID_WEAPON7,		ANIM_WEAPON7,	'7',			-1,		-1, -1},
	{"weapon 8",		"plasma gun",		ID_WEAPON8,		ANIM_WEAPON8,	'8',			-1,		-1, -1},
	{"weapon 9",		"BFG",				ID_WEAPON9,		ANIM_WEAPON9,	'9',			-1,		-1, -1},
	{"weapon 10",       "flame thrower",    ID_WEAPON10,    ANIM_WEAPON10,  '0',            -1,     -1, -1},
	{"+attack", 		"attack",			ID_ATTACK,		ANIM_ATTACK,	K_CTRL,			-1,		-1, -1},
	{"+button5",        "secondary attack", ID_ALT_ATTACK,  ANIM_ALT_ATTACK, K_MOUSE2,   -1,  -1,   -1},
	{"weapprev",		"prev weapon",		ID_WEAPPREV,	ANIM_IDLE,		'[',			-1,		-1, -1},
	{"weapnext", 		"next weapon",		ID_WEAPNEXT,	ANIM_IDLE,		']',			-1,		-1, -1},
	{"+button3", 		"horn",				ID_HORN,		ANIM_HORN,		'h',			-1,		-1, -1},
	{"messagemode", 	"chat",				ID_CHAT,		ANIM_CHAT,		't',			-1,		-1, -1},
	{"messagemode2", 	"chat - team",		ID_CHAT2,		ANIM_CHAT,		'y',				-1,		-1, -1},
	{"messagemode3", 	"chat - target",	ID_CHAT3,		ANIM_CHAT,		'i',				-1,		-1, -1},
	{"messagemode4", 	"chat - attacker",	ID_CHAT4,		ANIM_CHAT,		'o',				-1,		-1, -1},
	{"dropWeapon", 		"drop rear weapon",	ID_DROP_REAR,	ANIM_DROPREAR,	'r',			-1,		-1, -1},
    {"headlights", 		"lights",	ID_HEADLIGHT,	ANIM_HEADLIGHT,	'l',		    -1,		-1, -1},
    {"record",          "start demo record",            ID_STARTDEMO,   ANIM_STARTDEMO, 'z',            -1,     -1, -1},
    {"stoprecord",      "stop demo record",             ID_STOPDEMO,    ANIM_STOPDEMO,  'u',            -1,     -1, -1},
	{"nextcamera",		"next camera",		  ID_NEXTCAMERA,	  ANIM_IDLE,		  'v',			-1,		-1, -1},
	{"jukebox_play",		"jukebox play/stop",	ID_JUKEBOX_PLAY,		ANIM_IDLE,			K_F5,			-1, -1},
	{"jukebox_next",		"jukebox next track",	ID_JUKEBOX_NEXT,	ANIM_IDLE,			K_F6,			-1, -1},
	{"jukebox_prev",		"jukebox previous track", ID_JUKEBOX_PREV,	ANIM_IDLE,			K_F7,			-1, -1},
	{"jukebox_rescan",		"jukebox rescan tracks", ID_JUKEBOX_RESCAN,	ANIM_IDLE,			K_F8,			-1, -1},
	{"jukebox_shuffle_toggle",	"jukebox shuffle toggle", ID_JUKEBOX_SHUFFLE,	ANIM_IDLE,			K_F9,			-1, -1},
	{"jukebox_repeat_cycle",	"jukebox repeat mode",	ID_JUKEBOX_REPEAT,	ANIM_IDLE,			K_F10,			-1, -1},

	{"toggle cg_drawBotPaths",	"toggle bot paths",	ID_TOGGLE_BOT_PATHS,	ANIM_IDLE,	-1,			-1,		-1, -1},
	{"moveBPoint 10 0 0",	"move bezier point +x",	ID_MOVE_BPOINT_X_POS,	ANIM_IDLE,	-1,			-1,		-1, -1},
	{"moveBPoint -10 0 0",	"move bezier point -x",	ID_MOVE_BPOINT_X_NEG,	ANIM_IDLE,	-1,			-1,		-1, -1},
	{"moveBPoint 0 10 0",	"move bezier point +y",	ID_MOVE_BPOINT_Y_POS,	ANIM_IDLE,	-1,			-1,		-1, -1},
	{"moveBPoint 0 -10 0",	"move bezier point -y",	ID_MOVE_BPOINT_Y_NEG,	ANIM_IDLE,	-1,			-1,		-1, -1},
	{"moveBPoint 0 0 10",	"move bezier point +z",	ID_MOVE_BPOINT_Z_POS,	ANIM_IDLE,	-1,			-1,		-1, -1},
	{"moveBPoint 0 0 -10",	"move bezier point -z",	ID_MOVE_BPOINT_Z_NEG,	ANIM_IDLE,	-1,			-1,		-1, -1},
	{"moveBHandle 10 0 0",	"move bezier handle +x",	ID_MOVE_BHANDLE_X_POS,	ANIM_IDLE,	-1,			-1,		-1, -1},
	{"moveBHandle -10 0 0",	"move bezier handle -x",	ID_MOVE_BHANDLE_X_NEG,	ANIM_IDLE,	-1,			-1,		-1, -1},
	{"moveBHandle 0 10 0",	"move bezier handle +y",	ID_MOVE_BHANDLE_Y_POS,	ANIM_IDLE,	-1,			-1,		-1, -1},
	{"moveBHandle 0 -10 0",	"move bezier handle -y",	ID_MOVE_BHANDLE_Y_NEG,	ANIM_IDLE,	-1,			-1,		-1, -1},
	{"moveBHandle 0 0 10",	"move bezier handle +z",	ID_MOVE_BHANDLE_Z_POS,	ANIM_IDLE,	-1,			-1,		-1, -1},
	{"moveBHandle 0 0 -10",	"move bezier handle -z",	ID_MOVE_BHANDLE_Z_NEG,	ANIM_IDLE,	-1,			-1,		-1, -1},
	{"prevBPoint",		"previous bezier point",	ID_PREV_BPOINT,		ANIM_IDLE,	-1,			-1,		-1, -1},
	{"nextBPoint",		"next bezier point",	ID_NEXT_BPOINT,		ANIM_IDLE,	-1,			-1,		-1, -1},
	{"saveBPoints",		"save bezier points",	ID_SAVE_BPOINTS,		ANIM_IDLE,	-1,			-1,		-1, -1},

	{(char*)NULL,		(char*)NULL,		0,				0,				-1,				-1,		-1,	-1},
};

static configcvar_t g_configcvars[] =
{
// STONELANCE
	{"cg_autodrop",		0,					0},
// END
	{"cl_run",			0,					0},
	{"m_pitch",			0,					0},
	{"cg_autoswitch",	0,					0},
	{"sensitivity",		0,					0},
	{"cg_controlMode",	0,					0},
	{"in_joystick",		0,					0},
	{"in_joystickUseAnalog",	0,				0},
	{"joy_threshold",	0,					0},
	{"m_filter",		0,					0},
	{"cl_freelook",		0,					0},
	{NULL,				0,					0}
};

static menucommon_s *g_movement_controls[] = {
    (menucommon_s *)&s_controls.accel,
	(menucommon_s *)&s_controls.brake,
	(menucommon_s *)&s_controls.gearup,
	(menucommon_s *)&s_controls.geardown,
	(menucommon_s *)&s_controls.moveup,
	(menucommon_s *)&s_controls.handbrake,      
	(menucommon_s *)&s_controls.turbo,     
	(menucommon_s *)&s_controls.movedown,      
	(menucommon_s *)&s_controls.turnleft,      
	(menucommon_s *)&s_controls.turnright,    
	(menucommon_s *)&s_controls.headlight,
	(menucommon_s *)&s_controls.horn,
	NULL,
};

static menucommon_s *g_combat_controls[] = {
	(menucommon_s *)&s_controls.attack,
    (menucommon_s *)&s_controls.alt_attack,
	(menucommon_s *)&s_controls.rearattack,
	(menucommon_s *)&s_controls.droprear,
	(menucommon_s *)&s_controls.autodroprear,
	(menucommon_s *)&s_controls.useitem,
	(menucommon_s *)&s_controls.dropitem,
	NULL,
};

static menucommon_s *g_weapons_controls[] = {
	(menucommon_s *)&s_controls.nextweapon,
	(menucommon_s *)&s_controls.prevweapon,
	(menucommon_s *)&s_controls.autoswitch,    
	(menucommon_s *)&s_controls.chainsaw,         
	(menucommon_s *)&s_controls.machinegun,
	(menucommon_s *)&s_controls.shotgun,          
	(menucommon_s *)&s_controls.grenadelauncher,
	(menucommon_s *)&s_controls.rocketlauncher,   
	(menucommon_s *)&s_controls.lightning,   
	(menucommon_s *)&s_controls.railgun,          
	(menucommon_s *)&s_controls.plasma,           
	(menucommon_s *)&s_controls.bfg,
    (menucommon_s *)&s_controls.flamethrower,              
	NULL,
};

static menucommon_s *g_looking_controls[] = {
	(menucommon_s *)&s_controls.sensitivity,
	(menucommon_s *)&s_controls.smoothmouse,
	(menucommon_s *)&s_controls.invertmouse,
	(menucommon_s *)&s_controls.lookup,
	(menucommon_s *)&s_controls.lookdown,
	(menucommon_s *)&s_controls.mouselook,
	(menucommon_s *)&s_controls.freelook,
	(menucommon_s *)&s_controls.centerview,
	(menucommon_s *)&s_controls.zoomview,
	(menucommon_s *)&s_controls.nextcamera,
	(menucommon_s *)&s_controls.joyenable,
	(menucommon_s *)&s_controls.joyanalog,
	(menucommon_s *)&s_controls.joythreshold,
	NULL,
};

static menucommon_s *g_misc_controls[] = {
	(menucommon_s *)&s_controls.showscores,
	(menucommon_s *)&s_controls.jukeboxPlay,
	(menucommon_s *)&s_controls.jukeboxNext,
	(menucommon_s *)&s_controls.jukeboxPrev,
	(menucommon_s *)&s_controls.jukeboxRescan,
	(menucommon_s *)&s_controls.jukeboxShuffle,
	(menucommon_s *)&s_controls.jukeboxRepeat,
	(menucommon_s *)&s_controls.chat,
	(menucommon_s *)&s_controls.chat2,
	(menucommon_s *)&s_controls.chat3,
	(menucommon_s *)&s_controls.chat4,
	(menucommon_s *)&s_controls.startdemo,
	(menucommon_s *)&s_controls.stopdemo,
	NULL,
};

static menucommon_s *g_developer_controls[] = {
	(menucommon_s *)&s_controls.toggleBotPaths,
	(menucommon_s *)&s_controls.moveBPointXPos,
	(menucommon_s *)&s_controls.moveBPointXNeg,
	(menucommon_s *)&s_controls.moveBPointYPos,
	(menucommon_s *)&s_controls.moveBPointYNeg,
	(menucommon_s *)&s_controls.moveBPointZPos,
	(menucommon_s *)&s_controls.moveBPointZNeg,
	(menucommon_s *)&s_controls.moveBHandleXPos,
	(menucommon_s *)&s_controls.moveBHandleXNeg,
	(menucommon_s *)&s_controls.moveBHandleYPos,
	(menucommon_s *)&s_controls.moveBHandleYNeg,
	(menucommon_s *)&s_controls.moveBHandleZPos,
	(menucommon_s *)&s_controls.moveBHandleZNeg,
	(menucommon_s *)&s_controls.prevBPoint,
	(menucommon_s *)&s_controls.nextBPoint,
	(menucommon_s *)&s_controls.saveBPoints,
	NULL,
};

static menucommon_s **g_controls[] = {
	g_movement_controls,
	g_looking_controls,
	g_combat_controls,
	g_weapons_controls,
	g_misc_controls,
	g_developer_controls,
};

static qboolean Controls_SearchActive( void )
{
	return s_controlsSearchText[0] != '\0';
}

static qboolean Controls_SearchFieldHasFocus( void )
{
	if ( s_controls.menu.nitems <= 0 || s_controls.menu.cursor < 0 || s_controls.menu.cursor >= s_controls.menu.nitems ) {
		return qfalse;
	}

	return s_controls.menu.items[s_controls.menu.cursor] == (void *)&s_controls.search;
}

static qboolean Controls_ShowDeveloper( void )
{
	return trap_Cvar_VariableValue( "ui_controls_showDeveloper" ) != 0;
}

static void Controls_SearchFieldSyncFromState( void )
{
	Q_strncpyz( s_controls.search.field.buffer, s_controlsSearchText, sizeof( s_controls.search.field.buffer ) );
	s_controls.search.field.cursor = strlen( s_controls.search.field.buffer );
}

static bind_t* Controls_FindBindingById( int id )
{
	int i;

	for ( i = 0; g_bindings[i].command; i++ ) {
		if ( g_bindings[i].id == id ) {
			return &g_bindings[i];
		}
	}

	return NULL;
}

static char Controls_ToLowerAscii( char c )
{
	if ( c >= 'A' && c <= 'Z' ) {
		return c + ('a' - 'A');
	}

	return c;
}

static qboolean Controls_StringContainsCaseInsensitive( const char* haystack, const char* needle )
{
	int i;
	int needleLen;

	if ( !needle || !needle[0] ) {
		return qtrue;
	}

	if ( !haystack || !haystack[0] ) {
		return qfalse;
	}

	needleLen = strlen( needle );

	for ( i = 0; haystack[i]; i++ ) {
		int j;
		for ( j = 0; j < needleLen; j++ ) {
			char hc = haystack[i + j];
			char nc = needle[j];
			if ( !hc ) {
				break;
			}
			if ( Controls_ToLowerAscii( hc ) != Controls_ToLowerAscii( nc ) ) {
				break;
			}
		}

		if ( j == needleLen ) {
			return qtrue;
		}
	}

	return qfalse;
}

static const char* Controls_SectionTagForAction( int id )
{
	int i;
	menucommon_s** controls;
	menucommon_s* control;

	controls = g_movement_controls;
	for ( i = 0; (control = controls[i]); i++ ) {
		if ( control->id == id ) {
			return "DRIVE";
		}
	}

	controls = g_looking_controls;
	for ( i = 0; (control = controls[i]); i++ ) {
		if ( control->id == id ) {
			return "VIEW";
		}
	}

	controls = g_combat_controls;
	for ( i = 0; (control = controls[i]); i++ ) {
		if ( control->id == id ) {
			return "COMBAT";
		}
	}

	controls = g_weapons_controls;
	for ( i = 0; (control = controls[i]); i++ ) {
		if ( control->id == id ) {
			return "WEAPONS";
		}
	}

	controls = g_misc_controls;
	for ( i = 0; (control = controls[i]); i++ ) {
		if ( control->id == id ) {
			return "SYSTEM";
		}
	}

	controls = g_developer_controls;
	for ( i = 0; (control = controls[i]); i++ ) {
		if ( control->id == id ) {
			return "DEVELOPER";
		}
	}

	return "UNKNOWN";
}

static qboolean Controls_ControlVisibleForInputMode( menucommon_s *control )
{
	if ( s_controls.inputmode.curvalue != CONTROLS_INPUT_CONTROLLER ) {
		switch ( control->id ) {
			case ID_JOYENABLE:
			case ID_JOYANALOG:
			case ID_JOYTHRESHOLD:
				return qfalse;
			default:
				break;
		}
	}

	return qtrue;
}

static void Controls_BuildGlobalSearchList( void )
{
	int i;
	int j;
	menucommon_s** controls;
	menucommon_s* control;

	s_globalSearchControlCount = 0;

	for ( i = 0; i < C_MAX; i++ ) {
		if ( i == C_DEVELOPER && !Controls_ShowDeveloper() ) {
			continue;
		}

		controls = g_controls[i];
		for ( j = 0; (control = controls[j]); j++ ) {
			bind_t* binding = Controls_FindBindingById( control->id );

			qboolean match = qfalse;

			if ( !Controls_ControlVisibleForInputMode( control ) ) {
				continue;
			}

			if ( binding && Controls_StringContainsCaseInsensitive( binding->label, s_controlsSearchText ) ) {
				match = qtrue;
			} else if ( control->name && Controls_StringContainsCaseInsensitive( control->name, s_controlsSearchText ) ) {
				match = qtrue;
			}

			if ( match && s_globalSearchControlCount < (int)ARRAY_LEN( s_globalSearchControls ) ) {
				s_globalSearchControls[s_globalSearchControlCount++] = control;

            }
		}
	}
}

/*
=================
Controls_InitCvars
=================
*/
static void Controls_InitCvars( void )
{
	int				i;
	configcvar_t*	cvarptr;

	cvarptr = g_configcvars;
	for (i=0; ;i++,cvarptr++)
	{
		if (!cvarptr->name)
			break;

		// get current value
		cvarptr->value = trap_Cvar_VariableValue( cvarptr->name );

		// get default value
		trap_Cvar_Reset( cvarptr->name );
		cvarptr->defaultvalue = trap_Cvar_VariableValue( cvarptr->name );

		// restore current value
		trap_Cvar_SetValue( cvarptr->name, cvarptr->value );
	}
}

/*
=================
Controls_GetCvarDefault
=================
*/
static float Controls_GetCvarDefault( char* name )
{
	configcvar_t*	cvarptr;
	int				i;

	cvarptr = g_configcvars;
	for (i=0; ;i++,cvarptr++)
	{
		if (!cvarptr->name)
			return (0);

		if (!strcmp(cvarptr->name,name))
			break;
	}

	return (cvarptr->defaultvalue);
}

/*
=================
Controls_GetCvarValue
=================
*/
static float Controls_GetCvarValue( char* name )
{
	configcvar_t*	cvarptr;
	int				i;

	cvarptr = g_configcvars;
	for (i=0; ;i++,cvarptr++)
	{
		if (!cvarptr->name)
			return (0);

		if (!strcmp(cvarptr->name,name))
			break;
	}

	return (cvarptr->value);
}

static int Controls_BindingCount( void )
{
	int i;

	for ( i = 0; g_bindings[i].label && i < CONTROLS_PROFILE_BINDINGS; i++ ) {
	}

	return i;
}

static const char *Controls_ProfileName( int mode )
{
	return mode == CONTROLS_INPUT_CONTROLLER ? "controller" : "keyboard";
}

static qboolean Controls_ProfileCvarsInitialized( int mode )
{
	return trap_Cvar_VariableValue( va( "q3r_ctrl_%s_init", Controls_ProfileName( mode ) ) ) != 0;
}

static int Controls_DefaultKeyForProfile( int mode, int id, int fallback )
{
	if ( mode != CONTROLS_INPUT_CONTROLLER ) {
		return fallback;
	}

	switch ( id ) {
		case ID_SHOWSCORES: return K_PAD0_TOUCHPAD;
		case ID_ACCEL: return K_PAD0_RIGHTTRIGGER;
		case ID_BRAKE: return K_PAD0_LEFTTRIGGER;
		case ID_LEFT: return K_PAD0_LEFTSTICK_LEFT;
		case ID_RIGHT: return K_PAD0_LEFTSTICK_RIGHT;
		case ID_MOVEUP: return K_PAD0_A;
		case ID_HANDBRAKE: return K_PAD0_B;
		case ID_TURBO: return K_PAD0_Y;
		case ID_GEARUP: return K_PAD0_RIGHTSHOULDER;
		case ID_GEARDOWN: return K_PAD0_LEFTSHOULDER;
		case ID_HEADLIGHT: return K_PAD0_DPAD_UP;
		case ID_HORN: return K_PAD0_LEFTSTICK_CLICK;
		case ID_NEXTCAMERA: return K_PAD0_RIGHTSTICK_CLICK;
		case ID_USEITEM: return K_PAD0_X;
		case ID_DROPITEM: return K_PAD0_DPAD_DOWN;
		case ID_REARATTACK: return K_PAD0_DPAD_RIGHT;
		case ID_DROP_REAR: return K_PAD0_DPAD_LEFT;
		default: return -1;
	}
}

static void Controls_LoadDefaultProfile( int mode )
{
	int i;
	int count;

	count = Controls_BindingCount();
	for ( i = 0; i < count; i++ ) {
		s_controlsProfileKeys[mode][i] = Controls_DefaultKeyForProfile( mode, g_bindings[i].id, g_bindings[i].defaultbind1 );
	}

	s_controlsProfileJoyEnable[mode] = mode == CONTROLS_INPUT_CONTROLLER ? 1.0f : 0.0f;
	s_controlsProfileJoyAnalog[mode] = mode == CONTROLS_INPUT_CONTROLLER ? 1.0f : 0.0f;
	s_controlsProfileJoyThreshold[mode] = 0.15f;
	s_controlsProfileLoaded[mode] = qtrue;
}

static void Controls_StoreProfile( int mode )
{
	int i;
	int count;

	mode = mode == CONTROLS_INPUT_CONTROLLER ? CONTROLS_INPUT_CONTROLLER : CONTROLS_INPUT_KEYBOARD;
	count = Controls_BindingCount();

	for ( i = 0; i < count; i++ ) {
		s_controlsProfileKeys[mode][i] = g_bindings[i].bind1;
	}

	s_controlsProfileJoyEnable[mode] = s_controls.joyenable.curvalue;
	s_controlsProfileJoyAnalog[mode] = s_controls.joyanalog.curvalue;
	s_controlsProfileJoyThreshold[mode] = s_controls.joythreshold.curvalue;
	s_controlsProfileLoaded[mode] = qtrue;
}

static void Controls_StoreCurrentProfile( void )
{
	Controls_StoreProfile( s_controlsActiveProfileMode );
}

static void Controls_ApplyProfile( int mode )
{
	int i;
	int count;

	if ( !s_controlsProfileLoaded[mode] ) {
		Controls_LoadDefaultProfile( mode );
	}

	count = Controls_BindingCount();
	for ( i = 0; i < count; i++ ) {
		g_bindings[i].bind1 = s_controlsProfileKeys[mode][i];
		g_bindings[i].bind2 = -1;
	}

	s_controls.inputmode.curvalue = mode;
	s_controls.joyenable.curvalue = s_controlsProfileJoyEnable[mode];
	s_controls.joyanalog.curvalue = s_controlsProfileJoyAnalog[mode];
	s_controls.joythreshold.curvalue = s_controlsProfileJoyThreshold[mode];
	s_controlsActiveProfileMode = mode;
}

static void Controls_LoadProfileCvars( int mode )
{
	int i;
	int count;

	if ( !Controls_ProfileCvarsInitialized( mode ) ) {
		Controls_LoadDefaultProfile( mode );
		return;
	}

	count = Controls_BindingCount();
	for ( i = 0; i < count; i++ ) {
		int stored = (int)trap_Cvar_VariableValue( va( "q3r_ctrl_%s_%i", Controls_ProfileName( mode ), g_bindings[i].id ) );
		s_controlsProfileKeys[mode][i] = stored > 0 ? stored - 2 : -1;
	}

	s_controlsProfileJoyEnable[mode] = UI_ClampCvar( 0, 1, trap_Cvar_VariableValue( va( "q3r_ctrl_%s_joyenable", Controls_ProfileName( mode ) ) ) );
	s_controlsProfileJoyAnalog[mode] = UI_ClampCvar( 0, 1, trap_Cvar_VariableValue( va( "q3r_ctrl_%s_joyanalog", Controls_ProfileName( mode ) ) ) );
	s_controlsProfileJoyThreshold[mode] = UI_ClampCvar( 0.05f, 0.75f, trap_Cvar_VariableValue( va( "q3r_ctrl_%s_joythreshold", Controls_ProfileName( mode ) ) ) );
	s_controlsProfileLoaded[mode] = qtrue;
}

static void Controls_SaveProfileCvars( int mode )
{
	int i;
	int count;

	count = Controls_BindingCount();
	for ( i = 0; i < count; i++ ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, va( "seta q3r_ctrl_%s_%i \"%i\"\n", Controls_ProfileName( mode ), g_bindings[i].id, s_controlsProfileKeys[mode][i] + 2 ) );
	}

	trap_Cmd_ExecuteText( EXEC_APPEND, va( "seta q3r_ctrl_%s_joyenable \"%i\"\n", Controls_ProfileName( mode ), (int)s_controlsProfileJoyEnable[mode] ) );
	trap_Cmd_ExecuteText( EXEC_APPEND, va( "seta q3r_ctrl_%s_joyanalog \"%i\"\n", Controls_ProfileName( mode ), (int)s_controlsProfileJoyAnalog[mode] ) );
	trap_Cmd_ExecuteText( EXEC_APPEND, va( "seta q3r_ctrl_%s_joythreshold \"%.3f\"\n", Controls_ProfileName( mode ), s_controlsProfileJoyThreshold[mode] ) );
	trap_Cmd_ExecuteText( EXEC_APPEND, va( "seta q3r_ctrl_%s_init \"1\"\n", Controls_ProfileName( mode ) ) );
}

static void Controls_ApplyInputModeProfile( int mode )
{
	int oldMode;

	mode = mode == CONTROLS_INPUT_CONTROLLER ? CONTROLS_INPUT_CONTROLLER : CONTROLS_INPUT_KEYBOARD;
	oldMode = s_controlsActiveProfileMode == CONTROLS_INPUT_CONTROLLER ? CONTROLS_INPUT_CONTROLLER : CONTROLS_INPUT_KEYBOARD;

	Controls_StoreProfile( oldMode );
	if ( mode != oldMode ) {
		Controls_ApplyProfile( mode );
	}

	s_controls.changesmade = qtrue;
}

static void Controls_ClearCommandBindings( const char *command )
{
	int j;
	char b[256];

	for ( j = 0; j < MAX_KEYS; j++ )
	{
		trap_Key_GetBindingBuf( j, b, sizeof( b ) );
		if ( *b && !Q_stricmp( b, command ) )
		{
			trap_Key_SetBinding( j, "" );
		}
	}
}


/*
=================
Controls_UpdateModel
=================
*/
static void Controls_UpdateModel( int anim ) {
	VectorClear( s_controls.playerViewangles );
	VectorClear( s_controls.playerMoveangles );
	s_controls.playerViewangles[YAW] = 0;
	s_controls.playerMoveangles[YAW] = s_controls.playerViewangles[YAW];
	s_controls.playerLegs		     = LEGS_IDLE;
	s_controls.playerTorso			 = TORSO_STAND;
//	s_controls.playerWeapon			 = -1;
    s_controls.playerWeapon			 = WP_NUM_WEAPONS;
	s_controls.playerChat			 = qfalse;

	switch( anim ) {
// STONELANCE
/*
	case ANIM_RUN:	
		s_controls.playerLegs = LEGS_RUN;
		break;
*/
// END
	case ANIM_WALK:	
		s_controls.playerLegs = LEGS_WALK;
		break;

	case ANIM_BACK:	
		s_controls.playerLegs = LEGS_BACK;
		break;

	case ANIM_JUMP:	
		s_controls.playerLegs = LEGS_JUMP;
		break;

	case ANIM_CROUCH:	
		s_controls.playerLegs = LEGS_IDLECR;
		break;

	case ANIM_TURNLEFT:
		s_controls.playerViewangles[YAW] += 70;
		break;

	case ANIM_TURNRIGHT:
		s_controls.playerViewangles[YAW] -= 70;
		break;

	case ANIM_STEPLEFT:
		s_controls.playerLegs = LEGS_WALK;
		s_controls.playerMoveangles[YAW] = s_controls.playerViewangles[YAW] + 70;
		break;

	case ANIM_STEPRIGHT:
		s_controls.playerLegs = LEGS_WALK;
		s_controls.playerMoveangles[YAW] = s_controls.playerViewangles[YAW] - 70;
		break;

	case ANIM_LOOKUP:
		s_controls.playerViewangles[PITCH] = -45;
		break;

	case ANIM_LOOKDOWN:
		s_controls.playerViewangles[PITCH] = 45;
		break;

	case ANIM_WEAPON1:
		s_controls.playerWeapon = WP_GAUNTLET;
		break;

	case ANIM_WEAPON2:
		s_controls.playerWeapon = WP_MACHINEGUN;
		break;

	case ANIM_WEAPON3:
		s_controls.playerWeapon = WP_SHOTGUN;
		break;

	case ANIM_WEAPON4:
		s_controls.playerWeapon = WP_GRENADE_LAUNCHER;
		break;

	case ANIM_WEAPON5:
		s_controls.playerWeapon = WP_ROCKET_LAUNCHER;
		break;

	case ANIM_WEAPON6:
		s_controls.playerWeapon = WP_LIGHTNING;
		break;

	case ANIM_WEAPON7:
		s_controls.playerWeapon = WP_RAILGUN;
		break;

	case ANIM_WEAPON8:
		s_controls.playerWeapon = WP_PLASMAGUN;
		break;

	case ANIM_WEAPON9:
		s_controls.playerWeapon = WP_BFG;
		break;

	case ANIM_WEAPON10:
	   s_controls.playerWeapon = WP_FLAME_THROWER;
	   break;

	case ANIM_ATTACK:
		s_controls.playerTorso = TORSO_ATTACK;
		break;
		
	case ANIM_ALT_ATTACK:
	   s_controls.playerTorso = TORSO_ATTACK;
	   break;
// STONELANCE
/*
	case ANIM_GESTURE:
		s_controls.playerTorso = TORSO_GESTURE;
		break;
*/
// END
	case ANIM_DIE:
		s_controls.playerLegs = BOTH_DEATH1;
		s_controls.playerTorso = BOTH_DEATH1;
		s_controls.playerWeapon = WP_NONE;
		break;

	case ANIM_CHAT:
		s_controls.playerChat = qtrue;
		break;

	default:
		break;
	}

	UI_PlayerInfo_SetInfo( &s_controls.playerinfo, s_controls.playerLegs, s_controls.playerTorso, s_controls.playerViewangles, s_controls.playerMoveangles, s_controls.playerWeapon, s_controls.playerChat );
}


/*
=================
Controls_Update
=================
*/
static void Controls_Update( void ) {
	int		i;
	int		j;
	int		y;
	menucommon_s	**controls;
	menucommon_s	*control;
	qboolean	searchActive;

	searchActive = Controls_SearchActive();

	if ( s_controls.section == C_DEVELOPER && !Controls_ShowDeveloper() ) {
		s_controls.section = C_MOVEMENT;
	}

	if ( Controls_ShowDeveloper() ) {
		s_controls.developer.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	} else {
		s_controls.developer.generic.flags |= (QMF_HIDDEN|QMF_INACTIVE);
	}

	// disable all controls in all groups
	for( i = 0; i < C_MAX; i++ ) {
		controls = g_controls[i];
		for( j = 0;	(control = controls[j]); j++ ) {
			control->flags |= (QMF_HIDDEN|QMF_INACTIVE);
		}
	}

	if ( searchActive ) {
		Controls_BuildGlobalSearchList();
		for ( j = 0; j < s_globalSearchControlCount; j++ ) {
			s_globalSearchControls[j]->flags &= ~(QMF_GRAYED|QMF_HIDDEN|QMF_INACTIVE);
		}
		y = ( SCREEN_HEIGHT - s_globalSearchControlCount * SMALLCHAR_HEIGHT ) / 2;
		for ( j = 0; j < s_globalSearchControlCount; j++, y += SMALLCHAR_HEIGHT ) {
			control = s_globalSearchControls[j];
			control->x      = 410 + (int)(((y - 240) / 14.0F) * ((y - 240) / 14.0F));
			control->y      = y;
			control->left   = control->x - 19*SMALLCHAR_WIDTH;
			control->right  = control->x + 21*SMALLCHAR_WIDTH;
			control->top    = y;
			control->bottom = y + SMALLCHAR_HEIGHT;
		}
	} else {
		controls = g_controls[s_controls.section];

		// enable controls in active group (and count number of items for vertical centering)
		for( i = 0, j = 0; (control = controls[i]); i++ ) {
			if ( Controls_ControlVisibleForInputMode( control ) ) {
				control->flags &= ~(QMF_GRAYED|QMF_HIDDEN|QMF_INACTIVE);
				j++;
			}
		}

		// position controls
		y = ( SCREEN_HEIGHT - j * SMALLCHAR_HEIGHT ) / 2;
		for( j = 0;	(control = controls[j]); j++ ) {
			if ( !Controls_ControlVisibleForInputMode( control ) ) {
				continue;
			}

			control->x      = 300 + (int)(((y - 240) / 14.0F) * ((y - 240) / 14.0F));
			control->y      = y;
			control->left   = control->x - 19*SMALLCHAR_WIDTH;
			control->right  = control->x + 21*SMALLCHAR_WIDTH;
			control->top    = y;
			control->bottom = y + SMALLCHAR_HEIGHT;
			y += SMALLCHAR_HEIGHT;
		}
	}

	if( s_controls.waitingforkey ) {
		// disable everybody
		for( i = 0; i < s_controls.menu.nitems; i++ ) {
			((menucommon_s*)(s_controls.menu.items[i]))->flags |= QMF_GRAYED;
		}

		// enable action item
		((menucommon_s*)(s_controls.menu.items[s_controls.menu.cursor]))->flags &= ~QMF_GRAYED;

		return;
	}

	// enable everybody
	for( i = 0; i < s_controls.menu.nitems; i++ ) {
		((menucommon_s*)(s_controls.menu.items[i]))->flags &= ~QMF_GRAYED;
	}

	// makes sure flags are right on the group selection controls
	s_controls.looking.generic.flags  &= ~(QMF_GRAYED|QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
	s_controls.movement.generic.flags &= ~(QMF_GRAYED|QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
	s_controls.combat.generic.flags   &= ~(QMF_GRAYED|QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
	s_controls.weapons.generic.flags  &= ~(QMF_GRAYED|QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
	s_controls.misc.generic.flags     &= ~(QMF_GRAYED|QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
	s_controls.developer.generic.flags &= ~(QMF_GRAYED|QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);

	s_controls.looking.generic.flags  |= QMF_PULSEIFFOCUS;
	s_controls.movement.generic.flags |= QMF_PULSEIFFOCUS;
	s_controls.combat.generic.flags   |= QMF_PULSEIFFOCUS;
	s_controls.weapons.generic.flags  |= QMF_PULSEIFFOCUS;
	s_controls.misc.generic.flags     |= QMF_PULSEIFFOCUS;
	s_controls.developer.generic.flags |= QMF_PULSEIFFOCUS;

	// set buttons
	switch( s_controls.section ) {
	case C_MOVEMENT:
		s_controls.movement.generic.flags &= ~QMF_PULSEIFFOCUS;
		s_controls.movement.generic.flags |= (QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
		break;
	
	case C_LOOKING:
		s_controls.looking.generic.flags &= ~QMF_PULSEIFFOCUS;
		s_controls.looking.generic.flags |= (QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
		break;

	case C_COMBAT:
		s_controls.combat.generic.flags &= ~QMF_PULSEIFFOCUS;
		s_controls.combat.generic.flags |= (QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
		break;
	
	case C_WEAPONS:
		s_controls.weapons.generic.flags &= ~QMF_PULSEIFFOCUS;
		s_controls.weapons.generic.flags |= (QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
		break;		

	case C_MISC:
		s_controls.misc.generic.flags &= ~QMF_PULSEIFFOCUS;
		s_controls.misc.generic.flags |= (QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
		break;

	case C_DEVELOPER:
		s_controls.developer.generic.flags &= ~QMF_PULSEIFFOCUS;
		s_controls.developer.generic.flags |= (QMF_HIGHLIGHT|QMF_HIGHLIGHT_IF_FOCUS);
		break;
	}

	Menu_AdjustCursor( &s_controls.menu, 1 );
}


/*
=================
Controls_DrawKeyBinding
=================
*/
static void Controls_KeyNameForDisplay( int keynum, char *buf, int buflen )
{
	const char *name = NULL;

	switch ( keynum ) {
		case K_PAD0_A: name = "CROSS"; break;
		case K_PAD0_B: name = "CIRCLE"; break;
		case K_PAD0_X: name = "SQUARE"; break;
		case K_PAD0_Y: name = "TRIANGLE"; break;
		case K_PAD0_BACK: name = "CREATE"; break;
		case K_PAD0_GUIDE: name = "PS"; break;
		case K_PAD0_START: name = "OPTIONS"; break;
		case K_PAD0_LEFTSTICK_CLICK: name = "L3"; break;
		case K_PAD0_RIGHTSTICK_CLICK: name = "R3"; break;
		case K_PAD0_LEFTSHOULDER: name = "L1"; break;
		case K_PAD0_RIGHTSHOULDER: name = "R1"; break;
		case K_PAD0_DPAD_UP: name = "D-UP"; break;
		case K_PAD0_DPAD_DOWN: name = "D-DOWN"; break;
		case K_PAD0_DPAD_LEFT: name = "D-LEFT"; break;
		case K_PAD0_DPAD_RIGHT: name = "D-RIGHT"; break;
		case K_PAD0_LEFTSTICK_LEFT: name = "LS LEFT"; break;
		case K_PAD0_LEFTSTICK_RIGHT: name = "LS RIGHT"; break;
		case K_PAD0_LEFTSTICK_UP: name = "LS UP"; break;
		case K_PAD0_LEFTSTICK_DOWN: name = "LS DOWN"; break;
		case K_PAD0_RIGHTSTICK_LEFT: name = "RS LEFT"; break;
		case K_PAD0_RIGHTSTICK_RIGHT: name = "RS RIGHT"; break;
		case K_PAD0_RIGHTSTICK_UP: name = "RS UP"; break;
		case K_PAD0_RIGHTSTICK_DOWN: name = "RS DOWN"; break;
		case K_PAD0_LEFTTRIGGER: name = "L2"; break;
		case K_PAD0_RIGHTTRIGGER: name = "R2"; break;
		case K_PAD0_MISC1: name = "MISC"; break;
		case K_PAD0_PADDLE1: name = "PADDLE 1"; break;
		case K_PAD0_PADDLE2: name = "PADDLE 2"; break;
		case K_PAD0_PADDLE3: name = "PADDLE 3"; break;
		case K_PAD0_PADDLE4: name = "PADDLE 4"; break;
		case K_PAD0_TOUCHPAD: name = "TOUCHPAD"; break;
	}

	if ( name ) {
		Q_strncpyz( buf, name, buflen );
		return;
	}

	trap_Key_KeynumToStringBuf( keynum, buf, buflen );
	Q_strupr( buf );
}

static void Controls_DrawKeyBinding( void *self )
{
	menuaction_s*	a;
	int				x;
	int				y;
	int				fillLeft;
	int				fillRight;
	int				b1;
	int				b2;
	qboolean		c;
	char			name[96];
	char			label[96];

	a = (menuaction_s*) self;

	x =	a->generic.x;
	y = a->generic.y;

	c = (Menu_ItemAtCursor( a->generic.parent ) == a);

	// find the binding
	for (b1 = 0; g_bindings[b1].command; b1++) {
		if (g_bindings[b1].id == a->generic.id) {
			break;
		}
	}

	if (!g_bindings[b1].command) {
		strcpy(name, "<OUT OF RANGE>");
		strcpy(label, "<OUT OF RANGE>");
	} else {
		b2 = g_bindings[b1].bind1;
		if (b2 == -1) {
			strcpy(name, "-?-");
		} else {
			Controls_KeyNameForDisplay( b2, name, sizeof( name ) );
		}

		if ( Controls_SearchActive() ) {
			Com_sprintf( label, sizeof( label ), "[%s] %s", Controls_SectionTagForAction( a->generic.id ), g_bindings[b1].label );
		} else {
			Q_strncpyz( label, g_bindings[b1].label, sizeof( label ) );
		}
	}

	if (c)
	{
		fillLeft = x - SMALLCHAR_WIDTH - strlen( label ) * SMALLCHAR_WIDTH - 2;
		fillRight = x + SMALLCHAR_WIDTH + strlen( name ) * SMALLCHAR_WIDTH + 2;
		UI_FillRect( fillLeft, a->generic.top, fillRight - fillLeft + 1, a->generic.bottom-a->generic.top+1, listbar_color );

		UI_DrawString( x - SMALLCHAR_WIDTH, y, label, UI_RIGHT|UI_SMALLFONT, text_color_highlight );
		UI_DrawString( x + SMALLCHAR_WIDTH, y, name, UI_LEFT|UI_SMALLFONT|UI_PULSE, text_color_highlight );

		if (s_controls.waitingforkey)
		{
			UI_DrawChar( x, y, '=', UI_CENTER|UI_BLINK|UI_SMALLFONT, text_color_highlight);
			UI_DrawString(SCREEN_WIDTH * 0.50, SCREEN_HEIGHT * 0.84, "Waiting for new key ... Escape = cancel", UI_SMALLFONT|UI_CENTER|UI_PULSE, colorWhite );
		}
		else
		{
			UI_DrawChar( x, y, 13, UI_CENTER|UI_BLINK|UI_SMALLFONT, text_color_highlight);
			UI_DrawString(SCREEN_WIDTH * 0.50, SCREEN_HEIGHT * 0.84, "One input per action.", UI_SMALLFONT|UI_CENTER, colorWhite );
			UI_DrawString(SCREEN_WIDTH * 0.50, SCREEN_HEIGHT * 0.88, "Enter/Click = rebind", UI_SMALLFONT|UI_CENTER, colorWhite );
			UI_DrawString(SCREEN_WIDTH * 0.50, SCREEN_HEIGHT * 0.92, "Backspace = clear | Escape = cancel", UI_SMALLFONT|UI_CENTER, colorWhite );
		}
		UI_DrawString(SCREEN_WIDTH * 0.50, SCREEN_HEIGHT * 0.95, "Type to search (across all categories)", UI_SMALLFONT|UI_CENTER, colorWhite );
		if ( Controls_SearchActive() ) {
			UI_DrawString(SCREEN_WIDTH * 0.50, SCREEN_HEIGHT * 0.98, va("Search: %s", s_controlsSearchText), UI_SMALLFONT|UI_CENTER, colorWhite );
		}
	}
	else
	{
		if (a->generic.flags & QMF_GRAYED)
		{
			UI_DrawString( x - SMALLCHAR_WIDTH, y, label, UI_RIGHT|UI_SMALLFONT, text_color_disabled );
			UI_DrawString( x + SMALLCHAR_WIDTH, y, name, UI_LEFT|UI_SMALLFONT, text_color_disabled );
		}
		else
		{
			UI_DrawString( x - SMALLCHAR_WIDTH, y, label, UI_RIGHT|UI_SMALLFONT, text_color_normal );
			UI_DrawString( x + SMALLCHAR_WIDTH, y, name, UI_LEFT|UI_SMALLFONT, text_color_normal );
		}
	}
}

static void Controls_DrawRadioButton( void *self )
{
	menuradiobutton_s* rb;
	int x;
	int y;
	int fillLeft;
	int fillRight;
	int style;
	const char* value;
	float* color;
	qboolean focus;

	rb = (menuradiobutton_s*)self;
	x = rb->generic.x;
	y = rb->generic.y;
	value = rb->curvalue ? "on" : "off";
	focus = (rb->generic.parent->cursor == rb->generic.menuPosition);

	style = UI_LEFT|UI_SMALLFONT;
	if ( rb->generic.flags & QMF_GRAYED ) {
		color = text_color_disabled;
	} else if ( focus ) {
		color = text_color_highlight;
		style |= UI_PULSE;
	} else {
		color = text_color_normal;
	}

	if ( focus ) {
		fillLeft = x - SMALLCHAR_WIDTH - strlen( rb->generic.name ) * SMALLCHAR_WIDTH - 2;
		fillRight = x + SMALLCHAR_WIDTH + 16 + strlen( value ) * SMALLCHAR_WIDTH + 2;
		UI_FillRect( fillLeft, rb->generic.top, fillRight - fillLeft + 1, rb->generic.bottom-rb->generic.top+1, listbar_color );
		UI_DrawChar( x, y, 13, UI_CENTER|UI_BLINK|UI_SMALLFONT, color );
	}

	UI_DrawString( x - SMALLCHAR_WIDTH, y, rb->generic.name, UI_RIGHT|UI_SMALLFONT, color );
	UI_DrawHandlePic( x + SMALLCHAR_WIDTH, y + 2, 16, 16, rb->curvalue ? uis.rb_on : uis.rb_off );
	UI_DrawString( x + SMALLCHAR_WIDTH + 16, y, value, style, color );
}

static void Controls_DrawSlider( void *self )
{
	menuslider_s* s;
	int x;
	int y;
	int i;
	int fillLeft;
	int fillRight;
	int style;
	float* color;
	qboolean focus;

	s = (menuslider_s*)self;
	x = s->generic.x;
	y = s->generic.y;
	focus = (s->generic.parent->cursor == s->generic.menuPosition);

	style = UI_SMALLFONT;
	if ( s->generic.flags & QMF_GRAYED ) {
		color = text_color_disabled;
	} else if ( focus ) {
		color = text_color_highlight;
		style |= UI_PULSE;
	} else {
		color = text_color_normal;
	}

	if ( focus ) {
		fillLeft = x - SMALLCHAR_WIDTH - strlen( s->generic.name ) * SMALLCHAR_WIDTH - 2;
		fillRight = x + (SLIDER_RANGE + 3) * SMALLCHAR_WIDTH + 2;
		UI_FillRect( fillLeft, s->generic.top, fillRight - fillLeft + 1, s->generic.bottom-s->generic.top+1, listbar_color );
		UI_DrawChar( x, y, 13, UI_CENTER|UI_BLINK|UI_SMALLFONT, color );
	}

	UI_DrawString( x - SMALLCHAR_WIDTH, y, s->generic.name, UI_RIGHT|style, color );
	UI_DrawChar( x + SMALLCHAR_WIDTH, y, 128, UI_LEFT|style, color );
	for ( i = 0; i < SLIDER_RANGE; i++ ) {
		UI_DrawChar( x + (i+2)*SMALLCHAR_WIDTH, y, 129, UI_LEFT|style, color );
	}
	UI_DrawChar( x + (i+2)*SMALLCHAR_WIDTH, y, 130, UI_LEFT|style, color );

	if (s->maxvalue > s->minvalue) {
		s->range = ( s->curvalue - s->minvalue ) / ( float ) ( s->maxvalue - s->minvalue );
		if ( s->range < 0 ) {
			s->range = 0;
		} else if ( s->range > 1 ) {
			s->range = 1;
		}
	} else {
		s->range = 0;
	}

	if ( style & UI_PULSE ) {
		style &= ~UI_PULSE;
		style |= UI_BLINK;
	}
	UI_DrawChar( (int)( x + 2*SMALLCHAR_WIDTH + (SLIDER_RANGE-1)*SMALLCHAR_WIDTH* s->range ), y, 131, UI_LEFT|style, color );
}

static void Controls_DrawSearchField( void *self )
{
	menufield_s* f;
	int x;
	int y;
	int w;
	int style;
	int visibleChars;
	int fillRight;
	float* color;
	qboolean focus;

	f = (menufield_s*)self;
	x = f->generic.x;
	y = f->generic.y;
	w = SMALLCHAR_WIDTH;
	style = UI_SMALLFONT;
	focus = (Menu_ItemAtCursor( f->generic.parent ) == f);

	if ( focus ) {
		style |= UI_PULSE;
	}

	if ( f->generic.flags & QMF_GRAYED ) {
		color = text_color_disabled;
	} else if ( focus ) {
		color = text_color_highlight;
	} else {
		color = text_color_normal;
	}

	if ( focus ) {
		visibleChars = strlen( f->field.buffer ) - f->field.scroll;
		if ( visibleChars < 1 ) {
			visibleChars = 1;
		}
		if ( visibleChars > f->field.widthInChars ) {
			visibleChars = f->field.widthInChars;
		}

		fillRight = x + w + visibleChars * w + 2;
		UI_FillRect( x - 2, f->generic.top, fillRight - x + 3, f->generic.bottom-f->generic.top+1, listbar_color );
		UI_DrawChar( x, y, 13, UI_CENTER|UI_BLINK|style, color );
	}

	MField_Draw( &f->field, x + w, y, style, color );
}


/*
=================
Controls_StatusBar
=================
*/
static void Controls_StatusBar( void *self )
{
	UI_DrawString(SCREEN_WIDTH * 0.50, SCREEN_HEIGHT * 0.82, "One input per action.", UI_SMALLFONT|UI_CENTER, colorWhite );
	UI_DrawString(SCREEN_WIDTH * 0.50, SCREEN_HEIGHT * 0.86, "Enter/Click = rebind | Backspace = clear | Escape = cancel", UI_SMALLFONT|UI_CENTER, colorWhite );
	UI_DrawString(SCREEN_WIDTH * 0.50, SCREEN_HEIGHT * 0.90, "Use Arrow Keys or Click to change options", UI_SMALLFONT|UI_CENTER, colorWhite );
	UI_DrawString(SCREEN_WIDTH * 0.50, SCREEN_HEIGHT * 0.94, "Type to search (across all categories)", UI_SMALLFONT|UI_CENTER, colorWhite );
	if ( Controls_SearchActive() ) {
		UI_DrawString(SCREEN_WIDTH * 0.50, SCREEN_HEIGHT * 0.98, va("Search: %s", s_controlsSearchText), UI_SMALLFONT|UI_CENTER, colorWhite );
	}
}


/*
=================
Controls_DrawPlayer
=================
*/
static void Controls_DrawPlayer( void *self ) {
	menubitmap_s	*b;
	char			buf[MAX_QPATH];
// STONELANCE
	char	rim[MAX_QPATH];
	char	head[MAX_QPATH];
	char	plate[MAX_QPATH];
// END

	trap_Cvar_VariableStringBuffer( "model", buf, sizeof( buf ) );
	if ( strcmp( buf, s_controls.playerModel ) != 0 ) {
// STONELANCE
//		UI_PlayerInfo_SetModel( &s_controls.playerinfo, buf );
		trap_Cvar_VariableStringBuffer( "rim", rim, sizeof( rim ) );
		trap_Cvar_VariableStringBuffer( "head", head, sizeof( head ) );
		trap_Cvar_VariableStringBuffer( "plate", plate, sizeof( plate ) );
		UI_PlayerInfo_SetModel( &s_controls.playerinfo, buf, rim, head, plate );
// END
		strcpy( s_controls.playerModel, buf );
		Controls_UpdateModel( ANIM_IDLE );
	}

	b = (menubitmap_s*) self;
	UI_DrawPlayer( b->generic.x, b->generic.y, b->width, b->height, &s_controls.playerinfo, uis.realtime );
}


/*
=================
Controls_GetKeyAssignment
=================
*/
static void Controls_GetKeyAssignment (char *command, int *twokeys)
{
	int		count;
	int		j;
	char	b[256];

	twokeys[0] = twokeys[1] = -1;
	count = 0;

	for ( j = 0; j < MAX_KEYS; j++ )
	{
		trap_Key_GetBindingBuf( j, b, 256 );
		if ( *b == 0 ) {
			continue;
		}
		if ( !Q_stricmp( b, command ) ) {
			twokeys[count] = j;
			count++;
			if (count == 2)
				break;
		}
	}
}

/*
=================
RallyControls_GetConfig
=================
*/
static void RallyControls_GetConfig( void )
{
	int		i;
	int		mode;
	int		otherMode;
	int		twokeys[2];
	bind_t*	bindptr;

	// put the bindings into a local store
	bindptr = g_bindings;

	// iterate each command, get its numeric binding
	for (i=0; ;i++,bindptr++)
	{
		if (!bindptr->label)
			break;

		Controls_GetKeyAssignment(bindptr->command, twokeys);

		bindptr->bind1 = twokeys[0];
		bindptr->bind2 = -1;
	}

	s_controls.inputmode.curvalue    = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "cg_controlMode" ) );
	s_controls.invertmouse.curvalue  = Controls_GetCvarValue( "m_pitch" ) < 0;
	s_controls.smoothmouse.curvalue  = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "m_filter" ) );
	s_controls.alwaysrun.curvalue    = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "cl_run" ) );
	s_controls.autoswitch.curvalue   = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "cg_autoswitch" ) );
	s_controls.sensitivity.curvalue  = UI_ClampCvar( 2, 30, Controls_GetCvarValue( "sensitivity" ) );
	s_controls.joyenable.curvalue    = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "in_joystick" ) );
	s_controls.joyanalog.curvalue    = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "in_joystickUseAnalog" ) );
	s_controls.joythreshold.curvalue = UI_ClampCvar( 0.05, 0.75, Controls_GetCvarValue( "joy_threshold" ) );
        s_controls.freelook.curvalue     = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "cl_freelook" ) );
// STONELANCE
        s_controls.autodroprear.curvalue = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "cg_autodrop" ) );
// END

	mode = s_controls.inputmode.curvalue == CONTROLS_INPUT_CONTROLLER ? CONTROLS_INPUT_CONTROLLER : CONTROLS_INPUT_KEYBOARD;
	otherMode = mode == CONTROLS_INPUT_CONTROLLER ? CONTROLS_INPUT_KEYBOARD : CONTROLS_INPUT_CONTROLLER;

	if ( Controls_ProfileCvarsInitialized( mode ) ) {
		Controls_LoadProfileCvars( mode );
	} else {
		Controls_StoreProfile( mode );
	}

	Controls_LoadProfileCvars( otherMode );
	Controls_ApplyProfile( mode );
}

/*
=================
RallyControls_SetConfig
=================
*/
static void RallyControls_SetConfig( void )
{
	int		i;
	bind_t*	bindptr;

	Controls_StoreCurrentProfile();
	Controls_SaveProfileCvars( CONTROLS_INPUT_KEYBOARD );
	Controls_SaveProfileCvars( CONTROLS_INPUT_CONTROLLER );

	// set the bindings from the local store
	bindptr = g_bindings;

	// iterate each command, get its numeric binding
	for (i=0; ;i++,bindptr++)
	{
		if (!bindptr->label)
			break;

		Controls_ClearCommandBindings( bindptr->command );

		if (bindptr->bind1 != -1)
		{	
			trap_Key_SetBinding( bindptr->bind1, bindptr->command );
		}
	}

	if ( s_controls.invertmouse.curvalue )
		trap_Cvar_SetValue( "m_pitch", -fabs( trap_Cvar_VariableValue( "m_pitch" ) ) );
	else
		trap_Cvar_SetValue( "m_pitch", fabs( trap_Cvar_VariableValue( "m_pitch" ) ) );

	trap_Cvar_SetValue( "m_filter", s_controls.smoothmouse.curvalue );
	trap_Cvar_SetValue( "cl_run", s_controls.alwaysrun.curvalue );
	trap_Cvar_SetValue( "cg_autoswitch", s_controls.autoswitch.curvalue );
	trap_Cvar_SetValue( "sensitivity", s_controls.sensitivity.curvalue );
	trap_Cvar_SetValue( "cg_controlMode", s_controls.inputmode.curvalue );
	trap_Cvar_SetValue( "in_joystick", s_controls.joyenable.curvalue );
	trap_Cvar_SetValue( "in_joystickUseAnalog", s_controls.joyanalog.curvalue );
	trap_Cvar_SetValue( "joy_threshold", s_controls.joythreshold.curvalue );
	trap_Cmd_ExecuteText( EXEC_APPEND, s_controls.inputmode.curvalue == 1 ? "+strafe\n" : "-strafe\n" );
        trap_Cvar_SetValue( "cl_freelook", s_controls.freelook.curvalue );
// STONELANCE
        trap_Cvar_SetValue( "cg_autodrop", s_controls.autodroprear.curvalue );
// END
        trap_Cmd_ExecuteText( EXEC_APPEND, "in_restart\n" );
}

/*
=================
RallyControls_SetDefaults
=================
*/
static void RallyControls_SetDefaults( void )
{
	int mode;

	mode = s_controls.inputmode.curvalue == CONTROLS_INPUT_CONTROLLER ? CONTROLS_INPUT_CONTROLLER : CONTROLS_INPUT_KEYBOARD;
	Controls_LoadDefaultProfile( mode );
	Controls_ApplyProfile( mode );
}

static bind_t* Controls_FindConflictingBinding( int key, int currentId )
{
	bind_t *bindptr;
	int i;

	bindptr = g_bindings;
	for ( i = 0; ; i++, bindptr++ )
	{
		if ( !bindptr->label )
			break;

		if ( bindptr->id == currentId )
			continue;

		if ( bindptr->bind1 == key || bindptr->bind2 == key )
			return bindptr;
	}

	return NULL;
}

static qboolean Controls_ApplyBindingChange( int id, int key )
{
	int i;
	qboolean found;
	bind_t *bindptr;

	found = qfalse;
	s_controls.changesmade = qtrue;

	if ( key != -1 )
	{
		// remove from any other bind
		bindptr = g_bindings;
		for ( i = 0; ; i++, bindptr++ )
		{
			if ( !bindptr->label )
				break;

			if ( bindptr->bind2 == key )
				bindptr->bind2 = -1;

			if ( bindptr->bind1 == key )
			{
				bindptr->bind1 = -1;
				bindptr->bind2 = -1;
			}
		}
	}

	// assign key to local store
	bindptr = g_bindings;
	for ( i = 0; ; i++, bindptr++ )
	{
		if ( !bindptr->label )
			break;

		if ( bindptr->id == id )
		{
			found = qtrue;
			if ( key == -1 )
			{
				if ( bindptr->bind1 != -1 ) {
					trap_Key_SetBinding( bindptr->bind1, "" );
					bindptr->bind1 = -1;
				}
				if ( bindptr->bind2 != -1 ) {
					trap_Key_SetBinding( bindptr->bind2, "" );
					bindptr->bind2 = -1;
				}
			}
			else if ( bindptr->bind1 == -1 ) {
				bindptr->bind1 = key;
				bindptr->bind2 = -1;
			}
			else
			{
				if ( bindptr->bind1 != -1 )
					trap_Key_SetBinding( bindptr->bind1, "" );
				if ( bindptr->bind2 != -1 )
					trap_Key_SetBinding( bindptr->bind2, "" );
				bindptr->bind1 = key;
				bindptr->bind2 = -1;
			}
			break;
		}
	}

	s_controls.waitingforkey = qfalse;

	if ( found )
		Controls_Update();

	return found;
}

static void Controls_RebindConflict_Action( qboolean result )
{
	if ( result ) {
		Controls_ApplyBindingChange( s_rebindConfirmTargetId, s_rebindConfirmKey );
	} else {
		s_controls.waitingforkey = qfalse;
		Controls_Update();
	}

	s_rebindConfirmTargetId = -1;
	s_rebindConfirmKey = -1;
}

static void Controls_RebindConflict_Draw( void )
{
	UI_DrawString( SCREEN_WIDTH / 2, 210, s_rebindConfirmQuestion, UI_CENTER|UI_SMALLFONT, text_color_normal );
}

static void Controls_ExitConfirm_MenuEvent( void* ptr, int event )
{
	if ( event != QM_ACTIVATED ) {
		return;
	}

	if ( uis.activemenu != &s_controlsExitConfirm.menu ) {
		return;
	}

	switch ( ((menucommon_s*)ptr)->id ) {
		case ID_EXITCONFIRM_SAVE:
			RallyControls_SetConfig();
			s_controls.changesmade = qfalse;
			UI_PopMenu();
			UI_PopMenu();
			break;

		case ID_EXITCONFIRM_DISCARD:
			RallyControls_GetConfig();
			s_controls.changesmade = qfalse;
			UI_PopMenu();
			UI_PopMenu();
			break;

		case ID_EXITCONFIRM_CANCEL:
		default:
			UI_PopMenu();
			break;
	}
}

static sfxHandle_t Controls_ExitConfirm_MenuKey( int key )
{
	if ( key == K_ESCAPE || key == K_MOUSE2 || key == K_PAD0_B ) {
		Controls_ExitConfirm_MenuEvent( &s_controlsExitConfirm.cancel, QM_ACTIVATED );
		return menu_out_sound;
	}

	return Menu_DefaultKey( &s_controlsExitConfirm.menu, key );
}

static void Controls_ExitConfirm_Draw( void )
{
	vec4_t compactBoxColor = { 0.0f, 0.0f, 0.0f, 0.50f };

	UI_FillRect( 148, 176, 344, 156, compactBoxColor );
	UI_DrawString( SCREEN_WIDTH / 2, 202, "Save changes?", UI_CENTER|UI_SMALLFONT, text_color_normal );

	Menu_Draw( &s_controlsExitConfirm.menu );
}

static void Controls_ExitConfirmMenu( void )
{
	memset( &s_controlsExitConfirm, 0, sizeof( s_controlsExitConfirm ) );

	s_controlsExitConfirm.menu.draw = Controls_ExitConfirm_Draw;
	s_controlsExitConfirm.menu.key = Controls_ExitConfirm_MenuKey;
	s_controlsExitConfirm.menu.wrapAround = qtrue;
	s_controlsExitConfirm.menu.fullscreen = qtrue;
	s_controlsExitConfirm.menu.transparent = qtrue;

	s_controlsExitConfirm.save.generic.type = MTYPE_PTEXT;
	s_controlsExitConfirm.save.generic.flags = QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_controlsExitConfirm.save.generic.id = ID_EXITCONFIRM_SAVE;
	s_controlsExitConfirm.save.generic.x = SCREEN_WIDTH / 2;
	s_controlsExitConfirm.save.generic.y = 252;
	s_controlsExitConfirm.save.generic.callback = Controls_ExitConfirm_MenuEvent;
	s_controlsExitConfirm.save.string = "Save";
	s_controlsExitConfirm.save.style = UI_CENTER|UI_SMALLFONT;
	s_controlsExitConfirm.save.color = text_color_normal;

	s_controlsExitConfirm.discard.generic.type = MTYPE_PTEXT;
	s_controlsExitConfirm.discard.generic.flags = QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_controlsExitConfirm.discard.generic.id = ID_EXITCONFIRM_DISCARD;
	s_controlsExitConfirm.discard.generic.x = SCREEN_WIDTH / 2;
	s_controlsExitConfirm.discard.generic.y = 272;
	s_controlsExitConfirm.discard.generic.callback = Controls_ExitConfirm_MenuEvent;
	s_controlsExitConfirm.discard.string = "Discard";
	s_controlsExitConfirm.discard.style = UI_CENTER|UI_SMALLFONT;
	s_controlsExitConfirm.discard.color = text_color_normal;

	s_controlsExitConfirm.cancel.generic.type = MTYPE_PTEXT;
	s_controlsExitConfirm.cancel.generic.flags = QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	s_controlsExitConfirm.cancel.generic.id = ID_EXITCONFIRM_CANCEL;
	s_controlsExitConfirm.cancel.generic.x = SCREEN_WIDTH / 2;
	s_controlsExitConfirm.cancel.generic.y = 292;
	s_controlsExitConfirm.cancel.generic.callback = Controls_ExitConfirm_MenuEvent;
	s_controlsExitConfirm.cancel.string = "Cancel";
	s_controlsExitConfirm.cancel.style = UI_CENTER|UI_SMALLFONT;
	s_controlsExitConfirm.cancel.color = text_color_normal;

	Menu_AddItem( &s_controlsExitConfirm.menu, &s_controlsExitConfirm.save );
	Menu_AddItem( &s_controlsExitConfirm.menu, &s_controlsExitConfirm.discard );
	Menu_AddItem( &s_controlsExitConfirm.menu, &s_controlsExitConfirm.cancel );

	UI_PushMenu( &s_controlsExitConfirm.menu );
	Menu_SetCursorToItem( &s_controlsExitConfirm.menu, &s_controlsExitConfirm.cancel );
}

/*
=================
Controls_MenuKey
=================
*/
static sfxHandle_t Controls_MenuKey( int key )
{
	int			id;
	int			ch;
	qboolean	found;
	bind_t*		bindptr;
	found = qfalse;

	if ( !s_controls.waitingforkey && Controls_SearchFieldHasFocus() && ( key & K_CHAR_FLAG ) ) {
		ch = key & ~K_CHAR_FLAG;
		if ( ch == 8 ) {
			int len = strlen( s_controlsSearchText );
			if ( len > 0 ) {
				s_controlsSearchText[len - 1] = '\0';
				Controls_SearchFieldSyncFromState();
				Controls_Update();
				return menu_move_sound;
			}
		} else if ( ch >= 32 && ch < 127 ) {
			int len = strlen( s_controlsSearchText );
			if ( len < (int)sizeof( s_controlsSearchText ) - 1 ) {
				s_controlsSearchText[len] = (char)ch;
				s_controlsSearchText[len + 1] = '\0';
				Controls_SearchFieldSyncFromState();
				Controls_Update();
				return menu_move_sound;
			}
		}
	}

	if (!s_controls.waitingforkey)
	{
			switch (key)
			{
			case K_BACKSPACE:
			case K_DEL:
			case K_KP_DEL:
				key = -1;
				break;
		
				case K_MOUSE2:
				case K_ESCAPE:
				case K_PAD0_B:
					if (s_controls.changesmade) {
						Controls_ExitConfirmMenu();
						return menu_move_sound;
					}
					goto ignorekey;

			default:
				goto ignorekey;
		}
	}
	else
		{
			if (key & K_CHAR_FLAG)
				return 0;

			switch (key)
			{
				case K_ESCAPE:
				case K_PAD0_B:
					s_controls.waitingforkey = qfalse;
					Controls_Update();
					return (menu_out_sound);

				case '`':
					goto ignorekey;
			}
		}

	id      = ((menucommon_s*)(s_controls.menu.items[s_controls.menu.cursor]))->id;

	if ( key != -1 )
	{
		bindptr = Controls_FindConflictingBinding( key, id );
		if ( bindptr ) {
			s_rebindConfirmTargetId = id;
			s_rebindConfirmKey = key;
			Com_sprintf( s_rebindConfirmQuestion, sizeof( s_rebindConfirmQuestion ), "Key is already bound to %s. Replace?", bindptr->label );
			UI_ConfirmMenu_Style( "", UI_CENTER|UI_SMALLFONT, Controls_RebindConflict_Draw, Controls_RebindConflict_Action );
			return menu_move_sound;
		}
	}

	found = Controls_ApplyBindingChange( id, key );

	if (found)
	{	
		return (menu_out_sound);
	}

ignorekey:
	return Menu_DefaultKey( &s_controls.menu, key );
}

/*
=================
Controls_ResetDefaults_Action
=================
*/
static void Controls_ResetDefaults_Action( qboolean result ) {
	if( !result ) {
		return;
	}

	s_controls.changesmade = qtrue;
        RallyControls_SetDefaults();
	Controls_Update();
}

/*
=================
Controls_ResetDefaults_Draw
=================
*/
static void Controls_ResetDefaults_Draw( void ) {
	UI_DrawProportionalString( SCREEN_WIDTH/2, 356 + PROP_HEIGHT * 0, "WARNING: This will reset all", UI_CENTER|UI_SMALLFONT, color_yellow );
	UI_DrawProportionalString( SCREEN_WIDTH/2, 356 + PROP_HEIGHT * 1, "controls to their default values.", UI_CENTER|UI_SMALLFONT, color_yellow );
}

/*
=================
Controls_MenuEvent
=================
*/
static void Controls_MenuEvent( void* ptr, int event )
{
	switch (((menucommon_s*)ptr)->id)
	{
		case ID_MOVEMENT:
			if (event == QM_ACTIVATED)
			{
				s_controls.section = C_MOVEMENT; 
				Controls_Update();
			}
			break;

		case ID_LOOKING:
			if (event == QM_ACTIVATED)
			{
				s_controls.section = C_LOOKING; 
				Controls_Update();
			}
			break;

		case ID_COMBAT:
			if (event == QM_ACTIVATED)
			{
				s_controls.section = C_COMBAT;
				Controls_Update();
			}
			break;

		case ID_WEAPONS:
			if (event == QM_ACTIVATED)
			{
				s_controls.section = C_WEAPONS; 
				Controls_Update();
			}
			break;

		case ID_MISC:
			if (event == QM_ACTIVATED)
			{
				s_controls.section = C_MISC; 
				Controls_Update();
			}
			break;

		case ID_DEVELOPER:
			if (event == QM_ACTIVATED && Controls_ShowDeveloper())
			{
				s_controls.section = C_DEVELOPER;
				Controls_Update();
			}
			break;

		case ID_DEFAULTS:
			if (event == QM_ACTIVATED)
			{
				UI_ConfirmMenu( "SET TO DEFAULTS?", Controls_ResetDefaults_Draw, Controls_ResetDefaults_Action );
			}
			break;

		case ID_BACK:
			if (event == QM_ACTIVATED)
			{
				if (s_controls.changesmade) {
					Controls_ExitConfirmMenu();
				} else {
					UI_PopMenu();
				}
			}
			break;

		case ID_SAVEANDEXIT:
			if (event == QM_ACTIVATED)
			{
                                RallyControls_SetConfig();
				UI_PopMenu();
			}
			break;

		case ID_EXIT:
			if (event == QM_ACTIVATED)
			{
				UI_PopMenu();
			}
			break;

		case ID_FREELOOK:
		case ID_MOUSESPEED:
		case ID_INVERTMOUSE:
		case ID_SMOOTHMOUSE:
		case ID_ALWAYSRUN:
		case ID_AUTOSWITCH:
		case ID_JOYENABLE:
		case ID_JOYANALOG:
		case ID_JOYTHRESHOLD:
// STONELANCE
		case ID_AUTODROP:
// END
			if (event == QM_ACTIVATED)
			{
				s_controls.changesmade = qtrue;
			}
			break;		

		case ID_INPUTMODE:
			if ( event == QM_ACTIVATED )
			{
				Controls_ApplyInputModeProfile( s_controls.inputmode.curvalue );
				Controls_Update();
			}
			break;
	}
}

/*
=================
Controls_ActionEvent
=================
*/
static void Controls_ActionEvent( void* ptr, int event )
{
	if (event == QM_LOSTFOCUS)
	{
		Controls_UpdateModel( ANIM_IDLE );
	}
	else if (event == QM_GOTFOCUS)
	{
		bind_t* binding = Controls_FindBindingById( ((menucommon_s*)ptr)->id );
		Controls_UpdateModel( binding ? binding->anim : ANIM_IDLE );
	}
	else if ((event == QM_ACTIVATED) && !s_controls.waitingforkey)
	{
		s_controls.waitingforkey = 1;
		Controls_Update();
	}
}

/*
=================
Controls_InitModel
=================
*/
static void Controls_InitModel( void )
{
// STONELANCE
	char	model[MAX_QPATH];
	char	rim[MAX_QPATH];
	char	head[MAX_QPATH];
	char	plate[MAX_QPATH];
// END

	memset( &s_controls.playerinfo, 0, sizeof(playerInfo_t) );

// STONELANCE
//	UI_PlayerInfo_SetModel( &s_controls.playerinfo, UI_Cvar_VariableString( "model" ) );
	trap_Cvar_VariableStringBuffer( "model", model, sizeof( model ) );
	trap_Cvar_VariableStringBuffer( "rim", rim, sizeof( rim ) );
	trap_Cvar_VariableStringBuffer( "head", head, sizeof( head ) );
	trap_Cvar_VariableStringBuffer( "plate", plate, sizeof( plate ) );
	UI_PlayerInfo_SetModel( &s_controls.playerinfo, model, rim, head, plate );
// END

	Controls_UpdateModel( ANIM_IDLE );
}

/*
=================
Controls_InitWeapons
=================
*/
static void Controls_InitWeapons( void ) {
	gitem_t *	item;

	for ( item = bg_itemlist + 1 ; item->classname ; item++ ) {
		if ( item->giType != IT_WEAPON ) {
			continue;
		}
		trap_R_RegisterModel( item->world_model[0] );
	}
}

/*
=================
Controls_MenuInit
=================
*/
static void Controls_MenuInit( void )
{
// STONELANCE
	int			x;
// END

	// zero set all our globals
	memset( &s_controls, 0 ,sizeof(controls_t) );
	memset( s_controlsProfileKeys, 0, sizeof( s_controlsProfileKeys ) );
	memset( s_controlsProfileLoaded, 0, sizeof( s_controlsProfileLoaded ) );
	memset( s_controlsProfileJoyEnable, 0, sizeof( s_controlsProfileJoyEnable ) );
	memset( s_controlsProfileJoyAnalog, 0, sizeof( s_controlsProfileJoyAnalog ) );
	memset( s_controlsProfileJoyThreshold, 0, sizeof( s_controlsProfileJoyThreshold ) );
	s_controlsActiveProfileMode = CONTROLS_INPUT_KEYBOARD;
	s_controlsSearchText[0] = '\0';
	s_globalSearchControlCount = 0;
	Controls_SearchFieldSyncFromState();

	Controls_Cache();

	s_controls.menu.key        = Controls_MenuKey;
	s_controls.menu.wrapAround = qtrue;
	s_controls.menu.fullscreen = qtrue;

	s_controls.banner.generic.type	= MTYPE_BTEXT;
	s_controls.banner.generic.flags	= QMF_CENTER_JUSTIFY;
	s_controls.banner.generic.x		= 320;
	s_controls.banner.generic.y		= 16;
	s_controls.banner.string		= "CONTROLS";
	s_controls.banner.color			= color_white;
	s_controls.banner.style			= UI_CENTER;

// STONELANCE
/*
	s_controls.framel.generic.type  = MTYPE_BITMAP;
	s_controls.framel.generic.name  = ART_FRAMEL;
	s_controls.framel.generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_controls.framel.generic.x     = 0;
	s_controls.framel.generic.y     = 78;
	s_controls.framel.width  	    = 256;
	s_controls.framel.height  	    = 329;

	s_controls.framer.generic.type  = MTYPE_BITMAP;
	s_controls.framer.generic.name  = ART_FRAMER;
	s_controls.framer.generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_controls.framer.generic.x     = 376;
	s_controls.framer.generic.y     = 76;
	s_controls.framer.width  	    = 256;
	s_controls.framer.height  	    = 334;
*/

	x = 132;
// END

	s_controls.looking.generic.type     = MTYPE_PTEXT;
	s_controls.looking.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_controls.looking.generic.id	    = ID_LOOKING;
	s_controls.looking.generic.callback	= Controls_MenuEvent;
// STONELANCE
//	s_controls.looking.generic.x	    = 152;
	s_controls.looking.generic.x	    = x;
// END
	s_controls.looking.generic.y	    = 240 - 2 * PROP_HEIGHT;
	s_controls.looking.string			= "VIEW";
	s_controls.looking.style			= UI_RIGHT;
// STONELANCE
//	s_controls.looking.color			= color_red;
	s_controls.looking.color			= text_color_normal;
// END

	s_controls.movement.generic.type     = MTYPE_PTEXT;
	s_controls.movement.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_controls.movement.generic.id	     = ID_MOVEMENT;
	s_controls.movement.generic.callback = Controls_MenuEvent;
// STONELANCE
//	s_controls.movement.generic.x	    = 152;
	s_controls.movement.generic.x	    = x;
// END
	s_controls.movement.generic.y	     = 240 - 3 * PROP_HEIGHT;
	s_controls.movement.string			= "DRIVE";
	s_controls.movement.style			= UI_RIGHT;
// STONELANCE
//	s_controls.movement.color			= color_red;
	s_controls.movement.color			= text_color_normal;
// END

	s_controls.combat.generic.type	    = MTYPE_PTEXT;
	s_controls.combat.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_controls.combat.generic.id	    = ID_COMBAT;
	s_controls.combat.generic.callback	= Controls_MenuEvent;
// STONELANCE
//	s_controls.combat.generic.x	    = 152;
	s_controls.combat.generic.x	    = x;
// END
	s_controls.combat.generic.y	    = 240 - PROP_HEIGHT;
	s_controls.combat.string			= "COMBAT";
	s_controls.combat.style			= UI_RIGHT;
// STONELANCE
//	s_controls.combat.color			= color_red;
	s_controls.combat.color			= text_color_normal;
// END

	s_controls.weapons.generic.type	    = MTYPE_PTEXT;
	s_controls.weapons.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_controls.weapons.generic.id	    = ID_WEAPONS;
	s_controls.weapons.generic.callback	= Controls_MenuEvent;
// STONELANCE
//	s_controls.weapons.generic.x	    = 152;
	s_controls.weapons.generic.x	    = x;
// END
	s_controls.weapons.generic.y	    = 240;
	s_controls.weapons.string			= "WEAPONS";
	s_controls.weapons.style			= UI_RIGHT;
// STONELANCE
//	s_controls.weapons.color			= color_red;
	s_controls.weapons.color			= text_color_normal;
// END

	s_controls.misc.generic.type	 = MTYPE_PTEXT;
	s_controls.misc.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_controls.misc.generic.id	     = ID_MISC;
	s_controls.misc.generic.callback = Controls_MenuEvent;
// STONELANCE
//	s_controls.misc.generic.x	    = 152;
	s_controls.misc.generic.x	    = x;
// END
	s_controls.misc.generic.y		 = 240 + PROP_HEIGHT;
	s_controls.misc.string			= "SYSTEM";
	s_controls.misc.style			= UI_RIGHT;
// STONELANCE
//	s_controls.misc.color			= color_red;
	s_controls.misc.color			= text_color_normal;
// END

	s_controls.developer.generic.type	 = MTYPE_PTEXT;
	s_controls.developer.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_controls.developer.generic.id	     = ID_DEVELOPER;
	s_controls.developer.generic.callback = Controls_MenuEvent;
	s_controls.developer.generic.x	    = x;
	s_controls.developer.generic.y		 = 240 + 2 * PROP_HEIGHT;
	s_controls.developer.string			= "DEVELOPER";
	s_controls.developer.style			= UI_RIGHT;
	s_controls.developer.color			= text_color_normal;

// STONELANCE
/*
	s_controls.back.generic.type	 = MTYPE_BITMAP;
	s_controls.back.generic.name     = ART_BACK0;
	s_controls.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_controls.back.generic.x		 = 0;
	s_controls.back.generic.y		 = 480-64;
	s_controls.back.generic.id	     = ID_BACK;
	s_controls.back.generic.callback = Controls_MenuEvent;
	s_controls.back.width  		     = 128;
	s_controls.back.height  		 = 64;
	s_controls.back.focuspic         = ART_BACK1;
*/
	s_controls.back.generic.type		= MTYPE_PTEXT;
	s_controls.back.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_controls.back.generic.x			= 20;
	s_controls.back.generic.y			= 480 - 50;
	s_controls.back.generic.id			= ID_BACK;
	s_controls.back.generic.callback	= Controls_MenuEvent; 
	s_controls.back.string				= "< BACK";
	s_controls.back.color				= text_color_normal;
	s_controls.back.style				= UI_LEFT | UI_SMALLFONT;
// END

	s_controls.player.generic.type      = MTYPE_BITMAP;
	s_controls.player.generic.flags     = QMF_INACTIVE;
	s_controls.player.generic.ownerdraw = Controls_DrawPlayer;
	s_controls.player.height            = 56*10;
// STONELANCE
	s_controls.player.generic.x			= 340;
//	s_controls.player.generic.x			= 400;
	s_controls.player.generic.y	        = -80;
//	s_controls.player.generic.y	        = -40;
	s_controls.player.width	            = 34*10;
//	s_controls.player.width	            = 32*10;
// END

// STONELANCE
/*
	s_controls.walkforward.generic.type	     = MTYPE_ACTION;
	s_controls.walkforward.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.walkforward.generic.callback  = Controls_ActionEvent;
	s_controls.walkforward.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.walkforward.generic.id 	     = ID_FORWARD;

	s_controls.backpedal.generic.type	   = MTYPE_ACTION;
	s_controls.backpedal.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.backpedal.generic.callback  = Controls_ActionEvent;
	s_controls.backpedal.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.backpedal.generic.id 	   = ID_BACKPEDAL;

	s_controls.stepleft.generic.type	  = MTYPE_ACTION;
	s_controls.stepleft.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.stepleft.generic.callback  = Controls_ActionEvent;
	s_controls.stepleft.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.stepleft.generic.id 		  = ID_MOVELEFT;

	s_controls.stepright.generic.type	   = MTYPE_ACTION;
	s_controls.stepright.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.stepright.generic.callback  = Controls_ActionEvent;
	s_controls.stepright.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.stepright.generic.id        = ID_MOVERIGHT;
*/

	s_controls.accel.generic.type	     = MTYPE_ACTION;
	s_controls.accel.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.accel.generic.callback  = Controls_ActionEvent;
	s_controls.accel.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.accel.generic.id 	     = ID_ACCEL;

	s_controls.brake.generic.type	   = MTYPE_ACTION;
	s_controls.brake.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.brake.generic.callback  = Controls_ActionEvent;
	s_controls.brake.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.brake.generic.id 	   = ID_BRAKE;

	s_controls.handbrake.generic.type	  = MTYPE_ACTION;
	s_controls.handbrake.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.handbrake.generic.callback  = Controls_ActionEvent;
	s_controls.handbrake.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.handbrake.generic.id 		  = ID_HANDBRAKE;

	s_controls.turbo.generic.type	   = MTYPE_ACTION;
	s_controls.turbo.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.turbo.generic.callback  = Controls_ActionEvent;
	s_controls.turbo.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.turbo.generic.id        = ID_TURBO;

	s_controls.gearup.generic.type	     = MTYPE_ACTION;
	s_controls.gearup.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.gearup.generic.callback  = Controls_ActionEvent;
	s_controls.gearup.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.gearup.generic.id 	     = ID_GEARUP;

	s_controls.geardown.generic.type	   = MTYPE_ACTION;
	s_controls.geardown.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.geardown.generic.callback  = Controls_ActionEvent;
	s_controls.geardown.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.geardown.generic.id 	   = ID_GEARDOWN;
// END

	s_controls.moveup.generic.type	    = MTYPE_ACTION;
	s_controls.moveup.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.moveup.generic.callback  = Controls_ActionEvent;
	s_controls.moveup.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.moveup.generic.id        = ID_MOVEUP;

	s_controls.movedown.generic.type	  = MTYPE_ACTION;
	s_controls.movedown.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.movedown.generic.callback  = Controls_ActionEvent;
	s_controls.movedown.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.movedown.generic.id        = ID_MOVEDOWN;

	s_controls.turnleft.generic.type	  = MTYPE_ACTION;
	s_controls.turnleft.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.turnleft.generic.callback  = Controls_ActionEvent;
	s_controls.turnleft.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.turnleft.generic.id        = ID_LEFT;

	s_controls.turnright.generic.type	   = MTYPE_ACTION;
	s_controls.turnright.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.turnright.generic.callback  = Controls_ActionEvent;
	s_controls.turnright.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.turnright.generic.id        = ID_RIGHT;

// STONELANCE
/*
	s_controls.sidestep.generic.type	  = MTYPE_ACTION;
	s_controls.sidestep.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.sidestep.generic.callback  = Controls_ActionEvent;
	s_controls.sidestep.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.sidestep.generic.id        = ID_STRAFE;

	s_controls.run.generic.type	     = MTYPE_ACTION;
	s_controls.run.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.run.generic.callback  = Controls_ActionEvent;
	s_controls.run.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.run.generic.id        = ID_SPEED;
*/
// END

	s_controls.chainsaw.generic.type	  = MTYPE_ACTION;
	s_controls.chainsaw.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.chainsaw.generic.callback  = Controls_ActionEvent;
	s_controls.chainsaw.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.chainsaw.generic.id        = ID_WEAPON1;

	s_controls.machinegun.generic.type	    = MTYPE_ACTION;
	s_controls.machinegun.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.machinegun.generic.callback  = Controls_ActionEvent;
	s_controls.machinegun.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.machinegun.generic.id        = ID_WEAPON2;

	s_controls.shotgun.generic.type	     = MTYPE_ACTION;
	s_controls.shotgun.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.shotgun.generic.callback  = Controls_ActionEvent;
	s_controls.shotgun.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.shotgun.generic.id        = ID_WEAPON3;

	s_controls.grenadelauncher.generic.type	     = MTYPE_ACTION;
	s_controls.grenadelauncher.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.grenadelauncher.generic.callback  = Controls_ActionEvent;
	s_controls.grenadelauncher.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.grenadelauncher.generic.id        = ID_WEAPON4;

	s_controls.rocketlauncher.generic.type	    = MTYPE_ACTION;
	s_controls.rocketlauncher.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.rocketlauncher.generic.callback  = Controls_ActionEvent;
	s_controls.rocketlauncher.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.rocketlauncher.generic.id        = ID_WEAPON5;

	s_controls.lightning.generic.type	   = MTYPE_ACTION;
	s_controls.lightning.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.lightning.generic.callback  = Controls_ActionEvent;
	s_controls.lightning.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.lightning.generic.id        = ID_WEAPON6;

	s_controls.railgun.generic.type	     = MTYPE_ACTION;
	s_controls.railgun.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.railgun.generic.callback  = Controls_ActionEvent;
	s_controls.railgun.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.railgun.generic.id        = ID_WEAPON7;

	s_controls.plasma.generic.type	    = MTYPE_ACTION;
	s_controls.plasma.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.plasma.generic.callback  = Controls_ActionEvent;
	s_controls.plasma.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.plasma.generic.id        = ID_WEAPON8;

	s_controls.bfg.generic.type	     = MTYPE_ACTION;
	s_controls.bfg.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.bfg.generic.callback  = Controls_ActionEvent;
	s_controls.bfg.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.bfg.generic.id        = ID_WEAPON9;

  s_controls.flamethrower.generic.type      = MTYPE_ACTION; 
  s_controls.flamethrower.generic.flags      = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN; 
  s_controls.flamethrower.generic.callback  = Controls_ActionEvent; 
  s_controls.flamethrower.generic.ownerdraw = Controls_DrawKeyBinding; 
  s_controls.flamethrower.generic.id          = ID_WEAPON10;

	s_controls.attack.generic.type	    = MTYPE_ACTION;
	s_controls.attack.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.attack.generic.callback  = Controls_ActionEvent;
	s_controls.attack.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.attack.generic.id        = ID_ATTACK;

  s_controls.alt_attack.generic.type	    = MTYPE_ACTION;
	s_controls.alt_attack.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.alt_attack.generic.callback  = Controls_ActionEvent;
	s_controls.alt_attack.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.alt_attack.generic.id        = ID_ALT_ATTACK;

// STONELANCE
	s_controls.rearattack.generic.type	    = MTYPE_ACTION;
	s_controls.rearattack.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.rearattack.generic.callback  = Controls_ActionEvent;
	s_controls.rearattack.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.rearattack.generic.id        = ID_REARATTACK;

	s_controls.droprear.generic.type	  = MTYPE_ACTION;
	s_controls.droprear.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.droprear.generic.callback  = Controls_ActionEvent;
	s_controls.droprear.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.droprear.generic.id        = ID_DROP_REAR;

	s_controls.autodroprear.generic.type      = MTYPE_RADIOBUTTON;
	s_controls.autodroprear.generic.flags	  = QMF_SMALLFONT;
	s_controls.autodroprear.generic.x	      = SCREEN_WIDTH/2;
	s_controls.autodroprear.generic.name	  = "autodrop rear weapons";
	s_controls.autodroprear.generic.id        = ID_AUTODROP;
	s_controls.autodroprear.generic.callback  = Controls_MenuEvent;
	s_controls.autodroprear.generic.ownerdraw = Controls_DrawRadioButton;
	s_controls.autodroprear.generic.statusbar = Controls_StatusBar;
// END

	s_controls.prevweapon.generic.type	    = MTYPE_ACTION;
	s_controls.prevweapon.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.prevweapon.generic.callback  = Controls_ActionEvent;
	s_controls.prevweapon.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.prevweapon.generic.id        = ID_WEAPPREV;

	s_controls.nextweapon.generic.type	    = MTYPE_ACTION;
	s_controls.nextweapon.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.nextweapon.generic.callback  = Controls_ActionEvent;
	s_controls.nextweapon.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.nextweapon.generic.id        = ID_WEAPNEXT;

	s_controls.lookup.generic.type	    = MTYPE_ACTION;
	s_controls.lookup.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.lookup.generic.callback  = Controls_ActionEvent;
	s_controls.lookup.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.lookup.generic.id        = ID_LOOKUP;

	s_controls.lookdown.generic.type	  = MTYPE_ACTION;
	s_controls.lookdown.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.lookdown.generic.callback  = Controls_ActionEvent;
	s_controls.lookdown.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.lookdown.generic.id        = ID_LOOKDOWN;

	s_controls.mouselook.generic.type	   = MTYPE_ACTION;
	s_controls.mouselook.generic.flags     = QMF_LEFT_JUSTIFY|QMF_HIGHLIGHT_IF_FOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.mouselook.generic.callback  = Controls_ActionEvent;
	s_controls.mouselook.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.mouselook.generic.id        = ID_MOUSELOOK;

	s_controls.freelook.generic.type		= MTYPE_RADIOBUTTON;
	s_controls.freelook.generic.flags		= QMF_SMALLFONT;
	s_controls.freelook.generic.x			= SCREEN_WIDTH/2;
	s_controls.freelook.generic.name		= "free look";
	s_controls.freelook.generic.id			= ID_FREELOOK;
	s_controls.freelook.generic.callback	= Controls_MenuEvent;
	s_controls.freelook.generic.ownerdraw	= Controls_DrawRadioButton;
	s_controls.freelook.generic.statusbar	= Controls_StatusBar;

	s_controls.centerview.generic.type	    = MTYPE_ACTION;
	s_controls.centerview.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.centerview.generic.callback  = Controls_ActionEvent;
	s_controls.centerview.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.centerview.generic.id        = ID_CENTERVIEW;

	s_controls.zoomview.generic.type	  = MTYPE_ACTION;
	s_controls.zoomview.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.zoomview.generic.callback  = Controls_ActionEvent;
	s_controls.zoomview.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.zoomview.generic.id        = ID_ZOOMVIEW;

	s_controls.useitem.generic.type	     = MTYPE_ACTION;
	s_controls.useitem.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.useitem.generic.callback  = Controls_ActionEvent;
	s_controls.useitem.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.useitem.generic.id        = ID_USEITEM;

        s_controls.dropitem.generic.type      = MTYPE_ACTION;
        s_controls.dropitem.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
        s_controls.dropitem.generic.callback  = Controls_ActionEvent;
        s_controls.dropitem.generic.ownerdraw = Controls_DrawKeyBinding;
        s_controls.dropitem.generic.id        = ID_DROPITEM;

	s_controls.showscores.generic.type	    = MTYPE_ACTION;
	s_controls.showscores.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.showscores.generic.callback  = Controls_ActionEvent;
	s_controls.showscores.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.showscores.generic.id        = ID_SHOWSCORES;

	s_controls.invertmouse.generic.type      = MTYPE_RADIOBUTTON;
	s_controls.invertmouse.generic.flags	 = QMF_SMALLFONT;
	s_controls.invertmouse.generic.x	     = SCREEN_WIDTH/2;
	s_controls.invertmouse.generic.name	     = "invert mouse";
	s_controls.invertmouse.generic.id        = ID_INVERTMOUSE;
	s_controls.invertmouse.generic.callback  = Controls_MenuEvent;
	s_controls.invertmouse.generic.ownerdraw = Controls_DrawRadioButton;
	s_controls.invertmouse.generic.statusbar = Controls_StatusBar;

	s_controls.smoothmouse.generic.type      = MTYPE_RADIOBUTTON;
	s_controls.smoothmouse.generic.flags	 = QMF_SMALLFONT;
	s_controls.smoothmouse.generic.x	     = SCREEN_WIDTH/2;
	s_controls.smoothmouse.generic.name	     = "smooth mouse";
	s_controls.smoothmouse.generic.id        = ID_SMOOTHMOUSE;
	s_controls.smoothmouse.generic.callback  = Controls_MenuEvent;
	s_controls.smoothmouse.generic.ownerdraw = Controls_DrawRadioButton;
	s_controls.smoothmouse.generic.statusbar = Controls_StatusBar;

// STONELANCE
/*
	s_controls.alwaysrun.generic.type      = MTYPE_RADIOBUTTON;
	s_controls.alwaysrun.generic.flags	   = QMF_SMALLFONT;
	s_controls.alwaysrun.generic.x	       = SCREEN_WIDTH/2;
	s_controls.alwaysrun.generic.name	   = "always run";
	s_controls.alwaysrun.generic.id        = ID_ALWAYSRUN;
	s_controls.alwaysrun.generic.callback  = Controls_MenuEvent;
	s_controls.alwaysrun.generic.statusbar = Controls_StatusBar;
*/
// END

	s_controls.autoswitch.generic.type      = MTYPE_RADIOBUTTON;
	s_controls.autoswitch.generic.flags	    = QMF_SMALLFONT;
	s_controls.autoswitch.generic.x	        = SCREEN_WIDTH/2;
	s_controls.autoswitch.generic.name	    = "autoswitch weapons";
	s_controls.autoswitch.generic.id        = ID_AUTOSWITCH;
	s_controls.autoswitch.generic.callback  = Controls_MenuEvent;
	s_controls.autoswitch.generic.ownerdraw = Controls_DrawRadioButton;
	s_controls.autoswitch.generic.statusbar = Controls_StatusBar;

	s_controls.sensitivity.generic.type	     = MTYPE_SLIDER;
	s_controls.sensitivity.generic.x		 = SCREEN_WIDTH/2;
	s_controls.sensitivity.generic.flags	 = QMF_SMALLFONT;
	s_controls.sensitivity.generic.name	     = "mouse speed";
	s_controls.sensitivity.generic.id 	     = ID_MOUSESPEED;
	s_controls.sensitivity.generic.callback  = Controls_MenuEvent;
	s_controls.sensitivity.generic.ownerdraw = Controls_DrawSlider;
	s_controls.sensitivity.minvalue		     = 2;
	s_controls.sensitivity.maxvalue		     = 30;
	s_controls.sensitivity.generic.statusbar = Controls_StatusBar;

// STONELANCE
/*
	s_controls.gesture.generic.type	     = MTYPE_ACTION;
	s_controls.gesture.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.gesture.generic.callback  = Controls_ActionEvent;
	s_controls.gesture.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.gesture.generic.id        = ID_GESTURE;
*/
// END

	s_controls.chat.generic.type	  = MTYPE_ACTION;
	s_controls.chat.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.chat.generic.callback  = Controls_ActionEvent;
	s_controls.chat.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.chat.generic.id        = ID_CHAT;

	s_controls.chat2.generic.type	   = MTYPE_ACTION;
	s_controls.chat2.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.chat2.generic.callback  = Controls_ActionEvent;
	s_controls.chat2.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.chat2.generic.id        = ID_CHAT2;

	s_controls.chat3.generic.type	   = MTYPE_ACTION;
	s_controls.chat3.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.chat3.generic.callback  = Controls_ActionEvent;
	s_controls.chat3.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.chat3.generic.id        = ID_CHAT3;

	s_controls.chat4.generic.type	   = MTYPE_ACTION;
	s_controls.chat4.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.chat4.generic.callback  = Controls_ActionEvent;
	s_controls.chat4.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.chat4.generic.id        = ID_CHAT4;

// STONELANCE
	s_controls.headlight.generic.type	   = MTYPE_ACTION;
	s_controls.headlight.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.headlight.generic.callback  = Controls_ActionEvent;
	s_controls.headlight.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.headlight.generic.id        = ID_HEADLIGHT;

	s_controls.horn.generic.type	  = MTYPE_ACTION;
	s_controls.horn.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.horn.generic.callback  = Controls_ActionEvent;
	s_controls.horn.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.horn.generic.id        = ID_HORN;

	s_controls.nextcamera.generic.type       = MTYPE_ACTION;
	s_controls.nextcamera.generic.flags      = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.nextcamera.generic.callback   = Controls_ActionEvent;
	s_controls.nextcamera.generic.ownerdraw  = Controls_DrawKeyBinding;
	s_controls.nextcamera.generic.id         = ID_NEXTCAMERA;

	s_controls.jukeboxPlay.generic.type      = MTYPE_ACTION;
	s_controls.jukeboxPlay.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.jukeboxPlay.generic.callback  = Controls_ActionEvent;
	s_controls.jukeboxPlay.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.jukeboxPlay.generic.id        = ID_JUKEBOX_PLAY;

	s_controls.jukeboxNext.generic.type      = MTYPE_ACTION;
	s_controls.jukeboxNext.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.jukeboxNext.generic.callback  = Controls_ActionEvent;
	s_controls.jukeboxNext.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.jukeboxNext.generic.id        = ID_JUKEBOX_NEXT;

	s_controls.jukeboxPrev.generic.type      = MTYPE_ACTION;
	s_controls.jukeboxPrev.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.jukeboxPrev.generic.callback  = Controls_ActionEvent;
	s_controls.jukeboxPrev.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.jukeboxPrev.generic.id        = ID_JUKEBOX_PREV;

	s_controls.jukeboxRescan.generic.type      = MTYPE_ACTION;
	s_controls.jukeboxRescan.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.jukeboxRescan.generic.callback  = Controls_ActionEvent;
	s_controls.jukeboxRescan.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.jukeboxRescan.generic.id        = ID_JUKEBOX_RESCAN;

	s_controls.jukeboxShuffle.generic.type      = MTYPE_ACTION;
	s_controls.jukeboxShuffle.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.jukeboxShuffle.generic.callback  = Controls_ActionEvent;
	s_controls.jukeboxShuffle.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.jukeboxShuffle.generic.id        = ID_JUKEBOX_SHUFFLE;

	s_controls.jukeboxRepeat.generic.type      = MTYPE_ACTION;
	s_controls.jukeboxRepeat.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.jukeboxRepeat.generic.callback  = Controls_ActionEvent;
	s_controls.jukeboxRepeat.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.jukeboxRepeat.generic.id        = ID_JUKEBOX_REPEAT;

    s_controls.startdemo.generic.type      = MTYPE_ACTION;
    s_controls.startdemo.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
    s_controls.startdemo.generic.callback  = Controls_ActionEvent;
    s_controls.startdemo.generic.ownerdraw = Controls_DrawKeyBinding;
    s_controls.startdemo.generic.id        = ID_STARTDEMO;
    
    s_controls.stopdemo.generic.type      = MTYPE_ACTION;
    s_controls.stopdemo.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
    s_controls.stopdemo.generic.callback  = Controls_ActionEvent;
    s_controls.stopdemo.generic.ownerdraw = Controls_DrawKeyBinding;
    s_controls.stopdemo.generic.id        = ID_STOPDEMO;

	s_controls.moveBPointXPos.generic.type      = MTYPE_ACTION;
	s_controls.moveBPointXPos.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.moveBPointXPos.generic.callback  = Controls_ActionEvent;
	s_controls.moveBPointXPos.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.moveBPointXPos.generic.id        = ID_MOVE_BPOINT_X_POS;

	s_controls.moveBPointXNeg.generic.type      = MTYPE_ACTION;
	s_controls.moveBPointXNeg.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.moveBPointXNeg.generic.callback  = Controls_ActionEvent;
	s_controls.moveBPointXNeg.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.moveBPointXNeg.generic.id        = ID_MOVE_BPOINT_X_NEG;

	s_controls.moveBPointYPos.generic.type      = MTYPE_ACTION;
	s_controls.moveBPointYPos.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.moveBPointYPos.generic.callback  = Controls_ActionEvent;
	s_controls.moveBPointYPos.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.moveBPointYPos.generic.id        = ID_MOVE_BPOINT_Y_POS;

	s_controls.moveBPointYNeg.generic.type      = MTYPE_ACTION;
	s_controls.moveBPointYNeg.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.moveBPointYNeg.generic.callback  = Controls_ActionEvent;
	s_controls.moveBPointYNeg.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.moveBPointYNeg.generic.id        = ID_MOVE_BPOINT_Y_NEG;

	s_controls.moveBPointZPos.generic.type      = MTYPE_ACTION;
	s_controls.moveBPointZPos.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.moveBPointZPos.generic.callback  = Controls_ActionEvent;
	s_controls.moveBPointZPos.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.moveBPointZPos.generic.id        = ID_MOVE_BPOINT_Z_POS;

	s_controls.moveBPointZNeg.generic.type      = MTYPE_ACTION;
	s_controls.moveBPointZNeg.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.moveBPointZNeg.generic.callback  = Controls_ActionEvent;
	s_controls.moveBPointZNeg.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.moveBPointZNeg.generic.id        = ID_MOVE_BPOINT_Z_NEG;

	s_controls.moveBHandleXPos.generic.type      = MTYPE_ACTION;
	s_controls.moveBHandleXPos.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.moveBHandleXPos.generic.callback  = Controls_ActionEvent;
	s_controls.moveBHandleXPos.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.moveBHandleXPos.generic.id        = ID_MOVE_BHANDLE_X_POS;

	s_controls.moveBHandleXNeg.generic.type      = MTYPE_ACTION;
	s_controls.moveBHandleXNeg.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.moveBHandleXNeg.generic.callback  = Controls_ActionEvent;
	s_controls.moveBHandleXNeg.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.moveBHandleXNeg.generic.id        = ID_MOVE_BHANDLE_X_NEG;

	s_controls.moveBHandleYPos.generic.type      = MTYPE_ACTION;
	s_controls.moveBHandleYPos.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.moveBHandleYPos.generic.callback  = Controls_ActionEvent;
	s_controls.moveBHandleYPos.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.moveBHandleYPos.generic.id        = ID_MOVE_BHANDLE_Y_POS;

	s_controls.moveBHandleYNeg.generic.type      = MTYPE_ACTION;
	s_controls.moveBHandleYNeg.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.moveBHandleYNeg.generic.callback  = Controls_ActionEvent;
	s_controls.moveBHandleYNeg.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.moveBHandleYNeg.generic.id        = ID_MOVE_BHANDLE_Y_NEG;

	s_controls.moveBHandleZPos.generic.type      = MTYPE_ACTION;
	s_controls.moveBHandleZPos.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.moveBHandleZPos.generic.callback  = Controls_ActionEvent;
	s_controls.moveBHandleZPos.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.moveBHandleZPos.generic.id        = ID_MOVE_BHANDLE_Z_POS;

	s_controls.moveBHandleZNeg.generic.type      = MTYPE_ACTION;
	s_controls.moveBHandleZNeg.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.moveBHandleZNeg.generic.callback  = Controls_ActionEvent;
	s_controls.moveBHandleZNeg.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.moveBHandleZNeg.generic.id        = ID_MOVE_BHANDLE_Z_NEG;

	s_controls.prevBPoint.generic.type      = MTYPE_ACTION;
	s_controls.prevBPoint.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.prevBPoint.generic.callback  = Controls_ActionEvent;
	s_controls.prevBPoint.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.prevBPoint.generic.id        = ID_PREV_BPOINT;

	s_controls.nextBPoint.generic.type      = MTYPE_ACTION;
	s_controls.nextBPoint.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.nextBPoint.generic.callback  = Controls_ActionEvent;
	s_controls.nextBPoint.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.nextBPoint.generic.id        = ID_NEXT_BPOINT;

	s_controls.toggleBotPaths.generic.type      = MTYPE_ACTION;
	s_controls.toggleBotPaths.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.toggleBotPaths.generic.callback  = Controls_ActionEvent;
	s_controls.toggleBotPaths.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.toggleBotPaths.generic.id        = ID_TOGGLE_BOT_PATHS;

	s_controls.saveBPoints.generic.type      = MTYPE_ACTION;
	s_controls.saveBPoints.generic.flags     = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_GRAYED|QMF_HIDDEN;
	s_controls.saveBPoints.generic.callback  = Controls_ActionEvent;
	s_controls.saveBPoints.generic.ownerdraw = Controls_DrawKeyBinding;
	s_controls.saveBPoints.generic.id        = ID_SAVE_BPOINTS;
    
// END

	s_controls.inputmode.generic.type      = MTYPE_SPINCONTROL;
	s_controls.inputmode.generic.flags	   = QMF_SMALLFONT;
	s_controls.inputmode.generic.x	       = SCREEN_WIDTH/2;
	s_controls.inputmode.generic.y	       = 240 - 4 * PROP_HEIGHT;
	s_controls.inputmode.generic.name	   = "input mode";
	s_controls.inputmode.generic.id        = ID_INPUTMODE;
	s_controls.inputmode.generic.callback  = Controls_MenuEvent;
	s_controls.inputmode.generic.statusbar = Controls_StatusBar;
	s_controls.inputmode.itemnames         = s_controlsInputModes;

	s_controls.joyenable.generic.type      = MTYPE_RADIOBUTTON;
	s_controls.joyenable.generic.flags	   = QMF_SMALLFONT;
	s_controls.joyenable.generic.x	       = SCREEN_WIDTH/2;
	s_controls.joyenable.generic.name	   = "joystick";
	s_controls.joyenable.generic.id        = ID_JOYENABLE;
	s_controls.joyenable.generic.callback  = Controls_MenuEvent;
	s_controls.joyenable.generic.ownerdraw = Controls_DrawRadioButton;
	s_controls.joyenable.generic.statusbar = Controls_StatusBar;

	s_controls.joyanalog.generic.type      = MTYPE_RADIOBUTTON;
	s_controls.joyanalog.generic.flags	   = QMF_SMALLFONT;
	s_controls.joyanalog.generic.x	       = SCREEN_WIDTH/2;
	s_controls.joyanalog.generic.name	   = "analog input";
	s_controls.joyanalog.generic.id        = ID_JOYANALOG;
	s_controls.joyanalog.generic.callback  = Controls_MenuEvent;
	s_controls.joyanalog.generic.ownerdraw = Controls_DrawRadioButton;
	s_controls.joyanalog.generic.statusbar = Controls_StatusBar;

	s_controls.joythreshold.generic.type	  = MTYPE_SLIDER;
	s_controls.joythreshold.generic.x		  = SCREEN_WIDTH/2;
	s_controls.joythreshold.generic.flags	  = QMF_SMALLFONT;
	s_controls.joythreshold.generic.name	  = "joystick threshold";
	s_controls.joythreshold.generic.id 	      = ID_JOYTHRESHOLD;
	s_controls.joythreshold.generic.callback  = Controls_MenuEvent;
	s_controls.joythreshold.generic.ownerdraw = Controls_DrawSlider;
	s_controls.joythreshold.minvalue		  = 0.05;
	s_controls.joythreshold.maxvalue		  = 0.75;
	s_controls.joythreshold.generic.statusbar = Controls_StatusBar;

	{
		s_controls.searchLabel.generic.type		= MTYPE_PTEXT;
		s_controls.searchLabel.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_INACTIVE;
		s_controls.searchLabel.generic.x			= x;
		s_controls.searchLabel.generic.y			= 240 - 5 * PROP_HEIGHT;
		s_controls.searchLabel.string				= "SEARCH";
		s_controls.searchLabel.style				= UI_RIGHT;
		s_controls.searchLabel.color				= text_color_normal;

		s_controls.search.generic.type			= MTYPE_FIELD;
		s_controls.search.generic.flags			= QMF_SMALLFONT;
		s_controls.search.generic.x				= x + 24;
		s_controls.search.generic.y				= s_controls.searchLabel.generic.y + 5;
		s_controls.search.generic.ownerdraw	= Controls_DrawSearchField;
		s_controls.search.field.widthInChars	= 24;
		s_controls.search.field.maxchars		= sizeof( s_controlsSearchText ) - 1;
		Controls_SearchFieldSyncFromState();
	}


	Menu_AddItem( &s_controls.menu, &s_controls.banner );
// STONELANCE
//	Menu_AddItem( &s_controls.menu, &s_controls.framel );
//	Menu_AddItem( &s_controls.menu, &s_controls.framer );
// END
	Menu_AddItem( &s_controls.menu, &s_controls.player );

	Menu_AddItem( &s_controls.menu, &s_controls.movement );
	Menu_AddItem( &s_controls.menu, &s_controls.looking );
	Menu_AddItem( &s_controls.menu, &s_controls.combat );
	Menu_AddItem( &s_controls.menu, &s_controls.weapons );
	Menu_AddItem( &s_controls.menu, &s_controls.misc );
	Menu_AddItem( &s_controls.menu, &s_controls.developer );
	Menu_AddItem( &s_controls.menu, &s_controls.searchLabel );
	Menu_AddItem( &s_controls.menu, &s_controls.search );
	Menu_AddItem( &s_controls.menu, &s_controls.inputmode );

	Menu_AddItem( &s_controls.menu, &s_controls.sensitivity );
	Menu_AddItem( &s_controls.menu, &s_controls.smoothmouse );
	Menu_AddItem( &s_controls.menu, &s_controls.invertmouse );
	Menu_AddItem( &s_controls.menu, &s_controls.lookup );
	Menu_AddItem( &s_controls.menu, &s_controls.lookdown );
	Menu_AddItem( &s_controls.menu, &s_controls.mouselook );
	Menu_AddItem( &s_controls.menu, &s_controls.freelook );
	Menu_AddItem( &s_controls.menu, &s_controls.centerview );
	Menu_AddItem( &s_controls.menu, &s_controls.zoomview );
	Menu_AddItem( &s_controls.menu, &s_controls.joyenable );
	Menu_AddItem( &s_controls.menu, &s_controls.joyanalog );
	Menu_AddItem( &s_controls.menu, &s_controls.joythreshold );

// STONELANCE
//	Menu_AddItem( &s_controls.menu, &s_controls.alwaysrun );
//	Menu_AddItem( &s_controls.menu, &s_controls.run );
//	Menu_AddItem( &s_controls.menu, &s_controls.walkforward );
//	Menu_AddItem( &s_controls.menu, &s_controls.backpedal );
//	Menu_AddItem( &s_controls.menu, &s_controls.stepleft );
//	Menu_AddItem( &s_controls.menu, &s_controls.stepright );
	Menu_AddItem( &s_controls.menu, &s_controls.accel );
	Menu_AddItem( &s_controls.menu, &s_controls.brake );
	Menu_AddItem( &s_controls.menu, &s_controls.gearup );
	Menu_AddItem( &s_controls.menu, &s_controls.geardown );
	Menu_AddItem( &s_controls.menu, &s_controls.handbrake );
	Menu_AddItem( &s_controls.menu, &s_controls.turbo );
// END
	Menu_AddItem( &s_controls.menu, &s_controls.moveup );
	Menu_AddItem( &s_controls.menu, &s_controls.movedown );
	Menu_AddItem( &s_controls.menu, &s_controls.turnleft );
	Menu_AddItem( &s_controls.menu, &s_controls.turnright );
// STONELANCE
//	Menu_AddItem( &s_controls.menu, &s_controls.sidestep );
// END

	Menu_AddItem( &s_controls.menu, &s_controls.attack );
	Menu_AddItem( &s_controls.menu, &s_controls.alt_attack );
// STONELANCE
	Menu_AddItem( &s_controls.menu, &s_controls.rearattack );
	Menu_AddItem( &s_controls.menu, &s_controls.droprear );
	Menu_AddItem( &s_controls.menu, &s_controls.autodroprear );
// END
	Menu_AddItem( &s_controls.menu, &s_controls.nextweapon );
	Menu_AddItem( &s_controls.menu, &s_controls.prevweapon );
	Menu_AddItem( &s_controls.menu, &s_controls.autoswitch );
	Menu_AddItem( &s_controls.menu, &s_controls.chainsaw );
	Menu_AddItem( &s_controls.menu, &s_controls.machinegun );
	Menu_AddItem( &s_controls.menu, &s_controls.shotgun );
	Menu_AddItem( &s_controls.menu, &s_controls.grenadelauncher );
	Menu_AddItem( &s_controls.menu, &s_controls.rocketlauncher );
	Menu_AddItem( &s_controls.menu, &s_controls.lightning );
	Menu_AddItem( &s_controls.menu, &s_controls.railgun );
	Menu_AddItem( &s_controls.menu, &s_controls.plasma );
	Menu_AddItem( &s_controls.menu, &s_controls.bfg );
	Menu_AddItem( &s_controls.menu, &s_controls.flamethrower );

	Menu_AddItem( &s_controls.menu, &s_controls.showscores );
	Menu_AddItem( &s_controls.menu, &s_controls.useitem );
        Menu_AddItem( &s_controls.menu, &s_controls.dropitem );
	Menu_AddItem( &s_controls.menu, &s_controls.jukeboxPlay );
	Menu_AddItem( &s_controls.menu, &s_controls.jukeboxNext );
	Menu_AddItem( &s_controls.menu, &s_controls.jukeboxPrev );
	Menu_AddItem( &s_controls.menu, &s_controls.jukeboxRescan );
	Menu_AddItem( &s_controls.menu, &s_controls.jukeboxShuffle );
	Menu_AddItem( &s_controls.menu, &s_controls.jukeboxRepeat );
// STONELANCE
//	Menu_AddItem( &s_controls.menu, &s_controls.gesture );
// END
	Menu_AddItem( &s_controls.menu, &s_controls.chat );
	Menu_AddItem( &s_controls.menu, &s_controls.chat2 );
	Menu_AddItem( &s_controls.menu, &s_controls.chat3 );
	Menu_AddItem( &s_controls.menu, &s_controls.chat4 );
// STONELANCE
	Menu_AddItem( &s_controls.menu, &s_controls.headlight );
    Menu_AddItem( &s_controls.menu, &s_controls.startdemo );
    Menu_AddItem( &s_controls.menu, &s_controls.stopdemo );
	Menu_AddItem( &s_controls.menu, &s_controls.nextcamera );
	Menu_AddItem( &s_controls.menu, &s_controls.horn );
	Menu_AddItem( &s_controls.menu, &s_controls.toggleBotPaths );
	Menu_AddItem( &s_controls.menu, &s_controls.moveBPointXPos );
	Menu_AddItem( &s_controls.menu, &s_controls.moveBPointXNeg );
	Menu_AddItem( &s_controls.menu, &s_controls.moveBPointYPos );
	Menu_AddItem( &s_controls.menu, &s_controls.moveBPointYNeg );
	Menu_AddItem( &s_controls.menu, &s_controls.moveBPointZPos );
	Menu_AddItem( &s_controls.menu, &s_controls.moveBPointZNeg );
	Menu_AddItem( &s_controls.menu, &s_controls.moveBHandleXPos );
	Menu_AddItem( &s_controls.menu, &s_controls.moveBHandleXNeg );
	Menu_AddItem( &s_controls.menu, &s_controls.moveBHandleYPos );
	Menu_AddItem( &s_controls.menu, &s_controls.moveBHandleYNeg );
	Menu_AddItem( &s_controls.menu, &s_controls.moveBHandleZPos );
	Menu_AddItem( &s_controls.menu, &s_controls.moveBHandleZNeg );
	Menu_AddItem( &s_controls.menu, &s_controls.prevBPoint );
	Menu_AddItem( &s_controls.menu, &s_controls.nextBPoint );
	Menu_AddItem( &s_controls.menu, &s_controls.saveBPoints );

// END

	Menu_AddItem( &s_controls.menu, &s_controls.back );


	// initialize the configurable cvars
        Controls_InitCvars();

        // initialize the current config
        RallyControls_GetConfig();

	// intialize the model
	Controls_InitModel();

	// intialize the weapons
	Controls_InitWeapons ();

	// initial default section
	s_controls.section = C_MOVEMENT;

	// update the ui
	Controls_Update();
}


/*
=================
Controls_Cache
=================
*/
void Controls_Cache( void ) {
// STONELANCE
/*
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
*/
// END
}


/*
=================
UI_ControlsMenu
=================
*/
void UI_ControlsMenu( void ) {
	Controls_MenuInit();
	UI_PushMenu( &s_controls.menu );
}
