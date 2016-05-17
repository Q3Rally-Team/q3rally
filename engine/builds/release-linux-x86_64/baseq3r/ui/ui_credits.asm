data
export color_background
align 4
LABELV color_background
byte 4 0
byte 4 0
byte 4 0
byte 4 0
export color_headertext
align 4
LABELV color_headertext
byte 4 1065353216
byte 4 1065353216
byte 4 1065353216
byte 4 1065353216
export color_maintext
align 4
LABELV color_maintext
byte 4 1065353216
byte 4 1065353216
byte 4 1065353216
byte 4 1065353216
export credits
align 4
LABELV credits
address $82
byte 4 18465
address color_headertext
address $83
byte 4 2065
address color_blue
address $83
byte 4 2065
address color_blue
address $84
byte 4 18449
address color_headertext
address $83
byte 4 2065
address color_blue
address $85
byte 4 2065
address color_headertext
address $86
byte 4 2065
address color_headertext
address $87
byte 4 2065
address color_headertext
address $88
byte 4 2065
address color_headertext
address $89
byte 4 2065
address color_headertext
address $83
byte 4 2065
address color_blue
address $90
byte 4 18449
address color_headertext
address $83
byte 4 2065
address color_blue
address $91
byte 4 2065
address color_headertext
address $92
byte 4 2065
address color_headertext
address $93
byte 4 2065
address color_headertext
address $94
byte 4 2065
address color_headertext
address $95
byte 4 2065
address color_headertext
address $86
byte 4 2065
address color_headertext
address $96
byte 4 2065
address color_headertext
address $97
byte 4 2065
address color_headertext
address $98
byte 4 2065
address color_headertext
address $99
byte 4 2065
address color_headertext
address $100
byte 4 2065
address color_headertext
address $83
byte 4 2065
address color_blue
address $101
byte 4 18449
address color_headertext
address $83
byte 4 2065
address color_blue
address $91
byte 4 2065
address color_headertext
address $102
byte 4 2065
address color_headertext
address $98
byte 4 2065
address color_headertext
address $83
byte 4 2065
address color_blue
address $103
byte 4 18449
address color_headertext
address $83
byte 4 2065
address color_blue
address $104
byte 4 2065
address color_headertext
address $105
byte 4 2065
address color_headertext
address $106
byte 4 2065
address color_headertext
address $98
byte 4 2065
address color_headertext
address $83
byte 4 2065
address color_blue
address $107
byte 4 18449
address color_headertext
address $83
byte 4 2065
address color_blue
address $108
byte 4 2065
address color_headertext
address $99
byte 4 2065
address color_headertext
address $83
byte 4 2065
address color_blue
address $109
byte 4 18449
address color_headertext
address $83
byte 4 2065
address color_blue
address $110
byte 4 2065
address color_headertext
address $86
byte 4 2065
address color_headertext
address $111
byte 4 2065
address color_headertext
address $83
byte 4 2065
address color_blue
address $112
byte 4 18449
address color_headertext
address $83
byte 4 2065
address color_blue
address $113
byte 4 2065
address color_headertext
address $99
byte 4 2065
address color_headertext
address $86
byte 4 2065
address color_headertext
address $111
byte 4 2065
address color_headertext
address $114
byte 4 2065
address color_headertext
address $115
byte 4 2065
address color_headertext
address $83
byte 4 2065
address color_blue
address $83
byte 4 2065
address color_blue
address $83
byte 4 2065
address color_blue
address $83
byte 4 2065
address color_blue
address $83
byte 4 2065
address color_blue
address $116
byte 4 2081
address color_headertext
address $83
byte 4 2065
address color_blue
address $84
byte 4 2065
address color_headertext
address $117
byte 4 2065
address color_maintext
address $83
byte 4 2065
address color_blue
address $118
byte 4 2065
address color_headertext
address $119
byte 4 2065
address color_maintext
address $120
byte 4 2065
address color_maintext
address $83
byte 4 2065
address color_blue
address $121
byte 4 2065
address color_headertext
address $122
byte 4 2065
address color_maintext
address $83
byte 4 2065
address color_blue
address $123
byte 4 2065
address color_headertext
address $124
byte 4 2065
address color_maintext
address $125
byte 4 2065
address color_maintext
address $83
byte 4 2065
address color_blue
address $126
byte 4 2065
address color_headertext
address $127
byte 4 2065
address color_maintext
address $83
byte 4 2065
address color_blue
address $128
byte 4 2065
address color_headertext
address $129
byte 4 2065
address color_maintext
address $83
byte 4 2065
address color_blue
address $130
byte 4 2065
address color_headertext
address $131
byte 4 2065
address color_maintext
address $83
byte 4 2065
address color_blue
byte 4 0
skip 8
code
proc UI_CreditMenu_Key 4 8
ADDRFP4 0
INDIRI4
CNSTI4 1024
BANDI4
CNSTI4 0
EQI4 $133
CNSTI4 0
RETI4
ADDRGP4 $132
JUMPV
LABELV $133
ADDRGP4 $135
ARGP4
ADDRGP4 mvolume
INDIRF4
ARGF4
ADDRLP4 0
ADDRGP4 va
CALLP4
ASGNP4
CNSTI4 2
ARGI4
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 trap_Cmd_ExecuteText
CALLV
pop
CNSTI4 0
RETI4
LABELV $132
endproc UI_CreditMenu_Key 4 8
proc ScrollingCredits_Draw 24 20
ADDRLP4 12
CNSTI4 320
ASGNI4
ADDRLP4 8
CNSTI4 0
ASGNI4
ADDRLP4 8
INDIRI4
CNSTI4 0
NEI4 $137
ADDRLP4 0
CNSTI4 0
ASGNI4
ADDRGP4 $142
JUMPV
LABELV $139
CNSTI4 12
ADDRLP4 0
INDIRI4
MULI4
ADDRGP4 credits+4
ADDP4
INDIRI4
CNSTI4 16
BANDI4
CNSTI4 0
EQI4 $143
ADDRLP4 8
ADDRLP4 8
INDIRI4
CVIF4 4
CNSTF4 1098907648
ADDF4
CVFI4 4
ASGNI4
ADDRGP4 $144
JUMPV
LABELV $143
CNSTI4 12
ADDRLP4 0
INDIRI4
MULI4
ADDRGP4 credits+4
ADDP4
INDIRI4
CNSTI4 32
BANDI4
CNSTI4 0
EQI4 $146
ADDRLP4 8
ADDRLP4 8
INDIRI4
CNSTI4 32
ADDI4
ASGNI4
ADDRGP4 $147
JUMPV
LABELV $146
CNSTI4 12
ADDRLP4 0
INDIRI4
MULI4
ADDRGP4 credits+4
ADDP4
INDIRI4
CNSTI4 64
BANDI4
CNSTI4 0
EQI4 $149
ADDRLP4 8
ADDRLP4 8
INDIRI4
CVIF4 4
CNSTF4 1115684864
ADDF4
CVFI4 4
ASGNI4
LABELV $149
LABELV $147
LABELV $144
LABELV $140
ADDRLP4 0
ADDRLP4 0
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
LABELV $142
ADDRLP4 0
INDIRI4
CVIU4 4
CNSTU4 1055
LEU4 $139
LABELV $137
ADDRGP4 uis+11476
INDIRF4
NEGF4
ARGF4
CNSTF4 0
ARGF4
CNSTF4 1073741824
ADDRGP4 uis+11476
INDIRF4
MULF4
CNSTF4 1142947840
ADDF4
ARGF4
CNSTF4 1139802112
ARGF4
ADDRGP4 BackgroundShader
INDIRI4
ARGI4
ADDRGP4 UI_DrawHandlePic
CALLV
pop
ADDRLP4 4
CNSTF4 1139802112
CNSTF4 1078774989
ADDRGP4 uis+4
INDIRI4
ADDRGP4 starttime
INDIRI4
SUBI4
CVIF4 4
MULF4
CNSTF4 1120403456
DIVF4
SUBF4
CVFI4 4
ASGNI4
ADDRLP4 0
CNSTI4 0
ASGNI4
ADDRGP4 $158
JUMPV
LABELV $155
CNSTI4 12
ADDRLP4 0
INDIRI4
MULI4
ADDRGP4 credits
ADDP4
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $159
ADDRLP4 4
INDIRI4
CNSTI4 -16
GEI4 $157
ADDRGP4 $135
ARGP4
ADDRGP4 mvolume
INDIRF4
ARGF4
ADDRLP4 16
ADDRGP4 va
CALLP4
ASGNP4
CNSTI4 2
ARGI4
ADDRLP4 16
INDIRP4
ARGP4
ADDRGP4 trap_Cmd_ExecuteText
CALLV
pop
ADDRGP4 $157
JUMPV
LABELV $159
CNSTI4 12
ADDRLP4 0
INDIRI4
MULI4
ADDRGP4 credits
ADDP4
INDIRP4
ARGP4
ADDRLP4 16
ADDRGP4 strlen
CALLU4
ASGNU4
ADDRLP4 16
INDIRU4
CNSTU4 1
NEU4 $163
ADDRGP4 $156
JUMPV
LABELV $163
ADDRLP4 4
INDIRI4
CVIF4 4
CNSTF4 3263168512
LEF4 $165
ADDRLP4 12
INDIRI4
ARGI4
ADDRLP4 4
INDIRI4
ARGI4
ADDRLP4 20
CNSTI4 12
ADDRLP4 0
INDIRI4
MULI4
ASGNI4
ADDRLP4 20
INDIRI4
ADDRGP4 credits
ADDP4
INDIRP4
ARGP4
ADDRLP4 20
INDIRI4
ADDRGP4 credits+4
ADDP4
INDIRI4
ARGI4
ADDRLP4 20
INDIRI4
ADDRGP4 credits+8
ADDP4
INDIRP4
ARGP4
ADDRGP4 UI_DrawProportionalString
CALLV
pop
LABELV $165
CNSTI4 12
ADDRLP4 0
INDIRI4
MULI4
ADDRGP4 credits+4
ADDP4
INDIRI4
CNSTI4 16
BANDI4
CNSTI4 0
EQI4 $169
ADDRLP4 4
ADDRLP4 4
INDIRI4
CVIF4 4
CNSTF4 1098907648
ADDF4
CVFI4 4
ASGNI4
ADDRGP4 $170
JUMPV
LABELV $169
CNSTI4 12
ADDRLP4 0
INDIRI4
MULI4
ADDRGP4 credits+4
ADDP4
INDIRI4
CNSTI4 32
BANDI4
CNSTI4 0
EQI4 $172
ADDRLP4 4
ADDRLP4 4
INDIRI4
CNSTI4 32
ADDI4
ASGNI4
ADDRGP4 $173
JUMPV
LABELV $172
CNSTI4 12
ADDRLP4 0
INDIRI4
MULI4
ADDRGP4 credits+4
ADDP4
INDIRI4
CNSTI4 64
BANDI4
CNSTI4 0
EQI4 $175
ADDRLP4 4
ADDRLP4 4
INDIRI4
CVIF4 4
CNSTF4 1115684864
ADDF4
CVFI4 4
ASGNI4
LABELV $175
LABELV $173
LABELV $170
ADDRLP4 4
INDIRI4
CNSTI4 480
LEI4 $178
ADDRGP4 $157
JUMPV
LABELV $178
LABELV $156
ADDRLP4 0
ADDRLP4 0
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
LABELV $158
ADDRLP4 0
INDIRI4
CVIU4 4
CNSTU4 1055
LEU4 $155
LABELV $157
LABELV $136
endproc ScrollingCredits_Draw 24 20
export UI_CreditMenu
proc UI_CreditMenu 8 12
ADDRGP4 s_credits
ARGP4
CNSTI4 0
ARGI4
CNSTU4 560
ARGU4
ADDRGP4 memset
CALLP4
pop
ADDRGP4 s_credits+524
ADDRGP4 ScrollingCredits_Draw
ASGNP4
ADDRGP4 s_credits+528
ADDRGP4 UI_CreditMenu_Key
ASGNP4
ADDRGP4 s_credits+536
CNSTI4 1
ASGNI4
ADDRGP4 s_credits
ARGP4
ADDRGP4 UI_PushMenu
CALLV
pop
ADDRGP4 starttime
ADDRGP4 uis+4
INDIRI4
ASGNI4
ADDRGP4 $185
ARGP4
ADDRLP4 0
ADDRGP4 trap_Cvar_VariableValue
CALLF4
ASGNF4
ADDRGP4 mvolume
ADDRLP4 0
INDIRF4
ASGNF4
ADDRGP4 mvolume
INDIRF4
CNSTF4 1056964608
GEF4 $186
CNSTI4 2
ARGI4
ADDRGP4 $188
ARGP4
ADDRGP4 trap_Cmd_ExecuteText
CALLV
pop
LABELV $186
CNSTI4 2
ARGI4
ADDRGP4 $189
ARGP4
ADDRGP4 trap_Cmd_ExecuteText
CALLV
pop
ADDRGP4 $190
ARGP4
ADDRLP4 4
ADDRGP4 trap_R_RegisterShaderNoMip
CALLI4
ASGNI4
ADDRGP4 BackgroundShader
ADDRLP4 4
INDIRI4
ASGNI4
LABELV $180
endproc UI_CreditMenu 8 12
bss
export BackgroundShader
align 4
LABELV BackgroundShader
skip 4
export mvolume
align 4
LABELV mvolume
skip 4
export starttime
align 4
LABELV starttime
skip 4
align 4
LABELV s_credits
skip 560
import UI_RankStatusMenu
import RankStatus_Cache
import UI_SignupMenu
import Signup_Cache
import UI_LoginMenu
import Login_Cache
import UI_RankingsMenu
import Rankings_Cache
import Rankings_DrawPassword
import Rankings_DrawName
import Rankings_DrawText
import UI_InitGameinfo
import UI_SPUnlockMedals_f
import UI_SPUnlock_f
import UI_GetAwardLevel
import UI_LogAwardData
import UI_NewGame
import UI_GetCurrentGame
import UI_CanShowTierVideo
import UI_ShowTierVideo
import UI_TierCompleted
import UI_SetBestScore
import UI_GetBestScore
import UI_GetNumBots
import UI_GetBotInfoByName
import UI_GetBotInfoByNumber
import UI_GetNumSPTiers
import UI_GetNumSPArenas
import UI_GetNumArenas
import UI_GetSpecialArenaInfo
import UI_GetArenaInfoByMap
import UI_GetArenaInfoByNumber
import UI_NetworkOptionsMenu
import UI_NetworkOptionsMenu_Cache
import UI_Q3ROptionsMenu
import isRaceObserver
import GetValuesFromFavorite
import UI_BuildFileList
import UI_Random
import UI_SoundOptionsMenu
import UI_SoundOptionsMenu_Cache
import UI_DisplayOptionsMenu
import UI_DisplayOptionsMenu_Cache
import UI_SaveConfigMenu
import UI_SaveConfigMenu_Cache
import UI_LoadConfigMenu
import UI_LoadConfig_Cache
import UI_TeamOrdersMenu_Cache
import UI_TeamOrdersMenu_f
import UI_TeamOrdersMenu
import UI_RemoveBotsMenu
import UI_RemoveBots_Cache
import UI_AddBotsMenu
import UI_AddBots_Cache
import trap_SetPbClStatus
import trap_VerifyCDKey
import trap_SetCDKey
import trap_GetCDKey
import trap_MemoryRemaining
import trap_LAN_GetPingInfo
import trap_LAN_GetPing
import trap_LAN_ClearPing
import trap_LAN_ServerStatus
import trap_LAN_GetPingQueueCount
import trap_LAN_GetServerInfo
import trap_LAN_GetServerAddressString
import trap_LAN_GetServerCount
import trap_GetConfigString
import trap_GetGlconfig
import trap_GetClientState
import trap_GetClipboardData
import trap_Key_SetCatcher
import trap_Key_GetCatcher
import trap_Key_ClearStates
import trap_Key_SetOverstrikeMode
import trap_Key_GetOverstrikeMode
import trap_Key_IsDown
import trap_Key_SetBinding
import trap_Key_GetBindingBuf
import trap_Key_KeynumToStringBuf
import trap_S_RegisterSound
import trap_S_StartLocalSound
import trap_CM_LerpTag
import trap_UpdateScreen
import trap_R_DrawStretchPic
import trap_R_SetColor
import trap_R_RenderScene
import trap_R_AddLightToScene
import trap_R_AddPolyToScene
import trap_R_AddRefEntityToScene
import trap_R_ClearScene
import trap_R_RegisterShaderNoMip
import trap_R_RegisterSkin
import trap_R_RegisterModel
import trap_FS_Seek
import trap_FS_GetFileList
import trap_FS_FCloseFile
import trap_FS_Write
import trap_FS_Read
import trap_FS_FOpenFile
import trap_Cmd_ExecuteText
import trap_Argv
import trap_Argc
import trap_Cvar_InfoStringBuffer
import trap_Cvar_Create
import trap_Cvar_Reset
import trap_Cvar_SetValue
import trap_Cvar_VariableStringBuffer
import trap_Cvar_VariableValue
import trap_Cvar_Set
import trap_Cvar_Update
import trap_Cvar_Register
import trap_Milliseconds
import trap_Error
import trap_Print
import UI_SPSkillMenu_Cache
import UI_SPSkillMenu
import UI_SPPostgameMenu_f
import UI_SPPostgameMenu_Cache
import UI_SPArena_Start
import UI_SPLevelMenu_ReInit
import UI_SPLevelMenu_f
import UI_SPLevelMenu
import UI_SPLevelMenu_Cache
import uis
import m_entersound
import UI_StartDemoLoop
import UI_Cvar_VariableString
import UI_Argv
import UI_ForceMenuOff
import UI_PopMenu
import UI_PushMenu
import UI_SetActiveMenu
import UI_IsFullscreen
import UI_DrawTextBox
import UI_AdjustFrom640
import UI_CursorInRect
import UI_DrawChar
import UI_DrawString
import UI_ProportionalStringWidth
import UI_DrawProportionalString_AutoWrapped
import UI_DrawProportionalString
import UI_ProportionalSizeScale
import UI_DrawBannerString
import UI_LerpColor
import UI_SetColor
import UI_UpdateScreen
import UI_DrawRect
import UI_FillRect
import UI_DrawHandlePic
import UI_DrawNamedPic
import UI_ClampCvar
import UI_ConsoleCommand
import UI_Refresh
import UI_MouseEvent
import UI_KeyEvent
import UI_Shutdown
import UI_Init
import UI_RegisterClientModelname
import UI_PlayerInfo_SetInfo
import UI_PlayerInfo_SetModel
import UI_DrawPlayer
import DriverInfo_Cache
import GraphicsOptions_Cache
import UI_GraphicsOptionsMenu
import ServerInfo_Cache
import UI_ServerInfoMenu
import UI_BotSelectMenu_Cache
import UI_BotSelectMenu
import ServerOptions_Cache
import StartServer_Cache
import UI_StartServerMenu
import ArenaServers_Cache
import UI_ArenaServersMenu
import SpecifyServer_Cache
import UI_SpecifyServerMenu
import SpecifyLeague_Cache
import UI_SpecifyLeagueMenu
import Preferences_Cache
import UI_PreferencesMenu
import UI_PlateSelectionMenu
import PlayerSettings_Update
import PlayerSettings_Cache
import UI_PlayerSettingsMenu
import PlayerModel_Cache
import UI_PlayerModelMenu
import UI_CDKeyMenu_f
import UI_CDKeyMenu_Cache
import UI_CDKeyMenu
import UI_ModsMenu_Cache
import UI_ModsMenu
import UI_CinematicsMenu_Cache
import UI_CinematicsMenu_f
import UI_CinematicsMenu
import Demos_Cache
import UI_DemosMenu
import Controls_Cache
import UI_ControlsMenu
import UI_DrawConnectScreen
import TeamMain_Cache
import UI_TeamMainMenu
import UI_SetupMenu
import UI_SetupMenu_Cache
import UI_Message
import UI_ConfirmMenu_Style
import UI_ConfirmMenu
import ConfirmMenu_Cache
import UI_InGameMenu
import InGame_Cache
import UI_Rally_CreditMenu
import UI_GFX_Loading
import UI_UpdateCvars
import UI_RegisterCvars
import UI_MainMenu
import MainMenu_Cache
import MenuField_Key
import MenuField_Draw
import MenuField_Init
import MField_Draw
import MField_CharEvent
import MField_KeyDownEvent
import MField_Clear
import ui_medalSounds
import ui_medalPicNames
import ui_medalNames
import text_color
import text_color_highlight
import text_color_normal
import text_color_disabled
import listbar_color
import list_color
import name_color
import color_dim
import color_red
import color_orange
import color_blue
import color_yellow
import color_white
import color_black
import menu_back_color
import menu_dim_color
import menu_black_color
import menu_red_color
import menu_highlight_color
import menu_dark_color
import menu_grayed_color
import menu_text_color
import weaponChangeSound
import menu_null_sound
import menu_buzz_sound
import menu_out_sound
import menu_move_sound
import menu_in_sound
import ScrollList_Key
import ScrollList_Draw
import Bitmap_Draw
import Bitmap_Init
import Menu_DefaultKey
import Menu_SetCursorToItem
import Menu_SetCursor
import Menu_ActivateItem
import Menu_ItemAtCursor
import Menu_Draw
import Menu_AdjustCursor
import Menu_AddItem
import Menu_Focus
import Menu_Cache
import ui_mainViewRenderLevel
import ui_rearViewRenderLevel
import ui_tightCamTracking
import ui_engineSounds
import ui_drawPositionSprites
import ui_atmosphericLevel
import ui_checkpointArrowMode
import ui_drawRearView
import ui_minSkidLength
import ui_manualShift
import ui_controlMode
import ui_metricUnits
import ui_trackReversed
import ui_favoritecar4
import ui_favoritecar3
import ui_favoritecar2
import ui_favoritecar1
import ui_ioq3
import ui_cdkeychecked
import ui_cdkey
import ui_server16
import ui_server15
import ui_server14
import ui_server13
import ui_server12
import ui_server11
import ui_server10
import ui_server9
import ui_server8
import ui_server7
import ui_server6
import ui_server5
import ui_server4
import ui_server3
import ui_server2
import ui_server1
import ui_marks
import ui_drawCrosshairNames
import ui_drawCrosshair
import ui_brassTime
import ui_browserOnlyHumans
import ui_browserShowEmpty
import ui_browserShowFull
import ui_browserSortKey
import ui_browserGameType
import ui_browserMaster
import ui_spSelection
import ui_spSkill
import ui_spVideos
import ui_spAwards
import ui_spScores5
import ui_spScores4
import ui_spScores3
import ui_spScores2
import ui_spScores1
import ui_botsFile
import ui_arenasFile
import ui_ctf_friendly
import ui_ctf_timelimit
import ui_ctf_capturelimit
import ui_team_friendly
import ui_team_timelimit
import ui_team_fraglimit
import ui_racing_trackreversed
import ui_racing_tracklength
import ui_dm_timelimit
import ui_dm_fraglimit
import ui_derby_timelimit
import ui_team_racing_friendly
import ui_team_racing_timelimit
import ui_team_racing_laplimit
import ui_racing_timelimit
import ui_racing_laplimit
import BG_PlayerTouchesItem
import BG_PlayerStateToEntityStateExtraPolate
import BG_PlayerStateToEntityState
import BG_TouchJumpPad
import BG_AddPredictableEventToPlayerstate
import BG_EvaluateTrajectoryDelta
import BG_EvaluateTrajectory
import BG_CanItemBeGrabbed
import BG_FindItemForHoldable
import BG_FindItemForPowerup
import BG_FindItemForWeapon
import BG_FindItem
import bg_numItems
import bg_itemlist
import Pmove
import PM_UpdateViewAngles
import PM_SetCoM
import PM_CalculateSecondaryQuantities
import PM_InitializeVehicle
import PM_ApplyForce
import PM_DriveMove
import PM_CalculateNetForce
import PM_AddRoadForces
import CP_SWAYBAR_STRENGTH
import CP_SHOCK_STRENGTH
import CP_SPRING_STRENGTH
import CP_WR_STRENGTH
import CP_M_2_QU
import CP_CURRENT_GRAVITY
import getStringForTimeDuration
import getStringForTime
import Com_Printf
import Com_Error
import Info_NextPair
import Info_Validate
import Info_SetValueForKey_Big
import Info_SetValueForKey
import Info_RemoveKey_Big
import Info_RemoveKey
import Info_ValueForKey
import vectoyaw
import vtos
import AngleDifference
import WheelAngle
import Com_TruncateLongString
import va
import Q_CountChar
import Q_CleanStr
import Q_PrintStrlen
import Q_strcat
import Q_strncpyz
import Q_stristr
import Q_strupr
import Q_strlwr
import Q_stricmpn
import Q_strncmp
import Q_stricmp
import Q_isintegral
import Q_isanumber
import Q_isalpha
import Q_isupper
import Q_islower
import Q_isprint
import Com_RandomBytes
import Com_SkipCharset
import Com_SkipTokens
import Com_sprintf
import Com_HexStrToInt
import Parse3DMatrix
import Parse2DMatrix
import Parse1DMatrix
import SkipRestOfLine
import SkipBracedSection
import COM_MatchToken
import COM_ParseWarning
import COM_ParseError
import COM_Compress
import COM_ParseExt
import COM_Parse
import COM_GetCurrentParseLine
import COM_BeginParseSession
import COM_DefaultExtension
import COM_CompareExtension
import COM_StripExtension
import COM_GetExtension
import COM_SkipPath
import Com_Clamp
import PerpendicularVector
import AngleVectors
import MatrixMultiply
import QuaternionToVectors
import QuaternionL2ToOrientation
import QuaternionToOrientation
import OrientationToQuaternion
import QuaternionToAngles
import AnglesToQuaternion
import QuaternionSLERP
import QuaternionRotate
import QuaternionNormalize
import QuaternionFastNormalize
import QuaternionMultiply
import OrthonormalizeOrientation
import OrientationToVectors
import OrientationToAngles
import AnglesToDeltaAngles
import AnglesToOrientation
import MatrixScale
import MatrixAdd
import MatrixTranspose
import VectorNAN
import MakeNormalVectors
import RotateAroundDirection
import RotatePointAroundVector
import ProjectPointOnPlane
import PlaneFromPoints
import AngleDelta
import AngleNormalize180
import AngleNormalize360
import AnglesSubtract
import AngleSubtract
import LerpAngle
import AngleMod
import BoundsIntersectPoint
import BoundsIntersectSphere
import BoundsIntersect
import BoxOnPlaneSide
import SetPlaneSignbits
import AxisCopy
import AxisClear
import AnglesToAxis
import vectoangles
import Q_crandom
import Q_random
import Q_rand
import Q_log2
import VectorRotate
import Vector4Scale
import VectorNormalize2
import VectorNormalize
import CrossProduct
import VectorInverse
import VectorNormalizeFast
import DistanceSquared
import Distance
import VectorLengthSquared
import VectorLength
import VectorCompare
import AddPointToBounds
import ClearBounds
import RadiusFromBounds
import NormalizeColor
import ColorBytes4
import ColorBytes3
import _VectorMA
import _VectorScale
import _VectorCopy
import _VectorAdd
import _VectorSubtract
import _DotProduct
import ByteToDir
import DirToByte
import ClampShort
import ClampChar
import Q_rsqrt
import Q_fabs
import Q_isnan
import axisDefault
import vec3_origin
import g_color_table
import colorDkGrey
import colorMdGrey
import colorLtGrey
import colorWhite
import colorCyan
import colorMagenta
import colorYellow
import colorBlue
import colorGreen
import colorRed
import colorBlack
import bytedirs
import Hunk_AllocDebug
import FloatSwap
import LongSwap
import ShortSwap
import CopyLongSwap
import CopyShortSwap
import exp
import fabs
import abs
import tan
import atan2
import Q_asin
import Q_acos
import cos
import sin
import sqrt
import floor
import ceil
import memcmp
import memcpy
import memset
import memmove
import sscanf
import Q_vsnprintf
import strtol
import _atoi
import atoi
import strtod
import _atof
import atof
import toupper
import tolower
import strncpy
import strstr
import strrchr
import strchr
import strcmp
import strcpy
import strcat
import strlen
import rand
import srand
import qsort
lit
align 1
LABELV $190
byte 1 109
byte 1 101
byte 1 110
byte 1 117
byte 1 47
byte 1 97
byte 1 114
byte 1 116
byte 1 47
byte 1 109
byte 1 101
byte 1 110
byte 1 117
byte 1 95
byte 1 98
byte 1 97
byte 1 99
byte 1 107
byte 1 0
align 1
LABELV $189
byte 1 109
byte 1 117
byte 1 115
byte 1 105
byte 1 99
byte 1 32
byte 1 109
byte 1 117
byte 1 115
byte 1 105
byte 1 99
byte 1 47
byte 1 97
byte 1 109
byte 1 112
byte 1 54
byte 1 95
byte 1 50
byte 1 50
byte 1 107
byte 1 10
byte 1 0
align 1
LABELV $188
byte 1 115
byte 1 95
byte 1 109
byte 1 117
byte 1 115
byte 1 105
byte 1 99
byte 1 118
byte 1 111
byte 1 108
byte 1 117
byte 1 109
byte 1 101
byte 1 32
byte 1 48
byte 1 46
byte 1 53
byte 1 10
byte 1 0
align 1
LABELV $185
byte 1 115
byte 1 95
byte 1 109
byte 1 117
byte 1 115
byte 1 105
byte 1 99
byte 1 118
byte 1 111
byte 1 108
byte 1 117
byte 1 109
byte 1 101
byte 1 0
align 1
LABELV $135
byte 1 115
byte 1 95
byte 1 109
byte 1 117
byte 1 115
byte 1 105
byte 1 99
byte 1 118
byte 1 111
byte 1 108
byte 1 117
byte 1 109
byte 1 101
byte 1 32
byte 1 37
byte 1 102
byte 1 59
byte 1 32
byte 1 113
byte 1 117
byte 1 105
byte 1 116
byte 1 10
byte 1 0
align 1
LABELV $131
byte 1 68
byte 1 111
byte 1 110
byte 1 110
byte 1 97
byte 1 32
byte 1 74
byte 1 97
byte 1 99
byte 1 107
byte 1 115
byte 1 111
byte 1 110
byte 1 0
align 1
LABELV $130
byte 1 66
byte 1 105
byte 1 122
byte 1 32
byte 1 65
byte 1 115
byte 1 115
byte 1 105
byte 1 115
byte 1 116
byte 1 32
byte 1 97
byte 1 110
byte 1 100
byte 1 32
byte 1 105
byte 1 100
byte 1 32
byte 1 109
byte 1 111
byte 1 109
byte 1 58
byte 1 0
align 1
LABELV $129
byte 1 75
byte 1 97
byte 1 116
byte 1 104
byte 1 101
byte 1 114
byte 1 105
byte 1 110
byte 1 101
byte 1 32
byte 1 65
byte 1 110
byte 1 110
byte 1 97
byte 1 32
byte 1 75
byte 1 97
byte 1 110
byte 1 103
byte 1 0
align 1
LABELV $128
byte 1 68
byte 1 105
byte 1 114
byte 1 101
byte 1 99
byte 1 116
byte 1 111
byte 1 114
byte 1 32
byte 1 111
byte 1 102
byte 1 32
byte 1 66
byte 1 117
byte 1 115
byte 1 105
byte 1 110
byte 1 101
byte 1 115
byte 1 115
byte 1 32
byte 1 68
byte 1 101
byte 1 118
byte 1 101
byte 1 108
byte 1 111
byte 1 112
byte 1 109
byte 1 101
byte 1 110
byte 1 116
byte 1 58
byte 1 0
align 1
LABELV $127
byte 1 84
byte 1 111
byte 1 100
byte 1 100
byte 1 32
byte 1 72
byte 1 111
byte 1 108
byte 1 108
byte 1 101
byte 1 110
byte 1 115
byte 1 104
byte 1 101
byte 1 97
byte 1 100
byte 1 0
align 1
LABELV $126
byte 1 67
byte 1 69
byte 1 79
byte 1 58
byte 1 0
align 1
LABELV $125
byte 1 80
byte 1 97
byte 1 117
byte 1 108
byte 1 32
byte 1 74
byte 1 97
byte 1 113
byte 1 117
byte 1 97
byte 1 121
byte 1 115
byte 1 0
align 1
LABELV $124
byte 1 84
byte 1 105
byte 1 109
byte 1 32
byte 1 87
byte 1 105
byte 1 108
byte 1 108
byte 1 105
byte 1 116
byte 1 115
byte 1 44
byte 1 32
byte 1 67
byte 1 104
byte 1 114
byte 1 105
byte 1 115
byte 1 116
byte 1 105
byte 1 97
byte 1 110
byte 1 32
byte 1 65
byte 1 110
byte 1 116
byte 1 107
byte 1 111
byte 1 119
byte 1 0
align 1
LABELV $123
byte 1 76
byte 1 101
byte 1 118
byte 1 101
byte 1 108
byte 1 32
byte 1 68
byte 1 101
byte 1 115
byte 1 105
byte 1 103
byte 1 110
byte 1 58
byte 1 0
align 1
LABELV $122
byte 1 71
byte 1 114
byte 1 97
byte 1 101
byte 1 109
byte 1 101
byte 1 32
byte 1 68
byte 1 101
byte 1 118
byte 1 105
byte 1 110
byte 1 101
byte 1 0
align 1
LABELV $121
byte 1 71
byte 1 97
byte 1 109
byte 1 101
byte 1 32
byte 1 68
byte 1 101
byte 1 115
byte 1 105
byte 1 103
byte 1 110
byte 1 101
byte 1 114
byte 1 58
byte 1 0
align 1
LABELV $120
byte 1 80
byte 1 97
byte 1 117
byte 1 108
byte 1 32
byte 1 83
byte 1 116
byte 1 101
byte 1 101
byte 1 100
byte 1 44
byte 1 32
byte 1 75
byte 1 101
byte 1 110
byte 1 110
byte 1 101
byte 1 116
byte 1 104
byte 1 32
byte 1 83
byte 1 99
byte 1 111
byte 1 116
byte 1 116
byte 1 0
align 1
LABELV $119
byte 1 65
byte 1 100
byte 1 114
byte 1 105
byte 1 97
byte 1 110
byte 1 32
byte 1 67
byte 1 97
byte 1 114
byte 1 109
byte 1 97
byte 1 99
byte 1 107
byte 1 44
byte 1 32
byte 1 75
byte 1 101
byte 1 118
byte 1 105
byte 1 110
byte 1 32
byte 1 67
byte 1 108
byte 1 111
byte 1 117
byte 1 100
byte 1 44
byte 1 0
align 1
LABELV $118
byte 1 65
byte 1 114
byte 1 116
byte 1 58
byte 1 0
align 1
LABELV $117
byte 1 74
byte 1 111
byte 1 104
byte 1 110
byte 1 32
byte 1 67
byte 1 97
byte 1 114
byte 1 109
byte 1 97
byte 1 99
byte 1 107
byte 1 44
byte 1 32
byte 1 74
byte 1 111
byte 1 104
byte 1 110
byte 1 32
byte 1 67
byte 1 97
byte 1 115
byte 1 104
byte 1 0
align 1
LABELV $116
byte 1 105
byte 1 68
byte 1 32
byte 1 83
byte 1 111
byte 1 102
byte 1 116
byte 1 119
byte 1 97
byte 1 114
byte 1 101
byte 1 32
byte 1 105
byte 1 115
byte 1 58
byte 1 0
align 1
LABELV $115
byte 1 39
byte 1 79
byte 1 110
byte 1 97
byte 1 105
byte 1 39
byte 1 0
align 1
LABELV $114
byte 1 74
byte 1 101
byte 1 114
byte 1 101
byte 1 109
byte 1 105
byte 1 97
byte 1 104
byte 1 32
byte 1 97
byte 1 107
byte 1 97
byte 1 46
byte 1 32
byte 1 39
byte 1 66
byte 1 69
byte 1 84
byte 1 65
byte 1 77
byte 1 79
byte 1 78
byte 1 75
byte 1 69
byte 1 89
byte 1 39
byte 1 0
align 1
LABELV $113
byte 1 84
byte 1 111
byte 1 45
byte 1 77
byte 1 111
byte 1 115
byte 1 0
align 1
LABELV $112
byte 1 66
byte 1 69
byte 1 84
byte 1 65
byte 1 32
byte 1 84
byte 1 101
byte 1 115
byte 1 116
byte 1 105
byte 1 110
byte 1 103
byte 1 0
align 1
LABELV $111
byte 1 77
byte 1 105
byte 1 114
byte 1 99
byte 1 111
byte 1 32
byte 1 39
byte 1 80
byte 1 97
byte 1 110
byte 1 105
byte 1 67
byte 1 39
byte 1 32
byte 1 72
byte 1 101
byte 1 114
byte 1 114
byte 1 109
byte 1 97
byte 1 110
byte 1 110
byte 1 0
align 1
LABELV $110
byte 1 82
byte 1 105
byte 1 99
byte 1 104
byte 1 97
byte 1 114
byte 1 100
byte 1 32
byte 1 83
byte 1 109
byte 1 105
byte 1 116
byte 1 104
byte 1 0
align 1
LABELV $109
byte 1 68
byte 1 111
byte 1 99
byte 1 117
byte 1 109
byte 1 101
byte 1 110
byte 1 116
byte 1 32
byte 1 68
byte 1 101
byte 1 115
byte 1 105
byte 1 103
byte 1 110
byte 1 32
byte 1 97
byte 1 110
byte 1 100
byte 1 32
byte 1 76
byte 1 97
byte 1 121
byte 1 111
byte 1 117
byte 1 116
byte 1 0
align 1
LABELV $108
byte 1 77
byte 1 101
byte 1 108
byte 1 97
byte 1 110
byte 1 105
byte 1 101
byte 1 32
byte 1 97
byte 1 107
byte 1 97
byte 1 46
byte 1 32
byte 1 39
byte 1 84
byte 1 111
byte 1 120
byte 1 105
byte 1 99
byte 1 105
byte 1 116
byte 1 121
byte 1 39
byte 1 0
align 1
LABELV $107
byte 1 84
byte 1 101
byte 1 120
byte 1 116
byte 1 117
byte 1 114
byte 1 101
byte 1 32
byte 1 68
byte 1 101
byte 1 115
byte 1 105
byte 1 103
byte 1 110
byte 1 0
align 1
LABELV $106
byte 1 68
byte 1 97
byte 1 108
byte 1 101
byte 1 32
byte 1 87
byte 1 105
byte 1 108
byte 1 115
byte 1 111
byte 1 110
byte 1 0
align 1
LABELV $105
byte 1 70
byte 1 46
byte 1 83
byte 1 101
byte 1 103
byte 1 101
byte 1 114
byte 1 102
byte 1 97
byte 1 108
byte 1 107
byte 1 0
align 1
LABELV $104
byte 1 80
byte 1 46
byte 1 65
byte 1 110
byte 1 100
byte 1 101
byte 1 114
byte 1 115
byte 1 115
byte 1 111
byte 1 110
byte 1 0
align 1
LABELV $103
byte 1 83
byte 1 111
byte 1 117
byte 1 110
byte 1 100
byte 1 32
byte 1 68
byte 1 101
byte 1 115
byte 1 105
byte 1 103
byte 1 110
byte 1 32
byte 1 97
byte 1 110
byte 1 100
byte 1 32
byte 1 77
byte 1 117
byte 1 115
byte 1 105
byte 1 99
byte 1 0
align 1
LABELV $102
byte 1 39
byte 1 83
byte 1 116
byte 1 101
byte 1 101
byte 1 108
byte 1 32
byte 1 80
byte 1 97
byte 1 105
byte 1 110
byte 1 116
byte 1 101
byte 1 114
byte 1 39
byte 1 0
align 1
LABELV $101
byte 1 65
byte 1 114
byte 1 116
byte 1 32
byte 1 97
byte 1 110
byte 1 100
byte 1 32
byte 1 77
byte 1 111
byte 1 100
byte 1 101
byte 1 108
byte 1 115
byte 1 0
align 1
LABELV $100
byte 1 39
byte 1 97
byte 1 105
byte 1 108
byte 1 109
byte 1 97
byte 1 110
byte 1 107
byte 1 105
byte 1 39
byte 1 32
byte 1 97
byte 1 107
byte 1 97
byte 1 46
byte 1 32
byte 1 39
byte 1 112
byte 1 101
byte 1 121
byte 1 111
byte 1 116
byte 1 101
byte 1 39
byte 1 0
align 1
LABELV $99
byte 1 69
byte 1 100
byte 1 100
byte 1 121
byte 1 32
byte 1 86
byte 1 97
byte 1 108
byte 1 100
byte 1 101
byte 1 122
byte 1 32
byte 1 97
byte 1 107
byte 1 97
byte 1 46
byte 1 32
byte 1 39
byte 1 116
byte 1 104
byte 1 101
byte 1 98
byte 1 105
byte 1 103
byte 1 98
byte 1 117
byte 1 117
byte 1 39
byte 1 0
align 1
LABELV $98
byte 1 84
byte 1 104
byte 1 111
byte 1 109
byte 1 97
byte 1 115
byte 1 32
byte 1 97
byte 1 107
byte 1 97
byte 1 46
byte 1 32
byte 1 39
byte 1 84
byte 1 111
byte 1 45
byte 1 109
byte 1 111
byte 1 115
byte 1 39
byte 1 0
align 1
LABELV $97
byte 1 39
byte 1 79
byte 1 108
byte 1 105
byte 1 118
byte 1 101
byte 1 114
byte 1 86
byte 1 39
byte 1 0
align 1
LABELV $96
byte 1 39
byte 1 77
byte 1 121
byte 1 115
byte 1 116
byte 1 101
byte 1 114
byte 1 105
byte 1 111
byte 1 117
byte 1 115
byte 1 80
byte 1 111
byte 1 101
byte 1 116
byte 1 100
byte 1 39
byte 1 32
byte 1 97
byte 1 107
byte 1 97
byte 1 32
byte 1 39
byte 1 80
byte 1 111
byte 1 101
byte 1 116
byte 1 39
byte 1 0
align 1
LABELV $95
byte 1 77
byte 1 105
byte 1 99
byte 1 104
byte 1 97
byte 1 101
byte 1 108
byte 1 32
byte 1 39
byte 1 67
byte 1 121
byte 1 98
byte 1 101
byte 1 114
byte 1 100
byte 1 101
byte 1 109
byte 1 111
byte 1 110
byte 1 39
byte 1 32
byte 1 75
byte 1 97
byte 1 109
byte 1 105
byte 1 110
byte 1 115
byte 1 107
byte 1 121
byte 1 0
align 1
LABELV $94
byte 1 74
byte 1 111
byte 1 110
byte 1 97
byte 1 116
byte 1 104
byte 1 97
byte 1 110
byte 1 32
byte 1 39
byte 1 65
byte 1 109
byte 1 112
byte 1 104
byte 1 101
byte 1 116
byte 1 97
byte 1 109
byte 1 105
byte 1 110
byte 1 101
byte 1 39
byte 1 32
byte 1 71
byte 1 97
byte 1 114
byte 1 114
byte 1 111
byte 1 100
byte 1 0
align 1
LABELV $93
byte 1 83
byte 1 105
byte 1 109
byte 1 111
byte 1 110
byte 1 32
byte 1 39
byte 1 83
byte 1 121
byte 1 115
byte 1 116
byte 1 101
byte 1 109
byte 1 32
byte 1 75
byte 1 114
byte 1 97
byte 1 115
byte 1 104
byte 1 39
byte 1 32
byte 1 66
byte 1 97
byte 1 116
byte 1 116
byte 1 121
byte 1 0
align 1
LABELV $92
byte 1 74
byte 1 105
byte 1 109
byte 1 32
byte 1 39
byte 1 103
byte 1 111
byte 1 117
byte 1 116
byte 1 39
byte 1 32
byte 1 66
byte 1 97
byte 1 104
byte 1 101
byte 1 0
align 1
LABELV $91
byte 1 74
byte 1 101
byte 1 102
byte 1 102
byte 1 32
byte 1 39
byte 1 83
byte 1 116
byte 1 101
byte 1 99
byte 1 107
byte 1 105
byte 1 39
byte 1 32
byte 1 71
byte 1 97
byte 1 114
byte 1 115
byte 1 116
byte 1 101
byte 1 99
byte 1 107
byte 1 105
byte 1 0
align 1
LABELV $90
byte 1 77
byte 1 97
byte 1 112
byte 1 112
byte 1 105
byte 1 110
byte 1 103
byte 1 58
byte 1 0
align 1
LABELV $89
byte 1 69
byte 1 114
byte 1 97
byte 1 115
byte 1 101
byte 1 114
byte 1 32
byte 1 102
byte 1 114
byte 1 111
byte 1 109
byte 1 32
byte 1 69
byte 1 110
byte 1 116
byte 1 105
byte 1 116
byte 1 121
byte 1 80
byte 1 108
byte 1 117
byte 1 115
byte 1 0
align 1
LABELV $88
byte 1 90
byte 1 84
byte 1 117
byte 1 114
byte 1 116
byte 1 108
byte 1 101
byte 1 77
byte 1 97
byte 1 110
byte 1 32
byte 1 102
byte 1 114
byte 1 111
byte 1 109
byte 1 32
byte 1 84
byte 1 117
byte 1 114
byte 1 116
byte 1 108
byte 1 101
byte 1 65
byte 1 114
byte 1 101
byte 1 110
byte 1 97
byte 1 0
align 1
LABELV $87
byte 1 69
byte 1 100
byte 1 100
byte 1 121
byte 1 32
byte 1 86
byte 1 97
byte 1 108
byte 1 100
byte 1 101
byte 1 122
byte 1 32
byte 1 97
byte 1 107
byte 1 97
byte 1 46
byte 1 32
byte 1 39
byte 1 84
byte 1 104
byte 1 101
byte 1 66
byte 1 105
byte 1 103
byte 1 66
byte 1 117
byte 1 117
byte 1 39
byte 1 0
align 1
LABELV $86
byte 1 80
byte 1 101
byte 1 114
byte 1 32
byte 1 39
byte 1 80
byte 1 101
byte 1 114
byte 1 108
byte 1 101
byte 1 39
byte 1 32
byte 1 84
byte 1 104
byte 1 111
byte 1 114
byte 1 109
byte 1 97
byte 1 110
byte 1 110
byte 1 0
align 1
LABELV $85
byte 1 83
byte 1 116
byte 1 101
byte 1 118
byte 1 101
byte 1 110
byte 1 32
byte 1 39
byte 1 83
byte 1 116
byte 1 111
byte 1 110
byte 1 101
byte 1 108
byte 1 97
byte 1 110
byte 1 99
byte 1 101
byte 1 39
byte 1 32
byte 1 72
byte 1 101
byte 1 105
byte 1 106
byte 1 115
byte 1 116
byte 1 101
byte 1 114
byte 1 115
byte 1 0
align 1
LABELV $84
byte 1 80
byte 1 114
byte 1 111
byte 1 103
byte 1 114
byte 1 97
byte 1 109
byte 1 109
byte 1 105
byte 1 110
byte 1 103
byte 1 58
byte 1 0
align 1
LABELV $83
byte 1 0
align 1
LABELV $82
byte 1 81
byte 1 51
byte 1 82
byte 1 97
byte 1 108
byte 1 108
byte 1 121
byte 1 32
byte 1 84
byte 1 101
byte 1 97
byte 1 109
byte 1 0
