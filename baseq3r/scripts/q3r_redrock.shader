// ------------------------------------------------------------
// Red Rock Basin (q3r_lcs1)
// Terrain blends rock and sand by slope (vertex alpha from
// q3map_alphaMod), the apron blends the asphalt into the sand.
// ------------------------------------------------------------

// rim, berm and desert: rock on steep faces, sand on flat ones
textures/q3r_redrock/terrain
{
	qer_editorimage textures/dip_valley/sand.jpg
	surfaceparm SURF_SAND
	surfaceparm dust
	q3map_nonplanar
	q3map_shadeAngle 35
	q3map_alphaMod dotproduct2scale ( 0.0 0.0 1.0 0.5 0.85 )
	{
		map textures/q3r_dirt_grass/rock01.tga
		rgbGen identity
	}
	{
		map textures/dip_valley/sand.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		rgbGen identity
		tcMod scale 1.5 1.5
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// apron ring: asphalt at the inner edge fading into sand
// (the inner vertices get alpha 0 from textures/q3r_redrock/alpha_000)
textures/q3r_redrock/asphsand
{
	qer_editorimage textures/q3r_road/asph03.jpg
	surfaceparm SURF_SAND
	surfaceparm dust
	q3map_nonplanar
	q3map_shadeAngle 70
	{
		map textures/q3r_road/asph03.jpg
		rgbGen identity
	}
	{
		map textures/dip_valley/sand.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
		rgbGen identity
		tcMod scale 1.5 1.5
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// volume brush: sets vertex alpha 0 for the vertices inside it
textures/q3r_redrock/alpha_000
{
	qer_editorimage textures/q3r_layout/white.jpg
	qer_trans 0.5
	q3map_alphaMod volume
	q3map_alphaMod set 0
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nolightmap
}

// worn runway paint
textures/q3r_redrock/paint_white
{
	qer_editorimage textures/q3r_redrock/paint_white.jpg
	surfaceparm SURF_ASPHALT
	surfaceparm nonsolid
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/q3r_redrock/paint_white.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// repaired asphalt
textures/q3r_redrock/asph_patch
{
	qer_editorimage textures/q3r_redrock/asph_patch.jpg
	surfaceparm SURF_ASPHALT
	surfaceparm nonsolid
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/q3r_redrock/asph_patch.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// floodlight lens (glows a little, no light emission)
textures/q3r_redrock/floodlamp
{
	qer_editorimage textures/base_light/light5.jpg
	surfaceparm nomarks
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light5.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/light5.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen const ( 0.35 0.33 0.28 )
	}
}

// boulders, scree and rock towers: rock01, merged into few surfaces
textures/q3r_redrock/rock
{
	qer_editorimage textures/q3r_dirt_grass/rock01.tga
	q3map_nonplanar
	q3map_shadeAngle 45
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/q3r_dirt_grass/rock01.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// masts, posts, ribs (metrust01), vertex lit so the small parts merge
textures/q3r_redrock/metal
{
	qer_editorimage textures/q3r_metals/metrust01.tga
	surfaceparm nolightmap
	surfaceparm SURF_METAL
	q3map_tcGen ivector ( 128 256 0 ) ( 0 256 -128 )	// continuous texture coordinates so each part becomes one surface
	q3map_nonplanar
	q3map_shadeAngle 179
	{
		map textures/q3r_metals/metrust01.tga
		rgbGen vertex
	}
}

// lamp housings (cormetal02), vertex lit
textures/q3r_redrock/metal2
{
	qer_editorimage textures/q3r_metals/cormetal02.tga
	surfaceparm nolightmap
	surfaceparm SURF_METAL
	q3map_tcGen ivector ( 128 256 0 ) ( 0 256 -64 )	// continuous texture coordinates so each part becomes one surface
	q3map_nonplanar
	q3map_shadeAngle 179
	{
		map textures/q3r_metals/cormetal02.tga
		rgbGen vertex
	}
}

// hangar sheets (wellblech)
textures/q3r_redrock/sheet
{
	qer_editorimage textures/q3r_building/wellblech.jpg
	surfaceparm SURF_METAL
	q3map_nonplanar
	q3map_shadeAngle 30
	cull disable
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/q3r_building/wellblech.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// scree and rock towers: rock01, vertex lit so each rock is one surface
textures/q3r_redrock/rock_v
{
	qer_editorimage textures/q3r_dirt_grass/rock01.tga
	surfaceparm nolightmap
	q3map_tcGen ivector ( 192 384 0 ) ( 0 384 -192 )	// continuous texture coordinates so each part becomes one surface
	q3map_nonplanar
	q3map_shadeAngle 179
	{
		map textures/q3r_dirt_grass/rock01.tga
		rgbGen vertex
	}
}
