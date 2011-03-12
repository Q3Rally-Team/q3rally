models/weapons2/gauntlet/skin
{
	{
		map textures/base_wall/bluemetal2_shiny.tga
		rgbGen lightingDiffuse
	}
}

models/weapons2/gauntlet/saw
{
	{
		map textures/skies/topclouds.tga
		rgbGen lightingDiffuse
		tcMod rotate 4
		tcGen environment 
	}
	{
		map textures/effects/tinfx2c.tga
		blendfunc add
		rgbGen lightingDiffuse
		tcMod rotate -8
		tcGen environment 
	}
}

//this shader rules. made my lightsaber look 93% of the one in the movie. (episode IV)
//basically a bunch of sprites that go across the blade of the saber.
//i put many of them across to avoid spoiling that it's really a bunch of sprites,
//but it looks like a true smooth glow to the viewer.
models/weapons2/gauntlet/flare
{
	deformVertexes autosprite
	{
		map models/weapons2/gauntlet/glow.tga
		blendfunc add
		rgbGen wave sin 0.08 0.02 0 15 
	}
}

//this is the blade part of the saber. no sprites used here.
models/weapons2/gauntlet/glow
{
	{
		map models/weapons2/gauntlet/glow.tga
		blendfunc add
		rgbGen wave sin 1 0.1 0 15 
	}
}

