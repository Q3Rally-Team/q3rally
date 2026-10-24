// ------------------------------------------------------------
// Shaders for Q3Rally Skies - rewritten by P3rlE
// based on the skies and q3r_skies.shader
// ------------------------------------------------------------

// ------------------------------------------------------------
// 1.skies (hellsky)
// ------------------------------------------------------------

textures/skies/hellsky
{
	qer_editorimage textures/skies/dimclouds.tga
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nolightmap

	q3map_sun 4 3 3 65 290 75
	q3gl2_sun 4 3 3 65 290 75 0.6
	q3map_surfacelight 50
	skyparms - 512 -
	q3r_haze 0.42 0.38 0.34 3500 2500 0 150	// Q3Rally: haze of the maps with this sky
	q3r_colorLut gfx/luts/warm	// Q3Rally: colour grading look of the maps with this sky

	{
		map textures/skies/dimclouds.tga
		tcMod scroll 0.05 0
		tcMod scale 2 2
		depthWrite
	}
}

// ------------------------------------------------------------
// 2.skies (nightsky_xian_dm1)
// ------------------------------------------------------------

textures/skies/nightsky_xian_dm1
{
	surfaceparm noimpact
	surfaceparm nolightmap

	q3map_surfacelight 300
	q3map_sun 1 1 1 100 -41 58

	qer_editorimage textures/skies/xnight2_up.tga

	skyparms env/xnight2 - -
}

// ------------------------------------------------------------
// 3.skies (nightsky_xian_dm15)
// ------------------------------------------------------------

textures/skies/nightsky_xian_dm15
{
	qer_editorimage textures/skies/xnight2_up.tga
	surfaceparm nolightmap
	surfaceparm noimpact

	q3map_surfacelight 900
	q3map_sun 1 1 1 100 -58 58
	skyparms env/xnight2 - -

}

// ------------------------------------------------------------
// 5.skies (trainyard)
// ------------------------------------------------------------

textures/q3r_skies/env/trainyard/trainyard
{
	qer_editorimage textures/q3r_skies/env/trainyard/trainyard_lf.tga
	surfaceparm sky
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	q3map_surfacelight 500
	q3map_lightsubdivide 512
	// sun tilted for real-time shadows (was 1 1 1 32 90 90, straight down); maps relit
	q3map_sun 1 1 1 32 240 50
	q3gl2_sun 1 1 1 32 240 50 0.5
	skyparms textures/q3r_skies/env/trainyard/trainyard - -
	q3r_haze 0.70 0.76 0.84 6000	// Q3Rally: haze of the maps with this sky
}

// ------------------------------------------------------------
// 6.skies (clouds)
// ------------------------------------------------------------

textures/q3r_skies/clouds
{
	qer_editorimage textures/q3r_skies/clouds.jpg
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nolightmap

	q3map_sun 4 3 3 65 290 75
	q3map_surfacelight 50
	skyparms - 512 -

	{
		map textures/q3r_skies/clouds.jpg
		tcMod scroll 0.05 0
		tcMod scale 2 2
		depthWrite
	}
}

// ------------------------------------------------------------
// 7.skies (pjbasesky_arena1_sky)
// ------------------------------------------------------------

textures/skies/pjbasesky_arena1_sky
{
	qer_editorimage textures/skies/hellsky.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_lightsubdivide 256 
	q3map_sun	0.266383 0.274632 0.358662 150 60 85
	q3map_surfacelight 100

	skyparms - 512 -
	
	{
		map textures/skies/dimclouds.tga
		tcMod scroll 0.01 0.01
		tcMod scale 3 3
		depthWrite
	}
	{
		map textures/skies/hellsky.tga
		blendfunc GL_ONE GL_ONE
		tcMod scroll -0.01 -0.01
		tcMod scale 5 5
	}
}

// ------------------------------------------------------------
// 8.skies (mp_us)
// ------------------------------------------------------------

textures/q3r_skies/env/mp_us/mp_us
{
	qer_editorimage textures/q3r_skies/env/mp_us/urbansp_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_lightsubdivide 256
	q3map_surfacelight 100
	surfaceparm sky
	// sun tilted for real-time shadows (was 1 1 1 100 0 90, straight down); map relit
	q3map_sun 1 1 1 100 300 50
	q3gl2_sun 1 1 1 100 300 50 0.5
	skyparms textures/q3r_skies/env/mp_us/urbansp - -
	q3r_haze 0.22 0.09 0.05 4000	// Q3Rally: haze of the maps with this sky
	q3r_colorLut gfx/luts/dusk	// Q3Rally: colour grading look of the maps with this sky
}

// ------------------------------------------------------------
// 10.skies (night01)
// ------------------------------------------------------------

textures/stecki/night01
{
	qer_editorimage textures/stecki/night01_up.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
//        q3map_lightimage textures/stecki/night01_up.tga
	q3map_sun	.5 .8 1 30 30 85
	q3map_surfacelight 5

        skyparms textures/stecki/night01 512 textures/stecki/night01
	q3r_haze 0.10 0.09 0.08 4000	// Q3Rally: haze of the maps with this sky
        q3r_colorLut gfx/luts/night	// Q3Rally: colour grading look of the maps with this sky

}

// ------------------------------------------------------------
// 12.skies (sub01)
// ------------------------------------------------------------

textures/stecki/sub01
{
	qer_editorimage textures/stecki/sub01_rt.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
//        q3map_lightimage textures/stecki/sub01_up.tga
	q3map_sun	1 1 1 120 180 60
	q3gl2_sun 1 1 1 120 180 60 0.5
	q3map_surfacelight 320

        skyparms textures/stecki/sub01 512 textures/stecki/sub01
	q3r_haze 0.62 0.68 0.76 6000 3000 -150 160 0.5	// Q3Rally: haze of the maps with this sky

}

// ------------------------------------------------------------
// 19.skies (ame_nebula)
// ------------------------------------------------------------

textures/q3r_skies/env/ame_nebula
{
	qer_editorimage textures/q3r_skies/env/ame_nebula/purplenebula_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_sun 1.00 0.90 1.00 70 90 90
	q3map_surfacelight 70
	skyparms textures/q3r_skies/env/ame_nebula/purplenebula - -
	q3r_colorLut gfx/luts/night	// Q3Rally: colour grading look of the maps with this sky
}

textures/skies/ame_nebula
{
	qer_editorimage textures/q3r_skies/env/ame_nebula/purplenebula_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	// sun tilted for real-time shadows (was 1.00 0.90 1.00 70 90 90, straight down); maps relit
	q3map_sun 1.00 0.90 1.00 70 68 55
	q3gl2_sun 1.00 0.90 1.00 70 68 55 0.5
	q3map_surfacelight 70
	skyparms textures/q3r_skies/env/ame_nebula/purplenebula - -
	q3r_colorLut gfx/luts/night	// Q3Rally: colour grading look of the maps with this sky
}

// ------------------------------------------------------------
// 20.skies (tim_dm3_red)
// ------------------------------------------------------------

textures/skies/tim_dm3_red
{
	qer_editorimage textures/skies/stars_red.tga
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm sky

	q3map_sun .5 .37 .19 70 30 70
	q3gl2_sun .5 .37 .19 70 30 70 0.6

	//q3map_sun 1 .37 .19 70 30 70
	q3map_surfacelight 80
	skyparms - 512 -
	q3r_haze 0.40 0.12 0.06 3500	// Q3Rally: haze of the maps with this sky
	q3r_colorLut gfx/luts/warm	// Q3Rally: colour grading look of the maps with this sky

	//cloudparms 512 full
	//lightning

	{
		map textures/skies/killsky_1.tga
		tcMod scroll 0.05 .1
		tcMod scale 2 2
		depthWrite
	}
	{
		map textures/skies/killsky_2.tga
		blendfunc GL_ONE GL_ONE
		tcMod scroll 0.05 0.06
		tcMod scale 3 2
	}
}

textures/skies/night_dark
{
	qer_editorimage textures/skies/xnight2_up.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_surfacelight 35
	q3map_sun 0.545098 0.690196 0.705882 15 319 58
	skyParms env/xnight2 128 -
	q3r_haze 0.04 0.05 0.07 3000	// Q3Rally: haze of the maps with this sky
	q3r_colorLut gfx/luts/night	// Q3Rally: colour grading look of the maps with this sky
}

textures/vsky/vsky
{
	qer_editorimage textures/vsky/vsky_up.jpg
	surfaceparm sky
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_lightsubdivide 128
	q3map_surfacelight 100
	q3map_globaltexture
	
	q3map_sun 0.980392 0.67451 0.372549 100 334 57
	q3gl2_sun 0.980392 0.67451 0.372549 100 334 57 0.5
	skyparms textures/vsky/vsky - -
	q3r_haze 0.62 0.42 0.32 4500 0 0 128 0.6	// Q3Rally: haze of the maps with this sky
	q3r_colorLut gfx/luts/warm	// Q3Rally: colour grading look of the maps with this sky
}
