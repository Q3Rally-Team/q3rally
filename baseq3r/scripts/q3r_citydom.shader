// ------------------------------------------------------------
// q3r_citydom: skin of the glass towers (rework 2026)
// ------------------------------------------------------------

// reflective curtain wall glass, tinted
textures/q3r_citydom/glass
{
	qer_editorimage textures/q3r_citydom/glass_ed.jpg
	qer_trans 0.5
	surfaceparm trans
	surfaceparm nolightmap
	surfaceparm nomarks
	cull twosided
	{
		map $whiteimage
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen const ( 0.70 0.80 0.86 )
	}
	{
		map textures/q3r_citydom/glass_env.jpg
		tcGen environment
		blendFunc GL_ONE GL_ONE
		rgbGen const ( 0.24 0.26 0.28 )
	}
}

// darker glass of the cores
textures/q3r_citydom/glassdark
{
	qer_editorimage textures/q3r_citydom/glassdark_ed.jpg
	qer_trans 0.5
	surfaceparm trans
	surfaceparm nolightmap
	surfaceparm nomarks
	cull twosided
	{
		map $whiteimage
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen const ( 0.30 0.38 0.42 )
	}
	{
		map textures/q3r_citydom/glass_env.jpg
		tcGen environment
		blendFunc GL_ONE GL_ONE
		rgbGen const ( 0.18 0.19 0.20 )
	}
}

// ceiling panels with downlights under the slabs and the cantilevers
textures/q3r_citydom/soffit
{
	qer_editorimage textures/q3r_citydom/soffit.jpg
	q3map_surfacelight 60
	q3map_lightImage textures/q3r_citydom/soffit_glow.jpg
	q3map_lightSubdivide 128
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/q3r_citydom/soffit.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/q3r_citydom/soffit_glow.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen identity
	}
}

// car park: lit decks behind the openings, P sign, lamp heads
textures/q3r_citydom/parkdeck
{
	qer_editorimage textures/q3r_citydom/parkdeck.jpg
	surfaceparm nolightmap
	surfaceparm nomarks
	{
		map textures/q3r_citydom/parkdeck.jpg
		rgbGen const ( 0.85 0.85 0.85 )
	}
}

textures/q3r_citydom/psign
{
	qer_editorimage textures/q3r_citydom/psign.jpg
	surfaceparm nolightmap
	q3map_surfacelight 200
	{
		map textures/q3r_citydom/psign.jpg
		rgbGen identity
	}
}

textures/q3r_citydom/lamp
{
	qer_editorimage textures/q3r_citydom/lamp.jpg
	surfaceparm nolightmap
	q3map_surfacelight 2500
	{
		map textures/q3r_citydom/lamp.jpg
		rgbGen identity
	}
}
