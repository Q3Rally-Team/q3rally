models/mapobjects/blimp/green
{
	{
		map models/mapobjects/blimp/green.tga
//		blendfunc gl_one gl_one
		rgbGen wave sin .5 .5 0 .5	
	}
}

models/mapobjects/blimp/red
{
	{
		map models/mapobjects/blimp/red.tga
//		blendfunc gl_one gl_one
		rgbGen wave sin .5 .5 0 .5	
	}
}
models/mapobjects/blimp/blimp01
{
	{
		map models/mapobjects/blimp/blimp01.tga
		rgbGen lightingDiffuse	
	}
}
models/mapobjects/blimp/blimp02
{
	{
		map models/mapobjects/blimp/q3r.tga
		tcMod scroll .2 0
		rgbGen identity	
	}
	{
		map models/mapobjects/blimp/blimp02.tga
		blendFunc blend
		rgbGen vertex	
	}



}