textures/bubctf1/bubctf1_skybox
{
	qer_editorimage textures/bubctf1/bubctf1_view.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_lightsubdivide 256
	q3map_surfacelight 42
	q3map_globaltexture
	q3map_sun 0.419608 0.34902 0.270588 210 42 33
	skyParms env/bubctf1/bubctf1 128 -
}

textures/bubctf1/e8_launchpad1
{
	q3map_lightimage textures/bubctf1/e8_launchpad1.blend.tga
	surfaceparm nodamage
	q3map_surfacelight 100
	{
		map textures/bubctf1/e8_launchpad1_fx.tga
		rgbGen identity
		tcMod scroll 0 3
	}
	{
		map textures/bubctf1/e8_launchpad1.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map textures/bubctf1/e8_launchpad1.blend.tga
		blendfunc add
		rgbGen wave triangle 0.2 2 1 2 
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_dst_alpha
		rgbGen identity
		tcGen lightmap 
	}
}

textures/bubctf1/e8_launchpad1r
{
	q3map_lightimage textures/bubctf1/e8_launchpad1.blendr.tga
	surfaceparm nodamage
	q3map_surfacelight 100
	{
		map textures/bubctf1/e8_launchpad1_fxr.tga
		rgbGen identity
		tcMod scroll 0 3
	}
	{
		map textures/bubctf1/e8_launchpad1r.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map textures/bubctf1/e8_launchpad1.blendr.tga
		blendfunc add
		rgbGen wave triangle 0.2 2 1 2 
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_dst_alpha
		rgbGen identity
		tcGen lightmap 
	}
}

//changed the order of the mapping
// so it doesn't look like shit
// in vertex lighting 2-20-2007 dmn_clown
textures/bubctf1/e8_jumppad02
{
	q3map_lightimage textures/bubctf1/e8_jumppad02_fx.tga
	surfaceparm nodamage
	polygonoffset
	q3map_surfacelight 200
	{
		map textures/bubctf1/e8_jumppad02.tga
		rgbGen identity
	}
	{
		map textures/bubctf1/e8_jumppad02_fx.tga
		blendfunc add
		tcMod rotate 360
		tcMod stretch sin 1 0.8 1 0.4 
	}
	{
		map textures/bubctf1/e8_jumppad02.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_dst_alpha
		rgbGen identity
		tcGen lightmap 
	}
}
//changed the order of the mapping
// so it doesn't look like shit
// in vertex lighting 2-20-2007 dmn_clown
textures/bubctf1/e8_jumppad02r
{
	q3map_lightimage textures/bubctf1/e8_jumppad02_fxr.tga
	surfaceparm nodamage
	polygonoffset
	q3map_surfacelight 200
	{
		map textures/bubctf1/e8_jumppad02r.tga
		rgbGen identity
	}
	{
		map textures/bubctf1/e8_jumppad02_fxr.tga
		blendfunc add
		tcMod rotate 360
		tcMod stretch sin 1 0.8 1 0.4 
	}
	{
		map textures/bubctf1/e8_jumppad02r.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_dst_alpha
		rgbGen identity
		tcGen lightmap 
	}
}

textures/evil8_lights/e8btrimlight
{
	qer_editorimage textures/evil8_lights/e8btrimlight.tga
	q3map_lightimage textures/evil8_lights/e8btrimlight.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 250
	{
		map textures/evil8_lights/e8btrimlight.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_lights/e8btrimlight.blend.tga
		blendfunc add
	}
}

textures/ctf/blue_telep
{
	q3map_lightimage textures/effects/jumpcircblue.tga
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
		clampmap textures/effects/jumpcircblue.tga
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.6 -0.3 0 0.5 
	}
	{
		clampmap textures/effects/jumpcircblue.tga
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.5 0.3 0 0.5 
	}
}

textures/ctf/red_telep
{
	q3map_lightimage textures/effects/jumpcircred.tga
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
		clampmap textures/effects/jumpcircred.tga
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.6 -0.3 0 0.5 
	}
	{
		clampmap textures/effects/jumpcircred.tga
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.5 0.3 0 0.5 
	}
}

textures/bubctf1/weapfloor_red
{
	cull disable
	{
		clampmap textures/flares/largeglow.tga
		blendfunc add
		rgbGen const ( 0.85098 0.701961 0.701961 )
		tcMod rotate -40
	}
	{
		clampmap textures/flares/wide.tga
		blendfunc add
		rgbGen const ( 1 0 0 )
		tcMod rotate 42
		tcMod stretch sin 1 0.1 0 1 
	}
}

textures/bubctf1/weapfloor_blue
{
	cull disable
	{
		clampmap textures/flares/largeglow.tga
		blendfunc add
		rgbGen const ( 0.705882 0.717647 0.847059 )
		tcMod rotate -40
	}
	{
		clampmap textures/flares/wide.tga
		blendfunc add
		rgbGen const ( 0 0 1 )
		tcMod rotate 42
		tcMod stretch sin 1 0.1 0 1 
	}
}

textures/ctf_unified/ta_techspawn_blue
{
	cull disable
	{
		clampmap textures/flares/largeglow.tga
		blendfunc add
		rgbGen const ( 0.705882 0.717647 0.847059 )
		tcMod rotate -40
	}
	{
		clampmap textures/flares/wide.tga
		blendfunc add
		rgbGen const ( 0 0 1 )
		tcMod rotate 42
		tcMod stretch sin 1 0.1 0 1 
	}
}

textures/ctf_unified/ta_techspawn_red
{
	cull disable
	{
		clampmap textures/flares/largeglow.tga
		blendfunc add
		rgbGen const ( 0.85098 0.701961 0.701961 )
		tcMod rotate -40
	}
	{
		clampmap textures/flares/wide.tga
		blendfunc add
		rgbGen const ( 1 0 0 )
		tcMod rotate 42
		tcMod stretch sin 1 0.1 0 1 
	}
}

textures/bubctf1/weapfloor_neutral
{
	cull disable
	{
		clampmap textures/flares/largeglow.tga
		blendfunc add
		tcMod rotate -40
	}
	{
		clampmap textures/flares/wide.tga
		blendfunc add
		tcMod rotate 42
		tcMod stretch sin 1 0.1 0 1 
	}
}

textures/base_light/baslt4_1_2k
{
	qer_editorimage textures/base_light/xlight5.blend.tga
	q3map_lightimage textures/base_light/xlight5.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 250
	{
		map textures/base_light/xlight5.tga
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

textures/evil8_lights/e8trimlight
{
	qer_editorimage textures/evil8_lights/e8trimlight.tga
	q3map_lightimage textures/evil8_lights/e8trimlight.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 250
	{
		map textures/evil8_lights/e8trimlight.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_lights/e8trimlight.blend.tga
		blendfunc add
	}
}

textures/evil8_floor/e8warn2steplight
{
	qer_editorimage textures/evil8_floor/e8warn2steplight.tga
	q3map_lightimage textures/evil8_floor/e8warn2steplight.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 250
	{
		map textures/evil8_floor/e8warn2steplight.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_floor/e8warn2steplight.blend.tga
		blendfunc add
	}
}

textures/evil8_floor/e8minitrimlight
{
	qer_editorimage textures/evil8_lights/e8minitrimlight.tga
	q3map_lightimage textures/evil8_lights/e8minitrimlight.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 250
	{
		map textures/evil8_lights/e8minitrimlight.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_lights/e8minitrimlight.blend.tga
		blendfunc add
	}
}

textures/evil8_fx/e8_jumppad02
{
	q3map_lightimage textures/evil8_fx/e8_jumppad02_fx.tga
	surfaceparm metalsteps
	q3map_surfacelight 100
	
	{
		map textures/evil8_fx/e8_jumppad02.tga
	//	blendfunc add
		rgbGen identity
	}
	{
		map textures/evil8_fx/e8_jumppad02_fx.tga
		blendfunc add
		tcMod rotate 360
		tcMod stretch sin 1 0.8 1 0.4 
	}
	{
		map textures/evil8_fx/e8_jumppad02.tga
		blendfunc add
		rgbGen identity
	}
	{
		map textures/evil8_fx/e8_jumppad02.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_dst_alpha
		rgbGen identity
		tcGen lightmap 
	}
}

textures/evil8_lights/e8_rlightb2
{
	qer_editorimage textures/evil8_lights/e8_rlightb2.tga
	q3map_lightimage textures/evil8_lights/e8_rlightb2.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 450
	q3map_flare flareShader
	{
		map textures/evil8_lights/e8_rlightb2.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_lights/e8_rlightb2.blend.tga
		blendfunc add
	}
}

textures/evil8_lights/e8_rlightb
{
	qer_editorimage textures/evil8_lights/e8_rlightb.tga
	q3map_lightimage textures/evil8_lights/e8_rlightb.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 450
	{
		map textures/evil8_lights/e8_rlightb.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_lights/e8_rlightb.blend.tga
		blendfunc add
	}
}

textures/evil8_lights/evil8_rlight
{
	qer_editorimage textures/evil8_lights/e8_rlight.tga
	q3map_lightimage textures/evil8_lights/e8_rlight.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 950
	q3map_flare flareShader
	{
		map textures/evil8_lights/e8_rlight.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_lights/e8_rlight.blend.tga
		blendfunc add
	}
}

textures/evil8_lights/e8tmtllight
{
	qer_editorimage textures/evil8_lights/e8tmtllight.tga
	q3map_lightimage textures/evil8_lights/e8tmtllight.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 950
	{
		map textures/evil8_lights/e8tmtllight.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_lights/e8tmtllight.blend.tga
		blendfunc add
	}
}

textures/evil8_lights/e8tmtllight2
{
	qer_editorimage textures/evil8_lights/e8tmtllight2.tga
	q3map_lightimage textures/evil8_lights/e8tmtllight2.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 950
	{
		map textures/evil8_lights/e8tmtllight2.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_lights/e8tmtllight2.blend.tga
		blendfunc add
	}
}

textures/evil8_lights/e8tmtllight2b
{
	qer_editorimage textures/evil8_lights/e8tmtllight2b.tga
	q3map_lightimage textures/evil8_lights/e8tmtllight2b.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 950
	{
		map textures/evil8_lights/e8tmtllight2b.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_lights/e8tmtllight2b.blend.tga
		blendfunc add
	}
}

textures/evil8_grate/e8xgirder_small2
{
	cull disable
	{
		map textures/evil8_grate/e8xgirder_small2.tga
		rgbGen identity
		depthWrite
		alphaFunc GE128
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
		depthFunc equal
	}
}

textures/evil8_fx/e8alphaspawn
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm pointlight
	{
		map textures/evil8_fx/e8alphaspawn.tga
		blendfunc add
		rgbGen Vertex
	}
}

textures/evil8_fx/e8yarrow
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm pointlight
	{
		map textures/evil8_fx/e8yarrow.tga
		blendfunc add
		rgbGen Vertex
	}
}

textures/evil8_trim/e8lighttrim_static
{
	qer_editorimage textures/evil8_trim/e8lighttrim.tga
	surfaceparm nomarks
	q3map_surfacelight 400
	{
		map textures/evil8_trim/e8lighttrim.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_trim/e8lighttrim.blend.tga
		blendfunc add
	}
}

textures/evil8_trim/e8lighttrim_glow
{
	qer_editorimage textures/evil8_trim/e8lighttrim.tga
	surfaceparm nomarks
	q3map_surfacelight 400
	{
		map textures/evil8_trim/e8lighttrim.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_trim/e8lighttrim.blend.tga
		blendfunc add
		rgbGen wave sawtooth 0.2 1 1 0.2 
	}
}

textures/evil8_trim/e8lighttrim_b_static
{
	qer_editorimage textures/evil8_trim/e8lighttrim_b.tga
	surfaceparm nomarks
	q3map_surfacelight 400
	{
		map textures/evil8_trim/e8lighttrim_b.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_trim/e8lighttrim_b.blend.tga
		blendfunc add
	}
}

textures/evil8_trim/e8lighttrim_b_glow
{
	qer_editorimage textures/evil8_trim/e8lighttrim_b.tga
	surfaceparm nomarks
	q3map_surfacelight 400
	{
		map textures/evil8_trim/e8lighttrim_b.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_trim/e8lighttrim_b.blend.tga
		blendfunc add
		rgbGen wave sawtooth 0.2 1 1 0.2 
	}
}

textures/evil8_fx/e8jumpspawn02
{
	q3map_lightimage textures/evil8_fx/e8jumpspawn02_fx.tga
	surfaceparm metalsteps
	q3map_surfacelight 100
	{
		map textures/evil8_fx/e8jumpspawn02_fx.tga
		rgbGen identity
		tcMod rotate 760
	}
	{
		map textures/evil8_fx/e8jumpspawn02.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_dst_alpha
		rgbGen identity
		tcGen lightmap 
	}
}

textures/evil8_fx/e8jumpspawn02b
{
	q3map_lightimage textures/evil8_fx/e8jumpspawn02_fx.tga
	surfaceparm metalsteps
	q3map_surfacelight 100
	{
		map textures/evil8_fx/e8jumpspawn02_fx.tga
		rgbGen identity
		tcMod rotate 760
	}
	{
		map textures/evil8_fx/e8jumpspawn02b.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_dst_alpha
		rgbGen identity
		tcGen lightmap 
	}
}

textures/evil8_fx/e8jumpspawn02c
{
	q3map_lightimage textures/evil8_fx/e8jumpspawn02_fx.tga
	surfaceparm metalsteps
	q3map_surfacelight 100
	{
		map textures/evil8_fx/e8jumpspawn02_fx.tga
		rgbGen identity
		tcMod rotate 760
	}
	{
		map textures/evil8_fx/e8jumpspawn02c.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_dst_alpha
		rgbGen identity
		tcGen lightmap 
	}
}

textures/evil8_fx/e8_launchpad1
{
	q3map_lightimage textures/evil8_fx/e8_launchpad1.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 100
	{
		map textures/evil8_fx/e8_launchpad1_fx.tga
		rgbGen identity
		tcMod scroll 0 3
	}
	{
		map textures/evil8_fx/e8_launchpad1.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map textures/evil8_fx/e8_launchpad1.blend.tga
		blendfunc add
		rgbGen wave triangle 0.2 2 1 2 
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_dst_alpha
		rgbGen identity
		tcGen lightmap 
	}
}

textures/evil8_fx/e8red_dcl
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm pointlight
	{
		map textures/evil8_fx/e8red_dcl.tga
		blendfunc add
		rgbGen Vertex
	}
}

textures/evil8_fx/e8blue_dcl
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm pointlight
	{
		map textures/evil8_fx/e8blue_dcl.tga
		blendfunc add
		rgbGen Vertex
	}
}

textures/evil8_grate/e8xgirder
{
	surfaceparm alphashadow
	surfaceparm metalsteps
	surfaceparm nomarks
	surfaceparm trans
	cull disable
	nopicmip
	{
		map textures/evil8_grate/e8xgirder.tga
		rgbGen identity
		depthWrite
		alphaFunc GE128
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
		depthFunc equal
	}
}

textures/evil8_grate/e8xgirder_small
{
	cull disable
	{
		map textures/evil8_grate/e8xgirder_small.tga
		rgbGen identity
		depthWrite
		alphaFunc GE128
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
		depthFunc equal
	}
}

textures/evil8_trim/e8mtltrim2
{
	surfaceparm alphashadow
	surfaceparm metalsteps
	surfaceparm trans
	cull disable
	{
		map textures/evil8_trim/e8mtltrim2.tga
		rgbGen identity
		depthWrite
		alphaFunc GE128
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
		depthFunc equal
	}
}

textures/evil8_trim/e8wrntrim2
{
	surfaceparm alphashadow
	surfaceparm metalsteps
	surfaceparm trans
	cull disable
	{
		map textures/evil8_trim/e8wrntrim2.tga
		rgbGen identity
		depthWrite
		alphaFunc GE128
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
		depthFunc equal
	}
}

textures/evil8_trim/e8mtltrim2
{
	surfaceparm alphashadow
	surfaceparm metalsteps
	surfaceparm trans
	cull disable
	{
		map textures/evil8_trim/e8mtltrim2.tga
		rgbGen identity
		depthWrite
		alphaFunc GE128
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
		depthFunc equal
	}
}

textures/evil8_grate/e8bgrate01
{
	surfaceparm alphashadow
	surfaceparm metalsteps
	cull disable
	{
		map textures/evil8_grate/e8bgrate01.tga
		rgbGen identity
		depthWrite
		alphaFunc GE128
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
		depthFunc equal
	}
}

textures/evil8_fx/e8rail
{
	surfaceparm alphashadow
	cull disable
	nomipmaps
	{
		map textures/evil8_fx/e8rail.tga
		rgbGen identity
		depthWrite
		alphaFunc GE128
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
		depthFunc equal
	}
}

textures/evil8_fx/e8icon_red
{
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	cull disable
	{
		map textures/evil8_fx/e8icon_red.tga
		blendfunc add
		rgbGen wave triangle 0.2 0.5 0 0.2 
	}
}

textures/evil8_fx/e8icon_blue
{
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	cull disable
	{
		map textures/evil8_fx/e8icon_blue.tga
		blendfunc add
		rgbGen wave triangle 0.2 0.5 0 0.2 
	}
}

//doesn't work right, please edit as you see fit
textures/evil8_fx/e8scuffs1
{
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	cull disable
	qer_trans 0.4
	{
		map textures/evil8_fx/e8scuffs1.tga
		blendfunc blend
		rgbGen Vertex
		depthWrite
	}
}

textures/evil8_fx/e8circle_red
{
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	{
		map textures/evil8_fx/e8circle_red.tga
		blendfunc add
		rgbGen identity
	}
}

textures/evil8_fx/e8circle_blue
{
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	{
		map textures/evil8_fx/e8circle_blue.tga
		blendfunc add
		rgbGen identity
	}
}

textures/evil8_fx/e8circle_red_fade
{
	qer_editorimage textures/evil8_fx/e8circle_red.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	cull disable
	{
		map textures/evil8_fx/e8circle_red.tga
		blendfunc add
		rgbGen wave triangle 0.2 0.5 0 0.5 
	}
}

textures/evil8_fx/e8circle_blue_fade
{
	qer_editorimage textures/evil8_fx/e8circle_blue.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	cull disable
	{
		map textures/evil8_fx/e8circle_blue.tga
		blendfunc add
		rgbGen wave triangle 0.2 0.5 0 0.5 
	}
}

textures/evil8_fx/e8spawn01b
{
	{
		map textures/sfx/proto_zzztblu2.tga
		rgbGen identity
		tcMod turb 0 0.5 0 9.6
		tcMod scale 2 2
		tcMod scroll 9 5
	}
	{
		map textures/evil8_fx/e8spawn01b.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_dst_alpha
		rgbGen identity
		tcGen lightmap 
	}
}

textures/evil8_trim/e8trim_bluefx
{
	q3map_lightimage textures/e8trim/evil8_trimfx_b.tga
	q3map_surfacelight 100
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_trim/e8trim_bluefx.tga
		blendfunc filter
		rgbGen identity
	}
	{
		map textures/evil8_trim/e8trimfx_b.tga
		blendfunc add
		tcMod scroll -2 0
	}
}

textures/evil8_trim/e8trim_redfx
{
	q3map_lightimage textures/evil8_trim/e8trimfx_r.tga
	q3map_surfacelight 100
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_trim/e8trim_redfx.tga
		blendfunc filter
		rgbGen identity
	}
	{
		map textures/evil8_trim/e8trimfx_r.tga
		blendfunc add
		tcMod scroll -2 0
	}
}

//not for public release :P
textures/evil8_fx/e8evilspacetxt
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm pointlight
	{
		map textures/evil8_fx/e8evilspacetxt.tga
		blendfunc add
		rgbGen Vertex
	}
}

textures/evil8_lights/e8tinylight
{
	qer_editorimage textures/evil8_lights/e8tinylight.tga
	q3map_lightimage textures/evil8_lights/e8tinylight.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 950
	{
		map textures/evil8_lights/e8tinylight.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_lights/e8tinylight.blend.tga
		blendfunc add
	}
}

textures/evil8_lights/e8tinylightblue
{
	qer_editorimage textures/evil8_lights/e8tinylightblue.tga
	surfaceparm nomarks
	q3map_surfacelight 950
	q3map_flare flareShader
	{
		map textures/evil8_lights/e8tinylightblue.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_lights/e8tinylightblue.blend.tga
		blendfunc add
	}
}

textures/evil8_fx/e8beam
{
	qer_editorimage textures/evil8_fx/e8beam.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm trans
	cull disable
	qer_trans 0.6
	{
		map textures/evil8_fx/e8beam.tga
		blendfunc add
		tcMod scroll 0.3 0
	}
}

textures/evil8_fx/e8beam_blue
{
	qer_editorimage textures/evil8_fx/e8beam_blue.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm trans
	cull disable
	qer_trans 0.6
	{
		map textures/evil8_fx/e8beam_blue.tga
		blendfunc add
		tcMod scroll 0.3 0
	}
}

textures/evil8_base/e8metal03c_shiney
{
	qer_editorimage textures/evil8_base/e8metal03c.tga
	{
		map textures/effects/tinfx.tga
		rgbGen identity
		tcGen environment 
	}
	{
		map textures/evil8_base/e8metal03c.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_dst_alpha
		rgbGen identity
		tcGen lightmap 
	}
}

textures/evil8_base/e8metal_blue_shiney
{
	qer_editorimage textures/evil8_base/e8metal_blue.tga
	{
		map textures/effects/tinfx.tga
		rgbGen identity
		tcGen environment 
	}
	{
		map textures/evil8_base/e8metal_blue.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_dst_alpha
		rgbGen identity
		tcGen lightmap 
	}
}

textures/evil8_base/e8metal_red_shiney
{
	qer_editorimage textures/evil8_base/e8metal_red.tga
	{
		map textures/effects/tinfx.tga
		rgbGen identity
		tcGen environment 
	}
	{
		map textures/evil8_base/e8metal_red.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc gl_dst_color gl_one_minus_dst_alpha
		rgbGen identity
		tcGen lightmap 
	}
}

//broken shader D:?
textures/evil8_trim/e8trimlight
{
	q3map_lightimage textures/evil8_trim/e8trimlight_fx.tga
	q3map_surfacelight 100
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_trim/e8trimlight.tga
		blendfunc filter
		rgbGen identity
	}
	{
		map textures/evil8_trim/e8trimlight_fx.tga
		blendfunc add
		tcMod scroll -2 0
	}
}

textures/evil8_trim/e8trimlight2_blue
{
	qer_editorimage textures/evil8_trim/e8trimlight2_blue.tga
	surfaceparm nomarks
	q3map_surfacelight 400
	q3map_flare flareShader
	{
		map textures/evil8_trim/e8trimlight2_blue.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_trim/e8trimlight2_blue.blend.tga
		blendfunc add
		tcMod scroll -0.7 0
	}
}

textures/evil8_trim/e8trimlight2_red
{
	qer_editorimage textures/evil8_trim/e8trimlight2_red.tga
	surfaceparm nomarks
	q3map_surfacelight 400
	q3map_flare flareShader
	{
		map textures/evil8_trim/e8trimlight2_red.tga
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/evil8_trim/e8trimlight2_red.blend.tga
		blendfunc add
		tcMod scroll -0.7 0
	}
}

//===========================//
//=======floor sounds========//
//===========================//
textures/evil8_floor/e8clangfloor
{
	surfaceparm metalsteps
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_floor/e8clangfloor.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/evil8_floor/e8clangfloor01
{
	surfaceparm metalsteps
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_floor/e8clangfloor01.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/evil8_floor/e8clangfloor03
{
	surfaceparm metalsteps
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_floor/e8clangfloor03.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/evil8_floor/e8clangfloor04
{
	surfaceparm metalsteps
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_floor/e8clangfloor04.tga
		blendfunc filter
		rgbGen identity
	}
	{
		map textures/detail/d_conc.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 4 4
	}
}

textures/evil8_floor/e8clangfloor04_blue
{
	surfaceparm metalsteps
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_floor/e8clangfloor04_blue.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/evil8_floor/e8clangfloor04_red
{
	surfaceparm metalsteps
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_floor/e8clangfloor04_red.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/evil8_floor/e8clangfloor04b
{
	surfaceparm metalsteps
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_floor/e8clangfloor04b.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/evil8_floor/e8clangfloor04warn
{
	surfaceparm metalsteps
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_floor/e8clangfloor04warn.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/evil8_floor/e8clangfloor04warn2
{
	surfaceparm metalsteps
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_floor/e8clangfloor04warn2.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/evil8_floor/e8clangfloor05
{
	surfaceparm metalsteps
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_floor/e8clangfloor05.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/evil8_floor/e8clangfloor05b
{
	surfaceparm metalsteps
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_floor/e8clangfloor05b.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/evil8_floor/e8clangfloor05c
{
	surfaceparm metalsteps
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/evil8_floor/e8clangfloor05c.tga
		blendfunc filter
		rgbGen identity
	}
}

textures/dsi/weapspawn01
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm pointlight
	nopicmip
	polygonoffset
	{
		clampmap textures/dsi/weapspawn01.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate 45
	}
	{
		map textures/dsi/weapspawn01_glow.tga
		blendfunc add
		rgbGen Vertex
	}
}

textures/dsi/dsiglass
{
	qer_editorimage textures/dsi/dsiglass.tga
	surfaceparm trans
	cull disable
	qer_trans 0.5
	{
		map textures/effects/tinfx.tga
		blendfunc add
		rgbGen identity
		tcGen environment 
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
	}
}
