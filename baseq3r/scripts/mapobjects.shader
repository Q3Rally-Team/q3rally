//================
//	watertower	
//================

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

//==========
//	UFO	
//==========

models/mapobjects/ufo/ufo01

{
     	{
	        map textures/reflect/reflect.jpg
		tcGen environment
                rgbGen identity
       }


       {
	        map models/mapobjects/ufo/ufo01glow.tga
		tcMod scroll 2.1 0
               rgbGen wave sin 1 1 .5 .1
       }
      {
	        map models/mapobjects/ufo/ufo01glow.tga
		tcMod scroll 1.4 0
		blendFunc add
               rgbGen wave square 1 1 .5 2
       }
      {
	        map models/mapobjects/ufo/ufo01glow.tga
		tcMod scroll -.9 0
		blendFunc add
               rgbGen wave square 1 1 .25 .5
       }

       {
	        map models/mapobjects/ufo/ufo01.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
               rgbGen vertex
       }

}


models/mapobjects/ufo/ufo02

{
     	{
	        map textures/reflect/reflect.jpg
		tcGen environment
                rgbGen identity
       }

       {
	        map models/mapobjects/ufo/ufo02.tga
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

models/mapobjects/q3rtitle/q3rtitle_w
{
	{
		map textures/effects/envmapred.tga
//		map textures/sfx/fire_ctfred.jpg
//		map models/barrels/chrometest2.jpg
//		map textures/effects/tinfx2c.tga
//                map textures/sfx/specular.tga
                tcmod rotate 011
                tcmod scroll .1 .01
		tcGen environment
	}
}

models/mapobjects/q3rtitle/q3rtitle
{
	{
		map models/mapobjects/q3rtitle/q3rtitle_b.jpg
		
	}
}

models/mapobjects/barrels/b_lite

{
	{
		map models/mapobjects/barrels/b_lite.tga
		rgbGen identity
	}
	{
		map models/mapobjects/barrels/b_lite_g.tga
		blendfunc add
		rgbGen wave sin .6 1 0 .5	
	}
}

models/mapobjects/barrels/b_top03

{
	{
		map models/mapobjects/barrels/b_top03.tga
		rgbGen identity
	}
	{
		map models/mapobjects/barrels/b_top03_g.tga
		blendfunc add
		rgbGen wave sin .3 1 0 .5	
	}
}

//=====================================
// leave this in for replacement model
//=====================================


models/mapobjects/spotlamp/spotlamp
{
    cull disable
    surfaceparm alphashadow
        {
                map models/mapobjects/spotlamp/spotlamp.tga
                alphaFunc GE128
		depthWrite
		rgbGen vertex
        }


}
models/mapobjects/spotlamp/beam
{
        surfaceparm trans	
        surfaceparm nomarks	
        surfaceparm nonsolid
	surfaceparm nolightmap
	cull none
        nomipmaps
	{
		map models/mapobjects/spotlamp/beam.tga
                tcMod Scroll .3 0
                blendFunc GL_ONE GL_ONE
        }
        //{
	//	map models/mapobjects/spotlamp/beam.tga
         //       tcMod Scroll -.3 0
         //       blendFunc GL_ONE GL_ONE
        //}
     
}
models/mapobjects/spotlamp/spotlamp_l

{
      cull disable
      q3map_surfacelight	200

        {
                map models/mapobjects/spotlamp/spotlamp_l.tga
		rgbGen identity
        }


}

models/mapobjects/barrel/barrel2
{
	
	{
		Map models/mapobjects/barrel/barrel2.tga
                rgbgen vertex
               
	}	
        {
		clampmap models/mapobjects/barrel/barrel2fx.tga
		blendFunc GL_ONE GL_ONE
               // rgbgen wave triangle 1 1.4 0 9.5
                tcMod rotate 403
	}	
        {
		clampmap models/mapobjects/barrel/barrel2fx.tga
		blendFunc GL_ONE GL_ONE
               // rgbgen wave triangle 1 1 0 8.7
                tcMod rotate -100
	}	
	
}

//==========
// SPOTLAMP 
//==========

models/mapobjects/spotlamp/beam
{
  surfaceparm trans
  surfaceparm nomarks
  surfaceparm nonsolid
  surfaceparm nolightmap
  cull none
  nomipmaps
  {
    map models/mapobjects/spotlamp/beam.tga
    tcMod Scroll .3 0
    blendfunc add
  }
}

models/mapobjects/spotlamp/spotlamp
{
  cull disable
  surfaceparm alphashadow
  {
    map models/mapobjects/spotlamp/spotlamp.tga
    alphaFunc GT0
    depthWrite
    rgbGen vertex
  }
}

models/mapobjects/spotlamp/spotlamp_l
{
  cull disable
  q3map_surfacelight	200
  {
    map models/mapobjects/spotlamp/spotlamp_l.tga
    rgbGen identity
  }
}