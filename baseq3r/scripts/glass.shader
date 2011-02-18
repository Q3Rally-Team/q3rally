textures/q3r_glass/glassbrick1A
{
	qer_editorimage textures/q3r_glass/glassbrick1A.tga
	{
		map textures/effects/tinfx.tga
		rgbGen identity
		tcMod scale 0.5 0.5
		tcGen environment 
	}
	{
		map textures/q3r_glass/glassbrick1A.tga
		blendfunc blend
		rgbGen identity
		alphaFunc GE128
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
	}
}

textures/q3r_glass/glassbrick2A
{
	qer_editorimage textures/q3r_glass/glassbrick2A.tga
	{
		map textures/effects/tinfx.tga
		rgbGen identity
		tcMod scale 0.5 0.5
		tcGen environment 
	}
	{
		map textures/q3r_glass/glassbrick2A.tga
		blendfunc blend
		rgbGen identity
		alphaFunc GE128
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
	}
}

textures/q3r_glass/blueglass
{
	surfaceparm nolightmap
	surfaceparm nonsolid
	cull twosided
	{
		map textures/q3r_glass/blueglass.tga
		tcGen environment
		tcMod turb 0 0.01 0 0.01	        
		tcmod scroll .0 .0
		blendfunc GL_ONE GL_ONE
                }
}



textures/q3r_glass/darkglass
                      {
                      qer_trans .5
                      surfaceparm trans
//		      surfaceparm solid

                      cull disable
                      {
                      map textures/effects/tinfx.tga
                      tcgen environment
                      blendfunc gl_one gl_one
                      rgbGen identity
                      }
                      {
                      map $lightmap
                      blendFunc gl_dst_color gl_zero
                      rgbgen identity
                      }
                      }


textures/q3r_glass/plainglass
{
	surfaceparm nolightmap
	surfaceparm nonsolid
	cull twosided
	{
		map textures/q3r_glass/plainglass.tga
		tcGen environment
		blendfunc GL_ONE GL_ONE
                }
}
