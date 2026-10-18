// ------------------------------------------------------------
// Shaders for Q3Rally liquids - rewritten by P3rlE
// based on the q3r_liquids.shader from quake3rally
// ------------------------------------------------------------

textures/q3r_liquids/q3r_water_01
{
	qer_editorimage textures/q3r_liquids/water.tga
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm SURF_WET
	cull disable
	fogparms ( 0.215686 0.278431 0.333333 ) 1024
	{
		map textures/q3r_liquids/water.tga
		blendfunc filter
		tcMod scroll 0.05 0.05
	}
	{
		map textures/q3r_liquids/water.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll -0.03 -0.06
	}
	{
		map textures/q3r_liquids/water.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcGen environment 
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
	}
}

textures/q3r_liquids/q3r_water_03
{
	qer_editorimage textures/q3r_liquids/water.tga
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm SURF_WET
	cull disable
	deformVertexes wave 512 sin 0 7 0 0.4
	fogparms ( 0.215686 0.278431 0.333333 ) 1024
	// OpenGL2: rippled real-time reflection (pool of q3r_ctf_01); the
	// stages below are the OpenGL1 / r_water 0 look
	q3r_water textures/q3r_liquids/water_normal.tga 320 0.03 0.03
	q3r_waterColor 0.08 0.16 0.20 0.55
	q3r_waterFresnel 0.12 4
	q3r_waterSpecular 1.2
	{
		map textures/q3r_liquids/water_11.tga
		blendfunc filter
		tcMod scroll 0.01 0.01
		alphaGen wave inversesawtooth 6 0 0 7
	}
	{
		map textures/q3r_liquids/water_12.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll -0.01 -0.03
		alphaGen wave inversesawtooth 1 0 0 2
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
	}
}

textures/q3r_liquids/q3r_water_pool
{
	// Q3Rally: the calm green pool water of liquids/calm_poollight with the
	// OpenGL2 reflection (q3r_sk1); the stages are the OpenGL1 / r_water 0 look
	qer_editorimage textures/liquids/pool3d_4b.tga
	q3map_globaltexture
	qer_trans .75

	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm SURF_WET
	surfaceparm nolightmap
	q3map_surfacelight 50

	cull disable
	tesssize 64
	deformVertexes wave 100 sin 1 1 1 .1

	q3r_water textures/q3r_liquids/water_normal.tga 256 0.03 0.04
	q3r_waterColor 0.05 0.13 0.11 0.6
	q3r_waterFresnel 0.1 4
	q3r_waterSpecular 0.5

	{ 
		map textures/liquids/pool3d_5c2.tga
		blendFunc GL_dst_color GL_zero
		rgbgen identity
		tcmod scale .5 .5
		tcmod transform 1.5 0 1.5 1 1 2
		tcmod scroll -.05 .001
	}
	
	{ 
		map textures/liquids/pool3d_6c2.tga
		blendFunc GL_dst_color GL_zero
		rgbgen identity
		tcmod scale .5 .5
		tcmod transform 0 1.5 1 1.5 2 1
		tcmod scroll .025 -.001
	}

	{ 
		map textures/liquids/pool3d_3c2.tga
		blendFunc GL_dst_color GL_zero
		rgbgen identity
		tcmod scale .25 .5
		tcmod scroll .001 .025
	}
	{
		map textures/liquids/pool3d_4b2.tga
		blendfunc add
		tcmod scale .125 .125	
		rgbgen identity
	}

	//	END
}

textures/q3r_liquids/q3r_lavahell
{
	qer_editorimage textures/q3r_liquids/q3r_lavahell.tga
	q3map_globaltexture
	surfaceparm lava
	surfaceparm nodlight
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_surfacelight 600
	cull disable
	
	tesssize 128
	cull disable
	deformVertexes wave 100 sin 3 2 .1 0.1
	
	{
		map textures/q3r_liquids/q3r_lavahell.tga
		tcMod turb 0 .2 0 .1
		tcMod scroll 0.0 -0.5
	}
	
}

textures/liquids/flatripplewater_1500
{
	qer_editorimage textures/liquids/pool2.tga
	q3map_globaltexture
	q3map_lightsubdivide 32
	//surfaceparm trans
	//surfaceparm nonsolid
	surfaceparm noimpact
	surfaceparm lava
	surfaceparm nolightmap
	q3map_surfacelight 500
	cull disable
	
	deformVertexes wave 100 sin 3 2 .1 0.1
	
	{
		map textures/liquids/pool2.tga
		tcMod turb 0 .2 0 .1
	}
}

textures/liquids/clear_ripple2

//	*********************************************************
//	*      Small clear ripple water. Created September 9	*
//	*********************************************************
	
	{
		qer_editorimage textures/liquids/pool3d_3b.tga
		qer_trans .5
		q3map_globaltexture
		surfaceparm trans
		surfaceparm nonsolid
		surfaceparm SURF_WET

		cull disable
		deformVertexes wave 64 sin .5 .5 0 .5	

		{ 
			map textures/liquids/pool3d_5b.tga
			blendFunc GL_dst_color GL_one
			rgbgen identity
			tcmod scale .5 .5
			tcmod transform 1.5 0 1.5 1 1 2
			tcmod scroll -.05 .001
		}
	
		{ 
			map textures/liquids/pool3d_6b.tga
			blendFunc GL_dst_color GL_one
			rgbgen identity
			tcmod scale .5 .5
			tcmod transform 0 1.5 1 1.5 2 1
			tcmod scroll .025 -.001
		}

		{ 
			map textures/liquids/pool3d_3b.tga
			blendFunc GL_dst_color GL_one
			rgbgen identity
			tcmod scale .25 .5
			tcmod scroll .001 .025
		}

		{
			map $lightmap
			blendFunc GL_dst_color GL_zero
			rgbgen identity		
		}

//	END
	}

textures/liquids/calm_poollight
	{
		//	*************************************************
		//	* CALM GREEN WATER POOL may 5 1999
		//    * same as calm water but emits light		*
		//	* IF YOU CHANGE THIS PLEASE COMMENT THE CHANGE	*
		//	*************************************************

		qer_editorimage textures/liquids/pool3d_4b.tga
		q3map_globaltexture
		qer_trans .75

		surfaceparm trans
		surfaceparm nonsolid
		surfaceparm SURF_WET
		surfaceparm nolightmap
		q3map_surfacelight 50

		cull disable
		tesssize 64
		deformVertexes wave 100 sin 1 1 1 .1

		{ 
			map textures/liquids/pool3d_5c2.tga
			blendFunc GL_dst_color GL_zero
			rgbgen identity
			tcmod scale .5 .5
			tcmod transform 1.5 0 1.5 1 1 2
			tcmod scroll -.05 .001
		}
	
		{ 
			map textures/liquids/pool3d_6c2.tga
			blendFunc GL_dst_color GL_zero
			rgbgen identity
			tcmod scale .5 .5
			tcmod transform 0 1.5 1 1.5 2 1
			tcmod scroll .025 -.001
		}

		{ 
			map textures/liquids/pool3d_3c2.tga
			blendFunc GL_dst_color GL_zero
			rgbgen identity
			tcmod scale .25 .5
			tcmod scroll .001 .025
		}
		{
			map textures/liquids/pool3d_4b2.tga
			blendfunc add
			tcmod scale .125 .125	
			rgbgen identity
		}

		//	END
	}

textures/liquids/calm_pool_bgtess
	{
		//	*************************************************
		//	* CALM GREEN WATER POOL Feb 19, 1998		*
		//	* IF YOU CHANGE THIS PLEASE COMMENT THE CHANGE	*
		//      * Large tesselations added on May 26, '99
		//	*************************************************

		// Added to g3map_global texture on May 11, 1999
		qer_editorimage textures/liquids/pool2.tga
		qer_trans .5
		q3map_globaltexture

		surfaceparm trans
		surfaceparm nonsolid
		surfaceparm SURF_WET
		surfaceparm nolightmap

		cull disable
		tesssize 256
		deformVertexes wave 256 sin 1 1 1 .1

		{
			map textures/liquids/pool2.tga
			blendfunc GL_ONE GL_SRC_COLOR	
			tcMod scale .03 .03
			tcMod scroll .001 .001
		}

		//	END
	}

textures/q3r_liquids/lavabattlecanyon
{
qer_editorimage textures/q3r_liquids/protolava.tga
	q3map_globaltexture
	surfaceparm lava
	surfaceparm nodlight
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_surfacelight 600
	cull disable
deformVertexes wave 512 sin 0 7 0 0.4
    { 
        map textures/q3r_liquids/protolava.tga
        rgbgen identity
        tcmod scroll -.05 .001
    }

      fogparms ( .76 .73 .54 ) 30000
 
}

// ------------------------------------------------------------
// Mirror water (test)
// OpenGL2 (r_water 1): q3r_water draws GLSL water with a rippled real-time
// reflection, see-through from above and mirror-like at grazing angles. The
// mirror view is built from the water plane itself.
// OpenGL1 / r_water 0: the stages below are drawn instead, a plain mirror
// through "portal"; that needs a misc_portal_surface entity without target
// within 64 units of the water surface.
// Only one reflection is drawn per view, so use it for one water plane
// per area.
//
// q3r_water <normalmap> [tile size] [speed] [distortion]
// q3r_waterColor <r> <g> <b> [opacity looking straight down]
// q3r_waterFresnel <bias> <power>
// q3r_waterSpecular <sun glint strength>
// ------------------------------------------------------------

textures/q3r_liquids/q3r_water_mirror
{
	qer_editorimage textures/q3r_liquids/water.tga
	qer_trans 0.6
	portal
	q3r_water textures/q3r_liquids/water_normal.tga 320 0.03 0.03
	q3r_waterColor 0.02 0.07 0.08 0.7
	q3r_waterFresnel 0.12 4
	q3r_waterSpecular 1.5
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm water
	surfaceparm nolightmap
	surfaceparm SURF_WET
	{
		// distance fade: reflection fully visible nearby, dark water at the
		// portal range (the mirror view is not drawn beyond it)
		map $whiteimage
		rgbGen const ( 0.04 0.10 0.13 )
		alphaGen portal 3072
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
	}
	{
		// constant water tint over the reflection
		map $whiteimage
		rgbGen const ( 0.04 0.10 0.13 )
		alphaGen const 0.25
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		// moving highlights
		map textures/q3r_liquids/water.tga
		blendFunc GL_ONE GL_ONE
		rgbGen const ( 0.05 0.06 0.07 )
		tcMod scale 0.5 0.5
		tcMod scroll 0.02 0.01
	}
}

// ------------------------------------------------------------
// Flowing and falling water (Q3Rally)
// The streaks run along the texture's t axis and move towards +t, i.e.
// down the texture. Align the texture so that its top points upstream:
// on a wall that is the default (top = up); on a floor, rotate it so the
// top points against the flow. Texture scale 0.5 = one repeat per
// 64 x 128 units. No lightmap: the water keeps its brightness in shade.
// Example: the pipe and the waterfall into the pool of q3r_ctf_01.
// ------------------------------------------------------------

textures/q3r_liquids/q3r_waterfall
{
	qer_editorimage textures/q3r_liquids/waterfall_body.jpg
	qer_trans 0.7
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nolightmap
	cull disable
	{
		map textures/q3r_liquids/waterfall_body.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
		alphaGen const 0.72
		tcMod turb 0 0.012 0 0.6
		tcMod scroll 0 -1.1
	}
	{
		map textures/q3r_liquids/waterfall_streaks.tga
		blendFunc GL_SRC_ALPHA GL_ONE
		rgbGen identity
		tcMod scroll 0 -2.4
	}
	{
		map textures/q3r_liquids/waterfall_streaks.tga
		blendFunc GL_SRC_ALPHA GL_ONE
		rgbGen const ( 0.6 0.7 0.8 )
		tcMod scale 1.3 0.8
		tcMod scroll 0.06 -1.7
	}
}

textures/q3r_liquids/q3r_waterflow
{
	qer_editorimage textures/q3r_liquids/waterfall_body.jpg
	qer_trans 0.7
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nolightmap
	cull disable
	{
		map textures/q3r_liquids/waterfall_body.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
		alphaGen const 0.78
		tcMod turb 0 0.02 0 0.3
		tcMod scroll 0 -0.45
	}
	{
		map textures/q3r_liquids/waterfall_streaks.tga
		blendFunc GL_SRC_ALPHA GL_ONE
		rgbGen const ( 0.55 0.65 0.75 )
		tcMod scale 1 0.6
		tcMod scroll 0 -0.8
	}
	{
		map textures/q3r_liquids/water.tga
		blendFunc GL_ONE GL_ONE
		rgbGen const ( 0.2 0.2 0.2 )
		tcGen environment
	}
}
