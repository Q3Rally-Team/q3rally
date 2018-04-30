//CAMARO SHADER SCRIPTS

//CAMARO COLORS

//DEFAULT - YELLOW


models/players/camaro/body
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg            
            	tcGen environment
            	rgbGen lightingdiffuse
	}
      {
	  	map models/players/camaro/body.tga
            	blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

//DEFAULT - BLUE


models/players/camaro/body_blue
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg            
            	tcGen environment
		rgbGen lightingdiffuse
	}
      {
	  	map models/players/camaro/body_blue.tga
            	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

//DEFAULT - GREEN


models/players/camaro/body_green
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg            
            	tcGen environment
		rgbGen lightingdiffuse
	}
      {
	  	map models/players/camaro/body_green.tga
            	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

//DEFAULT - RED


models/players/camaro/body_green
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg            
            	tcGen environment
		rgbGen lightingdiffuse
	}
      {
	  	map models/players/camaro/body_red.tga
            	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

//DEFAULT - GREEN


models/players/camaro/body_black
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg            
            	tcGen environment
		rgbGen lightingdiffuse
	}
      {
	  	map models/players/camaro/body_black.tga
            	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}
