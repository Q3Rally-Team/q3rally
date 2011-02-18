dflareShader
{
	{
		clampmap textures/flares/flarey.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate -3
	}
	{
		clampmap textures/flares/flarey.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate 7
	}
}

flareShader
{
	{
		clampmap textures/flares/newflare.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate 3
	}
	{
		clampmap textures/flares/newflare.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate -2
	}
}

sun
{
	{
		clampmap textures/flares/flarey.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate -3
	}
	{
		clampmap textures/flares/flarey.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate 7
	}
}

flareShader-glow
{
	{
		clampmap textures/flares/largeglow.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate -3
	}
	{
		clampmap textures/flares/largeglow.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate 7
	}
}

flareShader-wide
{
	{
		clampmap textures/flares/wide.tga
		blendfunc add
		rgbGen Vertex
	}
}

textures/sfx/flameflare
{
	{
		clampmap textures/flares/lava.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate 57
	}
	{
		clampmap textures/flares/lava.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate -95
	}
}

textures/flares/lava
{
	{
		clampmap textures/flares/lava.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate 12
	}
	{
		clampmap textures/flares/lava.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate -5
	}
}

flareShader-lava
{
	{
		clampmap textures/flares/lava.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate 12
	}
	{
		clampmap textures/flares/lava.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate -5
	}
}

flareShader-portal
{
	{
		clampmap textures/flares/portalflare.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate 9
		tcMod stretch sin 0.5 -0.5 0 0.2 
	}
	{
		clampmap textures/flares/portalflare.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate -17
		tcMod stretch sin 0 1 0 0.2 
	}
	{
		clampmap textures/flares/flarey.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate 1
	}
}

flareShader-twilightflare
{
	{
		clampmap textures/flares/twilightflare.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate 12
	}
	{
		clampmap textures/flares/twilightflare.tga
		blendfunc add
		rgbGen Vertex
		tcMod rotate -5
	}
}

flareShader-subtle
{
	{
		clampmap textures/flares/twilightflare.tga
		blendfunc add
		rgbGen oneminusvertex
		tcMod rotate 12
	}
}
