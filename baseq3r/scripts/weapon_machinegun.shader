models/weapons2/machinegun/f_machinegun2
{
	cull disable
	{
		clampmap models/weapons2/machinegun/f_machinegun2.tga
		blendfunc add
		rgbGen identity
		tcMod rotate 5288
	}
}

models/weapons2/machinegun/f_machinegun3
{
	deformVertexes autosprite
	{
		clampmap models/weapons2/machinegun/f_machinegun3.tga
		blendfunc add
		rgbGen identity
		tcMod rotate 8288
	}
}

models/weapons2/machinegun/muzzlside
{
	cull disable
	{
		map models/weapons2/machinegun/muzzlside.tga
		blendfunc add
	}
}

models/weapons2/shells/mgunshell
{
	cull disable
	{
		map models/weapons2/shells/mgunshell.tga
		rgbGen lightingDiffuse
		
	}
}


models/weapons2/shells/mgunshell_2
{
	deformVertexes autosprite
    {
        clampmap models/weapons2/shells/mgunshell_2
        rgbGen lightingDiffuse
	blendFunc blend
	tcMod scale 1.4 1.4
    }

}

shellCase
{
	cull disable
	{
		map models/weapons2/shells/sgunshell.tga
		rgbGen lightingDiffuse
	}
	{
		map models/weapons2/shells/mgunshell.tga
		blendfunc add
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/weapons2/shells/sgunshell.tga
		blendfunc blend
		rgbGen lightingDiffuse
	}
}

models/weapons2/machinegun/muzzlecenter
{
	deformVertexes autosprite
	{
		map models/weapons2/machinegun/f_machinegun.tga
		blendfunc add
		tcMod rotate 1246
	}
	{
		map models/weapons2/machinegun/f_machinegun.tga
		blendfunc add
		tcMod rotate -1246
	}
}

models/weapons2/machinegun/flare
{
	deformVertexes autosprite
	{
		map models/weapons2/machinegun/flare.tga
		blendfunc add
	}
}

models/weapons2/machinegun/skin
{
	{
		map models/weapons2/machinegun/mgun.tga
		rgbGen lightingDiffuse
	}
}

models/weapons2/machinegun/sight
{
	cull disable
	{
		map models/weapons2/machinegun/sight.tga
		rgbGen lightingDiffuse
		alphaFunc GE128
	}
}

