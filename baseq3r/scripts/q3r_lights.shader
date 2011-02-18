//quake 3 rally lights

//////////////////////////////
//	orange blinking light	//
//	contruction	light	//
//////////////////////////////
textures/q3r_lights/orange_blink
{
	qer_editorimage textures/q3r_lights/construct_circle.tga
	surfaceparm nomarks
	surfaceparm nodlight
	cull disable
	{
		map textures/q3r_lights/construct_circle.tga
		rgbgen wave inversesawtooth 0 1 0 .5
		blendfunc gl_src_alpha gl_one
	}
}

//////////////////////////////////////////
//	orange blinking light	180deg phase//
//	contruction	light		//
//////////////////////////////////////////
textures/q3r_lights/oran_blink180
{
	qer_editorimage textures/q3r_lights/construct_circle.tga
	surfaceparm nomarks
	surfaceparm nodlight
	cull disable
	{
		map textures/q3r_lights/construct_circle.tga
		rgbgen wave inversesawtooth 0 1 .5 .5
		blendfunc gl_src_alpha gl_one
	}
}

//////////////////////////////////
//	orange blinking arrow light	//
//	contruction	light	//
//////////////////////////////////
textures/q3r_lights/arrows_oran
{
	qer_editorimage textures/q3r_lights/arrows_oran0.tga
	surfaceparm nomarks
	surfaceparm nodlight
	//q3map_surfacelight 20
	cull disable
	q3map_lightimage textures/q3r_lights/arrows_oran0.tga
	{
		animmap 1/3 textures/q3r_lights/arrows_oran0.tga  textures/q3r_lights/arrows_oran1.tga textures/q3r_lights/arrows_oran2.tga
		rgbgen wave inversesawtooth 0 1 0 1/3
		//blendfunc add
	}
}

//////////////////////////////////
//	orange sliding arrow light	//
//	contruction light		//
//////////////////////////////////
textures/q3r_lights/arrow_slide
{
	qer_editorimage textures/q3r_lights/arrows_oran0.tga
	surfaceparm nomarks
	surfaceparm nodlight
	//q3map_surfacelight 20
	cull disable
	q3map_lightimage textures/q3r_lights/arrows_oran0.tga
	{
		map textures/q3r_lights/arrows_oran0.tga
		rgbgen wave square 0 1 0 3
		tcmod scroll .3333 0
		//blendfunc add
	}
}

//***********************************************************************
//////////////////////////////////////
//	color shifting light for sky	//
//	epic fail just use as sky light 	//
//////////////////////////////////////
textures/q3r_lights/skycoloryellow
{
	qer_editorimage textures/q3r_layout/yellow.tga
	surfaceparm nomarks
	surfaceparm nodlight
	q3map_surfacelight 50
	cull disable
	//q3map_lightimage textures/q3r_layout/yellow.tga
	{
		map textures/q3r_layout/yellow.tga
		//rgbgen wave square 0 1 0 .1
		//blendfunc gl_src_alpha gl_one
		//depthwrite
	}
}

textures/q3r_lights/skycolororange
{
	qer_editorimage textures/q3r_layout/orange.tga
	surfaceparm nomarks
	surfaceparm nodlight
	q3map_surfacelight 50
	cull disable
	{
		map textures/q3r_layout/orange.tga
		//rgbgen wave square 0 1 0 .05
		//blendfunc gl_src_alpha gl_one
		//depthwrite
	}
}

//this is a bullshit darkness light, try it if you want to
textures/q3r_lights/skycolorlila
{
	qer_editorimage textures/q3r_layout/lila.tga
	surfaceparm nomarks
	surfaceparm nodlight
	q3map_surfacelight 50
	cull disable
	{	
		map textures/q3r_layout/lila.tga
		//rgbgen wave square 0 1 0 .05
		//blendfunc gl_src_alpha gl_one
		//depthwrite
	}
}

//these are the lights that are used in q3r_dm02 which doesn't have any
textures/base_light/baslt4_1_2k
{
	qer_editorimage textures/base_light/baslt4_1_2k.jpg
	surfaceparm nomarks
	surfaceparm nodlight
	q3map_surfacelight 2000
	cull disable
	{	
		map textures/base_light/baslt4_1_2k.jpg
	}
}

//these are the lights that are used in q3r_dm02 which doesn't have any
textures/base_light/baslt4_1_4k
{
	qer_editorimage textures/base_light/baslt4_1_4k.jpg
	surfaceparm nomarks
	surfaceparm nodlight
	q3map_surfacelight 4000
	cull disable
	{	
		map textures/base_light/baslt4_1_4k.jpg
	}
}

//these are the lights that are used in q3r_dm02 which doesn't have any
textures/base_light/proto_lightmap
{
	qer_editorimage textures/base_light/proto_lightmap.jpg
	surfaceparm nomarks
	surfaceparm nodlight
	q3map_surfacelight 4000
	cull disable
	{	
		map textures/base_light/proto_lightmap.jpg
	}
}
//simple blue surface light
textures/q3r_lights/bluesurfacelit
{
	qer_editorimage textures/q3r_lights/sky_rise.tga
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nodlight
	q3map_surfacelight 2000
	q3map_lightsubdivide 512
	cull disable
	{
		map textures/q3r_lights/sky_rise.tga
	}
//	{
//		map $lightmap
//		rgbgen identity
//	}
}
//simple orange surface light
textures/q3r_lights/oransurfacelit
{
	qer_editorimage textures/ridgeracer/sky_set.tga
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nodlight
	q3map_surfacelight 2000
	q3map_lightsubdivide 256
	cull disable
	{
		//map $lightmap
		map textures/ridgeracer/sky_set.tga
	}
}
