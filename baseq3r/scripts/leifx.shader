//Various subtle smoke effects, animated, designed to trail things and come off walls
leismoke1
{
	{
		clampmap sprites/le/smoke1.tga
		blendfunc blend
		rgbGen Vertex
		tcMod rotate 22
		tcMod stretch sin 0.5 0 0 1 
		alphaGen Vertex
	}
	{
		clampmap sprites/le/smoke2.tga
		blendfunc blend
		rgbGen Vertex
		tcMod rotate -55
		tcMod stretch sin 0.7 0 0 1 
		alphaGen Vertex
		detail
	}
}

leismoke2
{
	{
		clampmap sprites/le/smoke2.tga
		blendfunc blend
		rgbGen Vertex
		tcMod rotate 22
		tcMod stretch sin 0.7 0 0 1 
		alphaGen Vertex
	}
	{
		clampmap sprites/le/smoke3.tga
		blendfunc blend
		rgbGen Vertex
		tcMod rotate -55
		tcMod stretch sin 0.4 0 0 1 
		alphaGen Vertex
		detail
	}
}

leismoke3
{
	{
		clampmap sprites/le/smoke3.tga
		blendfunc blend
		rgbGen Vertex
		tcMod rotate 22
		tcMod stretch sin 0.6 0 0 1 
		alphaGen Vertex
	}
	{
		clampmap sprites/le/smoke4.tga
		blendfunc blend
		rgbGen Vertex
		tcMod rotate -55
		tcMod stretch sin 0.3 0 0 1 
		alphaGen Vertex
		detail
	}
}

leismoke4
{
	{
		clampmap sprites/le/smoke4.tga
		blendfunc blend
		rgbGen Vertex
		tcMod rotate 22
		tcMod stretch sin 0.4 0 0 1 
		alphaGen Vertex
	}
	{
		clampmap sprites/le/smoke1.tga
		blendfunc blend
		rgbGen Vertex
		tcMod rotate -55
		tcMod stretch sin 0.7 0 0 1 
		alphaGen Vertex
		detail
	}
}

//Splash effect for bullets entering the water
leisplash
{
	{
		clampmap sprites/le/splash.tga
		blendfunc gl_src_alpha gl_one
		rgbGen Vertex
		tcMod rotate -95
		tcMod stretch sin 0.4 0 0 1 
		alphaGen Vertex
	}
	{
		clampmap sprites/le/splash.tga
		blendfunc gl_src_alpha gl_one
		rgbGen Vertex
		tcMod rotate 79
		tcMod stretch sin 0.7 0 0 1 
		alphaGen Vertex
	}
}

//Boom sparks from explosions
leiboom1
{
	{
		clampmap sprites/le/blast.tga
		blendfunc gl_src_alpha gl_one
		tcMod stretch sin 0.6 0 0 1 
		tcMod rotate -88
		alphaFunc GE128
	}
	{
		clampmap sprites/le/blast.tga
		blendfunc gl_src_alpha gl_one
		tcMod stretch sin 0.8 0 0 1 
		tcMod rotate 122
		alphaFunc GE128
	}
	{
		clampmap sprites/le/glaw.tga
		blendfunc gl_src_alpha gl_one
		rgbGen const ( 0.886275 0.34902 0.321569 )
		alphaGen Vertex
	}
}

//Blood particle (from trailing gib or impact of bullet)
leiblood1
{
	{
		clampmap sprites/le/blood.tga
		blendfunc blend
		rgbGen const ( 0.372549 0.372549 0.372549 )
		tcMod rotate 3
		tcMod stretch sin 0.7 0 0 1 
		alphaGen Vertex
	}
	{
		clampmap sprites/le/blood.tga
		blendfunc blend
		rgbGen const ( 0.615686 0.141176 0.141176 )
		tcMod rotate -7
		tcMod stretch sin 0.4 0 0 1 
		alphaGen Vertex
		detail
	}
}

//Mark for blood particle
leiblood2
{
	polygonoffset
	{
		clampmap sprites/le/blood3.tga
		blendfunc gl_zero gl_one_minus_src_color
		rgbGen identity
	}
}

//preliminary fireball boom
leifball
{
	{
		clampmap sprites/le/glaw.tga
		blendfunc add
		rgbGen identity
	}
}

//plasma trail
leiptrail
{
	{
		clampmap sprites/le/ptrail.tga
		blendfunc gl_src_alpha gl_one
		rgbGen const ( 0.713726 0.819608 0.913726 )
		tcMod stretch sin 0.6 0 0 1 
		tcMod rotate -22
		alphaFunc GE128
	}
	{
		clampmap sprites/le/ptrail.tga
		blendfunc gl_src_alpha gl_one
		rgbGen const ( 0.337255 0.4 0.611765 )
		tcMod stretch sin 0.8 0 0 1 
		tcMod rotate 6
	}
}


// The crap that comes out of walls
leispark
{
	{
		clampmap sprites/le/spark2.tga
		blendfunc gl_src_alpha gl_one
		rgbGen identity
		//alphaGen Vertex
	}
}

// Metal Crap is brighter
leispark2
{
	{
		clampmap sprites/le/spark2.tga
		blendfunc gl_src_alpha gl_one
			rgbGen identity
		tcMod stretch sin 0.4 0.3 0 5 
		alphaGen Vertex
	}
	{
		clampmap sprites/le/spark2.tga
		blendfunc gl_src_alpha gl_one
			rgbGen identity
		tcMod stretch sin 0.7 0.3 0 3 
		alphaGen Vertex
	}
}
