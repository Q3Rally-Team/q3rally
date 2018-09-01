
textures/q3r_metals/towerstruts
{
	cull disable
	surfaceparm alphashadow
	surfaceparm playerclip
	{
		map textures/q3r_metals/towerstruts.tga
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


textures/q3r_metals/metrust02
{
	cull disable
	surfaceparm alphashadow
	surfaceparm playerclip
	{
		map textures/q3r_metals/metrust02.tga
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


textures/q3r_metals/metrust03
{
	cull disable
	surfaceparm alphashadow
	surfaceparm playerclip
	{
		map textures/q3r_metals/metrust03.tga
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


textures/q3r_metals/ladder01
{
	cull disable
	surfaceparm alphashadow
	surfaceparm playerclip
        surfaceparm trans	
        surfaceparm nomarks
	{
		map textures/q3r_metals/ladder01.tga
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


textures/q3r_metals/ladder02
{
	cull disable
	surfaceparm alphashadow
	surfaceparm playerclip
        surfaceparm trans	
        surfaceparm nomarks
	{
		map textures/q3r_metals/ladder02.tga
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

textures/q3r_metals/ladder03
{
	cull disable
	surfaceparm alphashadow
	surfaceparm playerclip
        surfaceparm trans	
        surfaceparm nomarks
	{
		map textures/q3r_metals/ladder03.tga
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


textures/q3r_metals/rail01
{
	cull disable
	surfaceparm alphashadow
	surfaceparm playerclip
        surfaceparm trans	
        surfaceparm nomarks
	{
		map textures/q3r_metals/rail01.tga
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

textures/q3r_metals/rail02
{
	cull disable
	surfaceparm alphashadow
	surfaceparm playerclip
        surfaceparm trans	
        surfaceparm nomarks
	{
		map textures/q3r_metals/rail02.tga
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


textures/q3r_metals/rail03
{
	cull disable
	surfaceparm alphashadow
	surfaceparm playerclip
        surfaceparm trans	
        surfaceparm nomarks
	{
		map textures/q3r_metals/rail03.tga
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

textures/q3r_metals/plate01

{
	surfaceparm SURF_METAL
     	{
	        map textures/effects/tinfx2d.tga
		tcGen environment
                rgbGen identity
       }


       {
	        map textures/q3r_metals/plate01.tga
		blendfunc GL_ONE GL_SRC_COLOR
                rgbGen vertex
       }

}
