// Ridge Pass - snowy mountain pass circuit.
// Written by tools/maptools/make_ridgepass.py. Compile with -custinfoparms.

textures/q3r_ridgepass/sky
{
	qer_editorimage textures/arctic/arc_sky3
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_globaltexture
	q3map_sun 1 0.97 0.92 210 215 34
	q3map_skylight 150 4
	q3gl2_sun 1 0.97 0.92 150 215 34 0.5
	skyparms env/arcsky/arcsky2 1024 -
	q3r_haze 0.70 0.74 0.82 6500	// Q3Rally: cold mountain haze
	q3r_colorLut gfx/luts/cold	// Q3Rally: colour grading
	{
		map textures/skies/dimclouds.tga
		tcMod scroll 0.01 0.005
		tcMod scale 2 2
		blendFunc GL_ONE GL_ONE
	}
}

textures/q3r_ridgepass/concrete
{
	qer_editorimage textures/arctic/concrete1_1.jpg
	q3map_lightmapSampleSize 32
	{
		map textures/arctic/concrete1_1.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/barrier
{
	qer_editorimage textures/q3r_road/cement_redwhite.tga
	{
		map textures/q3r_road/cement_redwhite.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/metal
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

textures/q3r_ridgepass/checker
{
	qer_editorimage textures/q3r_dirt_grass/checker01.tga
	{
		map textures/q3r_dirt_grass/checker01.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/startbanner
{
	qer_editorimage textures/q3r_signage/q3rstart.jpg
	{
		map textures/q3r_signage/q3rstart.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/roof
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

textures/q3r_ridgepass/pitwall
{
	qer_editorimage textures/q3r_building/concrete02.jpg
	{
		map textures/q3r_building/concrete02.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/pitsign
{
	qer_editorimage textures/q3r_sponsors/q3rlong01.tga
	{
		map textures/q3r_sponsors/q3rlong01.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/garage
{
	qer_editorimage textures/q3r_building/garage_np.jpg
	{
		map textures/q3r_building/garage_np.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/glass
{
	qer_editorimage textures/q3r_building/windows21A.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/windows21A.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/darkglass
{
	qer_editorimage textures/q3r_glass/darkglass.tga
	{
		map textures/q3r_glass/darkglass.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/poster
{
	qer_editorimage textures/q3r_sponsors/q3rposter.tga
	{
		map textures/q3r_sponsors/q3rposter.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/roofmetal
{
	qer_editorimage textures/q3r_metals/cormetal03.tga
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

textures/q3r_ridgepass/logo
{
	qer_editorimage textures/q3r_sponsors/q3rlogo01a.tga
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm nolightmap
	polygonOffset
	{
		map textures/q3r_sponsors/q3rlogo01a.tga
		blendFunc blend
		rgbGen identity
	}
}

textures/q3r_ridgepass/redlamp
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 0.95 0.08 0.05 )
	}
}

textures/q3r_ridgepass/bark
{
	qer_editorimage models/mapobjects/GR_trees/stamm.jpg
	{
		map models/mapobjects/GR_trees/stamm.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/boulder
{
	qer_editorimage textures/arctic/rock1_1.jpg
	q3map_shadeAngle 70
	{
		map textures/arctic/rock1_1.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/lamp
{
	qer_editorimage textures/arctic/light2_1.jpg
	q3map_surfacelight 900
	surfaceparm nomarks
	{
		map textures/arctic/light2_1.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/deck
{
	qer_editorimage textures/q3r_road/asph02.jpg
	q3map_nonplanar
	q3map_shadeAngle 30
	surfaceparm SURF_ASPHALT
	{
		map textures/q3r_road/asph02.jpg
		tcGen vector ( 0.0039063 0 0 ) ( 0 0.0039063 0 )
		rgbGen const ( 0.52 0.53 0.56 )
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/roofsnow
{
	qer_editorimage textures/q3r_ground/snowy02_slick.jpg
	surfaceparm SURF_SNOW
	{
		map textures/q3r_ground/snowy02_slick.jpg
		tcGen vector ( 0.003125 0 0 ) ( 0 0.003125 0 )
		rgbGen identity
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/line
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm trans
	polygonOffset
	{
		map textures/q3r_signage/white.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/kerb
{
	qer_editorimage textures/q3r_road/cement_redwhite.tga
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm trans
	polygonOffset
	{
		map textures/q3r_road/cement_redwhite.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ridgepass/propclip
{
	qer_editorimage textures/base_floor/concrete.jpg
	qer_trans 0.15
	// invisible solid hull of a model (floodlight masts): stops cars and shots, lets light through
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm detail
}

textures/q3r_ridgepass/boundary
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
// ter_<layer> / ter_<a>to<b> from maps/q3r_ridgepass_index.pcx. Layers: 0 snow, 1 slush, 2 asphalt, 3 rock, 4 cliff

textures/q3r_ridgepass/t_snow
{
	qer_editorimage textures/q3r_ground/snowy02_slick.jpg
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	surfaceparm SURF_SNOW
}

textures/q3r_ridgepass/t_asphalt
{
	qer_editorimage textures/q3r_road/asph02.jpg
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	surfaceparm SURF_ASPHALT
}

textures/q3r_ridgepass/t_rock
{
	qer_editorimage textures/arctic/rock1_1.jpg
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
}

textures/q3r_ridgepass/ter_0
{
	qer_editorimage textures/q3r_ground/snowy02_slick.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/q3r_ground/snowy02_slick.jpg
		tcGen vector ( 0.0031250 0 0 ) ( 0 0.0031250 0 )
		rgbGen identity
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/ter_1
{
	qer_editorimage textures/q3r_ground/snowy04.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/q3r_ground/snowy04.jpg
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/ter_2
{
	qer_editorimage textures/q3r_road/asph02.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/q3r_road/asph02.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen const ( 0.52 0.53 0.56 )
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/ter_3
{
	qer_editorimage textures/arctic/rock1_1.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/arctic/rock1_1.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/ter_4
{
	qer_editorimage textures/arctic/rock1_1.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/arctic/rock1_1.jpg
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/ter_0to1
{
	qer_editorimage textures/q3r_ground/snowy02_slick.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/q3r_ground/snowy02_slick.jpg
		tcGen vector ( 0.0031250 0 0 ) ( 0 0.0031250 0 )
		rgbGen identity
	}
	{
		map textures/q3r_ground/snowy04.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/ter_0to2
{
	qer_editorimage textures/q3r_ground/snowy02_slick.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/q3r_ground/snowy02_slick.jpg
		tcGen vector ( 0.0031250 0 0 ) ( 0 0.0031250 0 )
		rgbGen identity
	}
	{
		map textures/q3r_road/asph02.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen const ( 0.52 0.53 0.56 )
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/ter_0to3
{
	qer_editorimage textures/q3r_ground/snowy02_slick.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/q3r_ground/snowy02_slick.jpg
		tcGen vector ( 0.0031250 0 0 ) ( 0 0.0031250 0 )
		rgbGen identity
	}
	{
		map textures/arctic/rock1_1.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/ter_0to4
{
	qer_editorimage textures/q3r_ground/snowy02_slick.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/q3r_ground/snowy02_slick.jpg
		tcGen vector ( 0.0031250 0 0 ) ( 0 0.0031250 0 )
		rgbGen identity
	}
	{
		map textures/arctic/rock1_1.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/ter_1to2
{
	qer_editorimage textures/q3r_ground/snowy04.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/q3r_ground/snowy04.jpg
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/q3r_road/asph02.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen const ( 0.52 0.53 0.56 )
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/ter_1to3
{
	qer_editorimage textures/q3r_ground/snowy04.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/q3r_ground/snowy04.jpg
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/arctic/rock1_1.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/ter_1to4
{
	qer_editorimage textures/q3r_ground/snowy04.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/q3r_ground/snowy04.jpg
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/arctic/rock1_1.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/ter_2to3
{
	qer_editorimage textures/q3r_road/asph02.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/q3r_road/asph02.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen const ( 0.52 0.53 0.56 )
	}
	{
		map textures/arctic/rock1_1.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/ter_2to4
{
	qer_editorimage textures/q3r_road/asph02.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/q3r_road/asph02.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen const ( 0.52 0.53 0.56 )
	}
	{
		map textures/arctic/rock1_1.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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

textures/q3r_ridgepass/ter_3to4
{
	qer_editorimage textures/arctic/rock1_1.jpg
	q3map_shadeAngle 120
	q3map_nonplanar
	{
		map textures/arctic/rock1_1.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/arctic/rock1_1.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0022187 0.0022187 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_ridgepass/macro.jpg
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
