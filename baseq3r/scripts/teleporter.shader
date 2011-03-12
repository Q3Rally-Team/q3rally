teleptThis
{
	surfaceparm nolightmap
	cull disable
	deformVertexes autosprite
	{
		clampmap textures/flares/newflare.tga
		blendfunc add
		rgbGen wave sawtooth 0 1 0 2 
		tcMod stretch inversesawtooth 0 1 0 2 
		tcMod rotate 333
	}
	{
		clampmap textures/flares/lava.tga
		blendfunc add
		rgbGen wave sawtooth 0 1 0 0.5 
		tcMod stretch inversesawtooth 0 1 0 0.5 
		tcMod rotate -333
	}
	{
		clampmap textures/flares/newflare.tga
		blendfunc add
		rgbGen wave sawtooth 0 1 0 1.25 
		tcMod stretch inversesawtooth 0 1 0 1.25 
		tcMod rotate 500
	}
	{
		clampmap textures/flares/portalflare.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 4 
		tcMod stretch sawtooth 0 1 0 4 
		tcMod rotate 1323
	}
}

teleptThat
{
	q3map_lightimage textures/effects/jumpcirc.tga
	surfaceparm nolightmap
	cull disable
	deformVertexes autosprite
	q3map_surfacelight 300
	{
		clampmap textures/effects/jumpcirc.tga
		blendfunc add
		rgbGen wave sawtooth 0 1 0 2 
		tcMod stretch inversesawtooth 0 1 0 2 
		tcMod rotate 333
	}
	{
		clampmap textures/effects/jumpcirc.tga
		blendfunc add
		rgbGen wave sawtooth 0 1 0 0.5 
		tcMod stretch inversesawtooth 0 1 0 0.5 
		tcMod rotate -333
	}
	{
		clampmap textures/effects/jumpcirc.tga
		blendfunc add
		rgbGen wave sawtooth 0 1 0 1.25 
		tcMod stretch inversesawtooth 0 1 0 1.25 
		tcMod rotate 500
	}
}

