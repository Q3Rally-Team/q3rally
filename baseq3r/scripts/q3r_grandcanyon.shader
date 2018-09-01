textures/q3r_grandcanyon/evergreen
{
	cull none
	deformVertexes	autosprite2
	surfaceparm trans
	surfaceparm alphashadow
	surfaceparm solid
	qer_editorimage textures/bergburg/evergreen

	{
		map textures/bergburg/evergreen.tga
		alphaFunc GE128
		//blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
		depthfunc equal
	}

}

textures/q3r_grandcanyon/concrete
{
	Surfaceparm nolightmap //signifies vertex lighting only.
//	Q3map_novertexshadows //this is what keeps those caulk vis blockers from causing ugly shadows to form on your terrain.
	q3map_forcesunlight
	surfaceparm SURF_ASPHALT

	qer_editorimage textures/q3r_grandcanyon/concrete
	
	{
		tcmod scale 0.5 0.5
		map textures/q3r_grandcanyon/concrete.jpg
	}

	{
		map $lightmap
		rgbGen identity
		blendFunc filter
		depthfunc equal
	}
}

textures/q3r_grandcanyon/pjrock1
{
	surfaceparm SURF_GRAVEL
	Surfaceparm nolightmap //signifies vertex lighting only.
//	Q3map_novertexshadows //this is what keeps those caulk vis blockers from causing ugly shadows to form on your terrain.
	q3map_forcesunlight

	qer_editorimage textures/stone/pjrock1
	
	{
		rgbgen vertex
		tcmod scale 0.5 0.5
		map textures/stone/pjrock1.jpg
	}
}


textures/q3r_grandcanyon/pjrock16
{
	surfaceparm SURF_DIRT
	Surfaceparm nolightmap //signifies vertex lighting only.
//	Q3map_novertexshadows //this is what keeps those caulk vis blockers from causing ugly shadows to form on your terrain.
	q3map_forcesunlight

	qer_editorimage textures/q3r_grandcanyon/pjrock16-1b
	
	{

		rgbgen vertex
		tcmod scale 0.5 0.5
		map textures/q3r_grandcanyon/pjrock16-1b.jpg
	}

}

textures/q3r_grandcanyon/pjrock21
{
	Surfaceparm nolightmap //signifies vertex lighting only.
//	Q3map_novertexshadows //this is what keeps those caulk vis blockers from causing ugly shadows to form on your terrain.
	q3map_forcesunlight
	surfaceparm SURF_DIRT

	qer_editorimage textures/q3r_grandcanyon/pjrock21b
	
	{

		rgbgen vertex
		tcmod scale 0.5 0.5
		map textures/q3r_grandcanyon/pjrock21b.jpg
	}

}


textures/q3r_grandcanyon/pjrock16-1
{
	Surfaceparm nolightmap //signifies vertex lighting only.
//	Q3map_novertexshadows //this is what keeps those caulk vis blockers from causing ugly shadows to form on your terrain.
	q3map_forcesunlight
	surfaceparm SURF_DIRT
	{

		rgbgen vertex
		tcmod scale 0.5 0.5
		map textures/q3r_grandcanyon/pjrock16-1.jpg
	}
}


textures/q3r_grandcanyon/pjrock16-21
{
	Surfaceparm nolightmap //signifies vertex lighting only.
//	Q3map_novertexshadows //this is what keeps those caulk vis blockers from causing ugly shadows to form on your terrain.
	q3map_forcesunlight
	surfaceparm SURF_DIRT
	{

		rgbgen vertex
		tcmod scale 0.5 0.5
		map textures/q3r_grandcanyon/pjrock16-21.jpg
	}
}


textures/q3r_grandcanyon/pjrock21-1
{
	Surfaceparm nolightmap //signifies vertex lighting only.
//	Q3map_novertexshadows //this is what keeps those caulk vis blockers from causing ugly shadows to form on your terrain.
	q3map_forcesunlight
	surfaceparm SURF_DIRT
	{

		rgbgen vertex
		tcmod scale 0.5 0.5
		map textures/q3r_grandcanyon/pjrock21-1.jpg
	}

}

textures/q3r_grandcanyon/fog
{
	qer_editorimage textures/sfx/fog_grey.tga
	surfaceparm	trans
	surfaceparm	nonsolid
	surfaceparm	fog
	surfaceparm	nolightmap
	surfaceparm nodrop
//	q3map_surfacelight 100
	fogparms ( .85 .8 .8 ) 500
}

textures/q3r_grandcanyon/water
{
	qer_editorimage textures/q3r_grandcanyon/water
	qer_trans .6
	q3map_globaltexture
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm SURF_WET

	cull disable

//	deformVertexes wave 200 triangle 0 4 0.0 2.0	
	//deformVertexes wave 100 sawtooth 3 3 0.5 0.5

	{
		map textures/q3r_grandcanyon/water.jpg
		blendfunc gl_one gl_zero
		tcmod turb 0 0.1 0 0.2
	}	
	{
		map textures/liquids/pool2.tga
		blendfunc gl_one gl_one
		tcMod turb 0 .2 0 .1
		tcMod scroll 0.1 0.3
	}	

	{
		map $lightmap
		blendFunc GL_dst_color GL_zero
		rgbgen identity		
	}
}

textures/q3r_grandcanyon/sky
{
qer_editorimage env/q3r_grandcanyon/desert_ft

surfaceparm noimpact
surfaceparm nolightmap
surfaceparm sky
q3map_sun .5 .37 .19 85 317 33
q3map_surfacelight 120
//q3map_backsplash -1


skyparms env/q3r_grandcanyon/desert - -

}
