// ------------------------------------------------------------
// Shaders for Q3Rally Cars - rewritten by insellium
// based on all car shader files
// ------------------------------------------------------------

// ------------------------------------------------------------
// 1.ALPINE
// ------------------------------------------------------------

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

// ------------------------------------------------------------
// 2.CAMARO
// ------------------------------------------------------------

//CAMARO COLORS

//DEFAULT - YELLOW


models/players/camaro/body_yellow
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/body_yellow.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/lines1_yellow
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/lines1_yellow.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/lines2_yellow
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/lines2_yellow.tga
		alphaGen const 0.8
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
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/lines1_blue
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/lines1_blue.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/lines2_blue
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/lines2_blue.tga
		alphaGen const 0.8
		blendFunc blend
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
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/lines1_green
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/lines1_green.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/lines2_green
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/lines2_green.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

//DEFAULT - RED


models/players/camaro/body_red
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/body_red.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/lines1_red
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/lines1_red.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/lines2_red
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/lines2_red.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

//DEFAULT - WHITE

models/players/camaro/body_white
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/body_white.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/lines1_white
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/lines1_white.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/lines2_white
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/lines2_white.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

//DEFAULT - BLACK


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
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/lines1_black
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/lines1_black.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/lines2_black
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/lines2_black.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

//SHARED - GLASS


models/players/camaro/glass
{
	{
		map textures/reflect/reflect.jpg
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/glass.tga
		alphaGen const 0.2
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

// ------------------------------------------------------------
// 3.MACDADDY
// ------------------------------------------------------------

//MACDADDY SHARED

models/players/macdaddy/tlites

{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
  	{
	  	map models/players/macdaddy/tlites.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/macdaddy/tlites_g.tga
            blendFunc add
		rgbGen identity
	}    
}


//MACDADDY COLORS

//RED

models/players/macdaddy/red_main

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg              
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/red_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

models/players/macdaddy/red_hlites

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg              
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/red_hlites.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map models/players/macdaddy/hlites_g.tga
            blendFunc add
		rgbGen identity
	}  
         
}

//BLACK

models/players/macdaddy/black_main

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/black_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

models/players/macdaddy/red_hlites

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/black_hlites.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map models/players/macdaddy/hlites_g.tga
            blendFunc add
		rgbGen identity
	}  
         
}

//BLUE

models/players/macdaddy/blue_main

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/blue_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

models/players/macdaddy/blue_hlites

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/blue_hlites.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map models/players/macdaddy/hlites_g.tga
            blendFunc add
		rgbGen identity
	}  
         
}


//CHECKER

models/players/macdaddy/checker_main

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/checker_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

models/players/macdaddy/checker_hlites

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/checker_hlites.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map models/players/macdaddy/hlites_g.tga
            blendFunc add
		rgbGen identity
	}  
         
}

models/players/macdaddy/checker_ornament

{
	cull disable
	
	{
		map models/players/macdaddy/checker_main.tga
		alphaFunc GE128
		depthWrite
		rgbGen lightingdiffuse
	}
}



//CLASSIC

models/players/macdaddy/classic_main

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/classic_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

models/players/macdaddy/classic_hlites

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/classic_hlites.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map models/players/macdaddy/hlites_g.tga
            blendFunc add
		rgbGen identity
	}  
         
}

models/players/macdaddy/classic_ornament

{
	cull disable
	
	{
		map models/players/macdaddy/classic_main.tga
		alphaFunc GE128
		depthWrite
		rgbGen lightingdiffuse
	}
}


//GREEN

models/players/macdaddy/green_main

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/green_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

models/players/macdaddy/green_hlites

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/green_hlites.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map models/players/macdaddy/hlites_g.tga
            blendFunc add
		rgbGen identity
	}  
         
}

models/players/macdaddy/green_ornament

{
	cull disable
	
	{
		map models/players/macdaddy/green_main.tga
		alphaFunc GE128
		depthWrite
		rgbGen lightingdiffuse
	}
}


//PINK

models/players/macdaddy/pink_main

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/pink_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

models/players/macdaddy/pink_hlites

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/pink_hlites.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map models/players/macdaddy/hlites_g.tga
            blendFunc add
		rgbGen identity
	}  
         
}


//PURPLE

models/players/macdaddy/purple_main

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/purple_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

models/players/macdaddy/purple_hlites

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/purple_hlites.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map models/players/macdaddy/hlites_g.tga
            blendFunc add
		rgbGen identity
	}  
         
}

models/players/macdaddy/purple_ornament

{
	cull disable
	
	{
		map models/players/macdaddy/purple_main.tga
		alphaFunc GE128
		depthWrite
		rgbGen lightingdiffuse
	}
}

//WHITE

models/players/macdaddy/white_main

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/white_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

models/players/macdaddy/white_hlites

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/white_hlites.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map models/players/macdaddy/hlites_g.tga
            blendFunc add
		rgbGen identity
	}  
         
}


//YELLOW

models/players/macdaddy/yellow_main

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/yellow_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

models/players/macdaddy/yellow_hlites

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/yellow_hlites.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map models/players/macdaddy/hlites_g.tga
            blendFunc add
		rgbGen identity
	}  
         
}

models/players/macdaddy/yellow_ornament

{
	cull disable
	
	{
		map models/players/macdaddy/yellow_main.tga
		alphaFunc GE128
		depthWrite
		rgbGen lightingdiffuse
	}
}

//Hit or Miss

models/players/macdaddy/hitormiss_main

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/hitormiss_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

models/players/macdaddy/hitormiss_hlites

{      
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/hitormiss_hlites.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map models/players/macdaddy/hlites_g.tga
            blendFunc add
		rgbGen identity
	}  
         
}

// ------------------------------------------------------------
// 4.MINI
// ------------------------------------------------------------

//MINI SHADERS

models/players/mini/main
{      
      {
	  	map models/players/mini/main.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		blendFunc add       
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/main.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//          blendFunc GL_SRC_ALPHA GL_ONE
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/hlites
{

      {
	  	map models/players/mini/hlites.tga
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
	  	map models/players/mini/hlites.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
      {
		map models/players/mini/hlites_g.tga
            blendFunc add
		rgbGen lightingdiffuse
	}    
}

models/players/mini/tlites
{
  	{
	  	map models/players/mini/tlites.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/mini/tlites_g.tga
            blendFunc add
		rgbGen lightingdiffuse
	}    
}

models/players/mini/roof
{      
      {
	  	map models/players/mini/roof.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
            tcGen environment
		blendFunc add
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/roof.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/glass_int
{
	{
		map models/players/mini/glass_int.tga
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/mini/glass
{
	{
		map models/players/mini/glass.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		tcGen environment
		blendFunc add
	}
	{
		map models/players/mini/glass_r.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}


models/players/mini/int
{
	{
		map models/players/mini/int.tga
		rgbGen lightingdiffuse
	}
}
/*
models/players/mini/fogflare
{
	{
		map models/players/mini/fogflare.tga
		blendFunc add
		rgbGen lightingdiffuse
	}
}
*/
models/players/mini/foglite
{
	{
		map models/players/mini/foglite.tga
		rgbGen lightingdiffuse
	}
	{
		map models/players/mini/foglite_g.tga
		blendFunc add
		rgbGen lightingdiffuse
	}
}
/*
models/players/mini/rfoglite
{
	{
		map models/players/mini/rfoglite.tga
		rgbGen lightingdiffuse
	}
	{
		map models/players/mini/rfoglite_g.tga
		blendFunc add
		rgbGen lightingdiffuse
	}
}
*/

//UNION JACK VERSION

models/players/mini/main_uj
{      
      {
	  	map models/players/mini/main_uj.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		blendFunc add       
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/main_uj.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//          blendFunc GL_SRC_ALPHA GL_ONE
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/mini/hlites_uj
{

      {
	  	map models/players/mini/hlites_uj.tga
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
	  	map models/players/mini/hlites_uj.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
      {
		map models/players/mini/hlites_g.tga
            blendFunc add
		rgbGen lightingdiffuse
	}    
}

models/players/mini/roof_uj
{      
      {
	  	map models/players/mini/roof_uj.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
            tcGen environment
            blendFunc add
	}
      {
	  	map models/players/mini/roof_uj.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/glass_uj
{
	{
		map models/players/mini/glass_uj.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		tcGen environment
		blendFunc add
	}
	{
		map models/players/mini/glass_uj_r.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

//YELLOW VERSION

models/players/mini/main_yellow
{      
      {
	  	map models/players/mini/main_yellow.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		blendFunc add       
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/main_yellow.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//          blendFunc GL_SRC_ALPHA GL_ONE
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/mini/hlites_yellow
{

      {
	  	map models/players/mini/hlites_yellow.tga
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
	  	map models/players/mini/hlites_yellow.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
      {
		map models/players/mini/hlites_g.tga
            blendFunc add
		rgbGen lightingdiffuse
	}    
}

models/players/mini/roof_yellow
{      
      {
	  	map models/players/mini/roof_yellow.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
            tcGen environment
            blendFunc add
	}
      {
	  	map models/players/mini/roof_yellow.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/glass_yellow
{
	{
		map models/players/mini/glass_yellow.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		tcGen environment
		blendFunc add
	}
	{
		map models/players/mini/glass_yellow_r.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}


//GREEN VERSION

models/players/mini/main_green
{      
      {
	  	map models/players/mini/main_green.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		blendFunc add       
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/main_green.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//          blendFunc GL_SRC_ALPHA GL_ONE
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/mini/hlites_green
{

      {
	  	map models/players/mini/hlites_green.tga
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
	  	map models/players/mini/hlites_green.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
      {
		map models/players/mini/hlites_g.tga
            blendFunc add
		rgbGen lightingdiffuse
	}    
}


models/players/mini/glass_green
{
	{
		map models/players/mini/glass_green.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		tcGen environment
		blendFunc add
	}
	{
		map models/players/mini/glass_green_r.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}


//GAMESPY VERSION

models/players/mini/gamespy_main
{      
      {
	  	map models/players/mini/gamespy_main.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		blendFunc add       
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/gamespy_main.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//          blendFunc GL_SRC_ALPHA GL_ONE
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/mini/gamespy_hlites
{

      {
	  	map models/players/mini/gamespy_hlites.tga
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
	  	map models/players/mini/gamespy_hlites.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
      {
		map models/players/mini/hlites_g.tga
            blendFunc add
		rgbGen lightingdiffuse
	}    
}


models/players/mini/gamespy_glass
{
	{
		map models/players/mini/gamespy_glass.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		tcGen environment
		blendFunc add
	}
	{
		map models/players/mini/gamespy_glass_r.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}


models/players/mini/gamespy_roof
{      
      {
	  	map models/players/mini/gamespy_roof.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
            tcGen environment
		blendFunc add
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/gamespy_roof.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}



//TELEFRAGGED VERSION

models/players/mini/telefragged_main
{      
      {
	  	map models/players/mini/telefragged_main.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		blendFunc add       
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/telefragged_main.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//          blendFunc GL_SRC_ALPHA GL_ONE
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/mini/telefragged_hlites
{

      {
	  	map models/players/mini/telefragged_hlites.tga
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
	  	map models/players/mini/telefragged_hlites.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
      {
		map models/players/mini/hlites_g.tga
            blendFunc add
		rgbGen lightingdiffuse
	}    
}


models/players/mini/telefragged_glass
{
	{
		map models/players/mini/telefragged_glass.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		tcGen environment
		blendFunc add
	}
	{
		map models/players/mini/telefragged_glass_r.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/mini/telefragged_roof
{      
      {
	  	map models/players/mini/telefragged_roof.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
            tcGen environment
		blendFunc add
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/telefragged_roof.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

//VAPORWAVE VERSION

models/players/mini/main_vaporwave
{      
      {
	  	map models/players/mini/main_vaporwave.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		blendFunc add       
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/main_vaporwave.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//          blendFunc GL_SRC_ALPHA GL_ONE
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/mini/hlites_vaporwave
{

      {
	  	map models/players/mini/hlites_vaporwave.tga
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
	  	map models/players/mini/hlites_vaporwave.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
      {
		map models/players/mini/hlites_g.tga
            blendFunc add
		rgbGen lightingdiffuse
	}    
}

models/players/mini/roof_vaporwave
{      
      {
	  	map models/players/mini/roof_vaporwave.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
            tcGen environment
            blendFunc add
	}
      {
	  	map models/players/mini/roof_vaporwave.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/glass_vaporwave
{
	{
		map models/players/mini/glass_vaporwave.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		tcGen environment
		blendFunc add
	}
	{
		map models/players/mini/glass_uj_r.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

//BROWN

models/players/mini/brown_main
{      
      {
	  	map models/players/mini/brown_main.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		blendFunc add       
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/brown_main.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//          blendFunc GL_SRC_ALPHA GL_ONE
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/mini/hlites_brown
{

      {
	  	map models/players/mini/hlites_brown.tga
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
	  	map models/players/mini/hlites_brown.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
      {
		map models/players/mini/hlites_g.tga
            blendFunc add
		rgbGen lightingdiffuse
	}    
}


models/players/mini/glass_brown
{
	{
		map models/players/mini/glass_brown.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg  
		tcGen environment
		blendFunc add
	}
	{
		map models/players/mini/glass_brown_r.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

// ------------------------------------------------------------
// 5.MURCIELAGO
// ------------------------------------------------------------

//moddb
models/players/murcielago/moddb_main
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/moddb_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

//flame
models/players/murcielago/flame_main
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/flame_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

//whitestripe
models/players/murcielago/silverstripe_main
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/silverstripe_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

//bluestripe
models/players/murcielago/bluestripe_main
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/bluestripe_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

//redstripe
models/players/murcielago/redstripe_main
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/redstripe_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

//whitehot
models/players/murcielago/whitehot_main
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/whitehot_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

//yellow
models/players/murcielago/yellow_main
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/yellow_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/murcielago/yellow_under
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/yellow_under.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

//green
models/players/murcielago/green_main
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/green_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/murcielago/green_under
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/green_under.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

//black
models/players/murcielago/black_main
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/black_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/murcielago/black_under
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/black_under.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

//silver
models/players/murcielago/silver_main
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/silver_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/murcielago/silver_under
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/silver_under.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

//red
models/players/murcielago/red_main
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/red_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/murcielago/red_under
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/red_under.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

//blue
models/players/murcielago/blue_main
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/blue_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/murcielago/blue_under
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/blue_under.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

// standard
models/players/murcielago/glass
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/glass.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/murcielago/blite
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/blite.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
	{
		map models/players/murcielago/blite_g.tga
		blendfunc add
		rgbGen identity
	}
}

models/players/murcielago/blitec
{
	{
		map textures/reflect/reflect.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/blitec.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/murcielago/hlite
{
	{
		map models/players/murcielago/hlite.tga
		rgbGen lightingDiffuse
	}
	{
		map models/players/murcielago/hlite_g.tga
		blendfunc add
		rgbGen identity
	}
}

models/players/murcielago/hglass
{
	{
		map models/players/murcielago/hglass_t.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
		alphaFunc GT0
	}
	{
		map textures/reflect/reflect.tga
		blendfunc add
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/hglass.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

// ------------------------------------------------------------
// 6.RAPTOR
// ------------------------------------------------------------

models/players/raptor/hlites
{
  	{
	  	map models/players/raptor/hlites.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/raptor/hlites_g.tga
            blendFunc add
		rgbGen identity
	}    
}


models/players/raptor/flites
{
  	{
	  	map models/players/raptor/flites.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/raptor/flites_g.tga
            blendFunc add
		rgbGen identity
	}    
}



models/players/raptor/tlites
{
  	{
	  	map models/players/raptor/tlites.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/raptor/tlites.tga
            blendFunc add
		rgbGen identity
	}    
}

//GREEN

models/players/raptor/green
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/green.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/raptor/green_parts
{      

	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/green_parts.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}


//BLACK

models/players/raptor/black
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/black.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/raptor/black_parts
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/black_parts.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}


//BLUE

models/players/raptor/blue
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/blue.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/raptor/blue_parts
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/blue_parts.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

//JP

models/players/raptor/jp
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/jp.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/raptor/jp_parts
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/jp_parts.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}


//PINK

models/players/raptor/pink
{      

	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/pink.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/raptor/pink_parts
{      

	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/pink_parts.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}


//RED

models/players/raptor/red
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/red.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/raptor/red_parts
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/red_parts.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}


//YELLOW

models/players/raptor/yellow
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/yellow.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/raptor/yellow_parts
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/yellow_parts.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

//WEBDOG - WWW.WEBDOG.ORG

models/players/raptor/webdog
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/webdog.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/raptor/webdog_parts
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/webdog_parts.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/raptor/webdog_lites
{
  	{
	  	map models/players/raptor/webdog_lites2.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/raptor/flites_g.tga
            blendFunc add
		rgbGen identity
	}    
}

models/players/raptor/webdog_lites2
{
  	{
	  	map models/players/raptor/webdog_lites.tga
		depthWrite
		rgbGen lightingdiffuse
	}
}



//..::LvL - WWW.PLANETQUAKE.COM/LVL

models/players/raptor/lvl
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/lvl.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}


models/players/raptor/lvl_parts
{      
	{
	//	map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/lvl_parts.tga
       //     blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/raptor/lvl_lites
{
  	{
	  	map models/players/raptor/lvl_lites.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/raptor/flites_g.tga
            blendFunc add
		rgbGen identity
	}    
}

// DREAMCAST

models/players/raptor/dreamcast
{      
	{
		map textures/reflect/reflect.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/dreamcast.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

// ------------------------------------------------------------
// 7.REAPER
// ------------------------------------------------------------

//REAPER SHARED

models/players/reaper/net
{

	{
		map models/players/reaper/parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}

//REAPER COLORS

//DEFAULT SKIN

models/players/reaper/main
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/parts
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/parts.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/hlites
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/tlites
{
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
  	{
	  	map models/players/reaper/tlites.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/bumper

{
	{
		map models/players/reaper/parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}



//RED SKIN

models/players/reaper/net_red
{

	{
		map models/players/reaper/parts_red.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}


models/players/reaper/main_red
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/main_red.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/parts_red
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/parts_red.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/hlites_red
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/main_red.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/tlites_red
{
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
  	{
	  	map models/players/reaper/tlites_red.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/bumper_red

{
	{
		map models/players/reaper/parts_red.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}


//YELLOW SKIN

models/players/reaper/net_yellow
{

	{
		map models/players/reaper/parts_yellow.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}


models/players/reaper/main_yellow
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/main_yellow.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/parts_yellow
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/parts_yellow.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/hlites_yellow
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/main_yellow.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/tlites_yellow
{
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
  	{
	  	map models/players/reaper/tlites_yellow.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/bumper_yellow

{
	{
		map models/players/reaper/parts_yellow.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}


//GREEN SKIN

models/players/reaper/net_green
{

	{
		map models/players/reaper/parts_green.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}


models/players/reaper/main_green
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/main_green.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/parts_green
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/parts_green.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/hlites_green
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/main_green.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/tlites_green
{
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
  	{
	  	map models/players/reaper/tlites_green.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/bumper_green

{
	{
		map models/players/reaper/parts_green.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}


//BLUE SKIN

models/players/reaper/net_blue
{

	{
		map models/players/reaper/parts_blue.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}


models/players/reaper/main_blue
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/main_blue.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/parts_blue
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/parts_blue.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/hlites_blue
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/main_blue.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/tlites_blue
{
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
  	{
	  	map models/players/reaper/tlites_blue.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/bumper_blue

{
	{
		map models/players/reaper/parts_blue.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}


//TEAM RACE SKINS

//BARRYSWORLD SKIN

models/players/reaper/net_bw
{

	{
		map models/players/reaper/bw_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}


models/players/reaper/main_bw
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/bw.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/parts_bw
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/bw_parts.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/hlites_bw
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/bw.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/tlites_bw
{
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
  	{
	  	map models/players/reaper/bw_tlites.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/bumper_bw

{
	{
		map models/players/reaper/bw_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}


//SHUGASHACK SKIN

models/players/reaper/net_shack
{

	{
		map models/players/reaper/shack_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}


models/players/reaper/main_shack
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/shack.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/parts_shack
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/shack_parts.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/hlites_shack
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/shack.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/tlites_shack
{
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
  	{
	  	map models/players/reaper/shack_tlites.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/bumper_shack

{
	{
		map models/players/reaper/shack_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}

// ------------------------------------------------------------
// 8.SIDEPIPE
// ------------------------------------------------------------

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

// ------------------------------------------------------------
// 9.SLINGSHOT
// ------------------------------------------------------------

//COMMON

models/players/slingshot/glass
{
	{
		map textures/reflect/reflect.jpg
		tcGen environment
            rgbGen lightingdiffuse
	}
	{
		map models/players/slingshot/glass.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

models/players/slingshot/glass_conv
{
	cull none

	{
		map textures/reflect/reflect.jpg
		tcGen environment
		blendFunc add
            rgbGen lightingdiffuse
	}

	{
		map models/players/slingshot/glass.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}


models/players/slingshot/roof
{
	{
		map textures/reflect/reflect.jpg
		tcGen environment
            rgbGen lightingdiffuse

	}
	{
		map models/players/slingshot/roof.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}



//models/players/slingshot/int
//
//{
//	{
//		map models/players/slingshot/int.tga
//            rgbGen lightingdiffuse
//
//	}
//}

models/players/slingshot/tlites

{
	{
		map models/players/slingshot/tlites.tga
            rgbGen lightingdiffuse

	}
}
models/players/slingshot/hlites

{
	{
		map models/players/slingshot/hlites.tga
            rgbGen lightingdiffuse

	}
}

//SLINGSHOT COLORS

models/players/slingshot/main
{

	{
		map textures/reflect/reflect.jpg
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/slingshot/blue_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

models/players/slingshot/blue_main
{

	{
		map textures/reflect/reflect.jpg
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/slingshot/blue_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

models/players/slingshot/yellow_main
{

	{
		map textures/reflect/reflect.jpg
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/slingshot/yellow_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

models/players/slingshot/red_main
{

	{
		map textures/reflect/reflect.jpg
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/slingshot/red_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

models/players/slingshot/green_main
{

	{
		map textures/reflect/reflect.jpg
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/slingshot/green_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

//PS2
models/players/slingshot/ps2_main
{

	{
		map textures/reflect/reflect.jpg
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/slingshot/ps2_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

models/players/slingshot/ps2_roof
{
	{
		map textures/reflect/reflect.jpg
		tcGen environment
            rgbGen lightingdiffuse

	}
	{
		map models/players/slingshot/ps2_roof.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

//PLANETQUAKE.DE 

models/players/slingshot/pqde_main
{

	{
		map textures/reflect/reflect.jpg
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/slingshot/pqde_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}


models/players/slingshot/pqde_roof
{
	{
		map textures/reflect/reflect.jpg
		tcGen environment
            rgbGen lightingdiffuse

	}
	{
		map models/players/slingshot/pqde_roof.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

models/players/slingshot/pqde_tlites

{
	{
		map models/players/slingshot/pqde_tlites.tga
            rgbGen lightingdiffuse

	}
}

//THE BIG HOUSE 

models/players/slingshot/tbh_main
{

	{
		map textures/reflect/reflect.jpg
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/slingshot/tbh_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}


models/players/slingshot/tbh_roof
{
	{
		map textures/reflect/reflect.jpg
		tcGen environment
            rgbGen lightingdiffuse

	}
	{
		map models/players/slingshot/tbh_roof.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

models/players/slingshot/tbh_tlites

{
	{
		map models/players/slingshot/tbh_tlites.tga
            rgbGen lightingdiffuse

	}
}

// ------------------------------------------------------------
// 10.VIPER
// ------------------------------------------------------------

//red

models/players/viper/red
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/red.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/windows
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/windows.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/r_lites_red
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/r_lites_red.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/f_lites_red
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/f_lites_red.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

//blue

models/players/viper/blue
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/blue.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/windows
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/windows.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/r_lites_blue
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/r_lites_blue.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/f_lites_blue
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/f_lites_blue.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

//yellow

models/players/viper/yellow
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/yellow.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/windows
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/windows.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/r_lites_yellow
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/r_lites_yellow.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/f_lites_yellow
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/f_lites_yellow.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

//graphite

models/players/viper/grey
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/grey.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/windows
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/windows.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/r_lites_grey
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/r_lites_grey.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/f_lites_grey
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/f_lites_grey.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

//stripe_red

models/players/viper/stripe_red
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/stripe_red.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/windows
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/windows.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/r_lites_red
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/r_lites_red.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/f_lites_red
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/f_lites_red.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

//stripe_blue

models/players/viper/stripe_blue
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/stripe_blue.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/windows
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/windows.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/r_lites_blue
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/r_lites_blue.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/f_lites_blue
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/f_lites_blue.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

//white

models/players/viper/white
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/white.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/windows
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/windows.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/r_lites_white
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/r_lites_white.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/f_lites_white
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/f_lites_white.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

//green

models/players/viper/green
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/green.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/windows
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/windows.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/r_lites_green
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/r_lites_green.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/f_lites_green
{
	{
//		map textures/effects/tinfx2b.tga
		map textures/reflect/reflect.jpg          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/f_lites_green.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}


}

//GRAVELORD

models/players/gravelord/default
{
	{
		map textures/reflect/reflect.tga
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/default.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

//TEAM COLOR SKINS

models/players/gravelord/green
{
	{
		map textures/reflect/reflect.tga
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/green.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/gravelord/blue
{
	{
		map textures/reflect/reflect.tga
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/blue.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/gravelord/yellow
{
	{
		map textures/reflect/reflect.tga
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/yellow.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/gravelord/red
{
	{
		map textures/reflect/reflect.tga
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/red.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

//ECTO-1 GHOSTBUSTERS SKIN

models/players/gravelord/ecto
{
	{
		map textures/reflect/reflect.tga
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/ecto.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/gravelord/windows
{
	{
//		map textures/effects/tinfx2.tga
		map textures/reflect/reflect.tga
		blendfunc add
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/windows.tga
		blendfunc gl_one gl_src_alpha
		rgbGen lightingDiffuse
	}
}

models/players/gravelord/curtain
{
	cull disable
	{
		map models/players/gravelord/curtain.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/gravelord/coffin
{
	{
		map textures/reflect/reflect.tga
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/coffin.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/gravelord/no_curtain
{

	{
		map models/players/gravelord/no_curtain.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/gravelord/no_coffin
{

	{
		map models/players/gravelord/no_coffin.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/gravelord/gb_windows
{
	{
//		map textures/effects/tinfx2.tga
		map textures/reflect/reflect.tga
		blendfunc add
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/gb_windows.tga
		blendfunc gl_one gl_src_alpha
		rgbGen lightingDiffuse
	}
}