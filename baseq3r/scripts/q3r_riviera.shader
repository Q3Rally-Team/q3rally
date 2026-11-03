// Riviera Run - A to B sprint along a Mediterranean coast at sunset.
// Written by tools/maptools/make_riviera.py. Compile with -custinfoparms.

textures/q3r_riviera/sky
{
	qer_editorimage env/ely_sunset/sunset_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_globaltexture
	q3map_sun 1 0.66 0.42 170 350 12
	q3map_skylight 80 4
	q3gl2_sun 1 0.66 0.42 120 350 12 0.6
	skyparms env/ely_sunset/sunset - -
	q3r_haze 0.62 0.48 0.44 7500	// Q3Rally: warm evening haze over the sea
}

textures/q3r_riviera/sea
{
	qer_editorimage textures/q3r_liquids/water.tga
	qer_trans 0.35
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_globaltexture
	cull disable
	q3r_water textures/q3r_liquids/water_normal.tga 320 0.02 0.018
	q3r_waterColor 0.05 0.16 0.22 0.70
	q3r_waterFresnel 0.10 4
	q3r_waterSpecular 1.0
	{
		map textures/q3r_liquids/water.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen const ( 0.14 0.30 0.38 )
		alphaGen const 0.82
		tcMod scale 0.25 0.25
		tcMod scroll 0.006 0.009
	}
	{
		map textures/q3r_liquids/water.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen const ( 0.95 0.62 0.42 )
		alphaGen const 0.14
		tcMod scale 0.33 0.33
		tcMod scroll -0.005 0.007
	}
}

// palm models (models/mapobjects/palmier) name a shader that does not exist; the map remaps them
models/mapobjects/palmier/palmier1
{
	qer_editorimage models/mapobjects/palmier/palmier1.tga
	surfaceparm nomarks
	surfaceparm alphashadow
	cull none
	{
		map models/mapobjects/palmier/palmier1.tga
		alphaFunc GE128
		depthWrite
		rgbGen vertex
	}
}

textures/q3r_riviera/metal
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

textures/q3r_riviera/rail
{
	qer_editorimage textures/q3r_metals/plate01.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/plate01.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/railpost
{
	qer_editorimage textures/base_trim/tin.jpg
	surfaceparm SURF_METAL
	{
		map textures/base_trim/tin.jpg
		rgbGen const ( 0.60 0.60 0.62 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/stonewall
{
	qer_editorimage textures/q3r_dirt_grass/redrock03.tga
	q3map_lightmapSampleSize 24
	{
		map textures/q3r_dirt_grass/redrock03.tga
		rgbGen const ( 0.95 0.88 0.78 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/parapet
{
	qer_editorimage textures/q3r_dirt_grass/redrock03.tga
	surfaceparm trans
	{
		map textures/q3r_dirt_grass/redrock03.tga
		rgbGen const ( 1.00 0.94 0.84 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/quaywall
{
	qer_editorimage textures/q3r_stone/Stein2.tga
	{
		map textures/q3r_stone/Stein2.tga
		rgbGen const ( 0.95 0.90 0.82 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/pavement
{
	qer_editorimage textures/q3r_road/cobbles01.jpg
	{
		map textures/q3r_road/cobbles01.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/kerbstone
{
	qer_editorimage textures/q3r_road/concrete2.jpg
	{
		map textures/q3r_road/concrete2.jpg
		rgbGen const ( 0.85 0.83 0.80 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/deck
{
	qer_editorimage textures/q3r_road/asph02.jpg
	q3map_nonplanar
	q3map_lightmapMergable
	q3map_lightmapSampleOffset 4
	q3map_shadeAngle 30
	surfaceparm SURF_ASPHALT
	{
		map textures/q3r_road/asph02.jpg
		tcGen vector ( 0.0039063 0 0 ) ( 0 0.0039063 0 )
		rgbGen const ( 0.60 0.60 0.62 )
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/tunnelwall
{
	qer_editorimage textures/q3r_building/concrete02.jpg
	q3map_lightmapSampleSize 24
	{
		map textures/q3r_building/concrete02.jpg
		rgbGen const ( 0.85 0.82 0.78 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/portal
{
	qer_editorimage textures/q3r_dirt_grass/redrock03.tga
	{
		map textures/q3r_dirt_grass/redrock03.tga
		rgbGen const ( 0.90 0.84 0.74 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/rockmass
{
	qer_editorimage textures/q3r_dirt_grass/rock03.tga
	q3map_shadeAngle 70
	q3map_lightmapSampleSize 32
	{
		map textures/q3r_dirt_grass/rock03.tga
		tcGen vector ( 0.0034722 0 0 ) ( 0 0.0034722 0 )
		rgbGen const ( 0.92 0.88 0.80 )
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/tunnellamp
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm nonsolid
	q3map_surfacelight 400
	q3map_lightRGB 1.0 0.85 0.6
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 1.0 0.85 0.6 )
	}
}

textures/q3r_riviera/boulder
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

textures/q3r_riviera/white
{
	qer_editorimage textures/q3r_building/stucco01.jpg
	{
		map textures/q3r_building/stucco01.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/redband
{
	qer_editorimage textures/q3r_building/stucco01.jpg
	{
		map textures/q3r_building/stucco01.jpg
		rgbGen const ( 0.85 0.18 0.14 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/lantern
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm trans
	q3map_surfacelight 600
	q3map_lightRGB 1 0.9 0.7
	{
		map textures/q3r_signage/white.jpg
		rgbGen wave sin 0.75 0.25 0 0.2
	}
}

textures/q3r_riviera/housea
{
	qer_editorimage textures/q3r_building/stuccowin01.jpg
	{
		map textures/q3r_building/stuccowin01.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/houseb
{
	qer_editorimage textures/q3r_building/stuccowin01.jpg
	{
		map textures/q3r_building/stuccowin01.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/housec
{
	qer_editorimage textures/q3r_building/stuccowin03.jpg
	{
		map textures/q3r_building/stuccowin03.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/housed
{
	qer_editorimage textures/q3r_building/stuccowin04.jpg
	{
		map textures/q3r_building/stuccowin04.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/houseside
{
	qer_editorimage textures/q3r_building/stucco02.jpg
	{
		map textures/q3r_building/stucco02.jpg
		rgbGen const ( 0.74 0.68 0.60 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/house
{
	qer_editorimage textures/q3r_building/stuccowin02.jpg
	{
		map textures/q3r_building/stuccowin02.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/roof
{
	qer_editorimage textures/q3r_building/roof3.jpg
	{
		map textures/q3r_building/roof3.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/rooftiles
{
	qer_editorimage textures/q3r_building/roof3.jpg
	surfaceparm nonsolid
	{
		map textures/q3r_building/roof3.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/bollard
{
	qer_editorimage textures/q3r_metals/plate01.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/plate01.tga
		rgbGen const ( 0.35 0.35 0.38 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/hullblue
{
	qer_editorimage textures/q3r_building/stucco01.jpg
	surfaceparm nonsolid
	{
		map textures/q3r_building/stucco01.jpg
		rgbGen const ( 0.25 0.45 0.75 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/hullred
{
	qer_editorimage textures/q3r_building/stucco01.jpg
	surfaceparm nonsolid
	{
		map textures/q3r_building/stucco01.jpg
		rgbGen const ( 0.80 0.25 0.18 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/boatdeck
{
	qer_editorimage textures/q3r_wood/Holz.jpg
	surfaceparm nonsolid
	{
		map textures/q3r_wood/Holz.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/mastwood
{
	qer_editorimage textures/q3r_wood/Holz.jpg
	surfaceparm nonsolid
	{
		map textures/q3r_wood/Holz.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/chevron_r
{
	qer_editorimage textures/q3r_riviera/chevron_r.jpg
	{
		map textures/q3r_riviera/chevron_r.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/chevron_l
{
	qer_editorimage textures/q3r_riviera/chevron_l.jpg
	{
		map textures/q3r_riviera/chevron_l.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/banner_start
{
	qer_editorimage textures/q3r_riviera/banner_start.jpg
	{
		map textures/q3r_riviera/banner_start.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/banner_finish
{
	qer_editorimage textures/q3r_riviera/banner_finish.jpg
	{
		map textures/q3r_riviera/banner_finish.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/boundary
{
	qer_editorimage textures/base_floor/concrete.jpg
	qer_trans 0.15
	// invisible but SOLID; trans lets light through
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm noimpact
	surfaceparm detail
}

// ground: t_* parents on the brush sides (collision, friction); q3map2 draws
// ter_<layer> / ter_<a>to<b> from maps/q3r_riviera_index.pcx. Layers: 0 grass, 1 gravel, 2 asphalt, 3 rock, 4 cliff, 5 sand

textures/q3r_riviera/t_grass
{
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	surfaceparm SURF_GRASS
}

textures/q3r_riviera/t_gravel
{
	qer_editorimage textures/q3r_dirt_grass/stones01.tga
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	surfaceparm SURF_GRAVEL
}

textures/q3r_riviera/t_asphalt
{
	qer_editorimage textures/q3r_road/asph02.jpg
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	surfaceparm SURF_ASPHALT
}

textures/q3r_riviera/t_rock
{
	qer_editorimage textures/q3r_dirt_grass/rock03.tga
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
}

textures/q3r_riviera/t_sand
{
	qer_editorimage textures/dip_valley/sand.jpg
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	surfaceparm SURF_SAND
}

textures/q3r_riviera/ter_0
{
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/grass07.tga
		tcGen vector ( 0.0031250 0 0 ) ( 0 0.0031250 0 )
		rgbGen identity
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_1
{
	qer_editorimage textures/q3r_dirt_grass/stones01.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/stones01.tga
		tcGen vector ( 0.0062500 0 0 ) ( 0 0.0062500 0 )
		rgbGen identity
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_2
{
	qer_editorimage textures/q3r_road/asph02.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_road/asph02.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen const ( 0.60 0.60 0.62 )
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_3
{
	qer_editorimage textures/q3r_dirt_grass/rock03.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/rock03.tga
		tcGen vector ( 0.0034722 0 0 ) ( 0 0.0034722 0 )
		rgbGen const ( 0.92 0.88 0.80 )
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_4
{
	qer_editorimage textures/q3r_dirt_grass/rock03.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/rock03.tga
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen const ( 0.92 0.86 0.76 )
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_5
{
	qer_editorimage textures/dip_valley/sand.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/dip_valley/sand.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_0to1
{
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/grass07.tga
		tcGen vector ( 0.0031250 0 0 ) ( 0 0.0031250 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/stones01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0062500 0 0 ) ( 0 0.0062500 0 )
		rgbGen identity
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_0to2
{
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/grass07.tga
		tcGen vector ( 0.0031250 0 0 ) ( 0 0.0031250 0 )
		rgbGen identity
	}
	{
		map textures/q3r_road/asph02.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen const ( 0.60 0.60 0.62 )
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_0to3
{
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/grass07.tga
		tcGen vector ( 0.0031250 0 0 ) ( 0 0.0031250 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/rock03.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0034722 0 0 ) ( 0 0.0034722 0 )
		rgbGen const ( 0.92 0.88 0.80 )
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_0to4
{
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/grass07.tga
		tcGen vector ( 0.0031250 0 0 ) ( 0 0.0031250 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/rock03.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen const ( 0.92 0.86 0.76 )
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_0to5
{
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/grass07.tga
		tcGen vector ( 0.0031250 0 0 ) ( 0 0.0031250 0 )
		rgbGen identity
	}
	{
		map textures/dip_valley/sand.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_1to2
{
	qer_editorimage textures/q3r_dirt_grass/stones01.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/stones01.tga
		tcGen vector ( 0.0062500 0 0 ) ( 0 0.0062500 0 )
		rgbGen identity
	}
	{
		map textures/q3r_road/asph02.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen const ( 0.60 0.60 0.62 )
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_1to3
{
	qer_editorimage textures/q3r_dirt_grass/stones01.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/stones01.tga
		tcGen vector ( 0.0062500 0 0 ) ( 0 0.0062500 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/rock03.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0034722 0 0 ) ( 0 0.0034722 0 )
		rgbGen const ( 0.92 0.88 0.80 )
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_1to4
{
	qer_editorimage textures/q3r_dirt_grass/stones01.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/stones01.tga
		tcGen vector ( 0.0062500 0 0 ) ( 0 0.0062500 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/rock03.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen const ( 0.92 0.86 0.76 )
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_1to5
{
	qer_editorimage textures/q3r_dirt_grass/stones01.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/stones01.tga
		tcGen vector ( 0.0062500 0 0 ) ( 0 0.0062500 0 )
		rgbGen identity
	}
	{
		map textures/dip_valley/sand.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_2to3
{
	qer_editorimage textures/q3r_road/asph02.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_road/asph02.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen const ( 0.60 0.60 0.62 )
	}
	{
		map textures/q3r_dirt_grass/rock03.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0034722 0 0 ) ( 0 0.0034722 0 )
		rgbGen const ( 0.92 0.88 0.80 )
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_2to4
{
	qer_editorimage textures/q3r_road/asph02.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_road/asph02.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen const ( 0.60 0.60 0.62 )
	}
	{
		map textures/q3r_dirt_grass/rock03.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen const ( 0.92 0.86 0.76 )
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_2to5
{
	qer_editorimage textures/q3r_road/asph02.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_road/asph02.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen const ( 0.60 0.60 0.62 )
	}
	{
		map textures/dip_valley/sand.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_3to4
{
	qer_editorimage textures/q3r_dirt_grass/rock03.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/rock03.tga
		tcGen vector ( 0.0034722 0 0 ) ( 0 0.0034722 0 )
		rgbGen const ( 0.92 0.88 0.80 )
	}
	{
		map textures/q3r_dirt_grass/rock03.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen const ( 0.92 0.86 0.76 )
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_3to5
{
	qer_editorimage textures/q3r_dirt_grass/rock03.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/rock03.tga
		tcGen vector ( 0.0034722 0 0 ) ( 0 0.0034722 0 )
		rgbGen const ( 0.92 0.88 0.80 )
	}
	{
		map textures/dip_valley/sand.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_riviera/ter_4to5
{
	qer_editorimage textures/q3r_dirt_grass/rock03.tga
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/rock03.tga
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen const ( 0.92 0.86 0.76 )
	}
	{
		map textures/dip_valley/sand.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_riviera/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00023 0 0 ) ( 0 0.00023 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}
