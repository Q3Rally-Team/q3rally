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

// ------------------------------------------------------------
// Pool in the middle of the map. The top is reflective water
// (OpenGL2 with r_water 1: q3r_water, see liquids.shader); with
// OpenGL1 or r_water 0 it falls back to the stages below, the old
// calm_poollight look. No deformVertexes or tesssize, the mirror
// needs a flat surface. The other faces of the pool brushes keep
// the old shader (pool_side), otherwise a wall of the pool could
// become the mirror plane.
// ------------------------------------------------------------

textures/q3r_ctf_04/pool
{
	qer_editorimage textures/liquids/pool3d_4b.tga
	qer_trans .75
	q3map_globaltexture
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm SURF_WET
	surfaceparm nolightmap
	q3map_surfacelight 50
	cull disable
	q3r_water textures/q3r_liquids/water_normal.tga 192 0.025 0.02
	q3r_waterColor 0.04 0.13 0.11 0.45
	q3r_waterFresnel 0.08 4
	q3r_waterSpecular 1.2
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
}

textures/q3r_ctf_04/pool_side
{
	qer_editorimage textures/liquids/pool3d_4b.tga
	qer_trans .75
	q3map_globaltexture
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
}
