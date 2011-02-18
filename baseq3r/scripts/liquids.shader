textures/q3r_liquids/q3r_water_01
{
	qer_editorimage textures/q3r_liquids/water.tga
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	fogparms ( 0.215686 0.278431 0.333333 ) 1024
	{
		map textures/q3r_liquids/water.tga
		blendfunc filter
		tcMod scroll 0.05 0.05
	}
	{
		map textures/q3r_liquids/water.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll -0.03 -0.06
	}
	{
		map textures/q3r_liquids/water.tga
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

textures/q3r_liquids/q3r_water_02
{
	qer_editorimage textures/q3r_liquids/water.tga
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	deformVertexes wave 512 sin 0 7 0 0.4
	fogparms ( 0.215686 0.278431 0.333333 ) 1024
	{
		map textures/q3r_liquids/water_04.tga
		blendfunc filter
		tcMod scroll 0.01 0.01
		alphaGen wave inversesawtooth 6 0 0 7
	}
	{
		map textures/q3r_liquids/water_11.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll -0.01 -0.03
		alphaGen wave inversesawtooth 1 0 0 2
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
	}
}

textures/q3r_liquids/q3r_water_03
{
	qer_editorimage textures/q3r_liquids/water.tga
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	deformVertexes wave 512 sin 0 7 0 0.4
	fogparms ( 0.215686 0.278431 0.333333 ) 1024
	{
		map textures/q3r_liquids/water_11.tga
		blendfunc filter
		tcMod scroll 0.01 0.01
		alphaGen wave inversesawtooth 6 0 0 7
	}
	{
		map textures/q3r_liquids/water_12.tga
		blendfunc add
		rgbGen const ( 0.501961 0.501961 0.501961 )
		tcMod scroll -0.01 -0.03
		alphaGen wave inversesawtooth 1 0 0 2
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
	}
}

textures/q3r_liquids/q3r_lavahell
{
	qer_editorimage textures/q3r_liquids/q3r_lavahell.tga
	q3map_globaltexture
	surfaceparm lava
	surfaceparm nodlight
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_surfacelight 600
	cull disable
	
	tesssize 128
	cull disable
	deformVertexes wave 100 sin 3 2 .1 0.1
	
	{
		map textures/q3r_liquids/q3r_lavahell.tga
		tcMod turb 0 .2 0 .1
		tcMod scroll 0.0 -0.5
	}
	
}

// lava for q3r --tbb
textures/q3r_liquids/lava_1a
{
	qer_editorimage textures/q3r_liquids/lava_1a.tga
	q3map_globaltexture
	surfaceparm nodlight
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm lava
	q3map_surfacelight 300
	cull disable
	surfaceparm fog
	
	tesssize 128
	deformVertexes wave 32 sin 1 2 .1 0.1
	
	{
		map textures/q3r_liquids/lava_1a.tga
		tcMod turb 0 .2 0 .1
		tcMod scroll 0.0 0.25
	}
	
}

// rotating fireball --tbb
textures/q3r_liquids/fireball_rot2
{
	qer_editorimage textures/q3r_liquids/fireball_1a.tga
	surfaceparm nodlight
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm lava
	cull disable
	DeformVertexes autosprite
	{
		map textures/q3r_liquids/fireball_1a.tga
		//clampmap textures/q3r_liquids/fireball_1a.tga
		blendfunc gl_src_alpha gl_one
		//blendfunc filter
		//tcmod transform 1 1 -1 -1 1 1
		tcMod rotate 180
	}
}

// rotating fireball --tbb
textures/q3r_liquids/fireball_rot1
{
	qer_editorimage textures/q3r_liquids/fireball_1a.tga
	surfaceparm nodlight
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm lava
	cull disable
	{
		map textures/q3r_liquids/fireball_1a.tga
		blendfunc gl_src_alpha gl_one
		tcMod rotate 180
	}
}
textures/q3r_liquids/fireball_rotani1
{
	qer_editorimage textures/q3r_liquids/fireball_1a.tga
	surfaceparm nodlight
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm lava
	cull disable
	{
		animmap 9 textures/q3r_liquids/fireball_1a.tga textures/q3r_liquids/fireball_1b.tga textures/q3r_liquids/fireball_1c.tga
		blendfunc gl_src_alpha gl_one
	}
}
