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

models/powerups/sheild/sheild
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
models/powerups/regen/regen
{
	{
		map textures/effects/envmaprail.tga
                //map textures/sfx/specular.tga
		tcGen environment
	}
}

models/powerups/gear1
{
	{
		map textures/effects/envmaprail.tga
		tcGen environment
		
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

models/powerups/env/atomic
{
	{
		map textures/effects/envmapgold.tga
		blendfunc GL_ONE GL_ZERO
		tcGen environment
                rgbGen identity
	}
}

models/powerups/shield/shield
{
	{
		map textures/effects/envmapred.tga
		tcGen environment
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

models/powerups/env/atomic
{
	{
		map textures/effects/envmapgold.tga
		blendfunc GL_ONE GL_ZERO
		tcGen environment
                rgbGen identity
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

//q3r haste

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

//q3r turbo booster

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

//q3r turbo thrust

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

//*****************************************//
//  q3r armor shards - based on ids shard  //  
//*****************************************//

//
// mapobjects
//
models/powerups/armor/energy_red1
{
   
	{
		map models/powerups/armor/energy_red1.tga 
                blendFunc GL_ONE GL_ONE
		tcMod scroll 7.4 1.3
	}

}

models/powerups/armor/energy_yel1
{

	{
		map models/powerups/armor/energy_yel3.tga 
		blendFunc GL_ONE GL_ONE
		tcMod scroll 7.4 1.3
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

//green shard
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


//yellow shard
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

//red shard
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

//************************************//
//  q3r health - based on ids health  //  
//************************************//

models/powerups/health/red2
{	
	
	{
		map textures/effects/envmapred.tga
                tcGen environment
		blendfunc GL_ONE GL_ZERO
		
	}
//        {
//		map models/powerups/health/redelectric.jpg
//                tcmod scale 2 2
//                tcmod rotate 333
//                tcmod scroll 9 9
//		blendfunc GL_ONE GL_ONE
//		
//	}
}

models/powerups/health/red_sphere2
{
	{
		map textures/effects/tinfx2b.tga
		tcGen environment
		blendfunc GL_ONE GL_ONE
	}
}

//******************************************************//
//	models/powerups/health subfolder		//
//******************************************************//

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

models/powerups/health/green
{
	
	{
		map textures/effects/envmaprail.tga
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

models/powerups/health/red
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

models/powerups/health/yellow
{
	
	{
		map textures/effects/envmapyel.tga
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

//
// health
//
//STONELANCE - update
models/powerups/health/yellow
{
	
	{
		map textures/effects/envmapyel.tga
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
		map textures/effects/envmaprail.tga
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
// END

models/powerups/ammo/grenammo
{
{
map models/powerups/ammo/ammobox.tga
rgbGen lightingDiffuse
}
{
map models/powerups/ammo/ammolights.tga
blendFunc gl_src_alpha gl_one_minus_src_alpha
rgbGen const ( 0 0.56 0 )
alphaGen wave sawtooth 0 1 0 1
}
}

models/powerups/ammo/grenammo2
{
{
map models/powerups/ammo/grenammo2.tga
rgbGen identity
}
}

models/powerups/ammo/machammo
{
{
map models/powerups/ammo/ammobox.tga
rgbGen lightingDiffuse
}
{
map models/powerups/ammo/ammolights.tga
blendFunc gl_src_alpha gl_one_minus_src_alpha
rgbGen const ( 1 1 0 )
alphaGen wave sawtooth 0 1 0 1
}
}

models/powerups/ammo/machammo2
{
{
map models/powerups/ammo/machammo2.tga
rgbGen identity
}
}

models/powerups/ammo/flameammo
{
{
map models/powerups/ammo/ammobox.tga
rgbGen lightingDiffuse
}
{
map models/powerups/ammo/ammolights.tga
blendFunc gl_src_alpha gl_one_minus_src_alpha
rgbGen const ( 0 1 0 )
alphaGen wave sawtooth 0 1 0 1
}
}

models/powerups/ammo/flameammo2
{
{
map models/powerups/ammo/flameammo2.tga
rgbGen identity
}
}


models/powerups/ammo/railammo
{
{
map models/powerups/ammo/ammobox.tga
rgbGen lightingDiffuse
}
{
map models/powerups/ammo/ammolights.tga
blendFunc gl_src_alpha gl_one_minus_src_alpha
rgbGen const ( 0 1 0 )
alphaGen wave sawtooth 0 1 0 1
}
}

models/powerups/ammo/railammo2
{
{
map models/powerups/ammo/railammo2.tga
rgbGen identity
}
}

models/powerups/ammo/rockammo
{
{
map models/powerups/ammo/ammobox.tga
rgbGen lightingDiffuse
}
{
map models/powerups/ammo/ammolights.tga
blendFunc gl_src_alpha gl_one_minus_src_alpha
rgbGen const ( 1 0 0 )
alphaGen wave sawtooth 0 1 0 1
}
}

models/powerups/ammo/rockammo2
{
{
map models/powerups/ammo/rockammo2.tga
rgbGen identity
}
}

models/powerups/ammo/shotammo
{
{
map models/powerups/ammo/ammobox.tga
rgbGen lightingDiffuse
}
{
map models/powerups/ammo/ammolights.tga
blendFunc gl_src_alpha gl_one_minus_src_alpha
rgbGen const ( 1 0.5 0 )
alphaGen wave sawtooth 0 1 0 1
}
}

models/powerups/ammo/shotammo2
{
{
map models/powerups/ammo/shotammo2.tga
rgbGen identity
}
}
