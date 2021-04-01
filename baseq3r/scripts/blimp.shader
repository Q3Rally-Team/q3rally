// ------------------------------------------------------------
// Shaders for Q3Rally Blimp - rewritten by P3rlE (fixed by insellium)
// based on the blimp.shader from stecki
// ------------------------------------------------------------

// ------------------------------------------------------------
// 1.Blimp (green)
// ------------------------------------------------------------

models/mapobjects/blimp/green
{
	{
		map models/mapobjects/blimp/green.tga
//		blendfunc gl_one gl_one
		rgbGen wave sin .5 .5 0 .5	
	}
}

// ------------------------------------------------------------
// 2.Blimp (red)
// ------------------------------------------------------------

models/mapobjects/blimp/red
{
	{
		map models/mapobjects/blimp/red.tga
//		blendfunc gl_one gl_one
		rgbGen wave sin .5 .5 0 .5	
	}
}

// ------------------------------------------------------------
// 3.Blimp (blimp02)
// ------------------------------------------------------------

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