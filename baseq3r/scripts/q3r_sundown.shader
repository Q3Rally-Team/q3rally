// Warm architectural lighting for the Sundown Autoball stadium.
textures/q3r_sundown/amber_led
{
	qer_editorimage textures/q3r_lights/proto_light.jpg
	q3map_nolightmap
	q3map_surfacelight 120
	{
		map $whiteimage
		rgbGen const ( 1.0 0.42 0.18 )
	}
}

// ------------------------------------------------------------
// pitch, goals, ad boards and crowd (stadium dressing 2026)
// ------------------------------------------------------------

// the pitch: one picture over the whole field (x -2560..2560, y -1600..1600):
// tiled turf, mowing stripes and the markings; driving physics as before
textures/q3r_sundown/field
{
	qer_editorimage textures/q3r_sundown/field_lines.tga
	{
		map textures/q3r_sundown/turf.jpg
		tcMod scale 10 6.25
		rgbGen identity
	}
	{
		clampmap textures/q3r_sundown/field_shade.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		clampmap textures/q3r_sundown/field_lines.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// goal posts in team colour, glowing
textures/q3r_sundown/goalpost_red
{
	qer_editorimage textures/q3r_sundown/goalpost_red.jpg
	surfaceparm nolightmap
	q3map_surfacelight 60
	{
		map textures/q3r_sundown/goalpost_red.jpg
		rgbGen identity
	}
}

textures/q3r_sundown/goalpost_blue
{
	qer_editorimage textures/q3r_sundown/goalpost_blue.jpg
	surfaceparm nolightmap
	q3map_surfacelight 60
	{
		map textures/q3r_sundown/goalpost_blue.jpg
		rgbGen identity
	}
}

// goal nets painted on the walls of the goal boxes, lit in team colour
textures/q3r_sundown/net_red
{
	qer_editorimage textures/q3r_sundown/net_red.jpg
	{
		map textures/q3r_sundown/net_red.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/q3r_sundown/net_red.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen const ( 0.35 0.35 0.35 )
	}
}

textures/q3r_sundown/net_blue
{
	qer_editorimage textures/q3r_sundown/net_blue.jpg
	{
		map textures/q3r_sundown/net_blue.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/q3r_sundown/net_blue.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen const ( 0.35 0.35 0.35 )
	}
}

// LED ad board above the ramps: self lit, scrolling, not solid
textures/q3r_sundown/adboard
{
	qer_editorimage textures/q3r_sundown/adboard.jpg
	surfaceparm nonsolid
	surfaceparm nolightmap
	surfaceparm nomarks
	q3map_surfacelight 40
	{
		map textures/q3r_sundown/adboard.jpg
		tcMod scroll 0.025 0
		rgbGen identity
	}
}

// crowd on the upper walls and in the boxes (not solid)
textures/q3r_sundown/crowd_a
{
	qer_editorimage textures/q3r_people/crowd01.jpg
	surfaceparm nonsolid
	surfaceparm nomarks
	{
		map textures/q3r_people/crowd01.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_sundown/crowd_b
{
	qer_editorimage textures/q3r_people/crowd01a.jpg
	surfaceparm nonsolid
	surfaceparm nomarks
	{
		map textures/q3r_people/crowd01a.jpg
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_sundown/crowd_c
{
	qer_editorimage textures/q3r_people/crowd02.tga
	surfaceparm nonsolid
	surfaceparm nomarks
	{
		map textures/q3r_people/crowd02.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/q3r_sundown/fans
{
	qer_editorimage textures/q3r_people/peeprow02.tga
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	cull none
	{
		map textures/q3r_people/peeprow02.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
		depthFunc equal
	}
}
