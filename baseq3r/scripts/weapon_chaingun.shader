vulcanFlash
{
	cull disable
	deformVertexes bulge 3 3 76
	{
		clampmap models/weapons/vulcan/muzzy.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 30 
		tcMod rotate 1554
	}
	{
		clampmap models/weapons/vulcan/muzzy.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 30 
		tcMod rotate -2640
	}
	{
		clampmap models/weapons/vulcan/muzzy.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 30 
		tcMod rotate 7402
		tcMod stretch sin 0 1 0 1 
	}
	{
		clampmap models/weapons/vulcan/muzzy.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 30 
		tcMod rotate -6403
		tcMod stretch sin 0.5 0.5 0 1 
	}
}

models/weapons/vulcan/vulcan
{
	{
		map models/weapons/vulcan/vulcan.tga
		rgbGen lightingDiffuse
	}
}

