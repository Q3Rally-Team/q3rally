// Quarry King - King of the Hill in a limestone quarry.
// Written by tools/maptools/make_quarry.py. Compile with -custinfoparms.

textures/q3r_quarry/sky
{
	qer_editorimage textures/q3r_skies/env/trainyard/trainyard_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_globaltexture
	q3map_sun 1.0 0.97 0.91 230 310 52
	q3map_skylight 90 3
	q3gl2_sun 1.0 0.97 0.91 160 310 52 0.5
	skyparms textures/q3r_skies/env/trainyard/trainyard - -
	q3r_haze 0.70 0.75 0.82 6500	// Q3Rally: summer haze over the quarry
}

textures/q3r_quarry/block
{
	qer_editorimage textures/q3r_quarry/block.jpg
	{
		map textures/q3r_quarry/block.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/paint
{
	qer_editorimage textures/q3r_metals/plate01.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/plate01.tga
		rgbGen const ( 1.00 0.74 0.12 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/steel
{
	qer_editorimage textures/q3r_metals/plate01.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/plate01.tga
		rgbGen const ( 0.62 0.62 0.64 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/steelrust
{
	qer_editorimage textures/q3r_metals/metrust01.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/metrust01.tga
		rgbGen const ( 0.85 0.80 0.75 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/rubber
{
	qer_editorimage textures/q3r_metals/plate01.tga
	{
		map textures/q3r_metals/plate01.tga
		rgbGen const ( 0.16 0.16 0.17 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/belt
{
	qer_editorimage textures/q3r_metals/plate01.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/plate01.tga
		rgbGen const ( 0.12 0.12 0.12 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/glass
{
	qer_editorimage textures/q3r_metals/plate01.tga
	surfaceparm nonsolid
	{
		map textures/q3r_metals/plate01.tga
		rgbGen const ( 0.18 0.24 0.30 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/corrugated
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.82 0.84 0.86 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/cabin
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.42 0.62 0.78 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/silo
{
	qer_editorimage textures/q3r_building/concrete03.jpg
	{
		map textures/q3r_building/concrete03.jpg
		rgbGen const ( 0.92 0.92 0.90 )
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/concrete
{
	qer_editorimage textures/q3r_building/concrete01.jpg
	{
		map textures/q3r_building/concrete01.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/hills
{
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_nonplanar
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 64
	surfaceparm SURF_GRASS
	{
		map textures/q3r_dirt_grass/grass07.tga
		tcGen vector ( 0.0031250 0 0 ) ( 0 0.0031250 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/boundary
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
// ter_<layer> / ter_<a>to<b> from maps/q3r_quarry_index.pcx. Layers: 0 grass, 1 dust, 2 gravel, 3 rock, 4 facex, 5 facey

textures/q3r_quarry/t_grass
{
	qer_editorimage textures/q3r_dirt_grass/grass07.tga
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	surfaceparm SURF_GRASS
}

textures/q3r_quarry/t_dirt
{
	qer_editorimage textures/q3r_quarry/dust.jpg
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	surfaceparm SURF_DIRT
}

textures/q3r_quarry/t_gravel
{
	qer_editorimage textures/q3r_quarry/gravel.jpg
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
	surfaceparm SURF_GRAVEL
}

textures/q3r_quarry/t_plain
{
	qer_editorimage textures/q3r_quarry/limestone.jpg
	q3map_indexed
	q3map_shadeAngle 120
	q3map_nonplanar
}

textures/q3r_quarry/ter_0
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
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_1
{
	qer_editorimage textures/q3r_quarry/dust.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/dust.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_2
{
	qer_editorimage textures/q3r_quarry/gravel.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/gravel.jpg
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_3
{
	qer_editorimage textures/q3r_quarry/limestone.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/limestone.jpg
		tcGen vector ( 0.0034722 0 0 ) ( 0 0.0034722 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_4
{
	qer_editorimage textures/q3r_quarry/limestone.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/limestone.jpg
		tcGen vector ( 0 0.0031250 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_5
{
	qer_editorimage textures/q3r_quarry/limestone.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/limestone.jpg
		tcGen vector ( 0.0031250 0 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_0to1
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
		map textures/q3r_quarry/dust.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_0to2
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
		map textures/q3r_quarry/gravel.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_0to3
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
		map textures/q3r_quarry/limestone.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0034722 0 0 ) ( 0 0.0034722 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_0to4
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
		map textures/q3r_quarry/limestone.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0 0.0031250 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_0to5
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
		map textures/q3r_quarry/limestone.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0031250 0 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_1to2
{
	qer_editorimage textures/q3r_quarry/dust.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/dust.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/gravel.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_1to3
{
	qer_editorimage textures/q3r_quarry/dust.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/dust.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/limestone.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0034722 0 0 ) ( 0 0.0034722 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_1to4
{
	qer_editorimage textures/q3r_quarry/dust.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/dust.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/limestone.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0 0.0031250 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_1to5
{
	qer_editorimage textures/q3r_quarry/dust.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/dust.jpg
		tcGen vector ( 0.0039062 0 0 ) ( 0 0.0039062 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/limestone.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0031250 0 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_2to3
{
	qer_editorimage textures/q3r_quarry/gravel.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/gravel.jpg
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/limestone.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0034722 0 0 ) ( 0 0.0034722 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_2to4
{
	qer_editorimage textures/q3r_quarry/gravel.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/gravel.jpg
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/limestone.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0 0.0031250 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_2to5
{
	qer_editorimage textures/q3r_quarry/gravel.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/gravel.jpg
		tcGen vector ( 0.0052083 0 0 ) ( 0 0.0052083 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/limestone.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0031250 0 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_3to4
{
	qer_editorimage textures/q3r_quarry/limestone.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/limestone.jpg
		tcGen vector ( 0.0034722 0 0 ) ( 0 0.0034722 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/limestone.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0 0.0031250 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_3to5
{
	qer_editorimage textures/q3r_quarry/limestone.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/limestone.jpg
		tcGen vector ( 0.0034722 0 0 ) ( 0 0.0034722 0 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/limestone.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0031250 0 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_quarry/ter_4to5
{
	qer_editorimage textures/q3r_quarry/limestone.jpg
	q3map_shadeAngle 120
	q3map_lightmapSampleSize 24
	q3map_nonplanar
	{
		map textures/q3r_quarry/limestone.jpg
		tcGen vector ( 0 0.0031250 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/limestone.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		tcGen vector ( 0.0031250 0 0 ) ( 0 0 0.0031250 )
		rgbGen identity
	}
	{
		map textures/q3r_quarry/macro.jpg
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen vector ( 0.00027 0 0 ) ( 0 0.00027 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}
