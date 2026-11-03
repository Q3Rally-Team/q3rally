// Jungle Opening (q3r_david4) - CTF in a jungle clearing at night.
// Written by tools/maptools/make_david4.py. Compile with -custinfoparms.

textures/q3r_david4/sky
{
	qer_editorimage textures/q3r_skies/env/ame_nebula/purplenebula_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_globaltexture
	// moonlight from the nebula, tilted for real-time shadows
	q3map_sun 0.78 0.82 1.00 150 68 55
	q3map_skylight 95 3
	q3gl2_sun 0.78 0.82 1.00 70 68 55 0.5
	skyparms textures/q3r_skies/env/ame_nebula/purplenebula - -
	q3r_haze 0.09 0.11 0.14 9000	// Q3Rally: night mist over the clearing
	q3r_colorLut gfx/luts/night	// Q3Rally: colour grading
}

textures/q3r_david4/forest
{
	qer_editorimage textures/q3r_trees/pinewall01a.tga
	surfaceparm nonsolid
	surfaceparm nolightmap
	surfaceparm nomarks
	{
		map textures/q3r_trees/pinewall01a.tga
		rgbGen const ( 0.30 0.34 0.36 )
	}
}

textures/q3r_david4/backdrop
{
	qer_editorimage textures/q3r_dirt_grass/grass01.tga
	q3map_lightmapSampleSize 64
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	surfaceparm SURF_GRASS
	{
		map textures/q3r_dirt_grass/grass01.tga
		tcGen vector ( 0.0029296875 0 0 ) ( 0 0.0029296875 0 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ramp_red
{
	qer_editorimage textures/q3r_metals/Rost.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/Rost.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ramp_blue
{
	qer_editorimage textures/q3r_metals/rustblumet01.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/rustblumet01.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/deck_red
{
	qer_editorimage textures/q3r_metals/Rost.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/Rost.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/deck_blue
{
	qer_editorimage textures/q3r_metals/rustblumet01.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/rustblumet01.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ramp_stone
{
	qer_editorimage textures/q3r_stone/Stein2.tga
	q3map_shadeAngle 50
	{
		map textures/q3r_stone/Stein2.tga
		rgbGen const ( 0.8 0.78 0.74 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/trim
{
	qer_editorimage textures/base_trim/yellow_rustb.tga
	{
		map textures/base_trim/yellow_rustb.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/pad_red
{
	qer_editorimage textures/ctf/red_telep.jpg
	surfaceparm nomarks
	q3map_surfacelight 120
	q3map_lightRGB 1.0 0.35 0.2
	{
		clampMap textures/ctf/red_telep.jpg
		tcMod rotate 40
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		rgbGen identity
	}
}

textures/q3r_david4/glow_red
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_surfacelight 600
	q3map_lightRGB 1.0 0.35 0.2
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 1.0 0.35 0.2 )
	}
}

textures/q3r_david4/border_red
{
	qer_editorimage textures/q3r_signage/dirsign01_red.tga
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm trans
	polygonOffset
	{
		map textures/q3r_signage/dirsign01_red.tga
		blendFunc blend
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/paint_red
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.85 0.22 0.16 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/sign_red
{
	qer_editorimage textures/ctf/test2_r.tga
	{
		map textures/ctf/test2_r.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/pad_blue
{
	qer_editorimage textures/ctf/blue_telep.jpg
	surfaceparm nomarks
	q3map_surfacelight 120
	q3map_lightRGB 0.3 0.5 1.0
	{
		clampMap textures/ctf/blue_telep.jpg
		tcMod rotate 40
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		rgbGen identity
	}
}

textures/q3r_david4/glow_blue
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_surfacelight 600
	q3map_lightRGB 0.3 0.5 1.0
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 0.3 0.5 1.0 )
	}
}

textures/q3r_david4/border_blue
{
	qer_editorimage textures/q3r_signage/dirsign01_blu.tga
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm trans
	polygonOffset
	{
		map textures/q3r_signage/dirsign01_blu.tga
		blendFunc blend
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/paint_blue
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.22 0.36 0.85 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/sign_blue
{
	qer_editorimage textures/ctf/test2.tga
	{
		map textures/ctf/test2.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/stone
{
	qer_editorimage textures/q3r_stone/Stein2.tga
	q3map_shadeAngle 40
	{
		map textures/q3r_stone/Stein2.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/slab
{
	qer_editorimage textures/q3r_stone/Stein2.tga
	q3map_shadeAngle 60
	{
		map textures/q3r_stone/Stein2.tga
		rgbGen const ( 0.55 0.55 0.52 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/rock
{
	qer_editorimage textures/q3r_dirt_grass/rock01.tga
	q3map_shadeAngle 70
	{
		map textures/q3r_dirt_grass/rock01.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/concrete
{
	qer_editorimage textures/q3r_building/concrete02.jpg
	{
		map textures/q3r_building/concrete02.jpg
		rgbGen const ( 0.52 0.52 0.5 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/roof
{
	qer_editorimage textures/q3r_metals/cormetal03.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/cormetal03.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/metal
{
	qer_editorimage textures/q3r_metals/metrust01.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/metrust01.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/pipe
{
	qer_editorimage textures/base_trim/rusty_pewter_big.tga
	surfaceparm SURF_METAL
	q3map_shadeAngle 60
	{
		map textures/base_trim/rusty_pewter_big.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/bark
{
	qer_editorimage models/mapobjects/oak/oakstamm.jpg
	{
		map models/mapobjects/oak/oakstamm.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/struts
{
	qer_editorimage textures/q3r_metals/towerstruts.tga
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm alphashadow
	cull none
	{
		map textures/q3r_metals/towerstruts.tga
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

textures/q3r_david4/ladder
{
	qer_editorimage textures/q3r_metals/ladder01.tga
	surfaceparm nonsolid
	surfaceparm trans
	cull none
	{
		map textures/q3r_metals/ladder01.tga
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

textures/q3r_david4/lamp
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_surfacelight 400
	q3map_lightRGB 1.0 0.9 0.72
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 1.0 0.9 0.72 )
	}
}

textures/q3r_david4/dark
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nolightmap
	surfaceparm nonsolid
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 0.02 0.02 0.02 )
	}
}

textures/q3r_david4/water
{
	qer_editorimage textures/q3r_liquids/water.tga
	qer_trans 0.35
	// shallow: drawn as water, but no liquid contents (bots treat the bed as ground)
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_globaltexture
	cull disable
	q3r_water textures/q3r_liquids/water_normal.tga 240 0.025 0.022
	q3r_waterColor 0.05 0.09 0.08 0.40
	q3r_waterFresnel 0.06 4
	q3r_waterSpecular 0.9
	{
		map textures/q3r_liquids/water.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen const ( 0.30 0.42 0.42 )
		alphaGen const 0.40
		tcMod scale 0.6 0.6
		tcMod scroll 0.0 -0.03
	}
	{
		map textures/q3r_liquids/water.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen const ( 0.55 0.62 0.70 )
		alphaGen const 0.12
		tcMod scale 0.7 0.7
		tcMod scroll 0.01 -0.045
	}
}

textures/q3r_david4/waterfall
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
		rgbGen const ( 0.30 0.36 0.40 )
		alphaGen const 0.75
		tcMod turb 0 0.012 0 0.6
		tcMod scroll 0 -1.1
	}
	{
		map textures/q3r_liquids/waterfall_streaks.tga
		blendFunc GL_SRC_ALPHA GL_ONE
		rgbGen const ( 0.35 0.42 0.50 )
		tcMod scroll 0 -2.4
	}
	{
		map textures/q3r_liquids/waterfall_streaks.tga
		blendFunc GL_SRC_ALPHA GL_ONE
		rgbGen const ( 0.22 0.28 0.34 )
		tcMod scale 1.3 0.8
		tcMod scroll 0.06 -1.7
	}
}

textures/q3r_david4/boundary
{
	qer_editorimage textures/base_floor/concrete.jpg
	qer_trans 0.15
	// invisible but SOLID (Bullet only takes solid brushes); trans lets light through
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm noimpact
	surfaceparm detail
}

// ground: t_* parents on the brush sides (collision, friction); q3map2 draws
// ter_<layer> / ter_<a>to<b> from maps/q3r_david4_index.pcx. Layers: 0 grass, 1 moss, 2 dirt, 3 rock, 4 pebble

textures/q3r_david4/t_grass
{
	qer_editorimage textures/q3r_dirt_grass/grass01.tga
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	surfaceparm SURF_GRASS
}

textures/q3r_david4/t_dirt
{
	qer_editorimage textures/q3r_dirt_grass/darkdirt01.tga
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	surfaceparm SURF_DIRT
}

textures/q3r_david4/t_gravel
{
	qer_editorimage textures/q3r_dirt_grass/stones02.tga
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	surfaceparm SURF_GRAVEL
}

textures/q3r_david4/t_plain
{
	qer_editorimage textures/q3r_dirt_grass/rock01.tga
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
}

textures/q3r_david4/ter_0
{
	qer_editorimage textures/q3r_dirt_grass/grass01.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/grass01.tga
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ter_1
{
	qer_editorimage textures/q3r_dirt_grass/darkdirtgrass01.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/darkdirtgrass01.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ter_2
{
	qer_editorimage textures/q3r_dirt_grass/darkdirt01.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/darkdirt01.tga
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ter_3
{
	qer_editorimage textures/q3r_dirt_grass/rock01.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/rock01.tga
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ter_4
{
	qer_editorimage textures/q3r_dirt_grass/stones02.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/stones02.tga
		tcGen vector ( 0.0062500 0 0 ) ( 0 0.0062500 0 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ter_0to1
{
	qer_editorimage textures/q3r_dirt_grass/grass01.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/grass01.tga
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/darkdirtgrass01.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ter_0to2
{
	qer_editorimage textures/q3r_dirt_grass/grass01.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/grass01.tga
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/darkdirt01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ter_0to3
{
	qer_editorimage textures/q3r_dirt_grass/grass01.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/grass01.tga
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/rock01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ter_0to4
{
	qer_editorimage textures/q3r_dirt_grass/grass01.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/grass01.tga
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/stones02.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0062500 0 0 ) ( 0 0.0062500 0 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ter_1to2
{
	qer_editorimage textures/q3r_dirt_grass/darkdirtgrass01.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/darkdirtgrass01.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/darkdirt01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ter_1to3
{
	qer_editorimage textures/q3r_dirt_grass/darkdirtgrass01.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/darkdirtgrass01.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/rock01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ter_1to4
{
	qer_editorimage textures/q3r_dirt_grass/darkdirtgrass01.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/darkdirtgrass01.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/stones02.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0062500 0 0 ) ( 0 0.0062500 0 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ter_2to3
{
	qer_editorimage textures/q3r_dirt_grass/darkdirt01.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/darkdirt01.tga
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/rock01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ter_2to4
{
	qer_editorimage textures/q3r_dirt_grass/darkdirt01.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/darkdirt01.tga
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/stones02.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0062500 0 0 ) ( 0 0.0062500 0 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_david4/ter_3to4
{
	qer_editorimage textures/q3r_dirt_grass/rock01.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/rock01.tga
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/stones02.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0062500 0 0 ) ( 0 0.0062500 0 )
		rgbGen identity
	}
	{
		map textures/q3r_david4/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00021 0 0 ) ( 0 0.00021 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}
