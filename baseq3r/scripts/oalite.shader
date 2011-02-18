textures/base_light/xlight5
{
	qer_editorimage textures/base_light/xlight5.tga
	q3map_lightimage textures/base_light/xlight5.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 1000
	q3map_flare flareShader-glow
	{
		map textures/effects/envmap2.tga
		rgbGen identity
		tcGen environment 
	}
	{
		map textures/base_light/xlight5.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xlight5.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_34
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 300
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbgen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_22a
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 300
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

textures/base_light/proto_light
{
	qer_editorimage textures/base_light/proto_light.tga
	q3map_lightimage textures/base_light/proto_lightmap.tga
	surfaceparm metalsteps
	q3map_surfacelight 666
	q3map_flare flareShader-wide
	{
		map textures/base_light/proto_light.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/proto_lightmap.tga
		blendfunc add
	}
}

//03-04-07 dmn_clown
textures/base_light/proto_light_2k
{
	qer_editorimage textures/base_light/proto_light.tga
	q3map_lightimage textures/base_light/proto_lightmap.tga
	surfaceparm metalsteps
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map textures/base_light/proto_light.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/proto_lightmap.tga
		blendfunc add
	}
}

//for backwards compatibility
//01-27-07 dmn_clown
//03-04-07 better looking texture dmn_clown
textures/base_light/baslt4_1_2k
{
	qer_editorimage textures/base_light/baslt4_1.jpg
	q3map_lightimage textures/base_light/baslt4_1.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2000
	
	
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/baslt4_1.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/base_light/baslt4_1.blend.jpg
		blendfunc add
	}
}

textures/base_light/baslt4_1_4k
{
	qer_editorimage textures/base_light/baslt4_1.jpg
	surfaceparm nomarks
	q3map_surfacelight 4000
	
	
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/baslt4_1.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/base_light/baslt4_1.blend.jpg
		blendfunc add
	}
}

textures/base_light/proto_lightred
{
	surfaceparm nomarks
	q3map_surfacelight 400
	
	{
	
		map $lightmap
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_ONE GL_ONE
	}
}

textures/base_light/proto_lightred2
{
	qer_editorimage textures/base_light/proto_lightred.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	
	{
		map $lightmap
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen wave square  1 1 .5 1
	}
}

textures/base_light/ceil1_38
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 300
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_38_10k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_38_20k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 20000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_38_30k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 30000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_38_40k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 40000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_38_50k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 50000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_38_70k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 70000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_38_90k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 90000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_39
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 300
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/gothic_light/skulllight01
{
	qer_editorimage textures/gothic_light/skulllight01.tga
	q3map_lightimage textures/gothic_light/s_light2_blend.tga
	surfaceparm nomarks
	q3map_surfacelight 300
	{
		map textures/gothic_light/skulllight01.tga
		blendfunc gl_one gl_zero
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		tcgen lightmap
	}
	{
		map textures/gothic_light/s_light2_blend.tga
		blendfunc add
		rgbGen wave sin 0.25 0.25 0 .5
	}
}

//02-28-07 dmn_clown
textures/base_light/light1_1500
{
	qer_editorimage textures/base_light/light1.jpg
	q3map_lightimage textures/base_light/light1.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1.blend.jpg
		blendfunc add
	}
}

textures/base_light/light1dark
{
	qer_editorimage textures/base_light/light1.jpg
	q3map_lightimage textures/base_light/light1.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 500
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1.blend.jpg
		blendfunc add
	}
}


textures/base_light/light1_3000
{
	qer_editorimage textures/base_light/light1.jpg
	q3map_lightimage textures/base_light/light1.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 3000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1.blend.jpg
		blendfunc add
	}
}

textures/base_light/light1_5000
{
	qer_editorimage textures/base_light/light1.jpg
	q3map_lightimage textures/base_light/light1.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1.blend.jpg
		blendfunc add
	}
}

textures/base_light/light1blue_800
{
	qer_editorimage textures/base_light/light1blue.jpg
	q3map_lightimage textures/base_light/light1blue.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 800
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.blend.jpg
		blendfunc add
	}
}

textures/base_light/light1blue_1500
{
	qer_editorimage textures/base_light/light1blue.jpg
	q3map_lightimage textures/base_light/light1blue.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.blend.jpg
		blendfunc add
	}
}

textures/base_light/light1blue_2000
{
	qer_editorimage textures/base_light/light1blue.jpg
	q3map_lightimage textures/base_light/light1blue.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2000
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.blend.jpg
		blendfunc add
	}
}

textures/base_light/light1blue_5000
{
	qer_editorimage textures/base_light/light1blue.jpg
	q3map_lightimage textures/base_light/light1blue.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 5000
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.blend.jpg
		blendfunc add
	}
}

textures/base_light/light1red_2000
{
	qer_editorimage textures/base_light/light1red.jpg
	q3map_lightimage textures/base_light/light1red.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1red.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1red.blend.jpg
		blendfunc add
	}
}

textures/base_light/light1red_5000
{
	qer_editorimage textures/base_light/light1red.jpg
	q3map_lightimage textures/base_light/light1red.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1red.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1red.blend.jpg
		blendfunc add
	}
}

textures/base_light/light5_3000
{
	qer_editorimage textures/base_light/light5.jpg
	q3map_lightimage textures/base_light/xlight5.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 3000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.blend.jpg
		blendfunc add
	}
}

textures/base_light/light5_5k
{
	qer_editorimage textures/base_light/light5.jpg
	q3map_lightimage textures/base_light/xlight5.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.blend.jpg
		blendfunc add
	}
}

textures/base_light/light5_10k
{
	qer_editorimage textures/base_light/light5.jpg
	q3map_lightimage textures/base_light/xlight5.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.blend.jpg
		blendfunc add
	}
}

textures/base_light/light5_15k
{
	qer_editorimage textures/base_light/light5.jpg
	q3map_lightimage textures/base_light/xlight5.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 15000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.blend.jpg
		blendfunc add
	}
}

textures/base_light/patch10_pj_lite
{
	qer_editorimage textures/base_light/patch10_pj_lite.jpg
	q3map_lightimage textures/base_light/patch10_pj_lite.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2500
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/patch10_pj_lite.jpg
		blendfunc filter
		rgbgen identity
	}	
	{
		map textures/base_light/patch10_pj_lite.blend.jpg
		blendfunc add
	}
}

textures/base_light/patch10_pj_lite2
{
	qer_editorimage textures/base_light/patch10_pj_lite2.jpg
	q3map_lightimage textures/base_light/patch10_pj_lite2.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/patch10_pj_lite2.jpg
		blendfunc filter
		rgbgen identity
	}	
	{
		map textures/base_light/patch10_pj_lite2.blend.jpg
		blendfunc add
	}
}

textures/base_light/patch10_pj_lite2_1000
{
	qer_editorimage textures/base_light/patch10_pj_lite2.jpg
	q3map_lightimage textures/base_light/patch10_pj_lite2.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 1000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/patch10_pj_lite2.jpg
		blendfunc filter
		rgbgen identity
	}	
	{
		map textures/base_light/patch10_pj_lite2.blend.jpg
		blendfunc add
	}
}

textures/base_light/runway
{
	qer_editorimage textures/base_light/runway.tga
	q3map_lightimage textures/base_light/runwayb.jpg
	surfaceparm nomarks
	q3map_surfacelight 500
	
	{
		map textures/base_light/runway_glow.jpg
		rgbgen wave square 1 4 0 8
		tcmod scale 1 .3
		tcmod scroll 0 1
	}
	{
		map textures/base_light/runway.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/base_light/runwayb.tga
		blendFunc add
		rgbGen identity
	}

	
}

textures/base_light/ceil1_30
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_30_8k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 8000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}


textures/base_light/ceil1_30_trans
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	surfaceparm nomarks
	surfaceparm trans
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_34_10k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_34_1k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 1000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_34_2k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_34_5k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_37
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_4
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 1000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
		rgbGen wave noise 0.8 0.2 0 1
	}
}

//03-04-07 dmn_clown
textures/base_light/ceil1_22a_8k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 8000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_22a_10k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 8000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_22a_trans
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm nomarks
	surfaceparm trans
	q3map_surfacelight 300
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_34_10k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_34_1k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 1000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_34_2k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil1_34_5k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

textures/base_light/ceil_white5k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 300
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39_1
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 1000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39_10k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39_110k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 110000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39_15k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 15000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39_20k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 20000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39_2k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39_30k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 30000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39_500k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 500000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39_50k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 50000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39_5k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39_70k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 70000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39_90k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 90000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39_a
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 4000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga

		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/xceil1_39_b
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 8000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

textures/base_light/proto_lightred2_flare
{
	qer_editorimage textures/base_light/proto_lightred.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	q3map_flare flareShader-twilightflare
	
	{
		map $lightmap
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen wave square  1 1 .5 1
	}
}

textures/base_light/proto_lightred_flare
{
	qer_editorimage textures/base_light/proto_lightred.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	q3map_flare flareShader-twilightflare
	
	{
	
		map $lightmap
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_ONE GL_ONE
	}
}

textures/proto2/proto_lightblue_flare
{
	qer_editorimage textures/proto2/bluelight_on.tga
	surfaceparm nomarks
	q3map_surfacelight 1500
	q3map_flare flareShader-twilightflare
	
	{
		map $lightmap
		rgbGen identity
	}
	{	
		map textures/proto2/bluelight_on.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/proto2/bluelight_on.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave square  1 1 .5 1
	}
}

//03-04-07 dmn_clown
textures/base_light/geolight
{
	qer_editorimage textures/base_light/geolight.tga
	surfaceparm nomarks
	q3map_surfacelight 3000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/geolight.tga
		rgbgen identity
	}
	{
		map textures/base_light/geolight_glow.jpg
		blendfunc add
		rgbgen wave sin .1 .1 0 2
	}
}

textures/base_light/jaildr1_3
{
	qer_editorimage textures/base_light/jaildr1_3.jpg
	q3map_lightimage textures/base_light/jaildr1_3.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/jaildr1_3.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/jaildr1_3.blend.jpg
		blendfunc add
		rgbgen wave sin .1 .1 0 2
	}
}

textures/base_light/jaildr03_2
{
	qer_editorimage textures/base_light/jaildr03_2.jpg
	q3map_lightimage textures/base_light/jaildr03_2.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2000
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/jaildr03_2.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/jaildr03_2.blend.jpg
		blendfunc add
		rgbgen wave sin .9 .9 .5 2
	}
}

textures/base_floor/skylight1
{
	qer_editorimage textures/base_floor/skylight1.tga
	q3map_lightimage textures/base_floor/skylight1_lm.tga
	q3map_surfacelight 150
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_floor/skylight1.tga
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_floor/skylight1_lm.tga
		blendfunc add
		rgbgen wave sin .5 6 0 3.2
	}
}

textures/base_light/proto_lightblue
{
	qer_editorimage textures/base_light/proto_lightblue.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	{
	
		map $lightmap
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightblue.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightblue.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen wave square  1 1 .5 1
	}
}

textures/base_light/proto_lightblue_flare
{
	qer_editorimage textures/base_light/proto_lightblue.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	q3map_flare flareShader-twilightflare
	
	{
	
		map $lightmap
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightblue.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightblue.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen wave square  1 1 .5 1
	}
}

///////////////////////////////////////
// 	    Gothic Lights            //
/////////////////////////////////////// 


//moved from oanew to here
//textures/base_light/ceil1_4
//{
//	{
//		map textures/base_light/ceil1_34.tga
//	}
//	{
//		map textures/base_light/ceil1_34.blend.tga
//		blendfunc add
//		rgbGen wave noise 0.8 0.2 0 1 
//	}
//}

textures/gothic_light/pentagram_light1_3k
{
	qer_editorimage textures/gothic_light/pentagram_light1_3k.tga
	q3map_lightimage textures/gothic_light/pentagram_light1_3k_blend.tga
	surfaceparm nomarks
	q3map_surfacelight 3000
	q3map_flare flareShader
	{
		map textures/gothic_light/pentagram_light1_3k.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/gothic_light/pentagram_light1_3k_blend.tga
		blendfunc add
		rgbGen wave noise 0.8 0.2 0 1 
	}
}

textures/base_light/wsupprt1_12
{
	qer_editorimage textures/base_light/wsupprt1_12.tga
	q3map_lightimage textures/base_light/wsupprt1_12.tga
	surfaceparm nomarks
	q3map_surfacelight 3000
	q3map_flare flareShader
	{
		map textures/base_light/wsupprt1_12.tga
		rgbGen identity
	}
}

textures/base_light/wsupprt1_12
{
	q3map_lightimage textures/base_light/wsupprt1_12.tga
	q3map_surfacelight 600
	q3map_flare flareShader
	{
		map textures/base_light/wsupprt1_12.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/wsupprt1_12.tga
		blendfunc add
		rgbGen wave noise 0.8 0.2 0 1 
	}
}


textures/gothic_light/ironcrosslt2_1000
{
	qer_editorimage textures/gothic_light/ironcrosslt2_1000.tga
	q3map_lightimage textures/gothic_light/ironcrosslt2_1000_blend.tga
	surfaceparm nomarks
	q3map_surfacelight 1000
	q3map_flare flareShader
	{
		map textures/gothic_light/ironcrosslt2_1000.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/gothic_light/ironcrosslt2_1000_blend.tga
		blendfunc add
		rgbGen wave noise 0.8 0.2 0 1 
	}
}

textures/gothic_light/ironcrosslt2_10000
{
	qer_editorimage textures/gothic_light/ironcrosslt2_10000.tga
	q3map_lightimage textures/gothic_light/ironcrosslt2_10000_blend.tga
	surfaceparm nomarks
	q3map_surfacelight 10000
	q3map_flare flareShader
	{
		map textures/gothic_light/ironcrosslt2_10000.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/gothic_light/ironcrosslt2_10000_blend.tga
		blendfunc add
		rgbGen wave noise 0.8 0.2 0 1 
	}
}

textures/gothic_light/ironcrosslt2_5000
{
	qer_editorimage textures/gothic_light/ironcrosslt2_10000.tga
	q3map_lightimage textures/gothic_light/ironcrosslt2_5000_blend.tga
	q3map_surfacelight 5000
	q3map_flare flareShader
	{
		map textures/gothic_light/ironcrosslt2_5000.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/gothic_light/ironcrosslt2_5000_blend.tga
		blendfunc add
		rgbGen wave noise 0.8 0.2 0 1 
	}
}

textures/gothic_light/ironcrosslt2_20000
{
	qer_editorimage textures/gothic_light/ironcrosslt2.jpg
	q3map_lightimage textures/gothic_light/icl_blend.jpg
	q3map_surfacelight 20000
	q3map_flare flareShader
	{
		map textures/gothic_light/ironcrosslt2.jpg
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		tcgen lightmap
	}
	{
		map textures/gothic_light/icl_blend.jpg
		blendfunc add
		rgbGen wave sin .1 .1 0 .2
	}
}

textures/gothic_light/ironcrosslt2_25000
{
	qer_editorimage textures/gothic_light/ironcrosslt2.jpg
	q3map_lightimage textures/gothic_light/icl_blend.jpg
	q3map_surfacelight 25000
	q3map_flare flareShader
	{
		map textures/gothic_light/ironcrosslt2.jpg
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		tcgen lightmap
	}
	{
		map textures/gothic_light/icl_blend.jpg
		blendfunc add
		rgbGen wave sin .1 .1 0 .2
	}
}
//03-04-07 dmn_clown
textures/gothic_light/border7_ceil39_6k
{
	qer_editorimage textures/gothic_light/border7_ceil39.jpg
	q3map_lightimage textures/gothic_light/border7_ceil39blend.jpg
	q3map_surfacelight 6000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/border7_ceil39.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/border7_ceil39blend.jpg
		blendfunc add
	}
}

textures/gothic_light/gothic_light2_1k
{
	qer_editorimage textures/gothic_light/gothic_light2.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 1000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light2.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light2_2k
{
	qer_editorimage textures/gothic_light/gothic_light2.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 2000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light2.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light2_4k
{
	qer_editorimage textures/gothic_light/gothic_light2.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 4000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light2.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/flicker_light15k
{
	qer_editorimage textures/gothic_light/gothic_light3.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 15000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen wave noise .4 .4 0 .9
	}
	{
		map textures/gothic_light/gothic_light3.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/base_light/proto_light2.jpg
		blendfunc add
		rgbgen wave triangle 4 4 0 6
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave noise .4 .4 0 .9
	}
}

textures/gothic_light/gothic_light3_10K
{
	qer_editorimage textures/gothic_light/gothic_light3.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 10000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light3.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light3_1K
{
	qer_editorimage textures/gothic_light/gothic_light3.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 1000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light3.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light3_15K
{
	qer_editorimage textures/gothic_light/gothic_light3.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 15000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light3.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light3_20K
{
	qer_editorimage textures/gothic_light/gothic_light3.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 20000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light3.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light3_2K
{
	qer_editorimage textures/gothic_light/gothic_light3.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 2000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light3.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light3_3K
{
	qer_editorimage textures/gothic_light/gothic_light3.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 3000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light3.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light3_4K
{
	qer_editorimage textures/gothic_light/gothic_light3.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 4000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light3.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light3_5K
{
	qer_editorimage textures/gothic_light/gothic_light3.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 5000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light3.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light3_6K
{
	qer_editorimage textures/gothic_light/gothic_light3.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 6000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light3.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light3_7K
{
	qer_editorimage textures/gothic_light/gothic_light3.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 7000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light3.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light3_8K
{
	qer_editorimage textures/gothic_light/gothic_light3.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 8000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light3.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light2_lrg_1k
{
	qer_editorimage textures/gothic_light/gothic_light2.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 1000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light2.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light2_lrg_200
{
	qer_editorimage textures/gothic_light/gothic_light2.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 200
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light2.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light2_lrg_2k
{
	qer_editorimage textures/gothic_light/gothic_light2.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 2000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light2.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/goth_lt2_lrg2k
{
	qer_editorimage textures/gothic_light/gothic_light2.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 2000
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light2.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/gothic_light/gothic_light2_lrg_500
{
	qer_editorimage textures/gothic_light/gothic_light2.jpg
	q3map_lightimage textures/gothic_light/gothic_light2_blend.jpg
	q3map_surfacelight 500
	surfaceparm nomarks
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_light/gothic_light2.jpg
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/gothic_light/gothic_light2_blend.jpg
		blendfunc add
		rgbgen wave sin .4 .4 0 .1
	}
}

textures/dsi/cretetrimlight2
{
	qer_editorimage textures/dsi/cretetrimlight2.tga
	q3map_lightimage textures/dsi/cretelighttrim2_glow.tga
	surfaceparm nomarks
	q3map_lightsubdivide 32
	q3map_surfacelight 1000
	{
		map textures/dsi/cretetrimlight2.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/dsi/cretetrimlight2_glow.tga
		blendfunc add
	}
}

textures/dsi/cretetrimlight2r
{
	qer_editorimage textures/dsi/cretetrimlight2r.tga
	q3map_lightimage textures/dsi/cretelighttrim2r_glow.tga
	surfaceparm nomarks
	q3map_lightsubdivide 32
	q3map_surfacelight 1000
	{
		map textures/dsi/cretetrimlight2r.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/dsi/cretetrimlight2r_glow.tga
		blendfunc add
	}
}

textures/dsi/tlight
{
	qer_editorimage textures/dsi/tlight.tga
	q3map_lightimage textures/dsi/tlight.blend.tga
	surfaceparm nomarks
	q3map_lightsubdivide 32
	q3map_surfacelight 1000
	{
		map textures/dsi/tlight.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/dsi/tlight.blend.tga
		blendfunc add
	}
}

textures/dsi/tlightr
{
	qer_editorimage textures/dsi/tlightr.tga
	q3map_lightimage textures/dsi/tlightr.blend.tga
	surfaceparm nomarks
	q3map_lightsubdivide 32
	q3map_surfacelight 1000
	{
		map textures/dsi/tlightr.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/dsi/tlightr.blend.tga
		blendfunc add
	}
}
