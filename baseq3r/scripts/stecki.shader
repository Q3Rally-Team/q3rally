textures/stecki/beam
{
	qer_editorimage textures/stecki/beam.tga

        surfaceparm trans	
        surfaceparm nomarks	
        surfaceparm nonsolid
	surfaceparm nolightmap
	deformVertexes autosprite2
	cull none

	{
		map textures/stecki/beam.tga
                blendFunc GL_SRC_ALPHA GL_ONE
        }

     
}


textures/stecki/flare
{
	qer_editorimage textures/stecki/flare1.tga

        surfaceparm trans	
        surfaceparm nomarks	
        surfaceparm nonsolid
	surfaceparm nolightmap
	cull none
	{
		map textures/stecki/flare1.tga
                blendFunc add
        }

     
}


//steam1

textures/stecki/steam01
{
	qer_editorimage textures/stecki/steam1.tga


        surfaceparm trans	
        surfaceparm nomarks	
        surfaceparm nonsolid
//	surfaceparm nolightmap
	cull none
//	surfaceparm nomipmaps


//	{
//		map textures/stecki/steam_mask1.tga
//		blendFunc	add
//		depthWrite
//		rgbGen identity
//	}

//	{
//		map textures/stecki/steam1.tga
//		tcMod scroll 0 -1
//		blendFunc add
//		blendFunc	GL_DST_COLOR GL_ZERO
//		depthWrite
//		rgbGen identity
//	}
	{
		map textures/stecki/manhole1.tga
		tcMod scroll 0 -1.3
		blendFunc GL_ZERO GL_SRC_COLOR
		depthWrite
		rgbGen identity
	}
	{
		map textures/stecki/steam_mask1.tga
//		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		blendFunc GL_SRC_ALPHA GL_ONE
		depthWrite
		rgbGen identity
	}



//	{
//		map textures/stecki/steamscroll1.tga
//		tcMod scroll 0 -1.3
//		blendFunc add
//		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
//		depthWrite
//		rgbGen identity
//	}

}

//fffdrip1

textures/stecki/droplet
{
	qer_editorimage textures/stecki/droplet.tga


//      surfaceparm trans	
        surfaceparm nomarks	
        surfaceparm nonsolid
	surfaceparm nolightmap
	cull none
//	surfaceparm nomipmaps


	{
		animMap 3 textures/stecki/droplet.tga textures/stecki/droplet2.tga textures/stecki/droplet2.tga textures/stecki/droplet2.tga
		tcMod scroll 0 -3
		blendFunc add
//		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
//		depthWrite
	}

}

//fffsplash

textures/stecki/splash
{
        surfaceparm nomarks	
        surfaceparm nonsolid
	surfaceparm nolightmap
	cull none
	{
		clampmap textures/stecki/splash.tga
		blendfunc add
		tcMod stretch sin .7 .5 0 1.5
//		rgbGen wave square .5 .5 .25 1.5
//depthWrite
	}
}


textures/stecki/quadglobe
{
	qer_editorimage textures/effects/quadmap2.tga
	surfaceparm nolightmap

//	deformVertexes wave 100 sin 3 0 0 0
	{
		map textures/effects/quadmap2.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
		//tcMod turb 0 0.2 0 .2
                tcmod scroll 1 .1
	}
}

textures/stecki/fffchrome
{      
	surfaceparm nolightmap
	{
		map textures/effects/tinfx2c.tga
            tcGen environment
          rgbGen identity
	}
	{
		map textures/effects/quadmap2.tga
		blendfunc GL_ONE GL_ONE
//		tcGen environment
                tcmod rotate 30
                tcmod scroll 1 .1
	}
}

textures/stecki/lightning1
{
	qer_editorimage textures/stecki/lightning1
	surfaceparm trans	
      surfaceparm nomarks	
      surfaceparm nonsolid
	surfaceparm nolightmap
	cull none
	q3map_surfacelight 1000
	//light 1

	// lightning
//	{
//		map $lightmap
//		rgbGen identity
//	}
	{
		map textures/stecki/lightning1.tga
		blendFunc add
		tcmod scroll 0 10
		rgbGen identity
	}

}


