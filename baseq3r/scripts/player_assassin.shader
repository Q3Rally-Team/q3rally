//*******************************
//DEFAULT_SKIN:
//*******************************
models/players/assassin/upper
{
	cull disable
	{
		map models/players/assassin/upper.tga
		rgbGen lightingDiffuse
	}
}
models/players/assassin/lower
{
	cull disable
	{
		map models/players/assassin/lower.tga
		rgbGen lightingDiffuse
		alphaFunc GE128
	}
}

//*******************************
//BLUE_SKIN:
//*******************************
models/players/assassin/upper-blue
{
	cull disable
	{
		map models/players/assassin/upper-blue.tga
		rgbGen lightingDiffuse
	}
}
models/players/assassin/lower-blue
{
	cull disable
	{
		map models/players/assassin/lower-blue.tga
		rgbGen lightingDiffuse
		alphaFunc GE128
	}
}

//*******************************
//RED_SKIN:
//*******************************
models/players/assassin/upper-red
{
	cull disable
	{
		map models/players/assassin/upper-red.tga
		rgbGen lightingDiffuse
	}
}
models/players/assassin/lower-red
{
	cull disable
	{
		map models/players/assassin/lower-red.tga
		rgbGen lightingDiffuse
		alphaFunc GE128
	}
}

//*******************************
//GHOST_SKIN:
//*******************************
models/players/assassin/upper_ghost
{
	{
		map models/players/assassin/upper_ghost.tga
//		alphaFunc GE128
        blendfunc blend
        rgbGen wave sin 0.8 .2 0 0.5
	}
    {
		
        map textures/effects/envmap2.tga
        tcMod scroll -.2 -.05
        tcgen environment
		blendFunc GL_ONE GL_ONE
		rgbGen identity
        rgbGen wave sin 0.9 .1 0 0.05
    }
}
models/players/assassin/lower_ghost
{
 	{
		map models/players/assassin/lower_ghost.tga
//		alphaFunc GE128
        blendfunc blend
        rgbGen wave sin 0.8 .2 0 0.5
	}
    {
		
        map textures/effects/envmap2.tga
        tcMod scroll -.2 -.05
        tcgen environment
		blendFunc GL_ONE GL_ONE
		rgbGen identity
        rgbGen wave sin 0.9 .1 0 0.05
    }
}
