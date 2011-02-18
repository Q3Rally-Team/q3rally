textures/clown/blue_banner
{
	
	surfaceparm nomarks
	cull none
 	deformVertexes wave 256 sin 0 7 0 0.4
	
	{
		map $lightmap
		rgbGen identity
	}
	{
		clampmap textures/clown/blue_banner.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity

	}
}

textures/clown/red_banner
{
	
	surfaceparm nomarks
	cull none
	deformVertexes wave 256 sin 0 7 0 0.4
	
	{
		map $lightmap
		rgbGen identity
	}
	{
		clampmap textures/clown/red_banner.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		
	}
} 

textures/clown/bleeding-wall
{
	qer_editorimage textures/clown/bleeding-wall.tga
	surfaceparm nomarks
	surfaceparm trans
	cull none
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/evil8_base/e8crete03.jpg
		rgbGen identity
		blendFunc GL_ONE GL_ZERO
	}
	{
		map textures/clown/fleshy_t.tga
		blendFunc gl_dst_color gl_zero
		rgbGen identity
		alphaFunc GT0
		depthWrite
		tcMod scroll 0 -.1
		tcMod turb .02 .02 .5 .2
	}
}

textures/clown/portal
{
	qer_editorimage textures/clown/portal.tga
	q3map_surfacelight 1000
	surfaceparm nomarks
	surfaceparm trans
	cull none
	
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/skies/topclouds.jpg
		rgbGen identity
		blendFunc GL_ONE GL_ZERO
	}
	{
		map textures/clown/fleshy_t.tga
		blendFunc GL_ONE GL_ZERO
		rgbGen identity
		alphaGen lightingSpecular
		tcMod rotate 2
		tcMod scroll 0 -1
		
	}
	{
		map textures/clown/fleshy_t.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		alphaFunc GT0
		depthWrite
		tcMod rotate -2
		tcMod scroll 0 -1
		
	}
}

textures/clown/f_light2
{
	qer_editorimage textures/clown/f_light2.tga
	surfaceparm nomarks
	q3map_surfacelight 5000
	
	
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/clown/f_light2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/clown/f_light2.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/clown/red_1
{
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_surfacelight 30
//	cull none
	polygonoffset	
	{
		map textures/clown/red_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE
		rgbGen wave sin 1 1.4 2 1.2
		depthFunc equal
	}
}

textures/clown/red_2
{
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_surfacelight 30
	//cull none
	polygonoffset	
	{
		map textures/clown/red_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE
		rgbGen wave sin 1 1.4 2 1.2
		depthFunc equal
	}
}	

textures/clown/red_3
{
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_surfacelight 30
//	cull none
	polygonoffset	
	{
		map textures/clown/red_3.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE
		rgbGen wave sin 1 1.4 2 1.2
		depthFunc equal
	}
}

textures/clown/blue_1
{
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_surfacelight 30
//	cull none
	polygonoffset	
	{
		map textures/clown/blue_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE
		rgbGen wave sin 1 1.4 2 1.2
		depthFunc equal
	}
}

textures/clown/blue_2
{
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_surfacelight 30
//	cull none
	polygonoffset	
	{
		map textures/clown/blue_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE
		rgbGen wave sin 1 1.4 2 1.2
		depthFunc equal
	}
}	

textures/clown/blue_3
{
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_surfacelight 30
//	cull none
	polygonoffset
	{
		map textures/clown/blue_3.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE
		rgbGen wave sin 1 1.4 2 1.2
		depthFunc equal
	}
}

//note: the brush this is used on should be made a detail brush
//with this texture on one side, the rest of the sides should 
//be a "nodraw or caulk"
textures/clown/glass
{
		
	{
		map textures/clown/glass.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE
		rgbGen identity
		depthFunc equal
	}
}

textures/clown/portal_1r
{
	qer_editorimage textures/clown/portal_1r.tga
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nolightmap
	cull disable

	{
		map $lightmap
		blendFunc gl_dst_color gl_one_minus_src_alpha
		rgbGen identity
	}
	{
		map textures/clown/portal_1r.tga
		blendfunc gl_one gl_one
		rgbGen wave inversesawtooth .3 .01 0 .2
	}
	{
		clampmap textures/clown/portal_2r.tga
		blendFunc GL_ONE GL_ONE
		tcMod stretch sin 1.2 .3 0 1.2
	}
	{
		clampmap textures/clown/portal_4r.tga
		blendFunc GL_ONE GL_ONE
		tcMod stretch sin 1.2 .2 0.5 1.2
	}
	{
		clampmap textures/clown/portal_3r.tga
		blendfunc GL_ONE GL_ONE
		tcmod rotate 10
	}
}


textures/clown/dark_sky
{
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_lightimage textures/clown/dark_sky.tga
	q3map_sun	.5 .1 .1  50  65  35
	q3map_surfacelight  100
	

	skyparms - 1024 -
	{
	map textures/clown/dark_sky.tga
	tcMod scroll 0.02 0.05
	tcMod scale 2 1
	}
	
	
	
}

textures/clown/r_edge
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nomarks
	cull none
	
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE
		depthFunc equal
	}
	{
		map textures/clown/r_edge.tga
		blendFunc GL_ONE GL_ONE
		tcMod turb 3 .3 3 .3
	}
}

textures/clown/b_edge
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nomarks
	cull none
	
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE
		depthFunc equal
	}
	{
		map textures/clown/b_edge.tga
		blendFunc GL_ONE GL_ONE
		tcMod turb 3 .3 3 .3
	}
}

textures/clown/ankh_light
{
	qer_editorimage textures/clown/ankh_light.jpg
	surfaceparm nomarks
	q3map_surfacelight 3000
	
	
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/clown/ankh_light.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/clown/ankh_light.jpg
		blendfunc GL_ONE GL_ONE
	}
}

textures/clown/grate_1
{
	surfaceparm nomarks
	cull none
	{
		map textures/clown/grate_1.tga
		blendFunc GL_ONE GL_ZERO
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE
		rgbGen identity
		depthFunc equal
	}
}

textures/clown/grate_2
{
	
	surfaceparm nomarks
	cull none
	{
		map textures/clown/grate_2.tga
		blendFunc GL_ONE GL_ZERO	
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE
		rgbGen identity
		depthFunc equal
	}
}

textures/base_wall/main_q3abanner
{
	q3map_lightimage textures/base_wall/main_q3abanner.jpg
        q3map_surfacelight 100


	{
		map textures/base_wall/main_q3abanner.jpg
	        rgbGen wave square 0 1 0 .5
	}

	{
		map textures/base_wall/comp3textb.jpg
		blendfunc add
	        rgbGen identity
		tcmod scroll 2 2
		
	}

	{
		map textures/base_wall/comp3text.jpg
		blendfunc add
	        rgbGen identity
		tcmod scroll 3 3
	}


	{
		map $lightmap
	        rgbGen identity
		blendfunc gl_dst_color gl_zero
	}

	{
		map $lightmap
		tcgen environment
		tcmod scale .5 .5
	        rgbGen wave sin .25 0 0 0
		blendfunc add
	}	          		
}   

textures/base_wall/main_q3abanner_r
{
	q3map_lightimage textures/base_wall/main_q3abanner.jpg
        q3map_surfacelight 100


	{
		map textures/base_wall/main_q3abanner_r.jpg
	        rgbGen wave square 0 1 0 .5
	}

	{
		map textures/base_wall/comp3textb.jpg
		blendfunc add
	        rgbGen identity
		tcmod scroll -2 2
	}

	{
		map textures/base_wall/comp3text.jpg
		blendfunc add
	        rgbGen identity
		tcmod scroll 3 3
	}


	{
		map $lightmap
	        rgbGen identity
		blendfunc gl_dst_color gl_zero
	}

	{
		map $lightmap
		tcgen environment
		tcmod scale .5 .5
	        rgbGen wave sin .25 0 0 0
		blendfunc add
	}	          		
}   

textures/clown/steel_b_shine
{
	qer_editorimage textures/clown/steel_b.jpg
	{
		map textures/clown/steel_b.jpg
		rgbGen identity
	}
	{
		clampmap textures/base_wall/chrome_env.jpg
		blendfunc add
		rgbGen const ( 0.188235 0.188235 0.407843 )
		tcGen environment 
	}
	{
		map textures/clown/steel_b.jpg
		blendfunc filter
		
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_src_alpha
		tcGen lightmap 
	}
}

textures/clown/steel_r_shine
{
	qer_editorimage textures/clown/steel_r.jpg
	{
		map textures/clown/steel_r.jpg
		rgbGen identity
	}
	{
		clampmap textures/base_wall/chrome_env.jpg
		blendfunc add
		rgbGen const ( 0.188235 0.188235 0.407843 )
		tcGen environment 
	}
	{
		map textures/clown/steel_r.jpg
		blendfunc filter
		
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_src_alpha
		tcGen lightmap 
	}
}

textures/clown/steel_rust_shine
{
	qer_editorimage textures/clown/steel_rust.jpg
	{
		map textures/clown/steel_rust.jpg
		rgbGen identity
	}
	{
		clampmap textures/base_wall/chrome_env.jpg
		blendfunc add
		rgbGen const ( 0.188235 0.188235 0.407843 )
		tcGen environment 
	}
	{
		map textures/clown/steel_r.jpg
		blendfunc filter
		
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_src_alpha
		tcGen lightmap 
	}
}

textures/clown/burn_ow
{
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_surfacelight 30
	cull none	
	
	{
		map textures/clown/burn_ow.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		alphaFunc GT0
		depthWrite
		
	}
	{
		map $lightmap
		blendFunc gl_dst_color gl_one_minus_src_alpha
		rgbGen identity
		depthFunc equal
	}
	
}

textures/gothic_block/mkc_evil_e3window
{
	qer_editorimage textures/gothic_block/windowevil2c_block18c.jpg
	q3map_lightimage textures/gothic_block/evil2cglow.jpg
	q3map_surfacelight 200
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/gothic_block/windowevil2c_block18c.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/gothic_block/evil2cglow.jpg
		blendfunc add
	}
}

textures/gothic_block/evil_e3bwindow
{
	qer_editorimage textures/gothic_block/windowevil2c_killblock.jpg
	q3map_lightimage textures/gothic_block/evil2ckillblockglow.jpg
	q3map_surfacelight 200
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/gothic_block/windowevil2c_killblock.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/gothic_block/evil2ckillblockglow.jpg
		blendfunc add
	}
}

textures/gothic_block/evil_e3window
{
	qer_editorimage textures/gothic_block/windowevil2c_block18c.jpg
	q3map_lightimage textures/gothic_block/evil2cglow.jpg
	q3map_surfacelight 900
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/gothic_block/windowevil2c_block18c.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/gothic_block/evil2cglow.jpg
		blendfunc add
	}
}

textures/gothic_block/evil_e3window_dark
{
	qer_editorimage textures/gothic_block/windowevil2c_block18c.jpg
	q3map_lightimage textures/gothic_block/evil2cglow.jpg
	q3map_surfacelight 100
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/gothic_block/windowevil2c_block18c.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/gothic_block/evil2cglow.jpg
		blendfunc add
	}
}

textures/gothic_block/wetwall
{
	
        {
	        map textures/gothic_block/wetwall.tga
	        rgbGen identity
	}
        {
		map textures/gothic_block/wetwall_fx.tga
                tcmod scroll 0 -.1
		tcmod scale .9 .1
                blendFunc add
	}
	{
	        map textures/gothic_block/wetwall.tga
		blendFunc blend
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
}

textures/ctf/blue_telep2
{
	qer_editorimage textures/effects/jumpcircblue.tga
	q3map_lightimage textures/effects/jumpcircblue.tga
	surfaceparm nonsolid
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	cull disable
	q3map_surfacelight 455
	{
		clampmap textures/effects/jumpcircblue.tga
		blendfunc add
		tcMod rotate 46
	}
	{
		clampmap textures/sfx/jcb2.tga
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.9 -0.3 0 0.5 
	}
	{
		clampmap textures/sfx/jcb2.tga
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.5 0.3 0 0.5 
	}
}

textures/ctf/red_telep2
{
	qer_editorimage textures/effects/jumpcircred.tga
	q3map_lightimage textures/effects/jumpcircred.tga
	surfaceparm nonsolid
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	cull disable
	q3map_surfacelight 455
	{
		clampmap textures/effects/jumpcircred.tga
		blendfunc add
		tcMod rotate 46
	}
	{
		clampmap textures/sfx/jcr2.tga
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.9 -0.3 0 0.5 
	}
	{
		clampmap textures/sfx/jcr2.tga
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.5 0.3 0 0.5 
	}
}

textures/gothic_block/blocks18b_ow
{
	qer_editorimage textures/gothic_block/blocks18b_ow.tga
	surfaceparm nomarks
	
	{
		map textures/sfx/fireswirl2.jpg
		tcmod rotate 1
		tcmod turb 1 .4 0 .5
		blendfunc add
	}
	{
		map textures/gothic_block/blocks18b_ow.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

textures/base_floor/proto_rustygrate
{
	surfaceparm metalsteps
	surfaceparm nodamage
	cull none
	{
		map textures/base_floor/proto_rustygrate.tga
		blendFunc gl_one gl_zero	
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		depthFunc equal
	}
}

textures/base_floor/proto_grate
{
	surfaceparm metalsteps
	surfaceparm nodamage
	cull none
	{
		map textures/base_floor/proto_grate.tga
		blendFunc gl_one gl_zero	
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		depthFunc equal
	}
}

textures/base_floor/proto_grate4
{
	surfaceparm metalsteps
	surfaceparm nodamage
	cull none
	{
		map textures/base_floor/proto_grate4.tga
		blendFunc gl_one gl_zero	
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		depthFunc equal
	}
}

textures/base_floor/cybergrate2
{
	surfaceparm metalsteps
	cull none
	{
		map textures/sfx/portal_sfx_ring_electric.tga
		tcmod scroll 1 -1
		blendfunc add
	}
	{
		map textures/base_floor/cybergrate2.tga
		blendFunc blend	
		rgbGen identity
		
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/base_floor/cybergrate3
{
	surfaceparm metalsteps
	cull none
	{
		map textures/sfx/portal_sfx_ring_electric.tga
		tcmod scroll 1 -1
		blendfunc add
	}
	{
		map textures/base_floor/cybergrate3.tga
		blendFunc blend	
		rgbGen identity
		
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/base_trim/dark_tin2
{
	qer_editorimage textures/base_trim/dark_tin2.jpg
	{
		//mapped first so it looks good while vertex lit
		map textures/base_trim/dark_tin2.jpg
		rgbGen Identity
	}
	{
		map textures/base_wall/chrome_metal.jpg
		tcGen environment 
		rgbGen oneminusvertex
	}
	{
		map textures/base_trim/dark_tin2.jpg
		blendfunc add
	}
	
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_src_alpha
		tcGen lightmap 
	}
}

textures/base_trim/pewter_shiney
{
	qer_editorimage textures/base_trim/pewter_shiney.jpg
	{
		//mapped first so it looks good while vertex lit
		map textures/base_trim/pewter_shiney.jpg
		rgbGen Identity
	}
	{
		map textures/base_wall/chrome_env2.jpg
		tcGen environment 
		rgbGen vertex
	}
	{
		map textures/base_trim/pewter_shiney.jpg
		blendfunc add
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_src_alpha
		tcGen lightmap 
	}
}

textures/base_trim/tin
{
	qer_editorimage textures/base_trim/tin.tga
	{
		//mapped first so it looks good while vertex lit
		map textures/base_trim/tin.tga
		rgbGen Identity
	}
	{
		map textures/base_trim/tinfx.jpg
		tcGen environment 
		rgbGen vertex
	}
	{
		map textures/base_trim/tin.tga
		blendfunc add
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_src_alpha
		tcGen lightmap 
	}
}

textures/base_floor/hfloor3
{
	qer_editorimage textures/base_floor/hfloor3.jpg
	{
		//mapped first so it looks good while vertex lit
		map textures/base_floor/hfloor3.jpg
		rgbGen Identity
	}
	{
		map textures/base_trim/tinfx.jpg
		tcGen environment 
		rgbGen oneminusvertex
	}
	{
		map textures/base_floor/hfloor3.jpg
		blendfunc add
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_src_alpha
		tcGen lightmap 
	}
}

textures/base_wall/comp3b_dark
{
	qer_editorimage textures/base_wall/comp3b_dark.jpg
	q3map_lightimage textures/base_wall/comp3env.jpg
        q3map_surfacelight 10


	{
		map textures/base_wall/comp3textb.tga
	        rgbGen identity
		tcmod scroll -3 3
	}
	{
		map textures/base_wall/comp3text.jpg
		blendfunc add
	        rgbGen identity
		tcmod scroll -3 3
	}
	{
		map textures/base_wall/comp3env.jpg
		tcGen environment
	        blendfunc add
		rgbGen oneminusvertex
	}


	{
		map $lightmap
	        tcGen environment
		blendfunc gl_dst_color gl_one
	}
	{
		map textures/base_wall/comp3b.tga
        	blendFunc blend 
		rgbGen identity
		
	}
	{
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}

}   

textures/base_wall/comp3b
{
	qer_editorimage textures/base_wall/comp3b.tga
	q3map_lightimage textures/base_wall/comp3env.jpg
        q3map_surfacelight 900

	{
		map textures/base_wall/comp3b.tga
		rgbGen identity
		
	}
	{
		map textures/base_wall/comp3env.jpg
		tcGen environment
		rgbGen identity
	}
	{
		map textures/base_wall/comp3text.jpg
		blendfunc add
	        rgbGen identity
		tcmod scroll 3 3
	}
	{
		map $lightmap
	        tcGen environment
		blendfunc gl_dst_color gl_one
	}
	{
		map textures/base_wall/comp3b.tga
        	blendFunc blend
		rgbGen identity
		
	}
	{
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}

}  

textures/base_support/x_support
{
	surfaceparm metalsteps
	surfaceparm nomarks
	cull none
	{
		map textures/base_support/x_support.tga
		blendFunc gl_one gl_zero	
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		depthFunc equal
	}
}

textures/base_support/x_support2
{
	surfaceparm metalsteps
	surfaceparm nomarks
	cull none
	{
		map textures/base_support/x_support2.tga
		blendFunc gl_one gl_zero	
		rgbGen identity
		alphaFunc GT0
		depthWrite
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		depthFunc equal
	}
}

textures/base_wall/bluemetal1b_chrome
{
	qer_editorimage textures/base_wall/bluemetal1b_shiny.tga
	{
		//mapped first so it looks good while vertex lit
		map textures/base_wall/bluemetal1b_shiny.tga
		rgbGen Identity
	}
	{
		map textures/base_wall/chrome_env.jpg
		tcGen environment 
		rgbGen oneminusvertex
	}
	{
		map textures/base_wall/bluemetal1b_shiny.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
}

textures/base_wall/bluemetalsupport2fline
{
	qer_editorimage textures/base_wall/bluemetalsupport2fline.tga
	q3map_lightimage textures/clown/bms2fglow.tga
	surfaceparm nomarks
	surfaceparm metalsteps
	q3map_surfacelight 400
	{
		//mapped first so it looks good while vertex lit
		map textures/base_wall/bluemetalsupport2fline.tga
		rgbGen Identity
	}
	{
		map textures/base_wall/bms2fglow.tga
		rgbGen wave sin .4 .3 .5 1.5
		blendfunc filter
	}
	{
		map textures/base_wall/bluemetalsupport2fline.tga
		blendfunc add
		alphaGen lightingSpecular
	}
	{
		map textures/base_wall/chrome_env.jpg
		tcGen environment 
		rgbGen oneminusvertex
		tcmod scale .5 .5
		blendfunc add
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_src_alpha
		tcGen lightmap 
	}
}

textures/tim/redmetalsupport2fline
{
	qer_editorimage textures/tim/redmetalsupport2fline.tga
	q3map_lightimage textures/clown/bms2fl_rglow.tga
	surfaceparm nomarks
	surfaceparm metalsteps
	q3map_surfacelight 400
	{
		//mapped first so it looks good while vertex lit
		map textures/tim/redmetalsupport2fline.tga
		rgbGen Identity
	}
	
	{
		map textures/clown/bms2fl_rglow.tga
		tcmod scroll 1 0
		rgbGen wave sin .4 .3 .5 2
		blendfunc gl_dst_color gl_zero 
	}
	{
		map textures/tim/redmetalsupport2fline.tga
		blendfunc add
		
	}
	{
		map textures/base_wall/chrome_env.jpg
		tcGen environment 
		rgbGen oneminusvertex
		tcmod scale .5 .5
		blendfunc add
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_src_alpha
		tcGen lightmap 
	}
}

textures/base_wall/bluemetalsupport2ftv
{
	qer_editorimage textures/base_wall/bluemetalsupport2ftv.tga
	q3map_lightimage textures/base_wall/bms2ftv_glow.tga
	q3map_surfacelight 10
	
	{
		map textures/base_wall/bluemetalsupport2ftv.tga
		rgbGen identity
		
	}
	{
		map textures/base_wall/chrome_env.jpg
		tcGen environment 
		rgbGen oneminusvertex
		tcmod scale .5 .5
	}
	{
		map textures/base_wall/bms2ftv_glow.tga
		rgbGen wave sin .4 .3 0 1.1
		blendfunc add
	}
	{
		map textures/base_wall/bluemetalsupport2ftv.tga
		blendFunc add
		rgbGen identity
		
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/base_wall/bluemetalsupport2e
{
	qer_editorimage textures/base_wall/bluemetalsupport2e.tga
		
	{
		map textures/base_wall/bluemetalsupport2e.tga
		rgbGen identity
		
	}
	{
		map textures/base_wall/chrome_env.jpg
		tcGen environment 
		rgbGen oneminusvertex
		tcmod scale .5 .5
	}
	{
		map textures/base_wall/bluemetalsupport2e.tga
		blendFunc add
		rgbGen identity
		
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/base_wall/bluemetalsupport2e2
{
	qer_editorimage textures/base_wall/bluemetalsupport2e2.tga
		
	{
		map textures/base_wall/bluemetalsupport2e2.tga
		rgbGen identity
		
	}
	{
		map textures/base_wall/chrome_env.jpg
		tcGen environment 
		rgbGen oneminusvertex
		tcmod scale .5 .5
	}
	{
		map textures/base_wall/bluemetalsupport2e2.tga
		blendFunc add
		rgbGen identity
		
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/base_wall/bluemetalsupport2c_1
{
	qer_editorimage textures/base_wall/bluemetalsupport2c_1.jpg
		
	{
		map textures/base_wall/bluemetalsupport2c_1.jpg
		rgbGen identity
		
	}
	{
		map textures/base_wall/chrome_env.jpg
		tcGen environment 
		rgbGen oneminusvertex
		tcmod scale .5 .5
	}
	{
		map textures/base_wall/bluemetalsupport2c_1.jpg
		blendFunc add
		rgbGen identity
		
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/base_wall/bluemetalsupport2c
{
	qer_editorimage textures/base_wall/bluemetalsupport2c.jpg
		
	{
		map textures/base_wall/bluemetalsupport2c.jpg
		rgbGen identity
		
	}
	{
		map textures/base_wall/chrome_env.jpg
		tcGen environment 
		rgbGen oneminusvertex
		tcmod scale .5 .5
	}
	{
		map textures/base_wall/bluemetalsupport2c.jpg
		blendFunc add
		rgbGen identity
		
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/base_floor/clangdark_spot
{
	qer_editorimage textures/base_floor/clangdark_spot.tga
	surfaceparm metalsteps
	
	{
		map textures/base_floor/clangdark_spot.tga
		rgbGen identity
	}
	{
		map textures/liquids/proto_gruel2.jpg
		blendfunc gl_one gl_zero
		tcmod scroll .01 .01
		tcmod turb 0 0.01 0 .1
		alphagen lightingspecular
	}
	{
		map textures/base_floor/clangdark_spot.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/base_floor/diamond2cspot
{
	qer_editorimage textures/base_floor/diamond2cspot.tga
	surfaceparm metalsteps
	
	{
		map textures/base_floor/diamond2cspot.tga
		rgbGen identity
	}
	{
		map textures/liquids/proto_gruel3.jpg
		blendfunc gl_one gl_zero
		tcmod scroll .01 .01
		tcmod turb 0 0.01 0 .1
		alphagen lightingspecular
	}
	{
		map textures/base_floor/diamond2cspot.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/gothic_floor/q1metal7_99spot
{
	qer_editorimage textures/gothic_floor/q1metal7_99spot.tga
	surfaceparm metalsteps
	
	{
		map textures/gothic_floor/q1metal7_99spot.tga
		rgbGen identity
	}
	{
		map textures/liquids/proto_gruel3.jpg
		blendfunc gl_one gl_zero
		tcmod scroll .01 .01
		tcmod turb 0 0.01 0 .1
		alphagen lightingspecular
	}
	{
		map textures/gothic_floor/q1metal7_99spot.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendFunc filter 
		rgbGen identity
	}
}

textures/base_wall/patch10shiny
{
	qer_editorimage textures/base_wall/patch10.jpg
		
	{
		map textures/base_wall/patch10.jpg
		rgbGen identity
		
	}
	{
		map textures/base_wall/chrome_env.jpg
		tcGen environment 
		rgbGen oneminusvertex
		tcmod scale .5 .5
	}
	{
		map textures/base_wall/patch10.jpg
		blendFunc add
		rgbGen identity
		
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/base_wall/comp3
{
	qer_editorimage textures/base_wall/comp3.jpg
	q3map_lightimage textures/base_wall/comp3env.jpg
        q3map_surfacelight 100

	{
		map textures/base_wall/comp3b.tga
		rgbGen identity
		
	}
	{
		map textures/clown/text3.tga
	        rgbGen identity
		tcmod scroll 0 0.5
	}

	{
		map textures/clown/text4.tga
		blendfunc add
	        rgbGen identity 
		tcMod scroll 0 0.5 
	}
	{
		map textures/base_wall/comp3env.jpg
		tcGen environment
	        blendfunc add
		rgbGen oneminusvertex
	}

	{
		map $lightmap
	        tcGen environment
		blendfunc gl_dst_color gl_one
	}
	{
		map textures/base_wall/comp3b.tga
        	blendFunc blend
		rgbGen identity
		
	}
	{
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}

}   

textures/base_wall/concrete_ow
{
	qer_editorimage textures/base_wall/concrete_ow.tga

	{
		map textures/sfx/proto_zzztblu3.jpg
		tcmod scale 1.5 1.5
		tcmod rotate 1
		tcmod turb 8 4 0 5
		rgbgen identity
	}
	{
		map textures/base_wall/concrete_ow.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbgen identity
	}
}

textures/tim/rconcrete_ow
{
	qer_editorimage textures/tim/rconcrete_ow.tga

	{
		map textures/sfx/fireswirl2.jpg
		tcmod scale 1.5 1.5
		tcmod rotate 1
		tcmod turb 8 4 0 5
		rgbgen identity
	}
	{
		map textures/tim/rconcrete_ow.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbgen identity
	}
}

textures/gothic_block/blocks17bloody
{
	qer_editorimage textures/gothic_block/blocks17bloody.tga
	{
		map textures/gothic_block/blocks17bloody.tga
	}
	{
		map textures/liquids/proto_gruel3.jpg
		tcmod scroll .01 .01
		tcmod turb 0 0.01 0 .1
		alphagen lightingspecular
	}
	{
		map textures/gothic_block/blocks17bloody.tga
		blendFunc blend
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc filter 
		rgbGen identity
	}
}

textures/gothic_block/largerblock3blood
{
	qer_editorimage textures/gothic_block/largerblock3blood.tga
	{
		map textures/gothic_block/largerblock3blood.tga
	}
	{
		map textures/liquids/proto_gruel3.jpg
		tcmod scroll .01 .01
		tcmod turb 0 0.01 0 .1
		alphagen lightingspecular
	}
	{
		map textures/gothic_block/largerblock3blood.tga
		blendFunc blend
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc filter 
		rgbGen identity
	}
}

textures/gothic_floor/metalbridge06brokeb
{
	qer_editorimage textures/gothic_floor/metalbridge06brokeb.tga
	{
		map textures/gothic_floor/metalbridge06brokeb.tga
		rgbgen identity
	}
	{
		map textures/sfx/proto_zzztblu3.jpg
		tcmod scale 1.5 1.5
		tcmod rotate 1
		tcmod turb 8 4 0 5
		rgbgen identity
	}
	{
		map textures/gothic_floor/metalbridge06brokeb.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbgen identity
	}
}

textures/base_wall/chrome_metal
{
	qer_editorimage textures/base_wall/chrome_metal.tga
	{
		map textures/base_wall/chrome_metal.tga
		rgbgen identity
	}
	{
		map textures/base_wall/chrome_env.tga
		tcGen environment
		tcmod scale .5 .5 
		rgbgen vertex
		
	}
	{
		map textures/base_wall/chrome_metal.tga
		blendFunc add
		tcmod scale 0.07 0.06
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/gothic_floor/largerblock3b_ow
{
	qer_editorimage textures/gothic_floor/largerblock3b_ow.tga
	{
		map textures/gothic_floor/largerblock3b_ow.tga
		rgbgen identity
	}
	{
		map textures/sfx/fireswirl2.jpg
		tcmod scale 1.5 1.5
		tcmod rotate 1
		tcmod turb 8 4 0 5
		rgbgen identity
	}
	{
		map textures/gothic_floor/largerblock3b_ow.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbgen identity
	}
}

textures/base_trim/proto_fence
{
	qer_editorimage textures/base_trim/proto_fence.tga
	surfaceparm trans
	cull none
	{
		map textures/base_trim/proto_fence.tga
		alphafunc GE128
		depthwrite
		tcmod scale 2 2
		rgbgen identity
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/skin/skin6move
{
	qer_editorimage textures/skin/skin6move.jpg
	surfaceparm flesh
	tesssize 64
	deformVertexes wave 80 sin 3 2 .5 .9
	{
		map $lightmap
		rgbgen identity
	}
	
	{
		map textures/skin/skin6move.jpg
		blendfunc filter
		rgbgen identity
	}
	
}

textures/skin/chapthroatooz
{
	qer_editorimage textures/skin/chapthroatooz.jpg
	
	{
		map textures/liquids/proto_gruel2.jpg
		
		tcmod scale .5 .5
		tcmod scroll .01 .1
		rgbgen identity
	}
	{
		map textures/skin/chapthroatooz.jpg
		blendfunc add
		rgbgen identity
	}
	
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

textures/clown/fence
{
	qer_editorimage textures/clown/fence.tga
	surfaceparm nonsolid
	surfaceparm trans
	cull none
	{
		map textures/clown/fence.tga
		alphafunc GE128
		depthwrite
		rgbgen identity
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/clown/fence_shadow
{
	qer_editorimage textures/clown/fence_shadow.tga
	polygonoffset
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm trans
	cull none
	{
		map textures/clown/fence_shadow.tga
		alphafunc GE128
		depthwrite
		rgbgen identity
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/gothic_trim/metlsupport4i_shiney
{
	qer_editorimage textures/gothic_trim/metalsupport4i.tga
	{
		map textures/gothic_trim/metalsupport4i.tga
		rgbgen identity
	}
	{
		map textures/base_wall/chrome_env.tga
		tcGen environment
		tcmod scale .5 .5 
		rgbgen vertex
		
	}
	{
		map textures/gothic_trim/metalsupport4i.tga
		blendFunc add
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/base_wall/shinybluemetal1
{
	qer_editorimage textures/base_wall/shinybluemetal1.jpg
	{
		//mapped first so it looks good while vertex lit
		map textures/base_wall/shinybluemetal1.jpg
		rgbGen Identity
	}
	{
		map textures/base_wall/chrome_env.jpg
		tcGen environment 
		rgbGen oneminusvertex
	}
	{
		map textures/base_wall/shinybluemetal1.jpg
		blendfunc add
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_src_alpha
		tcGen lightmap 
	}
}

textures/base_wall/metalfloor_wall_14
{
	qer_editorimage textures/base_wall/metalfloor_wall_14.jpg
	{
		map textures/base_wall/metalfloor_wall_14.jpg
		rgbgen identity
	}
	{
		map textures/effects/tinfx.tga
		tcgen environment
		rgbgen vertex
	}
	{
		map textures/base_wall/metalfloor_wall_14.jpg
		blendfunc add
		rgbgen identity
	}
	{
		map textures/base_wall/metalfloor_wall_14_glow.tga
		blendfunc blend
		rgbGen wave sin 0 1 0 1
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

