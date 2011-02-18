wake
{
	cull disable
	{
		animmap 30 sprites/multwake1.tga sprites/multwake2.tga sprites/multwake3.tga sprites/multwake4.tga sprites/multwake5.tga sprites/multwake6.tga sprites/multwake7.tga sprites/multwake8.tga 
		blendfunc gl_dst_color gl_src_color
	}
}

newishwake
{
	{
		animmap 8 sprites/splash6.tga sprites/splash5.tga sprites/splash4.tga sprites/splash3.tga sprites/splash2.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave inversesawtooth 0 1 0 8 
	}
	{
		animmap 8 sprites/splash5.tga sprites/splash4.tga sprites/splash3.tga sprites/splash2.tga sprites/splash6.tga 
		blendfunc gl_src_alpha gl_one
		alphaGen wave sawtooth 0 1 0 8 
	}
}

gfx/misc/tracer
{
	cull disable
	{
		map gfx/misc/tracer2.tga
		blendfunc add
	}
}


gfx/damage/bullet_mrk
{
	polygonoffset
	{
		map gfx/damage/bulletmult.tga
		blendfunc gl_dst_color gl_src_color
		alphaGen Vertex
	}
}

oldgfx/damage/bullet_mrk
{
	polygonoffset
	{
		map gfx/damage/bullet_mrk.tga
		blendfunc gl_zero gl_one_minus_src_color
		rgbGen Vertex
	}
}

gfx/damage/burn_med_mrk
{
	polygonoffset
	{
		map gfx/damage/burn_med_mrk.tga
		blendfunc gl_zero gl_one_minus_src_color
		rgbGen Vertex
	}
}

gfx/damage/hole_lg_mrk
{
	polygonoffset
	{
		map gfx/damage/hole_lg_mrk.tga
		blendfunc gl_zero gl_one_minus_src_color
		rgbGen Vertex
	}
}

gfx/damage/plasma_mrk
{
	polygonoffset
	{
		map gfx/damage/plasma_mrk.tga
		blendfunc blend
		rgbGen Vertex
		alphaGen Vertex
	}
}

markShadow
{
	polygonoffset
	{
		map gfx/damage/shadow.tga
		blendfunc gl_zero gl_one_minus_src_color
		rgbGen Vertex
	}
}

projectionShadow
{
	polygonoffset
	deformvertexes projectionshadow		//need this
	{
		map $whiteimage
		blendfunc filter
		rgbGen wave square 0 0 0 0 
	}
}

oldwake
{
	{
		clampmap sprites/splash.tga
		blendfunc add
		rgbGen wave sin 0.7 0.3 0.25 0.5 
		tcMod rotate 250
		tcMod stretch sin 0.9 0.1 0 0.7 
	}
	{
		clampmap sprites/splash.tga
		blendfunc add
		rgbGen wave sin 0.7 0.3 0.25 0.4 
		tcMod rotate -230
		tcMod stretch sin 0.9 0.05 0 0.9 
	}
}


waterBubble
{
	sort underwater
	cull disable
	{
		map sprites/bubble.tga
		blendfunc blend
		rgbGen Vertex
		alphaGen Vertex
	}
}

