textures/sfx2/boostflame
{
	qer_editorimage textures/sfx/flame5.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_surfacelight 5500
	{
		animmap 15 textures/sfx/flame1.tga textures/sfx/flame3.tga textures/sfx/flame5.tga textures/sfx/flame7.tga 
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 10 
	}
	{
		animmap 15 textures/sfx/flame2.tga textures/sfx/flame4.tga textures/sfx/flame6.tga textures/sfx/flame8.tga 
		blendfunc add
		rgbGen wave sawtooth 0 1 0 10 
	}
}

//silly jet flame
//took out the light, was kinda sickening
textures/sfx/xflame3
{
	qer_editorimage textures/clown/flame8.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	deformVertexes autosprite2
	tessSize 64
	
	{
		animmap 10 textures/clown/flame1.tga textures/clown/flame2.tga textures/clown/flame3.tga textures/clown/flame4.tga textures/clown/flame5.tga textures/clown/flame6.tga textures/clown/flame7.tga textures/clown/flame8.tga
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 3 5 2 10
		
	}
	{
		animmap 10 textures/clown/flame2.tga textures/clown/flame3.tga textures/clown/flame4.tga textures/clown/flame5.tga textures/clown/flame6.tga textures/clown/flame7.tga textures/clown/flame8.tga textures/clown/flame1.tga
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 3 5 2 10 
		
	}
}

textures/sfx/xflame4
{
	q3map_lightimage textures/clown/fleshy.tga
	qer_editorimage textures/sfx/flame7.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_surfacelight 400
	deformVertexes autosprite2
	{
		animmap 10 textures/sfx/flame1.tga textures/sfx/flame3.tga textures/sfx/flame5.tga textures/sfx/flame7.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/flame2.tga textures/sfx/flame4.tga textures/sfx/flame6.tga textures/sfx/flame8.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
}

textures/sfx/beam
{
        surfaceparm trans	
        surfaceparm nomarks	
        surfaceparm nonsolid
	surfaceparm nolightmap
	cull none
	
	{
		map textures/sfx/beam.jpg
                tcMod Scroll .3 0
                blendFunc GL_ONE GL_ONE
        }
}

textures/sfx/beam_dusty2
{
	qer_editorimage textures/sfx/beam_1.jpg
        surfaceparm trans	
        surfaceparm nomarks	
        surfaceparm nonsolid
	surfaceparm nolightmap
	cull none
	
	{
		map textures/sfx/beam.jpg
		tcMod Scroll .3 0
		blendFunc GL_ONE GL_ONE
	}
	{
		map textures/sfx/beam.jpg
		tcMod Scroll -10 0
		blendFunc GL_ONE GL_ONE
	}
}

textures/sfx/beam_blue
{
	qer_editorimage textures/sfx/beam_blue4.jpg
	surfaceparm trans	
        surfaceparm nomarks	
        surfaceparm nonsolid
	surfaceparm nolightmap
	cull none
	
	{
		map textures/sfx/beam_blue4.jpg
                tcMod Scroll .3 0
                blendFunc GL_ONE GL_ONE
        }
}

textures/sfx/beam_red
{
	qer_editorimage textures/sfx/beam_red.jpg
	surfaceparm trans	
        surfaceparm nomarks	
        surfaceparm nonsolid
	surfaceparm nolightmap
	cull none
	
	{
		map textures/sfx/beam_red.jpg
                tcMod Scroll .3 0
                blendFunc GL_ONE GL_ONE
        }
}

textures/sfx/beam_waterlight2

{
	qer_editorimage textures/sfx/beam_waterlight.jpg
	surfaceparm trans	
        surfaceparm nomarks	
        surfaceparm nonsolid
	surfaceparm nolightmap
	cull none
	
	{
		map textures/sfx/beam_waterlight.jpg
                tcMod Scroll .3 0
                blendFunc GL_ONE GL_ONE
        }
	
}

textures/sfx/lavabeam

{
	qer_editorimage textures/sfx/lavabeam.jpg
	surfaceparm trans	
        surfaceparm nomarks	
        surfaceparm nonsolid
	surfaceparm nolightmap
	cull none
	
	{
		map textures/sfx/lavabeam.jpg
                tcMod Scroll 5 0
                blendFunc GL_ONE GL_ONE
        }
	
}

textures/sfx/portal_sfx
{
	deformVertexes wave 25 sin 3 2 .5 3
	portal
	surfaceparm nolightmap
	
	{
		map textures/acc_dm3/portal_sfx.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
	}
	{
		map textures/oa_fogs/kc_fogcloud3.tga
		blendfunc gl_src_alpha gl_one_minus_src_alpha
		alphagen portal 1024
		rgbGen identity	
		tcmod rotate .1 
		tcmod scroll .04 .01
	}
	{
		map textures/acc_dm3/fx_tintedportal.jpg
		rgbgen identity
		blendFunc filter
	}
}

textures/sfx/teslacoil
{
	qer_editorimage textures/sfx/tesla1.jpg
	q3map_lightimage textures/sfx/tesla1.jpg
	q3map_surfacelight 25
	cull none
	{
		map $lightmap
		tcgen environment
		blendfunc filter
	}
	{
		map textures/sfx/tesla1.jpg
		blendfunc add
		rgbgen wave sawtooth 0 1 0 5
		tcmod scale 4 .5
		tcmod turb 0 .2 0 1
		tcmod scroll -1 1
	}
	{
		map textures/sfx/tesla1b.jpg
		blendfunc add
		rgbgen wave square 0.5 0.5 0.5 2.5
		tcmod rotate 90
		tcmod scale 4 .5
		tcmod scroll -2 2
	}
	{
		map textures/sfx/electricslime.jpg
		blendfunc add
		rgbgen wave sawtooth .25 1 0 5
		tcmod scale 4 .5
		tcmod turb 0 .2 0 1
		tcmod scroll 2 -2
	}
	{
		map textures/clown/cable.tga
		blendfunc blend
	}
}

textures/sfx/teslacoil3
{
	qer_editorimage textures/sfx/tesla1b.jpg
	q3map_lightimage textures/sfx/electricslime.jpg
	q3map_surfacelight 25
	cull none
	{
		map $lightmap
		tcgen environment
		blendfunc filter
	}
	{
		map textures/sfx/tesla1.jpg
		blendfunc add
		rgbgen wave sawtooth 0 1 0 5
		tcmod scale 1 2
		tcmod turb 0 .2 0 1
		tcmod scroll 2 -2
	}
	{
		map textures/sfx/electricslime.jpg
		blendfunc add
		rgbgen wave sawtooth .25 1 0 5
		tcmod scale 1 2
		tcmod turb 0 .2 0 1
		tcmod scroll 2 -2
	}
	{
		map textures/clown/cable.tga
		blendfunc blend
	}
}

textures/sfx/proto_zzztblu3
{
	surfaceparm nolightmap
	surfaceparm nonsolid
	cull none

	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE
		depthFunc equal
	}

	{
		map textures/sfx/proto_zzztblu3.tga
		tcgen environment
		blendFunc GL_ONE GL_ONE
		tcMod turb 3 .3 0 .3
		tcmod scroll 2 2
		tcmod scale .2 .2
	}
}

textures/sfx/xian_dm3padwall
{
	qer_editorimage textures/sfx/xian_dm3padwall.jpg
	q3map_lightimage textures/sfx/xian_dm3padwallglow.jpg
	surfaceparm nomarks
	surfaceparm metalsteps
	q3map_surfacelight 25
		
	{
		map textures/sfx/xian_dm3padwall.jpg
		rgbgen identity
		
	}
	{
		map $lightmap 
		rgbgen identity 
		blendfunc filter
		
	}
	{
		map textures/sfx/xian_dm3padwallglow.jpg
		tcmod scroll 0 1
		rgbGen wave sin 0 2 .5 2
		blendfunc add

	}


}

textures/sfx/bugmirror
{
	qer_editorimage textures/sfx/mirror.tga
	surfaceparm nolightmap
	portal
	{
		map textures/common/invisible.tga
		blendfunc gl_one gl_one_minus_src_alpha
		depthWrite
	}
	{
		map textures/sfx/mirrorkc.tga
		blendfunc gl_src_alpha gl_one_minus_src_alpha
		alphagen portal 1024
		rgbgen identity
	}
}

textures/sfx/flame1
{
	qer_editorimage textures/sfx/flame1.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_surfacelight 2000
	q3map_flare flareShader-lava
	{
		animmap 10 textures/sfx/flame1.tga textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga textures/sfx/flame1.tga
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
}

textures/sfx/flame1dark
{
	qer_editorimage textures/sfx/flame3.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_surfacelight 100
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/flame1.tga textures/sfx/flame3.tga textures/sfx/flame5.tga textures/sfx/flame7.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/flame2.tga textures/sfx/flame4.tga textures/sfx/flame6.tga textures/sfx/flame8.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
}

textures/sfx/flame1km
{
	qer_editorimage textures/sfx/flame1.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_surfacelight 6000
	q3map_flare flareShader-lava
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/flame1.tga textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga textures/sfx/flame1.tga
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
}

textures/sfx/flame1side
{
	qer_editorimage textures/sfx/flame6.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/flame1.tga textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga textures/sfx/flame1.tga
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
}

textures/sfx/mkc_bigflame
{
	qer_editorimage textures/sfx/flame2.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_lightimage textures/sfx/flame1.tga
	q3map_surfacelight 500
	q3map_flare flareShader-lava
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/flame1.tga textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga textures/sfx/flame1.tga
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
}

textures/sfx/xflame2_2250
{
	qer_editorimage textures/sfx/flame2.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_lightimage textures/sfx/flame1.tga
	q3map_surfacelight 2250
	q3map_flare flareShader-lava
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/flame1.tga textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga textures/sfx/flame1.tga
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
}

textures/sfx/xflame2_1800
{
	qer_editorimage textures/sfx/flame5.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_lightimage textures/sfx/flame1.tga
	q3map_surfacelight 1800
	q3map_flare textures/sfx/flameflare
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/flame1.tga textures/sfx/flame3.tga textures/sfx/flame5.tga textures/sfx/flame7.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/flame2.tga textures/sfx/flame4.tga textures/sfx/flame6.tga textures/sfx/flame8.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
}

textures/sfx/xflame2_nolight
{
	qer_editorimage textures/sfx/flame5.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/flame1.tga textures/sfx/flame3.tga textures/sfx/flame5.tga textures/sfx/flame7.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/flame2.tga textures/sfx/flame4.tga textures/sfx/flame6.tga textures/sfx/flame8.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
}

textures/sfx/flameanim_dimmer
{
	qer_editorimage textures/sfx/flame1.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_surfacelight 1000
	q3map_flare flareShader-lava
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/flame1.tga textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga textures/sfx/flame1.tga
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
}

textures/sfx/flameanim_blue
{
	qer_editorimage textures/sfx/b_flame1.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_surfacelight 2000
	q3map_flare flareShader-lava
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/b_flame1.tga textures/sfx/b_flame2.tga textures/sfx/b_flame3.tga textures/sfx/b_flame4.tga textures/sfx/b_flame5.tga textures/sfx/b_flame6.tga textures/sfx/b_flame7.tga textures/sfx/b_flame8.tga 
		blendfunc gl_src_alpha gl_one
		rgbGen wave sawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/b_flame2.tga textures/sfx/b_flame3.tga textures/sfx/b_flame4.tga textures/sfx/b_flame5.tga textures/sfx/b_flame6.tga textures/sfx/b_flame7.tga textures/sfx/b_flame8.tga textures/sfx/b_flame1.tga
		blendfunc gl_src_alpha gl_one
		rgbGen wave inversesawtooth 0 1 0 10 
	}
	{
		map textures/sfx/b_flameball.tga
		blendfunc add
		rgbgen wave sin .4 .1 .5 2
	}
}

textures/sfx/flameanim_blue_nolight
{
	qer_editorimage textures/sfx/b_flame2.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/b_flame1.tga textures/sfx/b_flame2.tga textures/sfx/b_flame3.tga textures/sfx/b_flame4.tga textures/sfx/b_flame5.tga textures/sfx/b_flame6.tga textures/sfx/b_flame7.tga textures/sfx/b_flame8.tga 
		blendfunc gl_src_alpha gl_one
		rgbGen wave sawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/b_flame2.tga textures/sfx/b_flame3.tga textures/sfx/b_flame4.tga textures/sfx/b_flame5.tga textures/sfx/b_flame6.tga textures/sfx/b_flame7.tga textures/sfx/b_flame8.tga textures/sfx/b_flame1.tga
		blendfunc gl_src_alpha gl_one
		rgbGen wave inversesawtooth 0 1 0 10 
	}
	{
		map textures/sfx/b_flameball.tga
		blendfunc add
		rgbgen wave sin .4 .1 .5 2
	}
}

textures/sfx/flameanim_blue_pj
{
	qer_editorimage textures/sfx/b_flame2.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_surfacelight 2000
	q3map_flare flareShader-lava
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/b_flame1.tga textures/sfx/b_flame3.tga textures/sfx/b_flame5.tga textures/sfx/b_flame7.tga 
		blendfunc gl_src_alpha gl_one
		rgbGen wave sawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/b_flame2.tga textures/sfx/b_flame4.tga textures/sfx/b_flame6.tga textures/sfx/b_flame8.tga textures/sfx/b_flame1.tga
		blendfunc gl_src_alpha gl_one
		rgbGen wave inversesawtooth 0 1 0 10 
	}
}

textures/sfx/flameanim_green_pj
{
	qer_editorimage textures/sfx/g_flame1.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_surfacelight 2000
	q3map_flare flareShader-lava
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/g_flame1.tga textures/sfx/g_flame3.tga textures/sfx/g_flame5.tga textures/sfx/g_flame7.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/g_flame2.tga textures/sfx/g_flame4.tga textures/sfx/g_flame6.tga textures/sfx/g_flame8.tga textures/sfx/b_flame1.tga
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
	{
		map textures/sfx/g_flameball.tga
		blendfunc add
		rgbgen wave sin .4 .1 .5 2
	}
}

textures/sfx/flameanim_red
{
	qer_editorimage textures/sfx/r_flame1.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_surfacelight 2000
	q3map_flare flareShader-lava
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/r_flame1.tga textures/sfx/r_flame3.tga textures/sfx/r_flame5.tga textures/sfx/r_flame7.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/r_flame2.tga textures/sfx/r_flame4.tga textures/sfx/r_flame6.tga textures/sfx/r_flame8.tga textures/sfx/r_flame1.tga
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
}

textures/sfx/flameanim_red_nolight
{
	qer_editorimage textures/sfx/r_flame1.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/r_flame1.tga textures/sfx/r_flame3.tga textures/sfx/r_flame5.tga textures/sfx/r_flame7.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/r_flame2.tga textures/sfx/r_flame4.tga textures/sfx/r_flame6.tga textures/sfx/r_flame8.tga textures/sfx/r_flame1.tga
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
}

textures/sfx/flameanim_red_pj
{
	qer_editorimage textures/sfx/r_flame2.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_surfacelight 1500
	q3map_flare flareShader-lava
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/r_flame1.tga textures/sfx/r_flame3.tga textures/sfx/r_flame5.tga textures/sfx/r_flame7.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/r_flame2.tga textures/sfx/r_flame4.tga textures/sfx/r_flame6.tga textures/sfx/r_flame8.tga textures/sfx/r_flame1.tga
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
	{
		map textures/sfx/r_flameball.tga
		blendfunc add
		rgbgen wave sin .4 .1 .5 2
	}
}

textures/sfx/clangdark_bounce
{
	qer_editorimage textures/sfx/clangdark_bounce.jpg
	q3map_lightimage textures/clown/circ4glow.tga
	q3map_surfacelight 100
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/sfx/clangdark_bounce.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/clown/circ4glow.tga
		blendFunc add
		rgbGen wave sin 0.2 0.2 0 .9
	}
}

textures/sfx/bounce_concrete
{
	qer_editorimage textures/sfx/bounce_concrete.jpg
	q3map_lightimage textures/clown/circ4glow.tga
	q3map_surfacelight 100
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/sfx/bounce_concrete.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/clown/circ4glow.tga
		blendFunc add
		rgbGen wave sin 0.2 0.2 0 .9
	}
}

textures/sfx/bouncepad01_block17
{
	qer_editorimage textures/sfx/bouncepad01_block17.jpg
	q3map_lightimage textures/clown/circ4glow.tga
	q3map_surfacelight 100
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/sfx/bouncepad01_block17.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/clown/circ4glow.tga
		blendFunc add
		rgbGen wave sin 0.2 0.2 0 .9
	}
}

textures/sfx/bouncepad01block18b
{
	qer_editorimage textures/sfx/bouncepad01_block18b.jpg
	q3map_lightimage textures/clown/circ4glow.tga
	q3map_surfacelight 100
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/sfx/bouncepad01_block18b.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/clown/circ4glow.tga
		blendFunc add
		rgbGen wave sin 0.2 0.2 0 .9
	}
}

textures/sfx/bouncepad1q1metal7_99
{
	qer_editorimage textures/sfx/bouncepad1q1metal7_99.jpg
	q3map_lightimage textures/clown/circ4glow.tga
	q3map_surfacelight 100
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/sfx/bouncepad1q1metal7_99.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/clown/circ4glow.tga
		blendFunc add
		rgbGen wave sin 0.2 0.2 0 .9
	}
}

textures/sfx/bounce_largeblock3b
{
	qer_editorimage textures/sfx/bounce_largeblock3b.jpg
	q3map_lightimage textures/clown/circ4glow.tga
	q3map_surfacelight 100
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/sfx/bounce_largeblock3b.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/clown/circ4glow.tga
		blendFunc add
		rgbGen wave sin 0.2 0.2 0 .9
	}
}

textures/sfx/metalbridge06_bounce
{
	qer_editorimage textures/sfx/metalbridge06_bounce.jpg
	q3map_lightimage textures/clown/circ4glow.tga
	q3map_surfacelight 466
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/sfx/metalbridge06_bounce.jpg
		blendfunc filter
		rgbGen identity
	}
	{
		map textures/clown/circ4glow.tga
		blendFunc add
		rgbGen wave sin 0.2 0.2 0 .9
	}
}

textures/sfx/diamond2cjumppad
{
	qer_editorimage textures/sfx/bouncepad01_diamond2cTGA.jpg
	q3map_lightimage textures/clown/circ4glow.tga
	q3map_surfacelight 100
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/sfx/bouncepad01_diamond2cTGA.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/clown/circ4glow.tga
		blendFunc add
		rgbGen wave sin 0.2 0.2 0 .9
	}
}

textures/sfx/flame2
{
	qer_editorimage textures/sfx/flame5.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_surfacelight 3787
	q3map_flare textures/sfx/flameflare
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 8 textures/sfx/flame1.tga textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga 
		blendfunc gl_src_alpha gl_one
		rgbGen wave inversesawtooth 0 1 0 8 
	}
	{
		animmap 8 textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga textures/sfx/flame1.tga 
		blendfunc gl_src_alpha gl_one
		rgbGen wave sawtooth 0 1 0 8 
	}
}

textures/sfx/xflame2
{
	qer_editorimage textures/sfx/flame5.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	cull disable
	q3map_surfacelight 3787
	q3map_flare textures/sfx/flameflare
	deformVertexes wave 194 sin 0 1 0 .6
	{
		animmap 10 textures/sfx/flame1.tga textures/sfx/flame3.tga textures/sfx/flame5.tga textures/sfx/flame7.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 10 
	}
	{
		animmap 10 textures/sfx/flame2.tga textures/sfx/flame4.tga textures/sfx/flame6.tga textures/sfx/flame8.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 10 
	}
}

textures/sfx/fanofdeath
{
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm trans
	{
		map textures/sfx/fanofdeath.tga
		blendfunc blend
		tcMod rotate -888
	}
}

textures/sfx/forcefield
{
	q3map_lightimage textures/sfx/forcefieldd.tga
	surfaceparm nodlight
	surfaceparm nomarks
	surfaceparm trans
	q3map_surfacelight 222
	{
		map textures/sfx/forcefieldd.tga
		blendfunc add
		tcMod scroll 0.3 0.3
		tcGen vector ( 2 0 0 ) ( 0 2 0 ) 
	}
	{
		map textures/sfx/forcefieldd.tga
		blendfunc add
		tcMod scroll 0.32 0.35
	}
}

textures/sfx/rain
{
	qer_editorimage textures/clown/rain.tga
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm nolightmap
	deformvertexes  wave 25 sin 3 2 .1 3

	
	cull none
	{
		map textures/clown/rain.tga
		tcmod scroll .3 -6
		tcmod turb .08 .3 .5 -.3
		blendfunc add
	}
	{
		map textures/clown/rain.tga
		tcmod scroll .04 -7
		blendfunc add
	}

}

textures/sfx/xmetalfloor_wall_14b
{
	qer_editorimage textures/sfx/metalfloor_wall_14b.tga
	
	{
		map textures/sfx/metalfloor_wall_14b.tga
		rgbgen identity
	}
	{
		map textures/sfx/mfw14b_glow.jpg
		tcmod scale .5 .5
		tcmod scroll 0 1
		rgbgen identity
	}
	{
		map textures/sfx/metalfloor_wall_14b.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

textures/sfx/xmetalfloor_wall_5b
{
	qer_editorimage textures/sfx/metalfloor_wall_7b.tga
	
	{
		map textures/sfx/metalfloor_wall_7b.tga
		rgbgen identity
	}
	{
		map textures/sfx/mfw7b_glow.jpg
		tcmod scale .5 .5
		tcmod scroll 0 1
		rgbgen identity
	}
	{
		map textures/sfx/metalfloor_wall_7b.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

textures/sfx/xmetalfloor_wall_9b
{
	qer_editorimage textures/sfx/metalfloor_wall_9b.tga
	
	{
		map textures/sfx/metalfloor_wall_9b.tga
		rgbgen identity
	}
	{
		map textures/sfx/mfw9b_glow.jpg
		tcmod scale .5 .5
		tcmod scroll 0 1
		rgbgen identity
	}
	{
		map textures/sfx/metalfloor_wall_9b.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

textures/sfx/fan_grate
{
	surfaceparm metalsteps
	surfaceparm nomarks
	cull none
	{
		map textures/sfx/fan_grate.tga
		blendfunc blend
		rgbgen identity
		alphafunc GE128
		depthwrite
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
		depthfunc equal
	}
}

textures/sfx/launchpad_blocks17
{
	qer_editorimage textures/sfx/launchpad03.tga
	surfaceparm nomarks
	{
		map textures/sfx/tread.jpg
		tcmod scroll 0 1
		rgbgen identity
	}
	{
		map textures/sfx/nlaunch.jpg
		blendfunc add
		tcmod scroll 0 4
		rgbgen wave square 0 .7 0 1.4
	}
	{
		map textures/sfx/launchpad03.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/sfx/nlaunch2.jpg
		blendfunc add
		rgbgen wave square 0 .7 0 1.4
	}
}

textures/sfx/launchpad_blocks18d
{
	qer_editorimage textures/sfx/launchpad03.tga
	surfaceparm nomarks
	{
		map textures/sfx/tread.jpg
		tcmod scroll 0 1
		rgbgen identity
	}
	{
		map textures/sfx/nlaunch.jpg
		blendfunc add
		tcmod scroll 0 4
		rgbgen wave square 0 .7 0 1.4
	}
	{
		map textures/sfx/launchpad03.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/sfx/nlaunch2.jpg
		blendfunc add
		rgbgen wave square 0 .7 0 1.4
	}
}

textures/sfx/launchpad_diamond
{
	qer_editorimage textures/sfx/launchpad03.tga
	surfaceparm nomarks
	{
		map textures/sfx/tread.jpg
		tcmod scroll 0 1
		rgbgen identity
	}
	{
		map textures/sfx/nlaunch.jpg
		blendfunc add
		tcmod scroll 0 4
		rgbgen wave square 0 .7 0 1.4
	}
	{
		map textures/sfx/launchpad03.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/sfx/nlaunch2.jpg
		blendfunc add
		rgbgen wave square 0 .7 0 1.4
	}

}

textures/sfx/launchpad_metalbridge04d
{
	qer_editorimage textures/sfx/launchpad03.tga
	surfaceparm nomarks
	{
		map textures/sfx/tread.jpg
		tcmod scroll 0 1
		rgbgen identity
	}
	{
		map textures/sfx/nlaunch.jpg
		blendfunc add
		tcmod scroll 0 4
		rgbgen wave square 0 .7 0 1.4
	}
	{
		map textures/sfx/launchpad03.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/sfx/nlaunch2.jpg
		blendfunc add
		rgbgen wave square 0 .7 0 1.4
	}
}

textures/sfx2/ntrl_launchpad
{
	qer_editorimage textures/sfx/launchpad03.tga
	surfaceparm nomarks
	{
		map textures/sfx/tread.jpg
		tcmod scroll 0 1
		rgbgen identity
	}
	{
		map textures/sfx/nlaunch.jpg
		blendfunc add
		tcmod scroll 0 4
		rgbgen wave square 0 .7 0 1.4
	}
	{
		map textures/sfx/launchpad03.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/sfx/nlaunch2.jpg
		blendfunc add
		rgbgen wave square 0 .7 0 1.4
	}
}

textures/sfx2/blue_launchpad
{
	qer_editorimage textures/sfx/launchpad03.tga
	surfaceparm nomarks
	{
		map textures/sfx/tread.jpg
		tcmod scroll 0 1
		rgbgen identity
	}
	{
		map textures/sfx/blaunch.jpg
		blendfunc add
		tcmod scroll 0 4
		rgbgen wave square 0 .7 0 1.4
	}
	{
		map textures/sfx/launchpad03.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/sfx/blaunch2.jpg
		blendfunc add
		rgbgen wave square 0 .7 0 1.4
	}
}

textures/sfx2/red_launchpad
{
	qer_editorimage textures/sfx/launchpad03.tga
	surfaceparm nomarks
	{
		map textures/sfx/tread.jpg
		tcmod scroll 0 1
		rgbgen identity
	}
	{
		map textures/sfx/rlaunch.jpg
		blendfunc add
		tcmod scroll 0 4
		rgbgen wave square 0 .7 0 1.4
	}
	{
		map textures/sfx/launchpad03.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/sfx/rlaunch2.jpg
		blendfunc add
		rgbgen wave square 0 .7 0 1.4
	}
}

textures/sfx2/blue_jumpad
{
	qer_editorimage textures/sfx2/jump_pad02.tga
	q3map_lightimage textures/sfx2/b_blur.tga
	surfaceparm nodamage
	surfaceparm nomarks
	q3map_surfacelight 30
	{
		map textures/sfx2/b_blur.tga
		tcmod rotate 300
		rgbgen identity	
	}
	{
		clampmap textures/sfx/jcb2.tga
		blendfunc add
		tcmod rotate 100
		tcmod stretch sin 1.2 .8 0 1.4
		rgbgen wave square .5 .5 .25 1.4
	}
	{
		map textures/sfx2/jump_pad02.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/sfx2/red_jumpad
{
	qer_editorimage textures/sfx2/jump_pad02.tga
	q3map_lightimage textures/sfx2/r_blur.tga
	surfaceparm nodamage
	surfaceparm nomarks
	q3map_surfacelight 30
	{
		map textures/sfx2/r_blur.tga
		tcmod rotate 300
		rgbgen identity
	}
	{
		clampmap textures/sfx/jcr2.tga
		blendfunc add
		tcmod rotate 100
		tcmod stretch sin 1.2 .8 0 1.4
		rgbgen wave square .5 .5 .25 1.4
	}
	{
		map textures/sfx2/jump_pad02.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/sfx2/ntrl_jumpad
{
	qer_editorimage textures/sfx2/jump_pad02.tga
	q3map_lightimage textures/sfx2/n_blur.tga
	surfaceparm nodamage
	surfaceparm nomarks
	q3map_surfacelight 30
	{
		map textures/sfx2/n_blur.tga
		tcmod rotate 300
		rgbgen identity
	}
	{
		clampmap textures/sfx/jc2.tga
		blendfunc add
		tcmod rotate 100
		tcmod stretch sin 1.2 .8 0 1.4
		rgbgen wave square .5 .5 .25 1.4
	}
	{
		map textures/sfx2/jump_pad02.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}
