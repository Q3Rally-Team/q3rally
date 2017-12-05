export SeekToSection
code
proc SeekToSection 28 8
ADDRFP4 0
ADDRFP4 0
INDIRP4
ASGNP4
ADDRFP4 4
ADDRFP4 4
INDIRP4
ASGNP4
ADDRGP4 $86
JUMPV
LABELV $85
ADDRFP4 0
INDIRP4
ARGP4
ADDRLP4 4
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 4
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
EQU4 $90
ADDRLP4 0
INDIRP4
INDIRI1
CVII4 1
CNSTI4 0
NEI4 $88
LABELV $90
CNSTI4 0
RETI4
ADDRGP4 $84
JUMPV
LABELV $88
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $93
ARGP4
ADDRLP4 12
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 12
INDIRI4
CNSTI4 0
NEI4 $91
ADDRGP4 $95
JUMPV
LABELV $94
ADDRFP4 0
INDIRP4
ARGP4
ADDRLP4 16
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 16
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
EQU4 $99
ADDRLP4 0
INDIRP4
INDIRI1
CVII4 1
CNSTI4 0
NEI4 $97
LABELV $99
CNSTI4 0
RETI4
ADDRGP4 $84
JUMPV
LABELV $97
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $102
ARGP4
ADDRLP4 24
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 24
INDIRI4
CNSTI4 0
NEI4 $100
ADDRGP4 $96
JUMPV
LABELV $100
LABELV $95
ADDRGP4 $94
JUMPV
LABELV $96
LABELV $91
ADDRLP4 0
INDIRP4
ARGP4
ADDRFP4 4
INDIRP4
ARGP4
ADDRLP4 16
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 16
INDIRI4
CNSTI4 0
NEI4 $103
ADDRGP4 $87
JUMPV
LABELV $103
LABELV $86
ADDRGP4 $85
JUMPV
LABELV $87
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
EQU4 $107
ADDRLP4 0
INDIRP4
INDIRI1
CVII4 1
CNSTI4 0
NEI4 $105
LABELV $107
CNSTI4 0
RETI4
ADDRGP4 $84
JUMPV
LABELV $105
CNSTI4 1
RETI4
LABELV $84
endproc SeekToSection 28 8
export CG_ParseScriptedObject
proc CG_ParseScriptedObject 8520 12
ADDRFP4 4
ADDRFP4 4
INDIRP4
ASGNP4
ADDRFP4 4
INDIRP4
CVPU4 4
CNSTU4 0
EQU4 $111
ADDRFP4 4
INDIRP4
INDIRI1
CVII4 1
CNSTI4 0
NEI4 $109
LABELV $111
ADDRGP4 $112
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
CNSTI4 0
RETI4
ADDRGP4 $108
JUMPV
LABELV $109
ADDRLP4 136
ARGP4
ADDRFP4 4
INDIRP4
ARGP4
CNSTI4 64
ARGI4
ADDRGP4 Q_strncpyz
CALLV
pop
ADDRLP4 136
ARGP4
CNSTI4 46
ARGI4
ADDRLP4 8408
ADDRGP4 strchr
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8408
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $113
ADDRLP4 136
ARGP4
CNSTI4 64
ARGI4
ADDRGP4 $115
ARGP4
ADDRGP4 Q_strcat
CALLV
pop
LABELV $113
ADDRLP4 136
ARGP4
ADDRLP4 8396
ARGP4
CNSTI4 0
ARGI4
ADDRLP4 8412
ADDRGP4 trap_FS_FOpenFile
CALLI4
ASGNI4
ADDRLP4 200
ADDRLP4 8412
INDIRI4
ASGNI4
ADDRLP4 8396
INDIRI4
CNSTI4 0
NEI4 $116
ADDRGP4 $118
ARGP4
ADDRLP4 136
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
CNSTI4 0
RETI4
ADDRGP4 $108
JUMPV
LABELV $116
ADDRLP4 200
INDIRI4
CNSTI4 8192
LTI4 $119
ADDRLP4 200
CNSTI4 8191
ASGNI4
LABELV $119
ADDRLP4 204
ARGP4
ADDRLP4 200
INDIRI4
ARGI4
ADDRLP4 8396
INDIRI4
ARGI4
ADDRGP4 trap_FS_Read
CALLV
pop
ADDRLP4 200
INDIRI4
ADDRLP4 204
ADDP4
CNSTI1 0
ASGNI1
ADDRLP4 8396
INDIRI4
ARGI4
ADDRGP4 trap_FS_FCloseFile
CALLV
pop
ADDRLP4 4
ADDRLP4 204
ASGNP4
ADDRLP4 4
ARGP4
ADDRGP4 $123
ARGP4
ADDRLP4 8416
ADDRGP4 SeekToSection
CALLI4
ASGNI4
ADDRLP4 8416
INDIRI4
CNSTI4 0
NEI4 $121
ADDRGP4 $124
ARGP4
ADDRLP4 136
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
CNSTI4 0
RETI4
ADDRGP4 $108
JUMPV
LABELV $121
ADDRLP4 8420
CNSTI1 0
ASGNI1
ADDRLP4 8
ADDRLP4 8420
INDIRI1
ASGNI1
ADDRLP4 72
ADDRLP4 8420
INDIRI1
ASGNI1
ADDRGP4 $126
JUMPV
LABELV $125
ADDRLP4 4
ARGP4
ADDRLP4 8424
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8424
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
EQU4 $131
ADDRLP4 0
INDIRP4
INDIRI1
CVII4 1
CNSTI4 0
EQI4 $131
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $102
ARGP4
ADDRLP4 8432
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8432
INDIRI4
CNSTI4 0
NEI4 $128
LABELV $131
ADDRGP4 $127
JUMPV
LABELV $128
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $93
ARGP4
ADDRLP4 8436
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8436
INDIRI4
CNSTI4 0
NEI4 $132
ADDRGP4 $126
JUMPV
LABELV $132
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $136
ARGP4
ADDRLP4 8440
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8440
INDIRI4
CNSTI4 0
NEI4 $134
ADDRLP4 4
ARGP4
ADDRLP4 8444
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8444
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $126
ADDRGP4 $127
JUMPV
LABELV $134
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $141
ARGP4
ADDRLP4 8444
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8444
INDIRI4
CNSTI4 0
NEI4 $139
ADDRLP4 4
ARGP4
ADDRLP4 8448
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8448
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $142
ADDRGP4 $127
JUMPV
LABELV $142
ADDRLP4 8
ARGP4
ADDRLP4 0
INDIRP4
ARGP4
CNSTI4 64
ARGI4
ADDRGP4 Q_strncpyz
CALLV
pop
ADDRGP4 $126
JUMPV
LABELV $139
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $146
ARGP4
ADDRLP4 8448
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8448
INDIRI4
CNSTI4 0
NEI4 $144
ADDRLP4 4
ARGP4
ADDRLP4 8452
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8452
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $147
ADDRGP4 $127
JUMPV
LABELV $147
ADDRLP4 72
ARGP4
ADDRLP4 0
INDIRP4
ARGP4
CNSTI4 64
ARGI4
ADDRGP4 Q_strncpyz
CALLV
pop
ADDRGP4 $126
JUMPV
LABELV $144
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $151
ARGP4
ADDRLP4 8452
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8452
INDIRI4
CNSTI4 0
NEI4 $149
ADDRLP4 4
ARGP4
ADDRLP4 8456
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8456
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $126
ADDRGP4 $127
JUMPV
LABELV $149
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $156
ARGP4
ADDRLP4 8456
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8456
INDIRI4
CNSTI4 0
NEI4 $154
ADDRLP4 4
ARGP4
ADDRLP4 8460
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8460
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $126
ADDRGP4 $127
JUMPV
LABELV $154
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $161
ARGP4
ADDRLP4 8460
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8460
INDIRI4
CNSTI4 0
NEI4 $159
ADDRLP4 4
ARGP4
ADDRLP4 8464
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8464
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $126
ADDRGP4 $127
JUMPV
LABELV $159
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $166
ARGP4
ADDRLP4 8464
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8464
INDIRI4
CNSTI4 0
NEI4 $164
ADDRLP4 4
ARGP4
ADDRLP4 8468
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8468
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $126
ADDRGP4 $127
JUMPV
LABELV $164
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $171
ARGP4
ADDRLP4 8468
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8468
INDIRI4
CNSTI4 0
NEI4 $169
ADDRLP4 4
ARGP4
ADDRLP4 8472
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8472
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $126
ADDRGP4 $127
JUMPV
LABELV $169
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $176
ARGP4
ADDRLP4 8472
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8472
INDIRI4
CNSTI4 0
NEI4 $174
ADDRLP4 8400
CNSTI4 0
ASGNI4
LABELV $177
ADDRLP4 4
ARGP4
ADDRLP4 8476
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8476
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $181
ADDRGP4 $126
JUMPV
LABELV $181
LABELV $178
ADDRLP4 8400
ADDRLP4 8400
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
ADDRLP4 8400
INDIRI4
CNSTI4 3
LTI4 $177
ADDRGP4 $126
JUMPV
LABELV $174
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $185
ARGP4
ADDRLP4 8476
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8476
INDIRI4
CNSTI4 0
NEI4 $183
ADDRLP4 8400
CNSTI4 0
ASGNI4
LABELV $186
ADDRLP4 4
ARGP4
ADDRLP4 8480
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8480
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $190
ADDRGP4 $126
JUMPV
LABELV $190
LABELV $187
ADDRLP4 8400
ADDRLP4 8400
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
ADDRLP4 8400
INDIRI4
CNSTI4 3
LTI4 $186
ADDRGP4 $126
JUMPV
LABELV $183
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $194
ARGP4
ADDRLP4 8480
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8480
INDIRI4
CNSTI4 0
NEI4 $192
ADDRLP4 4
ARGP4
ADDRLP4 8484
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8484
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $195
ADDRGP4 $127
JUMPV
LABELV $195
ADDRLP4 0
INDIRP4
ARGP4
CNSTI4 0
ARGI4
ADDRLP4 8488
ADDRGP4 trap_S_RegisterSound
CALLI4
ASGNI4
ADDRFP4 0
INDIRP4
CNSTI4 836
ADDP4
ADDRLP4 8488
INDIRI4
ASGNI4
ADDRGP4 $126
JUMPV
LABELV $192
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $199
ARGP4
ADDRLP4 8484
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8484
INDIRI4
CNSTI4 0
NEI4 $197
ADDRLP4 4
ARGP4
ADDRLP4 8488
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8488
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $200
ADDRGP4 $127
JUMPV
LABELV $200
ADDRLP4 0
INDIRP4
ARGP4
CNSTI4 0
ARGI4
ADDRLP4 8492
ADDRGP4 trap_S_RegisterSound
CALLI4
ASGNI4
ADDRFP4 0
INDIRP4
CNSTI4 840
ADDP4
ADDRLP4 8492
INDIRI4
ASGNI4
ADDRGP4 $126
JUMPV
LABELV $197
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $204
ARGP4
ADDRLP4 8488
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8488
INDIRI4
CNSTI4 0
NEI4 $202
ADDRLP4 4
ARGP4
ADDRLP4 8492
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8492
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $205
ADDRGP4 $127
JUMPV
LABELV $205
ADDRLP4 0
INDIRP4
ARGP4
CNSTI4 0
ARGI4
ADDRLP4 8496
ADDRGP4 trap_S_RegisterSound
CALLI4
ASGNI4
ADDRFP4 0
INDIRP4
CNSTI4 844
ADDP4
ADDRLP4 8496
INDIRI4
ASGNI4
ADDRGP4 $126
JUMPV
LABELV $202
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $209
ARGP4
ADDRLP4 8492
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8492
INDIRI4
CNSTI4 0
NEI4 $207
ADDRLP4 4
ARGP4
ADDRLP4 8496
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8496
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $210
ADDRGP4 $127
JUMPV
LABELV $210
ADDRLP4 0
INDIRP4
ARGP4
CNSTI4 0
ARGI4
ADDRLP4 8500
ADDRGP4 trap_S_RegisterSound
CALLI4
ASGNI4
ADDRFP4 0
INDIRP4
CNSTI4 848
ADDP4
ADDRLP4 8500
INDIRI4
ASGNI4
ADDRGP4 $126
JUMPV
LABELV $207
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $214
ARGP4
ADDRLP4 8496
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8496
INDIRI4
CNSTI4 0
NEI4 $212
ADDRLP4 4
ARGP4
ADDRLP4 8500
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8500
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $215
ADDRGP4 $127
JUMPV
LABELV $215
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $93
ARGP4
ADDRLP4 8504
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8504
INDIRI4
CNSTI4 0
NEI4 $126
ADDRGP4 $220
JUMPV
LABELV $219
ADDRLP4 4
ARGP4
ADDRLP4 8508
ADDRGP4 COM_Parse
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 8508
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
EQU4 $224
ADDRLP4 0
INDIRP4
INDIRI1
CVII4 1
CNSTI4 0
NEI4 $222
LABELV $224
CNSTI4 0
RETI4
ADDRGP4 $108
JUMPV
LABELV $222
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 $102
ARGP4
ADDRLP4 8516
ADDRGP4 Q_stricmp
CALLI4
ASGNI4
ADDRLP4 8516
INDIRI4
CNSTI4 0
NEI4 $225
ADDRGP4 $126
JUMPV
LABELV $225
LABELV $220
ADDRGP4 $219
JUMPV
ADDRGP4 $126
JUMPV
LABELV $212
ADDRGP4 $227
ARGP4
ADDRLP4 0
INDIRP4
ARGP4
ADDRLP4 136
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
LABELV $126
ADDRGP4 $125
JUMPV
LABELV $127
ADDRLP4 8
INDIRI1
CVII4 1
CNSTI4 0
EQI4 $228
ADDRLP4 4
ADDRLP4 204
ASGNP4
ADDRLP4 4
ARGP4
ADDRLP4 8
ARGP4
ADDRLP4 8424
ADDRGP4 SeekToSection
CALLI4
ASGNI4
ADDRLP4 8424
INDIRI4
CNSTI4 0
NEI4 $230
ADDRLP4 8
ARGP4
ADDRLP4 8428
ADDRGP4 trap_R_RegisterModel
CALLI4
ASGNI4
ADDRFP4 0
INDIRP4
CNSTI4 852
ADDP4
ADDRLP4 8428
INDIRI4
ASGNI4
ADDRGP4 $231
JUMPV
LABELV $230
ADDRGP4 $232
ARGP4
ADDRLP4 8
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
LABELV $231
LABELV $228
ADDRLP4 72
INDIRI1
CVII4 1
CNSTI4 0
EQI4 $233
ADDRLP4 4
ADDRLP4 204
ASGNP4
ADDRLP4 4
ARGP4
ADDRLP4 72
ARGP4
ADDRLP4 8424
ADDRGP4 SeekToSection
CALLI4
ASGNI4
ADDRLP4 8424
INDIRI4
CNSTI4 0
NEI4 $235
ADDRLP4 8
ARGP4
ADDRLP4 8428
ADDRGP4 trap_R_RegisterModel
CALLI4
ASGNI4
ADDRFP4 0
INDIRP4
CNSTI4 856
ADDP4
ADDRLP4 8428
INDIRI4
ASGNI4
ADDRGP4 $236
JUMPV
LABELV $235
ADDRGP4 $237
ARGP4
ADDRLP4 72
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
LABELV $236
LABELV $233
CNSTI4 1
RETI4
LABELV $108
endproc CG_ParseScriptedObject 8520 12
export CG_Scripted_Object
proc CG_Scripted_Object 156 12
ADDRFP4 0
ADDRFP4 0
INDIRP4
ASGNP4
ADDRLP4 140
ADDRFP4 0
INDIRP4
ASGNP4
ADDRLP4 140
INDIRP4
CNSTI4 160
ADDP4
INDIRI4
CNSTI4 0
NEI4 $239
ADDRGP4 $238
JUMPV
LABELV $239
ADDRFP4 0
INDIRP4
CNSTI4 832
ADDP4
INDIRI4
CNSTI4 0
NEI4 $241
ADDRLP4 140
INDIRP4
CNSTI4 160
ADDP4
INDIRI4
CNSTI4 542
ADDI4
ARGI4
ADDRLP4 148
ADDRGP4 CG_ConfigString
CALLP4
ASGNP4
ADDRLP4 144
ADDRLP4 148
INDIRP4
ASGNP4
ADDRLP4 144
INDIRP4
INDIRI1
CVII4 1
CNSTI4 0
NEI4 $243
ADDRGP4 $238
JUMPV
LABELV $243
ADDRFP4 0
INDIRP4
ARGP4
ADDRLP4 144
INDIRP4
ARGP4
ADDRLP4 152
ADDRGP4 CG_ParseScriptedObject
CALLI4
ASGNI4
ADDRLP4 152
INDIRI4
CNSTI4 0
EQI4 $238
ADDRFP4 0
INDIRP4
CNSTI4 832
ADDP4
ADDRGP4 cg+107604
INDIRI4
ASGNI4
LABELV $246
LABELV $241
ADDRLP4 0
ARGP4
CNSTI4 0
ARGI4
CNSTU4 140
ARGU4
ADDRGP4 memset
CALLP4
pop
ADDRFP4 0
INDIRP4
CNSTI4 8
ADDP4
INDIRI4
CNSTI4 1
BANDI4
CNSTI4 0
EQI4 $248
ADDRLP4 0+8
ADDRFP4 0
INDIRP4
CNSTI4 856
ADDP4
INDIRI4
ASGNI4
ADDRGP4 $249
JUMPV
LABELV $248
ADDRLP4 0+8
ADDRFP4 0
INDIRP4
CNSTI4 852
ADDP4
INDIRI4
ASGNI4
LABELV $249
ADDRLP4 0+8
INDIRI4
CNSTI4 0
NEI4 $252
ADDRGP4 $238
JUMPV
LABELV $252
ADDRLP4 148
CNSTI4 0
ASGNI4
ADDRLP4 0+96
ADDRLP4 148
INDIRI4
ASGNI4
ADDRLP4 0+80
ADDRLP4 148
INDIRI4
ASGNI4
ADDRLP4 0+100
CNSTF4 0
ASGNF4
ADDRLP4 0+68
ADDRFP4 0
INDIRP4
CNSTI4 644
ADDP4
INDIRB
ASGNB 12
ADDRLP4 0+84
ADDRFP4 0
INDIRP4
CNSTI4 644
ADDP4
INDIRB
ASGNB 12
ADDRFP4 0
INDIRP4
CNSTI4 656
ADDP4
ARGP4
ADDRLP4 0+28
ARGP4
ADDRGP4 AnglesToAxis
CALLV
pop
ADDRLP4 0
ARGP4
ADDRGP4 trap_R_AddRefEntityToScene
CALLV
pop
LABELV $238
endproc CG_Scripted_Object 156 12
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
import CG_Atmospheric_SetParticles
import CG_AddAtmosphericEffects
import CG_CheckChangedPredictableEvents
import CG_TransitionPlayerState
import CG_Respawn
import CG_ShaderStateChanged
import CG_SetConfigValues
import CG_ParseServerinfo
import CG_ExecuteNewServerCommands
import CG_InitConsoleCommands
import CG_ConsoleCommand
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
LABELV $237
byte 1 76
byte 1 111
byte 1 97
byte 1 100
byte 1 105
byte 1 110
byte 1 103
byte 1 32
byte 1 100
byte 1 101
byte 1 97
byte 1 100
byte 1 109
byte 1 111
byte 1 100
byte 1 101
byte 1 108
byte 1 32
byte 1 105
byte 1 110
byte 1 102
byte 1 111
byte 1 32
byte 1 102
byte 1 111
byte 1 114
byte 1 32
byte 1 39
byte 1 37
byte 1 115
byte 1 39
byte 1 10
byte 1 0
align 1
LABELV $232
byte 1 76
byte 1 111
byte 1 97
byte 1 100
byte 1 105
byte 1 110
byte 1 103
byte 1 32
byte 1 109
byte 1 111
byte 1 100
byte 1 101
byte 1 108
byte 1 32
byte 1 105
byte 1 110
byte 1 102
byte 1 111
byte 1 32
byte 1 102
byte 1 111
byte 1 114
byte 1 32
byte 1 39
byte 1 37
byte 1 115
byte 1 39
byte 1 10
byte 1 0
align 1
LABELV $227
byte 1 87
byte 1 97
byte 1 114
byte 1 110
byte 1 105
byte 1 110
byte 1 103
byte 1 58
byte 1 32
byte 1 83
byte 1 107
byte 1 105
byte 1 112
byte 1 112
byte 1 105
byte 1 110
byte 1 103
byte 1 32
byte 1 117
byte 1 110
byte 1 107
byte 1 110
byte 1 111
byte 1 119
byte 1 110
byte 1 32
byte 1 116
byte 1 111
byte 1 107
byte 1 101
byte 1 110
byte 1 32
byte 1 37
byte 1 115
byte 1 32
byte 1 105
byte 1 110
byte 1 32
byte 1 37
byte 1 115
byte 1 10
byte 1 0
align 1
LABELV $214
byte 1 103
byte 1 105
byte 1 98
byte 1 115
byte 1 0
align 1
LABELV $209
byte 1 100
byte 1 101
byte 1 115
byte 1 116
byte 1 114
byte 1 111
byte 1 121
byte 1 115
byte 1 111
byte 1 117
byte 1 110
byte 1 100
byte 1 0
align 1
LABELV $204
byte 1 112
byte 1 111
byte 1 115
byte 1 116
byte 1 115
byte 1 111
byte 1 117
byte 1 110
byte 1 100
byte 1 0
align 1
LABELV $199
byte 1 112
byte 1 114
byte 1 101
byte 1 115
byte 1 111
byte 1 117
byte 1 110
byte 1 100
byte 1 0
align 1
LABELV $194
byte 1 104
byte 1 105
byte 1 116
byte 1 115
byte 1 111
byte 1 117
byte 1 110
byte 1 100
byte 1 0
align 1
LABELV $185
byte 1 109
byte 1 97
byte 1 120
byte 1 115
byte 1 0
align 1
LABELV $176
byte 1 109
byte 1 105
byte 1 110
byte 1 115
byte 1 0
align 1
LABELV $171
byte 1 104
byte 1 101
byte 1 97
byte 1 108
byte 1 116
byte 1 104
byte 1 0
align 1
LABELV $166
byte 1 102
byte 1 114
byte 1 97
byte 1 109
byte 1 101
byte 1 115
byte 1 0
align 1
LABELV $161
byte 1 109
byte 1 97
byte 1 115
byte 1 115
byte 1 0
align 1
LABELV $156
byte 1 101
byte 1 108
byte 1 97
byte 1 115
byte 1 116
byte 1 105
byte 1 99
byte 1 105
byte 1 116
byte 1 121
byte 1 0
align 1
LABELV $151
byte 1 109
byte 1 111
byte 1 118
byte 1 101
byte 1 97
byte 1 98
byte 1 108
byte 1 101
byte 1 0
align 1
LABELV $146
byte 1 100
byte 1 101
byte 1 97
byte 1 100
byte 1 109
byte 1 111
byte 1 100
byte 1 101
byte 1 108
byte 1 0
align 1
LABELV $141
byte 1 109
byte 1 111
byte 1 100
byte 1 101
byte 1 108
byte 1 0
align 1
LABELV $136
byte 1 116
byte 1 121
byte 1 112
byte 1 101
byte 1 0
align 1
LABELV $124
byte 1 83
byte 1 99
byte 1 114
byte 1 105
byte 1 112
byte 1 116
byte 1 32
byte 1 102
byte 1 105
byte 1 108
byte 1 101
byte 1 32
byte 1 39
byte 1 37
byte 1 115
byte 1 39
byte 1 32
byte 1 100
byte 1 105
byte 1 100
byte 1 32
byte 1 110
byte 1 111
byte 1 116
byte 1 32
byte 1 99
byte 1 111
byte 1 110
byte 1 116
byte 1 97
byte 1 105
byte 1 110
byte 1 32
byte 1 114
byte 1 97
byte 1 108
byte 1 108
byte 1 121
byte 1 95
byte 1 115
byte 1 99
byte 1 114
byte 1 105
byte 1 112
byte 1 116
byte 1 101
byte 1 100
byte 1 95
byte 1 111
byte 1 98
byte 1 106
byte 1 101
byte 1 99
byte 1 116
byte 1 10
byte 1 0
align 1
LABELV $123
byte 1 114
byte 1 97
byte 1 108
byte 1 108
byte 1 121
byte 1 95
byte 1 115
byte 1 99
byte 1 114
byte 1 105
byte 1 112
byte 1 116
byte 1 101
byte 1 100
byte 1 95
byte 1 111
byte 1 98
byte 1 106
byte 1 101
byte 1 99
byte 1 116
byte 1 0
align 1
LABELV $118
byte 1 67
byte 1 111
byte 1 117
byte 1 108
byte 1 100
byte 1 32
byte 1 110
byte 1 111
byte 1 116
byte 1 32
byte 1 102
byte 1 105
byte 1 110
byte 1 100
byte 1 32
byte 1 115
byte 1 99
byte 1 114
byte 1 105
byte 1 112
byte 1 116
byte 1 32
byte 1 37
byte 1 115
byte 1 10
byte 1 0
align 1
LABELV $115
byte 1 46
byte 1 115
byte 1 99
byte 1 114
byte 1 105
byte 1 112
byte 1 116
byte 1 0
align 1
LABELV $112
byte 1 78
byte 1 111
byte 1 32
byte 1 83
byte 1 99
byte 1 114
byte 1 105
byte 1 112
byte 1 116
byte 1 32
byte 1 102
byte 1 105
byte 1 108
byte 1 101
byte 1 32
byte 1 115
byte 1 112
byte 1 101
byte 1 99
byte 1 105
byte 1 102
byte 1 105
byte 1 101
byte 1 100
byte 1 10
byte 1 0
align 1
LABELV $102
byte 1 125
byte 1 0
align 1
LABELV $93
byte 1 123
byte 1 0
