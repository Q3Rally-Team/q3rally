textures/oquartz/+0sflob
{
	q3map_lightimage textures/oquartz/0sflovrox.tga
	surfaceparm metalsteps
	surfaceparm nomarks
	q3map_surfacelight 128
	{
		map textures/oquartz/0sflovrox.tga
		rgbGen wave sin 0.5 0.5 0 1 
		tcMod scroll -4 0
	}
	{
		map textures/oquartz/0sflovrox.tga
		blendfunc add
		rgbGen wave noise 0 0.5 0 0.2 
		tcMod scroll -2 0
	}
	{
		map textures/effects/tinfx2c.tga
		blendfunc add
		tcGen environment 
	}
	{
		map textures/oquartz/0sflob.tga
		blendfunc blend
	}
	{
		map $lightmap 
		blendfunc filter
	}
}

