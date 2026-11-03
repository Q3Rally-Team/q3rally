// Industrial Crossfire - four-team CTF in a harbour at dusk.
// Written by tools/maptools/make_industrialcrossfire.py. Compile with -custinfoparms.

textures/q3r_ctf4_03/sky
{
	qer_editorimage env/ely_sunset/sunset_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_globaltexture
	q3map_sun 1 0.62 0.36 150 0 14
	q3map_skylight 70 4
	q3gl2_sun 1 0.62 0.36 110 0 14 0.6
	skyparms env/ely_sunset/sunset - -
	q3r_haze 0.42 0.36 0.38 5200	// Q3Rally: harbour dusk haze
}

textures/q3r_ctf4_03/ground
{
	qer_editorimage textures/q3r_road/concrete.jpg
	q3map_nonplanar
	{
		map textures/q3r_road/concrete.jpg
		tcGen vector ( 0.005208 0 0 ) ( 0 0.005208 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/asphalt
{
	qer_editorimage textures/q3r_road/asph02.jpg
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm trans
	polygonOffset
	surfaceparm SURF_ASPHALT
	{
		map textures/q3r_road/asph02.jpg
		tcGen vector ( 0.003906 0 0 ) ( 0 0.003906 0 )
		rgbGen const ( 0.50 0.50 0.53 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/line
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm trans
	polygonOffset
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 0.85 0.80 0.55 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/ballast
{
	qer_editorimage textures/q3r_ctf4_03/ballast.jpg
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm trans
	polygonOffset
	{
		map textures/q3r_ctf4_03/ballast.jpg
		tcGen vector ( 0.007812 0 0 ) ( 0 0.007812 0 )
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/railplate
{
	qer_editorimage textures/q3r_metals/metrust01.tga
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm trans
	polygonOffset
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

textures/q3r_ctf4_03/paint_red
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.78 0.20 0.16 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/paint_red_gate
{
	qer_editorimage textures/q3r_building/garage_np.jpg
	{
		map textures/q3r_building/garage_np.jpg
		rgbGen const ( 1.00 0.59 0.56 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/paint_red_floor
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm trans
	polygonOffset
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 0.78 0.20 0.16 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/paint_blue
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.18 0.36 0.80 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/paint_blue_gate
{
	qer_editorimage textures/q3r_building/garage_np.jpg
	{
		map textures/q3r_building/garage_np.jpg
		rgbGen const ( 0.58 0.70 1.00 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/paint_blue_floor
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm trans
	polygonOffset
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 0.18 0.36 0.80 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/paint_green
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.22 0.58 0.26 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/paint_green_gate
{
	qer_editorimage textures/q3r_building/garage_np.jpg
	{
		map textures/q3r_building/garage_np.jpg
		rgbGen const ( 0.60 0.86 0.63 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/paint_green_floor
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm trans
	polygonOffset
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 0.22 0.58 0.26 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/paint_yellow
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.92 0.72 0.12 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/paint_yellow_gate
{
	qer_editorimage textures/q3r_building/garage_np.jpg
	{
		map textures/q3r_building/garage_np.jpg
		rgbGen const ( 1.00 0.95 0.53 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/paint_yellow_floor
{
	qer_editorimage textures/q3r_signage/white.jpg
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm trans
	polygonOffset
	{
		map textures/q3r_signage/white.jpg
		rgbGen const ( 0.92 0.72 0.12 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont0
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.80 0.36 0.12 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont0_end
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.62 0.28 0.09 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont1
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.72 0.72 0.70 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont1_end
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.56 0.56 0.55 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont2
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.20 0.42 0.50 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont2_end
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.16 0.33 0.39 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont3
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.48 0.26 0.16 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont3_end
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.37 0.20 0.12 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont4
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.62 0.14 0.12 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont4_end
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.48 0.11 0.09 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont5
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.18 0.30 0.56 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont5_end
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.14 0.23 0.44 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont6
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.30 0.48 0.24 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont6_end
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.23 0.37 0.19 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont7
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.86 0.66 0.16 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cont7_end
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.67 0.51 0.12 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/controof
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

textures/q3r_ctf4_03/metal
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

textures/q3r_ctf4_03/crane
{
	qer_editorimage textures/q3r_metals/cormetal03.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/cormetal03.tga
		rgbGen const ( 0.95 0.62 0.15 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/cranehouse
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.92 0.88 0.80 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/tank
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	{
		map textures/q3r_building/wellblech.jpg
		rgbGen const ( 0.86 0.86 0.82 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/tanktop
{
	qer_editorimage textures/base_floor/diamond2.jpg
	surfaceparm SURF_METAL
	{
		map textures/base_floor/diamond2.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/pipes
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

textures/q3r_ctf4_03/hallwall
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

textures/q3r_ctf4_03/hallwin
{
	qer_editorimage textures/q3r_building/ind-windows-1.jpg
	{
		map textures/q3r_building/ind-windows-1.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/gate
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

textures/q3r_ctf4_03/garage
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

textures/q3r_ctf4_03/roof
{
	qer_editorimage textures/q3r_metals/cormetal01.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/cormetal01.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/roofdeck
{
	qer_editorimage textures/base_floor/diamond2.jpg
	surfaceparm SURF_METAL
	{
		map textures/base_floor/diamond2.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/brick
{
	qer_editorimage textures/q3r_building/brick4.jpg
	{
		map textures/q3r_building/brick4.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/brickwin
{
	qer_editorimage textures/q3r_building/externalwall50a.jpg
	{
		map textures/q3r_building/externalwall50a.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/farwin
{
	qer_editorimage textures/q3r_building/ind-windows-1.jpg
	q3map_lightmapSampleSize 64
	{
		map textures/q3r_building/ind-windows-1.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/farbrick
{
	qer_editorimage textures/q3r_building/externalwall50a.jpg
	q3map_lightmapSampleSize 64
	{
		map textures/q3r_building/externalwall50a.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/concrete
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

textures/q3r_ctf4_03/deck
{
	qer_editorimage textures/q3r_road/asph02.jpg
	surfaceparm SURF_ASPHALT
	{
		map textures/q3r_road/asph02.jpg
		rgbGen const ( 0.55 0.55 0.58 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/ramp
{
	qer_editorimage textures/base_floor/diamond2.jpg
	surfaceparm SURF_METAL
	{
		map textures/base_floor/diamond2.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/railing
{
	qer_editorimage textures/base_trim/yellow_rustbx128.jpg
	{
		map textures/base_trim/yellow_rustbx128.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/coping
{
	qer_editorimage textures/q3r_road/concrete2.jpg
	{
		map textures/q3r_road/concrete2.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/bollard
{
	qer_editorimage textures/q3r_metals/Rost.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/Rost.tga
		rgbGen const ( 0.30 0.30 0.32 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/quaywall
{
	qer_editorimage textures/q3r_building/bricks03.jpg
	{
		map textures/q3r_building/bricks03.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/seabed
{
	qer_editorimage textures/q3r_road/concrete.jpg
	q3map_lightmapSampleSize 64
	{
		map textures/q3r_road/concrete.jpg
		rgbGen const ( 0.25 0.27 0.25 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/chimney
{
	qer_editorimage textures/q3r_building/brick4.jpg
	{
		map textures/q3r_building/brick4.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/chimney_ship
{
	qer_editorimage textures/q3r_metals/Rost.tga
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

textures/q3r_ctf4_03/soot
{
	qer_editorimage textures/q3r_road/concrete.jpg
	{
		map textures/q3r_road/concrete.jpg
		rgbGen const ( 0.12 0.11 0.10 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/silo
{
	qer_editorimage textures/q3r_building/concrete03.jpg
	{
		map textures/q3r_building/concrete03.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/hull
{
	qer_editorimage textures/q3r_metals/metrust01.tga
	surfaceparm SURF_METAL
	{
		map textures/q3r_metals/metrust01.tga
		rgbGen const ( 0.55 0.22 0.18 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/shipdeck
{
	qer_editorimage textures/base_floor/diamond2.jpg
	surfaceparm SURF_METAL
	{
		map textures/base_floor/diamond2.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/shiphouse
{
	qer_editorimage textures/q3r_building/concrete01.jpg
	{
		map textures/q3r_building/concrete01.jpg
		rgbGen const ( 0.95 0.95 0.92 )

	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_ctf4_03/clip
{
	qer_trans 0.3
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm detail
}

textures/q3r_ctf4_03/propclip
{
	qer_trans 0.15
	// invisible solid hull of a model (wagons, masts): stops cars and shots, lets light through
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm detail
	surfaceparm SURF_METAL
}

textures/q3r_ctf4_03/boundary
{
	qer_trans 0.15
	// invisible but SOLID (Bullet only takes solid brushes); trans lets light through
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm noimpact
	surfaceparm detail
}

textures/q3r_ctf4_03/water
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
	q3r_water textures/q3r_liquids/water_normal.tga 320 0.02 0.018
	q3r_waterColor 0.06 0.08 0.10 0.72
	q3r_waterFresnel 0.08 4
	q3r_waterSpecular 1.0
	{
		map textures/q3r_liquids/water.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen const ( 0.20 0.26 0.30 )
		alphaGen const 0.75
		tcMod scale 0.3 0.3
		tcMod scroll 0.006 0.009
	}
	{
		map textures/q3r_liquids/water.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen const ( 0.62 0.50 0.42 )
		alphaGen const 0.15
		tcMod scale 0.37 0.37
		tcMod scroll -0.005 0.007
	}
}
