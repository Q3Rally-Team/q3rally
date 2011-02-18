lowShotgunFlash
{
	deformVertexes autosprite
	{
		map models/weapons2/shotgun/shotgun_flash.tga
		blendfunc add
	}
}

lowMachgunFlash
{
	deformVertexes autosprite
	{
		map models/weapons2/machinegun/f_machinegun.tga
		blendfunc add
	}
}

lowPlasmaaFlash
{
	deformVertexes autosprite
	{
		map models/weapons2/plasma/f_plasma.tga
		blendfunc add
	}
}

lowRocketlFlash
{
	deformVertexes autosprite
	{
		map models/weapons2/rocketl/f_rocketl.tga
		blendfunc add
	}
}

lowGrenadeFlash
{
	deformVertexes autosprite
	{
		map models/weapons2/grenadel/f_grenadel.tga
		blendfunc add
	}
}

lowRailgunFlash
{
	deformVertexes autosprite
	{
		map models/weapons2/railgun/f_railgun2.tga
		blendfunc add
		rgbGen entity
	}
}

lapscrn
{
	tessSize 64
	{
		map models/mapobjects/laptop/lapscrn.tga
	}
	{
		map models/mapobjects/laptop/invert.tga
		blendfunc gl_one_minus_dst_color gl_one_minus_src_color
		tcGen environment 
	}
}

lowLightnnFlash
{
	deformVertexes autosprite
	{
		map models/weapons2/lightning/f_lightning.tga
		blendfunc add
		tcMod rotate 675756
	}
}

LightnnFlash
{
	cull disable
	{
		animmap 20 models/weapons2/lightning/muzzle1.tga models/weapons2/lightning/muzzle2.tga models/weapons2/lightning/muzzle3.tga 
		blendfunc gl_one gl_src_alpha
		rgbGen wave inversesawtooth 0 1 0 20 
	}
	{
		animmap 20 models/weapons2/lightning/muzzle2.tga models/weapons2/lightning/muzzle3.tga models/weapons2/lightning/muzzle1.tga 
		blendfunc gl_one gl_src_alpha
		rgbGen wave sawtooth 0 1 0 20 
	}
}

default
{
	{
		map textures/stone/pjrock1.tga
	}
}

textures/effects/jumpcirc
{
	q3map_lightimage textures/effects/jumpcirc.tga
	q3map_surfacelight 466
	{
		clampmap textures/effects/jumpcirc.tga
		tcMod stretch sawtooth 0 1 0 1 
		tcMod rotate 75
	}
	{
		clampmap textures/effects/jumpcirc.tga
		blendfunc add
		tcMod stretch sawtooth 0 1 0 1 
		tcMod rotate -120
	}
}

textures/effects/jumpcircblue
{
	q3map_lightimage textures/effects/jumpcircblue.tga
	q3map_surfacelight 466
	{
		clampmap textures/effects/jumpcircblue.tga
		tcMod stretch sawtooth 0 1 0 1 
		tcMod rotate 75
	}
	{
		clampmap textures/effects/jumpcircblue.tga
		blendfunc add
		tcMod stretch sawtooth 0 1 0 1 
		tcMod rotate -120
	}
}

textures/effects/jumpcircred
{
	q3map_lightimage textures/effects/jumpcircred.tga
	q3map_surfacelight 466
	{
		clampmap textures/effects/jumpcircred.tga
		tcMod stretch sawtooth 0 1 0 1 
		tcMod rotate 75
	}
	{
		clampmap textures/effects/jumpcircred.tga
		blendfunc add
		tcMod stretch sawtooth 0 1 0 1 
		tcMod rotate -120
	}
}

textures/ctf_unified/floor_decal_blue
{
	cull disable
	{
		map textures/symbols/blueteam.tga
		blendfunc add
	}
}

textures/ctf_unified/floor_decal_red
{
	cull disable
	{
		map textures/symbols/redteam.tga
		blendfunc add
	}
}

textures/ctf_unified/monologo_flash_blue
{
	cull disable
	{
		map textures/symbols/blueteam.tga
	}
}

textures/ctf_unified/monologo_flash_red
{
	cull disable
	{
		map textures/symbols/redteam.tga
	}
}

textures/base_wall/glass01
{
	surfaceparm trans
	cull disable
	{
		map textures/effects/tinfx.tga
		blendfunc add
		rgbGen identity
		tcGen environment 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 4 4
		detail
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
	}
}

redArmor
{
	{
		map models/powerups/armor/redarmor.tga
		rgbGen lightingDiffuse
	}
	{
		map textures/sfx/specular.tga
		blendfunc add
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/powerups/armor/redarmor.tga
		blendfunc blend
		rgbGen lightingDiffuse
	}
}

yellowArmor
{
	{
		map models/powerups/armor/yellowarmor.tga
		rgbGen lightingDiffuse
	}
	{
		map textures/sfx/specular.tga
		blendfunc add
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/powerups/armor/yellowarmor.tga
		blendfunc blend
		rgbGen lightingDiffuse
	}
}

textures/base_floor/metfloor1
{
	surfaceparm metalsteps
	{
		map textures/base_floor/metfloor1.tga
		rgbGen const ( 1 1 1 )
	}
	{
		clampmap textures/base_wall/chrome_env2.tga
		blendfunc add
		rgbGen const ( 0.188235 0.188235 0.188235 )
		tcGen environment 
	}
	{
		map textures/base_floor/metfloor1.tga
		blendfunc filter
		rgbGen const ( 0.737255 0.737255 0.737255 )
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
}

textures/base_trim/tinfx
{
	{
		map textures/base_trim/tinfx.tga
		tcGen environment 
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
}

console
{
	{
		map textures/sfx/logo256.tga
	}
}

grassobj
{
	cull disable
	{
		map models/mapobjects/out/grass.tga
		rgbGen lightingDiffuse
		alphaFunc GE128
	}
}

ameatygib
{
	{
		map models/gibs/genericgibmeat.tga
		rgbGen lightingDiffuse
	}
	{
		map models/gibs/gibmeatspec.tga
		blendfunc gl_src_alpha gl_dst_alpha
		rgbGen lightingDiffuse
		alphaGen lightingSpecular
	}
}

oalogo
{
	{
		map textures/oa/water.tga
		tcMod scroll 0.01 0.01
	}
	{
		map textures/oa/water.tga
		blendfunc add
		tcMod scroll 0.04 0.01
	}
	{
		map textures/oa/water.tga
		blendfunc add
		tcMod scroll -0.01 -0.02
	}
	{
		map textures/oa/flamelet.tga
		tcMod scroll -0.2 0.4
	}
	{
		map textures/oa/flamelet.tga
		blendfunc add
		tcMod scroll 0.2 0.7
	}
	{
		map textures/oa/flamelet.tga
		blendfunc add
		tcMod scroll 0 0.6
	}
}

//REALLY COOL MENU BACKGROUNDY
menubacknologo_blueish
{
	{
		map gfx/newbg/bluefiller.tga
		rgbGen identity
		tcMod turb 0 0.6 0 0.125
		tcMod rotate 6
		tcMod scroll 1 0
	}
	{
		map gfx/newbg/bluefiller.tga
		blendfunc add
		rgbGen identity
		tcMod turb 0 0.4 0 0.325
		tcMod rotate -12
		tcMod scroll -1 0
	}
	{
		clampmap gfx/newbg/blackfading.tga
		blendfunc gl_zero gl_one_minus_src_alpha
		rgbGen identity
		tcMod stretch sin 1.5 0.2 0 0.1 
		tcMod rotate 7
	}
	{
		clampmap gfx/newbg/blackfading.tga
		blendfunc blend
		tcMod stretch sin 1.3 0.2 0 0.12 
		tcMod rotate -17
		alphaGen wave sin 0.3 0 0 1 
	}
	{
		map textures/sfx/jcb2.tga
		blendfunc add
		rgbGen wave triangle 0 0.2 0 0.1 
		tcMod rotate 360
		tcMod stretch sin 2.3 0 0 1 
	}
}

//REALLY COOL MENU BACKGROUNDY
menuback_blueish
{
	{
		map gfx/newbg/bluefiller.tga
		rgbGen identity
		tcMod turb 0 0.6 0 0.125
		tcMod rotate 6
		tcMod scroll 1 0
	}
	{
		map gfx/newbg/bluefiller.tga
		blendfunc add
		rgbGen identity
		tcMod turb 0 0.4 0 0.325
		tcMod rotate -12
		tcMod scroll -1 0
	}
	{
		clampmap gfx/newbg/blackfading.tga
		blendfunc gl_zero gl_one_minus_src_alpha
		rgbGen identity
		tcMod stretch sin 1.5 0.2 0 0.1 
		tcMod rotate 7
	}
	{
		clampmap gfx/newbg/blackfading.tga
		blendfunc blend
		tcMod stretch sin 1.3 0.2 0 0.12 
		tcMod rotate -17
		alphaGen wave sin 0.3 0 0 1 
	}
	{
		map textures/sfx/jcb2.tga
		blendfunc add
		rgbGen wave triangle 0 0.2 0 0.1 
		tcMod rotate 360
		tcMod stretch sin 2.3 0 0 1 
	}
	{
		videomap video/idlogo.roq
		blendfunc gl_dst_color gl_one_minus_dst_color
		rgbGen const ( 0.482353 0.384314 0.615686 )
	}
}

OLDmenubacknologo
{
	{
		map textures/oa/water.tga
		tcMod scroll 0.01 0.01
	}
	{
		map textures/oa/water.tga
		blendfunc add
		tcMod scroll 0.04 0.01
	}
	{
		map textures/oa/water.tga
		blendfunc filter
		tcMod scroll -0.01 -0.02
	}
}

OLDmenuback
{
	{
		map textures/oa/bloodbg.tga
		tcMod scroll 0.01 0.01
		tcMod rotate 5
	}
	{
		map textures/oa/bloodbg.tga
		blendfunc add
		tcMod scroll 0.01 0.01
		tcMod rotate -6
	}
	{
		map textures/oa/bloodbg.tga
		blendfunc filter
		tcMod scroll -0.01 -0.02
		tcMod rotate 3
	}
}

bloodybg
{
	{
		map textures/oa/bloodbg.tga
		tcMod scroll 0.01 0.01
		tcMod rotate 5
	}
	{
		map textures/oa/bloodbg.tga
		blendfunc add
		tcMod scroll 0.01 0.01
		tcMod rotate -6
	}
	{
		map textures/oa/bloodbg.tga
		blendfunc filter
		tcMod scroll -0.01 -0.02
		tcMod rotate 3
	}
}

boomzor
{
	deformVertexes autosprite
	{
		animmap 8 models/weaphits/rlboom/rlboom_1.tga models/weaphits/rlboom/rlboom_2.tga models/weaphits/rlboom/rlboom_3.tga models/weaphits/rlboom/rlboom_4.tga models/weaphits/rlboom/rlboom_5.tga models/weaphits/rlboom/rlboom_6.tga models/weaphits/rlboom/rlboom_7.tga models/weaphits/rlboom/rlboom_8.tga 
		blendfunc gl_one gl_src_alpha
		rgbGen wave inversesawtooth 0 1 0 8 
	}
	{
		animmap 8 models/weaphits/rlboom/rlboom_2.tga models/weaphits/rlboom/rlboom_3.tga models/weaphits/rlboom/rlboom_4.tga models/weaphits/rlboom/rlboom_5.tga models/weaphits/rlboom/rlboom_6.tga models/weaphits/rlboom/rlboom_7.tga models/weaphits/rlboom/rlboom_8.tga models/weaphits/rlboom/rlboom_8.tga 
		blendfunc gl_one gl_src_alpha
		rgbGen wave sawtooth 0 1 0 8 
	}
	{
		clampmap textures/oa/fiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate 77
		tcMod stretch sin 0.3 0.4 0 0.4 
	}
	{
		clampmap textures/oa/fiar2.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate 33
		tcMod stretch sin 0.8 -0.4 0 0.1 
	}
	{
		clampmap textures/oa/fiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate -17
		tcMod stretch sawtooth 0 1.2 0 0.5 
	}
	{
		clampmap textures/oa/fiar2.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate -77
	}
}

smokePuff
{
	cull disable
	{
		clampmap gfx/misc/smokepuff3.tga
		blendfunc blend
		tcMod rotate -55
		alphaGen Vertex
	}
}

smokePuffRagePro
{
	cull disable
	{
		map gfx/misc/smokepuffragepro.tga
		blendfunc blend
		alphaGen Vertex
	}
}

hasteSmokePuff
{
	cull disable
	{
		map gfx/misc/hastesmoke.tga
		blendfunc blend
		tcMod rotate 64
		alphaGen Vertex
	}
}

shotgunSmokePuff
{
	cull disable
	{
		clampmap gfx/misc/smokepuff3.tga
		blendfunc blend
		tcMod rotate -45
		alphaGen entity
	}
}

sprites/plasma1
{
	{
		clampmap sprites/plasmaa.tga
		blendfunc gl_src_alpha gl_one
		tcMod rotate -145
	}
	{
		clampmap sprites/plasmaa.tga
		blendfunc gl_src_alpha gl_one
		tcMod rotate 177
	}
}

bfgshot
{
	cull disable
	deformVertexes autosprite
	{
		clampmap textures/oafx/bfgfx.tga
		blendfunc add
		tcMod rotate -700
	}
	{
		clampmap textures/oafx/bfgfx2.tga
		blendfunc add
		tcMod rotate 54
	}
	{
		clampmap textures/oafx/bfgfx2.tga
		blendfunc add
		tcMod rotate -14
	}
}

bfgsho
{
	cull disable
	deformVertexes autosprite
	{
		clampmap textures/oafx/bfgfx3.tga
		blendfunc add
		tcMod rotate 64
	}
}

// STUPID explosion thing introduced in q3's 1.30 patch
explode11
{
	{
		map textures/sfx/130boom/dpexplosion4_0000.tga
		blendfunc add
	}
}

explode12
{
	{
		map textures/sfx/130boom/dpexplosion4_0001.tga
		blendfunc add
	}
}

explode13
{
	{
		map textures/sfx/130boom/dpexplosion4_0002.tga
		blendfunc add
	}
}

explode14
{
	{
		map textures/sfx/130boom/dpexplosion4_0003.tga
		blendfunc add
	}
}

explode15
{
	{
		map textures/sfx/130boom/dpexplosion4_0004.tga
		blendfunc add
	}
}

explode16
{
	{
		map textures/sfx/130boom/dpexplosion4_0005.tga
		blendfunc add
	}
}

explode17
{
	{
		map textures/sfx/130boom/dpexplosion4_0006.tga
		blendfunc add
	}
}

explode18
{
	{
		map textures/sfx/130boom/dpexplosion4_0007.tga
		blendfunc add
	}
}

explode19
{
	{
		map textures/sfx/130boom/dpexplosion4_0008.tga
		blendfunc add
	}
}

explode110
{
	{
		map textures/sfx/130boom/dpexplosion4_0009.tga
		blendfunc add
	}
}

explode111
{
	{
		map textures/sfx/130boom/dpexplosion4_0010.tga
		blendfunc add
	}
}

explode112
{
	{
		map textures/sfx/130boom/dpexplosion4_0011.tga
		blendfunc add
	}
}

explode113
{
	{
		map textures/sfx/130boom/dpexplosion4_0012.tga
		blendfunc add
	}
}

explode114
{
	{
		map textures/sfx/130boom/dpexplosion4_0013.tga
		blendfunc add
	}
}

explode115
{
	{
		map textures/sfx/130boom/dpexplosion4_0014.tga
		blendfunc add
	}
}

explode116
{
	{
		map textures/sfx/130boom/dpexplosion4_0015.tga
		blendfunc add
	}
}

explode117
{
	{
		map textures/sfx/130boom/dpexplosion4_0016.tga
		blendfunc add
	}
}

explode118
{
	{
		map textures/sfx/130boom/dpexplosion4_0017.tga
		blendfunc add
	}
}

explode119
{
	{
		map textures/sfx/130boom/dpexplosion4_0018.tga
		blendfunc add
	}
}

explode120
{
	{
		map textures/sfx/130boom/dpexplosion4_0019.tga
		blendfunc add
	}
}

explode121
{
	{
		map textures/sfx/130boom/dpexplosion4_0020.tga
		blendfunc add
	}
}

explode122
{
	{
		map textures/sfx/130boom/dpexplosion4_0021.tga
		blendfunc add
	}
}

explode123
{
	{
		map textures/sfx/130boom/dpexplosion4_0022.tga
		blendfunc add
	}
}

explode124
{
	{
		map textures/sfx/130boom/dpexplosion4_0023.tga
		blendfunc add
	}
}

lightningBoltNew
{
	cull disable
	{
		animmap 30 textures/oafx/lbeam3.tga textures/oafx/lbeam4.tga textures/oafx/lbeam5.tga textures/oafx/lbeam6.tga textures/oafx/lbeam7.tga textures/oafx/lbeam8.tga textures/oafx/lbeam5.tga textures/oafx/lbeam7.tga 
		blendfunc add
		tcMod scale 0.5 1
		tcMod scroll -1 0
	}
	{
		animmap 40 textures/oafx/lbeam8.tga textures/oafx/lbeam7.tga textures/oafx/lbeam4.tga textures/oafx/lbeam5.tga textures/oafx/lbeam6.tga textures/oafx/lbeam3.tga textures/oafx/lbeam7.tga textures/oafx/lbeam4.tga 
		blendfunc add
		tcMod scale 0.2 1
		tcMod scroll -0.3 0
	}
}

