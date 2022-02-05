// ------------------------------------------------------------
// Shaders for Q3Rally powerups - rewritten by P3rlE & insellium
// based on powerups_shaders
// ------------------------------------------------------------

// ------------------------------------------------------------
// 1.haste
// ------------------------------------------------------------
icons/haste
{
       nopicmip
       {
              map icons/haste.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

powerups/haste
{
	deformVertexes wave 100 sin 3 0 0 0
	{
		map textures/effects/hastemap.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
		//tcMod turb 0 0.2 0 .2
                tcmod scroll 1 .1
	}
}

models/powerups/haste/haste
{
	{
		map models/powerups/haste/haste.tga
			tcGen environment
	}
	{
		map models/powerups/armor/energy_yel3.tga 
		blendFunc GL_ONE GL_ONE
		tcMod scroll 7.4 -1.3
	}

}

models/powerups/haste/hasteshell
{
	{
		map models/powerups/armor/energy_yel3.tga 
		blendFunc GL_ONE GL_ONE
		tcMod scroll 7.4 -1.3
	}
	{
		map models/powerups/armor/energy_yel3.tga 
		blendFunc GL_ONE GL_ONE
		tcMod scroll 7.4 -1.8
	}
}

// ------------------------------------------------------------
// 2.shield
// ------------------------------------------------------------

icons/shield
{
       nopicmip
       {
              map icons/shield.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

models/powerups/shield/shield
{
	{
		map textures/effects/envmapred.tga
		tcGen environment
	}
}

powerups/shield
{
	deformVertexes wave 100 sin 3 0 0 0
	{
		map textures/effects/shieldmap.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
		//tcMod turb 0 0.2 0 .2
                tcmod scroll 1 .1
	}
}

// ------------------------------------------------------------
// 3.regeneration
// ------------------------------------------------------------

icons/regen
{
       nopicmip
       {
              map icons/regen.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

models/powerups/regen/regen
{
	{
		map textures/effects/envmaprail.tga
                //map textures/sfx/specular.tga
		tcGen environment
	}
}

powerups/regen
{
	deformVertexes wave 100 sin 3 0 0 0
	{
		map textures/effects/regenmap2.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
		//tcMod turb 0 0.2 0 .2
                tcmod scroll 1 .1
	}
}

// ------------------------------------------------------------
// 4.gear1
// ------------------------------------------------------------

models/powerups/gear1
{
	{
		map textures/effects/envmaprail.tga
		tcGen environment
		
	}
}

// ------------------------------------------------------------
// 5.invisibility
// ------------------------------------------------------------

icons/invis
{
       nopicmip
       {
              map icons/invis.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

models/powerups/invis/invis
{
	{
		map textures/effects/tinfx2c.tga
                //map textures/sfx/specular.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
	}
}

powerups/invisibility
{
	{
		map textures/effects/invismap.tga
                //map textures/sfx/specular.tga
		blendfunc GL_ONE GL_ONE
		tcMod turb 0 0.15 0 0.25
		tcGen environment
	}
}

// ------------------------------------------------------------
// 6.atomic
// ------------------------------------------------------------

icons/env
{
       nopicmip
       {
              map icons/env.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

models/powerups/env/atomic
{
	{
		map textures/effects/envmapgold.tga
		blendfunc GL_ONE GL_ZERO
		tcGen environment
                rgbGen identity
	}
}

// ------------------------------------------------------------
// 7.quad
// ------------------------------------------------------------

icons/quad
{
       nopicmip
       {
              map icons/quad.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

models/powerups/quad/quad
{
	//{
	//	map gfx/colors/darkblue.tga
	//}
	{
		map textures/effects/envmapblue.tga
		blendfunc GL_ONE GL_ZERO
		tcGen environment
                rgbGen identity
	}
}

models/powerups/quad/quadpurple
{
	{
		map textures/effects/quadpurple.tga
		blendfunc GL_ONE GL_ZERO
		tcGen environment
		rgbGen identity
			tcmod rotate 30
			tcmod scroll 1 .1
	}
}

models/powerups/quad/mega2
{	
	
	{
		map textures/effects/envmapblue.tga
                tcGen environment
		blendfunc GL_ONE GL_ZERO
		
	}
        {
		map textures/sfx/kenelectric.tga
                tcmod scale 2 2
                tcmod rotate 333
                tcmod scroll 9 9
		blendfunc GL_ONE GL_ONE
		
	}
}

powerups/quad
{
	deformVertexes wave 100 sin 3 0 0 0
	{
		map textures/effects/quadmap2.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
		//tcMod turb 0 0.2 0 .2
                tcmod scroll 1 .1
	}
}

powerups/quadWeapon
{
	deformVertexes wave 100 sin 0.5 0 0 0
	{
		map textures/effects/quadmap2.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
		//tcMod turb 0 0.2 0 .2
                tcmod scroll 1 .1
	}
}

// ------------------------------------------------------------
// 8.turbo
// ------------------------------------------------------------

icons/turbo
{
       nopicmip
       {
              map icons/turbo.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

models/barrels/turbo
{	
	
	{
		map textures/effects/envmapgold.tga
                tcGen environment
		blendfunc GL_ONE GL_ZERO
		
	}
        {
		map textures/sfx/kenelectric.tga
                tcmod scale 2 2
                tcmod rotate 333
                tcmod scroll 9 9
		blendfunc GL_ONE GL_ONE
		
	}
}

models/powerups/turbo/booster
{

	{
//		map models/barrels/chrometest2.jpg
//		map textures/effects/tinfx2c.tga
               map textures/sfx/specular.tga

		tcGen environment
	}
	{
		map models/powerups/turbo/booster.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

models/powerups/turbo/thrust
{
	{
		map models/powerups/turbo/thrust.tga 
		blendFunc	GL_SRC_ALPHA GL_ONE
	}
	{
		map models/powerups/turbo/burn.tga
		tcMod scroll 0 -3
		blendFunc	GL_DST_COLOR GL_ONE
	}
	{
		map models/powerups/turbo/burn.tga
		tcMod scroll 0 -1
		blendFunc	GL_DST_COLOR GL_ONE
	}
}

// ------------------------------------------------------------
// 9.armor_red
// ------------------------------------------------------------

models/powerups/armor/energy_red1
{
   
	{
		map models/powerups/armor/energy_red1.tga 
                blendFunc GL_ONE GL_ONE
		tcMod scroll 7.4 1.3
	}

}

// ------------------------------------------------------------
// 10.armor_yellow
// ------------------------------------------------------------

models/powerups/armor/energy_yel1
{

	{
		map models/powerups/armor/energy_yel3.tga 
		blendFunc GL_ONE GL_ONE
		tcMod scroll 7.4 1.3
	}

}

// ------------------------------------------------------------
// 15.armor_green
// ------------------------------------------------------------

models/powerups/armor/energy_grn1
{
   deformVertexes wave 100 sin 2 0 0 0
	{
		map textures/effects/envmaprail.tga
		blendFunc GL_ONE GL_ONE
                tcGen environment
                tcmod rotate 30
		tcMod scroll 1 1
                rgbGen wave triangle -.3 1.3 0 .3
	}

}

// ------------------------------------------------------------
// 16.armor_shard2
// ------------------------------------------------------------

models/powerups/armor/shard2
{
        {
                map textures/effects/tinfx2c.tga
                tcGen environment
                rgbGen identity
        }
        {
                map models/powerups/armor/shard2.tga
                blendFunc blend
		    blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
                rgbGen lightingdiffuse
        }
}

models/powerups/armor/greenshard
{
        {
                map textures/effects/envmaprail.tga
                tcGen environment
                rgbGen identity
        }
        {
                map models/powerups/armor/shardgrn.tga
                blendFunc blend
//                rgbGen lightingdiffuse
                rgbGen identity
        }
}

models/powerups/armor/energy_grn1
{
   deformVertexes wave 100 sin 2 0 0 0
	{
		map textures/effects/envmaprail.tga
		blendFunc GL_ONE GL_ONE
                tcGen environment
                tcmod rotate 30
		tcMod scroll 1 1
                rgbGen wave triangle -.3 1.3 0 .3
	}

}

models/powerups/armor/yellowshard
{
        {
                map textures/effects/envmapmach.tga
                tcGen environment
                rgbGen identity
        }
        {
                map models/powerups/armor/shardylw.tga
                blendFunc blend
//                rgbGen lightingdiffuse
                rgbGen identity
        }
}

models/powerups/armor/energy_ylw1
{
   deformVertexes wave 100 sin 2 0 0 0
	{
		map textures/effects/envmapligh.tga
		blendFunc GL_ONE GL_ONE
                tcGen environment
                tcmod rotate 30
		tcMod scroll 1 1
                rgbGen wave triangle -.3 1.3 0 .3
	}

}

models/powerups/armor/redshard
{
        {
                map textures/effects/envmaproc.tga
                tcGen environment
                rgbGen identity
        }
        {
                map models/powerups/armor/shardred.tga
                blendFunc blend
//                rgbGen lightingdiffuse
                rgbGen identity
        }
}

models/powerups/armor/energy_red1
{
   deformVertexes wave 100 sin 2 0 0 0
	{
		map textures/effects/regenmap2.tga
		blendFunc GL_ONE GL_ONE
                tcGen environment
                tcmod rotate 30
		tcMod scroll 1 1
                rgbGen wave triangle -.3 1.3 0 .3
	}

}

// ------------------------------------------------------------
// 17.health
// ------------------------------------------------------------
models/powerups/health/megabox
{
	{
		map models/powerups/health/box.tga
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/health/lights.tga
		blendfunc blend
		rgbGen const ( 0 0.4 1 )
		alphaGen wave sawtooth 0 1 0 1 
	}
}

models/powerups/health/largebox
{
	{
		map models/powerups/health/box.tga
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/health/lights.tga
		blendfunc blend
		rgbGen const ( 1 0 0 )
		alphaGen wave sawtooth 0 1 0 1 
	}
}

models/powerups/health/mediumbox
{
	{
		map models/powerups/health/box.tga
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/health/lights.tga
		blendfunc blend
		rgbGen const ( 1 1 0 )
		alphaGen wave sawtooth 0 1 0 1 
	}
}

models/powerups/health/smallbox
{
	{
		map models/powerups/health/box.tga
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/health/lights.tga
		blendfunc blend
		rgbGen const ( 0 1 0 )
		alphaGen wave sawtooth 0 1 0 1 
	}
}

//HEALTHBOX EFFECTS

models/powerups/health/red2
{	
	
	{
		map textures/effects/envmapred.jpg
                tcGen environment
		blendfunc GL_ONE GL_ZERO
		
	}

}

models/powerups/health/red_sphere2
{
	{
		map textures/effects/tinfx2b.tga
		tcGen environment
		blendfunc GL_ONE GL_ONE
	}
}

models/powerups/health/blue
{
	{
		map textures/effects/envmapbfg.tga
		tcGen environment
		
	}
}

models/powerups/health/blue_sphere
{
	{
		map textures/reflect/reflect.jpg
		tcGen environment
		blendfunc GL_ONE GL_ONE
	}
}

models/powerups/health/mega
{	
	{
		map textures/effects/tinfx2b.tga
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}
	{
		map textures/sfx/proto_zzztblu3.tga
		rgbGen identity
		tcMod turb 1 1 0.5 0.3
		tcGen environment 
	}
}

models/powerups/health/mega1
{
	{
		map textures/effects/envmapblue2.tga
                tcGen environment
                tcmod rotate 33
                tcmod scroll 1 1
		blendfunc GL_ONE GL_ONE
	}
}

models/powerups/health/mega2
{	
	
	{
		map textures/effects/envmapblue.tga
                tcGen environment
		blendfunc GL_ONE GL_ZERO
		
	}
        {
		map textures/sfx/kenelectric.tga
                tcmod scale 2 2
                tcmod rotate 333
                tcmod scroll 9 9
		blendfunc GL_ONE GL_ONE
		
	}
}

models/powerups/health/yellow
{
	{
		map textures/effects/tinfx2b.tga
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}
	{
		map textures/effects/envmapyel.jpg
                tcGen environment
		//blendfunc GL_ONE GL_ONE
		
	}
        
}

models/powerups/health/yellow_sphere
{
	{
		map textures/reflect/reflect.jpg
		tcGen environment
		blendfunc GL_ONE GL_ONE
	}
}

models/powerups/health/red
{	
	{
		map textures/effects/tinfx2b.tga
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}
	{
		map textures/sfx/proto_zzzt.tga
		rgbGen identity
		tcMod turb 1 1 0.5 0.3
		tcGen environment 
	}
}

models/powerups/health/red_sphere
{
	{
		map textures/reflect/reflect.jpg
                tcGen environment
                tcmod rotate 33
                tcmod scroll 1 1
		blendfunc GL_ONE GL_ONE
	}
}

models/powerups/health/green
{	
	{
		map textures/effects/tinfx2b.tga
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}
	{
		map textures/sfx/proto_zzztgreen.tga
		rgbGen identity
		tcMod turb 1 1 0.5 0.3
		tcGen environment 
	}
}

models/powerups/health/green_sphere
{
	{
		map textures/reflect/reflect.jpg
		tcGen environment
		blendfunc GL_ONE GL_ONE
	}
}

models/powerups/health/blue
{
	{
		map textures/effects/envmapbfg.tga
		tcGen environment
		
	}
}

models/powerups/health/blue_sphere
{
	{
		map textures/reflect/reflect.jpg
		tcGen environment
		blendfunc GL_ONE GL_ONE
	}
}

smallCross
{
	{
		map textures/oafx/greenchrm.tga
		tcMod rotate -76
		tcGen environment 
	}
	{
		map textures/oafx/greenchrm.tga
		blendfunc add
		tcMod rotate 54
		tcGen environment 
	}
}

mediumCross
{
	{
		map textures/effects/envmapligh.tga
		tcMod rotate -76
		tcGen environment 
	}
	{
		map textures/effects/envmapyel.tga
		blendfunc add
		tcMod rotate 54
		tcGen environment 
	}
}

largeCross
{
	{
		map textures/oafx/orangechrm.tga
		tcMod rotate -76
		tcGen environment 
	}
	{
		map textures/oafx/orangechrm.tga
		blendfunc add
		tcMod rotate 54
		tcGen environment 
	}
}

megaCross
{
	{
		map textures/effects/envmapblue2.tga
		tcMod rotate -76
		tcGen environment 
	}
	{
		map textures/effects/megahelth.tga
		blendfunc add
		tcMod rotate 54
		tcGen environment 
	}
}

smallCrossFlare
{
	deformVertexes autosprite
	{
		clampmap textures/oafx/greenflr1.tga
		blendfunc add
		tcMod rotate -149
		tcMod stretch sin -1 1 0 0.2 
	}
	{
		clampmap textures/oafx/greenflr2.tga
		blendfunc add
		tcMod rotate 1765
		tcMod stretch sin 0 -1.2 0 0.3 
	}
}

oldflare
{
	deformVertexes autosprite
	{
		clampmap textures/oafx/flare.tga
		blendfunc add
		tcMod rotate 12
	}
	{
		clampmap textures/oafx/flare.tga
		blendfunc add
		tcMod rotate -17
	}
}

oldysunFlare
{
	deformVertexes autosprite
	{
		clampmap textures/oafx/flare.tga
		blendfunc add
		tcMod rotate 12
	}
	{
		clampmap textures/oafx/flare.tga
		blendfunc add
		tcMod rotate -17
	}
}

largeCrossFlare
{
	deformVertexes autosprite
	{
		clampmap textures/oafx/orangeflr1.tga
		blendfunc add
		tcMod rotate -46
		tcMod stretch sin -1 0.4 0 0.2 
	}
	{
		clampmap textures/oafx/orangeflr2.tga
		blendfunc add
		tcMod rotate 122
		tcMod stretch inversesawtooth -0.7 -1 0 2 
	}
}


// ------------------------------------------------------------
// HEALTH ICONS
// ------------------------------------------------------------

icons/iconh_green
{
	nopicmip
	{
		map icons/iconh_green.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconh_yellow
{
	nopicmip
	{
		map icons/iconh_yellow.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconh_red
{
	nopicmip
	{
		map icons/iconh_red.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}

}

icons/iconh_mega
{
	nopicmip
	{
		map icons/iconh_mega.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

// Weapons

icons/iconw_flame
{
       nopicmip
       {
              map icons/iconw_flame.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

icons/rf_bio
{
       nopicmip
       {
              map icons/rf_bio.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

icons/rf_flame
{
       nopicmip
       {
              map icons/rf_flame.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

icons/rf_smoke
{
       nopicmip
       {
              map icons/rf_smoke.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

icons/rf_mine
{
       nopicmip
       {
              map icons/rf_mine.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

icons/rf_oil
{
       nopicmip
       {
              map icons/rf_oil.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

// Ammo

icons/icona_flame
{
       nopicmip
       {
              map icons/icona_flame.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

// Flags

icons/iconf_neutral1
{
       nopicmip
       {
              map icons/iconf_neutral1.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

// Health

icons/redhealth
{
       nopicmip
       {
              map icons/redhealth.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

// Shards

icons/shardg
{
       nopicmip
       {
              map icons/shardg.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

icons/shardr
{
       nopicmip
       {
              map icons/shardr.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}

icons/shardy
{
       nopicmip
       {
              map icons/shardy.tga
              blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
       }
}


icons/iconw_gauntlet
{
	nopicmip
	{
		map icons/iconw_gauntlet.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconw_machinegun
{
	nopicmip
	{
		map icons/iconw_machinegun.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconw_rocket
{
	nopicmip
	{
		map icons/iconw_rocket.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconw_shotgun
{
	nopicmip
	{
		map icons/iconw_shotgun.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen identitylighting
	}
}

icons/iconw_grenade
{
	nopicmip
	{
		map icons/iconw_grenade.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconw_lightning
{
	nopicmip
	{
		map icons/iconw_lightning.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconw_plasma
{
	nopicmip
	{
		map icons/iconw_plasma.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconw_railgun
{
	nopicmip
	{
		map icons/iconw_railgun.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconw_bfg
{
	nopicmip
	{
		map icons/iconw_bfg.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconw_nailgun
{
	nopicmip
	{
		map icons/iconw_nailgun.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

//
// ammo icons
//

icons/icona_machinegun
{
	nopicmip
	{
		map icons/icona_machinegun.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/icona_rocket
{
	nopicmip
	{
		map icons/icona_rocket.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/icona_shotgun
{
	nopicmip
	{
		map icons/icona_shotgun.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen identitylighting
	}
}

icons/icona_grenade
{
	nopicmip
	{
		map icons/icona_grenade.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/icona_lightning
{
	nopicmip
	{
		map icons/icona_lightning.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/icona_plasma
{
	nopicmip
	{
		map icons/icona_plasma.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/icona_railgun
{
	nopicmip
	{
		map icons/icona_railgun.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/icona_bfg
{
	nopicmip
	{
		map icons/icona_bfg.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/icona_nailgun
{
	nopicmip
	{
		map icons/icona_nailgun.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

powerups/blueflag
{
	deformVertexes wave 100 sin 3 0 0 0
	{
		map textures/effects/blueflagmap.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
		tcMod turb 0 0.2 0 1
	}
}

powerups/redflag
{
	deformVertexes wave 100 sin 3 0 0 0
	{
		map textures/effects/redflagmap.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
		tcMod turb 0 0.2 0 1
	}
}

icons/medkit
{
	nopicmip
	{
		map icons/medkit.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/teleporter
{
	nopicmip
	{
		map icons/teleporter.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

medal_excellent
{
	nopicmip
	{
		clampmap menu/medals/medal_excellent.tga
		blendFunc blend
	}
}

medal_gauntlet
{
	nopicmip
	{
		clampmap menu/medals/medal_gauntlet.tga
		blendFunc blend
	}
}

medal_impressive
{
	nopicmip
	{
		clampmap menu/medals/medal_impressive.tga
		blendFunc blend
	}
}

medal_telefragged
{
	nopicmip
	{
		clampmap menu/medals/medal_telefragged.tga
		blendFunc blend
	}
}

medal_frags
{
	nopicmip
	{
		clampmap menu/medals/medal_frags.tga
		blendFunc blend
	}
}

medal_assist
{
	nopicmip
	{
		clampmap menu/medals/medal_assist.tga
		blendFunc blend
	}
}

medal_defend
{
	nopicmip
	{
		clampmap menu/medals/medal_defend.tga
		blendFunc blend
	}
}

medal_capture
{
	nopicmip
	{
		clampmap menu/medals/medal_capture.tga
		blendFunc blend
	}
}

gfx/2d/cursor
{
    nopicmip
	nomipmaps
    {
        map gfx/2d/cursor.tga
    }
}

sprites/balloon3
{
	{
		map sprites/balloon3.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

waterBubble
{
	sort	underwater
	cull none
	entityMergable		
	{
		map sprites/bubble.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen		vertex
		alphaGen	vertex
	}
}


Grareflaader
{
	cull none
	{
		map gfx/misc/flare.tga
		blendFunc GL_ONE GL_ONE
		rgbGen vertex
	}
}
boens
{
	cull none
	{
		map gfx/misc/sun.tga
		blendFunc GL_ONE GL_ONE
		rgbGen vertex
	}
}
gfx/misc/tracer
{
	cull none
	{
		map	gfx/misc/tracer2.tga
		blendFunc GL_ONE GL_ONE
	}
}

gfx/2d/select
{
	nopicmip
	{
		map gfx/2d/select.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
		rgbgen vertex
	}
}

// ------------------------------------------------------------
// FONT
// ------------------------------------------------------------

gfx/2d/bigchars
{
	nopicmip
	nomipmaps
	{
		map gfx/2d/bigchars.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}

gfx/2d/bigchars_plates
{
	nopicmip
	nomipmaps
	{
		map gfx/2d/bigchars_plates.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}

gfx/2d/numbers/zero_32b
{
	nopicmip
	{
		map gfx/2d/numbers/zero_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/one_32b
{
	nopicmip
	{
		map gfx/2d/numbers/one_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/two_32b
{
	nopicmip
	{
		map gfx/2d/numbers/two_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/three_32b
{
	nopicmip
	{
		map gfx/2d/numbers/three_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/four_32b
{
	nopicmip
	{
		map gfx/2d/numbers/four_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/five_32b
{
	nopicmip
	{
		map gfx/2d/numbers/five_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/six_32b
{
	nopicmip
	{
		map gfx/2d/numbers/six_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/seven_32b
{
	nopicmip
	{
		map gfx/2d/numbers/seven_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/eight_32b
{
	nopicmip
	{
		map gfx/2d/numbers/eight_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/nine_32b
{
	nopicmip
	{
		map gfx/2d/numbers/nine_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}
gfx/2d/numbers/minus_32b
{
	nopicmip
	{
		map gfx/2d/numbers/minus_32b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbgen vertex
	}
}

scoreboardName
{
	nopicmip
	nomipmaps
	{
		clampmap menu/tab/name.tga
		blendfunc blend
	}
}

scoreboardScore
{
	nopicmip
	nomipmaps
	{
		clampmap menu/tab/score.tga
		blendfunc blend
	}
}

scoreboardTime
{
	nopicmip
	nomipmaps
	{
		clampmap menu/tab/time.tga
		blendfunc blend
	}
}

scoreboardPing
{
	nopicmip
	nomipmaps
	{
		clampmap menu/tab/ping.tga
		blendfunc blend
	}
}

// ------------------------------------------------------------
// CTF FLAGS
// ------------------------------------------------------------

icons/iconf_blu1
{
	nopicmip
	{
		map icons/iconf_blu1.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconf_blu2
{
	nopicmip
	{
		map icons/iconf_blu2.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconf_blu3
{
	nopicmip
	{
		map icons/iconf_blu3.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconf_red1
{
	nopicmip
	{
		map icons/iconf_red1.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconf_red2
{
	nopicmip
	{
		map icons/iconf_red2.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconf_red3
{
	nopicmip
	{
		map icons/iconf_red3.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconf_green1
{
	nopicmip
	{
		map icons/iconf_green1.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconf_green2
{
	nopicmip
	{
		map icons/iconf_green2.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconf_green3
{
	nopicmip
	{
		map icons/iconf_green3.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconf_yell1
{
	nopicmip
	{
		map icons/iconf_yell1.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconf_yell2
{
	nopicmip
	{
		map icons/iconf_yell2.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

icons/iconf_yell3
{
	nopicmip
	{
		map icons/iconf_yell3.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}