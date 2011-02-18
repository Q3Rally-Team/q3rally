textures/czest3ctf/fir_branch
{
	qer_editorimage textures/czest3ctf/fir_branch.tga
	surfaceparm nomarks
	surfaceparm nonsolid
	cull disable
	{
		map textures/czest3ctf/fir_branch.tga
		depthWrite
		alphaFunc GE128
	}
	{
		map $lightmap
		blendfunc filter
		tcGen lightmap
		depthFunc equal
	}
}

models/mapobjects/czest2ctf/lamphanggrate
{
	cull none
	{
		map models/mapobjects/czest2ctf/lamphanggrate.tga
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

textures/czest2ctf/sundownsky
{
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_lightsubdivide 256
	q3map_globaltexture
	q3map_sun 1 0.8 0.7 70 70 30
	q3map_sun 1 0.8 0.7 30 250 30
	skyParms full 700 -
	{
		map textures/czest2ctf/redclouds.jpg
		tcMod scroll -0.013 0.04
	}
	{
		map textures/skies/dimclouds.tga
		blendfunc add
		tcMod scroll 0.02 0.07
	}
}

textures/czest2ctf/darkfog
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.05 0.05 0.05 ) 1024
}
