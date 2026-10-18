

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
	q3gl2_sun .9 .9 1 45 0 70 0.5
	q3map_surfacelight 25
	skyparms env/arcsky/arcsky2 1024 -
	q3r_haze 0.62 0.65 0.74 4000	// Q3Rally: haze of the maps with this sky
	q3r_colorLut gfx/luts/cold	// Q3Rally: colour grading look of the maps with this sky

	{
		map textures/skies/dimclouds.tga
		blendFunc add
		tcMod scroll 0.05 0.05
		tcMod scale 3 3
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

// Created by Kanaeda

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

