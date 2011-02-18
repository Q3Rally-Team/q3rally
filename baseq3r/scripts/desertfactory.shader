//------------------------------------------------------------------------------
// Shaders for the Desert Factory map project by sev
// All editor images are placed in the textures/desertfactory_shaders/ folder
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Sky
//------------------------------------------------------------------------------

textures/desertfactory_shaders/sky
{
        qer_editorimage textures/desertfactory_shaders/sky.jpg

        surfaceparm sky
        surfaceparm noimpact
        surfaceparm nolightmap
        surfaceparm nodlight
        surfaceparm nomarks

        nopicmip
        nomipmaps

        skyparms env/desert/desert - - //farbox cloudheight nearbox
        q3map_sunExt 1 0.95 0.9 400 315 50 0 16 //rgb intensity degrees elevation deviance samples
}

//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------

textures/desertfactory_shaders/metal01-metal02
{
	qer_editorimage textures/desertfactory_shaders/metal01-metal02.jpg

	q3map_alphaMod dotproduct2 ( 0 0 1.25 )
	{
		map textures/desertfactory_metal/metal01.tga
	}
	{
		map textures/desertfactory_metal/metal02.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA		
		alphaGen vertex
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/desertfactory_shaders/grates01
{
	qer_editorimage textures/desertfactory_shaders/grates01.jpg
	qer_trans 0.5

	surfaceparm trans
	surfaceparm alphashadow

	{
		map textures/desertfactory_metal/grates01.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/glass01
{
	qer_editorimage textures/desertfactory_shaders/glass01.jpg

	surfaceparm trans

	{
		map textures/desertfactory_misc/glass01.tga
		blendFunc blend
	}
}

textures/desertfactory_shaders/light02
{
	qer_editorimage textures/desertfactory_shaders/light02.jpg

	surfaceparm trans
	cull none

	{
		map textures/desertfactory_misc/light02.tga
		blendFunc blend
	}
}

textures/desertfactory_shaders/grass01
{
	qer_editorimage textures/desertfactory_shaders/grass01.jpg

	surfaceparm nomarks
	surfaceparm trans
	surfaceparm alphashadow

	cull none

	{
		map textures/desertfactory_misc/grass01.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/grass02
{
	qer_editorimage textures/desertfactory_shaders/grass02.jpg

	surfaceparm nomarks
	surfaceparm trans
	surfaceparm alphashadow

	cull none

	{
		map textures/desertfactory_misc/grass02.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/vines01
{
	qer_editorimage textures/desertfactory_shaders/vines01.jpg

	surfaceparm nomarks
	surfaceparm trans
	surfaceparm alphashadow

	cull none

	{
		map textures/desertfactory_misc/vines01.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

//------------------------------------------------------------------------------
// Signs
//------------------------------------------------------------------------------

textures/desertfactory_shaders/sign_above_clean
{
	qer_editorimage textures/desertfactory_shaders/sign_above_clean.jpg

	{
		map textures/desertfactory_signs/above_clean.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_explosion_clean
{
	qer_editorimage textures/desertfactory_shaders/sign_explosion_clean.jpg

	{
		map textures/desertfactory_signs/explosion_clean.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_ladder_clean
{
	qer_editorimage textures/desertfactory_shaders/sign_ladder_clean.tga

	{
		map textures/desertfactory_signs/ladder_clean.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_radioactive_clean
{
	qer_editorimage textures/desertfactory_shaders/sign_radioactive_clean.tga

	{
		map textures/desertfactory_signs/radioactive_clean.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_shock_clean
{
	qer_editorimage textures/desertfactory_shaders/sign_shock_clean.tga

	{
		map textures/desertfactory_signs/shock_clean.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_toxic_clean
{
	qer_editorimage textures/desertfactory_shaders/sign_toxic_clean.tga

	{
		map textures/desertfactory_signs/toxic_clean.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_warning_clean
{
	qer_editorimage textures/desertfactory_shaders/sign_warning_clean.tga

	{
		map textures/desertfactory_signs/warning_clean.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_wires_clean
{
	qer_editorimage textures/desertfactory_shaders/sign_wires_clean.tga

	{
		map textures/desertfactory_signs/wires_clean.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_above_dirty
{
	qer_editorimage textures/desertfactory_shaders/sign_above_dirty.tga

	{
		map textures/desertfactory_signs/above_dirty.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_explosion_dirty
{
	qer_editorimage textures/desertfactory_shaders/sign_explosion_dirty.tga

	{
		map textures/desertfactory_signs/explosion_dirty.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_ladder_dirty
{
	qer_editorimage textures/desertfactory_shaders/sign_ladder_dirty.tga

	{
		map textures/desertfactory_signs/ladder_dirty.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_radioactive_dirty
{
	qer_editorimage textures/desertfactory_shaders/sign_radioactive_dirty.tga

	{
		map textures/desertfactory_signs/radioactive_dirty.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_shock_dirty
{
	qer_editorimage textures/desertfactory_shaders/sign_shock_dirty.tga

	{
		map textures/desertfactory_signs/shock_dirty.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_toxic_dirty
{
	qer_editorimage textures/desertfactory_shaders/sign_toxic_dirty.tga

	{
		map textures/desertfactory_signs/toxic_dirty.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_warning_dirty
{
	qer_editorimage textures/desertfactory_shaders/sign_warning_dirty.tga

	{
		map textures/desertfactory_signs/warning_dirty.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

textures/desertfactory_shaders/sign_wires_dirty
{
	qer_editorimage textures/desertfactory_shaders/sign_wires_dirty.tga

	{
		map textures/desertfactory_signs/wires_dirty.tga
		alphaFunc GT0
		rgbGen vertex
	}
}

//------------------------------------------------------------------------------
// Writings
//------------------------------------------------------------------------------

textures/desertfactory_shaders/writing01
{
	qer_editorimage textures/desertfactory_shaders/writing01.jpg

	surfaceparm trans
	nopicmip
	nomipmaps

	{
		map textures/desertfactory_documents/writing01.tga
		blendFunc blend
	}
}

textures/desertfactory_shaders/writing02
{
	qer_editorimage textures/desertfactory_shaders/writing02.jpg

	surfaceparm trans
	nopicmip
	nomipmaps

	{
		map textures/desertfactory_documents/writing02.tga
		blendFunc blend
	}
}

textures/desertfactory_shaders/writing03
{
	qer_editorimage textures/desertfactory_shaders/writing03.jpg

	surfaceparm trans
	nopicmip
	nomipmaps

	{
		map textures/desertfactory_documents/writing03.tga
		blendFunc blend
	}
}

textures/desertfactory_shaders/writing04
{
	qer_editorimage textures/desertfactory_shaders/writing04.jpg

	surfaceparm trans
	nopicmip
	nomipmaps

	{
		map textures/desertfactory_documents/writing04.tga
		blendFunc blend
	}
}

//------------------------------------------------------------------------------
// End of the file. Greetings from this rarely visited place X)
//------------------------------------------------------------------------------
