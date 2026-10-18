// Direction & elevation checked and adjusted - Speaker

textures/skies/ely_sunset
{
	qer_editorimage env/ely_sunset/sunset_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_lightsubdivide 256
	surfaceparm sky
	q3map_surfacelight 50
	q3map_sun 1 0.7 0.7 80 350 10
	q3gl2_sun 1 0.7 0.7 80 350 10 0.5
	skyparms env/ely_sunset/sunset - -
	q3r_haze 0.66 0.48 0.40 5000 0 0 128 0.8	// Q3Rally: haze of the maps with this sky
	q3r_colorLut gfx/luts/dusk	// Q3Rally: colour grading look of the maps with this sky
}
