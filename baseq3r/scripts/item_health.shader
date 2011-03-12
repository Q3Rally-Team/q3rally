smallCross
{
	{
		map textures/oafx/greenchrm.tga
		tcMod rotate -76
		tcGen environment 
	}
	{
		map textures/oafx/greenchrm.tga
		blendfunc add
		tcMod rotate 54
		tcGen environment 
	}
}

mediumCross
{
	{
		map textures/effects/envmapligh.tga
		tcMod rotate -76
		tcGen environment 
	}
	{
		map textures/effects/envmapyel.tga
		blendfunc add
		tcMod rotate 54
		tcGen environment 
	}
}

largeCross
{
	{
		map textures/oafx/orangechrm.tga
		tcMod rotate -76
		tcGen environment 
	}
	{
		map textures/oafx/orangechrm.tga
		blendfunc add
		tcMod rotate 54
		tcGen environment 
	}
}

megaCross
{
	{
		map textures/effects/envmapblue2.tga
		tcMod rotate -76
		tcGen environment 
	}
	{
		map textures/effects/megahelth.tga
		blendfunc add
		tcMod rotate 54
		tcGen environment 
	}
}

smallCrossFlare
{
	deformVertexes autosprite
	{
		clampmap textures/oafx/greenflr1.tga
		blendfunc add
		tcMod rotate -149
		tcMod stretch sin -1 1 0 0.2 
	}
	{
		clampmap textures/oafx/greenflr2.tga
		blendfunc add
		tcMod rotate 1765
		tcMod stretch sin 0 -1.2 0 0.3 
	}
}

oldflare
{
	deformVertexes autosprite
	{
		clampmap textures/oafx/flare.tga
		blendfunc add
		tcMod rotate 12
	}
	{
		clampmap textures/oafx/flare.tga
		blendfunc add
		tcMod rotate -17
	}
}

oldysunFlare
{
	deformVertexes autosprite
	{
		clampmap textures/oafx/flare.tga
		blendfunc add
		tcMod rotate 12
	}
	{
		clampmap textures/oafx/flare.tga
		blendfunc add
		tcMod rotate -17
	}
}

largeCrossFlare
{
	deformVertexes autosprite
	{
		clampmap textures/oafx/orangeflr1.tga
		blendfunc add
		tcMod rotate -46
		tcMod stretch sin -1 0.4 0 0.2 
	}
	{
		clampmap textures/oafx/orangeflr2.tga
		blendfunc add
		tcMod rotate 122
		tcMod stretch inversesawtooth -0.7 -1 0 2 
	}
}

