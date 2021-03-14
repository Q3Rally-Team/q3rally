// ------------------------------------------------------------
// flags (n_flag)
// ------------------------------------------------------------

models/flags/n_flag
{
	cull disable
	deformVertexes wave 80 sin 2 8 0 3 
	deformVertexes wave 20 square 0 2 0.5 0.2 
	{
		map textures/sfx/proto_zzztwhite.tga
		rgbGen identity
		tcMod turb 1 1 0.5 0.3
		tcGen environment 
	}
	{
		map models/flags/n_flag.tga
		rgbGen lightingDiffuse
		alphaFunc GE128
	}
	{
		map textures/effects/flagenv.tga
		blendfunc gl_dst_color gl_src_color
		tcGen environment 
	}
}