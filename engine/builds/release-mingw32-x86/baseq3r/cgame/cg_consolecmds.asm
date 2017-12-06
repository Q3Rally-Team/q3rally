export CG_TargetCommand_f
code
proc CG_TargetCommand_f 20 12
ADDRLP4 8
ADDRGP4 CG_CrosshairPlayer
CALLI4
ASGNI4
ADDRLP4 0
ADDRLP4 8
INDIRI4
ASGNI4
ADDRLP4 0
INDIRI4
CNSTI4 -1
NEI4 $85
ADDRGP4 $84
JUMPV
LABELV $85
CNSTI4 1
ARGI4
ADDRLP4 4
ARGP4
CNSTI4 4
ARGI4
ADDRGP4 trap_Argv
CALLV
pop
ADDRLP4 4
ARGP4
ADDRLP4 12
ADDRGP4 atoi
CALLI4
ASGNI4
ADDRGP4 $87
ARGP4
ADDRLP4 0
INDIRI4
ARGI4
ADDRLP4 12
INDIRI4
ARGI4
ADDRLP4 16
ADDRGP4 va
CALLP4
ASGNP4
ADDRLP4 16
INDIRP4
ARGP4
ADDRGP4 trap_SendClientCommand
CALLV
pop
LABELV $84
endproc CG_TargetCommand_f 20 12
proc CG_SizeUp_f 4 8
ADDRGP4 $90
ARGP4
ADDRGP4 cg_viewsize+12
INDIRI4
CNSTI4 10
ADDI4
ARGI4
ADDRLP4 0
ADDRGP4 va
CALLP4
ASGNP4
ADDRGP4 $89
ARGP4
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 trap_Cvar_Set
CALLV
pop
LABELV $88
endproc CG_SizeUp_f 4 8
proc CG_SizeDown_f 4 8
ADDRGP4 $90
ARGP4
ADDRGP4 cg_viewsize+12
INDIRI4
CNSTI4 10
SUBI4
ARGI4
ADDRLP4 0
ADDRGP4 va
CALLP4
ASGNP4
ADDRGP4 $89
ARGP4
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 trap_Cvar_Set
CALLV
pop
LABELV $92
endproc CG_SizeDown_f 4 8
proc CG_Viewpos_f 0 20
ADDRGP4 $95
ARGP4
ADDRGP4 cg+109200+24
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 cg+109200+24+4
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 cg+109200+24+8
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 cg+110304+4
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 CG_Printf
CALLV
pop
LABELV $94
endproc CG_Viewpos_f 0 20
proc CG_ScoresDown_f 0 4
ADDRGP4 cg+111352
INDIRI4
CNSTI4 2000
ADDI4
ADDRGP4 cg+107604
INDIRI4
GEI4 $107
ADDRGP4 cg+111352
ADDRGP4 cg+107604
INDIRI4
ASGNI4
ADDRGP4 $113
ARGP4
ADDRGP4 trap_SendClientCommand
CALLV
pop
ADDRGP4 cg+116004
INDIRI4
CNSTI4 0
NEI4 $108
ADDRGP4 cg+116004
CNSTI4 1
ASGNI4
ADDRGP4 cg+111356
CNSTI4 0
ASGNI4
ADDRGP4 $108
JUMPV
LABELV $107
ADDRGP4 cg+116004
CNSTI4 1
ASGNI4
LABELV $108
LABELV $106
endproc CG_ScoresDown_f 0 4
proc CG_ScoresUp_f 0 0
ADDRGP4 cg+116004
INDIRI4
CNSTI4 0
EQI4 $121
ADDRGP4 cg+116004
CNSTI4 0
ASGNI4
ADDRGP4 cg+116012
ADDRGP4 cg+107604
INDIRI4
ASGNI4
LABELV $121
LABELV $120
endproc CG_ScoresUp_f 0 0
proc CG_TellTarget_f 264 20
ADDRLP4 260
ADDRGP4 CG_CrosshairPlayer
CALLI4
ASGNI4
ADDRLP4 0
ADDRLP4 260
INDIRI4
ASGNI4
ADDRLP4 0
INDIRI4
CNSTI4 -1
NEI4 $128
ADDRGP4 $127
JUMPV
LABELV $128
ADDRLP4 132
ARGP4
CNSTI4 128
ARGI4
ADDRGP4 trap_Args
CALLV
pop
ADDRLP4 4
ARGP4
CNSTI4 128
ARGI4
ADDRGP4 $130
ARGP4
ADDRLP4 0
INDIRI4
ARGI4
ADDRLP4 132
ARGP4
ADDRGP4 Com_sprintf
CALLI4
pop
ADDRLP4 4
ARGP4
ADDRGP4 trap_SendClientCommand
CALLV
pop
LABELV $127
endproc CG_TellTarget_f 264 20
proc CG_TellAttacker_f 264 20
ADDRLP4 260
ADDRGP4 CG_LastAttacker
CALLI4
ASGNI4
ADDRLP4 0
ADDRLP4 260
INDIRI4
ASGNI4
ADDRLP4 0
INDIRI4
CNSTI4 -1
NEI4 $132
ADDRGP4 $131
JUMPV
LABELV $132
ADDRLP4 132
ARGP4
CNSTI4 128
ARGI4
ADDRGP4 trap_Args
CALLV
pop
ADDRLP4 4
ARGP4
CNSTI4 128
ARGI4
ADDRGP4 $130
ARGP4
ADDRLP4 0
INDIRI4
ARGI4
ADDRLP4 132
ARGP4
ADDRGP4 Com_sprintf
CALLI4
pop
ADDRLP4 4
ARGP4
ADDRGP4 trap_SendClientCommand
CALLV
pop
LABELV $131
endproc CG_TellAttacker_f 264 20
proc CG_StartOrbit_f 1028 12
ADDRGP4 $135
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
ADDRLP4 1024
ADDRGP4 atoi
CALLI4
ASGNI4
ADDRLP4 1024
INDIRI4
CNSTI4 0
NEI4 $136
ADDRGP4 $134
JUMPV
LABELV $136
ADDRGP4 cg_cameraOrbit+8
INDIRF4
CNSTF4 0
EQF4 $138
ADDRGP4 $141
ARGP4
ADDRGP4 $142
ARGP4
ADDRGP4 trap_Cvar_Set
CALLV
pop
ADDRGP4 $143
ARGP4
ADDRGP4 $142
ARGP4
ADDRGP4 trap_Cvar_Set
CALLV
pop
ADDRGP4 $139
JUMPV
LABELV $138
ADDRGP4 $141
ARGP4
ADDRGP4 $144
ARGP4
ADDRGP4 trap_Cvar_Set
CALLV
pop
ADDRGP4 $143
ARGP4
ADDRGP4 $145
ARGP4
ADDRGP4 trap_Cvar_Set
CALLV
pop
ADDRGP4 $146
ARGP4
ADDRGP4 $142
ARGP4
ADDRGP4 trap_Cvar_Set
CALLV
pop
ADDRGP4 $147
ARGP4
ADDRGP4 $148
ARGP4
ADDRGP4 trap_Cvar_Set
CALLV
pop
LABELV $139
LABELV $134
endproc CG_StartOrbit_f 1028 12
proc CG_HUDDown_f 0 0
ADDRGP4 cg+126160
CNSTI4 1
ASGNI4
LABELV $149
endproc CG_HUDDown_f 0 0
proc CG_HUDUp_f 0 0
ADDRGP4 cg+126160
CNSTI4 0
ASGNI4
LABELV $151
endproc CG_HUDUp_f 0 0
proc CG_ControlMode_f 12 8
ADDRLP4 0
CNSTI4 1868
ADDRGP4 cg+36
INDIRP4
CNSTI4 184
ADDP4
INDIRI4
MULI4
ADDRGP4 cgs+40984+680
ADDP4
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
ADDRLP4 0
INDIRI4
CNSTI4 2
LTI4 $157
ADDRLP4 0
CNSTI4 0
ASGNI4
LABELV $157
ADDRLP4 0
INDIRI4
CNSTI4 0
EQI4 $161
ADDRLP4 0
INDIRI4
CNSTI4 1
EQI4 $164
ADDRGP4 $159
JUMPV
LABELV $161
ADDRGP4 $162
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
ADDRGP4 $163
ARGP4
ADDRGP4 trap_SendConsoleCommand
CALLV
pop
ADDRGP4 $160
JUMPV
LABELV $164
ADDRGP4 $165
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
ADDRGP4 $166
ARGP4
ADDRGP4 trap_SendConsoleCommand
CALLV
pop
LABELV $159
LABELV $160
ADDRGP4 $90
ARGP4
ADDRLP4 0
INDIRI4
ARGI4
ADDRLP4 8
ADDRGP4 va
CALLP4
ASGNP4
ADDRGP4 $167
ARGP4
ADDRLP4 8
INDIRP4
ARGP4
ADDRGP4 trap_Cvar_Set
CALLV
pop
LABELV $153
endproc CG_ControlMode_f 12 8
proc CG_PhysicsDebug_f 16 8
CNSTI4 1
ARGI4
ADDRLP4 0
ADDRGP4 CG_Argv
CALLP4
ASGNP4
ADDRGP4 $169
ARGP4
ADDRLP4 0
INDIRP4
ARGP4
ADDRLP4 4
ADDRGP4 va
CALLP4
ASGNP4
ADDRLP4 4
INDIRP4
ARGP4
ADDRGP4 trap_SendClientCommand
CALLV
pop
CNSTI4 1
ARGI4
ADDRLP4 8
ADDRGP4 CG_Argv
CALLP4
ASGNP4
ADDRLP4 8
INDIRP4
ARGP4
ADDRLP4 12
ADDRGP4 atoi
CALLI4
ASGNI4
ADDRGP4 cg+126156
ADDRLP4 12
INDIRI4
ASGNI4
LABELV $168
endproc CG_PhysicsDebug_f 16 8
proc CG_NextBezierPoint_f 4 0
ADDRLP4 0
ADDRGP4 cg+126168
ASGNP4
ADDRLP4 0
INDIRP4
ADDRLP4 0
INDIRP4
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
LABELV $171
endproc CG_NextBezierPoint_f 4 0
proc CG_PrevBezierPoint_f 4 0
ADDRLP4 0
ADDRGP4 cg+126168
ASGNP4
ADDRLP4 0
INDIRP4
ADDRLP4 0
INDIRP4
INDIRI4
CNSTI4 1
SUBI4
ASGNI4
LABELV $173
endproc CG_PrevBezierPoint_f 4 0
proc CG_MoveBezierPoint_f 292 28
CNSTI4 1
ARGI4
ADDRLP4 268
ADDRGP4 CG_Argv
CALLP4
ASGNP4
ADDRLP4 268
INDIRP4
ARGP4
ADDRLP4 272
ADDRGP4 atof
CALLF4
ASGNF4
ADDRLP4 0
ADDRLP4 272
INDIRF4
ASGNF4
CNSTI4 2
ARGI4
ADDRLP4 276
ADDRGP4 CG_Argv
CALLP4
ASGNP4
ADDRLP4 276
INDIRP4
ARGP4
ADDRLP4 280
ADDRGP4 atof
CALLF4
ASGNF4
ADDRLP4 0+4
ADDRLP4 280
INDIRF4
ASGNF4
CNSTI4 3
ARGI4
ADDRLP4 284
ADDRGP4 CG_Argv
CALLP4
ASGNP4
ADDRLP4 284
INDIRP4
ARGP4
ADDRLP4 288
ADDRGP4 atof
CALLF4
ASGNF4
ADDRLP4 0+8
ADDRLP4 288
INDIRF4
ASGNF4
ADDRLP4 12
ARGP4
CNSTI4 256
ARGI4
ADDRGP4 $178
ARGP4
ADDRGP4 cg+126168
INDIRI4
ARGI4
ADDRLP4 0
INDIRF4
ARGF4
ADDRLP4 0+4
INDIRF4
ARGF4
ADDRLP4 0+8
INDIRF4
ARGF4
ADDRGP4 Com_sprintf
CALLI4
pop
ADDRGP4 $182
ARGP4
ADDRLP4 12
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
ADDRLP4 12
ARGP4
ADDRGP4 trap_SendClientCommand
CALLV
pop
LABELV $175
endproc CG_MoveBezierPoint_f 292 28
proc CG_MoveBezierHandle_f 292 28
CNSTI4 1
ARGI4
ADDRLP4 268
ADDRGP4 CG_Argv
CALLP4
ASGNP4
ADDRLP4 268
INDIRP4
ARGP4
ADDRLP4 272
ADDRGP4 atof
CALLF4
ASGNF4
ADDRLP4 0
ADDRLP4 272
INDIRF4
ASGNF4
CNSTI4 2
ARGI4
ADDRLP4 276
ADDRGP4 CG_Argv
CALLP4
ASGNP4
ADDRLP4 276
INDIRP4
ARGP4
ADDRLP4 280
ADDRGP4 atof
CALLF4
ASGNF4
ADDRLP4 0+4
ADDRLP4 280
INDIRF4
ASGNF4
CNSTI4 3
ARGI4
ADDRLP4 284
ADDRGP4 CG_Argv
CALLP4
ASGNP4
ADDRLP4 284
INDIRP4
ARGP4
ADDRLP4 288
ADDRGP4 atof
CALLF4
ASGNF4
ADDRLP4 0+8
ADDRLP4 288
INDIRF4
ASGNF4
ADDRLP4 12
ARGP4
CNSTI4 256
ARGI4
ADDRGP4 $186
ARGP4
ADDRGP4 cg+126168
INDIRI4
ARGI4
ADDRLP4 0
INDIRF4
ARGF4
ADDRLP4 0+4
INDIRF4
ARGF4
ADDRLP4 0+8
INDIRF4
ARGF4
ADDRGP4 Com_sprintf
CALLI4
pop
ADDRGP4 $182
ARGP4
ADDRLP4 12
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
ADDRLP4 12
ARGP4
ADDRGP4 trap_SendClientCommand
CALLV
pop
LABELV $183
endproc CG_MoveBezierHandle_f 292 28
data
align 4
LABELV commands
address $191
address CG_TestGun_f
address $192
address CG_TestModel_f
address $193
address CG_TestModelNextFrame_f
address $194
address CG_TestModelPrevFrame_f
address $195
address CG_TestModelNextSkin_f
address $196
address CG_TestModelPrevSkin_f
address $197
address CG_Viewpos_f
address $198
address CG_ScoresDown_f
address $199
address CG_ScoresUp_f
address $200
address CG_ZoomDown_f
address $201
address CG_ZoomUp_f
address $202
address CG_SizeUp_f
address $203
address CG_SizeDown_f
address $204
address CG_NextWeapon_f
address $205
address CG_PrevWeapon_f
address $206
address CG_Weapon_f
address $207
address CG_TargetCommand_f
address $208
address CG_TellTarget_f
address $209
address CG_TellAttacker_f
address $210
address CG_HUDDown_f
address $211
address CG_HUDUp_f
address $212
address CG_ControlMode_f
address $213
address CG_PhysicsDebug_f
address $214
address CG_NextBezierPoint_f
address $215
address CG_PrevBezierPoint_f
address $216
address CG_MoveBezierPoint_f
address $217
address CG_MoveBezierHandle_f
address $218
address CG_StartOrbit_f
address $219
address CG_LoadDeferredPlayers
export CG_ConsoleCommand
code
proc CG_ConsoleCommand 16 8
CNSTI4 0
ARGI4
ADDRLP4 8
ADDRGP4 CG_Argv
CALLP4
ASGNP4
ADDRLP4 4
ADDRLP4 8
INDIRP4
ASGNP4
ADDRLP4 0
CNSTI4 0
ASGNI4
ADDRGP4 $224
JUMPV
LABELV $221
ADDRLP4 4
INDIRP4
ARGP4
ADDRLP4 0
INDIRI4
CNSTI4 3
LSHI4
ADDRGP4 commands
ADDP4
INDIRP4
ARGP4
ADDRLP4 12
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 12
INDIRI4
CNSTI4 0
NEI4 $225
ADDRLP4 0
INDIRI4
CNSTI4 3
LSHI4
ADDRGP4 commands+4
ADDP4
INDIRP4
CALLV
pop
CNSTI4 1
RETI4
ADDRGP4 $220
JUMPV
LABELV $225
LABELV $222
ADDRLP4 0
ADDRLP4 0
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
LABELV $224
ADDRLP4 0
INDIRI4
CVIU4 4
CNSTU4 29
LTU4 $221
CNSTI4 0
RETI4
LABELV $220
endproc CG_ConsoleCommand 16 8
export CG_InitConsoleCommands
proc CG_InitConsoleCommands 4 4
ADDRLP4 0
CNSTI4 0
ASGNI4
ADDRGP4 $232
JUMPV
LABELV $229
ADDRLP4 0
INDIRI4
CNSTI4 3
LSHI4
ADDRGP4 commands
ADDP4
INDIRP4
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
LABELV $230
ADDRLP4 0
ADDRLP4 0
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
LABELV $232
ADDRLP4 0
INDIRI4
CVIU4 4
CNSTU4 29
LTU4 $229
ADDRGP4 $233
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $234
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $235
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $236
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $237
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $238
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $239
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $240
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $241
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $242
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $243
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $244
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $245
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $246
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $247
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $248
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $249
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $250
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $251
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $252
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $253
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $254
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
ADDRGP4 $255
ARGP4
ADDRGP4 trap_AddCommand
CALLV
pop
LABELV $228
endproc CG_InitConsoleCommands 4 4
import CG_NewParticleArea
import initparticles
import CG_ParticleExplosion
import CG_ParticleMisc
import CG_ParticleDust
import CG_ParticleSparks
import CG_ParticleBulletDebris
import CG_ParticleSnowFlurry
import CG_AddParticleShrapnel
import CG_ParticleSmoke
import CG_ParticleSnow
import CG_AddParticles
import CG_ClearParticles
import trap_GetEntityToken
import trap_getCameraInfo
import trap_startCamera
import trap_loadCamera
import trap_SnapVector
import trap_RealTime
import trap_CIN_SetExtents
import trap_CIN_DrawCinematic
import trap_CIN_RunCinematic
import trap_CIN_StopCinematic
import trap_CIN_PlayCinematic
import trap_Key_GetKey
import trap_Key_SetCatcher
import trap_Key_GetCatcher
import trap_Key_IsDown
import trap_R_RegisterFont
import trap_MemoryRemaining
import testPrintFloat
import testPrintInt
import trap_SetUserCmdValue
import trap_GetUserCmd
import trap_GetCurrentCmdNumber
import trap_GetServerCommand
import trap_GetSnapshot
import trap_GetCurrentSnapshotNumber
import trap_GetGameState
import trap_GetGlconfig
import trap_R_inPVS
import trap_R_RemapShader
import trap_R_LerpTag
import trap_R_ModelBounds
import trap_R_DrawStretchPic
import trap_R_SetColor
import trap_R_RenderScene
import trap_R_LightForPoint
import trap_R_AddAdditiveLightToScene
import trap_R_AddLightToScene
import trap_R_AddPolysToScene
import trap_R_AddPolyToScene
import trap_R_AddRefEntityToScene
import trap_R_ClearScene
import trap_R_RegisterShaderNoMip
import trap_R_RegisterShader
import trap_R_RegisterSkin
import trap_R_RegisterModel
import trap_R_LoadWorldMap
import trap_S_StopBackgroundTrack
import trap_S_StartBackgroundTrack
import trap_S_RegisterSound
import trap_S_Respatialize
import trap_S_UpdateEntityPosition
import trap_S_AddRealLoopingSound
import trap_S_AddLoopingSound
import trap_S_ClearLoopingSounds
import trap_S_StartLocalSound
import trap_S_StopLoopingSound
import trap_S_StartSound
import trap_CM_MarkFragments
import trap_CM_TransformedCapsuleTrace
import trap_CM_TransformedBoxTrace
import trap_CM_CapsuleTrace
import trap_CM_BoxTrace
import trap_CM_TransformedPointContents
import trap_CM_PointContents
import trap_CM_TempBoxModel
import trap_CM_InlineModel
import trap_CM_NumInlineModels
import trap_CM_LoadMap
import trap_UpdateScreen
import trap_SendClientCommand
import trap_RemoveCommand
import trap_AddCommand
import trap_SendConsoleCommand
import trap_FS_Seek
import trap_FS_FCloseFile
import trap_FS_Write
import trap_FS_Read
import trap_FS_FOpenFile
import trap_Args
import trap_Argv
import trap_Argc
import trap_Cvar_VariableStringBuffer
import trap_Cvar_Set
import trap_Cvar_Update
import trap_Cvar_Register
import trap_Milliseconds
import trap_Error
import trap_Print
import CG_DropBio
import CG_StartFlameTrail
import CG_DropOil
import CG_StartSmokeScreen
import CG_FireRearWeapon
import CreateLicensePlateImage
import propMap
import CG_DrawHUD
import CG_DrawMMap
import CG_DrawLowerLeftHUD
import CG_DrawLowerRightHUD
import CG_DrawUpperRightHUD
import CG_DrawRearviewMirror
import colors
import CG_RaceCountDown
import CG_DrawRaceCountDown
import CG_StartRace
import CG_FinishedRace
import CG_NewLapTime
import CG_InsideBox
import isRaceObserver
import isRallyNonDMRace
import isRallyRace
import Q3DistanceToRL
import Q3VelocityToRL
import CG_Draw3DBezierCurve
import CG_Draw3DLine
import CG_DrawModel
import CG_CopyLevelReflectionImage
import TiedWinner
import GetTeamAtRank
import TeamCount
import CreateSmokeCloudEntity
import CreateFireEntity
import CG_Hazard
import CG_FrictionCalc
import CG_Sparks
import CG_DrawCheckpointLinks
import CG_DrawTinyStringColor
import CG_DrawTinyString
import CG_DrawTinyDigitalStringColor
import CG_DrawTinyDigitalString
import CG_DrawSmallDigitalStringColor
import CG_DrawSmallDigitalString
import CG_DrawGiantDigitalStringColor
import CG_DrawGiantDigitalString
import CG_Scripted_Object
import CG_Atmospheric_SetParticles
import CG_AddAtmosphericEffects
import CG_CheckChangedPredictableEvents
import CG_TransitionPlayerState
import CG_Respawn
import CG_ShaderStateChanged
import CG_SetConfigValues
import CG_ParseServerinfo
import CG_ExecuteNewServerCommands
import CG_DrawOldScoreboard
import CG_DrawInformation
import CG_LoadingClient
import CG_LoadingItem
import CG_LoadingString
import CG_ProcessSnapshots
import CG_MakeExplosion
import CG_Bleed
import CG_BREAKMETAL
import CG_BREAKWOOD
import CG_BreakGlass
import CG_LightningArc
import CG_BigExplode
import CG_GibPlayer
import CG_ParticlesFromEntityState
import CG_Earthquake
import CG_StartEarthquake
import CG_ShowDebris
import CG_ScorePlum
import CG_SpawnEffect
import CG_BubbleTrail
import CG_SmokePuff
import CG_AddLocalEntities
import CG_AllocLocalEntity
import CG_InitLocalEntities
import CG_SkidMark
import CG_ImpactMark2
import CG_ImpactMark
import CG_AddMarks
import CG_InitMarkPolys
import CG_OutOfAmmoChange
import CG_DrawWeaponSelect
import CG_AddPlayerWeapon
import CG_AddViewWeapon
import CG_GrappleTrail
import CG_RailTrail
import CG_Bullet
import CG_ShotgunFire
import CG_MissileHitPlayer
import CG_MissileHitWall
import CG_FireAltWeapon
import CG_FireWeapon
import CG_RegisterItemVisuals
import CG_RegisterWeapon
import CG_Weapon_f
import CG_PrevWeapon_f
import CG_NextWeapon_f
import CG_PositionRotatedEntityOnTag
import CG_PositionEntityOnTag
import CG_GetTagPosition
import CG_TagExists
import CG_AdjustPositionForMover
import CG_Beam
import CG_AddPacketEntities
import CG_SetEntitySoundPosition
import CG_AddCEntity
import CG_PainEvent
import CG_EntityEvent
import CG_PlaceString
import CG_CheckEvents
import CG_LoadDeferredPlayers
import CG_PredictPlayerState
import CG_Trace
import CG_PointContents
import CG_BuildSolidList
import CG_UpdateCarFromPS
import Com_LogPrintf
import CG_CustomSound
import CG_NewClientInfo
import CG_AddRefEntityWithPowerups
import CG_ResetPlayerEntity
import CG_Player
import CG_StatusHandle
import CG_OtherTeamHasFlag
import CG_YourTeamHasFlag
import CG_GameTypeString
import CG_CheckOrderPending
import CG_Text_PaintChar
import CG_Draw3DModel
import CG_GetKillerText
import CG_GetGameStatusText
import CG_GetTeamColor
import CG_InitTeamChat
import CG_SetPrintString
import CG_ShowResponseHead
import CG_RunMenuScript
import CG_OwnerDrawVisible
import CG_GetValue
import CG_SelectNextPlayer
import CG_SelectPrevPlayer
import CG_Text_Height
import CG_Text_Width
import CG_Text_Paint
import CG_OwnerDraw
import CG_DrawTeamBackground
import CG_DrawFlagModel
import CG_DrawActive
import CG_CenterPrint
import CG_AddLagometerSnapshotInfo
import CG_AddLagometerFrameInfo
import CG_DrawScores
import teamChat2
import teamChat1
import systemChat
import drawTeamOverlayModificationCount
import numSortedTeamPlayers
import sortedTeamPlayers
import CG_DrawTopBottom
import CG_DrawSides
import CG_DrawRect
import UI_DrawProportionalString
import CG_GetColorForHealth
import CG_ColorForHealth
import CG_TileClear
import CG_TeamColor
import CG_FadeColor
import CG_DrawStrlen
import CG_DrawSmallStringColor
import CG_DrawSmallString
import CG_DrawBigStringColor
import CG_DrawBigString
import CG_DrawStringExt
import CG_DrawString
import CG_DrawPic
import CG_FillRect
import CG_AdjustFrom640
import CG_DrawActiveFrame
import CG_AddBufferedSound
import CG_ZoomUp_f
import CG_ZoomDown_f
import CG_TestModelPrevSkin_f
import CG_TestModelNextSkin_f
import CG_TestModelPrevFrame_f
import CG_TestModelNextFrame_f
import CG_TestGun_f
import CG_TestModel_f
import CG_BuildSpectatorString
import CG_GetSelectedScore
import CG_SetScoreSelection
import CG_RankRunFrame
import CG_EventHandling
import CG_MouseEvent
import CG_KeyEvent
import CG_LoadMenus
import CG_LastAttacker
import CG_CrosshairPlayer
import CG_UpdateCvars
import CG_StartMusic
import CG_DebugLogPrintf
import CG_Error
import CG_Printf
import CG_Argv
import CG_ConfigString
import cg_drawBotPaths
import cg_engineSoundDelay
import cg_engineSounds
import cg_debugpredict
import cg_mainViewRenderLevel
import cg_rearViewRenderLevel
import cg_tightCamTracking
import cg_drawPositionSprites
import cg_autodrop
import cg_developer
import cg_atmosphericLevel
import cg_checkpointArrowMode
import cg_drawMMap
import cg_drawRearView
import cg_manualShift
import cg_controlMode
import cg_minSkidLength
import cg_metricUnits
import cg_trueLightning
import cg_oldPlasma
import cg_oldRocket
import cg_oldRail
import cg_noProjectileTrail
import cg_cameraMode
import cg_timescale
import cg_timescaleFadeSpeed
import cg_timescaleFadeEnd
import cg_cameraOrbitDelay
import cg_cameraOrbit
import pmove_msec
import pmove_fixed
import cg_smoothClients
import cg_scorePlum
import cg_teamChatsOnly
import cg_drawFriend
import cg_deferPlayers
import cg_predictItems
import cg_blood
import cg_paused
import cg_buildScript
import cg_forceModel
import cg_stats
import cg_teamChatHeight
import cg_teamChatTime
import cg_synchronousClients
import cg_drawAttacker
import cg_lagometer
import cg_thirdPerson
import cg_thirdPersonAngle
import cg_thirdPersonHeight
import cg_thirdPersonRange
import cg_zoomFov
import cg_fov
import cg_simpleItems
import cg_ignore
import cg_autoswitch
import cg_tracerLength
import cg_tracerWidth
import cg_tracerChance
import cg_viewsize
import cg_drawGun
import cg_gun_z
import cg_gun_y
import cg_gun_x
import cg_gun_frame
import cg_brassTime
import cg_addMarks
import cg_footsteps
import cg_showmiss
import cg_noPlayerAnims
import cg_nopredict
import cg_errorDecay
import cg_railTrailTime
import cg_debugEvents
import cg_debugPosition
import cg_debugAnim
import cg_animSpeed
import cg_draw2D
import cg_drawStatus
import cg_crosshairHealth
import cg_crosshairSize
import cg_crosshairY
import cg_crosshairX
import cg_teamOverlayUserinfo
import cg_drawTeamOverlay
import cg_drawRewards
import cg_drawCrosshairNames
import cg_drawCrosshair
import cg_drawAmmoWarning
import cg_drawIcons
import cg_draw3dIcons
import cg_drawSnapshot
import cg_drawFPS
import cg_drawTimer
import cg_gibs
import cg_shadows
import cg_swingSpeed
import cg_bobroll
import cg_bobpitch
import cg_bobup
import cg_runroll
import cg_runpitch
import cg_centertime
import cg_markPolys
import cg_items
import cg_weapons
import cg_entities
import cg
import cgs
import bgColor
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
LABELV $255
byte 1 108
byte 1 111
byte 1 97
byte 1 100
byte 1 100
byte 1 101
byte 1 102
byte 1 101
byte 1 114
byte 1 101
byte 1 100
byte 1 0
align 1
LABELV $254
byte 1 116
byte 1 101
byte 1 97
byte 1 109
byte 1 116
byte 1 97
byte 1 115
byte 1 107
byte 1 0
align 1
LABELV $253
byte 1 115
byte 1 116
byte 1 97
byte 1 116
byte 1 115
byte 1 0
align 1
LABELV $252
byte 1 116
byte 1 101
byte 1 97
byte 1 109
byte 1 118
byte 1 111
byte 1 116
byte 1 101
byte 1 0
align 1
LABELV $251
byte 1 99
byte 1 97
byte 1 108
byte 1 108
byte 1 116
byte 1 101
byte 1 97
byte 1 109
byte 1 118
byte 1 111
byte 1 116
byte 1 101
byte 1 0
align 1
LABELV $250
byte 1 118
byte 1 111
byte 1 116
byte 1 101
byte 1 0
align 1
LABELV $249
byte 1 99
byte 1 97
byte 1 108
byte 1 108
byte 1 118
byte 1 111
byte 1 116
byte 1 101
byte 1 0
align 1
LABELV $248
byte 1 115
byte 1 101
byte 1 116
byte 1 118
byte 1 105
byte 1 101
byte 1 119
byte 1 112
byte 1 111
byte 1 115
byte 1 0
align 1
LABELV $247
byte 1 97
byte 1 100
byte 1 100
byte 1 98
byte 1 111
byte 1 116
byte 1 0
align 1
LABELV $246
byte 1 108
byte 1 101
byte 1 118
byte 1 101
byte 1 108
byte 1 115
byte 1 104
byte 1 111
byte 1 116
byte 1 0
align 1
LABELV $245
byte 1 102
byte 1 111
byte 1 108
byte 1 108
byte 1 111
byte 1 119
byte 1 112
byte 1 114
byte 1 101
byte 1 118
byte 1 0
align 1
LABELV $244
byte 1 102
byte 1 111
byte 1 108
byte 1 108
byte 1 111
byte 1 119
byte 1 110
byte 1 101
byte 1 120
byte 1 116
byte 1 0
align 1
LABELV $243
byte 1 102
byte 1 111
byte 1 108
byte 1 108
byte 1 111
byte 1 119
byte 1 0
align 1
LABELV $242
byte 1 116
byte 1 101
byte 1 97
byte 1 109
byte 1 0
align 1
LABELV $241
byte 1 119
byte 1 104
byte 1 101
byte 1 114
byte 1 101
byte 1 0
align 1
LABELV $240
byte 1 110
byte 1 111
byte 1 99
byte 1 108
byte 1 105
byte 1 112
byte 1 0
align 1
LABELV $239
byte 1 110
byte 1 111
byte 1 116
byte 1 97
byte 1 114
byte 1 103
byte 1 101
byte 1 116
byte 1 0
align 1
LABELV $238
byte 1 103
byte 1 111
byte 1 100
byte 1 0
align 1
LABELV $237
byte 1 103
byte 1 105
byte 1 118
byte 1 101
byte 1 0
align 1
LABELV $236
byte 1 116
byte 1 101
byte 1 108
byte 1 108
byte 1 0
align 1
LABELV $235
byte 1 115
byte 1 97
byte 1 121
byte 1 95
byte 1 116
byte 1 101
byte 1 97
byte 1 109
byte 1 0
align 1
LABELV $234
byte 1 115
byte 1 97
byte 1 121
byte 1 0
align 1
LABELV $233
byte 1 107
byte 1 105
byte 1 108
byte 1 108
byte 1 0
align 1
LABELV $219
byte 1 108
byte 1 111
byte 1 97
byte 1 100
byte 1 100
byte 1 101
byte 1 102
byte 1 101
byte 1 114
byte 1 114
byte 1 101
byte 1 100
byte 1 0
align 1
LABELV $218
byte 1 115
byte 1 116
byte 1 97
byte 1 114
byte 1 116
byte 1 79
byte 1 114
byte 1 98
byte 1 105
byte 1 116
byte 1 0
align 1
LABELV $217
byte 1 109
byte 1 111
byte 1 118
byte 1 101
byte 1 66
byte 1 72
byte 1 97
byte 1 110
byte 1 100
byte 1 108
byte 1 101
byte 1 0
align 1
LABELV $216
byte 1 109
byte 1 111
byte 1 118
byte 1 101
byte 1 66
byte 1 80
byte 1 111
byte 1 105
byte 1 110
byte 1 116
byte 1 0
align 1
LABELV $215
byte 1 112
byte 1 114
byte 1 101
byte 1 118
byte 1 66
byte 1 80
byte 1 111
byte 1 105
byte 1 110
byte 1 116
byte 1 0
align 1
LABELV $214
byte 1 110
byte 1 101
byte 1 120
byte 1 116
byte 1 66
byte 1 80
byte 1 111
byte 1 105
byte 1 110
byte 1 116
byte 1 0
align 1
LABELV $213
byte 1 112
byte 1 68
byte 1 101
byte 1 98
byte 1 117
byte 1 103
byte 1 0
align 1
LABELV $212
byte 1 99
byte 1 111
byte 1 110
byte 1 116
byte 1 114
byte 1 111
byte 1 108
byte 1 77
byte 1 111
byte 1 100
byte 1 101
byte 1 0
align 1
LABELV $211
byte 1 45
byte 1 104
byte 1 117
byte 1 100
byte 1 0
align 1
LABELV $210
byte 1 43
byte 1 104
byte 1 117
byte 1 100
byte 1 0
align 1
LABELV $209
byte 1 116
byte 1 101
byte 1 108
byte 1 108
byte 1 95
byte 1 97
byte 1 116
byte 1 116
byte 1 97
byte 1 99
byte 1 107
byte 1 101
byte 1 114
byte 1 0
align 1
LABELV $208
byte 1 116
byte 1 101
byte 1 108
byte 1 108
byte 1 95
byte 1 116
byte 1 97
byte 1 114
byte 1 103
byte 1 101
byte 1 116
byte 1 0
align 1
LABELV $207
byte 1 116
byte 1 99
byte 1 109
byte 1 100
byte 1 0
align 1
LABELV $206
byte 1 119
byte 1 101
byte 1 97
byte 1 112
byte 1 111
byte 1 110
byte 1 0
align 1
LABELV $205
byte 1 119
byte 1 101
byte 1 97
byte 1 112
byte 1 112
byte 1 114
byte 1 101
byte 1 118
byte 1 0
align 1
LABELV $204
byte 1 119
byte 1 101
byte 1 97
byte 1 112
byte 1 110
byte 1 101
byte 1 120
byte 1 116
byte 1 0
align 1
LABELV $203
byte 1 115
byte 1 105
byte 1 122
byte 1 101
byte 1 100
byte 1 111
byte 1 119
byte 1 110
byte 1 0
align 1
LABELV $202
byte 1 115
byte 1 105
byte 1 122
byte 1 101
byte 1 117
byte 1 112
byte 1 0
align 1
LABELV $201
byte 1 45
byte 1 122
byte 1 111
byte 1 111
byte 1 109
byte 1 0
align 1
LABELV $200
byte 1 43
byte 1 122
byte 1 111
byte 1 111
byte 1 109
byte 1 0
align 1
LABELV $199
byte 1 45
byte 1 115
byte 1 99
byte 1 111
byte 1 114
byte 1 101
byte 1 115
byte 1 0
align 1
LABELV $198
byte 1 43
byte 1 115
byte 1 99
byte 1 111
byte 1 114
byte 1 101
byte 1 115
byte 1 0
align 1
LABELV $197
byte 1 118
byte 1 105
byte 1 101
byte 1 119
byte 1 112
byte 1 111
byte 1 115
byte 1 0
align 1
LABELV $196
byte 1 112
byte 1 114
byte 1 101
byte 1 118
byte 1 115
byte 1 107
byte 1 105
byte 1 110
byte 1 0
align 1
LABELV $195
byte 1 110
byte 1 101
byte 1 120
byte 1 116
byte 1 115
byte 1 107
byte 1 105
byte 1 110
byte 1 0
align 1
LABELV $194
byte 1 112
byte 1 114
byte 1 101
byte 1 118
byte 1 102
byte 1 114
byte 1 97
byte 1 109
byte 1 101
byte 1 0
align 1
LABELV $193
byte 1 110
byte 1 101
byte 1 120
byte 1 116
byte 1 102
byte 1 114
byte 1 97
byte 1 109
byte 1 101
byte 1 0
align 1
LABELV $192
byte 1 116
byte 1 101
byte 1 115
byte 1 116
byte 1 109
byte 1 111
byte 1 100
byte 1 101
byte 1 108
byte 1 0
align 1
LABELV $191
byte 1 116
byte 1 101
byte 1 115
byte 1 116
byte 1 103
byte 1 117
byte 1 110
byte 1 0
align 1
LABELV $186
byte 1 109
byte 1 111
byte 1 118
byte 1 101
byte 1 66
byte 1 72
byte 1 97
byte 1 110
byte 1 100
byte 1 108
byte 1 101
byte 1 32
byte 1 37
byte 1 105
byte 1 32
byte 1 37
byte 1 102
byte 1 32
byte 1 37
byte 1 102
byte 1 32
byte 1 37
byte 1 102
byte 1 0
align 1
LABELV $182
byte 1 83
byte 1 101
byte 1 110
byte 1 100
byte 1 105
byte 1 110
byte 1 103
byte 1 32
byte 1 99
byte 1 111
byte 1 110
byte 1 115
byte 1 111
byte 1 108
byte 1 101
byte 1 32
byte 1 99
byte 1 111
byte 1 109
byte 1 109
byte 1 97
byte 1 110
byte 1 100
byte 1 58
byte 1 32
byte 1 39
byte 1 37
byte 1 115
byte 1 39
byte 1 10
byte 1 0
align 1
LABELV $178
byte 1 109
byte 1 111
byte 1 118
byte 1 101
byte 1 66
byte 1 80
byte 1 111
byte 1 105
byte 1 110
byte 1 116
byte 1 32
byte 1 37
byte 1 105
byte 1 32
byte 1 37
byte 1 102
byte 1 32
byte 1 37
byte 1 102
byte 1 32
byte 1 37
byte 1 102
byte 1 0
align 1
LABELV $169
byte 1 112
byte 1 68
byte 1 101
byte 1 98
byte 1 117
byte 1 103
byte 1 32
byte 1 37
byte 1 115
byte 1 0
align 1
LABELV $167
byte 1 99
byte 1 103
byte 1 95
byte 1 99
byte 1 111
byte 1 110
byte 1 116
byte 1 114
byte 1 111
byte 1 108
byte 1 77
byte 1 111
byte 1 100
byte 1 101
byte 1 0
align 1
LABELV $166
byte 1 43
byte 1 115
byte 1 116
byte 1 114
byte 1 97
byte 1 102
byte 1 101
byte 1 0
align 1
LABELV $165
byte 1 67
byte 1 111
byte 1 110
byte 1 116
byte 1 114
byte 1 111
byte 1 108
byte 1 32
byte 1 77
byte 1 111
byte 1 100
byte 1 101
byte 1 58
byte 1 32
byte 1 74
byte 1 111
byte 1 121
byte 1 115
byte 1 116
byte 1 105
byte 1 99
byte 1 107
byte 1 10
byte 1 0
align 1
LABELV $163
byte 1 45
byte 1 115
byte 1 116
byte 1 114
byte 1 97
byte 1 102
byte 1 101
byte 1 0
align 1
LABELV $162
byte 1 67
byte 1 111
byte 1 110
byte 1 116
byte 1 114
byte 1 111
byte 1 108
byte 1 32
byte 1 77
byte 1 111
byte 1 100
byte 1 101
byte 1 58
byte 1 32
byte 1 77
byte 1 111
byte 1 117
byte 1 115
byte 1 101
byte 1 10
byte 1 0
align 1
LABELV $148
byte 1 49
byte 1 48
byte 1 48
byte 1 0
align 1
LABELV $147
byte 1 99
byte 1 103
byte 1 95
byte 1 116
byte 1 104
byte 1 105
byte 1 114
byte 1 100
byte 1 80
byte 1 101
byte 1 114
byte 1 115
byte 1 111
byte 1 110
byte 1 82
byte 1 97
byte 1 110
byte 1 103
byte 1 101
byte 1 0
align 1
LABELV $146
byte 1 99
byte 1 103
byte 1 95
byte 1 116
byte 1 104
byte 1 105
byte 1 114
byte 1 100
byte 1 80
byte 1 101
byte 1 114
byte 1 115
byte 1 111
byte 1 110
byte 1 65
byte 1 110
byte 1 103
byte 1 108
byte 1 101
byte 1 0
align 1
LABELV $145
byte 1 49
byte 1 0
align 1
LABELV $144
byte 1 53
byte 1 0
align 1
LABELV $143
byte 1 99
byte 1 103
byte 1 95
byte 1 116
byte 1 104
byte 1 105
byte 1 114
byte 1 100
byte 1 80
byte 1 101
byte 1 114
byte 1 115
byte 1 111
byte 1 110
byte 1 0
align 1
LABELV $142
byte 1 48
byte 1 0
align 1
LABELV $141
byte 1 99
byte 1 103
byte 1 95
byte 1 99
byte 1 97
byte 1 109
byte 1 101
byte 1 114
byte 1 97
byte 1 79
byte 1 114
byte 1 98
byte 1 105
byte 1 116
byte 1 0
align 1
LABELV $135
byte 1 100
byte 1 101
byte 1 118
byte 1 101
byte 1 108
byte 1 111
byte 1 112
byte 1 101
byte 1 114
byte 1 0
align 1
LABELV $130
byte 1 116
byte 1 101
byte 1 108
byte 1 108
byte 1 32
byte 1 37
byte 1 105
byte 1 32
byte 1 37
byte 1 115
byte 1 0
align 1
LABELV $113
byte 1 115
byte 1 99
byte 1 111
byte 1 114
byte 1 101
byte 1 0
align 1
LABELV $95
byte 1 40
byte 1 37
byte 1 105
byte 1 32
byte 1 37
byte 1 105
byte 1 32
byte 1 37
byte 1 105
byte 1 41
byte 1 32
byte 1 58
byte 1 32
byte 1 37
byte 1 105
byte 1 10
byte 1 0
align 1
LABELV $90
byte 1 37
byte 1 105
byte 1 0
align 1
LABELV $89
byte 1 99
byte 1 103
byte 1 95
byte 1 118
byte 1 105
byte 1 101
byte 1 119
byte 1 115
byte 1 105
byte 1 122
byte 1 101
byte 1 0
align 1
LABELV $87
byte 1 103
byte 1 99
byte 1 32
byte 1 37
byte 1 105
byte 1 32
byte 1 37
byte 1 105
byte 1 0
