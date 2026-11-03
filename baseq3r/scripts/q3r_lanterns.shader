// Lantern models for the maps (misc_model): models/mapobjects/q3r_lanterns/.
// Written for tools/mapobjects/make_lanterns.py. The models are baked into the
// BSP and vertex lit by q3map2; glass, flame and lens glow by themselves (the
// light itself comes from a light entity next to the model). floodmast*.md3
// uses galv, housing and one of the floodlens shaders.

models/mapobjects/q3r_lanterns/iron
{
	qer_editorimage models/mapobjects/q3r_lanterns/iron.tga
	surfaceparm nomarks
	{
		map models/mapobjects/q3r_lanterns/iron.tga
		rgbGen vertex
	}
}

models/mapobjects/q3r_lanterns/pole
{
	qer_editorimage models/mapobjects/q3r_lanterns/pole.tga
	surfaceparm nomarks
	{
		map models/mapobjects/q3r_lanterns/pole.tga
		rgbGen vertex
	}
}

models/mapobjects/q3r_lanterns/glass
{
	qer_editorimage models/mapobjects/q3r_lanterns/glass.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm nonsolid
	cull none
	{
		map models/mapobjects/q3r_lanterns/glass.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave noise 0.88 0.12 0 5
	}
}

models/mapobjects/q3r_lanterns/flame
{
	qer_editorimage models/mapobjects/q3r_lanterns/flame.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm nonsolid
	{
		map models/mapobjects/q3r_lanterns/flame.tga
		rgbGen wave noise 0.9 0.1 0 7
	}
}

models/mapobjects/q3r_lanterns/lens
{
	qer_editorimage models/mapobjects/q3r_lanterns/lens.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	{
		map models/mapobjects/q3r_lanterns/lens.tga
		rgbGen identity
	}
}

models/mapobjects/q3r_lanterns/galv
{
	qer_editorimage models/mapobjects/q3r_lanterns/galv.tga
	surfaceparm nomarks
	{
		map models/mapobjects/q3r_lanterns/galv.tga
		rgbGen vertex
	}
}

models/mapobjects/q3r_lanterns/housing
{
	qer_editorimage models/mapobjects/q3r_lanterns/housing.tga
	surfaceparm nomarks
	{
		map models/mapobjects/q3r_lanterns/housing.tga
		rgbGen vertex
	}
}

models/mapobjects/q3r_lanterns/floodlens
{
	qer_editorimage models/mapobjects/q3r_lanterns/floodlens.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	{
		map models/mapobjects/q3r_lanterns/floodlens.tga
		rgbGen identity
	}
}

models/mapobjects/q3r_lanterns/floodlens_red
{
	qer_editorimage models/mapobjects/q3r_lanterns/floodlens_red.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	{
		map models/mapobjects/q3r_lanterns/floodlens_red.tga
		rgbGen identity
	}
}

models/mapobjects/q3r_lanterns/floodlens_blue
{
	qer_editorimage models/mapobjects/q3r_lanterns/floodlens_blue.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	{
		map models/mapobjects/q3r_lanterns/floodlens_blue.tga
		rgbGen identity
	}
}
