models/players/police/police_rlite_red
{
	surfaceparm trans
	cull disable
	deformVertexes autosprite
	deformVertexes move 0 0.1 0 sin -2 47 0 5 
	{
		map models/players/police/police_rlite_red.tga
		blendfunc gl_src_alpha gl_one
		rgbGen identity
		alphaGen wave square 0 1 0 4 
	}
}

models/players/police/police_rlite_blue
{
	surfaceparm trans
	cull disable
	deformVertexes autosprite
	deformVertexes move 0 0.1 0 sin -2 47 0 5 
	{
		map models/players/police/police_rlite_blue.tga
		blendfunc gl_src_alpha gl_one
		rgbGen identity
		alphaGen wave square 0 1 0 4 
	}
}

models/players/police/xpreflect
{
	{
		map models/players/police/xpreflect.tga
		tcGen environment
	}
}

models/players/police/body_skin
{
	{
		map models/players/police/preflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/police/body_skin.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen lightingDiffuse
	}
	{
		map models/players/police/body_skin_alpha.tga
		blendfunc blend
		rgbGen identity
	}
}

models/players/police/lights_alpha
{
	cull disable
	{
		map models/players/police/lights_alpha.tga
		blendfunc add
		rgbGen lightingDiffuse
	}
	{
		map models/players/police/lights.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen lightingDiffuse
	}
}

models/players/police/lights_alphab
{
	cull disable
	{
		map models/players/police/lights_alphab.tga
		blendfunc add
		rgbGen lightingDiffuse
	}
	{
		map models/players/police/lights_blue.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen lightingDiffuse
	}
}

models/players/police/lights_alphar
{
	cull disable
	{
		map models/players/police/lights_alphar.tga
		blendfunc add
		rgbGen lightingDiffuse
	}
	{
		map models/players/police/lights_red.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen lightingDiffuse
	}
}

models/players/police/zeroskin
{
	{
		map models/players/police/zeroskin.tga
		blendfunc add
		rgbGen lightingDiffuse
	}
}

models/players/police/body_skinw
{
	{
		map models/players/police/preflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/police/body_skinw.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen lightingDiffuse
	}
}

models/players/police/body_skinb
{
	{
		map models/players/police/preflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/police/body_skinb.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen lightingDiffuse
	}
}