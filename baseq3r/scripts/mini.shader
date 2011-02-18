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
