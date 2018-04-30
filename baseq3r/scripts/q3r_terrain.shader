//****************************************************
// *************************************************
// *
// * Q3Rcountry1
// *
// *************************************************
textures/terrain/q3rcountry1_0
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/dirt04.tga
		rgbGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_1
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/dirt01.tga
		rgbGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_2
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/stones01grass.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry1_3
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/rock03.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry1_4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/grass09.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry1_5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/grass02.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry1_6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}


textures/terrain/q3rcountry1_0to1
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/dirt04.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/dirt01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}


textures/terrain/q3rcountry1_0to2
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/dirt04.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/stones01grass.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry1_0to3
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/dirt04.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/rock03.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry1_0to4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/dirt04.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass09.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry1_0to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/dirt04.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass02.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry1_0to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/dirt04.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_1to2
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/dirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/stones01grass.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_1to3
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/dirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/rock03.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_1to4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/dirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass09.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_1to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/dirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass02.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_1to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/dirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_2to3
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/stones01grass.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/rock03.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_2to4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/stones01grass.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass09.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_2to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/stones01grass.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass02.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_2to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/stones01grass.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_3to4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/rock03.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/grass09.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_3to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/rock03.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass02.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_3to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/rock03.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_4to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/grass09.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass02.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_4to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/grass09.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry1_5to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/grass02.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/skies2/clouds.tga
		blendfunc filter
		detail
		tcmod scale 0.01 0.01
		tcMod scroll -0.05 0.05
		tcmod transform 1 0 1 1 1 1
	}
}



//****************************************************
// *************************************************
// *
// * Q3Rcountry2  noclouds
// *
// *************************************************
textures/terrain/q3rcountry2_0
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/darkdirt02.tga
		rgbGen vertex
		tcmod scale 0.125 0.125
	}

}

textures/terrain/q3rcountry2_1
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/darkdirt01.tga
		rgbGen vertex
		tcmod scale 0.125 0.125
	}

}

textures/terrain/q3rcountry2_2
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/stones01grass.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
	}

}
textures/terrain/q3rcountry2_3
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/rock03.tga
		tcmod scale 0.0625 0.0625
		rgbGen vertex
	}

}
textures/terrain/q3rcountry2_4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
	}

}
textures/terrain/q3rcountry2_5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/greengrass.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
	}

}
textures/terrain/q3rcountry2_6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/darkdirtgrass01.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
	}

}


textures/terrain/q3rcountry2_0to1
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/darkdirt02.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/darkdirt01.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}

}


textures/terrain/q3rcountry2_0to2
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/darkdirt02.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/stones01grass.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}


textures/terrain/q3rcountry2_0to3
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/darkdirt02.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/rock03.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.0625 0.0625
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}


textures/terrain/q3rcountry2_0to4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/darkdirt02.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}


textures/terrain/q3rcountry2_0to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/darkdirt02.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/greengrass.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}


textures/terrain/q3rcountry2_0to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/darkdirt02.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/darkdirtgrass01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_1to2
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/darkdirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/stones01grass.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_1to3
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/darkdirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/rock03.tga
		tcmod scale 0.0625 0.0625
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_1to4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/darkdirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_1to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/darkdirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/greengrass.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_1to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/darkdirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/darkdirtgrass01.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_2to3
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/stones01grass.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/rock03.tga
		tcmod scale 0.0625 0.0625
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_2to4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/stones01grass.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_2to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/stones01grass.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/greengrass.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_2to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/stones01grass.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/darkdirtgrass01.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_3to4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/rock03.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.0625 0.0625
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_3to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/rock03.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.0625 0.0625
	}
	{
		map textures/q3r_dirt_grass/greengrass.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_3to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/rock03.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.0625 0.0625
	}
	{
		map textures/q3r_dirt_grass/darkdirtgrass01.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_4to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/checker01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/greengrass.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_4to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/checker01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/darkdirtgrass01.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

textures/terrain/q3rcountry2_5to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/greengrass.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/darkdirtgrass01.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}



//****************************************************
// *************************************************
// *
// * Q3Rcountry3 test pathmap
// *
// *************************************************
textures/terrain/q3rcountry3_0
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/dirt04.tga
		rgbGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_1
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/dirt01.tga
		rgbGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_2
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/stones01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry3_3
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/rockwall01.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry3_4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/grass08.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry3_5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/grass02.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail		
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry3_6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}


textures/terrain/q3rcountry3_0to1
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/dirt04.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/dirt01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}


textures/terrain/q3rcountry3_0to2
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/dirt04.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/stones01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry3_0to3
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/dirt04.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/rockwall01.tga
		rgbGen vertex

		alphaGen vertex
		tcmod scale 0.125 0.125
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry3_0to4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/dirt04.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass08.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry3_0to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/dirt04.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass02.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}
textures/terrain/q3rcountry3_0to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight
	{
		map textures/q3r_dirt_grass/dirt04.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_1to2
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/dirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/stones01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_1to3
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/dirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/rockwall01.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_1to4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/dirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass08.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_1to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/dirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass02.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_1to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/dirt01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_2to3
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/stones01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/rockwall01.tga
		tcmod scale 0.125 0.125
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_2to4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/stones01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass08.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_2to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/stones01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass02.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_2to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/stones01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_3to4
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/rockwall01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/grass08.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_3to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/rockwall01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/grass02.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail		
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_3to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/rockwall01.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.125 0.125
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_4to5
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/grass08.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/grass02.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_4to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/grass08.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}

textures/terrain/q3rcountry3_5to6
{
	surfaceparm nolightmap
	q3map_novertexshadows
	q3map_forcesunlight

	{
		map textures/q3r_dirt_grass/grass02.tga
		rgbGen vertex
		alphaGen vertex
		tcmod scale 0.25 0.25
	}
	{
		map textures/q3r_dirt_grass/checker01.tga
		tcmod scale 0.25 0.25
		rgbGen vertex
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map textures/terra/q3r_countrydetail.tga
		blendfunc filter
		detail
		tcmod scale 0.0039 0.0039
//		tcmod scale 0.015625 0.015625
//		tcmod transform 1 0 1 1 1 1
	}
}


