//ALPINE SHADER SCRIPTS

//ALPINE SHARED


models/players/alpine/tlites
{
  	{
	  	map models/players/alpine/tlites.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/alpine/tlites_g.tga
            blendFunc add
		rgbGen identity
	}    
}

//ALPINE COLORS

//DEFAULT - YELLOW

models/players/alpine/yellowglass
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}

	{
		map models/players/alpine/yellow.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/alpine/yellow
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg            
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/yellow.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/yellowhglass
{      

      {
	  	map models/players/alpine/yellowhglass_t.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg   
		blendFunc add         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/yellowhglass.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/yellowhlite
{
  	{
	  	map models/players/alpine/yellowhlite.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/alpine/hliteglow.tga
            blendFunc add
		rgbGen identity
	}    
}


//BLUE

models/players/alpine/blueglass
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg            
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}

	{
		map models/players/alpine/blue.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/alpine/blue
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg            
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/blue.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/bluehglass
{      

      {
	  	map models/players/alpine/bluehglass_t.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg   
		blendFunc add         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/bluehglass.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/bluehlite
{
  	{
	  	map models/players/alpine/bluehlite.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/alpine/hliteglow.tga
            blendFunc add
		rgbGen identity
	}    
}


//RED


models/players/alpine/redglass
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg   
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}

	{
		map models/players/alpine/red.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/alpine/red
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg   
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/red.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/redhglass
{      

      {
	  	map models/players/alpine/redhglass_t.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg   
		blendFunc add         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/redhglass.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/redhlite
{
  	{
	  	map models/players/alpine/redhlite.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/alpine/hliteglow.tga
            blendFunc add
		rgbGen identity
	}    
}

//GREEN


models/players/alpine/greenglass
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg   
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}

	{
		map models/players/alpine/green.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/alpine/green
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg   
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/green.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/greenhglass
{      

      {
	  	map models/players/alpine/greenhglass_t.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg   
		blendFunc add         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/greenhglass.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/greenhlite
{
  	{
	  	map models/players/alpine/greenhlite.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/alpine/hliteglow.tga
            blendFunc add
		rgbGen identity
	}    
}


//FRENCH FRAG FACTORY SKIN


models/players/alpine/fffglass
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg   
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}

	{
		map models/players/alpine/fff.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/alpine/fff
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg   
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/fff.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/fffhglass
{      

      {
	  	map models/players/alpine/fffhglass_t.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg   
		blendFunc add         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/fffhglass.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/fffhlite
{
  	{
	  	map models/players/alpine/fffhlite.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/alpine/hliteglow.tga
            blendFunc add
		rgbGen identity
	}    
}

models/players/alpine/fffflite
{
  	{
	  	map models/players/alpine/fffflite.tga
		depthWrite
		rgbGen lightingdiffuse
	} 
}

//STOMPED SPONSOR SKIN

models/players/alpine/stompedglass
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg   
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}

	{
		map models/players/alpine/stomped.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/alpine/stomped
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg   
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/stomped.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/stompedhglass
{      

      {
	  	map models/players/alpine/stompedhglass_t.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg   
		blendFunc add         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/stompedhglass.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/stompedhlite
{
  	{
	  	map models/players/alpine/stompedhlite.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/alpine/hliteglow.tga
            blendFunc add
		rgbGen identity
	}    
}

models/players/alpine/stompedflite
{
  	{
	  	map models/players/alpine/stompedflite.tga
		depthWrite
		rgbGen lightingdiffuse
	} 
}
