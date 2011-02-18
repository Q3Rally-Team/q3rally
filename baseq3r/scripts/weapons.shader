models/rearfire/flametrail03
{
//	deformVertexes autoSprite	
        {
		clampmap models/mapobjects/barrel/barrel2fx.tga
		blendFunc GL_ONE GL_ONE
               // rgbgen wave triangle 1 1.4 0 9.5
                tcMod rotate 200
	}	
        {
		clampmap models/mapobjects/barrel/barrel2fx.tga
		blendFunc GL_ONE GL_ONE
               // rgbgen wave triangle 1 1 0 8.7
                tcMod rotate -100
	}	
	
}

models/rearfire/flametrail02
{

	//	*************************************************
	//	*      	Yellow Flame Surface Light 		*
	//	*      	April 30 1999				*	
	//	*	ripped from id hahahaha			*
	//	*************************************************
	
	surfaceparm nomarks
	surfaceparm nolightmap
	cull none

	{
		animMap 10 textures/sfx/flame1.tga textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave inverseSawtooth 0 1 0 10
		
	}	
	{
		animMap 10 textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga textures/sfx/flame1.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sawtooth 0 1 0 10
	}	


	{
		map textures/sfx/flameball.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sin .6 .2 0 .6	
	}

}

models/rearfire/flametrail01
{

	//	*************************************************
	//	*      	Yellow Flame Surface Light 		*
	//	*      	April 30 1999				*	
	//	*	ripped from id hahahaha			*
	//	*************************************************
	
	surfaceparm nomarks
	surfaceparm nolightmap
	cull none

	{
		animMap 10 textures/sfx/flame1.tga textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave inverseSawtooth 0 1 0 10
		
	}	
	{
		animMap 10 textures/sfx/flame2.tga textures/sfx/flame3.tga textures/sfx/flame4.tga textures/sfx/flame5.tga textures/sfx/flame6.tga textures/sfx/flame7.tga textures/sfx/flame8.tga textures/sfx/flame1.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sawtooth 0 1 0 10
	}	


	{
		map textures/sfx/flameball.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sin .6 .2 0 .6	
	}

}

gfx/damage/bio_mark
{
	polygonOffset
	{
		map gfx/damage/bio_mark.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
//          tcmod rotate 011
//          tcmod turb .999 .02 1 .1
		rgbGen exactVertex
	}
	{
		map gfx/damage/biomark_glow.tga
		blendFunc add
//          tcmod rotate 011
//          tcmod turb .999 .02 1 .1
		rgbGen exactVertex
//		rgbGen wave sin 1 .5 1 1
	}

}
gfx/damage/oil_mark
{
	polygonOffset


	{
		map gfx/damage/oil_mark.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
	{	
	      map textures/reflect/oilreflect.tga
		blendFunc add
	      rgbGen exactVertex
	}
	{
		map gfx/damage/oil_mark_reflect.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
}

sprites/flameball
{
	cull disable
	{
		clampmap sprites/flameball.tga
		blendfunc GL_ONE GL_ONE
                tcMod rotate 931
	}
}

