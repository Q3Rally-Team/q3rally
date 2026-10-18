// ------------------------------------------------------------
// Shaders for q3r_ctf_04 that were missing from baseq3r (restored from
// the Q3Rally mapping kit). oasisnight sky by Amethyst7, see
// env/oasisnight/readme.txt.
// ------------------------------------------------------------

textures/skies/oasisnight
{
	qer_editorimage env/oasisnight/oasisnight_ft.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_surfacelight 30
	q3map_sun 1.0 0.95 0.8 40 270 25
	skyParms env/oasisnight/oasisnight - -
	q3r_haze 0.12 0.12 0.20 4000	// Q3Rally: haze of the maps with this sky
	q3r_colorLut gfx/luts/night	// Q3Rally: colour grading look of the maps with this sky
}

models/mapobjects/spotlamp1/spotlamp
{
	qer_editorimage models/mapobjects/spotlamp/spotlamp.tga
	cull disable
	surfaceparm alphashadow
	{
		map models/mapobjects/spotlamp/spotlamp.tga
		alphaFunc GE128
		depthWrite
		rgbGen vertex
	}
}

models/mapobjects/spotlamp1/beam
{
	qer_editorimage models/mapobjects/spotlamp/beam.jpg
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm nolightmap
	cull none
	nomipmaps
	{
		map models/mapobjects/spotlamp/beam.jpg
		tcMod scroll .3 0
		blendFunc GL_ONE GL_ONE
	}
}

models/mapobjects/spotlamp1/spotlamp_l
{
	qer_editorimage models/mapobjects/spotlamp/spotlamp_l.jpg
	cull disable
	q3map_surfacelight 200
	{
		map models/mapobjects/spotlamp/spotlamp_l.jpg
		rgbGen identity
	}
}

textures/tech1soc_floor/grate2b_trans
{
	qer_editorimage textures/tech1soc_floor/grate2b.tga
	surfaceparm metalsteps
	cull none
	{
		map textures/tech1soc_floor/grate2b.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
		rgbGen identity
	}
}
