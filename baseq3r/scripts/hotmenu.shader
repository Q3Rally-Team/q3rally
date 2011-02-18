//REALLY COOL MENU BACKGROUNDY
menuback_blueish
{
	{
		map gfx/newbg/bluefiller.tga
		rgbGen identity
		tcMod turb 0 0.6 0 0.125
		tcMod rotate 6
		tcMod scroll 1 0
	}
	{
		map gfx/newbg/bluefiller.tga
		blendfunc add
		rgbGen identity
		tcMod turb 0 0.4 0 0.325
		tcMod rotate -12
		tcMod scroll -1 0
	}
	{
		clampmap gfx/newbg/blackfading.tga
		blendfunc gl_zero gl_one_minus_src_alpha
		rgbGen identity
		tcMod stretch sin 1.5 0.2 0 0.1 
		tcMod rotate 7
	}
	{
		clampmap gfx/newbg/blackfading.tga
		blendfunc blend
		tcMod stretch sin 1.3 0.2 0 0.12 
		tcMod rotate -17
		alphaGen wave sin 0.3 0 0 1 
	}
	{
		map textures/sfx/jcb2.tga
		blendfunc add
		rgbGen wave triangle 0 0.2 0 0.1 
		tcMod rotate 360
		tcMod stretch sin 2.3 0 0 1 
	}
	{
		videomap video/idlogo.roq
		blendfunc gl_dst_color gl_one_minus_dst_color
		rgbGen const ( 0.482353 0.384314 0.615686 )
	}
}

