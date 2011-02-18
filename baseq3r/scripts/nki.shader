//modification of /skies/IndustrySky for islandctf
textures/nki_shaders/tropicalsky
{
	qer_editorimage env/sky1/sky001_ft.jpg
	nopicmip
	noMipMaps
//	q3map_lightmapFilterRadius self other
	q3map_lightmapFilterRadius 0 8 //to blur q3map_skylight when it has less than 4 iterations, to avoid the "stadium effect"
//	q3map_skylight amount iterations
	q3map_skylight 70 4
	q3map_lightRGB 0.4 0.75 1
//	q3map_lightmapBrightness 2.0 
//	q3map_sunExt red green blue intensity degrees elevation deviance samples
	q3map_sunExt 1 0.84 0.51 140 200 40 4 4
//	q3map_lightsubdivide 256
	q3map_globaltexture
	skyParms env/sky1/sky001 512 -
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
}

// modification of /liquids/justwater for island ctf
textures/nki_shaders/tropicalwater
{
	qer_editorimage textures/liquids/pool3d_4b2.tga
	qer_trans 20
	surfaceparm lightfilter
	surfaceparm nomarks
	surfaceparm trans
//	surfaceparm fog
	surfaceparm water
//	surfaceparm alphashadow
	surfaceparm nolightmap
	//deformVertexes wave <div> <func> <base> <amplitude> <phase> <freq>
	deformVertexes wave 256 sin 0 16 0 0.1 
	tessSize 256
	q3map_globaltexture
	q3map_flare textures/flares/twilightflare
	fogparms ( 0.215686 0.278431 0.333333 ) 1024
	cull disable
	{
		map textures/liquids/pool3d_4b2.tga
		blendfunc blend
		rgbGen const ( 0.384314 0.501961 0.568627 )
		tcMod scroll 0.05 -0.05
		alphaGen wave inversesawtooth 0.5 0 0 1 
	}
	{
		map textures/liquids/pool3d_3.jpg
		blendfunc gl_dst_color gl_one
		rgbGen const ( 1 1 1 )
		tcMod scroll 0.09 0.06
	}
	{
		map textures/liquids/pool3d_5.tga
		blendfunc gl_dst_color gl_one
		rgbGen const ( 1 1 1 )
		tcMod scroll -0.04 -0.08
	}
}

textures/nki_shaders/lavared_1k5
{
	q3map_lightimage textures/nki/nki_lava4.tga  // q3map_lightimage must appear before any qer_editorimage !!
	qer_editorimage textures/nki/nki_lava4.tga
	q3map_lightmapBrightness 2.0
	//q3map_lightmapFilterRadius self other
	surfaceparm lava
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm noimpact
	surfaceparm trans
	q3map_surfacelight 1500
	cull none
	deformvertexes wave 50 sin 2 3 0 0.2 
	q3map_flare textures/flares/lava
	{
		map textures/nki/nki_lava4.tga
//		tcmod scroll -.05 .001
		tcMod turb 0 -0.2 0 0.02
	}
	{
		map textures/nki/nki_lava5l3.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		tcmod scroll -.05 .001
		tcMod turb 0.1 -0.1 0 0.01
	}
	
}

textures/nki_shaders/lavabright_1k5
{

	q3map_lightimage textures/nki/nki_lava5.tga  // q3map_lightimage must appear before qer_editorimage !!
	qer_editorimage textures/nki/nki_lava5.tga
	surfaceparm lava
//	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm noimpact
	surfaceparm trans
	q3map_surfacelight 1500
	cull none
	deformvertexes wave 50 sin 2 3 0 0.2 
	q3map_flare textures/flares/lava
	{
		map textures/nki/nki_lava5.tga
		tcmod scroll -.05 .001
		tcMod turb .2 -0.3 0 0.07
	}

}

textures/mapobjects/palmtree1
{
	qer_editorimage models/mapobjects/palmier/palmier1.tga
	surfaceparm nonsolid
	surfaceparm noimpact
//	surfaceparm trans
	surfaceparm nomarks
	surfaceparm alphashadow
	cull disable
	{
		map models/mapobjects/palmier/palmier1.tga
		alphaFunc GE128
		RgbGen vertex
	}
}

textures/nki_shaders/forest4
{
	qer_editorimage textures/nki/nki_forest4.tga
	q3map_lightmapMergable
	q3map_nonPlanar
	q3map_shadeAngle 160
	surfaceparm nonsolid
	surfaceparm lightfilter
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm alphashadow

	cull disable
	{
		map textures/nki/nki_forest4.tga
//		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GE128
//		rgbGen identity
	}
	// {
		// map $lightmap
		// blendFunc GL_DST_COLOR GL_ZERO
		// rgbGen identity
	// }
}


textures/mapobjects/leafs1
{
	qer_editorimage textures/mapobjects/leafs1.tga
	surfaceparm nonsolid
	surfaceparm lightfilter
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm noimpact
	surfaceparm alphashadow
	cull disable
	{
		map textures/mapobjects/leafs1.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GE128
//		rgbGen identity
	}
	// {
		// map $lightmap
		// blendFunc GL_DST_COLOR GL_ZERO
		// rgbGen identity
	// }
}

textures/mapobjects/leafs2
{
	qer_editorimage textures/mapobjects/leafs2.tga
	surfaceparm nonsolid
	surfaceparm lightfilter
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm alphashadow
	cull disable
	{
		map textures/mapobjects/leafs2.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GE128
//		rgbGen identity
	}
	// {
		// map $lightmap
		// blendFunc GL_DST_COLOR GL_ZERO
		// rgbGen identity
	// }
}

textures/mapobjects/leafs3
{
	qer_editorimage textures/mapobjects/leafs3.tga
	surfaceparm nonsolid
	surfaceparm lightfilter
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm alphashadow
	cull disable
	{
		map textures/mapobjects/leafs3.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		//alphaFunc GE128
//		rgbGen identity
	}
	// {
		// map $lightmap
		// blendFunc GL_DST_COLOR GL_ZERO
		// rgbGen identity
	// }
}

textures/mapobjects/leafs4
{
	qer_editorimage textures/mapobjects/leafs4.tga
	surfaceparm nonsolid
	surfaceparm lightfilter
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm alphashadow
	cull disable
	{
		map textures/mapobjects/leafs4.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		//alphaFunc GE128
//		rgbGen identity
	}
	// {
		// map $lightmap
		// blendFunc GL_DST_COLOR GL_ZERO
		// rgbGen identity
	// }
}