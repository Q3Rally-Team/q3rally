export CG_DrawHUD_Times
code
proc CG_DrawHUD_Times 56 20
ADDRFP4 0
ADDRFP4 0
INDIRF4
ASGNF4
ADDRFP4 4
ADDRFP4 4
INDIRF4
ASGNF4
ADDRLP4 12
CNSTI4 0
ASGNI4
ADDRLP4 4
CNSTI4 884
ADDRGP4 cg+36
INDIRP4
CNSTI4 184
ADDP4
INDIRI4
MULI4
ADDRGP4 cg_entities
ADDP4
ASGNP4
ADDRLP4 4
INDIRP4
CNSTI4 668
ADDP4
INDIRI4
CNSTI4 0
EQI4 $86
ADDRLP4 36
ADDRLP4 4
INDIRP4
CNSTI4 668
ADDP4
ASGNP4
ADDRLP4 24
ADDRLP4 36
INDIRP4
INDIRI4
ADDRLP4 4
INDIRP4
CNSTI4 676
ADDP4
INDIRI4
SUBI4
ASGNI4
ADDRLP4 28
ADDRLP4 36
INDIRP4
INDIRI4
ADDRLP4 4
INDIRP4
CNSTI4 672
ADDP4
INDIRI4
SUBI4
ASGNI4
ADDRGP4 $87
JUMPV
LABELV $86
ADDRLP4 4
INDIRP4
CNSTI4 672
ADDP4
INDIRI4
CNSTI4 0
EQI4 $88
ADDRLP4 24
ADDRGP4 cg+107604
INDIRI4
ADDRLP4 4
INDIRP4
CNSTI4 676
ADDP4
INDIRI4
SUBI4
ASGNI4
ADDRLP4 28
ADDRGP4 cg+107604
INDIRI4
ADDRLP4 4
INDIRP4
CNSTI4 672
ADDP4
INDIRI4
SUBI4
ASGNI4
ADDRGP4 $89
JUMPV
LABELV $88
ADDRLP4 32
CNSTI4 0
ASGNI4
ADDRLP4 24
ADDRLP4 32
INDIRI4
ASGNI4
ADDRLP4 28
ADDRLP4 32
INDIRI4
ASGNI4
LABELV $89
LABELV $87
ADDRLP4 4
INDIRP4
CNSTI4 680
ADDP4
INDIRI4
CNSTI4 0
EQI4 $92
ADDRLP4 20
ADDRLP4 4
INDIRP4
CNSTI4 676
ADDP4
INDIRI4
ADDRLP4 4
INDIRP4
CNSTI4 680
ADDP4
INDIRI4
SUBI4
ASGNI4
ADDRGP4 $93
JUMPV
LABELV $92
ADDRLP4 20
CNSTI4 0
ASGNI4
LABELV $93
ADDRFP4 0
INDIRF4
ARGF4
ADDRFP4 4
INDIRF4
ARGF4
CNSTF4 1126825984
ARGF4
CNSTF4 1099956224
ARGF4
ADDRGP4 bgColor
ARGP4
ADDRGP4 CG_FillRect
CALLV
pop
ADDRFP4 0
INDIRF4
CNSTF4 1094713344
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 $94
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRFP4 4
ADDRFP4 4
INDIRF4
CNSTF4 1101004800
ADDF4
ASGNF4
ADDRFP4 0
INDIRF4
CNSTF4 1094713344
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 $95
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRLP4 24
INDIRI4
ARGI4
ADDRLP4 36
ADDRGP4 getStringForTime
CALLP4
ASGNP4
ADDRLP4 8
ADDRLP4 36
INDIRP4
ASGNP4
ADDRFP4 0
INDIRF4
CNSTF4 1120665600
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRLP4 8
INDIRP4
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRFP4 4
ADDRFP4 4
INDIRF4
CNSTF4 1101004800
ADDF4
ASGNF4
ADDRFP4 0
INDIRF4
CNSTF4 1094713344
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 $96
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRLP4 20
INDIRI4
CNSTI4 0
EQI4 $97
ADDRLP4 20
INDIRI4
ARGI4
ADDRLP4 40
ADDRGP4 getStringForTime
CALLP4
ASGNP4
ADDRLP4 8
ADDRLP4 40
INDIRP4
ASGNP4
ADDRFP4 0
INDIRF4
CNSTF4 1120665600
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRLP4 8
INDIRP4
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRGP4 $98
JUMPV
LABELV $97
ADDRFP4 0
INDIRF4
CNSTF4 1120665600
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 $99
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
LABELV $98
ADDRFP4 4
ADDRFP4 4
INDIRF4
CNSTF4 1101004800
ADDF4
ASGNF4
ADDRFP4 0
INDIRF4
CNSTF4 1094713344
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 $100
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRLP4 28
INDIRI4
ARGI4
ADDRLP4 40
ADDRGP4 getStringForTime
CALLP4
ASGNP4
ADDRLP4 8
ADDRLP4 40
INDIRP4
ASGNP4
ADDRFP4 0
INDIRF4
CNSTF4 1120665600
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRLP4 8
INDIRP4
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRFP4 4
ADDRFP4 4
INDIRF4
CNSTF4 1101004800
ADDF4
ASGNF4
ADDRGP4 cgs+31456
INDIRI4
CNSTI4 6
EQI4 $105
ADDRGP4 cgs+31456
INDIRI4
CNSTI4 7
NEI4 $101
LABELV $105
ADDRGP4 cg+111352
INDIRI4
CNSTI4 2000
ADDI4
ADDRGP4 cg+107604
INDIRI4
GEI4 $106
ADDRGP4 cg+111352
ADDRGP4 cg+107604
INDIRI4
ASGNI4
ADDRGP4 $112
ARGP4
ADDRGP4 trap_SendClientCommand
CALLV
pop
LABELV $106
ADDRLP4 44
CNSTI4 0
ASGNI4
ADDRLP4 16
ADDRLP4 44
INDIRI4
ASGNI4
ADDRLP4 0
ADDRLP4 44
INDIRI4
ASGNI4
ADDRGP4 $116
JUMPV
LABELV $113
CNSTI4 1868
ADDRLP4 0
INDIRI4
MULI4
ADDRGP4 cgs+40984
ADDP4
INDIRI4
CNSTI4 0
NEI4 $118
ADDRGP4 $114
JUMPV
LABELV $118
ADDRLP4 48
CNSTI4 1868
ASGNI4
ADDRLP4 48
INDIRI4
ADDRLP4 0
INDIRI4
MULI4
ADDRGP4 cgs+40984+68
ADDP4
INDIRI4
ADDRLP4 48
INDIRI4
ADDRGP4 cg+36
INDIRP4
CNSTI4 184
ADDP4
INDIRI4
MULI4
ADDRGP4 cgs+40984+68
ADDP4
INDIRI4
NEI4 $121
ADDRLP4 12
ADDRLP4 12
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
ADDRLP4 4
CNSTI4 884
ADDRLP4 0
INDIRI4
MULI4
ADDRGP4 cg_entities
ADDP4
ASGNP4
ADDRLP4 4
INDIRP4
CNSTI4 668
ADDP4
INDIRI4
CNSTI4 0
EQI4 $128
ADDRLP4 16
ADDRLP4 16
INDIRI4
ADDRLP4 4
INDIRP4
CNSTI4 668
ADDP4
INDIRI4
ADDRLP4 4
INDIRP4
CNSTI4 676
ADDP4
INDIRI4
SUBI4
ADDI4
ASGNI4
ADDRGP4 $129
JUMPV
LABELV $128
ADDRLP4 4
INDIRP4
CNSTI4 672
ADDP4
INDIRI4
CNSTI4 0
EQI4 $130
ADDRLP4 16
ADDRLP4 16
INDIRI4
ADDRGP4 cg+107604
INDIRI4
ADDRLP4 4
INDIRP4
CNSTI4 672
ADDP4
INDIRI4
SUBI4
ADDI4
ASGNI4
LABELV $130
LABELV $129
LABELV $121
LABELV $114
ADDRLP4 0
ADDRLP4 0
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
LABELV $116
ADDRLP4 0
INDIRI4
ADDRGP4 cgs+31484
INDIRI4
LTI4 $113
ADDRGP4 cgs+31456
INDIRI4
CNSTI4 7
NEI4 $133
ADDRLP4 48
CNSTI4 1868
ADDRGP4 cg+36
INDIRP4
CNSTI4 184
ADDP4
INDIRI4
MULI4
ADDRGP4 cgs+40984+68
ADDP4
INDIRI4
ASGNI4
ADDRLP4 48
INDIRI4
CNSTI4 1
LTI4 $137
ADDRLP4 48
INDIRI4
CNSTI4 4
GTI4 $137
ADDRLP4 48
INDIRI4
CNSTI4 2
LSHI4
ADDRGP4 $162-4
ADDP4
INDIRP4
JUMPV
lit
align 4
LABELV $162
address $142
address $147
address $152
address $157
code
LABELV $142
ADDRGP4 cgs+34804
INDIRI4
CNSTI4 0
LEI4 $137
ADDRLP4 16
ADDRLP4 16
INDIRI4
CNSTI4 5000
ADDRGP4 cgs+34804
INDIRI4
MULI4
SUBI4
ASGNI4
ADDRGP4 $137
JUMPV
LABELV $147
ADDRGP4 cgs+34808
INDIRI4
CNSTI4 0
LEI4 $137
ADDRLP4 16
ADDRLP4 16
INDIRI4
CNSTI4 5000
ADDRGP4 cgs+34808
INDIRI4
MULI4
SUBI4
ASGNI4
ADDRGP4 $137
JUMPV
LABELV $152
ADDRGP4 cgs+34812
INDIRI4
CNSTI4 0
LEI4 $137
ADDRLP4 16
ADDRLP4 16
INDIRI4
CNSTI4 5000
ADDRGP4 cgs+34812
INDIRI4
MULI4
SUBI4
ASGNI4
ADDRGP4 $137
JUMPV
LABELV $157
ADDRGP4 cgs+34816
INDIRI4
CNSTI4 0
LEI4 $137
ADDRLP4 16
ADDRLP4 16
INDIRI4
CNSTI4 5000
ADDRGP4 cgs+34816
INDIRI4
MULI4
SUBI4
ASGNI4
LABELV $137
LABELV $133
ADDRLP4 16
ADDRLP4 16
INDIRI4
ADDRLP4 12
INDIRI4
DIVI4
ASGNI4
ADDRFP4 0
INDIRF4
CNSTF4 1094713344
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 $164
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRLP4 16
INDIRI4
ARGI4
ADDRLP4 48
ADDRGP4 getStringForTime
CALLP4
ASGNP4
ADDRLP4 8
ADDRLP4 48
INDIRP4
ASGNP4
ADDRFP4 0
INDIRF4
CNSTF4 1120665600
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRLP4 8
INDIRP4
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
LABELV $101
LABELV $84
endproc CG_DrawHUD_Times 56 20
export CG_DrawHUD_Positions
proc CG_DrawHUD_Positions 28 20
ADDRFP4 0
ADDRFP4 0
INDIRF4
ASGNF4
ADDRFP4 4
ADDRFP4 4
INDIRF4
ASGNF4
ADDRLP4 12
CNSTI4 0
ASGNI4
ADDRLP4 4
ADDRLP4 12
INDIRI4
ASGNI4
ADDRLP4 0
ADDRLP4 12
INDIRI4
ASGNI4
LABELV $166
CNSTI4 -1
ARGI4
ADDRLP4 0
INDIRI4
CNSTI4 1
ADDI4
ARGI4
ADDRLP4 16
ADDRGP4 TeamCount
CALLI4
ASGNI4
ADDRLP4 16
INDIRI4
CNSTI4 0
NEI4 $170
ADDRGP4 $167
JUMPV
LABELV $170
ADDRLP4 4
ADDRLP4 4
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
LABELV $167
ADDRLP4 0
ADDRLP4 0
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
ADDRLP4 0
INDIRI4
CNSTI4 4
LTI4 $166
ADDRLP4 16
CNSTI4 0
ASGNI4
ADDRLP4 8
ADDRLP4 16
INDIRI4
ASGNI4
ADDRLP4 0
ADDRLP4 16
INDIRI4
ASGNI4
LABELV $172
ADDRLP4 0
INDIRI4
CNSTI4 1
ADDI4
ARGI4
ADDRLP4 20
ADDRGP4 GetTeamAtRank
CALLI4
ASGNI4
ADDRLP4 20
INDIRI4
CNSTI4 1868
ADDRGP4 cg+36
INDIRP4
CNSTI4 184
ADDP4
INDIRI4
MULI4
ADDRGP4 cgs+40984+68
ADDP4
INDIRI4
NEI4 $176
ADDRLP4 8
ADDRLP4 0
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
LABELV $176
LABELV $173
ADDRLP4 0
ADDRLP4 0
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
ADDRLP4 0
INDIRI4
CNSTI4 4
LTI4 $172
ADDRFP4 0
INDIRF4
ARGF4
ADDRFP4 4
INDIRF4
ARGF4
CNSTF4 1126825984
ARGF4
CNSTF4 1099956224
ARGF4
ADDRGP4 bgColor
ARGP4
ADDRGP4 CG_FillRect
CALLV
pop
ADDRFP4 0
INDIRF4
CNSTF4 1094713344
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 $181
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRFP4 4
ADDRFP4 4
INDIRF4
CNSTF4 1101004800
ADDF4
ASGNF4
ADDRFP4 0
INDIRF4
CNSTF4 1094713344
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 $182
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRGP4 $183
ARGP4
CNSTI4 884
ADDRGP4 cg+36
INDIRP4
CNSTI4 184
ADDP4
INDIRI4
MULI4
ADDRGP4 cg_entities+692
ADDP4
INDIRI4
ARGI4
ADDRGP4 cgs+163812
INDIRI4
ARGI4
ADDRLP4 20
ADDRGP4 va
CALLP4
ASGNP4
ADDRFP4 0
INDIRF4
CNSTF4 1120665600
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRLP4 20
INDIRP4
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRFP4 4
ADDRFP4 4
INDIRF4
CNSTF4 1101004800
ADDF4
ASGNF4
ADDRGP4 cgs+31456
INDIRI4
CNSTI4 6
EQI4 $191
ADDRGP4 cgs+31456
INDIRI4
CNSTI4 7
NEI4 $187
LABELV $191
ADDRFP4 0
INDIRF4
CNSTF4 1094713344
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 $164
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRGP4 $183
ARGP4
ADDRLP4 8
INDIRI4
ARGI4
ADDRLP4 4
INDIRI4
ARGI4
ADDRLP4 24
ADDRGP4 va
CALLP4
ASGNP4
ADDRFP4 0
INDIRF4
CNSTF4 1120665600
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRLP4 24
INDIRP4
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
LABELV $187
LABELV $165
endproc CG_DrawHUD_Positions 28 20
export CG_DrawHUD_Laps
proc CG_DrawHUD_Laps 4 20
ADDRFP4 0
ADDRFP4 0
INDIRF4
ASGNF4
ADDRFP4 4
ADDRFP4 4
INDIRF4
ASGNF4
ADDRFP4 0
INDIRF4
ARGF4
ADDRFP4 4
INDIRF4
ARGF4
CNSTF4 1126825984
ARGF4
CNSTF4 1099956224
ARGF4
ADDRGP4 bgColor
ARGP4
ADDRGP4 CG_FillRect
CALLV
pop
ADDRFP4 0
INDIRF4
CNSTF4 1094713344
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 $95
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRGP4 $183
ARGP4
CNSTI4 884
ADDRGP4 cg+36
INDIRP4
CNSTI4 184
ADDP4
INDIRI4
MULI4
ADDRGP4 cg_entities+696
ADDP4
INDIRI4
ARGI4
ADDRGP4 cgs+31472
INDIRI4
ARGI4
ADDRLP4 0
ADDRGP4 va
CALLP4
ASGNP4
ADDRFP4 0
INDIRF4
CNSTF4 1120665600
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
LABELV $192
endproc CG_DrawHUD_Laps 4 20
export CG_DrawHUD_OpponentList
proc CG_DrawHUD_OpponentList 200 20
ADDRFP4 0
ADDRFP4 0
INDIRF4
ASGNF4
ADDRFP4 4
ADDRFP4 4
INDIRF4
ASGNF4
ADDRLP4 176
CNSTI4 884
ADDRGP4 cg+36
INDIRP4
CNSTI4 184
ADDP4
INDIRI4
MULI4
ADDRGP4 cg_entities
ADDP4
ASGNP4
ADDRLP4 176
INDIRP4
CNSTI4 692
ADDP4
INDIRI4
CNSTI4 4
SUBI4
CNSTI4 1
GEI4 $199
ADDRLP4 180
CNSTI4 1
ASGNI4
ADDRGP4 $200
JUMPV
LABELV $199
ADDRLP4 180
ADDRLP4 176
INDIRP4
CNSTI4 692
ADDP4
INDIRI4
CNSTI4 4
SUBI4
ASGNI4
LABELV $200
ADDRLP4 172
ADDRLP4 180
INDIRI4
ASGNI4
ADDRLP4 172
INDIRI4
CNSTI4 8
ADDI4
ADDRGP4 cgs+163812
INDIRI4
LEI4 $204
ADDRLP4 184
ADDRGP4 cgs+163812
INDIRI4
ASGNI4
ADDRGP4 $205
JUMPV
LABELV $204
ADDRLP4 184
ADDRLP4 172
INDIRI4
CNSTI4 8
ADDI4
ASGNI4
LABELV $205
ADDRLP4 160
ADDRLP4 184
INDIRI4
ASGNI4
ADDRLP4 160
INDIRI4
CNSTI4 8
SUBI4
CNSTI4 1
GEI4 $207
ADDRLP4 188
CNSTI4 1
ASGNI4
ADDRGP4 $208
JUMPV
LABELV $207
ADDRLP4 188
ADDRLP4 160
INDIRI4
CNSTI4 8
SUBI4
ASGNI4
LABELV $208
ADDRLP4 172
ADDRLP4 188
INDIRI4
ASGNI4
ADDRLP4 164
CNSTF4 1128660992
ASGNF4
ADDRLP4 168
CNSTF4 1099956224
ASGNF4
ADDRFP4 0
INDIRF4
ARGF4
ADDRFP4 4
INDIRF4
ARGF4
ADDRLP4 164
INDIRF4
ARGF4
ADDRLP4 168
INDIRF4
ARGF4
ADDRGP4 bgColor
ARGP4
ADDRGP4 CG_FillRect
CALLV
pop
ADDRFP4 0
INDIRF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRGP4 $209
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRGP4 $183
ARGP4
ADDRLP4 176
INDIRP4
CNSTI4 692
ADDP4
INDIRI4
ARGI4
ADDRGP4 cgs+163812
INDIRI4
ARGI4
ADDRLP4 192
ADDRGP4 va
CALLP4
ASGNP4
ADDRFP4 0
INDIRF4
CNSTF4 1118044160
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRLP4 192
INDIRP4
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRFP4 4
ADDRFP4 4
INDIRF4
CNSTF4 1101004800
ADDF4
ASGNF4
ADDRLP4 8
ADDRLP4 172
INDIRI4
ASGNI4
ADDRGP4 $214
JUMPV
LABELV $211
ADDRLP4 12
CNSTI4 -1
ASGNI4
ADDRLP4 4
CNSTI4 0
ASGNI4
ADDRGP4 $218
JUMPV
LABELV $215
ADDRLP4 0
CNSTI4 884
ADDRLP4 4
INDIRI4
MULI4
ADDRGP4 cg_entities
ADDP4
ASGNP4
ADDRLP4 0
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $220
ADDRGP4 $216
JUMPV
LABELV $220
CNSTI4 1868
ADDRLP4 0
INDIRP4
CNSTI4 168
ADDP4
INDIRI4
MULI4
ADDRGP4 cgs+40984+68
ADDP4
INDIRI4
CNSTI4 5
NEI4 $222
ADDRGP4 $216
JUMPV
LABELV $222
CNSTI4 1868
ADDRLP4 0
INDIRP4
CNSTI4 168
ADDP4
INDIRI4
MULI4
ADDRGP4 cgs+40984+1864
ADDP4
INDIRI4
ADDRLP4 8
INDIRI4
NEI4 $226
ADDRLP4 12
ADDRLP4 0
INDIRP4
CNSTI4 168
ADDP4
INDIRI4
ASGNI4
ADDRGP4 $217
JUMPV
LABELV $226
LABELV $216
ADDRLP4 4
ADDRLP4 4
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
LABELV $218
ADDRLP4 4
INDIRI4
ADDRGP4 cgs+31484
INDIRI4
LTI4 $215
LABELV $217
ADDRLP4 12
INDIRI4
CNSTI4 0
LTI4 $233
ADDRLP4 12
INDIRI4
ADDRGP4 cgs+31484
INDIRI4
LTI4 $230
LABELV $233
ADDRGP4 $212
JUMPV
LABELV $230
CNSTI4 1868
ADDRLP4 12
INDIRI4
MULI4
ADDRGP4 cgs+40984+68
ADDP4
INDIRI4
CNSTI4 1
NEI4 $234
ADDRLP4 16
ADDRGP4 colorRed
INDIRF4
ASGNF4
ADDRLP4 16+4
ADDRGP4 colorRed+4
INDIRF4
ASGNF4
ADDRLP4 16+8
ADDRGP4 colorRed+8
INDIRF4
ASGNF4
ADDRLP4 16+12
ADDRGP4 colorRed+12
INDIRF4
ASGNF4
ADDRLP4 16+12
CNSTF4 1056964608
ASGNF4
ADDRGP4 $235
JUMPV
LABELV $234
CNSTI4 1868
ADDRLP4 12
INDIRI4
MULI4
ADDRGP4 cgs+40984+68
ADDP4
INDIRI4
CNSTI4 2
NEI4 $245
ADDRLP4 16
ADDRGP4 colorBlue
INDIRF4
ASGNF4
ADDRLP4 16+4
ADDRGP4 colorBlue+4
INDIRF4
ASGNF4
ADDRLP4 16+8
ADDRGP4 colorBlue+8
INDIRF4
ASGNF4
ADDRLP4 16+12
ADDRGP4 colorBlue+12
INDIRF4
ASGNF4
ADDRLP4 16+12
CNSTF4 1056964608
ASGNF4
ADDRGP4 $246
JUMPV
LABELV $245
CNSTI4 1868
ADDRLP4 12
INDIRI4
MULI4
ADDRGP4 cgs+40984+68
ADDP4
INDIRI4
CNSTI4 3
NEI4 $256
ADDRLP4 16
ADDRGP4 colorGreen
INDIRF4
ASGNF4
ADDRLP4 16+4
ADDRGP4 colorGreen+4
INDIRF4
ASGNF4
ADDRLP4 16+8
ADDRGP4 colorGreen+8
INDIRF4
ASGNF4
ADDRLP4 16+12
ADDRGP4 colorGreen+12
INDIRF4
ASGNF4
ADDRLP4 16+12
CNSTF4 1056964608
ASGNF4
ADDRGP4 $257
JUMPV
LABELV $256
CNSTI4 1868
ADDRLP4 12
INDIRI4
MULI4
ADDRGP4 cgs+40984+68
ADDP4
INDIRI4
CNSTI4 4
NEI4 $267
ADDRLP4 16
ADDRGP4 colorYellow
INDIRF4
ASGNF4
ADDRLP4 16+4
ADDRGP4 colorYellow+4
INDIRF4
ASGNF4
ADDRLP4 16+8
ADDRGP4 colorYellow+8
INDIRF4
ASGNF4
ADDRLP4 16+12
ADDRGP4 colorYellow+12
INDIRF4
ASGNF4
ADDRLP4 16+12
CNSTF4 1056964608
ASGNF4
ADDRGP4 $268
JUMPV
LABELV $267
ADDRLP4 16
ADDRGP4 bgColor
INDIRF4
ASGNF4
ADDRLP4 16+4
ADDRGP4 bgColor+4
INDIRF4
ASGNF4
ADDRLP4 16+8
ADDRGP4 bgColor+8
INDIRF4
ASGNF4
ADDRLP4 16+12
ADDRGP4 bgColor+12
INDIRF4
ASGNF4
LABELV $268
LABELV $257
LABELV $246
LABELV $235
ADDRFP4 0
INDIRF4
ARGF4
ADDRFP4 4
INDIRF4
ARGF4
ADDRLP4 164
INDIRF4
ARGF4
ADDRLP4 168
INDIRF4
ARGF4
ADDRLP4 16
ARGP4
ADDRGP4 CG_FillRect
CALLV
pop
ADDRLP4 96
ARGP4
CNSTI4 1868
ADDRLP4 12
INDIRI4
MULI4
ADDRGP4 cgs+40984+4
ADDP4
ARGP4
CNSTI4 16
ARGI4
ADDRGP4 Q_strncpyz
CALLV
pop
ADDRLP4 32
ARGP4
CNSTI4 64
ARGI4
ADDRGP4 $286
ARGP4
CNSTI4 1868
ADDRLP4 12
INDIRI4
MULI4
ADDRGP4 cgs+40984+1864
ADDP4
INDIRI4
ARGI4
ADDRLP4 96
ARGP4
ADDRGP4 Com_sprintf
CALLI4
pop
ADDRFP4 0
INDIRF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CVFI4 4
ARGI4
ADDRLP4 32
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRFP4 4
ADDRFP4 4
INDIRF4
CNSTF4 1101004800
ADDF4
ASGNF4
LABELV $212
ADDRLP4 8
ADDRLP4 8
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
LABELV $214
ADDRLP4 8
INDIRI4
ADDRLP4 160
INDIRI4
LEI4 $211
LABELV $196
endproc CG_DrawHUD_OpponentList 200 20
export CG_DrawHUD_Scores
proc CG_DrawHUD_Scores 36 20
ADDRFP4 0
ADDRFP4 0
INDIRF4
ASGNF4
ADDRFP4 4
ADDRFP4 4
INDIRF4
ASGNF4
ADDRGP4 $290
ARGP4
CNSTI4 1868
ADDRGP4 cg+36
INDIRP4
CNSTI4 184
ADDP4
INDIRI4
MULI4
ADDRGP4 cgs+40984+108
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
ADDRLP4 0
INDIRP4
ARGP4
ADDRLP4 16
ADDRGP4 CG_DrawStrlen
CALLI4
ASGNI4
ADDRLP4 4
ADDRLP4 16
INDIRI4
CNSTI4 5
LSHI4
ASGNI4
ADDRFP4 0
INDIRF4
ARGF4
ADDRFP4 4
INDIRF4
ARGF4
CNSTF4 1119879168
ARGF4
CNSTF4 1116733440
ARGF4
ADDRGP4 bgColor
ARGP4
ADDRGP4 CG_FillRect
CALLV
pop
ADDRFP4 0
INDIRF4
CNSTF4 1101004800
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CNSTF4 1086324736
ADDF4
CVFI4 4
ARGI4
ADDRGP4 $294
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRFP4 0
INDIRF4
CNSTF4 1111490560
ADDF4
ADDRLP4 4
INDIRI4
CNSTI4 2
DIVI4
CVIF4 4
SUBF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CNSTF4 1104150528
ADDF4
CVFI4 4
ARGI4
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawGiantDigitalStringColor
CALLV
pop
ADDRFP4 4
ADDRFP4 4
INDIRF4
CNSTF4 1116733440
ADDF4
ASGNF4
ADDRGP4 cgs+31456
INDIRI4
CNSTI4 5
LTI4 $295
ADDRLP4 20
ADDRGP4 cg+36
INDIRP4
CNSTI4 304
ADDP4
INDIRI4
ASGNI4
ADDRLP4 20
INDIRI4
CNSTI4 1
LTI4 $298
ADDRLP4 20
INDIRI4
CNSTI4 4
GTI4 $298
ADDRLP4 20
INDIRI4
CNSTI4 2
LSHI4
ADDRGP4 $310-4
ADDP4
INDIRP4
JUMPV
lit
align 4
LABELV $310
address $302
address $304
address $306
address $308
code
LABELV $298
LABELV $302
ADDRLP4 8
ADDRGP4 cgs+34804
INDIRI4
ASGNI4
ADDRGP4 $299
JUMPV
LABELV $304
ADDRLP4 8
ADDRGP4 cgs+34808
INDIRI4
ASGNI4
ADDRGP4 $299
JUMPV
LABELV $306
ADDRLP4 8
ADDRGP4 cgs+34812
INDIRI4
ASGNI4
ADDRGP4 $299
JUMPV
LABELV $308
ADDRLP4 8
ADDRGP4 cgs+34816
INDIRI4
ASGNI4
LABELV $299
ADDRGP4 $290
ARGP4
ADDRLP4 8
INDIRI4
ARGI4
ADDRLP4 28
ADDRGP4 va
CALLP4
ASGNP4
ADDRLP4 0
ADDRLP4 28
INDIRP4
ASGNP4
ADDRLP4 0
INDIRP4
ARGP4
ADDRLP4 32
ADDRGP4 CG_DrawStrlen
CALLI4
ASGNI4
ADDRLP4 4
ADDRLP4 32
INDIRI4
CNSTI4 5
LSHI4
ASGNI4
ADDRFP4 0
INDIRF4
ARGF4
ADDRFP4 4
INDIRF4
ARGF4
CNSTF4 1119879168
ARGF4
CNSTF4 1117519872
ARGF4
ADDRGP4 bgColor
ARGP4
ADDRGP4 CG_FillRect
CALLV
pop
ADDRFP4 0
INDIRF4
CNSTF4 1103101952
ADDF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CNSTF4 1086324736
ADDF4
CVFI4 4
ARGI4
ADDRGP4 $164
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawSmallDigitalStringColor
CALLV
pop
ADDRFP4 0
INDIRF4
CNSTF4 1111490560
ADDF4
ADDRLP4 4
INDIRI4
CNSTI4 2
DIVI4
CVIF4 4
SUBF4
CVFI4 4
ARGI4
ADDRFP4 4
INDIRF4
CNSTF4 1104150528
ADDF4
CVFI4 4
ARGI4
ADDRLP4 0
INDIRP4
ARGP4
ADDRGP4 colorWhite
ARGP4
ADDRGP4 CG_DrawGiantDigitalStringColor
CALLV
pop
ADDRFP4 4
ADDRFP4 4
INDIRF4
CNSTF4 1117519872
ADDF4
ASGNF4
LABELV $295
ADDRFP4 0
INDIRF4
CNSTF4 1119879168
ADDF4
ARGF4
ADDRFP4 4
INDIRF4
ARGF4
ADDRGP4 CG_DrawScores
CALLF4
pop
LABELV $289
endproc CG_DrawHUD_Scores 36 20
export CG_DrawHUD
proc CG_DrawHUD 8 8
ADDRGP4 cg_paused+12
INDIRI4
CNSTI4 0
EQI4 $313
CNSTI4 0
RETI4
ADDRGP4 $312
JUMPV
LABELV $313
ADDRGP4 cg+126160
INDIRI4
CNSTI4 0
NEI4 $316
CNSTI4 0
RETI4
ADDRGP4 $312
JUMPV
LABELV $316
ADDRGP4 cg+111352
INDIRI4
CNSTI4 2000
ADDI4
ADDRGP4 cg+107604
INDIRI4
GEI4 $319
ADDRGP4 cg+111352
ADDRGP4 cg+107604
INDIRI4
ASGNI4
ADDRGP4 $112
ARGP4
ADDRGP4 trap_SendClientCommand
CALLV
pop
LABELV $319
ADDRLP4 0
ADDRGP4 cgs+31456
INDIRI4
ASGNI4
ADDRLP4 0
INDIRI4
CNSTI4 0
LTI4 $325
ADDRLP4 0
INDIRI4
CNSTI4 8
GTI4 $325
ADDRLP4 0
INDIRI4
CNSTI4 2
LSHI4
ADDRGP4 $333
ADDP4
INDIRP4
JUMPV
lit
align 4
LABELV $333
address $329
address $330
address $325
address $326
address $331
address $331
address $329
address $330
address $331
code
LABELV $325
LABELV $329
CNSTF4 0
ARGF4
CNSTF4 1121976320
ARGF4
ADDRGP4 CG_DrawHUD_Times
CALLV
pop
CNSTF4 0
ARGF4
CNSTF4 1130627072
ARGF4
ADDRGP4 CG_DrawHUD_Positions
CALLV
pop
CNSTF4 0
ARGF4
CNSTF4 1134034944
ARGF4
ADDRGP4 CG_DrawHUD_Laps
CALLV
pop
CNSTF4 1138491392
ARGF4
CNSTF4 1124204544
ARGF4
ADDRGP4 CG_DrawHUD_OpponentList
CALLV
pop
ADDRGP4 $326
JUMPV
LABELV $330
CNSTF4 0
ARGF4
CNSTF4 1121976320
ARGF4
ADDRGP4 CG_DrawHUD_Times
CALLV
pop
CNSTF4 0
ARGF4
CNSTF4 1130627072
ARGF4
ADDRGP4 CG_DrawHUD_Positions
CALLV
pop
CNSTF4 0
ARGF4
CNSTF4 1134034944
ARGF4
ADDRGP4 CG_DrawHUD_Laps
CALLV
pop
CNSTF4 1138491392
ARGF4
CNSTF4 1124204544
ARGF4
ADDRGP4 CG_DrawHUD_OpponentList
CALLV
pop
CNSTF4 1132724224
ARGF4
CNSTF4 1124204544
ARGF4
ADDRGP4 CG_DrawHUD_Scores
CALLV
pop
ADDRGP4 $326
JUMPV
LABELV $331
CNSTF4 1132724224
ARGF4
CNSTF4 1124204544
ARGF4
ADDRGP4 CG_DrawHUD_Scores
CALLV
pop
LABELV $326
CNSTI4 1
RETI4
LABELV $312
endproc CG_DrawHUD 8 8
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
import trap_CM_TransformedBoxTrace
import trap_CM_BoxTrace
import trap_CM_TransformedPointContents
import trap_CM_PointContents
import trap_CM_TempBoxModel
import trap_CM_InlineModel
import trap_CM_NumInlineModels
import trap_CM_LoadMap
import trap_UpdateScreen
import trap_SendClientCommand
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
import cg_noTaunt
import cg_bigFont
import cg_smallFont
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
LABELV $294
byte 1 70
byte 1 82
byte 1 65
byte 1 71
byte 1 83
byte 1 58
byte 1 0
align 1
LABELV $290
byte 1 37
byte 1 105
byte 1 0
align 1
LABELV $286
byte 1 32
byte 1 37
byte 1 105
byte 1 58
byte 1 32
byte 1 37
byte 1 115
byte 1 0
align 1
LABELV $209
byte 1 80
byte 1 79
byte 1 83
byte 1 58
byte 1 0
align 1
LABELV $183
byte 1 37
byte 1 105
byte 1 47
byte 1 37
byte 1 105
byte 1 0
align 1
LABELV $182
byte 1 89
byte 1 79
byte 1 85
byte 1 58
byte 1 0
align 1
LABELV $181
byte 1 80
byte 1 79
byte 1 83
byte 1 73
byte 1 84
byte 1 73
byte 1 79
byte 1 78
byte 1 0
align 1
LABELV $164
byte 1 84
byte 1 69
byte 1 65
byte 1 77
byte 1 58
byte 1 0
align 1
LABELV $112
byte 1 115
byte 1 99
byte 1 111
byte 1 114
byte 1 101
byte 1 0
align 1
LABELV $100
byte 1 84
byte 1 79
byte 1 84
byte 1 65
byte 1 76
byte 1 58
byte 1 0
align 1
LABELV $99
byte 1 78
byte 1 47
byte 1 65
byte 1 0
align 1
LABELV $96
byte 1 76
byte 1 65
byte 1 83
byte 1 84
byte 1 58
byte 1 0
align 1
LABELV $95
byte 1 76
byte 1 65
byte 1 80
byte 1 58
byte 1 0
align 1
LABELV $94
byte 1 84
byte 1 73
byte 1 77
byte 1 69
byte 1 83
byte 1 0
