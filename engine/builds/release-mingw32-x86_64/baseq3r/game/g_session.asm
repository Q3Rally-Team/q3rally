export G_WriteClientSessionData
code
proc G_WriteClientSessionData 20 32
ADDRFP4 0
ADDRFP4 0
INDIRP4
ASGNP4
ADDRGP4 $65
ARGP4
ADDRFP4 0
INDIRP4
CNSTI4 624
ADDP4
INDIRI4
ARGI4
ADDRFP4 0
INDIRP4
CNSTI4 628
ADDP4
INDIRI4
ARGI4
ADDRFP4 0
INDIRP4
CNSTI4 632
ADDP4
INDIRI4
ARGI4
ADDRFP4 0
INDIRP4
CNSTI4 640
ADDP4
INDIRI4
ARGI4
ADDRFP4 0
INDIRP4
CNSTI4 644
ADDP4
INDIRI4
ARGI4
ADDRFP4 0
INDIRP4
CNSTI4 648
ADDP4
INDIRI4
ARGI4
ADDRFP4 0
INDIRP4
CNSTI4 652
ADDP4
INDIRI4
ARGI4
ADDRLP4 12
ADDRGP4 va
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 12
INDIRP4
ASGNP4
ADDRGP4 $66
ARGP4
ADDRFP4 0
INDIRP4
CVPU4 4
ADDRGP4 level
INDIRP4
CVPU4 4
SUBU4
CVUI4 4
CNSTI4 8248
DIVI4
ARGI4
ADDRLP4 16
ADDRGP4 va
CALLP4
ASGNP4
ADDRLP4 4
ADDRLP4 16
INDIRP4
ASGNP4
ADDRLP4 4
INDIRP4
ARGP4
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 trap_Cvar_Set
CALLV
pop
LABELV $64
endproc G_WriteClientSessionData 20 32
export G_ReadSessionData
proc G_ReadSessionData 1048 36
ADDRFP4 0
ADDRFP4 0
INDIRP4
ASGNP4
ADDRGP4 $66
ARGP4
ADDRFP4 0
INDIRP4
CVPU4 4
ADDRGP4 level
INDIRP4
CVPU4 4
SUBU4
CVUI4 4
CNSTI4 8248
DIVI4
ARGI4
ADDRLP4 1040
ADDRGP4 va
CALLP4
ASGNP4
ADDRLP4 1024
ADDRLP4 1040
INDIRP4
ASGNP4
ADDRLP4 1024
INDIRP4
ARGP4
ADDRLP4 0
ARGP4
CNSTI4 1024
ARGI4
ADDRGP4 trap_Cvar_VariableStringBuffer
CALLV
pop
ADDRLP4 0
ARGP4
ADDRGP4 $65
ARGP4
ADDRLP4 1036
ARGP4
ADDRFP4 0
INDIRP4
CNSTI4 628
ADDP4
ARGP4
ADDRLP4 1032
ARGP4
ADDRFP4 0
INDIRP4
CNSTI4 640
ADDP4
ARGP4
ADDRFP4 0
INDIRP4
CNSTI4 644
ADDP4
ARGP4
ADDRFP4 0
INDIRP4
CNSTI4 648
ADDP4
ARGP4
ADDRLP4 1028
ARGP4
ADDRGP4 sscanf
CALLI4
pop
ADDRFP4 0
INDIRP4
CNSTI4 624
ADDP4
ADDRLP4 1036
INDIRI4
ASGNI4
ADDRFP4 0
INDIRP4
CNSTI4 632
ADDP4
ADDRLP4 1032
INDIRI4
ASGNI4
ADDRFP4 0
INDIRP4
CNSTI4 652
ADDP4
ADDRLP4 1028
INDIRI4
ASGNI4
LABELV $67
endproc G_ReadSessionData 1048 36
export G_InitSessionData
proc G_InitSessionData 28 8
ADDRFP4 0
ADDRFP4 0
INDIRP4
ASGNP4
ADDRLP4 4
ADDRFP4 0
INDIRP4
CNSTI4 624
ADDP4
ASGNP4
ADDRFP4 4
INDIRP4
ARGP4
ADDRGP4 $69
ARGP4
ADDRLP4 8
ADDRGP4 Info_ValueForKey
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8
INDIRP4
ASGNP4
ADDRLP4 12
CNSTI4 0
ASGNI4
ADDRLP4 0
INDIRP4
INDIRI1
CVII4 1
ADDRLP4 12
INDIRI4
NEI4 $70
ADDRGP4 g_localTeamPref+16
INDIRI1
CVII4 1
ADDRLP4 12
INDIRI4
EQI4 $70
ADDRFP4 0
INDIRP4
CNSTI4 496
ADDP4
INDIRI4
ADDRLP4 12
INDIRI4
EQI4 $70
ADDRLP4 0
ADDRGP4 g_localTeamPref+16
ASGNP4
ADDRGP4 $74
ARGP4
ADDRGP4 $75
ARGP4
ADDRGP4 trap_Cvar_Set
CALLV
pop
LABELV $70
ADDRGP4 g_gametype+12
INDIRI4
CNSTI4 5
LTI4 $76
ADDRLP4 4
INDIRP4
CNSTI4 5
ASGNI4
ADDRLP4 4
INDIRP4
CNSTI4 8
ADDP4
CNSTI4 1
ASGNI4
ADDRLP4 16
CNSTI4 0
ASGNI4
ADDRLP4 0
INDIRP4
INDIRI1
CVII4 1
ADDRLP4 16
INDIRI4
NEI4 $83
ADDRGP4 g_teamAutoJoin+12
INDIRI4
ADDRLP4 16
INDIRI4
EQI4 $77
LABELV $83
ADDRLP4 20
ADDRGP4 isRallyRace
CALLI4
ASGNI4
ADDRLP4 24
CNSTI4 0
ASGNI4
ADDRLP4 20
INDIRI4
ADDRLP4 24
INDIRI4
EQI4 $84
ADDRGP4 level+9500
INDIRI4
ADDRLP4 24
INDIRI4
NEI4 $77
LABELV $84
CNSTI4 940
ADDRFP4 0
INDIRP4
CVPU4 4
ADDRGP4 level
INDIRP4
CVPU4 4
SUBU4
CVUI4 4
CNSTI4 8248
DIVI4
MULI4
ADDRGP4 g_entities
ADDP4
ARGP4
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 SetTeam
CALLV
pop
ADDRGP4 $77
JUMPV
LABELV $76
ADDRLP4 0
INDIRP4
INDIRI1
CVII4 1
CNSTI4 115
NEI4 $85
ADDRLP4 4
INDIRP4
CNSTI4 5
ASGNI4
ADDRGP4 $86
JUMPV
LABELV $85
ADDRLP4 16
ADDRGP4 g_gametype+12
INDIRI4
ASGNI4
ADDRLP4 16
INDIRI4
CNSTI4 0
LTI4 $87
ADDRLP4 16
INDIRI4
CNSTI4 4
GTI4 $87
ADDRLP4 16
INDIRI4
CNSTI4 2
LSHI4
ADDRGP4 $105
ADDP4
INDIRP4
JUMPV
lit
align 4
LABELV $105
address $90
address $90
address $99
address $90
address $99
code
LABELV $87
LABELV $90
ADDRGP4 g_maxGameClients+12
INDIRI4
CNSTI4 0
LEI4 $91
ADDRGP4 level+108
INDIRI4
ADDRGP4 g_maxGameClients+12
INDIRI4
LTI4 $91
ADDRLP4 4
INDIRP4
CNSTI4 5
ASGNI4
ADDRGP4 $88
JUMPV
LABELV $91
ADDRGP4 level+9500
INDIRI4
CNSTI4 0
EQI4 $96
ADDRLP4 4
INDIRP4
CNSTI4 5
ASGNI4
ADDRGP4 $88
JUMPV
LABELV $96
ADDRLP4 4
INDIRP4
CNSTI4 0
ASGNI4
ADDRGP4 $88
JUMPV
LABELV $99
ADDRGP4 g_maxGameClients+12
INDIRI4
CNSTI4 0
LEI4 $100
ADDRGP4 level+108
INDIRI4
ADDRGP4 g_maxGameClients+12
INDIRI4
LTI4 $100
ADDRLP4 4
INDIRP4
CNSTI4 5
ASGNI4
ADDRGP4 $88
JUMPV
LABELV $100
ADDRLP4 4
INDIRP4
CNSTI4 0
ASGNI4
LABELV $88
LABELV $86
ADDRLP4 4
INDIRP4
CNSTI4 8
ADDP4
CNSTI4 1
ASGNI4
LABELV $77
ADDRFP4 0
INDIRP4
ARGP4
ADDRGP4 AddTournamentQueue
CALLV
pop
ADDRFP4 0
INDIRP4
ARGP4
ADDRGP4 G_WriteClientSessionData
CALLV
pop
LABELV $68
endproc G_InitSessionData 28 8
export G_InitWorldSession
proc G_InitWorldSession 1032 12
ADDRGP4 $107
ARGP4
ADDRLP4 0
ARGP4
CNSTI4 1024
ARGI4
ADDRGP4 trap_Cvar_VariableStringBuffer
CALLV
pop
ADDRLP4 0
ARGP4
ADDRLP4 1028
ADDRGP4 atoi
CALLI4
ASGNI4
ADDRLP4 1024
ADDRLP4 1028
INDIRI4
ASGNI4
ADDRGP4 g_gametype+12
INDIRI4
ADDRLP4 1024
INDIRI4
EQI4 $108
ADDRGP4 level+96
CNSTI4 1
ASGNI4
ADDRGP4 $112
ARGP4
ADDRGP4 G_Printf
CALLV
pop
LABELV $108
LABELV $106
endproc G_InitWorldSession 1032 12
export G_WriteSessionData
proc G_WriteSessionData 8 8
ADDRGP4 $114
ARGP4
ADDRGP4 g_gametype+12
INDIRI4
ARGI4
ADDRLP4 4
ADDRGP4 va
CALLP4
ASGNP4
ADDRGP4 $107
ARGP4
ADDRLP4 4
INDIRP4
ARGP4
ADDRGP4 trap_Cvar_Set
CALLV
pop
ADDRLP4 0
CNSTI4 0
ASGNI4
ADDRGP4 $119
JUMPV
LABELV $116
CNSTI4 8248
ADDRLP4 0
INDIRI4
MULI4
ADDRGP4 level
INDIRP4
ADDP4
CNSTI4 468
ADDP4
INDIRI4
CNSTI4 2
NEI4 $121
CNSTI4 8248
ADDRLP4 0
INDIRI4
MULI4
ADDRGP4 level
INDIRP4
ADDP4
ARGP4
ADDRGP4 G_WriteClientSessionData
CALLV
pop
LABELV $121
LABELV $117
ADDRLP4 0
ADDRLP4 0
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
LABELV $119
ADDRLP4 0
INDIRI4
ADDRGP4 level+24
INDIRI4
LTI4 $116
LABELV $113
endproc G_WriteSessionData 8 8
import visible
import findradius
import trap_SnapVector
import trap_GeneticParentsAndChildSelection
import trap_BotResetWeaponState
import trap_BotFreeWeaponState
import trap_BotAllocWeaponState
import trap_BotLoadWeaponWeights
import trap_BotGetWeaponInfo
import trap_BotChooseBestFightWeapon
import trap_BotAddAvoidSpot
import trap_BotInitMoveState
import trap_BotFreeMoveState
import trap_BotAllocMoveState
import trap_BotPredictVisiblePosition
import trap_BotMovementViewTarget
import trap_BotReachabilityArea
import trap_BotResetLastAvoidReach
import trap_BotResetAvoidReach
import trap_BotMoveInDirection
import trap_BotMoveToGoal
import trap_BotResetMoveState
import trap_BotFreeGoalState
import trap_BotAllocGoalState
import trap_BotMutateGoalFuzzyLogic
import trap_BotSaveGoalFuzzyLogic
import trap_BotInterbreedGoalFuzzyLogic
import trap_BotFreeItemWeights
import trap_BotLoadItemWeights
import trap_BotUpdateEntityItems
import trap_BotInitLevelItems
import trap_BotSetAvoidGoalTime
import trap_BotAvoidGoalTime
import trap_BotGetLevelItemGoal
import trap_BotGetMapLocationGoal
import trap_BotGetNextCampSpotGoal
import trap_BotItemGoalInVisButNotVisible
import trap_BotTouchingGoal
import trap_BotChooseNBGItem
import trap_BotChooseLTGItem
import trap_BotGetSecondGoal
import trap_BotGetTopGoal
import trap_BotGoalName
import trap_BotDumpGoalStack
import trap_BotDumpAvoidGoals
import trap_BotEmptyGoalStack
import trap_BotPopGoal
import trap_BotPushGoal
import trap_BotResetAvoidGoals
import trap_BotRemoveFromAvoidGoals
import trap_BotResetGoalState
import trap_BotSetChatName
import trap_BotSetChatGender
import trap_BotLoadChatFile
import trap_BotReplaceSynonyms
import trap_UnifyWhiteSpaces
import trap_BotMatchVariable
import trap_BotFindMatch
import trap_StringContains
import trap_BotGetChatMessage
import trap_BotEnterChat
import trap_BotChatLength
import trap_BotReplyChat
import trap_BotNumInitialChats
import trap_BotInitialChat
import trap_BotNumConsoleMessages
import trap_BotNextConsoleMessage
import trap_BotRemoveConsoleMessage
import trap_BotQueueConsoleMessage
import trap_BotFreeChatState
import trap_BotAllocChatState
import trap_Characteristic_String
import trap_Characteristic_BInteger
import trap_Characteristic_Integer
import trap_Characteristic_BFloat
import trap_Characteristic_Float
import trap_BotFreeCharacter
import trap_BotLoadCharacter
import trap_EA_ResetInput
import trap_EA_GetInput
import trap_EA_EndRegular
import trap_EA_View
import trap_EA_Move
import trap_EA_DelayedJump
import trap_EA_Jump
import trap_EA_SelectWeapon
import trap_EA_MoveRight
import trap_EA_MoveLeft
import trap_EA_MoveBack
import trap_EA_MoveForward
import trap_EA_MoveDown
import trap_EA_MoveUp
import trap_EA_Crouch
import trap_EA_Respawn
import trap_EA_Use
import trap_EA_Attack
import trap_EA_Talk
import trap_EA_Gesture
import trap_EA_Action
import trap_EA_Command
import trap_EA_SayTeam
import trap_EA_Say
import trap_AAS_PredictClientMovement
import trap_AAS_Swimming
import trap_AAS_AlternativeRouteGoals
import trap_AAS_PredictRoute
import trap_AAS_EnableRoutingArea
import trap_AAS_AreaTravelTimeToGoalArea
import trap_AAS_AreaReachability
import trap_AAS_IntForBSPEpairKey
import trap_AAS_FloatForBSPEpairKey
import trap_AAS_VectorForBSPEpairKey
import trap_AAS_ValueForBSPEpairKey
import trap_AAS_NextBSPEntity
import trap_AAS_PointContents
import trap_AAS_TraceAreas
import trap_AAS_PointReachabilityAreaIndex
import trap_AAS_PointAreaNum
import trap_AAS_Time
import trap_AAS_PresenceTypeBoundingBox
import trap_AAS_Initialized
import trap_AAS_EntityInfo
import trap_AAS_AreaInfo
import trap_AAS_BBoxAreas
import trap_BotUserCommand
import trap_BotGetServerCommand
import trap_BotGetSnapshotEntity
import trap_BotLibTest
import trap_BotLibUpdateEntity
import trap_BotLibLoadMap
import trap_BotLibStartFrame
import trap_BotLibDefine
import trap_BotLibVarGet
import trap_BotLibVarSet
import trap_BotLibShutdown
import trap_BotLibSetup
import trap_DebugPolygonDelete
import trap_DebugPolygonCreate
import trap_GetEntityToken
import trap_GetUsercmd
import trap_BotFreeClient
import trap_BotAllocateClient
import trap_EntityContact
import trap_EntitiesInBox
import trap_UnlinkEntity
import trap_LinkEntity
import trap_AreasConnected
import trap_AdjustAreaPortalState
import trap_InPVSIgnorePortals
import trap_InPVS
import trap_PointContents
import trap_TraceCapsule
import trap_Trace
import trap_SetBrushModel
import trap_GetServerinfo
import trap_SetUserinfo
import trap_GetUserinfo
import trap_GetConfigstring
import trap_SetConfigstring
import trap_SendServerCommand
import trap_DropClient
import trap_LocateGameData
import trap_Cvar_VariableStringBuffer
import trap_Cvar_VariableValue
import trap_Cvar_VariableIntegerValue
import trap_Cvar_Set
import trap_Cvar_Update
import trap_Cvar_Register
import trap_SendConsoleCommand
import trap_FS_Seek
import trap_FS_GetFileList
import trap_FS_FCloseFile
import trap_FS_Write
import trap_FS_Read
import trap_FS_FOpenFile
import trap_Args
import trap_Argv
import trap_Argc
import trap_RealTime
import trap_Milliseconds
import trap_Error
import trap_Print
import car_friction_scale
import car_air_frac_to_df
import car_air_cof
import car_body_elasticity
import car_IT_zScale
import car_IT_yScale
import car_IT_xScale
import car_frontweight_dist
import car_wheel_damp
import car_wheel
import car_swaybar
import car_shock_down
import car_shock_up
import car_spring
import g_damageScale
import g_developer
import g_trackLength
import g_trackReversed
import g_finishRaceDelay
import g_forceEngineStart
import g_humanplayers
import g_localTeamPref
import g_proxMineTimeout
import g_singlePlayer
import g_enableBreath
import g_enableDust
import g_rankings
import pmove_msec
import pmove_fixed
import g_smoothClients
import g_blueteam
import g_redteam
import g_cubeTimeout
import g_obeliskRespawnDelay
import g_obeliskRegenAmount
import g_obeliskRegenPeriod
import g_obeliskHealth
import g_filterBan
import g_banIPs
import g_teamForceBalance
import g_teamAutoJoin
import g_allowVote
import g_blood
import g_doWarmup
import g_warmup
import g_motd
import g_synchronousClients
import g_weaponTeamRespawn
import g_weaponRespawn
import g_debugDamage
import g_debugAlloc
import g_debugMove
import g_inactivity
import g_forcerespawn
import g_quadfactor
import g_knockback
import g_speed
import g_gravity
import g_needpass
import g_password
import g_friendlyFire
import g_capturelimit
import g_timelimit
import g_laplimit
import g_fraglimit
import g_dmflags
import g_restarted
import g_maxGameClients
import g_maxclients
import g_cheats
import g_dedicated
import g_gametype
import g_entities
import level
import Pickup_Team
import CheckTeamStatus
import TeamplayInfoMessage
import Team_GetLocationMsg
import Team_GetLocation
import SelectCTFSpawnPoint
import Team_FreeEntity
import Team_ReturnFlag
import Team_InitGame
import Team_CheckHurtCarrier
import Team_FragBonuses
import Team_DroppedFlagThink
import AddTeamScore
import TeamColorString
import TeamName
import OtherTeam
import BotTestAAS
import BotAIStartFrame
import BotAIShutdownClient
import BotAISetupClient
import BotAILoadMap
import BotAIShutdown
import BotAISetup
import BotInterbreedEndMatch
import Svcmd_BotList_f
import Svcmd_AddBot_f
import G_BotConnect
import G_RemoveQueuedBotBegin
import G_CheckBotSpawn
import G_GetBotInfoByName
import G_GetBotInfoByNumber
import G_InitBots
import Svcmd_AbortPodium_f
import SpawnModelsOnVictoryPads
import UpdateTournamentInfo
import Svcmd_GameMem_f
import G_InitMemory
import G_Alloc
import CheckObeliskAttack
import Team_CheckDroppedItem
import OnSameTeam
import G_RunClient
import ClientEndFrame
import ClientThink
import SelectSpectatorSpawnPoint
import ClientCommand
import ClientBegin
import ClientDisconnect
import ClientUserinfoChanged
import ClientConnect
import G_Error
import G_Printf
import SendScoreboardMessageToAllClients
import G_DebugLogPrintf
import G_LogPrintf
import AddTournamentQueue
import G_RunThink
import CheckTeamLeader
import SetLeader
import FindIntermissionPoint
import MoveClientToIntermission
import DeathmatchScoreboardMessage
import CreateSmokeHazard
import CreatePoisonCloudHazard
import CreatePoisonHazard
import CreateOilHazard
import CheckForOil
import CreateBioHazard
import CreateFireHazard
import FireRearWeapon
import SelectGridPositionSpawn
import SelectLastMarkerForSpawn
import Cmd_Times_f
import Cmd_RacePositions_f
import CalculatePlayerPositions
import CreateRallyStarter
import GetTeamAtRank
import G_PrintMapStats
import isRaceObserver
import isRallyNonDMRace
import isRallyRace
import CenterPrint_All
import G_DropRearWeapon
import G_ResetCar
import G_TempRallyEntity
import G_FrictionCalc
import G_Get2ndDervOnCurveBetweenCheckpoints
import G_GetDervOnCurveBetweenCheckpoints
import G_GetPointOnCurveBetweenCheckpoints
import DropToFloor
import UpdateObserverSpot
import FindBestObserverSpot
import FireAltWeapon
import FireWeapon
import G_FilterPacket
import G_ProcessIPBans
import ConsoleCommand
import SpotWouldTelefrag
import CalculateRanks
import AddScore
import player_die
import ClientSpawn
import InitBodyQue
import BeginIntermission
import ClientRespawn
import CopyToBodyQue
import SelectSpawnPoint
import SetClientViewAngle
import PickTeam
import TeamLeader
import TeamCount
import weapon_telefrag_fire
import TelefragPlayer
import CheckGauntletAttack
import SnapVectorTowards
import CalcMuzzlePoint
import LogAccuracyHit
import TeleportPlayer
import trigger_teleporter_touch
import Break_Breakable
import Touch_DoorTrigger
import G_RunMover
import fire_plasma_bounce
import fire_mine
import fire_cluster_flame
import fire_flame
import fire_bfg
import fire_homing_rocket
import fire_rocket
import fire_cluster_grenade2
import fire_cluster_grenade
import fire_grenade
import fire_plasma
import G_RunMissile
import TossClientCubes
import TossClientItems
import body_die
import G_InvulnerabilityEffect
import G_RadiusDamage_NoKnockBack
import G_RadiusDamage
import G_Damage
import CanDamage
import PickDebrisType
import BuildShaderStateConfig
import AddRemap
import G_SetOrigin
import G_AddEvent
import G_AddPredictableEvent
import tv
import G_TouchTriggers
import G_EntitiesFree
import G_FreeEntity
import G_Sound
import G_TempEntity
import G_Spawn
import G_InitGentity
import G_SetMovedir
import G_UseTargets
import G_PickTarget
import G_Find
import G_KillBox
import G_TeamCommand
import G_SoundIndex
import G_ModelIndex
import Rally_Sound
import G_ScriptIndex
import SaveRegisteredItems
import RegisterItem
import ClearRegisteredItems
import Touch_Item
import Add_Ammo
import ArmorIndex
import Think_Weapon
import FinishSpawningItem
import G_SpawnItem
import SetRespawn
import LaunchItem
import Drop_Item
import PrecacheItem
import UseHoldableItem
import RespawnItem
import G_RunItem
import G_CheckTeamItems
import Cmd_FollowCycle_f
import SetTeam
import BroadcastTeamChange
import StopFollowing
import Cmd_Score_f
import G_NewString
import G_SpawnEntitiesFromString
import G_SpawnVector
import G_SpawnInt
import G_SpawnFloat
import G_SpawnString
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
LABELV $114
byte 1 37
byte 1 105
byte 1 0
align 1
LABELV $112
byte 1 71
byte 1 97
byte 1 109
byte 1 101
byte 1 116
byte 1 121
byte 1 112
byte 1 101
byte 1 32
byte 1 99
byte 1 104
byte 1 97
byte 1 110
byte 1 103
byte 1 101
byte 1 100
byte 1 44
byte 1 32
byte 1 99
byte 1 108
byte 1 101
byte 1 97
byte 1 114
byte 1 105
byte 1 110
byte 1 103
byte 1 32
byte 1 115
byte 1 101
byte 1 115
byte 1 115
byte 1 105
byte 1 111
byte 1 110
byte 1 32
byte 1 100
byte 1 97
byte 1 116
byte 1 97
byte 1 46
byte 1 10
byte 1 0
align 1
LABELV $107
byte 1 115
byte 1 101
byte 1 115
byte 1 115
byte 1 105
byte 1 111
byte 1 110
byte 1 0
align 1
LABELV $75
byte 1 0
align 1
LABELV $74
byte 1 103
byte 1 95
byte 1 108
byte 1 111
byte 1 99
byte 1 97
byte 1 108
byte 1 84
byte 1 101
byte 1 97
byte 1 109
byte 1 80
byte 1 114
byte 1 101
byte 1 102
byte 1 0
align 1
LABELV $69
byte 1 116
byte 1 101
byte 1 97
byte 1 109
byte 1 112
byte 1 114
byte 1 101
byte 1 102
byte 1 0
align 1
LABELV $66
byte 1 115
byte 1 101
byte 1 115
byte 1 115
byte 1 105
byte 1 111
byte 1 110
byte 1 37
byte 1 105
byte 1 0
align 1
LABELV $65
byte 1 37
byte 1 105
byte 1 32
byte 1 37
byte 1 105
byte 1 32
byte 1 37
byte 1 105
byte 1 32
byte 1 37
byte 1 105
byte 1 32
byte 1 37
byte 1 105
byte 1 32
byte 1 37
byte 1 105
byte 1 32
byte 1 37
byte 1 105
byte 1 0