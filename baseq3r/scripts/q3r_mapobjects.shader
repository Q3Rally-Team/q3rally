//watertower

models/mapobjects/watertower/plate01

{
     	{
	        map textures/effects/tinfx2d.tga
		tcGen environment
                rgbGen identity
       }


       {
	        map textures/q3r_metals/plate01.tga
		blendfunc GL_ONE GL_SRC_COLOR
                rgbGen vertex
       }

}

models/mapobjects/watertower/plate02

{
     	{
	        map textures/effects/tinfx2d.tga
		tcGen environment
                rgbGen identity
       }


       {
	        map textures/q3r_metals/plate01.tga
		blendfunc GL_ONE GL_SRC_COLOR
                rgbGen vertex
       }

}


models/mapobjects/watertower/ferris

{
	{
	        map textures/effects/tinfx2d.tga
		tcGen environment
                rgbGen identity
       }


       {
	        map models/mapobjects/watertower/ferris.tga
		blendfunc GL_ONE GL_SRC_COLOR
                rgbGen vertex
       }


}

models/mapobjects/watertower/rail01
{
	cull disable
	surfaceparm alphashadow
	surfaceparm playerclip
	{
		map textures/q3r_metals/rail01.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
		rgbGen vertex
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}

}

models/mapobjects/watertower/walkway01

{
	cull disable
	
	{
	map models/mapobjects/watertower/walkway01.tga
	rgbGen vertex
	}
}

//UFO


models/mapobjects/ufo/ufo01

{
     	{
	        map textures/reflect/reflect.jpg
		tcGen environment
                rgbGen identity
       }


       {
	        map models/mapobjects/ufo/ufo01.tga
		blendfunc GL_ONE GL_SRC_COLOR
                rgbGen vertex
       }

}


models/mapobjects/ufo/ufo03

{
     	{
	        map textures/reflect/reflect.jpg
		tcGen environment
                rgbGen identity
       }


       {
	        map models/mapobjects/ufo/ufo03.tga
		blendfunc GL_ONE GL_SRC_COLOR
                rgbGen vertex
       }

}

