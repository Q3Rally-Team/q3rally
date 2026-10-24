// Crossroads V0.2 - outdoor four-team CTF.
// Uses existing Q3Rally images. Compile with -custinfoparms so terrain
// surface flags match baseq3r/scripts/custinfoparms.txt.
// The ground shaders at the end are written by tools/maptools/crossroads_textures.py.

textures/q3r_crossroads/wood
{
	qer_editorimage textures/q3r_building/wood02.jpg
	{
		map textures/q3r_building/wood02.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/roof
{
	qer_editorimage textures/q3r_building/roof01.jpg
	{
		map textures/q3r_building/roof01.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/metal
{
	qer_editorimage textures/base_trim/tin.jpg
	surfaceparm SURF_METAL
	{
		map textures/base_trim/tin.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/concrete
{
	qer_editorimage textures/base_floor/concrete.jpg
	{
		map textures/base_floor/concrete.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// Saturated paint on the four base markers, name bands and team parts
// (light stucco base so the colors read in daylight and shade).
// The normal lightmap pass keeps the paint readable in daylight and shade.

textures/q3r_crossroads/red
{
	qer_editorimage textures/q3r_building/stucco01.jpg
	{
		map textures/q3r_building/stucco01.jpg
		rgbGen const ( 1.0 0.28 0.20 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/blue
{
	qer_editorimage textures/q3r_building/stucco01.jpg
	{
		map textures/q3r_building/stucco01.jpg
		rgbGen const ( 0.25 0.50 1.0 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/green
{
	qer_editorimage textures/q3r_building/stucco01.jpg
	{
		map textures/q3r_building/stucco01.jpg
		rgbGen const ( 0.30 0.85 0.38 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/yellow
{
	qer_editorimage textures/q3r_building/stucco01.jpg
	{
		map textures/q3r_building/stucco01.jpg
		rgbGen const ( 1.0 0.80 0.18 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// Shallow, drivable brook. No vertex deformation or slick flag.

textures/q3r_crossroads/rock
{
	qer_editorimage textures/q3r_dirt_grass/rock03.tga
	q3map_shadeAngle 70
	{
		map textures/q3r_dirt_grass/rock03.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// Clear, shallow creek: OpenGL2 draws a rippled reflection (q3r_water),
// OpenGL1 / r_water 0 the two scrolling stages. No slick flag, the fords stay
// drivable; deep water slows the cars through the normal water drag.
textures/q3r_crossroads/water
{
	qer_editorimage textures/q3r_liquids/water.tga
	qer_trans 0.35
	surfaceparm water
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_globaltexture
	cull disable
	q3r_water textures/q3r_liquids/water_normal.tga 240 0.025 0.022
	q3r_waterColor 0.10 0.15 0.12 0.30
	q3r_waterFresnel 0.06 4
	q3r_waterSpecular 0.9
	{
		map textures/q3r_liquids/water.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen const ( 0.55 0.72 0.70 )
		alphaGen const 0.30
		tcMod scale 0.6 0.6
		tcMod scroll 0.012 0.018
	}
	{
		map textures/q3r_liquids/water.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen const ( 0.78 0.87 0.89 )
		alphaGen const 0.10
		tcMod scale 0.7 0.7
		tcMod scroll -0.009 0.011
	}
}

textures/q3r_crossroads/sky
{
	qer_editorimage textures/q3r_skies/env/trainyard/trainyard_ft.tga
	surfaceparm sky
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_sun 1.0 0.97 0.91 230 310 58
	q3map_skylight 85 3
	q3gl2_sun 1.0 0.97 0.91 160 310 58 0.5
	skyparms textures/q3r_skies/env/trainyard/trainyard - -
	q3r_haze 0.66 0.72 0.80 5200	// Q3Rally: light summer haze over the hills
}

// Invisible perimeter behind the visible fence. Keep the default SOLID
// contents: playerclip-only brushes are omitted from Bullet's static world.
// Translucent compile flags let skylight pass and preserve the landscape view.

textures/q3r_crossroads/boundary
{
	qer_editorimage textures/base_floor/concrete.jpg
	qer_trans 0.15
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm noimpact
	surfaceparm detail
}

// Unreachable surrounding hills use a coarser lightmap than the play area.

// Unreachable surrounding hills use a coarser lightmap than the play area.
textures/q3r_crossroads/backdrop
{
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_lightmapSampleSize 64
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/grass07.tga
		tcGen vector ( 0.0029296875 0 0 ) ( 0 0.0029296875 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00015625 0 0 ) ( 0 0.00015625 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// Team paint on corrugated sheet metal (roofs of the bases).
textures/q3r_crossroads/paint_red
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.78 0.24 0.18 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/paint_blue
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.22 0.38 0.80 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/paint_green
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.30 0.55 0.30 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/paint_yellow
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.95 0.74 0.18 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// ---------------------------------------------------------------- ground
// Written by tools/maptools/crossroads_textures.py. The terrain brushes use
// the t_* parents (collision, tyre friction). q3map2 swaps the drawn
// surfaces for ter_<layer> or ter_<a>to<b> from maps/q3r_crossroads_index.pcx
// and sets the vertex alpha of the blend. Layers: 0 grass, 1 dirt, 2 gravel, 3 pebbles, 4 drygrass, 5 concrete

textures/q3r_crossroads/t_grass
{
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	surfaceparm SURF_GRASS
}

textures/q3r_crossroads/t_dirt
{
	qer_editorimage textures/q3r_dirt_grass/dirt02.tga
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	surfaceparm SURF_DIRT
}

textures/q3r_crossroads/t_gravel
{
	qer_editorimage textures/q3r_dirt_grass/stones01.tga
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	surfaceparm SURF_GRAVEL
}

textures/q3r_crossroads/t_plain
{
	qer_editorimage textures/base_floor/concrete.jpg
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
}

textures/q3r_crossroads/ter_0
{
	// grass
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/grass07.tga
		tcGen vector ( 0.00585938 0 0 ) ( 0 0.00585938 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_1
{
	// dirt
	qer_editorimage textures/q3r_dirt_grass/dirt02.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/dirt02.tga
		tcGen vector ( 0.0078125 0 0 ) ( 0 0.0078125 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_2
{
	// gravel
	qer_editorimage textures/q3r_dirt_grass/stones01.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/stones01.tga
		tcGen vector ( 0.0078125 0 0 ) ( 0 0.0078125 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_3
{
	// pebbles
	qer_editorimage textures/q3r_ground/pebble01.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_ground/pebble01.jpg
		tcGen vector ( 0.00488281 0 0 ) ( 0 0.00488281 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_4
{
	// drygrass
	qer_editorimage textures/q3r_dirt_grass/grass09.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/grass09.tga
		tcGen vector ( 0.00585938 0 0 ) ( 0 0.00585938 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_5
{
	// concrete
	qer_editorimage textures/base_floor/concrete.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/base_floor/concrete.jpg
		tcGen vector ( 0.00292969 0 0 ) ( 0 0.00292969 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_0to1
{
	// grass to dirt
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/grass07.tga
		tcGen vector ( 0.00585938 0 0 ) ( 0 0.00585938 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/dirt02.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0078125 0 0 ) ( 0 0.0078125 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_0to2
{
	// grass to gravel
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/grass07.tga
		tcGen vector ( 0.00585938 0 0 ) ( 0 0.00585938 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/stones01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0078125 0 0 ) ( 0 0.0078125 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_0to3
{
	// grass to pebbles
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/grass07.tga
		tcGen vector ( 0.00585938 0 0 ) ( 0 0.00585938 0 )
		rgbGen identity
	}
	{
		map textures/q3r_ground/pebble01.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.00488281 0 0 ) ( 0 0.00488281 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_0to4
{
	// grass to drygrass
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/grass07.tga
		tcGen vector ( 0.00585938 0 0 ) ( 0 0.00585938 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/grass09.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.00585938 0 0 ) ( 0 0.00585938 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_0to5
{
	// grass to concrete
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/grass07.tga
		tcGen vector ( 0.00585938 0 0 ) ( 0 0.00585938 0 )
		rgbGen identity
	}
	{
		map textures/base_floor/concrete.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.00292969 0 0 ) ( 0 0.00292969 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_1to2
{
	// dirt to gravel
	qer_editorimage textures/q3r_dirt_grass/dirt02.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/dirt02.tga
		tcGen vector ( 0.0078125 0 0 ) ( 0 0.0078125 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/stones01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0078125 0 0 ) ( 0 0.0078125 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_1to3
{
	// dirt to pebbles
	qer_editorimage textures/q3r_dirt_grass/dirt02.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/dirt02.tga
		tcGen vector ( 0.0078125 0 0 ) ( 0 0.0078125 0 )
		rgbGen identity
	}
	{
		map textures/q3r_ground/pebble01.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.00488281 0 0 ) ( 0 0.00488281 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_1to4
{
	// dirt to drygrass
	qer_editorimage textures/q3r_dirt_grass/dirt02.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/dirt02.tga
		tcGen vector ( 0.0078125 0 0 ) ( 0 0.0078125 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/grass09.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.00585938 0 0 ) ( 0 0.00585938 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_1to5
{
	// dirt to concrete
	qer_editorimage textures/q3r_dirt_grass/dirt02.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/dirt02.tga
		tcGen vector ( 0.0078125 0 0 ) ( 0 0.0078125 0 )
		rgbGen identity
	}
	{
		map textures/base_floor/concrete.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.00292969 0 0 ) ( 0 0.00292969 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_2to3
{
	// gravel to pebbles
	qer_editorimage textures/q3r_dirt_grass/stones01.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/stones01.tga
		tcGen vector ( 0.0078125 0 0 ) ( 0 0.0078125 0 )
		rgbGen identity
	}
	{
		map textures/q3r_ground/pebble01.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.00488281 0 0 ) ( 0 0.00488281 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_2to4
{
	// gravel to drygrass
	qer_editorimage textures/q3r_dirt_grass/stones01.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/stones01.tga
		tcGen vector ( 0.0078125 0 0 ) ( 0 0.0078125 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/grass09.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.00585938 0 0 ) ( 0 0.00585938 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_2to5
{
	// gravel to concrete
	qer_editorimage textures/q3r_dirt_grass/stones01.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/stones01.tga
		tcGen vector ( 0.0078125 0 0 ) ( 0 0.0078125 0 )
		rgbGen identity
	}
	{
		map textures/base_floor/concrete.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.00292969 0 0 ) ( 0 0.00292969 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_3to4
{
	// pebbles to drygrass
	qer_editorimage textures/q3r_ground/pebble01.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_ground/pebble01.jpg
		tcGen vector ( 0.00488281 0 0 ) ( 0 0.00488281 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/grass09.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.00585938 0 0 ) ( 0 0.00585938 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_3to5
{
	// pebbles to concrete
	qer_editorimage textures/q3r_ground/pebble01.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_ground/pebble01.jpg
		tcGen vector ( 0.00488281 0 0 ) ( 0 0.00488281 0 )
		rgbGen identity
	}
	{
		map textures/base_floor/concrete.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.00292969 0 0 ) ( 0 0.00292969 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_crossroads/ter_4to5
{
	// drygrass to concrete
	qer_editorimage textures/q3r_dirt_grass/grass09.tga
	q3map_shadeAngle 120
	q3map_nonplanar
	q3map_lightmapAxis z
	{
		map textures/q3r_dirt_grass/grass09.tga
		tcGen vector ( 0.00585938 0 0 ) ( 0 0.00585938 0 )
		rgbGen identity
	}
	{
		map textures/base_floor/concrete.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.00292969 0 0 ) ( 0 0.00292969 0 )
		rgbGen identity
	}
	{
		map textures/q3r_crossroads/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023437 0 0 ) ( 0 0.00023437 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}
