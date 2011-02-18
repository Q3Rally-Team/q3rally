// textures/islandctf/terrain_0
// {
	// q3map_lightmapMergable
	// q3map_nonPlanar
	// q3map_shadeAngle 170
	// q3map_texturesize 512 512
	// q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	// {
		// map textures/nki/nki_forest2.tga
	// }
	// {
		// map $lightmap
		// blendFunc GL_DST_COLOR GL_SRC_COLOR
		// tcGen lightmap
	// }
// }

textures/islandctf/terrain_1
{
	q3map_lightmapMergable
	q3map_nonPlanar
	q3map_shadeAngle 150
	q3map_texturesize 512 512
	q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	{
		map textures/nki/nki_sand2.tga
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen lightmap
		RgbGen identity
	}
}

textures/islandctf/terrain_2
{
	q3map_lightmapMergable
	q3map_nonPlanar
	q3map_shadeAngle 90
	q3map_texturesize 512 512
	q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	{
		map textures/stone/pjrock16.jpg
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen lightmap
		RgbGen identity
	}
}

textures/islandctf/terrain_3
{
	q3map_lightmapMergable
	q3map_nonPlanar
	q3map_shadeAngle 150
	q3map_texturesize 512 512
	q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	{
		map textures/nki/nki_ground1.tga
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen lightmap
		RgbGen identity
	}
}

textures/islandctf/terrain_4
{
	q3map_texturesize 512 512
	q3map_lightmapMergable
	q3map_nonPlanar
	q3map_shadeAngle 90
	q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	{
		map textures/stone/pjrock21.jpg
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen lightmap
		RgbGen identity
	}
}

// textures/islandctf/terrain_0to1
// {
	// q3map_texturesize 512 512
	// q3map_lightmapMergable
	// q3map_nonPlanar
	// q3map_shadeAngle 170
	// q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	
	// {
		// map textures/nki/nki_forest2.tga
	// }
	// {
		// map textures/nki/nki_sand2.tga
		// alphaGen vertex
		// blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	// }
	// {
		// map $lightmap
		// blendFunc GL_DST_COLOR GL_SRC_COLOR
		// tcGen lightmap
	// }
// }

// textures/islandctf/terrain_0to2
// {
	// q3map_texturesize 512 512
	// q3map_lightmapMergable
	// q3map_nonPlanar
	// q3map_shadeAngle 90
	// q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	
	// {
		// map textures/nki/nki_forest2.tga
	// }
	// {
		// map textures/stone/pjrock16.jpg
		// alphaGen vertex
		// blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	// }
	// {
		// map $lightmap
		// blendFunc GL_DST_COLOR GL_SRC_COLOR
		// tcGen lightmap
	// }
// }

// textures/islandctf/terrain_0to3
// {
	// q3map_texturesize 512 512
	// q3map_lightmapMergable
	// q3map_nonPlanar
	// q3map_shadeAngle 90
	// q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	
	// {
		// map textures/nki/nki_forest2.tga
	// }
	// {
		// map textures/nki/nki_ground1.tga
		// alphaGen vertex
		// blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	// }
	// {
		// map $lightmap
		// blendFunc GL_DST_COLOR GL_SRC_COLOR
		// tcGen lightmap
	// }
// }

// textures/islandctf/terrain_0to4
// {
	// q3map_texturesize 512 512
	// q3map_lightmapMergable
	// q3map_nonPlanar
	// q3map_shadeAngle 90
	// q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	
	// {
		// map textures/nki/nki_forest2.tga
	// }
	// {
		// map textures/stone/pjrock21.jpg
		// alphaGen vertex
		// blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	// }
	// {
		// map $lightmap
		// blendFunc GL_DST_COLOR GL_SRC_COLOR
		// tcGen lightmap
	// }
// }

textures/islandctf/terrain_1to2
{
	q3map_texturesize 512 512
	q3map_lightmapMergable
	q3map_nonPlanar
	q3map_shadeAngle 90
	q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	
	{
		map textures/nki/nki_sand2.tga
	}
	{
		map textures/stone/pjrock16.jpg
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen lightmap
		RgbGen identity
	}
}

textures/islandctf/terrain_1to3
{
	q3map_texturesize 512 512
	q3map_lightmapMergable
	q3map_nonPlanar
	q3map_shadeAngle 170
	q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	
	{
		map textures/nki/nki_sand2.tga
	}
	{
		map textures/nki/nki_ground1.tga
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen lightmap
		RgbGen identity
	}
}

textures/islandctf/terrain_1to4
{
	q3map_texturesize 512 512
	q3map_lightmapMergable
	q3map_nonPlanar
	q3map_shadeAngle 150
	q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	
	{
		map textures/nki/nki_sand2.tga
	}
	{
		map textures/stone/pjrock21.jpg
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen lightmap
		RgbGen identity
	}
}

textures/islandctf/terrain_2to3
{
	q3map_texturesize 512 512
	q3map_lightmapMergable
	q3map_nonPlanar
	q3map_shadeAngle 120
	q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	
	{
		map textures/stone/pjrock16.jpg
	}
	{
		map textures/nki/nki_ground1.tga
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen lightmap
		RgbGen identity
	}
}

textures/islandctf/terrain_2to4
{
	q3map_texturesize 512 512
	q3map_lightmapMergable
	q3map_nonPlanar
	q3map_shadeAngle 120
	q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	
	{
		map textures/stone/pjrock16.jpg
	}
	{
		map textures/stone/pjrock21.jpg
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen lightmap
		RgbGen identity
	}
}

textures/islandctf/terrain_3to4
{
	q3map_texturesize 512 512
	q3map_lightmapMergable
	q3map_nonPlanar
	q3map_shadeAngle 90
	q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	
	{
		map textures/nki/nki_ground1.tga
	}
	{
		map textures/stone/pjrock21.jpg
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		tcGen lightmap
		RgbGen identity
	}
}

textures/islandctf/terrain.vertex
{
	{
		map textures/nki/nki_sand2.tga
		rgbGen vertex
	}
}