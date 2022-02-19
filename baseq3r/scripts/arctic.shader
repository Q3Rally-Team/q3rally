textures/arctic/arc_skybox1
{
	qer_editorimage textures/arctic/arc_sky1
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_sun .9 .9 1 70 45 30
	q3map_surfacelight 70
	skyparms env/arcsky/arcsky1 1024 -
}

textures/arctic/arc_skybox1_kam
{
	qer_editorimage textures/arctic/arc_sky1
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_sun .9 .9 1 70 45 30
	q3map_surfacelight 100
	skyparms env/arcsky/arcsky1 1024 -
}

textures/arctic/arc_skybox2
{
	qer_editorimage textures/arctic/arc_sky2
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_sun .9 .9 1 45 0 70
	q3map_surfacelight 45
	skyparms env/arcsky/arcsky2 1024 -
}

textures/arctic/arc_skybox3
{
	qer_editorimage textures/arctic/arc_sky3
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_sun .9 .9 1 45 0 70
	q3map_surfacelight 25
	skyparms env/arcsky/arcsky2 1024 -

	{
		map textures/skies/dimclouds.tga
		blendFunc add
		tcMod scroll 0.05 0.05
		tcMod scale 3 3
	}
}

textures/arctic/concrete1_13_1k
{
	qer_editorimage textures/arctic/concrete1_13.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_13.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/arctic/concrete1_13_1k_pulse
{
	qer_editorimage textures/arctic/concrete1_13.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_13.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_9.blend.tga
		blendfunc GL_ONE GL_ONE
		rgbGen wave inversesawtooth 0.5 0.5 0 0.5
	}
}

textures/arctic/concrete1_13_1k_pulse_star
{
	qer_editorimage textures/arctic/concrete1_13.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_13.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_9.blend_b.tga
		blendfunc GL_ONE GL_ONE
		rgbGen wave inversesawtooth 0.5 0.5 0 0.5
	}
}

textures/arctic/concrete1_15
{
	qer_editorimage textures/arctic/concrete1_15.tga
	surfaceparm nomarks
	q3map_surfacelight 500

	{
		map textures/arctic/trim6_1.fx.tga
		tcmod scale 100 100
		tcmod scroll 3 0
		blendFunc GL_ONE GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/trim6_1.fx.tga
		tcmod scale .500 0
		tcmod scroll 1 0
		blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_15.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
}

textures/arctic/concrete1_15_b
{
	qer_editorimage textures/arctic/concrete1_15.tga
	surfaceparm nomarks
	q3map_surfacelight 500

	{
		map textures/arctic/trim6_1.fx.tga
		rgbGen wave sin 0.4 0.1 0.5 1
		blendFunc GL_ONE GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_15.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
}

textures/arctic/concrete1_8_1k
{
	qer_editorimage textures/arctic/concrete1_8.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_8.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_8.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/concrete1_8_1k_pulse
{
	qer_editorimage textures/arctic/concrete1_8.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_8.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_8.blend.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/concrete1_8_1k_pulse_star
{
	qer_editorimage textures/arctic/concrete1_8.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_8.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_8.blend_b.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/concrete1_9_1k
{
	qer_editorimage textures/arctic/concrete1_9.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_9.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_9.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/concrete1_9_1k_pulse
{
	qer_editorimage textures/arctic/concrete1_9.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_9.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_9.blend.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/concrete1_9_1k_pulse_star
{
	qer_editorimage textures/arctic/concrete1_9.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_9.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_9.blend_b.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/fence_snow1_1
{
	qer_editorimage textures/arctic/sprite2_1.tga
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/sprite2_1.tga
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/flag1_1
{
	qer_editorimage textures/arctic/flag1_1.tga
	tessSize 64
	deformVertexes wave 194 sin 0 3 0 .4
	deformVertexes normal .5 .1
	surfaceparm nomarks
	cull none

	{
		map textures/arctic/flag1_1.tga
		rgbGen identity
	}
	{
		map textures/arctic/flag1_1.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
	{
		map textures/sfx/shadow.tga
		tcGen environment
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/arctic/floodlight_1k
{
	qer_editorimage textures/arctic/wall2_4.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map textures/arctic/wall2_4.tga
		blendFunc GL_ONE GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/wall2_4.blend.tga
		blendfunc GL_ONE GL_ONE
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		clampmap textures/arctic/wall2_4.fx.tga
		tcMod rotate 180
		blendFunc GL_ONE GL_ONE
	}
}

textures/arctic/floorgrate1_1
{
	qer_editorimage textures/arctic/floorgrate1_1.TGA
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/floorgrate1_1.TGA
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/floorgrate1_2
{
	qer_editorimage textures/arctic/floorgrate1_2.TGA
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/floorgrate1_2.TGA
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/floorgrate1_3
{
	qer_editorimage textures/arctic/floorgrate1_3.TGA
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/floorgrate1_3.TGA
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// Kamarov's Glass O'Love (really it is lovely, trust us)
textures/arctic/glass1
{
	qer_editorimage textures/arctic/glass1.tga
        surfaceparm trans
	surfaceparm nolightmap
	surfaceparm nomarks
	cull none
	qer_trans 	0.5

        {
		map textures/effects/tinfx.tga
                tcgen environment
		blendFunc GL_ONE GL_ONE
		rgbGen identity
	}
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
           
}

textures/arctic/health_pad
{
	qer_editorimage textures/arctic/concrete1_14.tga
	q3map_lightimage textures/arctic/concrete1_14.fx.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map textures/arctic/concrete1_14.tga
		blendFunc GL_ONE GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/concrete1_14.blend.tga
		blendfunc GL_ONE GL_ONE
		rgbGen wave inversesawtooth 0.5 0.5 0 0.5
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		clampmap textures/arctic/concrete1_14.fx.tga
		tcMod rotate 180
		blendFunc GL_ONE GL_ONE
	}
}

textures/arctic/icefog_blue
{
	qer_editorimage textures/arctic/fog.tga
	qer_trans 0.15
	surfaceparm nonsolid
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm fog
	fogparms ( .162 .192 .234 ) 300
}

textures/arctic/icefog_white
{
	qer_editorimage textures/arctic/fog.tga
	qer_trans 0.15
	surfaceparm nonsolid
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm fog
	fogparms ( .0 .0 .0 ) 300
}

textures/arctic/icicle1_1
{
	qer_editorimage textures/arctic/sprite1_1.tga
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/sprite1_1.tga
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/icicle1_2
{
	qer_editorimage textures/arctic/sprite1_2.tga
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/sprite1_2.tga
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/icicle1_3
{
	qer_editorimage textures/arctic/sprite1_3.tga
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/sprite1_3.tga
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/icicle1_4
{
	qer_editorimage textures/arctic/sprite1_4.tga
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/sprite1_4.tga
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/icicle1_5
{
	qer_editorimage textures/arctic/sprite1_5.tga
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/sprite1_5.tga
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/icicle1_6
{
	qer_editorimage textures/arctic/sprite1_6.tga
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/sprite1_6.tga
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/icicle1_6_trans
{
	qer_editorimage textures/arctic/sprite1_6.tga
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm nolightmap
	cull disable

	{
		map textures/arctic/sprite1_6.tga
		blendFunc add
		rgbGen identity
	}
}

textures/arctic/icicle1_7
{
	qer_editorimage textures/arctic/sprite1_7.tga
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/sprite1_7.tga
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/jumppad1_1_concrete
{
	qer_editorimage textures/arctic/jumppad1_3.tga
	q3map_lightimage textures/arctic/jumppad1_1.fx.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		clampmap textures/arctic/jumppad1_1.fx.tga
		tcMod stretch sawtooth .1 6.5 1 1
		rgbGen identity
	}
	{
		map textures/arctic/jumppad1_3.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map textures/arctic/jumppad1_1.blend.tga
		blendfunc GL_ONE GL_ONE
		rgbGen wave inversesawtooth 0.5 0.5 0 0.5
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
}

textures/arctic/jumppad1_1_floor
{
	qer_editorimage textures/arctic/jumppad1_5.tga
	q3map_lightimage textures/arctic/jumppad1_1.fx.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		clampmap textures/arctic/jumppad1_1.fx.tga
		tcMod stretch sawtooth .1 6.5 1 1
		rgbGen identity
	}
	{
		map textures/arctic/jumppad1_5.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map textures/arctic/jumppad1_1.blend.tga
		blendfunc GL_ONE GL_ONE
		rgbGen wave inversesawtooth 0.5 0.5 0 0.5
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
}

textures/arctic/jumppad1_1_snow
{
	qer_editorimage textures/arctic/jumppad1_1.tga
	q3map_lightimage textures/arctic/jumppad1_1.fx.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		clampmap textures/arctic/jumppad1_1.fx.tga
		tcMod stretch sawtooth .1 6.5 1 1
		rgbGen identity
	}
	{
		map textures/arctic/jumppad1_1.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map textures/arctic/jumppad1_1.blend.tga
		blendfunc GL_ONE GL_ONE
		rgbGen wave inversesawtooth 0.5 0.5 0 0.5
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
}

textures/arctic/jumppad1_2_concrete
{
	qer_editorimage textures/arctic/jumppad1_4.tga
	q3map_lightimage textures/arctic/jumppad1_1.fx.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		clampmap textures/arctic/jumppad1_1.fx.tga
		tcMod stretch sawtooth .1 6.5 1 1
		rgbGen identity
	}
	{
		map textures/arctic/jumppad1_4.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map textures/arctic/jumppad1_1.blend.tga
		blendfunc GL_ONE GL_ONE
		rgbGen wave inversesawtooth 0.5 0.5 0 0.5
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
}

textures/arctic/jumppad1_2_floor
{
	qer_editorimage textures/arctic/jumppad1_6.tga
	q3map_lightimage textures/arctic/jumppad1_1.fx.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		clampmap textures/arctic/jumppad1_1.fx.tga
		tcMod stretch sawtooth .1 6.5 1 1
		rgbGen identity
	}
	{
		map textures/arctic/jumppad1_6.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map textures/arctic/jumppad1_1.blend.tga
		blendfunc GL_ONE GL_ONE
		rgbGen wave inversesawtooth 0.5 0.5 0 0.5
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
}

textures/arctic/jumppad1_2_snow
{
	qer_editorimage textures/arctic/jumppad1_2.tga
	q3map_lightimage textures/arctic/jumppad1_1.fx.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		clampmap textures/arctic/jumppad1_1.fx.tga
		tcMod stretch sawtooth .1 6.5 1 1
		rgbGen identity
	}
	{
		map textures/arctic/jumppad1_2.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map textures/arctic/jumppad1_1.blend.tga
		blendfunc GL_ONE GL_ONE
		rgbGen wave inversesawtooth 0.5 0.5 0 0.5
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
}

textures/arctic/light1_1_1k
{
	qer_editorimage textures/arctic/light1_1.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light1_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		tcMod scale 1 1
	}
	{
		map textures/arctic/light1_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light1_2_1k
{
	qer_editorimage textures/arctic/light1_2.tga
	q3map_lightimage textures/arctic/jumppad1_1.fx.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light1_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		tcMod scale 1 1
	}
	{
		map textures/arctic/light1_2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light2_1_1k
{
	qer_editorimage textures/arctic/light2_1.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light2_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light2_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light2_1_1k_pulse
{
	qer_editorimage textures/arctic/light2_1.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light2_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light2_1.blend.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light2_2_1k
{
	qer_editorimage textures/arctic/light2_2.tga
	q3map_lightimage textures/arctic/light2_2.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light2_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light2_2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light2_2_1k_pulse
{
	qer_editorimage textures/arctic/light2_2.tga
	q3map_lightimage textures/arctic/light2_2.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light2_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light2_2.blend.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light3_1_1k
{
	qer_editorimage textures/arctic/light3_1.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light3_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light3_2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light3_1_1k_pulse
{
	qer_editorimage textures/arctic/light3_1.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light3_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light3_1.blend.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light3_1_2k
{
	qer_editorimage textures/arctic/light3_1.tga
	surfaceparm nomarks
	q3map_surfacelight 2000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light3_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light3_2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light3_1_5k
{
	qer_editorimage textures/arctic/light3_1.tga
	surfaceparm nomarks
	q3map_surfacelight 5000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light3_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light3_2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light3_2_1k
{
	qer_editorimage textures/arctic/light3_2.tga
	q3map_lightimage textures/arctic/light3_2.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light3_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light3_2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light3_2_1k_pulse
{
	qer_editorimage textures/arctic/light3_2.tga
	q3map_lightimage textures/arctic/light3_2.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light3_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light3_2.blend.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light4_1_1k
{
	qer_editorimage textures/arctic/light4_1.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light4_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light4_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light4_1_1k_pulse
{
	qer_editorimage textures/arctic/light4_1.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light4_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light4_1.blend.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light4_1_5k
{
	qer_editorimage textures/arctic/light4_1.tga
	surfaceparm nomarks
	q3map_surfacelight 5000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light4_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light4_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// Created by Kanaeda
textures/arctic/light4_1_flicker_2k
{
	qer_editorimage textures/arctic/light4_1.tga
	surfaceparm nomarks
	q3map_surfacelight 2000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light4_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light4_1.blend.tga
		blendFunc add
		rgbGen wave triangle .1 .6 0 3  //change the last number to make it fade faster/slower  
	}
	{
		map textures/arctic/light4_1.blend.tga
		blendFunc add
		rgbGen wave triangle 0 .7 .25 4  //change the last number to make it fade faster/slower  
	}
}

textures/arctic/wall1_20_05k
{
	qer_editorimage textures/arctic/wall1_20.tga
	surfaceparm nomarks
	q3map_surfacelight 500

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/wall1_20.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/wall1_20.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light4_2_1k
{
	qer_editorimage textures/arctic/light4_2.tga
	q3map_lightimage textures/arctic/light4_2.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light4_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light4_2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light4_2_1k_pulse
{
	qer_editorimage textures/arctic/light4_2.tga
	q3map_lightimage textures/arctic/light4_2.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light4_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light4_2.blend.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light4_2_5k
{
	qer_editorimage textures/arctic/light4_2.tga
	surfaceparm nomarks
	q3map_surfacelight 5000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light4_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light4_2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light5_1_1k
{
	qer_editorimage textures/arctic/light5_1.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light5_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light5_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light5_1_1k_pulse
{
	qer_editorimage textures/arctic/light5_1.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light5_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light5_1.blend.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light5_1_2k
{
	qer_editorimage textures/arctic/light5_1.tga
	surfaceparm nomarks
	q3map_surfacelight 2000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light5_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light5_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light7_2_1k
{
	qer_editorimage textures/arctic/light7_2.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light7_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light7_2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light7_2_1k_pulse
{
	qer_editorimage textures/arctic/light7_2.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light7_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light7_2.blend.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light7_2_2k
{
	qer_editorimage textures/arctic/light7_2.tga
	surfaceparm nomarks
	q3map_surfacelight 2000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light7_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light7_2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light7_3_1k
{
	qer_editorimage textures/arctic/light7_3.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light7_3.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light5_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light7_3_1k_fx
{
	qer_editorimage textures/arctic/light7_3.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map textures/arctic/light7_3.fx.tga
		tcmod scale .4 .4
		tcmod scroll 0 .3
		blendFunc GL_ONE GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light7_3.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
}

textures/arctic/light7_3_1k_pulse
{
	qer_editorimage textures/arctic/light7_3.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light7_3.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light5_1.blend.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light7_3_2k
{
	qer_editorimage textures/arctic/light7_3.tga
	surfaceparm nomarks
	q3map_surfacelight 2000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light7_3.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light5_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light8_1_1k
{
	qer_editorimage textures/arctic/light8_1.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light8_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light8_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light8_2_1k
{
	qer_editorimage textures/arctic/light8_2.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light8_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light8_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light8_3_1k
{
	qer_editorimage textures/arctic/light8_3.tga
	q3map_lightimage textures/arctic/light8_2.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light8_3.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light8_2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light8_4_1k
{
	qer_editorimage textures/arctic/light8_4.tga
	q3map_lightimage textures/arctic/light8_2.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light8_4.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light8_2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light9_1_1k
{
	qer_editorimage textures/arctic/light9_1.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light9_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light9_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/light9_1_5k
{
	qer_editorimage textures/arctic/light9_1.tga
	surfaceparm nomarks
	q3map_surfacelight 5000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/light9_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/light9_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/puter1_1_a
{

	{
		map textures/arctic/puter1_1.line.tga
		rgbGen identity
		tcmod scroll 3 1
	}
	{
		map textures/effects/tinfx.tga
		tcGen environment
		blendfunc add
		rgbGen identity
	}
	{
		map textures/arctic/puter1_1.tga
		blendfunc blend
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/arctic/puter1_1.blend.tga
		blendfunc GL_ONE GL_ONE
		rgbGen wave inversesawtooth 0.5 0.5 0 0.5
	}
}

textures/arctic/puter1_1_b
{

	{
		map textures/arctic/puter1_1.static.tga
		rgbGen identity
		tcmod scroll 3 3
	}
	{
		map textures/effects/tinfx.tga
		tcGen environment
		blendfunc add
		rgbGen identity
	}
	{
		map textures/arctic/puter1_1.tga
		blendfunc blend
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/arctic/puter1_1.blend.tga
		blendfunc GL_ONE GL_ONE
		rgbGen wave inversesawtooth 0.5 0.5 0 0.5
	}
}

textures/arctic/puter2_1_a
{

	{
		map textures/arctic/puter1_1.static.tga
		rgbGen identity
		tcmod scroll 3 3
	}
	{
		map textures/effects/tinfx.tga
		tcGen environment
		blendfunc add
		rgbGen identity
	}
	{
		map textures/arctic/puter2_1.tga
		blendfunc blend
	}
	{
		map $lightmap
		blendFunc filter
		rgbGen identity
	}
}

textures/arctic/snowfall_bk
{
	qer_editorimage textures/arctic/snow_bk1.tga
	qer_trans .3
	surfaceparm trans
	surfaceparm nolightmap
	cull none
	deformVertexes wave 20 sin 2 2 .1 0.5

	{
		Map textures/arctic/snow_bk.tga
		blendFunc GL_ONE GL_ONE
		tcMod scale 1 1
		tcMod scroll 0.05 -0.2
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/snowfall_ft
{
	qer_editorimage textures/arctic/snow_ft1.tga
	qer_trans .3
	surfaceparm trans
	surfaceparm nolightmap
	cull none
	deformVertexes wave 20 sin 2 2 .9 0.5

	{
		Map textures/arctic/snow_ft.tga
		blendFunc GL_ONE GL_ONE
		tcMod scale 1 1
		tcMod scroll 0.05 -0.3
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/steam
{
	qer_editorimage textures/arctic/steam1_1.tga
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nolightmap

	{
		Map textures/arctic/steam1_1.tga
		blendFunc GL_ONE GL_ONE
		tcMod scale 1 1
		tcMod scroll 0 4
	}
	{
		Map textures/arctic/steam1_1b.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
		tcMod scale 1 1
		tcMod scroll 0 0
	}
}

textures/arctic/steam_b
{
	qer_editorimage textures/arctic/steam1_1.tga
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nolightmap
	deformvertexes move 0 0 1 sawtooth 0 64 0 .1
	deformvertexes autosprite2

	{
		Map textures/arctic/steam1_1.tga
		blendFunc add
		rgbgen wave inversesawtooth 0 1 0 .1
	}
}

textures/arctic/support5_1
{
	qer_editorimage textures/arctic/support5_1.tga
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/support5_1.tga
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/support5_2
{
	qer_editorimage textures/arctic/support5_2.tga
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/support5_2.tga
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/support5_3
{
	qer_editorimage textures/arctic/support5_3.tga
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/support5_3.tga
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/support5_4
{
	qer_editorimage textures/arctic/support5_4.tga
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/support5_4.tga
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/trim1_10
{
	qer_editorimage textures/arctic/trim1_10.tga
	q3map_lightimage textures/arctic/trim6_1.fx.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map textures/arctic/trim6_1.fx.tga
		tcmod scale .500 0
		tcmod scroll 1 0
		blendFunc GL_ONE GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/trim1_10.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
}

textures/arctic/trim1_9_1k
{
	qer_editorimage textures/arctic/trim1_9.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/trim1_9.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/trim1_9.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/trim1_9_1k_pulse
{
	qer_editorimage textures/arctic/trim1_9.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/trim1_9.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/trim1_9.blend.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/trim1_9_2k
{
	qer_editorimage textures/arctic/trim1_9.tga
	surfaceparm nomarks
	q3map_surfacelight 2000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/trim1_9.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/trim1_9.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/trim4_12
{
	qer_editorimage textures/arctic/trim4_12.tga
	q3map_lightimage textures/arctic/trim6_1.fx.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map textures/arctic/trim6_1.fx.tga
		tcmod scale 100 100
		tcmod scroll 3 0
		blendFunc GL_ONE GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/trim6_1.fx.tga
		tcmod scale .500 0
		tcmod scroll 1 0
		blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
	{
		map textures/arctic/trim4_12.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
}

textures/arctic/trim6_1_50_fx
{
	qer_editorimage textures/arctic/trim6_1.tga
	surfaceparm nomarks
	q3map_surfacelight 50

	{
		map textures/arctic/trim6_1.fx.tga
		tcmod scale 100 100
		tcmod scroll 3 0
		blendFunc GL_ONE GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/trim6_1.fx.tga
		tcmod scale .500 0
		tcmod scroll 1 0
		blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
	{
		map textures/arctic/trim6_1.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
}

textures/arctic/trim6_1_50_fx_b
{
	qer_editorimage textures/arctic/trim6_1.tga
	surfaceparm nomarks
	q3map_surfacelight 50

	{
		map textures/arctic/trim6_1.fx.tga
		tcmod scale .500 0
		tcmod scroll 1 0
		blendFunc GL_ONE GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/trim6_1.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
}

textures/arctic/wall1_18_shiny
{
	qer_editorimage textures/arctic/wall1_18.tga

	{
		map textures/effects/tinfx2.tga
		tcGen environment
		rgbGen identity
	}
	{
		map textures/arctic/wall1_18.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

textures/arctic/wall1_20_1k
{
	qer_editorimage textures/arctic/wall1_20.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/wall1_20.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/wall1_20.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/wall1_20_1k_pulse
{
	qer_editorimage textures/arctic/wall1_20.tga
	surfaceparm nomarks
	q3map_surfacelight 1000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/wall1_20.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/wall1_20.blend.tga
		rgbGen wave sin 0.2 0.8 1 1
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/wall1_20_2k
{
	qer_editorimage textures/arctic/wall1_20.tga
	surfaceparm nomarks
	q3map_surfacelight 2000

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/arctic/wall1_20.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/wall1_20.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/arctic/wall1_3_shiny
{
	qer_editorimage textures/arctic/wall1_3.tga

	{
		map textures/effects/tinfx2.tga
		tcGen environment
		rgbGen identity
	}
	{
		map textures/arctic/wall1_3.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

textures/arctic/wall9_8
{
	qer_editorimage textures/arctic/wall9_8.tga
	surfaceparm nomarks
	q3map_surfacelight 500

	{
		map textures/arctic/trim6_1.fx.tga
		tcmod scale -50 100
		tcmod scroll 1 3
		blendFunc GL_ONE GL_ZERO
		rgbGen identity
	}
	{
		map textures/arctic/trim6_1.fx.tga
		tcmod scale 500 1
		tcmod scroll 0 1
		blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
	{
		map textures/arctic/wall9_8.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
}

textures/arctic/water1_1
{
	qer_editorimage textures/arctic/water1_1.tga
	surfaceparm water
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm nolightmap
	q3map_surfacelight 60
	q3map_lightimage textures/arctic/water1_1.tga

	{
		map textures/arctic/water1_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		tcMod scroll 0.1 0.2
	}
	{
		map textures/arctic/water1_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		tcMod scroll -0.2 0.02
	}
}

textures/arctic/water1_2
{
	qer_editorimage textures/arctic/water1_2.tga
	surfaceparm water
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm nolightmap
	q3map_surfacelight 60
	q3map_lightimage textures/arctic/water1_2.tga

	{
		map textures/arctic/water1_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		tcMod scroll 0.1 0.2
	}
	{
		map textures/arctic/water1_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		tcMod scroll -0.2 0.02
	}
}

textures/arctic/water2_1
{
	qer_editorimage textures/arctic/water2_1.tga
	surfaceparm water
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm nolightmap
	q3map_surfacelight 60
	q3map_lightimage textures/arctic/water2_1.tga

	{
		map textures/arctic/water2_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		tcMod scroll 0.1 0.2
	}
	{
		map textures/arctic/water2_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		tcMod scroll -0.2 0.02
	}
}

textures/arctic/wires1_1
{
	qer_editorimage textures/arctic/wires1_1.TGA
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/wires1_1.TGA
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/wires1_2
{
	qer_editorimage textures/arctic/wires1_2.TGA
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/wires1_2.TGA
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/wires2_1
{
	qer_editorimage textures/arctic/wires2_1.TGA
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/wires2_1.TGA
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/arctic/wires2_2
{
	qer_editorimage textures/arctic/wires2_2.TGA
	surfaceparm trans
	cull none
	nopicmip

	{
		map textures/arctic/wires2_2.TGA
		tcMod scale 1 1
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}
