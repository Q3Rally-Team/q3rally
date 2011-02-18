
textures/trak2/trak2_light1a
{
	qer_editorimage textures/trak2/trak2_light1a.tga
	q3map_surfacelight 4000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/trak2/trak2_light1a.tga
		blendFunc GL_DST_COLOR GL_ZERO
		//blendFunc blend
		rgbGen identity
	}
	{
		map textures/trak2/trak2_light1a.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}


textures/trak2/trak2_light1b
{
	qer_editorimage textures/trak2/trak2_light1b.tga
	q3map_surfacelight 4000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/trak2/trak2_light1b.tga
		blendFunc GL_DST_COLOR GL_ZERO
		//blendFunc blend
		rgbGen identity
	}
	{
		map textures/trak2/trak2_light1b.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}


textures/trak2/trak2_light2a
{
	qer_editorimage textures/trak2/trak2_light2a.tga
	q3map_surfacelight 4000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/trak2/trak2_light2a.tga
		blendFunc GL_DST_COLOR GL_ZERO
		//blendFunc blend
		rgbGen identity
	}
	{
		map textures/trak2/trak2_light2a.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

textures/trak2/trak2_light3a
{
	qer_editorimage textures/trak2/trak2_light3a.tga
	q3map_surfacelight 4000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/trak2/trak2_light3a.tga
		blendFunc GL_DST_COLOR GL_ZERO
		//blendFunc blend
		rgbGen identity
	}
	{
		map textures/trak2/trak2_light3a.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}


textures/trak2/trak2_faded1a_nonsolid
{
	qer_editorimage textures/trak2/trak2_faded1a_nonsolid
	surfaceparm nonsolid
	{
		map textures/trak2/trak2_faded1a_nonsolid
	}
	{
		map $lightmap
		blendfunc filter
		tcGen lightmap
	}
}

textures/trak2/bigorbit
{
        qer_editorimage textures/station15-beta3/Asteroid
	q3map_skylight 70 4
	q3map_noFast
	q3map_lightmapFilterRadius 0 2
	surfaceparm sky
      surfaceparm noimpact
      surfaceparm nolightmap
      
      q3map_sun  1 .86 .48 40 290 40
     
      //skyparms env/bigorbit/bigorbit - -
	// In the meanwhile, while we wait for 'bigorbit' to be gpled, use the other skybox
	skyParms env/space1 128 -
}