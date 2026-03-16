textures/fx/koth_beam
{
	cull none
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm trans
	{
		map textures/fx/koth_beam.tga
		blendFunc GL_SRC_ALPHA GL_ONE
		rgbGen  vertex
		alphaGen vertex
		tcMod   scroll 0 -0.6
	}
}

textures/fx/koth_ring
{
	cull none
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm trans
	{
		map textures/fx/koth_ring.tga
		blendFunc GL_SRC_ALPHA GL_ONE
		rgbGen  vertex
		alphaGen vertex
	}
}
