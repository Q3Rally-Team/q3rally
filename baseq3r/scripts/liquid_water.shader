textures/liquids/hydrowater
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	fogparms ( 0.215686 0.278431 0.333333 ) 1024
	{
		map textures/liquids/pool2.tga
		blendfunc filter
		tcMod scroll 0.05 0.05
	}
	{
		map textures/liquids/pool3d_4b2.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll -0.03 -0.06
	}
	{
		map textures/liquids/pool3d_4b2.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcGen environment 
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
	}
}


textures/liquids/islewater
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	fogparms ( 0.345098 0.513726 0.788235 ) 1024
	{
		map textures/liquids/pool2.tga
		blendfunc filter
		tcMod scroll 0.05 0.05
	}
	{
		map textures/liquids/pool3d_4b2.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll -0.03 -0.06
	}
	{
		map textures/liquids/pool3d_4b2.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcGen environment 
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
	}
}


textures/ice/coldwater
{
	qer_editorimage textures/ice/coldwater.tga
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm water
	cull disable
	qer_trans 0.5
	q3map_globaltexture
	{
		map textures/ice/coldwater.tga
		blendfunc filter
		tcMod scroll 0.01 0.01
	}
	{
		map textures/effects/tinfx2.tga
		blendfunc add
		tcGen environment 
	}
	{
		map textures/ice/coldwater.tga
		blendfunc add
		tcMod scroll 0.04 0.01
	}
}

textures/liquids/vorwater
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	{
		map textures/liquids/vorwater.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll -0.009 -0.005
	}
	{
		map textures/liquids/vorwater.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll 0.004 -0.007
	}
}

textures/savdm6ish/#teleport
{
	q3map_lightimage textures/liquids/pool3d_3.tga
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	q3map_surfacelight 900
	q3map_flare flareShader-portal
	{
		map textures/liquids/pool3d_5e.tga
		rgbGen const ( 0.478431 0.294118 0.172549 )
		tcMod scroll 0.05 0.05
	}
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc add
		rgbGen const ( 0.517647 0.305882 0.2 )
		tcMod rotate 8
		tcMod turb 0 1 0 0.04
	}
	{
		map textures/liquids/pool3d_5.tga
		blendfunc add
		rgbGen const ( 0.584314 0.376471 0.25098 )
		tcMod scroll 0.07 0.01
	}
}

textures/liquids/tele
{
	q3map_lightimage textures/liquids/pool3d_3.tga
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	q3map_surfacelight 900
	q3map_flare flareShader-portal
	{
		map textures/liquids/pool3d_5e.tga
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll 0.05 0.05
		tcMod rotate 545
	}
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll -0.03 -0.06
		tcMod rotate 8
		tcMod turb 0 1 0 0.04
	}
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll 0.07 0.01
		tcMod rotate -3
		tcMod turb 0 4 0 0.01
	}
}

textures/liquids/refwater
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	portal
	{
		map textures/liquids/vorwater.tga
		blendfunc filter
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll 0.05 0.05
	}
	{
		map textures/liquids/pool3d_3.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll -0.03 -0.06
	}
	{
		map textures/liquids/pool3d_3.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll 0.07 0.01
	}
}

//updated for current textures 02-15-07 dmn_clown
textures/liquids/waveywater
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	{
		map textures/liquids/pool2.jpg
		blendfunc filter
		rgbGen const ( 1 1 1 )
		tcMod scroll 0.05 0.05
	}
	{
		map textures/liquids/ripple.jpg
		blendfunc gl_dst_color gl_one
		rgbGen const ( 1 1 1 )
		tcMod scroll -0.03 -0.06
	}
	{
		map textures/liquids/ripple.jpg
		blendfunc gl_dst_color gl_one
		rgbGen const ( 1 1 1 )
		tcMod scroll -0.02 -0.08
	}
}
//updated for current textures 02-15-07 dmn_clown
textures/liquids/justwater
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	{
		map textures/liquids/pool3d_4b2.jpg
		blendfunc blend
		rgbGen const ( 0.384314 0.501961 0.568627 )
		tcMod scroll 0.05 -0.05
		alphaGen wave inversesawtooth 0.5 0 0 1 
	}
	{
		map textures/liquids/pool3d_3.jpg
		blendfunc gl_dst_color gl_one
		rgbGen const ( 1 1 1 )
		tcMod scroll 0.09 0.06
	}
	{
		map textures/liquids/pool3d_5.tga
		blendfunc gl_dst_color gl_one
		rgbGen const ( 1 1 1 )
		tcMod scroll -0.04 -0.08
	}
}

//updated for current textures 02-15-07 dmn_clown
textures/liquids/rapidwater
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	{
		map textures/liquids/ripple.jpg
		blendfunc gl_dst_color gl_src_color
		tcMod scroll -0.2 -0.4
	}
	{
		map textures/liquids/ripple.jpg
		blendfunc gl_dst_color gl_src_color
		tcMod scroll -0.2 -0.5
	}
}

textures/liquids/softwater
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	{
		map textures/liquids/pool2.tga
		blendfunc filter
		tcMod scroll 0.05 0.05
	}
	{
		map textures/liquids/pool2.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll -0.03 -0.06
	}
	{
		map textures/liquids/pool2.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll 0.07 0.01
	}
}

//added for backwards compat
//02-15-07 dmn_clown

textures/liquids/clear_ripple3
{
	qer_editorimage textures/liquids/pool3d_3.jpg
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm water
	cull disable
	deformVertexes wave 64 sin .25 .25 0 .5	
		
	{ 
		map textures/liquids/pool3d_5.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll -.05 .001
	}

	{ 
		map textures/liquids/pool3d_6.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .025 -.001
	}
	{ 
		map textures/liquids/pool3d_5e.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .001 .025
	}
	{
		map $lightmap
		blendFunc gl_dst_color gl_zero
		rgbgen identity		
	}
	

}

textures/liquids/clear_ripple1
{
	qer_editorimage textures/liquids/pool3d_3.jpg
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm water
	cull disable
	deformVertexes wave 64 sin .5 .5 0 .5	
		
	{ 
		map textures/liquids/pool3d_5.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll -.05 .001
	}

	{ 
		map textures/liquids/pool3d_6.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .025 -.001
	}
	{
		map $lightmap
		blendFunc gl_dst_color gl_zero
		rgbgen identity		
	}
	

}

textures/liquids/clear_ripple2
{
	qer_editorimage textures/liquids/pool3d_3.jpg
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm water
	cull disable
	deformVertexes wave 64 sin .25 .25 0 .5	
		
	{ 
		map textures/liquids/pool3d_5.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll -.025 .001
	}

	{ 
		map textures/liquids/pool3d_6.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .03 -.001
	}
	{ 
		map textures/liquids/pool3d_5e.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .001 .025
	}
	{
		map $lightmap
		blendFunc gl_dst_color gl_zero
		rgbgen identity		
	}
	

}

textures/liquids/clear_calm1
{
	qer_editorimage textures/liquids/pool3d_5e.jpg
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm water
	cull none
	
		
	{ 
		map textures/liquids/pool3d_5e.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod turb .04 .01 .5 .03
	}

	{ 
		map textures/liquids/pool3d_6.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .025 -.001
	}
	{ 
		map textures/liquids/pool3d_5.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .001 .025
	}
	{
		map $lightmap
		blendFunc gl_dst_color gl_zero
		rgbgen identity		
	}
	

}

textures/liquids/calm_poollight
{
	qer_editorimage textures/liquids/pool3d_3.jpg
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm water
	cull disable
	q3map_surfacelight 100
	deformVertexes wave 64 sin .25 .25 0 .5	
		
	{ 
		map textures/liquids/pool3d_5.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll -.05 .001
	}

	{ 
		map textures/liquids/pool3d_6.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .025 -.001
	}
	{ 
		map textures/liquids/pool3d_5e.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .001 .025
	}
	{
		map $lightmap
		blendFunc gl_dst_color gl_zero
		rgbgen identity		
	}
	

}

textures/liquids/clear_calm2
{
	qer_editorimage textures/liquids/pool3d_5e.jpg
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm water
	cull none
	
	{ 
		map textures/liquids/pool3d_6.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .025 -.001
	}
	{ 
		map textures/liquids/pool3d_5.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .001 .025
	}
	{
		map $lightmap
		blendFunc gl_dst_color gl_zero
		rgbgen identity		
	}
	{
		map textures/effects/sky.jpg
		tcgen environment
		blendfunc add
		rgbgen vertex
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

textures/liquids/hydrowater2
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	deformVertexes wave 256 sin 0 7 0 0.4
	fogparms ( 0.215686 0.278431 0.333333 ) 1024
	{
		map textures/liquids/pool2.tga
		blendfunc filter
		tcMod scroll 0.05 0.05
	}
	{
		map textures/liquids/pool3d_4b2.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll -0.03 -0.06
	}
	{
		map textures/liquids/pool3d_4b2.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcGen environment 
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
	}
}

textures/liquids2/clear_calm1v
{
	qer_editorimage textures/liquids/pool3d_5e.jpg
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm water
	cull none
	fogparms ( 0.215686 0.278431 0.333333 ) 512
	{ 
		map textures/liquids/pool3d_5e.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod turb .04 .01 .5 .03
	}

	{ 
		map textures/liquids/pool3d_6.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .025 -.001
	}
	{ 
		map textures/liquids/pool3d_5.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .001 .025
	}
	{ 
		map textures/effects/sky.jpg
		blendFunc add
		rgbgen const ( .5 .5 .5 )
		tcgen environment
	}
	{
		map $lightmap
		blendFunc filter
		rgbgen identity		
	}
}

textures/liquids2/clear_ripple1_q3dm1v
{
	qer_editorimage textures/liquids/pool3d_5e.jpg
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm water
	cull none
	fogparms ( 0.215686 0.278431 0.333333 ) 512
	deformvertexes wave 128 sin .5 .5 .5 .5
	{ 
		map textures/liquids/pool3d_5e.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod turb .04 .01 .5 .03
	}

	{ 
		map textures/liquids/pool3d_6.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .025 -.001
	}
	{ 
		map textures/liquids/pool3d_5.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .001 .025
	}
	{ 
		map textures/effects/sky.jpg
		blendFunc add
		rgbgen const ( .5 .5 .5 )
		tcgen environment
	}
	{
		map $lightmap
		blendFunc filter
		rgbgen identity		
	}
}

textures/liquids2/clear_ripple1_q3dm1light
{
	qer_editorimage textures/liquids/pool3d_5e.jpg
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm water
	cull none
	q3map_surfacelight 100
	fogparms ( 0.215686 0.278431 0.333333 ) 1024
	deformvertexes wave 256 sin .5 .5 .5 .5
	{ 
		map textures/liquids/pool3d_5e.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod turb .04 .01 .5 .03
	}

	{ 
		map textures/liquids/pool3d_6.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .025 -.001
	}
	{ 
		map textures/liquids/pool3d_5.jpg
		blendFunc gl_dst_color gl_one
		rgbgen identity
		tcmod scroll .001 .025
	}
	{ 
		map textures/effects/sky.jpg
		blendFunc add
		rgbgen const ( .5 .5 .5 )
		tcgen environment
	}
	{
		map $lightmap
		blendFunc filter
		rgbgen identity		
	}
}

////////////////////////////////////////////
///////////////// slimey ///////////////////
////////////////////////////////////////////

textures/liquids/acid
{
	q3map_lightimage textures/liquids/slime7.tga
	surfaceparm nomarks
	surfaceparm slime
	surfaceparm trans
	surfaceparm water
	cull disable
	deformVertexes wave 256 sin 0 9 0 0.4 
	tessSize 128
	q3map_surfacelight 900
	{
		map textures/effects/tinfx2.tga
		blendfunc add
		tcGen environment 
	}
	{
		map textures/liquids/slime7c.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll -0.03 -0.06
	}
	{
		map textures/liquids/slime7.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll 0.07 0.01
	}
}

textures/liquids/oldacid
{
	q3map_lightimage textures/liquids/slime7.tga
	surfaceparm nomarks
	surfaceparm slime
	surfaceparm trans
	surfaceparm water
	cull disable
	deformVertexes wave 32 sin 0 3 0 0.2 
	deformVertexes wave 64 sin 0 7 0 0.6 
	q3map_surfacelight 333
	{
		map textures/liquids/slime7.tga
		blendfunc filter
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll 0.05 0.05
	}
	{
		map textures/effects/tinfx2.tga
		blendfunc add
		tcGen environment 
	}
	{
		map textures/liquids/slime7c.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll -0.03 -0.06
	}
	{
		map textures/liquids/slime7.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll 0.07 0.01
	}
}

textures/liquids/slime1
{
	qer_editorimage textures/liquids/slime8.jpg
	q3map_lightimage textures/liquids/slime8.jpg
	surfaceparm noimpact
	surfaceparm slime
	surfaceparm nolightmap
	surfaceparm trans
	q3map_surfacelight 150
	deformVertexes wave 64 sin .25 .25 0 .5	
	{
		map textures/liquids/slime8.jpg
		tcmod turb .05 -0.5 0 0.02
		tcmod scroll .05 -.01
	}
	{
		map textures/liquids/slime8.jpg
		blendfunc filter
		tcmod turb .012 -0.1 0 0.04
		tcmod scroll .003 -.008
	}
}

textures/liquids/slime1_2000
{
	qer_editorimage textures/liquids/slime8.jpg
	q3map_lightimage textures/liquids/slime8.jpg
	surfaceparm noimpact
	surfaceparm slime
	surfaceparm nolightmap
	surfaceparm trans
	q3map_surfacelight 2000
	deformVertexes wave 64 sin .25 .25 0 .5	
	{
		map textures/liquids/slime8.jpg
		tcmod turb .05 -0.5 0 0.02
		tcmod scroll .05 -.01
	}
	{
		map textures/liquids/slime8.jpg
		blendfunc filter
		tcmod turb .012 -0.1 0 0.04
		tcmod scroll .003 -.008
	}
}

textures/liquids/proto_poolpass
{
	q3map_globaltexture
	qer_editorimage textures/liquids/proto_gruel3.jpg
	q3map_surfacelight 300
	surfaceparm slime
	surfaceparm trans
	{
		map textures/liquids/proto_gruel3.jpg
		tcmod turb .1 .3 .5 .1
	}
	{
		map textures/liquids/proto_gruel2.jpg
		blendfunc add
		tcmod turb 012 -0.1 0 0.04
	}
	{
	        map $lightmap
		rgbgen identity
		blendfunc filter
        }
	{
		map textures/sfx/kenelectric.jpg
		blendfunc add
		tcmod scale .4 .4
		tcmod scroll .4 -.4
		tcmod rotate 500
	}
}

textures/liquids/slime2
{
	qer_editorimage textures/liquids/slime10.jpg
	q3map_lightimage textures/liquids/slime8.jpg
	surfaceparm noimpact
	surfaceparm slime
	surfaceparm nolightmap
	surfaceparm trans
	q3map_surfacelight 150
	deformVertexes wave 64 sin .25 .25 0 .5	
	{
		map textures/liquids/slime8.jpg
		tcmod turb .05 -0.5 0 0.02
		tcmod scroll .05 -.01
	}
	{
		map textures/liquids/slime10.jpg
		blendfunc filter
		tcmod scale .3 .3
		tcmod turb .012 -0.1 0 0.04
		tcmod scroll .003 -.008
	}
	{
		map textures/liquids/proto_gruel3.jpg
		blendfunc add
		tcmod scale .9 .9
		tcmod turb 012 -0.1 0 0.04
	}
}

textures/liquids/slime3
{
	qer_editorimage textures/liquids/slime9.jpg
	q3map_lightimage textures/liquids/slime9.jpg
	surfaceparm noimpact
	surfaceparm slime
	surfaceparm nolightmap
	surfaceparm trans
	q3map_surfacelight 150
	deformVertexes wave 64 sin .25 .25 0 .5	
	{
		map textures/liquids/slime9.jpg
		tcmod scale .5 .5
		tcmod turb .05 -0.5 0 0.02
		tcmod scroll .05 -.01
	}
	{
		map textures/liquids/slime9.jpg
		blendfunc filter
		tcmod scale .3 .3
		tcmod turb .012 -0.1 0 0.04
		tcmod scroll .003 -.008
	}
}
