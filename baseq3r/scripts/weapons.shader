// ------------------------------------------------------------
// Shaders for Q3Rally Weapons - rewritten by P3rlE
// based on all weapons_xxx.shader files
// ------------------------------------------------------------

// ------------------------------------------------------------
// 1.BigFuckingGun
// ------------------------------------------------------------

f_bfg3
{
	deformVertexes autosprite
	{
		clampmap models/weapons2/bfg/f_bfg3.tga
		blendfunc add
		rgbGen const ( 0.376471 0.576471 0.403922 )
		tcMod rotate 8288
	}
	{
		clampmap models/weapons2/bfg/f_bfg3.tga
		blendfunc add
		rgbGen const ( 0.478431 0.603922 0.447059 )
		tcMod rotate -2288
		tcMod stretch sin 1.3 0 0 0 
	}
}

f_bfg1
{
	deformVertexes autosprite
	{
		clampmap models/weapons2/bfg/f_bfg3.tga
		blendfunc add
		rgbGen const ( 0.376471 0.576471 0.403922 )
		tcMod rotate 8288
	}
	{
		clampmap models/weapons2/bfg/f_bfg3.tga
		blendfunc add
		rgbGen const ( 0.478431 0.603922 0.447059 )
		tcMod rotate -2288
		tcMod stretch sin 1.3 0 0 0 
	}
}

models/weapons2/bfg/bfgbody
{
	cull disable
	{
		map models\weapons2\bfg\bfgbody.tga
	}
	{
		map textures\effects\skinspeciso.tga
		blendfunc add
		rgbGen lightingDiffuse
		tcGen environment 
	}
}

models/weapons2/bfg/bfgtube
{
	cull disable
	{
		map models\weapons2\bfg\bfgtube.tga
		blendfunc blend
		rgbGen lightingDiffuse
		tcMod scroll -0.04 0
		alphaGen wave inversesawtooth 0.5 0 0 1 
	}
	{
		map models\weapons2\bfg\bubbles.tga
		blendfunc gl_src_alpha gl_one
		rgbGen lightingDiffuse
		tcMod scale 1.8 1
		tcMod scroll -2 0
	}
	{
		map textures\effects\tinfx3.tga
		blendfunc add
		rgbGen lightingDiffuse
		tcGen environment 
	}
}

f_bfg2
{
	cull disable
	{
		clampmap models/weapons2/bfg/f_bfg2.tga
		blendfunc add
		rgbGen const ( 0.694118 0.694118 0.694118 )
		tcMod rotate 2122
	}
	{
		clampmap models/weapons2/bfg/f_bfg2.tga
		blendfunc add
		rgbGen const ( 0.286275 0.54902 0.25098 )
		tcMod rotate -3201
		tcMod stretch sin 1.3 0 0 0 
	}
}


BFGNexus
{
	deformVertexes autoSprite
	{
		clampmap models/weaphits/bfg.tga
		blendfunc add
		tcMod rotate 999
	}
	{
		clampmap models/weaphits/bfg3.tga
		blendfunc add
		rgbGen const ( 0.188235 0.247059 0.180392 )
		tcMod rotate 700
		tcMod stretch sin 1.3 0.3 0 4 
	}
	{
		clampmap models/weaphits/bfg2.tga
		blendfunc add
		rgbGen const ( 0.254902 0.352941 0.227451 )
		tcMod rotate 244
		tcMod stretch sin 1.3 0.3 0 4 
	}
}

BFGTrail
{
	cull disable
	{
		clampmap models/weaphits/bfg2.tga
		blendfunc add
		rgbGen const ( 0.694118 0.694118 0.694118 )
		tcMod rotate 1522
	}
	{
		clampmap models/weaphits/bfg2.tga
		blendfunc add
		rgbGen const ( 0.286275 0.54902 0.25098 )
		tcMod rotate -1222
		tcMod stretch sin 1.3 0.3 0 4 
	}
}

// ------------------------------------------------------------
// 2.Lightning Gun
// ------------------------------------------------------------


lightningBoltnew
{
	cull disable
	{
		animmap 30 textures/oafx/lbeam3.tga textures/oafx/lbeam4.tga textures/oafx/lbeam5.tga textures/oafx/lbeam6.tga textures/oafx/lbeam7.tga textures/oafx/lbeam8.tga textures/oafx/lbeam5.tga textures/oafx/lbeam7.tga 
		blendfunc add
		tcMod scroll -1 0
	}
}

lightnnFlash
{
	cull disable
	{
		clampmap models/weapons2/lightning/muzzle1.tga
		blendfunc add
		rgbGen entity
		tcMod rotate -74
	}
	{
		clampmap models/weapons2/lightning/muzzle1.tga
		blendfunc add
		rgbGen entity
		tcMod rotate 53
	}

}

models/weapons2/lightning/f_lightning
{
	sort additive
	cull disable
	{
		map models/weapons2/lightning/f_lightning.tga
		blendfunc GL_ONE GL_ONE
	}
}

models/weaphits/electric
{
	cull none
	
	{
		clampmap models/weaphits/electric.jpg
		blendFunc GL_ONE GL_ONE
                rgbgen wave triangle .8 2 0 9
                tcMod rotate 360
	}	
        {
		clampmap models/weaphits/electric.tga
		blendFunc GL_ONE GL_ONE
                rgbgen wave triangle 1 1.4 0 9.5
                tcMod rotate -202
	}	
	
}

// ------------------------------------------------------------
// 3.Flamethrower
// ------------------------------------------------------------

models/weapons2/flamethrower/f_flamethrower
{
	cull disable
	{
		clampmap models/weapons2/flamethrower/f_flamethrower.tga
		blendfunc add
		rgbGen identity
		tcMod rotate 5288
	}
}

// ------------------------------------------------------------
// 4.Gauntlet
// ------------------------------------------------------------

models/weapons2/gauntlet/skin
{
	{
		map textures/base_wall/bluemetal2_shiny.tga
		rgbGen lightingDiffuse
	}
}

models/weapons2/gauntlet/saw
{
	{
		map textures/skies/topclouds.tga
		rgbGen lightingDiffuse
		tcMod rotate 4
		tcGen environment 
	}
	{
		map textures/effects/tinfx2c.tga
		blendfunc add
		rgbGen lightingDiffuse
		tcMod rotate -8
		tcGen environment 
	}
}

models/weapons2/gauntlet/flare
{
	deformVertexes autosprite
	{
		map models/weapons2/gauntlet/glow.tga
		blendfunc add
		rgbGen wave sin 0.08 0.02 0 15 
	}
}

models/weapons2/gauntlet/glow
{
	{
		map models/weapons2/gauntlet/glow.tga
		blendfunc add
		rgbGen wave sin 1 0.1 0 15 
	}
}

// ------------------------------------------------------------
// 5.Grenadelauncher
// ------------------------------------------------------------

models/weapons2/grenadel/grenadel
{
	{
		map models/weapons2/grenadel/grenadel.tga
		rgbGen lightingDiffuse

	}
	{
		map models/weapons2/grenadel/chromey.tga
		rgbGen lightingDiffuse
		tcGen environment 
		blendfunc add
	}
	{
		map models/weapons2/grenadel/grenadel.tga
		rgbGen lightingDiffuse
		blendfunc blend
	}
}

models/weapons2/grenadel/newgren.tga
{
	{
		map models/weapons2/grenadel/newgren.tga
		rgbGen lightingDiffuse

	}
	{
		map models/weapons2/grenadel/chromey.tga
		rgbGen lightingDiffuse
		tcGen environment 
		blendfunc add
	}
	{
		map models/weapons2/grenadel/newgren.tga
		rgbGen lightingDiffuse
		blendfunc blend
	}
}
models/weapons2/grenadel/blackhandle
{
	{
		map models/weapons2/grenadel/blackhandle.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}

}

models/weapons2/grenadel/muzside
{
//	deformVertexes autosprite2
	cull disable
	{
		map models/weapons2/grenadel/muzzlside.tga
		blendfunc add
	}
}

models/weapons2/grenadel/muzcenter
{
	deformVertexes autosprite
	{
		clampmap models/weapons2/grenadel/f_plasma.tga
		blendfunc add
		tcMod rotate 1246
	}
	{
		clampmap models/weapons2/grenadel/f_plasma.tga
		blendfunc add
		tcMod rotate -1246
	}
}

models/weapons2/grenadel/flare
{
	deformVertexes autosprite
	{
		map models/weapons2/grenadel/flare.tga
		blendfunc add
		rgbGen wave noise 0.5 0.5 0 1 
	}
}
 
cmuz_grn_cone1
{
	cull disable
	{
		map textures/oa/muzzle/muz3.tga
		blendfunc add
		rgbGen const ( 0.517647 0.184314 0.184314 )
		tcMod rotate 1024
	}
	{
		map textures/oa/muzzle/muz2.tga
		blendfunc add
		rgbGen const ( 0.419608 0.0705882 0.0705882 )
		tcMod rotate -1024
	}
}

cmuz_grn_cone2
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.560784 0.121569 0.121569 )
		tcMod rotate 128
	}
}

cmuz_grn_cone3
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.0901961 0 0 )
		tcMod rotate 128
	}
}

cmuz_grn_fuzz1
{
	deformVertexes autosprite
	{
		animmap 27 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz8.tga 
		blendfunc add
		rgbGen const ( 0.505882 0.152941 0.152941 )
	}
}

cmuz_grn_fuzz2
{
	deformVertexes autosprite
	{
		animmap 36 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga 
		blendfunc add
		rgbGen const ( 0.478431 0.188235 0.188235 )
	}
}

cmuz_grn_fuzz3
{
	deformVertexes autosprite
	{
		animmap 21 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.388235 0.180392 0.180392 )
	}
}

cmuz_grn_fuzz4
{
	deformVertexes autosprite
	{
		animmap 52 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.388235 0.152941 0.152941 )
	}
}

// ------------------------------------------------------------
// 6.Machinegun
// ------------------------------------------------------------

models/weapons2/machinegun/f_machinegun2
{
	cull disable
	{
		clampmap models/weapons2/machinegun/f_machinegun2.tga
		blendfunc add
		rgbGen identity
		tcMod rotate 5288
	}
}

models/weapons2/machinegun/f_machinegun3
{
	deformVertexes autosprite
	{
		clampmap models/weapons2/machinegun/f_machinegun3.tga
		blendfunc add
		rgbGen identity
		tcMod rotate 8288
	}
}

models/weapons2/machinegun/muzzlside
{
	cull disable
	{
		map models/weapons2/machinegun/muzzlside.tga
		blendfunc add
	}
}

models/weapons2/shells/mgunshell
{
	cull disable
	{
		map models/weapons2/shells/mgunshell.tga
		rgbGen lightingDiffuse
		
	}
}


models/weapons2/shells/mgunshell_2
{
	deformVertexes autosprite
    {
        clampmap models/weapons2/shells/mgunshell_2
        rgbGen lightingDiffuse
	blendFunc blend
	tcMod scale 1.4 1.4
    }

}

shellCase
{
	cull disable
	{
		map models/weapons2/shells/sgunshell.tga
		rgbGen lightingDiffuse
	}
	{
		map models/weapons2/shells/mgunshell.tga
		blendfunc add
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/weapons2/shells/sgunshell.tga
		blendfunc blend
		rgbGen lightingDiffuse
	}
}

models/weapons2/machinegun/muzzlecenter
{
	deformVertexes autosprite
	{
		map models/weapons2/machinegun/f_machinegun.tga
		blendfunc add
		tcMod rotate 1246
	}
	{
		map models/weapons2/machinegun/f_machinegun.tga
		blendfunc add
		tcMod rotate -1246
	}
}

models/weapons2/machinegun/flare
{
	deformVertexes autosprite
	{
		map models/weapons2/machinegun/flare.tga
		blendfunc add
	}
}

models/weapons2/machinegun/skin
{
	{
		map models/weapons2/machinegun/mgun.tga
		rgbGen lightingDiffuse
	}
}

models/weapons2/machinegun/sight
{
	cull disable
	{
		map models/weapons2/machinegun/sight.tga
		rgbGen lightingDiffuse
		alphaFunc GE128
	}
}

machinegunFlash
{
	cull disable
	{
		map models/weapons2/machinegun/muzzley.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 7.Plasmagun
// ------------------------------------------------------------

models/weapons2/plasma/f_plasmagun2
{
	cull disable
	{
		clampmap textures/flares/lava.tga
		blendfunc add
		rgbGen const ( 0 0.0862745 0.235294 )
		tcMod rotate 8455
		tcMod stretch sin 0 1 0 2 
	}
	{
		clampmap textures/flares/flarey.tga
		blendfunc add
		rgbGen const ( 0.447059 0.623529 0.921569 )
		tcMod rotate 1466
		tcMod stretch sin 0 1 0 1 
	}
	{
		clampmap textures/flares/twilightflare.tga
		blendfunc add
		rgbGen const ( 0.447059 0.623529 0.921569 )
		tcMod rotate -6455
	}
}

models/weapons2/plasma/f_plasmagun3
{
	deformVertexes autosprite
	{
		clampmap textures/flares/twilightflare.tga
		blendfunc add
		tcMod rotate 1246
	}
	{
		clampmap textures/flares/twilightflare.tga
		blendfunc add
		tcMod rotate -1246
	}
}

models/weapons2/plasma/muzzlecenter
{
	deformVertexes autosprite
	{
		clampmap textures/flares/twilightflare.tga
		blendfunc add
		tcMod rotate 1246
	}
	{
		clampmap textures/flares/twilightflare.tga
		blendfunc add
		tcMod rotate -1246
	}
}

models/weapons2/plasma/blackchrome
{
	{
		map models/weapons2/plasma/blackchrome.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
}

models/weapons2/plasma/skin
{
	{
		map models/weapons2/plasma/skin.tga
		rgbGen identity
	}
	{
		map textures/effects/tinfx2d.tga
		blendfunc add
		rgbGen identity
		tcMod scroll 1 0
		tcMod scale 2 2
	}
	{
		map models/weapons2/plasma/skin.tga
		rgbGen lightingDiffuse
		alphaFunc GE128
	}
}

models/weapons2/plasma/blackhandle
{
	{
		map models/weapons2/plasma/blackhandle.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
}

models/weapons2/plasma/muzzlside
{
	cull disable
	{
		map models/weapons2/plasma/muzzlside.tga
		blendfunc add
	}
}

models/weapons2/plasma/muzzlecenter
{
	deformVertexes autosprite
	{
		clampmap models/weapons2/plasma/f_plasma.tga
		blendfunc add
		tcMod rotate 1246
	}
	{
		clampmap models/weapons2/plasma/f_plasma.tga
		blendfunc add
		tcMod rotate -1246
	}
}

models/weapons2/plasma/flare
{
	deformVertexes autosprite
	{
		map models/weapons2/plasma/flare.tga
		blendfunc add
	}
}
 
cmuz_pls_cone1
{
	cull disable
	{
		map textures/oa/muzzle/muz3.tga
		blendfunc add
		rgbGen const ( 0.133333 0.152941 0.176471 )
		tcMod rotate 1024
	}
	{
		map textures/oa/muzzle/muz2.tga
		blendfunc add
		rgbGen const ( 0.270588 0.427451 0.603922 )
		tcMod rotate -1024
	}
}

cmuz_pls_cone2
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.113725 0.145098 0.160784 )
		tcMod rotate 128
	}
}

cmuz_pls_cone3
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.0745098 0.0862745 0.101961 )
		tcMod rotate 128
	}
}

cmuz_pls_fuzz1
{
	deformVertexes autosprite
	{
		animmap 27 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz8.tga 
		blendfunc add
		rgbGen const ( 0.196078 0.27451 0.352941 )
	}
}

cmuz_pls_fuzz2
{
	deformVertexes autosprite
	{
		animmap 36 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga 
		blendfunc add
		rgbGen const ( 0.160784 0.180392 0.278431 )
	}
}

cmuz_pls_fuzz3
{
	deformVertexes autosprite
	{
		animmap 21 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.0862745 0.168627 0.313726 )
	}
}

cmuz_pls_fuzz4
{
	deformVertexes autosprite
	{
		animmap 52 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.196078 0.278431 0.372549 )
	}
}

// ------------------------------------------------------------
// 8.Railgun
// ------------------------------------------------------------

models/weapons2/railgun/blackchrome
{
	{
		map models/weapons2/railgun/blackchrome.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
}

models/weapons2/railgun/blackhandle
{
	{
		map models/weapons2/railgun/blackhandle.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
}

models/weapons2/railgun/muzzlside
{
	cull disable
	{
		map models/weapons2/railgun/muzzlside.tga
		blendfunc add
	}
}

models/weapons2/railgun/muzzlecenter
{
	deformVertexes autosprite
	{
		clampmap models/weapons2/railgun/f_plasma.tga
		blendfunc add
		tcMod rotate 1246
	}
	{
		clampmap models/weapons2/railgun/f_railgun.tga
		blendfunc add
		tcMod rotate -1246
	}
}

models/weapons2/railgun/flare
{
	deformVertexes autosprite
	{
		map models/weapons2/railgun/flare.tga
		blendfunc add
		rgbGen wave noise 0.5 0.5 0 1 
	}
}

models/weapons2/railgun/skin
{
	{
		map models/weapons2/railgun/skin.tga
		rgbGen lightingDiffuse
	}
}

models/weapons2/railgun/energy
{
	{
		map models/weapons2/railgun/scrolly.tga
		rgbGen entity
		tcMod scroll -2 0
	}
	{
		map models/weapons2/railgun/scrolly.tga
		blendfunc add
		rgbGen entity
		tcMod scroll -1 0
	}
	{
		map gfx/damage/shadow.tga
		blendfunc filter
		tcMod scale 1.1 1
	}
}

models/weapons2/railgun/glass
{
	cull disable
	{
		map textures/effects/tinfx2.tga
		blendfunc add
		rgbGen lightingDiffuse
		tcGen environment 
	}
}

lowRailgunFlash
{
	cull disable
	{
		clampmap models/weapons2/railgun/f_railgun2.jpg
		blendfunc add
		rgbGen entity
		tcMod rotate -74
	}
	{
		clampmap models/weapons2/railgun/f_railgun2.jpg
		blendfunc add
		rgbGen entity
		tcMod rotate 53
	}
}

railDisc
{
	cull disable
	{
		clampmap models/weapons2/railgun/f_railgun3.jpg
		blendfunc add
		rgbGen vertex
		tcmod rotate 130
	}
}

railCore
{
	sort nearest
	cull disable
	{
		map models/weapons2/railgun/railcore.tga
		blendfunc add
		rgbGen Vertex
		tcMod scroll -1 0
	}
}

// ------------------------------------------------------------
// 9.Rocketlauncher
// ------------------------------------------------------------

models/weapons2/rocketl/blackchrome
{
	{
		map models/weapons2/rocketl/blackchrome.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
}

models/weapons2/rocketl/blackhandle
{
	{
		map models/weapons2/rocketl/blackhandle.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
}

models/weapons2/rocketl/muzzlside
{
	cull disable
	{
		map models/weapons2/rocketl/muzzlside.tga
		blendfunc add
	}
}

rocketFlash
{
	deformVertexes autosprite
	{
		clampmap models/weapons2/rocketl/muzzy.tga
		blendfunc add
		tcMod rotate 1246
	}
	{
		clampmap models/weapons2/rocketl/muzzy.tga
		blendfunc add
		tcMod rotate -1246
	}
}

rocketFlare
{
	deformVertexes autosprite
	{
		clampmap textures/flares/wide.tga
		blendfunc add
		rgbGen const ( 0.980392 0.882353 0.737255 )
		tcMod stretch inversesawtooth 0.6 0.4 0 4 
	}
}

rocketThrust
{
	cull disable
	{
		clampmap textures/flares/flarey.tga
		blendfunc add
		rgbGen const ( 0.541176 0.341176 0.156863 )
		tcMod rotate -96
		tcMod stretch sin 0.3 0.7 0 8 
	}
	{
		clampmap textures/flares/wide.tga
		blendfunc add
		rgbGen const ( 0.843137 0.588235 0.4 )
	}
	{
		clampmap textures/flares/newflare.tga
		blendfunc add
		rgbGen const ( 0.713726 0.270588 0.121569 )
		tcMod rotate 46
		tcMod stretch inversesawtooth 0.6 0.4 0 4 
	}
}

models/weapons2/rocketl/flare
{
	deformVertexes autosprite
	{
		map models/weapons2/rocketl/flare.tga
		blendfunc add
	}
}

cmuz_rck_cone1
{
	cull disable
	{
		map textures/oa/muzzle/muz3.tga
		blendfunc add
		rgbGen const ( 0.67451 0.286275 0.156863 )
		tcMod rotate 1024
	}
	{
		map textures/oa/muzzle/muz2.tga
		blendfunc add
		rgbGen const ( 0.784314 0.317647 0.0901961 )
		tcMod rotate -1024
	}
}

cmuz_rck_cone2
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.576471 0.352941 0.192157 )
		tcMod rotate 128
	}
}

cmuz_rck_cone3
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.0352941 0.0196078 0.0156863 )
		tcMod rotate 128
	}
}

cmuz_rck_fuzz1
{
	deformVertexes autosprite
	{
		animmap 27 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz8.tga 
		blendfunc add
		rgbGen const ( 0.639216 0.203922 0.0941177 )
	}
}

cmuz_rck_fuzz2
{
	deformVertexes autosprite
	{
		animmap 36 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga 
		blendfunc add
		rgbGen const ( 0.466667 0.176471 0.0745098 )
	}
}

cmuz_rck_fuzz3
{
	deformVertexes autosprite
	{
		animmap 21 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.47451 0.133333 0.0745098 )
	}
}

cmuz_rck_fuzz4
{
	deformVertexes autosprite
	{
		animmap 52 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.337255 0.121569 0.0470588 )
	}
}

rocketProjectile
{

	{
		map	models/ammo/rocket/rocket.tga
	rgbGen lightingDiffuse
	}
	{
		map models/weapons2/machinegun/woodspec.tga
		blendfunc gl_dst_color gl_src_color
		//rgbGen lightingDiffuse
		tcGen environment 
	}
}

rocketThrustf
{
	deformVertexes autosprite2
	{
		map models/weapons2/machinegun/f_machinegun.tga
		blendfunc add
		tcMod rotate 1246
	}
	{
		map models/weapons2/machinegun/f_machinegun.tga
		blendfunc add
		tcMod rotate -1246
	}
}

rocketThrust
{
	{
	// do nothing.
	}
}
rocketFlare
{
	deformVertexes autosprite
	{
		map textures/oafx/rocketflare.tga
		blendfunc add
		
	}
	
}

// ------------------------------------------------------------
// 10.Shotgun
// ------------------------------------------------------------

models/weapons2/shotgun/skinoa
{
    {
        map models/weapons2/shotgun/shotgun.tga
        rgbGen lightingDiffuse
    }
    {
        map models/weapons2/shotgun/shotgun_spec.jpg
        blendFunc GL_SRC_ALPHA GL_ONE
        detail
        alphaGen lightingSpecular
    }
}

models/weapons2/shotgun/shotgun_laser
{
	
	{
		map	models/weapons2/shotgun/shotgun_laser.tga
		blendfunc add
	}
}

models/weapons2/shells/sgunshell_2
{
	deformVertexes autosprite
    {
        map models/weapons2/shells/sgunshell_2
        rgbGen lightingDiffuse
	blendFunc blend
    }

}

shotgunFlash
{
	cull disable
	{
		map models/weapons2/shotgun/muzzley.tga
		blendfunc add
	}
}

cmuz_def_cone1
{
	cull disable
	{
		map textures/oa/muzzle/muz3.tga
		blendfunc add
		rgbGen const ( 0.992157 0.533333 0.192157 )
		tcMod rotate 1024
	}
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.992157 0.533333 0.192157 )
		tcMod rotate -1024
	}
}

//The "fuzz" cone, used only for the _1 LOD mesh in subsitution
cmuz_def_cone2
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.980392 0.458824 0.0705882 )
		tcMod rotate 128
	}
}

//Only seen in highest LOD mesh for some more soft detail
cmuz_def_cone3
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.152941 0.0666667 0.00392157 )
		tcMod rotate 128
	}
}

//Muzzfuzzes for highest detail muzzleflash, four of them for maximum variance
cmuz_def_fuzz1
{
	deformVertexes autosprite
	{
		animmap 27 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz8.tga 
		blendfunc add
		rgbGen const ( 0.584314 0.352941 0.14902 )
	}
}

cmuz_def_fuzz2
{
	deformVertexes autosprite
	{
		animmap 36 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga 
		blendfunc add
		rgbGen const ( 0.584314 0.352941 0.14902 )
	}
}

cmuz_def_fuzz3
{
	deformVertexes autosprite
	{
		animmap 21 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.584314 0.352941 0.14902 )
	}
}

cmuz_def_fuzz4
{
	deformVertexes autosprite
	{
		animmap 52 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.584314 0.352941 0.14902 )
	}
}

muzzleSprite
{
	sort additive
	cull disable
	deformVertexes autosprite
	{
		map	models/weapons2/shotgun/shotgun_flash.tga
		blendfunc add
	}
}
muzzleSide
{
	sort additive
	deformVertexes autosprite2
	{
		map	models/weapons2/shotgun/shotgun_flashside.tga
		blendfunc add
	}
}

muzzleSphere
{
	sort additive
	cull disable
	{
		map	models/weapons2/shotgun/shotgun_flash.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 11.Rearfire Flametrail
// ------------------------------------------------------------

models/rearfire/flametrail03
{
//	deformVertexes autoSprite	
        {
		clampmap models/mapobjects/barrel/barrel2fx.tga
		blendFunc GL_ONE GL_ONE
               // rgbgen wave triangle 1 1.4 0 9.5
                tcMod rotate 200
	}	
        {
		clampmap models/mapobjects/barrel/barrel2fx.tga
		blendFunc GL_ONE GL_ONE
               // rgbgen wave triangle 1 1 0 8.7
                tcMod rotate -100
	}	
	
}

models/rearfire/flametrail02
{
	
	surfaceparm nomarks
	surfaceparm nolightmap
	cull none

	{
		animMap 10 textures/sfx/flame1.tga textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave inverseSawtooth 0 1 0 10
		
	}	
	{
		animMap 10 textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga textures/sfx/flame1.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sawtooth 0 1 0 10
	}	


	{
		map textures/sfx/flameball.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sin .6 .2 0 .6	
	}

}

models/rearfire/flametrail01
{
	
	surfaceparm nomarks
	surfaceparm nolightmap
	cull none

	{
		animMap 10 textures/sfx/flame1.tga textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave inverseSawtooth 0 1 0 10
		
	}	
	{
		animMap 10 textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga textures/sfx/flame1.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sawtooth 0 1 0 10
	}	


	{
		map textures/sfx/flameball.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sin .6 .2 0 .6	
	}

}

// ------------------------------------------------------------
// 12.Rearfire Bioweapon
// ------------------------------------------------------------

gfx/damage/bio_mark
{
	polygonOffset
	{
		map gfx/damage/bio_mark.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
//          tcmod rotate 011
//          tcmod turb .999 .02 1 .1
		rgbGen exactVertex
	}
	{
		map gfx/damage/biomark_glow.tga
		blendFunc add
//          tcmod rotate 011
//          tcmod turb .999 .02 1 .1
		rgbGen exactVertex
//		rgbGen wave sin 1 .5 1 1
	}

}

// ------------------------------------------------------------
// 13.Rearfire Oilmarks
// ------------------------------------------------------------

gfx/damage/oil_mark
{
	polygonOffset


	{
		map gfx/damage/oil_mark.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
	{	
	      map textures/reflect/oilreflect.tga
		blendFunc add
	      rgbGen exactVertex
	}
	{
		map gfx/damage/oil_mark_reflect.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
}

// ------------------------------------------------------------
// 14.Rearfire Flameballs
// ------------------------------------------------------------

sprites/flameball
{
	cull disable
	{
		clampmap sprites/flameball.tga
		blendfunc GL_ONE GL_ONE
                tcMod rotate 931
	}
}

