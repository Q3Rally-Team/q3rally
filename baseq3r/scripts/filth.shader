
textures/filth/trak_light
{
	qer_editorimage textures/filth/trak_light.tga
	q3map_surfacelight 8000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/filth/trak_light.tga
		blendFunc GL_DST_COLOR GL_ZERO
		//blendFunc blend
		rgbGen identity
	}
	{
		map textures/filth/trak_light.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}


textures/filth/trak_light1
{
	qer_editorimage textures/filth/trak_light1.tga
	q3map_surfacelight 4000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/filth/trak_light1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		//blendFunc blend
		rgbGen identity
	}
	{
		map textures/filth/trak_light1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}


textures/filth/trak_light2
{
	qer_editorimage textures/filth/trak_light2.tga
	q3map_surfacelight 8000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/filth/trak_light2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		//blendFunc blend
		rgbGen identity
	}
	{
		map textures/filth/trak_light2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/filth/trak_light2a
{
	qer_editorimage textures/filth/trak_light2a.tga
	q3map_surfacelight 8000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/filth/trak_light2a.tga
		blendFunc GL_DST_COLOR GL_ZERO
		//blendFunc blend
		rgbGen identity
	}
	{
		map textures/filth/trak_light2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/filth/trak_light2b
{
	qer_editorimage textures/filth/trak_light2b.tga
	q3map_surfacelight 8000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/filth/trak_light2b.tga
		blendFunc GL_DST_COLOR GL_ZERO
		//blendFunc blend
		rgbGen identity
	}
	{
		map textures/filth/trak_light2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/filth/trak_light2c
{
	qer_editorimage textures/filth/trak_light2c.tga
	q3map_surfacelight 8000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/filth/trak_light2c.tga
		blendFunc GL_DST_COLOR GL_ZERO
		//blendFunc blend
		rgbGen identity
	}
	{
		map textures/filth/trak_light2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/filth/trak_light3_64
{
	qer_editorimage textures/filth/trak_light3_64.tga
	q3map_surfacelight 8000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/filth/trak_light3_64.tga
		blendFunc GL_DST_COLOR GL_ZERO
		//blendFunc blend
		rgbGen identity
	}
	{
		map textures/filth/trak_light3_64.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}


textures/filth/trak_light3_128
{
	qer_editorimage textures/filth/trak_light3_128.tga
	q3map_surfacelight 4000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/filth/trak_light3_128.tga
		blendFunc GL_DST_COLOR GL_ZERO
		//blendFunc blend
		rgbGen identity
	}
	{
		map textures/filth/trak_light3_128.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/filth/trak_trim_trans1
{
	qer_editorimage textures/filth/trak_trim_trans1.tga
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm alphashadow
	cull none

	{
		map textures/filth/trak_trim_trans1.tga
		alphafunc GE128
		depthwrite
	}
	{
		map $lightmap
		depthfunc equal
		blendfunc filter
	}
}

textures/filth/trak_trim_trans2
{
	qer_editorimage textures/filth/trak_trim_trans2.tga
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm alphashadow
	cull none

	{
		map textures/filth/trak_trim_trans2.tga
		alphafunc GE128
		depthwrite
	}
	{
		map $lightmap
		depthfunc equal
		blendfunc filter
	}
}


textures/filth/trak_rustdecal1
{
	surfaceparm noimpact
	surfaceparm nonsolid
	surfaceparm trans
	polygonoffset
	{
		map textures/filth/trak_rustdecal1.tga
		blendfunc filter
	}
}


textures/filth/trak_fx1
{
  surfaceparm nolightmap
  surfaceparm nonsolid
  cull twosided
  {
    map textures/filth/trak_fx1.tga
    tcGen environment
    tcMod turb 0 0.25 0 0.5
    tcmod scroll 1 1
    blendfunc GL_ONE GL_ONE
  }
}


textures/filth/fog
{
		qer_editorimage textures/common/fog.tga
		surfaceparm	trans
		surfaceparm	nonsolid
		surfaceparm	fog
		surfaceparm	nolightmap

		fogparms ( .929 .839 .518 ) 1000

}