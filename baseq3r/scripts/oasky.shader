skyMoon
{
	{
		map textures/skies/topclouds.tga
		tcMod scroll -0.02 0.02
		tcMod scale 2 2
	}
	{
		map textures/skies/topclouds.tga
		blendfunc add
		tcMod scroll 0.01 0.01
		tcMod scale 0.7 0.7
	}
	{
		map textures/skies/multsky2.tga
		blendfunc filter
	}
}

skyClouds
{
	deformVertexes autosprite
	{
		map textures/skies/moonspr.tga
		blendfunc add
	}
}

textures/skies/industrysky
{
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_lightsubdivide 256
	q3map_surfacelight 400
	q3map_globaltexture
	q3map_sun 0.882353 0.921569 0.941177 150 45 55
	skyParms full 800 -
	{
		map $whiteimage 
		rgbGen const ( 0.137255 0.172549 0.227451 )
	}
	{
		map textures/skies/topclouds.tga
		blendfunc add
		rgbGen const ( 0.219608 0.396078 0.588235 )
		tcMod scroll -0.004 0.004
		tcMod scale 2 2
	}
}

textures/skies/moonsky
{
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_lightsubdivide 256
	q3map_surfacelight 19
	q3map_globaltexture
	q3map_sun 0.901961 0.92549 0.92549 150 297 90
	skyParms full 800 -
	{
		map textures/skies/stars.tga
		rgbGen const ( 0.407843 0.392157 0.356863 )
		tcMod scale 5 5
		detail
	}
	{
		map textures/skies/topclouds.tga
		blendfunc add
		rgbGen const ( 0.262745 0.301961 0.313726 )
		tcMod scroll -0.008 0.008
		tcMod scale 2 2
	}
}

// SKY ROOM SHADER
//-- for models/sky/skyisland.md3 --
skyIsland
{
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_lightsubdivide 256
	q3map_surfacelight 400
	q3map_globaltexture
	q3map_sun 0.901961 0.552941 0.407843 121 149 33
	skyParms full 700 -
	{
		map $whiteimage 
		rgbGen const ( 0.309804 0.121569 0.0666667 )
	}
}

textures/skyIsland
{
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_lightsubdivide 256
	q3map_surfacelight 45
	q3map_globaltexture
	q3map_sun 0.588235 0.243137 0.0941177 121 149 26
	skyParms full 700 -
	{
		map $whiteimage 
		rgbGen const ( 0.305882 0.12549 0.0705882 )
	}
}

skyIsland_cloud
{
	surfaceparm nolightmap
	sort sky
	{
		map textures/skies/justgradient.tga
		blendfunc blend
		rgbGen const ( 0.454902 0.223529 0.113725 )
	}
	{
		map textures/skies/islandclouds.tga
		blendfunc add
		rgbGen const ( 0.521569 0.317647 0.247059 )
		tcMod scroll -0.01 0
	}
	{
		map textures/skies/islandclouds.tga
		blendfunc add
		rgbGen const ( 0.380392 0.152941 0.152941 )
		tcMod scroll -0.007 0
	}
}

skyIsland_water
{
	surfaceparm nolightmap
	cull disable
	{
		map $whiteimage 
		rgbGen const ( 0.129412 0.0470588 0.027451 )
	}
}

skyIsland_waterwave
{
	surfaceparm nolightmap
	sort 12
	{
		map $whiteimage 
		blendfunc blend
		rgbGen const ( 0.129412 0.0470588 0.027451 )
		alphaGen wave sin 0.7 0 0 0.2 
	}
	{
		map textures/skies/skywater.tga
		rgbGen const ( 0.129412 0.0470588 0.027451 )
		tcMod scale 8 4
		tcMod scroll -0.4 0
		alphaFunc GE128
		alphaGen wave sin 0.6 0 0 0.2 
	}
	{
		map textures/skies/skywater.tga
		rgbGen const ( 0.129412 0.0470588 0.027451 )
		tcMod scale 4 8
		tcMod scroll 0.1 0
		alphaFunc GE128
	}
}

skyIsland_islands
{
	surfaceparm nolightmap
	sort 10
	{
		map textures/skies/islandys.tga
		rgbGen const ( 0.129412 0.0509804 0.027451 )
		alphaFunc GE128
	}
}

skyIsland_sun
{
	surfaceparm nolightmap
	sort 11
	{
		map textures/skies/sun.tga
		blendfunc add
	}
	{
		clampmap textures/flares/newflare.tga
		blendfunc add
		rgbGen const ( 0.560784 0.376471 0.203922 )
		tcMod rotate 4
	}
	{
		clampmap textures/flares/newflare.tga
		blendfunc add
		rgbGen const ( 0.560784 0.0862745 0.0862745 )
		tcMod rotate -7
	}
}

textures/skies/hellsky
{
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_lightsubdivide 256
	q3map_surfacelight 400
	q3map_globaltexture
	q3map_sun 1 1 0.5 150 30 60
	skyParms full 700 -
	{
		map textures/skies/dimclouds.tga
		tcMod scroll -0.013 0.04
	}
	{
		map textures/skies/dimclouds.tga
		blendfunc add
		tcMod scroll 0.02 0.07
	}
}

textures/skies/hellsky2
{
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_lightsubdivide 256
	q3map_surfacelight 400
	q3map_globaltexture
	q3map_sun 1 1 0.5 150 30 60
	skyParms full 700 -
	{
		map textures/skies/dimclouds.tga
		tcMod scroll -0.013 0.04
	}
	{
		map textures/skies/dimclouds.tga
		blendfunc add
		tcMod scroll 0.02 0.07
	}
}

textures/skies/hellsky2bright
{
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_lightsubdivide 256
	q3map_surfacelight 400
	q3map_globaltexture
	q3map_sun 1 1 0.5 150 30 60
	skyParms full 700 -
	{
		map textures/skies/inteldimclouds.tga
		tcMod scroll -0.013 0.04
	}
	{
		map textures/skies/topclouds.tga
		blendfunc add
		tcMod scroll 0.02 0.07
	}
}

textures/skies/nitesky
{
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_lightsubdivide 256
	q3map_surfacelight 20
	q3map_globaltexture
	q3map_sun 0.333333 0.45098 0.576471 155 360 90
	skyParms full 1280 -
	{
		map textures/skies/stars.tga
		tcMod scale 3 3
	}
	{
		map textures/skies/nitesky.tga
		blendfunc add
		tcMod scroll 0.02 0.07
	}
}

black
{
	cull disable
	{
		map gfx/colors/black.tga
		rgbGen const ( 0 0 0 )
	}
}

textures/sky_void
{
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	{
		map gfx/colors/black.tga
		rgbGen const ( 0 0 0 )
	}
}

textures/black
{
	cull disable
	{
		map gfx/colors/black.tga
		rgbGen const ( 0 0 0 )
	}
}

textures/skies/toxicskytim_dm8
{
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_lightsubdivide 256
	q3map_surfacelight 400
	q3map_globaltexture
	q3map_sun 1 1 0.5 150 30 60
	skyParms full 700 -
	{
		map textures/skies/dimclouds.tga
		tcMod scroll -0.013 0.04
	}
	{
		map textures/skies/dimclouds.tga
		blendfunc add
		tcMod scroll 0.02 0.07
	}
}

textures/space1_skybox
{
	qer_editorimage textures/bubctf1/bubctf1_view.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_lightsubdivide 1024
	q3map_surfacelight 33
	q3map_globaltexture
	q3map_sun 0.5 0.5 0.5 66 105 78
	skyParms env/space1 128 -
}

textures/common/sky_space
{
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_surfacelight 50
	skyParms env/space1 128 -
}

//01-30-07 dmn_clown
textures/skies/blacksky
{
	qer_editorimage textures/sfx/blakk.tga
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm sky
	q3map_surfacelight 50
	q3map_sun	1 1 1  32  90  90
	{
		map	textures/sfx/blakk.tga
	}
}

textures/skies/xblacksky_dm7
{
	qer_editorimage textures/sfx/blakk.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 50
	q3map_sun	1 1 1  32  60  90
	{
		map	textures/sfx/blakk.tga
	}
}

textures/sfx/blackness
{	
	qer_editorimage textures/sfx/blakk.tga
	surfaceparm nolightmap
	surfaceparm noimpact
	surfaceparm nomarks
	{
		map textures/sfx/blakk.tga
	}
}

//02-21-07 dmn_clown
textures/skies/q3dm0
{
	qer_editorimage textures/skies/dimclouds.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_lightimage textures/skies/skywater.tga
	q3map_sun	1 1 1  32  90  90
	skyParms full 512 -
	{
		map textures/skies/dimclouds.tga
		tcmod scale 2 3
		tcmod scroll -.03 .04
	}
	{
		map textures/skies/inteldimclouds.jpg
		blendfunc add
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/tim_hell
{
	qer_editorimage textures/skies/intelredclouds.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_lightimage textures/skies/skywater.tga
	q3map_sun	1 1 1  32  60  90
	skyParms full 1024 -
	{
		map textures/skies/intelredclouds.jpg
		tcmod scale 2 3
		tcmod scroll -.03 .04
	}
	{
		map textures/skies/topclouds.tga
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/toxicskytim_dm5
{
	qer_editorimage textures/skies/dimclouds.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_lightimage textures/skies/skywater.tga
	q3map_sun	1 1 1  32  60  90
	skyParms full 756 -
	{
	map textures/skies/dimclouds.jpg
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/intelredclouds.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/killsky
{
	qer_editorimage textures/skies/killsky_1.jpg
	q3map_lightimage textures/skies/skywater.tga
	surfaceparm nomarks
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_sun	1 1 1  32  45  45
	skyParms full 512 -
	{
		map textures/skies/intelredclouds.jpg
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/killsky_1.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/xtoxicsky_dm9
{
	qer_editorimage textures/skies/intelredclouds.jpg
	q3map_lightimage textures/skies/skywater.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_sun	1 1 1  32  45  45
	skyParms full 512 -
	{
		map textures/skies/intelredclouds.jpg
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/intelredclouds.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/toxicskytim_dm2
{
	qer_editorimage textures/skies/intelredclouds.jpg
	q3map_lightimage textures/skies/skywater.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_sun	1 1 1  32  60  90
	skyParms full 756 -
	{
		map textures/skies/intelredclouds.jpg
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/topclouds.tga
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/xtoxicskytim_q3dm5
{
	qer_editorimage textures/skies/killsky_1.jpg
	q3map_lightimage textures/skies/skywater.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_sun	1 1 1  32  60  60
	skyParms full 512 -
	{
		map textures/skies/killsky_1.jpg
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/topclouds.tga
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/pj_arena4_sky
{
	qer_editorimage textures/skies/inteldimclouds.jpg
	q3map_lightimage textures/skies/skywater.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_sun	1 1 1  32  60  45
	skyParms full 756 -
	{
		map textures/skies/inteldimclouds.jpg
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/topclouds.tga
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/toxicskytim_dm7
{
	qer_editorimage textures/skies/multsky1.tga
	q3map_lightimage textures/skies/skywater.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 150
	q3map_globaltexture
	q3map_sun	1 1 1  32  60  35
	skyParms full 1024 -
	{
		map textures/skies/multsky1.tga
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/intelredclouds.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/skybox
{
	qer_editorimage textures/skies/moons1.jpg
	q3map_lightimage textures/skies/skywater.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_sun	1 1 1  32  60  90
	skyParms env/space1 512 -
	{
		map textures/skies/inteldimclouds.jpg
		blendfunc add
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/intelredclouds.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/pj_dm9sky
{
	qer_editorimage textures/skies/intelredclouds.jpg
	q3map_lightimage textures/skies/skywater.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_sun	1 1 1  32  25  90
	skyParms full 1024 -
	{
		map textures/skies/intelredclouds.jpg
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/intelredclouds.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/pj_dm10sky
{
	qer_editorimage textures/skies/bluedimclouds.jpg
	q3map_lightimage textures/skies/skywater.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 50
	q3map_globaltexture
	q3map_sun	1 1 1  32  50  90
	skyParms full 756 -
	{
		map textures/skies/bluedimclouds.jpg
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/nitesky.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/tim_dm3_red
{
	qer_editorimage textures/skies/killsky_1.jpg
	q3map_lightimage textures/skies/killsky_1.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 150
	q3map_globaltexture
	q3map_sun	0.639 0.129 0.128  32  75  90
	skyParms full 512 -
	{
		map textures/skies/killsky_1.jpg
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/killsky_1.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/toxicskytim_dm9
{
	qer_editorimage textures/skies/intelredclouds.jpg
	q3map_lightimage textures/skies/killsky_1.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_sun	0.639 0.129 0.128  32  90  90
	skyParms full 756 -
	{
		map textures/skies/intelredclouds.jpg
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/intelredclouds.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/tim_dm14red
{
	qer_editorimage textures/skies/killsky_1.jpg
	q3map_lightimage textures/skies/killsky_1.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 150
	q3map_globaltexture
	q3map_sun	0.639 0.129 0.128  32  45  45
	skyParms full 512 -
	{
		map textures/skies/killsky_1.jpg
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/intelredclouds.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/nightsky_xian_dm1
{
	qer_editorimage textures/skies/pjbasesky.jpg
	q3map_lightimage textures/skies/nitesky.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 40
	q3map_globaltexture
	q3map_sun	0.078 0.109 0.384  32  90  90
	skyParms full 512 -
	{
		map textures/skies/pjbasesky.jpg
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/nitesky.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/xtoxicsky_tourney
{
	qer_editorimage textures/skies/intelredclouds.jpg
	q3map_lightimage textures/skies/intelredclouds.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_sun	0.514 0.549 0.203  32  60  75
	skyParms - 756 -
	{
		map textures/skies/intelredclouds.jpg
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/bluedimclouds.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/tim_km2
{
	qer_editorimage textures/skies/intelredclouds.jpg
	q3map_lightimage textures/skies/intelredclouds.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_sun	0.514 0.549 0.203  32  45  60
	skyParms full 512 -
	{
		map textures/skies/intelredclouds.jpg
		tcmod scale 2 3
		tcmod scroll -.01 .02
	}
	{
		map textures/skies/intelredclouds.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/pjbasesky_arena1_sky
{
	qer_editorimage textures/skies/killsky_1.jpg
	q3map_lightimage textures/skies/intelredclouds.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_sun	0.639 0.129 0.128  32  90  90
	skyParms full 512 -
	{
		map textures/skies/killsky_1.jpg
		tcmod scale 2 3
		tcmod scroll -.03 .01
	}
	{
		map textures/skies/intelredclouds.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/toxicskydim
{
	qer_editorimage textures/skies/intelredclouds.jpg
	q3map_lightimage textures/skies/skywater.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 100
	q3map_globaltexture
	q3map_sun	1 1 1  32  15  15
	skyParms full 512 -
	{
		map textures/skies/intelredclouds.jpg
		tcmod scale 2 3
		tcmod scroll -.03 .01
	}
	{
		map textures/skies/intelredclouds.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 0.03
	}
}

textures/skies/toxicskytim_blue
{
	qer_editorimage textures/skies/toxicskytim_blue.jpg
	q3map_lightimage textures/skies/dimclouds.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_surfacelight 150
	q3map_globaltexture
	q3map_sun	0.145 0.113 0.752  32  90  90
	skyParms full 512 -
	{
		map textures/skies/dimclouds.jpg
		tcmod scale 2 3
		tcmod scroll -.03 .01
	}
	{
		map textures/skies/intelredclouds.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.04 -0.03
	}

}

textures/skies/pj_ctf2_sky
{
	qer_editorimage textures/skies/pj_ctf2_sky.jpg
	q3map_lightimage textures/skies/skywater.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_globaltexture
	q3map_sun 	1 1 1  32  45  90
	q3map_surfacelight 100
	skyParms full 1024 -
	{
		map textures/skies/dimclouds.jpg
		tcmod scale 2 3
		tcmod scroll -.03 .01
	}
	{
		map textures/skies/intelredclouds.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 -0.03
	}
}

textures/skies/xtoxicsky_q3ctf3
{
	qer_editorimage textures/skies/xtoxicsky_q3ctf3.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_lightimage textures/skies/intelredclouds.jpg
	q3map_sun	0.4 0.5 0.3  50  90  90
	q3map_surfacelight 100
	skyParms env/moon1/moon1 1024 -
	{
		map textures/skies/dimclouds.jpg
		blendfunc add
		tcmod scale 2 3
		tcmod scroll -.03 .01
	}
	{
		map textures/skies/intelredclouds.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.05 -0.03
	}
}

textures/skies/earthsky01
{
	qer_editorimage textures/skies/earthsky01
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_sun 0.933333 0.541176 03.13725 60 160 11
	q3map_surfacelight 80 //Diffuse light
	skyParms env/earthsky/earthsky 1024 -
}

textures/skies/moons1
{
	qer_editorimage textures/skies/moons1.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_lightimage textures/skies/intelredclouds.jpg
	q3map_surfacelight 50
	q3map_sun	.5 .1 .1  50  65  35
	skyParms env/moon1/moon1 1024 -
	{
		map textures/skies/killsky_1.jpg
		blendFunc add
		tcMod scale 3 2
		tcMod scroll 0.15 0.15
	}
	{
		map textures/skies/intelredclouds.jpg
		blendFunc filter
		tcMod scale 3 3
		tcMod scroll 0.05 0.05
	}
	
}

textures/skies/tim_tourney2
{
	qer_editorimage textures/skies/s18.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_lightimage textures/skies/s17.tga
	q3map_surfacelight 100
	q3map_sun .5 .5 .5 75 65 45
	skyparms - 1024 -
	{
		map textures/skies/s18.tga
		tcmod scale 2 3
		tcmod scroll 0.03 0.03
	}
	{
		map textures/skies/s14.tga
		blendfunc add
		tcmod scale 3 2
		tcmod scroll 0.015 0.015
	}
}

textures/skies/mkc_ctf1
{
	qer_editorimage textures/skies/s8.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_lightimage textures/skies/s9.tga
	q3map_surfacelight 100
	q3map_sun .5 .5 .5 75 65 45
	skyparms - 512 -
	{
		map textures/skies/s8.tga
		tcmod scale 3 2
		tcmod scroll 0.02 0.03
	}
	{
		map textures/skies/s9.tga
		blendfunc filter
		tcmod scale 2 3
		tcmod scroll 0.015 0.025
	}

}

textures/skies2/ctf6sky
{
	qer_editorimage textures/skies/s3.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_lightimage textures/skies/s4.tga
	q3map_surfacelight 100
	q3map_sun .5 .5 .5 35 65 45
	skyparms - 256 -
	{
		map textures/skies/s3.tga
		tcmod scale 2 3
		tcmod scroll 0.018 -0.025
	}
	{
		map textures/skies/s4.tga
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.023 -0.016
	}
	{
		map textures/skies/s14.tga
		blendfunc filter
		tcmod scale 2 3
		tcmod scroll 0.015 0.025
	}
		
}

textures/skies/terrain1
{
	qer_editorimage textures/skies/s18.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_lightimage textures/skies/s1.tga
	q3map_surfacelight 100
	q3map_sun .5 .5 .5 95 75 45
	skyparms - 2048 -
	{
		map textures/skies/s18.tga
		tcmod scale 3 2 
		tcmod scroll -0.018 -0.025
	}
	{
		map textures/skies/s12.tga
		blendfunc filter
		tcmod scale 2 3
		tcmod scroll -0.008 -0.009
	}
	{
		map textures/skies/clouds.tga
		blendfunc add
		tcmod scale 2 3
		tcmod scroll -0.013 -0.008
	}
}

textures/skies2/ctf6sky_ter
{
	qer_editorimage textures/skies/s20.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_lightimage textures/skies/s5.tga
	q3map_surfacelight 100
	q3map_sun .5 .5 .5 35 65 45
	skyparms - 1024 -
	{
		map textures/skies/s2.tga
		tcmod scale 2 3
		tcmod scroll 0.018 -0.025
	}
	{
		map textures/skies/s6.tga
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll 0.023 -0.016
	}
	{
		map textures/skies/topclouds.tga
		blendfunc add
		tcmod scale .9 .7
		tcmod scroll -0.013 -0.008
	}	
}

textures/skies2/nebula2
{
	qer_editorimage textures/skies/nebulae.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_lightimage textures/skies/nebulae.jpg
	q3map_surfacelight 100
	q3map_sun .5 .5 .5 35 65 45
	skyparms env/nebulae/nebulae 2048 -
}

textures/skies2/nebula
{
	qer_editorimage textures/skies/nebulae2.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_lightimage textures/skies/nebulae2.jpg
	q3map_surfacelight 70
	q3map_sun .5 .5 .5 90 315 40
	skyparms env/nebulae/nebulae2 2048 -
}

textures/skies2/nebula3nolit
{
	qer_editorimage textures/skies/nebulae.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	skyparms env/nebulae/nebulae2 2048 -
}

textures/skies2/nebula3halflit
{
	qer_editorimage textures/skies/nebulae.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_lightimage textures/skies/nebulae.jpg
	q3map_surfacelight 150
	q3map_sun .5 .5 .5 90 315 40
	skyparms env/nebulae/nebulae2 2048 -
}

textures/skies2/nebula3
{
	qer_editorimage textures/skies/nebulae.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_lightimage textures/skies/nebulae.jpg
	q3map_surfacelight 300
	q3map_sun .5 .5 .5 90 315 40
	skyparms env/nebulae/nebulae2 2048 -
}

textures/skies2/nebulax
{
	qer_editorimage textures/skies/nebulae.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	skyparms env/nebulae/nebulae2 2048 -
}

textures/skies2/x_nebula3
{
	qer_editorimage textures/skies/nebulae.jpg
	q3map_lightsubdivide 512
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_lightimage textures/skies/nebulae2.jpg
	q3map_surfacelight 200
	q3map_sun .5 .5 .5 90 315 40
	skyparms env/nebulae/nebulae2 2048 -
}

textures/skies/xproto_sky_tourney2
{
	qer_editorimage textures/skies/intelredclouds.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_lightimage textures/skies/s9.tga
	q3map_surfacelight 200
	q3map_sun .5 .5 .5 100 180 80
	skyparms - 1024 -
	{
		map textures/skies/intelredclouds.tga
		tcmod scroll .02 .04
		tcmod scale 2 3
	}
	{
		map textures/skies/topclouds.jpg
		blendfunc add
		tcmod scroll -.005 .005
		tcmod scale 5 4
	}
	{
		map textures/skies/anoice1.jpg
		blendfunc filter
		tcmod scroll .005 .005
		tcmod scale 4 4
	}
	
}

textures/skies/xmkc_ctf5sky
{
	qer_editorimage textures/skies/stars.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_sun	0.9 0.8 1.0 85 85 75
	skyparms - 1024 -
	{
		map textures/skies/stars.jpg
		tcmod scale 4 7
	}
	{
		map textures/skies/nitesky.jpg
		blendfunc add
		tcmod scale 2 4
		tcmod scroll 0.2 0.001
	}
}

textures/skies/tim_tourney1
{
	qer_editorimage textures/skies/dimclouds.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm sky
	q3map_sun	.6 .37 .19 100 320 60
	skyparms - 1024 -
	{
		map textures/skies/intelredclouds.jpg
		tcmod scale 6 4
		tcmod scroll 0.04 0.01
	}
	{
		map textures/skies/dimclouds.jpg
		blendfunc add
		tcmod scale 2 3
		tcmod scroll 0.03 0.02
	}
}

