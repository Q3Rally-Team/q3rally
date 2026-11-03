// Railway models for the maps (misc_model): models/mapobjects/q3r_rail/.
// Written for tools/mapobjects/make_rail.py. Small parts are vertex lit by
// q3map2; the large panels (wagon bodies, roof, tank, deck) are turned into
// lightmapped surfaces with q3map_forceMeta so they get proper shadows.

models/mapobjects/q3r_rail/rail
{
	qer_editorimage models/mapobjects/q3r_rail/rail.tga
	surfaceparm nomarks
	{
		map models/mapobjects/q3r_rail/rail.tga
		rgbGen vertex
	}
}

models/mapobjects/q3r_rail/sleeper
{
	qer_editorimage models/mapobjects/q3r_rail/sleeper.tga
	surfaceparm nomarks
	{
		map models/mapobjects/q3r_rail/sleeper.tga
		rgbGen vertex
	}
}

models/mapobjects/q3r_rail/steel
{
	qer_editorimage models/mapobjects/q3r_rail/steel.tga
	surfaceparm nomarks
	{
		map models/mapobjects/q3r_rail/steel.tga
		rgbGen vertex
	}
}

models/mapobjects/q3r_rail/wheel
{
	qer_editorimage models/mapobjects/q3r_rail/wheel.tga
	surfaceparm nomarks
	{
		map models/mapobjects/q3r_rail/wheel.tga
		rgbGen vertex
	}
}

models/mapobjects/q3r_rail/handrail
{
	qer_editorimage models/mapobjects/q3r_rail/handrail.tga
	surfaceparm nomarks
	{
		map models/mapobjects/q3r_rail/handrail.tga
		rgbGen vertex
	}
}

models/mapobjects/q3r_rail/body_brown
{
	qer_editorimage models/mapobjects/q3r_rail/body_brown.tga
	q3map_forceMeta
	q3map_nonplanar
	q3map_shadeAngle 60
	{
		map $lightmap
		rgbGen identity
	}
	{
		map models/mapobjects/q3r_rail/body_brown.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

models/mapobjects/q3r_rail/body_grey
{
	qer_editorimage models/mapobjects/q3r_rail/body_grey.tga
	q3map_forceMeta
	q3map_nonplanar
	q3map_shadeAngle 60
	{
		map $lightmap
		rgbGen identity
	}
	{
		map models/mapobjects/q3r_rail/body_grey.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

models/mapobjects/q3r_rail/body_red
{
	qer_editorimage models/mapobjects/q3r_rail/body_red.tga
	q3map_forceMeta
	q3map_nonplanar
	q3map_shadeAngle 60
	{
		map $lightmap
		rgbGen identity
	}
	{
		map models/mapobjects/q3r_rail/body_red.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

models/mapobjects/q3r_rail/roof
{
	qer_editorimage models/mapobjects/q3r_rail/roof.tga
	q3map_forceMeta
	q3map_nonplanar
	q3map_shadeAngle 60
	{
		map $lightmap
		rgbGen identity
	}
	{
		map models/mapobjects/q3r_rail/roof.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

models/mapobjects/q3r_rail/tank
{
	qer_editorimage models/mapobjects/q3r_rail/tank.tga
	q3map_forceMeta
	q3map_nonplanar
	q3map_shadeAngle 60
	{
		map $lightmap
		rgbGen identity
	}
	{
		map models/mapobjects/q3r_rail/tank.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

models/mapobjects/q3r_rail/deck
{
	qer_editorimage models/mapobjects/q3r_rail/deck.tga
	q3map_forceMeta
	q3map_nonplanar
	q3map_shadeAngle 60
	{
		map $lightmap
		rgbGen identity
	}
	{
		map models/mapobjects/q3r_rail/deck.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}
