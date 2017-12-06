export isRaceObserver
code
proc isRaceObserver 0 0
CNSTI4 0
RETI4
LABELV $80
endproc isRaceObserver 0 0
data
align 4
LABELV q3roptions_units
address $82
address $83
byte 4 0
align 4
LABELV q3roptions_control_mode
address $84
address $85
byte 4 0
align 4
LABELV q3roptions_cp_arrow_mode
address $86
address $87
address $88
byte 4 0
align 4
LABELV q3roptions_atmospheric
address $89
address $90
address $91
byte 4 0
code
proc Q3ROptions_MenuEvent 60 8
ADDRFP4 4
INDIRI4
CNSTI4 3
EQI4 $93
ADDRGP4 $92
JUMPV
LABELV $93
ADDRLP4 4
ADDRFP4 0
INDIRP4
CNSTI4 8
ADDP4
INDIRI4
ASGNI4
ADDRLP4 4
INDIRI4
CNSTI4 10
LTI4 $95
ADDRLP4 4
INDIRI4
CNSTI4 40
GTI4 $95
ADDRLP4 4
INDIRI4
CNSTI4 2
LSHI4
ADDRGP4 $199-40
ADDP4
INDIRP4
JUMPV
lit
align 4
LABELV $199
address $98
address $132
address $106
address $120
address $124
address $102
address $116
address $128
address $136
address $140
address $144
address $144
address $144
address $144
address $144
address $95
address $95
address $95
address $95
address $95
address $171
address $171
address $171
address $171
address $171
address $95
address $95
address $95
address $95
address $95
address $198
code
LABELV $98
ADDRGP4 $99
ARGP4
ADDRGP4 s_q3roptions+632+64
INDIRI4
CVIF4 4
ARGF4
ADDRGP4 trap_Cvar_SetValue
CALLV
pop
ADDRGP4 $96
JUMPV
LABELV $102
ADDRGP4 $103
ARGP4
ADDRGP4 s_q3roptions+736+64
INDIRI4
CVIF4 4
ARGF4
ADDRGP4 trap_Cvar_SetValue
CALLV
pop
ADDRGP4 $96
JUMPV
LABELV $106
ADDRGP4 $107
ARGP4
ADDRGP4 s_q3roptions+840+64
INDIRI4
CVIF4 4
ARGF4
ADDRGP4 trap_Cvar_SetValue
CALLV
pop
ADDRGP4 s_q3roptions+840+64
INDIRI4
CNSTI4 0
NEI4 $110
CNSTI4 2
ARGI4
ADDRGP4 $114
ARGP4
ADDRGP4 trap_Cmd_ExecuteText
CALLV
pop
ADDRGP4 $96
JUMPV
LABELV $110
CNSTI4 2
ARGI4
ADDRGP4 $115
ARGP4
ADDRGP4 trap_Cmd_ExecuteText
CALLV
pop
ADDRGP4 $96
JUMPV
LABELV $116
ADDRGP4 $117
ARGP4
ADDRGP4 s_q3roptions+944+64
INDIRI4
CVIF4 4
ARGF4
ADDRGP4 trap_Cvar_SetValue
CALLV
pop
ADDRGP4 $96
JUMPV
LABELV $120
ADDRGP4 $121
ARGP4
ADDRGP4 s_q3roptions+1048+60
INDIRI4
CVIF4 4
ARGF4
ADDRGP4 trap_Cvar_SetValue
CALLV
pop
ADDRGP4 $96
JUMPV
LABELV $124
ADDRGP4 $125
ARGP4
ADDRGP4 s_q3roptions+1112+60
INDIRI4
CVIF4 4
ARGF4
ADDRGP4 trap_Cvar_SetValue
CALLV
pop
ADDRGP4 $96
JUMPV
LABELV $128
ADDRGP4 $129
ARGP4
ADDRGP4 s_q3roptions+1176+60
INDIRI4
CVIF4 4
ARGF4
ADDRGP4 trap_Cvar_SetValue
CALLV
pop
ADDRGP4 $96
JUMPV
LABELV $132
ADDRGP4 $133
ARGP4
ADDRGP4 s_q3roptions+1240+68
INDIRF4
ARGF4
ADDRGP4 trap_Cvar_SetValue
CALLV
pop
ADDRGP4 $96
JUMPV
LABELV $136
ADDRGP4 $137
ARGP4
ADDRGP4 s_q3roptions+1316+68
INDIRF4
ARGF4
ADDRGP4 trap_Cvar_SetValue
CALLV
pop
ADDRGP4 $96
JUMPV
LABELV $140
ADDRGP4 $141
ARGP4
ADDRGP4 s_q3roptions+2176+60
INDIRI4
CVIF4 4
ARGF4
ADDRGP4 trap_Cvar_SetValue
CALLV
pop
ADDRGP4 $96
JUMPV
LABELV $144
ADDRLP4 32
CNSTI4 0
ASGNI4
ADDRLP4 0
ADDRLP4 32
INDIRI4
ASGNI4
ADDRGP4 s_q3roptions+1392+60
INDIRI4
ADDRLP4 32
INDIRI4
EQI4 $148
ADDRLP4 12
CNSTI4 1
ASGNI4
ADDRGP4 $149
JUMPV
LABELV $148
ADDRLP4 12
CNSTI4 0
ASGNI4
LABELV $149
ADDRLP4 0
ADDRLP4 0
INDIRI4
ADDRLP4 12
INDIRI4
BORI4
ASGNI4
ADDRGP4 s_q3roptions+1456+60
INDIRI4
CNSTI4 0
EQI4 $153
ADDRLP4 16
CNSTI4 2
ASGNI4
ADDRGP4 $154
JUMPV
LABELV $153
ADDRLP4 16
CNSTI4 0
ASGNI4
LABELV $154
ADDRLP4 0
ADDRLP4 0
INDIRI4
ADDRLP4 16
INDIRI4
BORI4
ASGNI4
ADDRGP4 s_q3roptions+1520+60
INDIRI4
CNSTI4 0
EQI4 $158
ADDRLP4 20
CNSTI4 4
ASGNI4
ADDRGP4 $159
JUMPV
LABELV $158
ADDRLP4 20
CNSTI4 0
ASGNI4
LABELV $159
ADDRLP4 0
ADDRLP4 0
INDIRI4
ADDRLP4 20
INDIRI4
BORI4
ASGNI4
ADDRGP4 s_q3roptions+1584+60
INDIRI4
CNSTI4 0
EQI4 $163
ADDRLP4 24
CNSTI4 8
ASGNI4
ADDRGP4 $164
JUMPV
LABELV $163
ADDRLP4 24
CNSTI4 0
ASGNI4
LABELV $164
ADDRLP4 0
ADDRLP4 0
INDIRI4
ADDRLP4 24
INDIRI4
BORI4
ASGNI4
ADDRGP4 s_q3roptions+1648+60
INDIRI4
CNSTI4 0
EQI4 $168
ADDRLP4 28
CNSTI4 16
ASGNI4
ADDRGP4 $169
JUMPV
LABELV $168
ADDRLP4 28
CNSTI4 0
ASGNI4
LABELV $169
ADDRLP4 0
ADDRLP4 0
INDIRI4
ADDRLP4 28
INDIRI4
BORI4
ASGNI4
ADDRGP4 $170
ARGP4
ADDRLP4 0
INDIRI4
CVIF4 4
ARGF4
ADDRGP4 trap_Cvar_SetValue
CALLV
pop
ADDRGP4 $96
JUMPV
LABELV $171
ADDRLP4 56
CNSTI4 0
ASGNI4
ADDRLP4 0
ADDRLP4 56
INDIRI4
ASGNI4
ADDRGP4 s_q3roptions+1712+60
INDIRI4
ADDRLP4 56
INDIRI4
EQI4 $175
ADDRLP4 36
CNSTI4 1
ASGNI4
ADDRGP4 $176
JUMPV
LABELV $175
ADDRLP4 36
CNSTI4 0
ASGNI4
LABELV $176
ADDRLP4 0
ADDRLP4 0
INDIRI4
ADDRLP4 36
INDIRI4
BORI4
ASGNI4
ADDRGP4 s_q3roptions+1776+60
INDIRI4
CNSTI4 0
EQI4 $180
ADDRLP4 40
CNSTI4 2
ASGNI4
ADDRGP4 $181
JUMPV
LABELV $180
ADDRLP4 40
CNSTI4 0
ASGNI4
LABELV $181
ADDRLP4 0
ADDRLP4 0
INDIRI4
ADDRLP4 40
INDIRI4
BORI4
ASGNI4
ADDRGP4 s_q3roptions+1840+60
INDIRI4
CNSTI4 0
EQI4 $185
ADDRLP4 44
CNSTI4 4
ASGNI4
ADDRGP4 $186
JUMPV
LABELV $185
ADDRLP4 44
CNSTI4 0
ASGNI4
LABELV $186
ADDRLP4 0
ADDRLP4 0
INDIRI4
ADDRLP4 44
INDIRI4
BORI4
ASGNI4
ADDRGP4 s_q3roptions+1904+60
INDIRI4
CNSTI4 0
EQI4 $190
ADDRLP4 48
CNSTI4 8
ASGNI4
ADDRGP4 $191
JUMPV
LABELV $190
ADDRLP4 48
CNSTI4 0
ASGNI4
LABELV $191
ADDRLP4 0
ADDRLP4 0
INDIRI4
ADDRLP4 48
INDIRI4
BORI4
ASGNI4
ADDRGP4 s_q3roptions+1968+60
INDIRI4
CNSTI4 0
EQI4 $195
ADDRLP4 52
CNSTI4 16
ASGNI4
ADDRGP4 $196
JUMPV
LABELV $195
ADDRLP4 52
CNSTI4 0
ASGNI4
LABELV $196
ADDRLP4 0
ADDRLP4 0
INDIRI4
ADDRLP4 52
INDIRI4
BORI4
ASGNI4
ADDRGP4 $197
ARGP4
ADDRLP4 0
INDIRI4
CVIF4 4
ARGF4
ADDRGP4 trap_Cvar_SetValue
CALLV
pop
ADDRGP4 $96
JUMPV
LABELV $198
ADDRGP4 UI_PopMenu
CALLV
pop
LABELV $95
LABELV $96
LABELV $92
endproc Q3ROptions_MenuEvent 60 8
proc Q3ROptions_StatusBar 12 20
ADDRLP4 0
ADDRGP4 $202
ASGNP4
ADDRLP4 4
ADDRFP4 0
INDIRP4
CNSTI4 8
ADDP4
INDIRI4
ASGNI4
ADDRLP4 4
INDIRI4
CNSTI4 10
LTI4 $203
ADDRLP4 4
INDIRI4
CNSTI4 34
GTI4 $203
ADDRLP4 4
INDIRI4
CNSTI4 2
LSHI4
ADDRGP4 $256-40
ADDP4
INDIRP4
JUMPV
lit
align 4
LABELV $256
address $206
address $230
address $210
address $219
address $226
address $208
address $217
address $228
address $232
address $234
address $236
address $238
address $240
address $242
address $244
address $203
address $203
address $203
address $203
address $203
address $246
address $248
address $250
address $252
address $254
code
LABELV $206
ADDRLP4 0
ADDRGP4 $207
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $208
ADDRLP4 0
ADDRGP4 $209
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $210
ADDRGP4 s_q3roptions+840+64
INDIRI4
CNSTI4 0
NEI4 $211
ADDRLP4 0
ADDRGP4 $215
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $211
ADDRLP4 0
ADDRGP4 $216
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $217
ADDRLP4 0
ADDRGP4 $218
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $219
ADDRGP4 s_q3roptions+1048+60
INDIRI4
CNSTI4 0
NEI4 $220
ADDRLP4 0
ADDRGP4 $224
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $220
ADDRLP4 0
ADDRGP4 $225
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $226
ADDRLP4 0
ADDRGP4 $227
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $228
ADDRLP4 0
ADDRGP4 $229
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $230
ADDRLP4 0
ADDRGP4 $231
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $232
ADDRLP4 0
ADDRGP4 $233
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $234
ADDRLP4 0
ADDRGP4 $235
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $236
ADDRLP4 0
ADDRGP4 $237
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $238
ADDRLP4 0
ADDRGP4 $239
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $240
ADDRLP4 0
ADDRGP4 $241
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $242
ADDRLP4 0
ADDRGP4 $243
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $244
ADDRLP4 0
ADDRGP4 $245
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $246
ADDRLP4 0
ADDRGP4 $247
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $248
ADDRLP4 0
ADDRGP4 $249
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $250
ADDRLP4 0
ADDRGP4 $251
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $252
ADDRLP4 0
ADDRGP4 $253
ASGNP4
ADDRGP4 $204
JUMPV
LABELV $254
ADDRLP4 0
ADDRGP4 $255
ASGNP4
LABELV $203
LABELV $204
CNSTI4 320
ARGI4
CNSTI4 408
ARGI4
ADDRLP4 0
INDIRP4
ARGP4
CNSTI4 17
ARGI4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 UI_DrawString
CALLV
pop
LABELV $201
endproc Q3ROptions_StatusBar 12 20
export Q3ROptions_MenuInit
proc Q3ROptions_MenuInit 40 12
ADDRGP4 s_q3roptions
ARGP4
CNSTI4 0
ARGI4
CNSTU4 2312
ARGU4
ADDRGP4 memset
CALLP4
pop
ADDRGP4 s_q3roptions+532
CNSTI4 1
ASGNI4
ADDRGP4 s_q3roptions+536
CNSTI4 1
ASGNI4
ADDRGP4 s_q3roptions+632+64
ADDRGP4 ui_metricUnits+12
INDIRI4
ASGNI4
ADDRGP4 s_q3roptions+736+64
ADDRGP4 ui_checkpointArrowMode+12
INDIRI4
ASGNI4
ADDRGP4 s_q3roptions+840+64
ADDRGP4 ui_controlMode+12
INDIRI4
ASGNI4
ADDRGP4 s_q3roptions+944+64
ADDRGP4 ui_atmosphericLevel+12
INDIRI4
ASGNI4
ADDRGP4 s_q3roptions+1048+60
ADDRGP4 ui_manualShift+12
INDIRI4
ASGNI4
ADDRGP4 s_q3roptions+1112+60
ADDRGP4 ui_drawRearView+12
INDIRI4
ASGNI4
ADDRGP4 s_q3roptions+1176+60
ADDRGP4 ui_drawPositionSprites+12
INDIRI4
ASGNI4
ADDRGP4 s_q3roptions+1240+68
ADDRGP4 ui_minSkidLength+12
INDIRI4
CVIF4 4
ASGNF4
ADDRGP4 s_q3roptions+1316+68
ADDRGP4 ui_tightCamTracking+12
INDIRI4
CVIF4 4
ASGNF4
ADDRGP4 s_q3roptions+2176+60
ADDRGP4 ui_engineSounds+12
INDIRI4
ASGNI4
ADDRGP4 ui_rearViewRenderLevel+12
INDIRI4
CNSTI4 1
BANDI4
CNSTI4 0
EQI4 $295
ADDRLP4 0
CNSTI4 1
ASGNI4
ADDRGP4 $296
JUMPV
LABELV $295
ADDRLP4 0
CNSTI4 0
ASGNI4
LABELV $296
ADDRGP4 s_q3roptions+1392+60
ADDRLP4 0
INDIRI4
ASGNI4
ADDRGP4 ui_rearViewRenderLevel+12
INDIRI4
CNSTI4 2
BANDI4
CNSTI4 0
EQI4 $301
ADDRLP4 4
CNSTI4 1
ASGNI4
ADDRGP4 $302
JUMPV
LABELV $301
ADDRLP4 4
CNSTI4 0
ASGNI4
LABELV $302
ADDRGP4 s_q3roptions+1456+60
ADDRLP4 4
INDIRI4
ASGNI4
ADDRGP4 ui_rearViewRenderLevel+12
INDIRI4
CNSTI4 4
BANDI4
CNSTI4 0
EQI4 $307
ADDRLP4 8
CNSTI4 1
ASGNI4
ADDRGP4 $308
JUMPV
LABELV $307
ADDRLP4 8
CNSTI4 0
ASGNI4
LABELV $308
ADDRGP4 s_q3roptions+1520+60
ADDRLP4 8
INDIRI4
ASGNI4
ADDRGP4 ui_rearViewRenderLevel+12
INDIRI4
CNSTI4 8
BANDI4
CNSTI4 0
EQI4 $313
ADDRLP4 12
CNSTI4 1
ASGNI4
ADDRGP4 $314
JUMPV
LABELV $313
ADDRLP4 12
CNSTI4 0
ASGNI4
LABELV $314
ADDRGP4 s_q3roptions+1584+60
ADDRLP4 12
INDIRI4
ASGNI4
ADDRGP4 ui_rearViewRenderLevel+12
INDIRI4
CNSTI4 16
BANDI4
CNSTI4 0
EQI4 $319
ADDRLP4 16
CNSTI4 1
ASGNI4
ADDRGP4 $320
JUMPV
LABELV $319
ADDRLP4 16
CNSTI4 0
ASGNI4
LABELV $320
ADDRGP4 s_q3roptions+1648+60
ADDRLP4 16
INDIRI4
ASGNI4
ADDRGP4 ui_mainViewRenderLevel+12
INDIRI4
CNSTI4 1
BANDI4
CNSTI4 0
EQI4 $325
ADDRLP4 20
CNSTI4 1
ASGNI4
ADDRGP4 $326
JUMPV
LABELV $325
ADDRLP4 20
CNSTI4 0
ASGNI4
LABELV $326
ADDRGP4 s_q3roptions+1712+60
ADDRLP4 20
INDIRI4
ASGNI4
ADDRGP4 ui_mainViewRenderLevel+12
INDIRI4
CNSTI4 2
BANDI4
CNSTI4 0
EQI4 $331
ADDRLP4 24
CNSTI4 1
ASGNI4
ADDRGP4 $332
JUMPV
LABELV $331
ADDRLP4 24
CNSTI4 0
ASGNI4
LABELV $332
ADDRGP4 s_q3roptions+1776+60
ADDRLP4 24
INDIRI4
ASGNI4
ADDRGP4 ui_mainViewRenderLevel+12
INDIRI4
CNSTI4 4
BANDI4
CNSTI4 0
EQI4 $337
ADDRLP4 28
CNSTI4 1
ASGNI4
ADDRGP4 $338
JUMPV
LABELV $337
ADDRLP4 28
CNSTI4 0
ASGNI4
LABELV $338
ADDRGP4 s_q3roptions+1840+60
ADDRLP4 28
INDIRI4
ASGNI4
ADDRGP4 ui_mainViewRenderLevel+12
INDIRI4
CNSTI4 8
BANDI4
CNSTI4 0
EQI4 $343
ADDRLP4 32
CNSTI4 1
ASGNI4
ADDRGP4 $344
JUMPV
LABELV $343
ADDRLP4 32
CNSTI4 0
ASGNI4
LABELV $344
ADDRGP4 s_q3roptions+1904+60
ADDRLP4 32
INDIRI4
ASGNI4
ADDRGP4 ui_mainViewRenderLevel+12
INDIRI4
CNSTI4 16
BANDI4
CNSTI4 0
EQI4 $349
ADDRLP4 36
CNSTI4 1
ASGNI4
ADDRGP4 $350
JUMPV
LABELV $349
ADDRLP4 36
CNSTI4 0
ASGNI4
LABELV $350
ADDRGP4 s_q3roptions+1968+60
ADDRLP4 36
INDIRI4
ASGNI4
ADDRGP4 s_q3roptions+560
CNSTI4 10
ASGNI4
ADDRGP4 s_q3roptions+560+44
CNSTU4 8
ASGNU4
ADDRGP4 s_q3roptions+560+12
CNSTI4 320
ASGNI4
ADDRGP4 s_q3roptions+560+16
CNSTI4 16
ASGNI4
ADDRGP4 s_q3roptions+560+60
ADDRGP4 $360
ASGNP4
ADDRGP4 s_q3roptions+560+68
ADDRGP4 color_white
ASGNP4
ADDRGP4 s_q3roptions+560+64
CNSTI4 1
ASGNI4
ADDRGP4 s_q3roptions+632
CNSTI4 3
ASGNI4
ADDRGP4 s_q3roptions+632+4
ADDRGP4 $368
ASGNP4
ADDRGP4 s_q3roptions+632+44
CNSTU4 258
ASGNU4
ADDRGP4 s_q3roptions+632+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+632+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+632+8
CNSTI4 10
ASGNI4
ADDRGP4 s_q3roptions+632+12
CNSTI4 200
ASGNI4
ADDRGP4 s_q3roptions+632+16
CNSTI4 90
ASGNI4
ADDRGP4 s_q3roptions+632+76
ADDRGP4 q3roptions_units
ASGNP4
ADDRGP4 s_q3roptions+736
CNSTI4 3
ASGNI4
ADDRGP4 s_q3roptions+736+4
ADDRGP4 $386
ASGNP4
ADDRGP4 s_q3roptions+736+44
CNSTU4 258
ASGNU4
ADDRGP4 s_q3roptions+736+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+736+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+736+8
CNSTI4 15
ASGNI4
ADDRGP4 s_q3roptions+736+12
CNSTI4 200
ASGNI4
ADDRGP4 s_q3roptions+736+16
CNSTI4 110
ASGNI4
ADDRGP4 s_q3roptions+736+76
ADDRGP4 q3roptions_cp_arrow_mode
ASGNP4
ADDRGP4 s_q3roptions+840
CNSTI4 3
ASGNI4
ADDRGP4 s_q3roptions+840+4
ADDRGP4 $404
ASGNP4
ADDRGP4 s_q3roptions+840+44
CNSTU4 258
ASGNU4
ADDRGP4 s_q3roptions+840+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+840+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+840+8
CNSTI4 12
ASGNI4
ADDRGP4 s_q3roptions+840+12
CNSTI4 200
ASGNI4
ADDRGP4 s_q3roptions+840+16
CNSTI4 130
ASGNI4
ADDRGP4 s_q3roptions+840+76
ADDRGP4 q3roptions_control_mode
ASGNP4
ADDRGP4 s_q3roptions+944
CNSTI4 3
ASGNI4
ADDRGP4 s_q3roptions+944+4
ADDRGP4 $422
ASGNP4
ADDRGP4 s_q3roptions+944+44
CNSTU4 258
ASGNU4
ADDRGP4 s_q3roptions+944+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+944+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+944+8
CNSTI4 16
ASGNI4
ADDRGP4 s_q3roptions+944+12
CNSTI4 200
ASGNI4
ADDRGP4 s_q3roptions+944+16
CNSTI4 150
ASGNI4
ADDRGP4 s_q3roptions+944+76
ADDRGP4 q3roptions_atmospheric
ASGNP4
ADDRGP4 s_q3roptions+1048
CNSTI4 5
ASGNI4
ADDRGP4 s_q3roptions+1048+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1048+12
CNSTI4 500
ASGNI4
ADDRGP4 s_q3roptions+1048+16
CNSTI4 90
ASGNI4
ADDRGP4 s_q3roptions+1048+4
ADDRGP4 $446
ASGNP4
ADDRGP4 s_q3roptions+1048+8
CNSTI4 13
ASGNI4
ADDRGP4 s_q3roptions+1048+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1048+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+1112
CNSTI4 5
ASGNI4
ADDRGP4 s_q3roptions+1112+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1112+12
CNSTI4 500
ASGNI4
ADDRGP4 s_q3roptions+1112+16
CNSTI4 110
ASGNI4
ADDRGP4 s_q3roptions+1112+4
ADDRGP4 $462
ASGNP4
ADDRGP4 s_q3roptions+1112+8
CNSTI4 14
ASGNI4
ADDRGP4 s_q3roptions+1112+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1112+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+1176
CNSTI4 5
ASGNI4
ADDRGP4 s_q3roptions+1176+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1176+12
CNSTI4 500
ASGNI4
ADDRGP4 s_q3roptions+1176+16
CNSTI4 130
ASGNI4
ADDRGP4 s_q3roptions+1176+4
ADDRGP4 $478
ASGNP4
ADDRGP4 s_q3roptions+1176+8
CNSTI4 17
ASGNI4
ADDRGP4 s_q3roptions+1176+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1176+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+2176
CNSTI4 5
ASGNI4
ADDRGP4 s_q3roptions+2176+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+2176+12
CNSTI4 500
ASGNI4
ADDRGP4 s_q3roptions+2176+16
CNSTI4 150
ASGNI4
ADDRGP4 s_q3roptions+2176+4
ADDRGP4 $494
ASGNP4
ADDRGP4 s_q3roptions+2176+8
CNSTI4 19
ASGNI4
ADDRGP4 s_q3roptions+2176+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+2176+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+1240
CNSTI4 1
ASGNI4
ADDRGP4 s_q3roptions+1240+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1240+12
CNSTI4 200
ASGNI4
ADDRGP4 s_q3roptions+1240+16
CNSTI4 200
ASGNI4
ADDRGP4 s_q3roptions+1240+4
ADDRGP4 $510
ASGNP4
ADDRGP4 s_q3roptions+1240+8
CNSTI4 11
ASGNI4
ADDRGP4 s_q3roptions+1240+60
CNSTF4 1082130432
ASGNF4
ADDRGP4 s_q3roptions+1240+64
CNSTF4 1103101952
ASGNF4
ADDRGP4 s_q3roptions+1240+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1240+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+1316
CNSTI4 1
ASGNI4
ADDRGP4 s_q3roptions+1316+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1316+12
CNSTI4 500
ASGNI4
ADDRGP4 s_q3roptions+1316+16
CNSTI4 200
ASGNI4
ADDRGP4 s_q3roptions+1316+4
ADDRGP4 $530
ASGNP4
ADDRGP4 s_q3roptions+1316+8
CNSTI4 18
ASGNI4
ADDRGP4 s_q3roptions+1316+60
CNSTF4 0
ASGNF4
ADDRGP4 s_q3roptions+1316+64
CNSTF4 1084227584
ASGNF4
ADDRGP4 s_q3roptions+1316+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1316+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+2032
CNSTI4 9
ASGNI4
ADDRGP4 s_q3roptions+2032+44
CNSTU4 16392
ASGNU4
ADDRGP4 s_q3roptions+2032+12
CNSTI4 180
ASGNI4
ADDRGP4 s_q3roptions+2032+16
CNSTI4 240
ASGNI4
ADDRGP4 s_q3roptions+2032+8
CNSTI4 0
ASGNI4
ADDRGP4 s_q3roptions+2032+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+2032+60
ADDRGP4 $554
ASGNP4
ADDRGP4 s_q3roptions+2032+68
ADDRGP4 text_color_normal
ASGNP4
ADDRGP4 s_q3roptions+2032+64
CNSTI4 17
ASGNI4
ADDRGP4 s_q3roptions+1392
CNSTI4 5
ASGNI4
ADDRGP4 s_q3roptions+1392+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1392+12
CNSTI4 200
ASGNI4
ADDRGP4 s_q3roptions+1392+16
CNSTI4 260
ASGNI4
ADDRGP4 s_q3roptions+1392+4
ADDRGP4 $568
ASGNP4
ADDRGP4 s_q3roptions+1392+8
CNSTI4 20
ASGNI4
ADDRGP4 s_q3roptions+1392+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1392+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+1456
CNSTI4 5
ASGNI4
ADDRGP4 s_q3roptions+1456+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1456+12
CNSTI4 200
ASGNI4
ADDRGP4 s_q3roptions+1456+16
CNSTI4 280
ASGNI4
ADDRGP4 s_q3roptions+1456+4
ADDRGP4 $584
ASGNP4
ADDRGP4 s_q3roptions+1456+8
CNSTI4 21
ASGNI4
ADDRGP4 s_q3roptions+1456+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1456+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+1520
CNSTI4 5
ASGNI4
ADDRGP4 s_q3roptions+1520+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1520+12
CNSTI4 200
ASGNI4
ADDRGP4 s_q3roptions+1520+16
CNSTI4 300
ASGNI4
ADDRGP4 s_q3roptions+1520+4
ADDRGP4 $600
ASGNP4
ADDRGP4 s_q3roptions+1520+8
CNSTI4 22
ASGNI4
ADDRGP4 s_q3roptions+1520+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1520+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+1584
CNSTI4 5
ASGNI4
ADDRGP4 s_q3roptions+1584+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1584+12
CNSTI4 200
ASGNI4
ADDRGP4 s_q3roptions+1584+16
CNSTI4 320
ASGNI4
ADDRGP4 s_q3roptions+1584+4
ADDRGP4 $616
ASGNP4
ADDRGP4 s_q3roptions+1584+8
CNSTI4 23
ASGNI4
ADDRGP4 s_q3roptions+1584+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1584+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+1648
CNSTI4 5
ASGNI4
ADDRGP4 s_q3roptions+1648+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1648+12
CNSTI4 200
ASGNI4
ADDRGP4 s_q3roptions+1648+16
CNSTI4 340
ASGNI4
ADDRGP4 s_q3roptions+1648+4
ADDRGP4 $632
ASGNP4
ADDRGP4 s_q3roptions+1648+8
CNSTI4 24
ASGNI4
ADDRGP4 s_q3roptions+1648+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1648+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+2104
CNSTI4 9
ASGNI4
ADDRGP4 s_q3roptions+2104+44
CNSTU4 16392
ASGNU4
ADDRGP4 s_q3roptions+2104+12
CNSTI4 480
ASGNI4
ADDRGP4 s_q3roptions+2104+16
CNSTI4 240
ASGNI4
ADDRGP4 s_q3roptions+2104+8
CNSTI4 0
ASGNI4
ADDRGP4 s_q3roptions+2104+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+2104+60
ADDRGP4 $652
ASGNP4
ADDRGP4 s_q3roptions+2104+68
ADDRGP4 text_color_normal
ASGNP4
ADDRGP4 s_q3roptions+2104+64
CNSTI4 17
ASGNI4
ADDRGP4 s_q3roptions+1712
CNSTI4 5
ASGNI4
ADDRGP4 s_q3roptions+1712+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1712+12
CNSTI4 500
ASGNI4
ADDRGP4 s_q3roptions+1712+16
CNSTI4 260
ASGNI4
ADDRGP4 s_q3roptions+1712+4
ADDRGP4 $568
ASGNP4
ADDRGP4 s_q3roptions+1712+8
CNSTI4 30
ASGNI4
ADDRGP4 s_q3roptions+1712+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1712+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+1776
CNSTI4 5
ASGNI4
ADDRGP4 s_q3roptions+1776+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1776+12
CNSTI4 500
ASGNI4
ADDRGP4 s_q3roptions+1776+16
CNSTI4 280
ASGNI4
ADDRGP4 s_q3roptions+1776+4
ADDRGP4 $584
ASGNP4
ADDRGP4 s_q3roptions+1776+8
CNSTI4 31
ASGNI4
ADDRGP4 s_q3roptions+1776+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1776+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+1840
CNSTI4 5
ASGNI4
ADDRGP4 s_q3roptions+1840+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1840+12
CNSTI4 500
ASGNI4
ADDRGP4 s_q3roptions+1840+16
CNSTI4 300
ASGNI4
ADDRGP4 s_q3roptions+1840+4
ADDRGP4 $600
ASGNP4
ADDRGP4 s_q3roptions+1840+8
CNSTI4 32
ASGNI4
ADDRGP4 s_q3roptions+1840+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1840+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+1904
CNSTI4 5
ASGNI4
ADDRGP4 s_q3roptions+1904+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1904+12
CNSTI4 500
ASGNI4
ADDRGP4 s_q3roptions+1904+16
CNSTI4 320
ASGNI4
ADDRGP4 s_q3roptions+1904+4
ADDRGP4 $616
ASGNP4
ADDRGP4 s_q3roptions+1904+8
CNSTI4 33
ASGNI4
ADDRGP4 s_q3roptions+1904+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1904+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+1968
CNSTI4 5
ASGNI4
ADDRGP4 s_q3roptions+1968+44
CNSTU4 2
ASGNU4
ADDRGP4 s_q3roptions+1968+12
CNSTI4 500
ASGNI4
ADDRGP4 s_q3roptions+1968+16
CNSTI4 340
ASGNI4
ADDRGP4 s_q3roptions+1968+4
ADDRGP4 $632
ASGNP4
ADDRGP4 s_q3roptions+1968+8
CNSTI4 34
ASGNI4
ADDRGP4 s_q3roptions+1968+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+1968+52
ADDRGP4 Q3ROptions_StatusBar
ASGNP4
ADDRGP4 s_q3roptions+2240
CNSTI4 9
ASGNI4
ADDRGP4 s_q3roptions+2240+44
CNSTU4 260
ASGNU4
ADDRGP4 s_q3roptions+2240+12
CNSTI4 20
ASGNI4
ADDRGP4 s_q3roptions+2240+16
CNSTI4 430
ASGNI4
ADDRGP4 s_q3roptions+2240+8
CNSTI4 40
ASGNI4
ADDRGP4 s_q3roptions+2240+48
ADDRGP4 Q3ROptions_MenuEvent
ASGNP4
ADDRGP4 s_q3roptions+2240+60
ADDRGP4 $745
ASGNP4
ADDRGP4 s_q3roptions+2240+68
ADDRGP4 text_color_normal
ASGNP4
ADDRGP4 s_q3roptions+2240+64
CNSTI4 16
ASGNI4
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+560
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+2240
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+632
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+736
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+840
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+944
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1048
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1112
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1176
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+2176
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1240
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1316
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+2032
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1392
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1456
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1520
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1584
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1648
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+2104
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1712
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1776
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1840
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1904
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 s_q3roptions+1968
ARGP4
ADDRGP4 Menu_AddItem
CALLV
pop
LABELV $258
endproc Q3ROptions_MenuInit 40 12
export UI_Q3ROptionsMenu
proc UI_Q3ROptionsMenu 0 4
ADDRGP4 uis+11496
CNSTI4 0
ASGNI4
ADDRGP4 Q3ROptions_MenuInit
CALLV
pop
ADDRGP4 s_q3roptions
ARGP4
ADDRGP4 UI_PushMenu
CALLV
pop
LABELV $774
endproc UI_Q3ROptionsMenu 0 4
bss
align 4
LABELV s_q3roptions
skip 2312
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
import UI_CreditMenu
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
LABELV $745
byte 1 60
byte 1 32
byte 1 66
byte 1 65
byte 1 67
byte 1 75
byte 1 0
align 1
LABELV $652
byte 1 77
byte 1 97
byte 1 105
byte 1 110
byte 1 32
byte 1 86
byte 1 105
byte 1 101
byte 1 119
byte 1 32
byte 1 82
byte 1 101
byte 1 110
byte 1 100
byte 1 101
byte 1 114
byte 1 32
byte 1 76
byte 1 101
byte 1 118
byte 1 101
byte 1 108
byte 1 58
byte 1 0
align 1
LABELV $632
byte 1 83
byte 1 112
byte 1 97
byte 1 114
byte 1 107
byte 1 115
byte 1 58
byte 1 0
align 1
LABELV $616
byte 1 77
byte 1 97
byte 1 114
byte 1 107
byte 1 115
byte 1 58
byte 1 0
align 1
LABELV $600
byte 1 83
byte 1 109
byte 1 111
byte 1 107
byte 1 101
byte 1 58
byte 1 0
align 1
LABELV $584
byte 1 79
byte 1 98
byte 1 106
byte 1 101
byte 1 99
byte 1 116
byte 1 115
byte 1 58
byte 1 0
align 1
LABELV $568
byte 1 80
byte 1 108
byte 1 97
byte 1 121
byte 1 101
byte 1 114
byte 1 115
byte 1 58
byte 1 0
align 1
LABELV $554
byte 1 82
byte 1 101
byte 1 97
byte 1 114
byte 1 32
byte 1 86
byte 1 105
byte 1 101
byte 1 119
byte 1 32
byte 1 82
byte 1 101
byte 1 110
byte 1 100
byte 1 101
byte 1 114
byte 1 32
byte 1 76
byte 1 101
byte 1 118
byte 1 101
byte 1 108
byte 1 58
byte 1 0
align 1
LABELV $530
byte 1 67
byte 1 97
byte 1 109
byte 1 101
byte 1 114
byte 1 97
byte 1 32
byte 1 84
byte 1 114
byte 1 97
byte 1 99
byte 1 107
byte 1 105
byte 1 110
byte 1 103
byte 1 32
byte 1 83
byte 1 99
byte 1 97
byte 1 108
byte 1 101
byte 1 58
byte 1 0
align 1
LABELV $510
byte 1 83
byte 1 107
byte 1 105
byte 1 100
byte 1 32
byte 1 83
byte 1 101
byte 1 103
byte 1 109
byte 1 101
byte 1 110
byte 1 116
byte 1 32
byte 1 76
byte 1 101
byte 1 110
byte 1 103
byte 1 116
byte 1 104
byte 1 58
byte 1 0
align 1
LABELV $494
byte 1 69
byte 1 110
byte 1 103
byte 1 105
byte 1 110
byte 1 101
byte 1 32
byte 1 83
byte 1 111
byte 1 117
byte 1 110
byte 1 100
byte 1 115
byte 1 58
byte 1 0
align 1
LABELV $478
byte 1 82
byte 1 97
byte 1 99
byte 1 101
byte 1 32
byte 1 80
byte 1 111
byte 1 115
byte 1 105
byte 1 116
byte 1 105
byte 1 111
byte 1 110
byte 1 32
byte 1 83
byte 1 112
byte 1 114
byte 1 105
byte 1 116
byte 1 101
byte 1 115
byte 1 58
byte 1 0
align 1
LABELV $462
byte 1 82
byte 1 101
byte 1 97
byte 1 114
byte 1 32
byte 1 86
byte 1 105
byte 1 101
byte 1 119
byte 1 32
byte 1 77
byte 1 105
byte 1 114
byte 1 114
byte 1 111
byte 1 114
byte 1 58
byte 1 0
align 1
LABELV $446
byte 1 77
byte 1 97
byte 1 110
byte 1 117
byte 1 97
byte 1 108
byte 1 32
byte 1 70
byte 1 111
byte 1 114
byte 1 119
byte 1 97
byte 1 114
byte 1 100
byte 1 47
byte 1 82
byte 1 101
byte 1 118
byte 1 101
byte 1 114
byte 1 115
byte 1 101
byte 1 58
byte 1 0
align 1
LABELV $422
byte 1 65
byte 1 116
byte 1 109
byte 1 111
byte 1 115
byte 1 112
byte 1 104
byte 1 101
byte 1 114
byte 1 105
byte 1 99
byte 1 32
byte 1 76
byte 1 101
byte 1 118
byte 1 101
byte 1 108
byte 1 58
byte 1 0
align 1
LABELV $404
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
byte 1 0
align 1
LABELV $386
byte 1 67
byte 1 104
byte 1 101
byte 1 99
byte 1 107
byte 1 112
byte 1 111
byte 1 105
byte 1 110
byte 1 116
byte 1 32
byte 1 65
byte 1 114
byte 1 114
byte 1 111
byte 1 119
byte 1 32
byte 1 77
byte 1 111
byte 1 100
byte 1 101
byte 1 58
byte 1 0
align 1
LABELV $368
byte 1 85
byte 1 110
byte 1 105
byte 1 116
byte 1 32
byte 1 84
byte 1 121
byte 1 112
byte 1 101
byte 1 58
byte 1 0
align 1
LABELV $360
byte 1 81
byte 1 51
byte 1 82
byte 1 32
byte 1 79
byte 1 80
byte 1 84
byte 1 73
byte 1 79
byte 1 78
byte 1 83
byte 1 32
byte 1 0
align 1
LABELV $255
byte 1 83
byte 1 104
byte 1 111
byte 1 119
byte 1 32
byte 1 115
byte 1 112
byte 1 97
byte 1 114
byte 1 107
byte 1 115
byte 1 32
byte 1 105
byte 1 110
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 109
byte 1 97
byte 1 105
byte 1 110
byte 1 32
byte 1 118
byte 1 105
byte 1 101
byte 1 119
byte 1 46
byte 1 0
align 1
LABELV $253
byte 1 83
byte 1 104
byte 1 111
byte 1 119
byte 1 32
byte 1 109
byte 1 97
byte 1 114
byte 1 107
byte 1 115
byte 1 32
byte 1 105
byte 1 110
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 109
byte 1 97
byte 1 105
byte 1 110
byte 1 32
byte 1 118
byte 1 105
byte 1 101
byte 1 119
byte 1 46
byte 1 0
align 1
LABELV $251
byte 1 83
byte 1 104
byte 1 111
byte 1 119
byte 1 32
byte 1 115
byte 1 109
byte 1 111
byte 1 107
byte 1 101
byte 1 32
byte 1 105
byte 1 110
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 109
byte 1 97
byte 1 105
byte 1 110
byte 1 32
byte 1 118
byte 1 105
byte 1 101
byte 1 119
byte 1 46
byte 1 0
align 1
LABELV $249
byte 1 83
byte 1 104
byte 1 111
byte 1 119
byte 1 32
byte 1 109
byte 1 97
byte 1 112
byte 1 32
byte 1 111
byte 1 98
byte 1 106
byte 1 101
byte 1 99
byte 1 116
byte 1 115
byte 1 32
byte 1 105
byte 1 110
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 109
byte 1 97
byte 1 105
byte 1 110
byte 1 32
byte 1 118
byte 1 105
byte 1 101
byte 1 119
byte 1 46
byte 1 0
align 1
LABELV $247
byte 1 83
byte 1 104
byte 1 111
byte 1 119
byte 1 32
byte 1 111
byte 1 116
byte 1 104
byte 1 101
byte 1 114
byte 1 32
byte 1 112
byte 1 108
byte 1 97
byte 1 121
byte 1 101
byte 1 114
byte 1 115
byte 1 32
byte 1 105
byte 1 110
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 109
byte 1 97
byte 1 105
byte 1 110
byte 1 32
byte 1 118
byte 1 105
byte 1 101
byte 1 119
byte 1 46
byte 1 0
align 1
LABELV $245
byte 1 83
byte 1 104
byte 1 111
byte 1 119
byte 1 32
byte 1 115
byte 1 112
byte 1 97
byte 1 114
byte 1 107
byte 1 115
byte 1 32
byte 1 105
byte 1 110
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 114
byte 1 101
byte 1 97
byte 1 114
byte 1 32
byte 1 118
byte 1 105
byte 1 101
byte 1 119
byte 1 32
byte 1 109
byte 1 105
byte 1 114
byte 1 114
byte 1 111
byte 1 114
byte 1 46
byte 1 0
align 1
LABELV $243
byte 1 83
byte 1 104
byte 1 111
byte 1 119
byte 1 32
byte 1 115
byte 1 107
byte 1 105
byte 1 100
byte 1 109
byte 1 97
byte 1 114
byte 1 107
byte 1 115
byte 1 32
byte 1 105
byte 1 110
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 114
byte 1 101
byte 1 97
byte 1 114
byte 1 32
byte 1 118
byte 1 105
byte 1 101
byte 1 119
byte 1 32
byte 1 109
byte 1 105
byte 1 114
byte 1 114
byte 1 111
byte 1 114
byte 1 46
byte 1 0
align 1
LABELV $241
byte 1 83
byte 1 104
byte 1 111
byte 1 119
byte 1 32
byte 1 115
byte 1 109
byte 1 111
byte 1 107
byte 1 101
byte 1 32
byte 1 105
byte 1 110
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 114
byte 1 101
byte 1 97
byte 1 114
byte 1 32
byte 1 118
byte 1 105
byte 1 101
byte 1 119
byte 1 32
byte 1 109
byte 1 105
byte 1 114
byte 1 114
byte 1 111
byte 1 114
byte 1 46
byte 1 0
align 1
LABELV $239
byte 1 83
byte 1 104
byte 1 111
byte 1 119
byte 1 32
byte 1 109
byte 1 97
byte 1 112
byte 1 32
byte 1 111
byte 1 98
byte 1 106
byte 1 101
byte 1 99
byte 1 116
byte 1 115
byte 1 32
byte 1 105
byte 1 110
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 114
byte 1 101
byte 1 97
byte 1 114
byte 1 32
byte 1 118
byte 1 105
byte 1 101
byte 1 119
byte 1 32
byte 1 109
byte 1 105
byte 1 114
byte 1 114
byte 1 111
byte 1 114
byte 1 46
byte 1 0
align 1
LABELV $237
byte 1 83
byte 1 104
byte 1 111
byte 1 119
byte 1 32
byte 1 111
byte 1 116
byte 1 104
byte 1 101
byte 1 114
byte 1 32
byte 1 112
byte 1 108
byte 1 97
byte 1 121
byte 1 101
byte 1 114
byte 1 115
byte 1 32
byte 1 105
byte 1 110
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 114
byte 1 101
byte 1 97
byte 1 114
byte 1 32
byte 1 118
byte 1 105
byte 1 101
byte 1 119
byte 1 32
byte 1 109
byte 1 105
byte 1 114
byte 1 114
byte 1 111
byte 1 114
byte 1 46
byte 1 0
align 1
LABELV $235
byte 1 84
byte 1 117
byte 1 114
byte 1 110
byte 1 115
byte 1 32
byte 1 111
byte 1 110
byte 1 32
byte 1 101
byte 1 110
byte 1 103
byte 1 105
byte 1 110
byte 1 101
byte 1 32
byte 1 115
byte 1 111
byte 1 117
byte 1 110
byte 1 100
byte 1 115
byte 1 32
byte 1 105
byte 1 110
byte 1 32
byte 1 103
byte 1 97
byte 1 109
byte 1 101
byte 1 46
byte 1 0
align 1
LABELV $233
byte 1 84
byte 1 105
byte 1 103
byte 1 104
byte 1 116
byte 1 110
byte 1 101
byte 1 115
byte 1 115
byte 1 32
byte 1 111
byte 1 102
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 106
byte 1 111
byte 1 121
byte 1 115
byte 1 116
byte 1 105
byte 1 99
byte 1 107
byte 1 32
byte 1 99
byte 1 97
byte 1 109
byte 1 101
byte 1 114
byte 1 97
byte 1 32
byte 1 116
byte 1 114
byte 1 97
byte 1 99
byte 1 107
byte 1 105
byte 1 110
byte 1 103
byte 1 32
byte 1 109
byte 1 111
byte 1 100
byte 1 101
byte 1 46
byte 1 32
byte 1 32
byte 1 65
byte 1 32
byte 1 104
byte 1 105
byte 1 103
byte 1 104
byte 1 101
byte 1 114
byte 1 32
byte 1 118
byte 1 97
byte 1 108
byte 1 117
byte 1 101
byte 1 32
byte 1 109
byte 1 97
byte 1 107
byte 1 101
byte 1 115
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 99
byte 1 97
byte 1 109
byte 1 101
byte 1 114
byte 1 97
byte 1 32
byte 1 115
byte 1 116
byte 1 97
byte 1 121
byte 1 32
byte 1 98
byte 1 101
byte 1 104
byte 1 105
byte 1 110
byte 1 100
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 99
byte 1 97
byte 1 114
byte 1 32
byte 1 109
byte 1 111
byte 1 114
byte 1 101
byte 1 46
byte 1 0
align 1
LABELV $231
byte 1 84
byte 1 104
byte 1 101
byte 1 32
byte 1 108
byte 1 101
byte 1 110
byte 1 103
byte 1 116
byte 1 104
byte 1 32
byte 1 111
byte 1 102
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 115
byte 1 107
byte 1 105
byte 1 100
byte 1 32
byte 1 115
byte 1 101
byte 1 103
byte 1 109
byte 1 101
byte 1 110
byte 1 116
byte 1 115
byte 1 46
byte 1 32
byte 1 32
byte 1 65
byte 1 32
byte 1 108
byte 1 111
byte 1 119
byte 1 101
byte 1 114
byte 1 32
byte 1 118
byte 1 97
byte 1 108
byte 1 117
byte 1 101
byte 1 32
byte 1 119
byte 1 105
byte 1 108
byte 1 108
byte 1 32
byte 1 109
byte 1 97
byte 1 107
byte 1 101
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 115
byte 1 107
byte 1 105
byte 1 100
byte 1 32
byte 1 109
byte 1 97
byte 1 114
byte 1 107
byte 1 115
byte 1 32
byte 1 108
byte 1 111
byte 1 111
byte 1 107
byte 1 32
byte 1 115
byte 1 109
byte 1 111
byte 1 111
byte 1 116
byte 1 104
byte 1 101
byte 1 114
byte 1 46
byte 1 0
align 1
LABELV $229
byte 1 68
byte 1 114
byte 1 97
byte 1 119
byte 1 32
byte 1 112
byte 1 111
byte 1 115
byte 1 105
byte 1 116
byte 1 105
byte 1 111
byte 1 110
byte 1 32
byte 1 115
byte 1 112
byte 1 114
byte 1 105
byte 1 116
byte 1 101
byte 1 115
byte 1 32
byte 1 97
byte 1 98
byte 1 111
byte 1 118
byte 1 101
byte 1 32
byte 1 99
byte 1 97
byte 1 114
byte 1 115
byte 1 32
byte 1 119
byte 1 104
byte 1 101
byte 1 110
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 105
byte 1 114
byte 1 32
byte 1 112
byte 1 111
byte 1 115
byte 1 105
byte 1 116
byte 1 105
byte 1 111
byte 1 110
byte 1 32
byte 1 99
byte 1 104
byte 1 97
byte 1 110
byte 1 103
byte 1 101
byte 1 115
byte 1 46
byte 1 0
align 1
LABELV $227
byte 1 68
byte 1 114
byte 1 97
byte 1 119
byte 1 32
byte 1 114
byte 1 101
byte 1 97
byte 1 114
byte 1 32
byte 1 118
byte 1 105
byte 1 101
byte 1 119
byte 1 32
byte 1 109
byte 1 105
byte 1 114
byte 1 114
byte 1 111
byte 1 114
byte 1 32
byte 1 111
byte 1 110
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 72
byte 1 85
byte 1 68
byte 1 46
byte 1 0
align 1
LABELV $225
byte 1 85
byte 1 115
byte 1 101
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 103
byte 1 101
byte 1 97
byte 1 114
byte 1 32
byte 1 117
byte 1 112
byte 1 32
byte 1 97
byte 1 110
byte 1 100
byte 1 32
byte 1 103
byte 1 101
byte 1 97
byte 1 114
byte 1 32
byte 1 100
byte 1 111
byte 1 119
byte 1 110
byte 1 32
byte 1 107
byte 1 101
byte 1 121
byte 1 115
byte 1 32
byte 1 116
byte 1 111
byte 1 32
byte 1 115
byte 1 119
byte 1 105
byte 1 116
byte 1 99
byte 1 104
byte 1 32
byte 1 98
byte 1 101
byte 1 116
byte 1 119
byte 1 101
byte 1 101
byte 1 110
byte 1 32
byte 1 102
byte 1 111
byte 1 114
byte 1 119
byte 1 97
byte 1 114
byte 1 100
byte 1 32
byte 1 97
byte 1 110
byte 1 100
byte 1 32
byte 1 114
byte 1 101
byte 1 118
byte 1 101
byte 1 114
byte 1 115
byte 1 101
byte 1 32
byte 1 109
byte 1 97
byte 1 110
byte 1 117
byte 1 97
byte 1 108
byte 1 108
byte 1 121
byte 1 46
byte 1 0
align 1
LABELV $224
byte 1 84
byte 1 104
byte 1 101
byte 1 32
byte 1 99
byte 1 97
byte 1 114
byte 1 32
byte 1 119
byte 1 105
byte 1 108
byte 1 108
byte 1 32
byte 1 97
byte 1 117
byte 1 116
byte 1 111
byte 1 109
byte 1 97
byte 1 116
byte 1 105
byte 1 99
byte 1 97
byte 1 108
byte 1 108
byte 1 121
byte 1 32
byte 1 115
byte 1 104
byte 1 105
byte 1 102
byte 1 116
byte 1 32
byte 1 102
byte 1 114
byte 1 111
byte 1 109
byte 1 32
byte 1 102
byte 1 111
byte 1 114
byte 1 119
byte 1 97
byte 1 114
byte 1 100
byte 1 32
byte 1 116
byte 1 111
byte 1 32
byte 1 114
byte 1 101
byte 1 118
byte 1 101
byte 1 114
byte 1 115
byte 1 101
byte 1 32
byte 1 119
byte 1 104
byte 1 101
byte 1 110
byte 1 32
byte 1 121
byte 1 111
byte 1 117
byte 1 32
byte 1 115
byte 1 116
byte 1 111
byte 1 112
byte 1 46
byte 1 0
align 1
LABELV $218
byte 1 68
byte 1 101
byte 1 116
byte 1 101
byte 1 114
byte 1 109
byte 1 105
byte 1 110
byte 1 101
byte 1 115
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 114
byte 1 101
byte 1 108
byte 1 97
byte 1 116
byte 1 105
byte 1 118
byte 1 101
byte 1 32
byte 1 110
byte 1 117
byte 1 109
byte 1 98
byte 1 101
byte 1 114
byte 1 32
byte 1 111
byte 1 102
byte 1 32
byte 1 101
byte 1 110
byte 1 118
byte 1 105
byte 1 114
byte 1 111
byte 1 110
byte 1 109
byte 1 101
byte 1 110
byte 1 116
byte 1 32
byte 1 112
byte 1 97
byte 1 114
byte 1 116
byte 1 105
byte 1 99
byte 1 108
byte 1 101
byte 1 115
byte 1 32
byte 1 116
byte 1 111
byte 1 32
byte 1 115
byte 1 104
byte 1 111
byte 1 119
byte 1 46
byte 1 0
align 1
LABELV $216
byte 1 74
byte 1 111
byte 1 121
byte 1 115
byte 1 116
byte 1 105
byte 1 99
byte 1 107
byte 1 32
byte 1 99
byte 1 111
byte 1 110
byte 1 116
byte 1 114
byte 1 111
byte 1 108
byte 1 32
byte 1 109
byte 1 97
byte 1 112
byte 1 115
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 97
byte 1 120
byte 1 105
byte 1 115
byte 1 32
byte 1 100
byte 1 105
byte 1 114
byte 1 101
byte 1 99
byte 1 116
byte 1 108
byte 1 121
byte 1 32
byte 1 116
byte 1 111
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 119
byte 1 104
byte 1 101
byte 1 101
byte 1 108
byte 1 32
byte 1 97
byte 1 110
byte 1 103
byte 1 108
byte 1 101
byte 1 46
byte 1 0
align 1
LABELV $215
byte 1 77
byte 1 111
byte 1 117
byte 1 115
byte 1 101
byte 1 32
byte 1 99
byte 1 111
byte 1 110
byte 1 116
byte 1 114
byte 1 111
byte 1 108
byte 1 32
byte 1 97
byte 1 108
byte 1 108
byte 1 111
byte 1 119
byte 1 115
byte 1 32
byte 1 97
byte 1 32
byte 1 102
byte 1 114
byte 1 101
byte 1 101
byte 1 108
byte 1 111
byte 1 111
byte 1 107
byte 1 32
byte 1 108
byte 1 105
byte 1 107
byte 1 101
byte 1 32
byte 1 110
byte 1 111
byte 1 114
byte 1 109
byte 1 97
byte 1 108
byte 1 32
byte 1 81
byte 1 51
byte 1 65
byte 1 32
byte 1 97
byte 1 110
byte 1 100
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 99
byte 1 97
byte 1 114
byte 1 10
byte 1 32
byte 1 116
byte 1 117
byte 1 114
byte 1 110
byte 1 115
byte 1 32
byte 1 116
byte 1 111
byte 1 119
byte 1 97
byte 1 114
byte 1 100
byte 1 115
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 100
byte 1 105
byte 1 114
byte 1 101
byte 1 99
byte 1 116
byte 1 105
byte 1 111
byte 1 110
byte 1 32
byte 1 121
byte 1 111
byte 1 117
byte 1 32
byte 1 97
byte 1 114
byte 1 101
byte 1 32
byte 1 108
byte 1 111
byte 1 111
byte 1 107
byte 1 105
byte 1 110
byte 1 103
byte 1 46
byte 1 0
align 1
LABELV $209
byte 1 68
byte 1 105
byte 1 115
byte 1 112
byte 1 108
byte 1 97
byte 1 121
byte 1 32
byte 1 111
byte 1 112
byte 1 116
byte 1 105
byte 1 111
byte 1 110
byte 1 115
byte 1 32
byte 1 102
byte 1 111
byte 1 114
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 110
byte 1 101
byte 1 120
byte 1 116
byte 1 32
byte 1 99
byte 1 104
byte 1 101
byte 1 99
byte 1 107
byte 1 112
byte 1 111
byte 1 105
byte 1 110
byte 1 116
byte 1 32
byte 1 97
byte 1 114
byte 1 114
byte 1 111
byte 1 119
byte 1 46
byte 1 0
align 1
LABELV $207
byte 1 85
byte 1 115
byte 1 101
byte 1 32
byte 1 75
byte 1 80
byte 1 72
byte 1 32
byte 1 111
byte 1 114
byte 1 32
byte 1 77
byte 1 80
byte 1 72
byte 1 32
byte 1 111
byte 1 110
byte 1 32
byte 1 116
byte 1 104
byte 1 101
byte 1 32
byte 1 115
byte 1 112
byte 1 101
byte 1 101
byte 1 100
byte 1 111
byte 1 109
byte 1 101
byte 1 116
byte 1 101
byte 1 114
byte 1 46
byte 1 0
align 1
LABELV $202
byte 1 85
byte 1 115
byte 1 101
byte 1 32
byte 1 65
byte 1 114
byte 1 114
byte 1 111
byte 1 119
byte 1 32
byte 1 75
byte 1 101
byte 1 121
byte 1 115
byte 1 32
byte 1 111
byte 1 114
byte 1 32
byte 1 67
byte 1 76
byte 1 73
byte 1 67
byte 1 75
byte 1 32
byte 1 116
byte 1 111
byte 1 32
byte 1 99
byte 1 104
byte 1 97
byte 1 110
byte 1 103
byte 1 101
byte 1 46
byte 1 0
align 1
LABELV $197
byte 1 99
byte 1 103
byte 1 95
byte 1 109
byte 1 97
byte 1 105
byte 1 110
byte 1 86
byte 1 105
byte 1 101
byte 1 119
byte 1 82
byte 1 101
byte 1 110
byte 1 100
byte 1 101
byte 1 114
byte 1 76
byte 1 101
byte 1 118
byte 1 101
byte 1 108
byte 1 0
align 1
LABELV $170
byte 1 99
byte 1 103
byte 1 95
byte 1 114
byte 1 101
byte 1 97
byte 1 114
byte 1 86
byte 1 105
byte 1 101
byte 1 119
byte 1 82
byte 1 101
byte 1 110
byte 1 100
byte 1 101
byte 1 114
byte 1 76
byte 1 101
byte 1 118
byte 1 101
byte 1 108
byte 1 0
align 1
LABELV $141
byte 1 99
byte 1 103
byte 1 95
byte 1 101
byte 1 110
byte 1 103
byte 1 105
byte 1 110
byte 1 101
byte 1 83
byte 1 111
byte 1 117
byte 1 110
byte 1 100
byte 1 115
byte 1 0
align 1
LABELV $137
byte 1 99
byte 1 103
byte 1 95
byte 1 116
byte 1 105
byte 1 103
byte 1 104
byte 1 116
byte 1 67
byte 1 97
byte 1 109
byte 1 84
byte 1 114
byte 1 97
byte 1 99
byte 1 107
byte 1 105
byte 1 110
byte 1 103
byte 1 0
align 1
LABELV $133
byte 1 99
byte 1 103
byte 1 95
byte 1 109
byte 1 105
byte 1 110
byte 1 83
byte 1 107
byte 1 105
byte 1 100
byte 1 76
byte 1 101
byte 1 110
byte 1 103
byte 1 116
byte 1 104
byte 1 0
align 1
LABELV $129
byte 1 99
byte 1 103
byte 1 95
byte 1 100
byte 1 114
byte 1 97
byte 1 119
byte 1 80
byte 1 111
byte 1 115
byte 1 105
byte 1 116
byte 1 105
byte 1 111
byte 1 110
byte 1 83
byte 1 112
byte 1 114
byte 1 105
byte 1 116
byte 1 101
byte 1 115
byte 1 0
align 1
LABELV $125
byte 1 99
byte 1 103
byte 1 95
byte 1 100
byte 1 114
byte 1 97
byte 1 119
byte 1 82
byte 1 101
byte 1 97
byte 1 114
byte 1 86
byte 1 105
byte 1 101
byte 1 119
byte 1 0
align 1
LABELV $121
byte 1 99
byte 1 103
byte 1 95
byte 1 109
byte 1 97
byte 1 110
byte 1 117
byte 1 97
byte 1 108
byte 1 83
byte 1 104
byte 1 105
byte 1 102
byte 1 116
byte 1 0
align 1
LABELV $117
byte 1 99
byte 1 103
byte 1 95
byte 1 97
byte 1 116
byte 1 109
byte 1 111
byte 1 115
byte 1 112
byte 1 104
byte 1 101
byte 1 114
byte 1 105
byte 1 99
byte 1 76
byte 1 101
byte 1 118
byte 1 101
byte 1 108
byte 1 0
align 1
LABELV $115
byte 1 43
byte 1 115
byte 1 116
byte 1 114
byte 1 97
byte 1 102
byte 1 101
byte 1 0
align 1
LABELV $114
byte 1 45
byte 1 115
byte 1 116
byte 1 114
byte 1 97
byte 1 102
byte 1 101
byte 1 0
align 1
LABELV $107
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
LABELV $103
byte 1 99
byte 1 103
byte 1 95
byte 1 99
byte 1 104
byte 1 101
byte 1 99
byte 1 107
byte 1 112
byte 1 111
byte 1 105
byte 1 110
byte 1 116
byte 1 65
byte 1 114
byte 1 114
byte 1 111
byte 1 119
byte 1 77
byte 1 111
byte 1 100
byte 1 101
byte 1 0
align 1
LABELV $99
byte 1 99
byte 1 103
byte 1 95
byte 1 109
byte 1 101
byte 1 116
byte 1 114
byte 1 105
byte 1 99
byte 1 85
byte 1 110
byte 1 105
byte 1 116
byte 1 115
byte 1 0
align 1
LABELV $91
byte 1 72
byte 1 105
byte 1 103
byte 1 104
byte 1 0
align 1
LABELV $90
byte 1 76
byte 1 111
byte 1 119
byte 1 0
align 1
LABELV $89
byte 1 78
byte 1 111
byte 1 110
byte 1 101
byte 1 0
align 1
LABELV $88
byte 1 65
byte 1 98
byte 1 111
byte 1 118
byte 1 101
byte 1 32
byte 1 67
byte 1 97
byte 1 114
byte 1 0
align 1
LABELV $87
byte 1 79
byte 1 110
byte 1 32
byte 1 72
byte 1 85
byte 1 68
byte 1 0
align 1
LABELV $86
byte 1 79
byte 1 102
byte 1 102
byte 1 0
align 1
LABELV $85
byte 1 75
byte 1 101
byte 1 121
byte 1 98
byte 1 111
byte 1 97
byte 1 114
byte 1 100
byte 1 47
byte 1 74
byte 1 111
byte 1 121
byte 1 115
byte 1 116
byte 1 105
byte 1 99
byte 1 107
byte 1 0
align 1
LABELV $84
byte 1 77
byte 1 111
byte 1 117
byte 1 115
byte 1 101
byte 1 0
align 1
LABELV $83
byte 1 77
byte 1 101
byte 1 116
byte 1 114
byte 1 105
byte 1 99
byte 1 0
align 1
LABELV $82
byte 1 73
byte 1 109
byte 1 112
byte 1 101
byte 1 114
byte 1 105
byte 1 97
byte 1 108
byte 1 0
