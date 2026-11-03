// King of the Hill indicator: floating crown and border bollards.
// Models and textures: tools/koth/make_koth_models.py. Drawn by cgame
// (cg_hud_vehicle.c, CG_AddKOTHHillIndicatorToScene). Surfaces with
// rgbGen entity take the colour cgame gives them (hill owner / capturing team).

models/mapobjects/koth/crown_gold
{
	qer_editorimage models/mapobjects/koth/crown_gold.tga
	{
		map models/mapobjects/koth/crown_gold.tga
		rgbGen lightingDiffuse
	}
	{
		map models/mapobjects/koth/crown_env.tga
		tcGen environment
		blendFunc GL_ONE GL_ONE
		rgbGen const ( 0.80 0.58 0.22 )
	}
}

models/mapobjects/koth/crown_gem
{
	qer_editorimage models/mapobjects/koth/crown_gem.tga
	{
		map models/mapobjects/koth/crown_gem.tga
		rgbGen entity
	}
	{
		map models/mapobjects/koth/crown_env.tga
		tcGen environment
		blendFunc GL_ONE GL_ONE
		rgbGen const ( 0.55 0.55 0.55 )
	}
}

models/mapobjects/koth/crown_velvet
{
	qer_editorimage models/mapobjects/koth/crown_velvet.tga
	{
		map models/mapobjects/koth/crown_velvet.tga
		rgbGen entity
	}
}

models/mapobjects/koth/post_metal
{
	qer_editorimage models/mapobjects/koth/post_metal.tga
	{
		map models/mapobjects/koth/post_metal.tga
		rgbGen lightingDiffuse
	}
	{
		map models/mapobjects/koth/crown_env.tga
		tcGen environment
		blendFunc GL_ONE GL_ONE
		rgbGen const ( 0.12 0.12 0.13 )
	}
}

models/mapobjects/koth/post_lamp
{
	qer_editorimage models/mapobjects/koth/post_lamp.tga
	{
		map models/mapobjects/koth/post_lamp.tga
		rgbGen entity
	}
}

gfx/misc/koth_glow
{
	nopicmip
	cull none
	sort additive
	{
		clampMap gfx/misc/koth_glow.tga
		blendFunc GL_ONE GL_ONE
		rgbGen vertex
	}
}
