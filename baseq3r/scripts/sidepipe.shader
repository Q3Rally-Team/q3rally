
//SIDEPIPE MAPS

//SIDEPIPE SHARED

//HEADLITE AND TAILLITE 

models/players/sidepipe/facias

{
  	{
	  	map models/players/sidepipe/facias.tga
		rgbGen lightingdiffuse
	}
      {
		map models/players/sidepipe/facias_g.tga
            blendFunc add
		rgbGen identity
	}    
}


//TRANSMISSION CASING

models/players/sidepipe/tranny

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
            tcGen environment
            rgbGen identity
	}
      {
	  	map models/players/sidepipe/tranny.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
         
}




//SIDEPIPE COLORS

//ORANGE

models/players/sidepipe/orange
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
            tcGen environment
            rgbGen identity
	}
      {
	  	map models/players/sidepipe/orange.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}


//RED

models/players/sidepipe/red
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/sidepipe/red.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}



//BLUE

models/players/sidepipe/blue
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/sidepipe/blue.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
         
}


//GREEN

models/players/sidepipe/green
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/sidepipe/green.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
         
}


//YELLOW

models/players/sidepipe/yellow
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg           
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/sidepipe/yellow.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
         
}

//POLYCOUNT SPONSOR SKIN - www.planetquake.com/polycount
//SKIN BY

models/players/sidepipe/polycount
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/sidepipe/polycount.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
         
}

models/players/sidepipe/polycount_f
{
  	{
	  	map models/players/sidepipe/polycount_f.tga
		rgbGen lightingdiffuse
	}
      {
		map models/players/sidepipe/facias_g.tga
            blendFunc add
		rgbGen identity
	}    
}


//ROCKET ARENA 3 SPONSOR SKIN - www.planetquake.com/arena

models/players/sidepipe/ra3
{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/sidepipe/ra3.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
         
}

models/players/sidepipe/ra3_f
{
  	{
	  	map models/players/sidepipe/ra3_f.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/sidepipe/facias_g.tga
            blendFunc add
		rgbGen identity
	}    
}


