textures/q3r_trees/birch01_f
{
	cull disable

	{
		map textures/q3r_trees/birch01_f.tga
		blendfunc blend
	}
}

textures/q3r_trees/birch01_s
{
	cull disable

	{
		map textures/q3r_trees/birch01_s.tga
		blendfunc blend
	}
}

textures/q3r_trees/pine01_f
{
	cull disable

	{
		map textures/q3r_trees/pine01_f.tga
		blendfunc blend
	}
}

textures/q3r_trees/pine01_s
{
	cull disable

	{
		map textures/q3r_trees/pine01_s.tga
		blendfunc blend
	}
}

textures/q3r_trees/pinetrim01a
{
	cull disable
	surfaceparm trans
	surfaceparm alphashadow

	{
		map textures/q3r_trees/pinetrim01a.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/q3r_trees/pinetrim01b
{
	cull disable
	surfaceparm trans
	surfaceparm alphashadow

	{
		map textures/q3r_trees/pinetrim01b.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/q3r_trees/pinetrim02a
{
	cull disable
	surfaceparm trans
	surfaceparm alphashadow

	{
		map textures/q3r_trees/pinetrim02a.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/q3r_trees/pinetrim02b
{
	cull disable
	surfaceparm trans
	surfaceparm alphashadow

	{
		map textures/q3r_trees/pinetrim02b.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/q3r_trees/pinetrim02c
{
	cull disable
	surfaceparm trans
	surfaceparm alphashadow

	{
		map textures/q3r_trees/pinetrim02c.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/q3r_trees/pinetrim02d
{
	cull disable
	surfaceparm trans
	surfaceparm alphashadow

	{
		map textures/q3r_trees/pinetrim02d.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/q3r_trees/pinetrim02e
{
	cull disable
	surfaceparm trans
	surfaceparm alphashadow

	{
		map textures/q3r_trees/pinetrim02e.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/q3r_trees/pinewall01b
{
	surfaceparm trans
	surfaceparm alphashadow
	surfaceparm playerclip

	{
		map textures/q3r_trees/pinewall01b.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/q3r_trees/pinewall02b
{
	surfaceparm trans
	surfaceparm alphashadow
	surfaceparm playerclip

	{
		map textures/q3r_trees/pinewall02b.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/q3r_trees/tree01_still
{
	surfaceparm trans
	surfaceparm alphashadow

	{
		map textures/q3r_trees/tree01_still.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/q3r_trees/tree03_still
{
	surfaceparm trans
	surfaceparm alphashadow

	{
		map textures/q3r_trees/tree03_still.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}