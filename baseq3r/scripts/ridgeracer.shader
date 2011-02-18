textures/ridgeracer/waterwaveshort_01
{
	qer_editorimage textures/ridgeracer/waterwave1.tga
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
//	deformvertexes wave 16 sin 0 5 0 1.5
//	tesssize 16
	{
		map textures/ridgeracer/waterwave1.tga
		blendfunc filter
		tcmod scale .5 .5
		tcmod scroll .8 .9
		depthwrite
	}
	{
		map textures/ridgeracer/waterwave1.tga
		blendfunc filter
		tcmod scale .5 .5
		tcmod scroll .3 .4
	}
}
textures/ridgeracer/skyrise
{
	qer_editorimage textures/ridgeracer/sky_rise.tga
	cull disable
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nodlight
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_surfacelight 250
	q3map_lightsubdivide 512
	{
		map textures/ridgeracer/sky_rise.tga
	}
//	{
//		map $lightmap
//		rgbgen identity
//	}
}

textures/ridgeracer/skyrise_yel
{
	qer_editorimage textures/ridgeracer/beach_01.tga
	cull disable
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nodlight
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_surfacelight 200
	q3map_lightsubdivide 1024
	{
		map textures/ridgeracer/beach_01.tga
		tcmod scale 0.5 0.5
	}
}

textures/ridgeracer/skyset
{
	qer_editorimage textures/ridgeracer/sky_set.tga
	cull disable
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nodlight
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_surfacelight 10
	q3map_lightsubdivide 256
	{
		map textures/ridgeracer/sky_set.tga
	}
}
textures/ridgeracer/clouds_01
{
	qer_editorimage textures/ridgeracer/clouds_01.tga
	cull disable
	q3map_globaltexture
	//q3map_sun 1 1 1 100 0 90
	q3map_surfacelight 100
	q3map_lightsubdivide 1024
	surfaceparm nomarks
	surfaceparm noimpact
	surfaceparm nodlight
	surfaceparm nolightmap
	//surfaceparm trans
	//surfaceparm alphashadow
	{
		map textures/ridgeracer/clouds_01.tga
		//depthwrite
		//blendfunc filter
		//blendfunc blend
		tcmod scale .2 .2
		tcmod scroll .2 .3
	}
}