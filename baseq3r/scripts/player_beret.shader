models/players/beret/cooldude
{
	{
		map models/players/beret/cooldude.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
}

models/players/beret/headcrash1
{
	{
		map models/players/beret/headcrash1.tga
		rgbGen lightingDiffuse
	}
}

models/players/beret/headcrash2
{
	{
		map models/players/beret/headcrash1.tga
		rgbGen lightingDiffuse
		alphaGen lightingSpecular
	}
	{
		map textures/oafx/flare.tga
		blendfunc add
		tcMod rotate 4
		tcGen environment 
		alphaGen lightingSpecular
	}
}

