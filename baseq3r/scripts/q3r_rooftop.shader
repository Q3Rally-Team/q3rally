// Rooftop Arena - Autoball on a skyscraper roof at night.
// Written by tools/maptools/make_rooftop.py. Compile with -custinfoparms.

textures/q3r_rooftop/sky
{
	qer_editorimage env/ely_darkcity/darkcity_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_globaltexture
	q3map_sun 0.75 0.55 1 45 230 40
	q3map_skylight 55 4
	skyparms env/ely_darkcity/darkcity - -
	q3r_haze 0.16 0.09 0.24 9000	// Q3Rally: night haze over the city
	q3r_colorLut gfx/luts/night
}

textures/q3r_rooftop/pitch
{
	qer_editorimage textures/q3r_rooftop/pitch_lines.jpg
	q3map_nonplanar
	{
		map textures/q3r_rooftop/pitch_base.jpg
		tcMod scale 20 12
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		clampmap textures/q3r_rooftop/pitch_lines.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen identity
	}
}

textures/q3r_rooftop/goalfloor
{
	qer_editorimage textures/q3r_rooftop/pitch_base.jpg
	{
		map textures/q3r_rooftop/pitch_base.jpg
		rgbGen const ( 0.6 0.6 0.65 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_rooftop/fillet
{
	qer_editorimage textures/q3r_rooftop/fillet.jpg
	{
		map textures/q3r_rooftop/fillet.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_rooftop/roof
{
	qer_editorimage textures/q3r_building/roof01.jpg
	q3map_lightmapSampleSize 32
	{
		map textures/q3r_building/roof01.jpg
		rgbGen const ( 0.55 0.55 0.6 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_rooftop/concrete
{
	qer_editorimage textures/q3r_building/concrete02.jpg
	q3map_lightmapSampleSize 24
	{
		map textures/q3r_building/concrete02.jpg
		rgbGen const ( 0.55 0.55 0.62 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_rooftop/rail
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

textures/q3r_rooftop/facade_red
{
	qer_editorimage textures/q3r_rooftop/facade_red.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_rooftop/facade_red.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_rooftop/facade_blue
{
	qer_editorimage textures/q3r_rooftop/facade_blue.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_rooftop/facade_blue.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_rooftop/acunit
{
	qer_editorimage textures/q3r_rooftop/acunit.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_rooftop/acunit.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_rooftop/grille
{
	qer_editorimage textures/q3r_rooftop/grille.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_rooftop/grille.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_rooftop/door
{
	qer_editorimage textures/q3r_rooftop/door.jpg
	surfaceparm nonsolid
	{
		map textures/q3r_rooftop/door.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_rooftop/glass
{
	qer_editorimage textures/q3r_rooftop/glass.jpg
	qer_trans 0.4
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nolightmap
	cull none
	{
		map textures/q3r_rooftop/glass.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen identity
	}
}

textures/q3r_rooftop/neon_red
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	q3map_surfacelight 300
	q3map_lightRGB 1.0 0.16 0.12
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 1.0 0.16 0.12 )
	}
}

textures/q3r_rooftop/neon_blue
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	q3map_surfacelight 300
	q3map_lightRGB 0.12 0.42 1.0
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 0.12 0.42 1.0 )
	}
}

textures/q3r_rooftop/neon_cyan
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 0.2 0.9 1.0 )
	}
}

textures/q3r_rooftop/neon_magenta
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 1.0 0.2 0.85 )
	}
}

textures/q3r_rooftop/exitsign
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	q3map_surfacelight 80
	q3map_lightRGB 0.2 1.0 0.35
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 0.2 1.0 0.35 )
	}
}

textures/q3r_rooftop/beacon
{
	qer_editorimage textures/q3r_rooftop/beacon.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	{
		map textures/q3r_rooftop/beacon.jpg
		rgbGen wave square 0.15 0.85 0 0.6
	}
}

textures/q3r_rooftop/ledscreen
{
	qer_editorimage textures/q3r_rooftop/ledscreen.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_surfacelight 60
	{
		map textures/q3r_rooftop/ledscreen.jpg
		tcMod scroll 0.06 0
		rgbGen identity
	}
}

textures/q3r_rooftop/win_a
{
	qer_editorimage textures/q3r_rooftop/win_a.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm nonsolid
	{
		map textures/q3r_rooftop/win_a.jpg
		rgbGen identity
	}
}

textures/q3r_rooftop/win_b
{
	qer_editorimage textures/q3r_rooftop/win_b.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm nonsolid
	{
		map textures/q3r_rooftop/win_b.jpg
		rgbGen identity
	}
}

textures/q3r_rooftop/win_c
{
	qer_editorimage textures/q3r_rooftop/win_c.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm nonsolid
	{
		map textures/q3r_rooftop/win_c.jpg
		rgbGen identity
	}
}

textures/q3r_rooftop/sign_q3rally
{
	qer_editorimage textures/q3r_rooftop/sign_q3rally.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm nonsolid
	{
		map textures/q3r_rooftop/sign_q3rally.jpg
		rgbGen identity
	}
}

textures/q3r_rooftop/sign_autoball
{
	qer_editorimage textures/q3r_rooftop/sign_autoball.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm nonsolid
	{
		map textures/q3r_rooftop/sign_autoball.jpg
		rgbGen identity
	}
}

textures/q3r_rooftop/sign_nitro
{
	qer_editorimage textures/q3r_rooftop/sign_nitro.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm nonsolid
	{
		map textures/q3r_rooftop/sign_nitro.jpg
		rgbGen identity
	}
}

textures/q3r_rooftop/streets
{
	qer_editorimage textures/q3r_rooftop/streets.jpg
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm nonsolid
	{
		map textures/q3r_rooftop/streets.jpg
		tcGen vector ( 0.00025 0 0 ) ( 0 0.00025 0 )
		rgbGen identity
	}
}

textures/q3r_rooftop/cityroof
{
	qer_editorimage textures/q3r_building/roof01.jpg
	surfaceparm nonsolid
	surfaceparm nolightmap
	surfaceparm nomarks
	{
		map textures/q3r_building/roof01.jpg
		rgbGen const ( 0.06 0.05 0.09 )
	}
}
