data
align 4
LABELV allocPoint
byte 4 0
code
proc CG_Alloc 12 8
ADDRGP4 allocPoint
INDIRI4
ADDRFP4 0
INDIRI4
ADDI4
CNSTI4 524288
LEI4 $85
ADDRGP4 $87
ARGP4
ADDRFP4 0
INDIRI4
ARGI4
ADDRGP4 CG_Error
CALLV
pop
CNSTP4 0
RETP4
ADDRGP4 $84
JUMPV
LABELV $85
ADDRLP4 4
ADDRGP4 allocPoint
ASGNP4
ADDRLP4 8
ADDRLP4 4
INDIRP4
INDIRI4
ASGNI4
ADDRLP4 0
ADDRLP4 8
INDIRI4
ADDRGP4 memoryPool
ADDP4
ASGNP4
ADDRLP4 4
INDIRP4
ADDRLP4 8
INDIRI4
ADDRFP4 0
INDIRI4
CNSTI4 31
ADDI4
CNSTI4 -32
BANDI4
ADDI4
ASGNI4
ADDRLP4 0
INDIRP4
RETP4
LABELV $84
endproc CG_Alloc 12 8
lit
align 1
LABELV $90
byte 1 0
byte 1 0
byte 1 2
byte 1 0
byte 1 0
byte 1 0
byte 1 0
byte 1 0
byte 1 0
byte 1 0
byte 1 0
byte 1 0
export LoadTGA
code
proc LoadTGA 72 12
ADDRFP4 0
ADDRFP4 0
INDIRP4
ASGNP4
ADDRFP4 4
ADDRFP4 4
INDIRP4
ASGNP4
ADDRLP4 24
ADDRGP4 $90
INDIRB
ASGNB 12
ADDRFP4 4
INDIRP4
ARGP4
ADDRLP4 8
ARGP4
CNSTI4 0
ARGI4
ADDRGP4 trap_FS_FOpenFile
CALLI4
pop
ADDRLP4 8
INDIRI4
CNSTI4 0
NEI4 $91
ADDRGP4 $93
ARGP4
ADDRFP4 4
INDIRP4
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
CNSTI4 0
RETI4
ADDRGP4 $89
JUMPV
LABELV $91
ADDRLP4 12
ARGP4
CNSTI4 12
ARGI4
ADDRLP4 8
INDIRI4
ARGI4
ADDRGP4 trap_FS_Read
CALLV
pop
ADDRLP4 24
ARGP4
ADDRLP4 12
ARGP4
CNSTU4 12
ARGU4
ADDRLP4 44
ADDRGP4 memcmp
CALLI4
ASGNI4
ADDRLP4 44
INDIRI4
CNSTI4 0
EQI4 $94
ADDRLP4 8
INDIRI4
ARGI4
ADDRGP4 trap_FS_FCloseFile
CALLV
pop
ADDRLP4 12+2
INDIRU1
CVUI4 1
CNSTI4 10
NEI4 $96
ADDRGP4 $99
ARGP4
ADDRFP4 4
INDIRP4
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
ADDRGP4 $97
JUMPV
LABELV $96
ADDRGP4 $100
ARGP4
ADDRFP4 4
INDIRP4
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
LABELV $97
CNSTI4 0
RETI4
ADDRGP4 $89
JUMPV
LABELV $94
ADDRLP4 0
ARGP4
CNSTI4 6
ARGI4
ADDRLP4 8
INDIRI4
ARGI4
ADDRGP4 trap_FS_Read
CALLV
pop
ADDRLP4 48
CNSTI4 8
ASGNI4
ADDRFP4 0
INDIRP4
ADDRLP4 48
INDIRI4
ADDP4
ADDRLP4 0+1
INDIRU1
CVUI4 1
ADDRLP4 48
INDIRI4
LSHI4
ADDRLP4 0
INDIRU1
CVUI4 1
ADDI4
ASGNI4
ADDRFP4 0
INDIRP4
CNSTI4 12
ADDP4
ADDRLP4 0+3
INDIRU1
CVUI4 1
CNSTI4 8
LSHI4
ADDRLP4 0+2
INDIRU1
CVUI4 1
ADDI4
ASGNI4
ADDRLP4 56
CNSTI4 0
ASGNI4
ADDRFP4 0
INDIRP4
CNSTI4 8
ADDP4
INDIRI4
ADDRLP4 56
INDIRI4
LEI4 $109
ADDRFP4 0
INDIRP4
CNSTI4 12
ADDP4
INDIRI4
ADDRLP4 56
INDIRI4
LEI4 $109
ADDRLP4 0+4
INDIRU1
CVUI4 1
CNSTI4 24
EQI4 $104
ADDRLP4 0+4
INDIRU1
CVUI4 1
CNSTI4 32
EQI4 $104
LABELV $109
ADDRLP4 8
INDIRI4
ARGI4
ADDRGP4 trap_FS_FCloseFile
CALLV
pop
ADDRGP4 $110
ARGP4
ADDRFP4 4
INDIRP4
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
CNSTI4 0
RETI4
ADDRGP4 $89
JUMPV
LABELV $104
ADDRFP4 0
INDIRP4
CNSTI4 4
ADDP4
ADDRLP4 0+4
INDIRU1
CVUI4 1
ASGNI4
ADDRLP4 64
CNSTI4 8
ASGNI4
ADDRLP4 40
ADDRFP4 0
INDIRP4
CNSTI4 4
ADDP4
INDIRI4
ADDRLP4 64
INDIRI4
DIVI4
ASGNI4
ADDRLP4 36
ADDRFP4 0
INDIRP4
ADDRLP4 64
INDIRI4
ADDP4
INDIRI4
ADDRFP4 0
INDIRP4
CNSTI4 12
ADDP4
INDIRI4
MULI4
ADDRLP4 40
INDIRI4
MULI4
ASGNI4
ADDRLP4 36
INDIRI4
ARGI4
ADDRLP4 68
ADDRGP4 CG_Alloc
CALLP4
ASGNP4
ADDRFP4 0
INDIRP4
ADDRLP4 68
INDIRP4
ASGNP4
ADDRFP4 0
INDIRP4
INDIRP4
ARGP4
ADDRLP4 36
INDIRI4
ARGI4
ADDRLP4 8
INDIRI4
ARGI4
ADDRGP4 trap_FS_Read
CALLV
pop
ADDRFP4 0
INDIRP4
INDIRP4
CVPU4 4
CNSTU4 0
NEU4 $112
ADDRLP4 8
INDIRI4
ARGI4
ADDRGP4 trap_FS_FCloseFile
CALLV
pop
ADDRGP4 $114
ARGP4
ADDRFP4 4
INDIRP4
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
CNSTI4 0
RETI4
ADDRGP4 $89
JUMPV
LABELV $112
ADDRLP4 8
INDIRI4
ARGI4
ADDRGP4 trap_FS_FCloseFile
CALLV
pop
CNSTI4 1
RETI4
LABELV $89
endproc LoadTGA 72 12
proc WriteNameOnTexture 232 12
ADDRFP4 0
ADDRFP4 0
INDIRP4
ASGNP4
ADDRFP4 8
ADDRFP4 8
INDIRI4
ASGNI4
ADDRLP4 32
ARGP4
ADDRGP4 $118
ARGP4
ADDRLP4 108
ADDRGP4 LoadTGA
CALLI4
ASGNI4
ADDRLP4 108
INDIRI4
CNSTI4 0
NEI4 $116
CNSTI4 0
RETI4
ADDRGP4 $115
JUMPV
LABELV $116
ADDRLP4 112
CNSTI4 8
ASGNI4
ADDRLP4 64
ADDRLP4 32+4
INDIRI4
ADDRLP4 112
INDIRI4
DIVI4
ASGNI4
ADDRLP4 48
ADDRFP4 0
INDIRP4
CNSTI4 4
ADDP4
INDIRI4
ADDRLP4 112
INDIRI4
DIVI4
ASGNI4
ADDRLP4 104
ADDRFP4 0
INDIRP4
ADDRLP4 112
INDIRI4
ADDP4
INDIRI4
CNSTI4 6
DIVI4
CNSTI4 1
SUBI4
ASGNI4
ADDRLP4 104
INDIRI4
ADDRFP4 8
INDIRI4
GEI4 $120
ADDRFP4 8
ADDRLP4 104
INDIRI4
ASGNI4
LABELV $120
ADDRFP4 4
INDIRP4
ARGP4
ADDRLP4 120
ADDRGP4 strlen
CALLU4
ASGNU4
ADDRLP4 104
ADDRLP4 120
INDIRU4
CVUI4 4
ASGNI4
ADDRLP4 104
INDIRI4
ADDRFP4 8
INDIRI4
LEI4 $122
ADDRLP4 104
ADDRFP4 8
INDIRI4
ASGNI4
LABELV $122
ADDRLP4 124
CNSTF4 1073741824
ASGNF4
ADDRLP4 84
ADDRFP4 0
INDIRP4
CNSTI4 8
ADDP4
INDIRI4
CVIF4 4
ADDRLP4 124
INDIRF4
DIVF4
CNSTI4 7
ADDRLP4 104
INDIRI4
MULI4
CVIF4 4
ADDRLP4 124
INDIRF4
DIVF4
SUBF4
CNSTF4 1077936128
SUBF4
CVFI4 4
CVIF4 4
ASGNF4
ADDRLP4 84
INDIRF4
CNSTF4 3225419776
GEF4 $124
ADDRLP4 84
CNSTF4 3225419776
ASGNF4
LABELV $124
ADDRLP4 96
CNSTF4 1093664768
ASGNF4
ADDRLP4 76
CNSTF4 1098907648
ASGNF4
ADDRLP4 128
CNSTF4 0
ASGNF4
ADDRLP4 12
ADDRLP4 128
INDIRF4
ASGNF4
ADDRLP4 12+4
ADDRLP4 128
INDIRF4
ASGNF4
ADDRLP4 12+8
CNSTF4 0
ASGNF4
ADDRLP4 12+12
CNSTF4 1065353216
ASGNF4
ADDRLP4 68
ADDRFP4 4
INDIRP4
ASGNP4
ADDRLP4 100
CNSTI4 0
ASGNI4
ADDRGP4 $130
JUMPV
LABELV $129
ADDRLP4 68
INDIRP4
CVPU4 4
CNSTU4 0
EQU4 $132
ADDRLP4 68
INDIRP4
INDIRI1
CVII4 1
CNSTI4 94
NEI4 $132
ADDRLP4 136
ADDRLP4 68
INDIRP4
CNSTI4 1
ADDP4
INDIRI1
CVII4 1
ASGNI4
ADDRLP4 136
INDIRI4
CNSTI4 0
EQI4 $132
ADDRLP4 136
INDIRI4
CNSTI4 65
LTI4 $135
ADDRLP4 136
INDIRI4
CNSTI4 90
LEI4 $134
LABELV $135
ADDRLP4 140
ADDRLP4 68
INDIRP4
CNSTI4 1
ADDP4
INDIRI1
CVII4 1
ASGNI4
ADDRLP4 140
INDIRI4
CNSTI4 97
LTI4 $136
ADDRLP4 140
INDIRI4
CNSTI4 122
LEI4 $134
LABELV $136
ADDRLP4 144
ADDRLP4 68
INDIRP4
CNSTI4 1
ADDP4
INDIRI1
CVII4 1
ASGNI4
ADDRLP4 144
INDIRI4
CNSTI4 48
LTI4 $132
ADDRLP4 144
INDIRI4
CNSTI4 57
GTI4 $132
LABELV $134
ADDRLP4 12
ARGP4
ADDRLP4 68
INDIRP4
CNSTI4 1
ADDP4
INDIRI1
CVII4 1
CNSTI4 48
SUBI4
CNSTI4 7
BANDI4
CNSTI4 4
LSHI4
ADDRGP4 g_color_table
ADDP4
ARGP4
CNSTU4 16
ARGU4
ADDRGP4 memcpy
CALLP4
pop
ADDRLP4 68
ADDRLP4 68
INDIRP4
CNSTI4 2
ADDP4
ASGNP4
ADDRGP4 $130
JUMPV
LABELV $132
ADDRLP4 88
ADDRLP4 68
INDIRP4
INDIRI1
CVII4 1
CNSTI4 127
BANDI4
CVIU4 4
CVUU1 4
ASGNU1
ADDRLP4 88
INDIRU1
CVUI4 1
CNSTI4 32
NEI4 $137
ADDRLP4 28
CNSTF4 1096810496
ASGNF4
ADDRGP4 $138
JUMPV
LABELV $137
CNSTI4 12
ADDRLP4 88
INDIRU1
CVUI4 1
MULI4
ADDRGP4 propMap+8
ADDP4
INDIRI4
CNSTI4 -1
EQI4 $139
ADDRLP4 148
CNSTF4 1098907648
ASGNF4
ADDRLP4 152
ADDRLP4 88
INDIRU1
CVUI4 1
ASGNI4
ADDRLP4 92
ADDRLP4 148
INDIRF4
ADDRLP4 152
INDIRI4
CNSTI4 4
RSHI4
CVIF4 4
MULF4
ASGNF4
ADDRLP4 56
ADDRLP4 148
INDIRF4
ADDRLP4 152
INDIRI4
CNSTI4 15
BANDI4
CVIF4 4
MULF4
ASGNF4
ADDRLP4 60
ADDRLP4 148
INDIRF4
ASGNF4
ADDRLP4 80
ADDRLP4 148
INDIRF4
ASGNF4
ADDRLP4 28
CNSTF4 1096810496
ASGNF4
ADDRLP4 72
CNSTI4 0
ASGNI4
ADDRGP4 $145
JUMPV
LABELV $142
ADDRLP4 164
ADDRLP4 72
INDIRI4
CVIF4 4
ASGNF4
ADDRLP4 0
ADDRFP4 0
INDIRP4
CNSTI4 12
ADDP4
INDIRI4
CVIF4 4
ADDRLP4 96
INDIRF4
ADDRLP4 76
INDIRF4
ADDF4
ADDRLP4 164
INDIRF4
SUBF4
SUBF4
ADDRFP4 0
INDIRP4
CNSTI4 8
ADDP4
INDIRI4
CVIF4 4
MULF4
ADDRLP4 84
INDIRF4
ADDF4
CVFI4 4
ADDRLP4 48
INDIRI4
MULI4
ASGNI4
ADDRLP4 52
ADDRLP4 32+12
INDIRI4
CVIF4 4
ADDRLP4 92
INDIRF4
ADDRLP4 80
INDIRF4
ADDF4
ADDRLP4 80
INDIRF4
ADDRLP4 164
INDIRF4
ADDRLP4 76
INDIRF4
DIVF4
MULF4
SUBF4
SUBF4
CVFI4 4
ADDRLP4 32+8
INDIRI4
MULI4
ASGNI4
ADDRLP4 8
CNSTI4 0
ASGNI4
ADDRGP4 $151
JUMPV
LABELV $148
ADDRLP4 4
ADDRLP4 52
INDIRI4
ADDRLP4 56
INDIRF4
ADDRLP4 60
INDIRF4
ADDRLP4 8
INDIRI4
CVIF4 4
ADDRLP4 28
INDIRF4
DIVF4
MULF4
ADDF4
CVFI4 4
ADDI4
ADDRLP4 64
INDIRI4
MULI4
CNSTI4 3
ADDI4
ADDRLP4 32
INDIRP4
ADDP4
INDIRU1
CVUI4 1
CVIF4 4
ASGNF4
ADDRLP4 176
ADDRLP4 0
INDIRI4
ADDRFP4 0
INDIRP4
INDIRP4
ADDP4
ASGNP4
ADDRLP4 184
ADDRLP4 176
INDIRP4
INDIRU1
CVUI4 1
CVIF4 4
CNSTF4 1065353216
ADDRLP4 4
INDIRF4
CNSTF4 1132396544
DIVF4
SUBF4
MULF4
ADDRLP4 12+8
INDIRF4
ADDRLP4 4
INDIRF4
MULF4
ADDF4
ASGNF4
ADDRLP4 188
CNSTF4 1325400064
ASGNF4
ADDRLP4 184
INDIRF4
ADDRLP4 188
INDIRF4
LTF4 $154
ADDRLP4 172
ADDRLP4 184
INDIRF4
ADDRLP4 188
INDIRF4
SUBF4
CVFI4 4
CVIU4 4
CNSTU4 2147483648
ADDU4
ASGNU4
ADDRGP4 $155
JUMPV
LABELV $154
ADDRLP4 172
ADDRLP4 184
INDIRF4
CVFI4 4
CVIU4 4
ASGNU4
LABELV $155
ADDRLP4 176
INDIRP4
ADDRLP4 172
INDIRU4
CVUU1 4
ASGNU1
ADDRLP4 196
ADDRLP4 0
INDIRI4
CNSTI4 1
ADDI4
ADDRFP4 0
INDIRP4
INDIRP4
ADDP4
ASGNP4
ADDRLP4 204
ADDRLP4 196
INDIRP4
INDIRU1
CVUI4 1
CVIF4 4
CNSTF4 1065353216
ADDRLP4 4
INDIRF4
CNSTF4 1132396544
DIVF4
SUBF4
MULF4
ADDRLP4 12+4
INDIRF4
ADDRLP4 4
INDIRF4
MULF4
ADDF4
ASGNF4
ADDRLP4 208
CNSTF4 1325400064
ASGNF4
ADDRLP4 204
INDIRF4
ADDRLP4 208
INDIRF4
LTF4 $158
ADDRLP4 192
ADDRLP4 204
INDIRF4
ADDRLP4 208
INDIRF4
SUBF4
CVFI4 4
CVIU4 4
CNSTU4 2147483648
ADDU4
ASGNU4
ADDRGP4 $159
JUMPV
LABELV $158
ADDRLP4 192
ADDRLP4 204
INDIRF4
CVFI4 4
CVIU4 4
ASGNU4
LABELV $159
ADDRLP4 196
INDIRP4
ADDRLP4 192
INDIRU4
CVUU1 4
ASGNU1
ADDRLP4 216
ADDRLP4 0
INDIRI4
CNSTI4 2
ADDI4
ADDRFP4 0
INDIRP4
INDIRP4
ADDP4
ASGNP4
ADDRLP4 224
ADDRLP4 216
INDIRP4
INDIRU1
CVUI4 1
CVIF4 4
CNSTF4 1065353216
ADDRLP4 4
INDIRF4
CNSTF4 1132396544
DIVF4
SUBF4
MULF4
ADDRLP4 12
INDIRF4
ADDRLP4 4
INDIRF4
MULF4
ADDF4
ASGNF4
ADDRLP4 228
CNSTF4 1325400064
ASGNF4
ADDRLP4 224
INDIRF4
ADDRLP4 228
INDIRF4
LTF4 $161
ADDRLP4 212
ADDRLP4 224
INDIRF4
ADDRLP4 228
INDIRF4
SUBF4
CVFI4 4
CVIU4 4
CNSTU4 2147483648
ADDU4
ASGNU4
ADDRGP4 $162
JUMPV
LABELV $161
ADDRLP4 212
ADDRLP4 224
INDIRF4
CVFI4 4
CVIU4 4
ASGNU4
LABELV $162
ADDRLP4 216
INDIRP4
ADDRLP4 212
INDIRU4
CVUU1 4
ASGNU1
ADDRLP4 0
ADDRLP4 0
INDIRI4
ADDRLP4 48
INDIRI4
ADDI4
ASGNI4
LABELV $149
ADDRLP4 8
ADDRLP4 8
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
LABELV $151
ADDRLP4 8
INDIRI4
CVIF4 4
ADDRLP4 28
INDIRF4
LTF4 $148
LABELV $143
ADDRLP4 72
ADDRLP4 72
INDIRI4
CNSTI4 1
ADDI4
ASGNI4
LABELV $145
ADDRLP4 72
INDIRI4
CVIF4 4
ADDRLP4 76
INDIRF4
LTF4 $142
ADDRGP4 $140
JUMPV
LABELV $139
ADDRLP4 28
CNSTF4 0
ASGNF4
LABELV $140
LABELV $138
ADDRLP4 84
ADDRLP4 84
INDIRF4
ADDRLP4 28
INDIRF4
CNSTF4 1088421888
SUBF4
ADDF4
ASGNF4
ADDRLP4 148
CNSTI4 1
ASGNI4
ADDRLP4 100
ADDRLP4 100
INDIRI4
ADDRLP4 148
INDIRI4
ADDI4
ASGNI4
ADDRLP4 68
ADDRLP4 68
INDIRP4
ADDRLP4 148
INDIRI4
ADDP4
ASGNP4
LABELV $130
ADDRLP4 68
INDIRP4
INDIRI1
CVII4 1
CNSTI4 0
EQI4 $163
ADDRLP4 100
INDIRI4
ADDRFP4 8
INDIRI4
LTI4 $129
LABELV $163
CNSTI4 1
RETI4
LABELV $115
endproc WriteNameOnTexture 232 12
lit
align 1
LABELV $165
byte 1 0
byte 1 0
byte 1 2
byte 1 0
byte 1 0
byte 1 0
byte 1 0
byte 1 0
byte 1 0
byte 1 0
byte 1 0
byte 1 0
export SaveTGA
code
proc SaveTGA 48 12
ADDRFP4 0
ADDRFP4 0
INDIRP4
ASGNP4
ADDRLP4 12
ADDRGP4 $165
INDIRB
ASGNB 12
ADDRFP4 4
INDIRP4
ARGP4
ADDRLP4 8
ARGP4
CNSTI4 1
ARGI4
ADDRGP4 trap_FS_FOpenFile
CALLI4
pop
ADDRLP4 8
INDIRI4
CNSTI4 0
NEI4 $166
ADDRGP4 $168
ARGP4
ADDRFP4 4
INDIRP4
ARGP4
ADDRGP4 Com_Printf
CALLV
pop
CNSTI4 0
RETI4
ADDRGP4 $164
JUMPV
LABELV $166
ADDRLP4 36
CNSTI4 8
ASGNI4
ADDRLP4 24
ADDRFP4 0
INDIRP4
CNSTI4 4
ADDP4
INDIRI4
ADDRLP4 36
INDIRI4
DIVI4
ASGNI4
ADDRLP4 40
ADDRFP4 0
INDIRP4
ADDRLP4 36
INDIRI4
ADDP4
ASGNP4
ADDRLP4 28
ADDRLP4 40
INDIRP4
INDIRI4
ADDRFP4 0
INDIRP4
CNSTI4 12
ADDP4
INDIRI4
MULI4
ADDRLP4 24
INDIRI4
MULI4
ASGNI4
ADDRLP4 44
CNSTI4 256
ASGNI4
ADDRLP4 0
ADDRLP4 40
INDIRP4
INDIRI4
ADDRLP4 44
INDIRI4
MODI4
CVIU4 4
CVUU1 4
ASGNU1
ADDRLP4 0+1
ADDRLP4 40
INDIRP4
INDIRI4
ADDRLP4 44
INDIRI4
DIVI4
CVIU4 4
CVUU1 4
ASGNU1
ADDRLP4 0+2
ADDRFP4 0
INDIRP4
CNSTI4 12
ADDP4
INDIRI4
CNSTI4 256
MODI4
CVIU4 4
CVUU1 4
ASGNU1
ADDRLP4 0+3
ADDRFP4 0
INDIRP4
CNSTI4 12
ADDP4
INDIRI4
CNSTI4 256
DIVI4
CVIU4 4
CVUU1 4
ASGNU1
ADDRLP4 0+4
ADDRFP4 0
INDIRP4
CNSTI4 4
ADDP4
INDIRI4
CVIU4 4
CVUU1 4
ASGNU1
ADDRLP4 0+5
CNSTU1 0
ASGNU1
ADDRLP4 12
ARGP4
CNSTI4 12
ARGI4
ADDRLP4 8
INDIRI4
ARGI4
ADDRGP4 trap_FS_Write
CALLV
pop
ADDRLP4 0
ARGP4
CNSTI4 6
ARGI4
ADDRLP4 8
INDIRI4
ARGI4
ADDRGP4 trap_FS_Write
CALLV
pop
ADDRFP4 0
INDIRP4
INDIRP4
ARGP4
ADDRLP4 28
INDIRI4
ARGI4
ADDRLP4 8
INDIRI4
ARGI4
ADDRGP4 trap_FS_Write
CALLV
pop
ADDRLP4 8
INDIRI4
ARGI4
ADDRGP4 trap_FS_FCloseFile
CALLV
pop
CNSTI4 1
RETI4
LABELV $164
endproc SaveTGA 48 12
export CreateLicensePlateImage
proc CreateLicensePlateImage 24 12
ADDRGP4 allocPoint
CNSTI4 0
ASGNI4
ADDRLP4 0
ARGP4
ADDRFP4 0
INDIRP4
ARGP4
ADDRLP4 16
ADDRGP4 LoadTGA
CALLI4
ASGNI4
ADDRLP4 16
INDIRI4
CNSTI4 0
NEI4 $175
ADDRGP4 $174
JUMPV
LABELV $175
ADDRLP4 0
ARGP4
ADDRFP4 8
INDIRP4
ARGP4
ADDRFP4 12
INDIRI4
ARGI4
ADDRLP4 20
ADDRGP4 WriteNameOnTexture
CALLI4
ASGNI4
ADDRLP4 20
INDIRI4
CNSTI4 0
NEI4 $177
ADDRGP4 $174
JUMPV
LABELV $177
ADDRLP4 0
ARGP4
ADDRFP4 4
INDIRP4
ARGP4
ADDRGP4 SaveTGA
CALLI4
pop
LABELV $174
endproc CreateLicensePlateImage 24 12
bss
align 1
LABELV memoryPool
skip 524288
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
LABELV $168
byte 1 94
byte 1 51
byte 1 81
byte 1 51
byte 1 82
byte 1 32
byte 1 87
byte 1 97
byte 1 114
byte 1 110
byte 1 105
byte 1 110
byte 1 103
byte 1 58
byte 1 32
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
byte 1 111
byte 1 112
byte 1 101
byte 1 110
byte 1 32
byte 1 37
byte 1 115
byte 1 32
byte 1 102
byte 1 111
byte 1 114
byte 1 32
byte 1 116
byte 1 101
byte 1 120
byte 1 116
byte 1 117
byte 1 114
byte 1 101
byte 1 32
byte 1 111
byte 1 117
byte 1 116
byte 1 112
byte 1 117
byte 1 116
byte 1 46
byte 1 10
byte 1 0
align 1
LABELV $118
byte 1 103
byte 1 102
byte 1 120
byte 1 47
byte 1 50
byte 1 100
byte 1 47
byte 1 98
byte 1 105
byte 1 103
byte 1 99
byte 1 104
byte 1 97
byte 1 114
byte 1 115
byte 1 46
byte 1 116
byte 1 103
byte 1 97
byte 1 0
align 1
LABELV $114
byte 1 94
byte 1 51
byte 1 81
byte 1 51
byte 1 82
byte 1 32
byte 1 87
byte 1 97
byte 1 114
byte 1 110
byte 1 105
byte 1 110
byte 1 103
byte 1 58
byte 1 32
byte 1 78
byte 1 111
byte 1 116
byte 1 32
byte 1 101
byte 1 110
byte 1 111
byte 1 117
byte 1 103
byte 1 104
byte 1 32
byte 1 109
byte 1 101
byte 1 109
byte 1 111
byte 1 114
byte 1 121
byte 1 32
byte 1 116
byte 1 111
byte 1 32
byte 1 108
byte 1 111
byte 1 97
byte 1 100
byte 1 32
byte 1 37
byte 1 115
byte 1 46
byte 1 10
byte 1 0
align 1
LABELV $110
byte 1 94
byte 1 51
byte 1 81
byte 1 51
byte 1 82
byte 1 32
byte 1 87
byte 1 97
byte 1 114
byte 1 110
byte 1 105
byte 1 110
byte 1 103
byte 1 58
byte 1 32
byte 1 37
byte 1 115
byte 1 32
byte 1 104
byte 1 97
byte 1 115
byte 1 32
byte 1 105
byte 1 110
byte 1 118
byte 1 97
byte 1 108
byte 1 105
byte 1 100
byte 1 32
byte 1 100
byte 1 105
byte 1 109
byte 1 101
byte 1 110
byte 1 115
byte 1 105
byte 1 111
byte 1 110
byte 1 115
byte 1 32
byte 1 111
byte 1 114
byte 1 32
byte 1 98
byte 1 112
byte 1 112
byte 1 115
byte 1 46
byte 1 10
byte 1 0
align 1
LABELV $100
byte 1 94
byte 1 51
byte 1 81
byte 1 51
byte 1 82
byte 1 32
byte 1 87
byte 1 97
byte 1 114
byte 1 110
byte 1 105
byte 1 110
byte 1 103
byte 1 58
byte 1 32
byte 1 72
byte 1 101
byte 1 97
byte 1 100
byte 1 101
byte 1 114
byte 1 32
byte 1 111
byte 1 102
byte 1 32
byte 1 37
byte 1 115
byte 1 32
byte 1 100
byte 1 111
byte 1 101
byte 1 115
byte 1 32
byte 1 110
byte 1 111
byte 1 116
byte 1 32
byte 1 109
byte 1 97
byte 1 116
byte 1 99
byte 1 104
byte 1 32
byte 1 107
byte 1 110
byte 1 111
byte 1 119
byte 1 110
byte 1 32
byte 1 104
byte 1 101
byte 1 97
byte 1 100
byte 1 101
byte 1 114
byte 1 32
byte 1 102
byte 1 111
byte 1 114
byte 1 109
byte 1 97
byte 1 116
byte 1 46
byte 1 10
byte 1 0
align 1
LABELV $99
byte 1 94
byte 1 51
byte 1 81
byte 1 51
byte 1 82
byte 1 32
byte 1 87
byte 1 97
byte 1 114
byte 1 110
byte 1 105
byte 1 110
byte 1 103
byte 1 58
byte 1 32
byte 1 67
byte 1 97
byte 1 110
byte 1 110
byte 1 111
byte 1 116
byte 1 32
byte 1 108
byte 1 111
byte 1 97
byte 1 100
byte 1 32
byte 1 37
byte 1 115
byte 1 44
byte 1 32
byte 1 82
byte 1 117
byte 1 110
byte 1 45
byte 1 76
byte 1 101
byte 1 110
byte 1 103
byte 1 116
byte 1 104
byte 1 32
byte 1 69
byte 1 110
byte 1 99
byte 1 111
byte 1 100
byte 1 101
byte 1 100
byte 1 32
byte 1 84
byte 1 71
byte 1 65
byte 1 115
byte 1 32
byte 1 97
byte 1 114
byte 1 101
byte 1 32
byte 1 117
byte 1 110
byte 1 115
byte 1 117
byte 1 112
byte 1 112
byte 1 111
byte 1 114
byte 1 116
byte 1 101
byte 1 100
byte 1 46
byte 1 10
byte 1 0
align 1
LABELV $93
byte 1 94
byte 1 51
byte 1 81
byte 1 51
byte 1 82
byte 1 32
byte 1 87
byte 1 97
byte 1 114
byte 1 110
byte 1 105
byte 1 110
byte 1 103
byte 1 58
byte 1 32
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
byte 1 111
byte 1 112
byte 1 101
byte 1 110
byte 1 32
byte 1 37
byte 1 115
byte 1 32
byte 1 102
byte 1 111
byte 1 114
byte 1 32
byte 1 108
byte 1 105
byte 1 99
byte 1 101
byte 1 110
byte 1 115
byte 1 101
byte 1 32
byte 1 112
byte 1 108
byte 1 97
byte 1 116
byte 1 101
byte 1 46
byte 1 10
byte 1 0
align 1
LABELV $87
byte 1 67
byte 1 71
byte 1 95
byte 1 65
byte 1 108
byte 1 108
byte 1 111
byte 1 99
byte 1 58
byte 1 32
byte 1 102
byte 1 97
byte 1 105
byte 1 108
byte 1 101
byte 1 100
byte 1 32
byte 1 111
byte 1 110
byte 1 32
byte 1 97
byte 1 108
byte 1 108
byte 1 111
byte 1 99
byte 1 97
byte 1 116
byte 1 105
byte 1 111
byte 1 110
byte 1 32
byte 1 111
byte 1 102
byte 1 32
byte 1 37
byte 1 105
byte 1 32
byte 1 98
byte 1 121
byte 1 116
byte 1 101
byte 1 115
byte 1 10
byte 1 0
