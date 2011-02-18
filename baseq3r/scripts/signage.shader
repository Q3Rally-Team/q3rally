//======================
//	Direction Signs	
//======================

textures/q3r_signage/dirsign03_blu

{
	qer_editorimage textures/q3r_signage/dirsign02_blu.tga
	cull disable
	
	{
		map textures/q3r_signage/dirsign02_blu.tga
		rgbgen vertex
	}

}

textures/q3r_signage/dirsign03_bnw

{
	cull disable
	qer_editorimage textures/q3r_signage/dirsign02_bnw.tga
	
	{
		map textures/q3r_signage/dirsign02_bnw.tga
		rgbgen identity
	}

}

textures/q3r_signage/dirsign03_grn

{
	cull disable
	qer_editorimage textures/q3r_signage/dirsign02_grn.tga
	
	{
		map textures/q3r_signage/dirsign02_grn.tga
		rgbgen identity
	}

}

textures/q3r_signage/dirsign03_red

{
	cull disable
	qer_editorimage textures/q3r_signage/dirsign02_red.tga
	{
		map textures/q3r_signage/dirsign02_red.tga
		rgbgen vertex
	}

}

textures/q3r_signage/dirsign03_ylw

{
	cull disable
	qer_editorimage textures/q3r_signage/dirsign02_ylw.tga
	
	{
		map textures/q3r_signage/dirsign02_ylw.tga
		rgbgen identity
	}

}

//================
//	Deco Flags	
//================

textures/q3r_signage/flags01_blu
{
	cull disable
	surfaceparm alphashadow
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks

	{
		map textures/q3r_signage/flags01_blu.tga
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

textures/q3r_signage/flags01_bnw
{
	cull disable
	surfaceparm alphashadow
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks

	{
		map textures/q3r_signage/flags01_bnw.tga
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

textures/q3r_signage/flags01_grn
{
	cull disable
	surfaceparm alphashadow
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks

	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/q3r_signage/flags01_grn.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		alphaGen lightingSpecular
		rgbGen identity
	}
}

textures/q3r_signage/flags01_red
{
	cull disable
	surfaceparm alphashadow
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks

	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/q3r_signage/flags01_red.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		alphaGen lightingSpecular
		rgbGen identity
	}
}

textures/q3r_signage/flags01_ylw
{
	cull disable
	surfaceparm alphashadow
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks

	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/q3r_signage/flags01_ylw.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		alphaGen lightingSpecular
		rgbGen identity
	}
}
