// Canyon Rush - A2B sprint through a red desert canyon.
// Written by tools/maptools/make_canyonrush.py. Compile with -custinfoparms.

textures/q3r_canyonrush/sky
{
	qer_editorimage env/ame_desert/desertsky_ft.tga
	surfaceparm sky
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_sun 1.0 0.88 0.72 240 230 36
	q3map_skylight 170 4
	q3gl2_sun 1.0 0.88 0.72 160 230 36 0.5
	skyparms env/ame_desert/desertsky - -
	q3r_haze 0.74 0.62 0.50 7000	// Q3Rally: warm desert haze
}

textures/q3r_canyonrush/rock
{
	qer_editorimage textures/q3r_canyonrush/strata.jpg
	q3map_shadeAngle 50
	{
		map textures/q3r_canyonrush/strata.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/caprock
{
	qer_editorimage textures/q3r_dirt_grass/rock01.tga
	q3map_shadeAngle 50
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

textures/q3r_canyonrush/boulder
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

textures/q3r_canyonrush/metal
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

textures/q3r_canyonrush/checker
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

textures/q3r_canyonrush/hills
{
	qer_editorimage textures/dip_valley/sand.jpg
	// unreachable desert hills (q3r_mesaking backdrop), coarse lightmap
	q3map_lightmapSampleSize 64
	q3map_nonplanar
	q3map_shadeAngle 120
	{
		map textures/dip_valley/sand.jpg
		tcGen vector ( 0.002 0 0 ) ( 0 0.002 0 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/boundary
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
// ter_<layer> / ter_<a>to<b> from maps/q3r_canyonrush_index.pcx. Layers: 0 sand, 1 track, 2 gravel, 3 rock, 4 cliff

textures/q3r_canyonrush/t_sand
{
	qer_editorimage textures/dip_valley/sand.jpg
	q3map_indexed
	q3map_shadeAngle 70
	q3map_nonplanar
	surfaceparm SURF_SAND
}

textures/q3r_canyonrush/t_dirt
{
	qer_editorimage textures/q3r_dirt_grass/dirt01.tga
	q3map_indexed
	q3map_shadeAngle 70
	q3map_nonplanar
	surfaceparm SURF_DIRT
}

textures/q3r_canyonrush/t_gravel
{
	qer_editorimage textures/q3r_dirt_grass/stones02.tga
	q3map_indexed
	q3map_shadeAngle 70
	q3map_nonplanar
	surfaceparm SURF_GRAVEL
}

textures/q3r_canyonrush/t_plain
{
	qer_editorimage textures/q3r_dirt_grass/rock01.tga
	q3map_indexed
	q3map_shadeAngle 70
	q3map_nonplanar
}

textures/q3r_canyonrush/ter_0
{
	qer_editorimage textures/dip_valley/sand.jpg
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/dip_valley/sand.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/ter_1
{
	qer_editorimage textures/q3r_dirt_grass/dirt01.tga
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/dirt01.tga
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/ter_2
{
	qer_editorimage textures/q3r_dirt_grass/stones02.tga
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/stones02.tga
		tcGen vector ( 0.0062500 0 0 ) ( 0 0.0062500 0 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/ter_3
{
	qer_editorimage textures/q3r_dirt_grass/rock01.tga
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/rock01.tga
		tcGen vector ( 0.0044643 0 0 ) ( 0 0.0044643 0 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/ter_4
{
	qer_editorimage textures/q3r_canyonrush/strata.jpg
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/q3r_canyonrush/strata.jpg
		tcGen vector ( 0.0010677 0.0007422 0 ) ( 0 0 0.0013021 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/ter_0to1
{
	qer_editorimage textures/dip_valley/sand.jpg
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/dip_valley/sand.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/dirt01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/ter_0to2
{
	qer_editorimage textures/dip_valley/sand.jpg
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/dip_valley/sand.jpg
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
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/ter_0to3
{
	qer_editorimage textures/dip_valley/sand.jpg
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/dip_valley/sand.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/rock01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0044643 0 0 ) ( 0 0.0044643 0 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/ter_0to4
{
	qer_editorimage textures/dip_valley/sand.jpg
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/dip_valley/sand.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/strata.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0010677 0.0007422 0 ) ( 0 0 0.0013021 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/ter_1to2
{
	qer_editorimage textures/q3r_dirt_grass/dirt01.tga
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/dirt01.tga
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
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
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/ter_1to3
{
	qer_editorimage textures/q3r_dirt_grass/dirt01.tga
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/dirt01.tga
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/rock01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0044643 0 0 ) ( 0 0.0044643 0 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/ter_1to4
{
	qer_editorimage textures/q3r_dirt_grass/dirt01.tga
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/dirt01.tga
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/strata.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0010677 0.0007422 0 ) ( 0 0 0.0013021 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/ter_2to3
{
	qer_editorimage textures/q3r_dirt_grass/stones02.tga
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/stones02.tga
		tcGen vector ( 0.0062500 0 0 ) ( 0 0.0062500 0 )
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/rock01.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0044643 0 0 ) ( 0 0.0044643 0 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/ter_2to4
{
	qer_editorimage textures/q3r_dirt_grass/stones02.tga
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/stones02.tga
		tcGen vector ( 0.0062500 0 0 ) ( 0 0.0062500 0 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/strata.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0010677 0.0007422 0 ) ( 0 0 0.0013021 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_canyonrush/ter_3to4
{
	qer_editorimage textures/q3r_dirt_grass/rock01.tga
	q3map_shadeAngle 70
	q3map_nonplanar
	{
		map textures/q3r_dirt_grass/rock01.tga
		tcGen vector ( 0.0044643 0 0 ) ( 0 0.0044643 0 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/strata.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0010677 0.0007422 0 ) ( 0 0 0.0013021 )
		rgbGen identity
	}
	{
		map textures/q3r_canyonrush/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.0002 0 0 ) ( 0 0.0002 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}
