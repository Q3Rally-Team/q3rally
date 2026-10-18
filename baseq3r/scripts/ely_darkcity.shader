// Direction & elevation checked and adjusted - Speaker

textures/skies/ely_darkcity
{
	qer_editorimage env/ely_darkcity/darkcity_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_lightsubdivide 256
	surfaceparm sky
	q3map_surfacelight 100
	q3map_sun 1 0.5 1 100 230 45
	skyparms env/ely_darkcity/darkcity - -
	q3r_haze 0.14 0.09 0.22 3500	// Q3Rally: haze of the maps with this sky
	q3r_colorLut gfx/luts/night	// Q3Rally: colour grading look of the maps with this sky
}
