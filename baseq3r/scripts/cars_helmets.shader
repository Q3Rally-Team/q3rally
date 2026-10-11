// Helmet liveries: one skin per driver-avatar helmet (#13, #7, #27, #44, #88, #1) for every car.
// Generated from the stock red skins; see Claude outputs/helmet_skins.

// ALPINE RALLY13 (helmet #13 livery)
models/players/alpine/rally13_glass
{
	{
		map $dynamicreflection   
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}

	{
		map models/players/alpine/rally13.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/alpine/rally13_hglass
{      

      {
	  	map models/players/alpine/rally13_hglass_t.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map $dynamicreflection   
		blendFunc add         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/rally13_hglass.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/rally13_hlite
{
  	{
	  	map models/players/alpine/rally13_hlite.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/alpine/hliteglow.tga
            blendFunc add
		rgbGen identity
	}    
}

models/players/alpine/rally13
{      
	{
		map $dynamicreflection   
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/rally13.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

// ALPINE RALLY07 (helmet #7 livery)
models/players/alpine/rally07_glass
{
	{
		map $dynamicreflection   
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}

	{
		map models/players/alpine/rally07.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/alpine/rally07_hglass
{      

      {
	  	map models/players/alpine/rally07_hglass_t.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map $dynamicreflection   
		blendFunc add         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/rally07_hglass.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/rally07_hlite
{
  	{
	  	map models/players/alpine/rally07_hlite.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/alpine/hliteglow.tga
            blendFunc add
		rgbGen identity
	}    
}

models/players/alpine/rally07
{      
	{
		map $dynamicreflection   
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/rally07.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

// ALPINE RALLY27 (helmet #27 livery)
models/players/alpine/rally27_glass
{
	{
		map $dynamicreflection   
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}

	{
		map models/players/alpine/rally27.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/alpine/rally27_hglass
{      

      {
	  	map models/players/alpine/rally27_hglass_t.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map $dynamicreflection   
		blendFunc add         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/rally27_hglass.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/rally27_hlite
{
  	{
	  	map models/players/alpine/rally27_hlite.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/alpine/hliteglow.tga
            blendFunc add
		rgbGen identity
	}    
}

models/players/alpine/rally27
{      
	{
		map $dynamicreflection   
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/rally27.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

// ALPINE RALLY44 (helmet #44 livery)
models/players/alpine/rally44_glass
{
	{
		map $dynamicreflection   
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}

	{
		map models/players/alpine/rally44.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/alpine/rally44_hglass
{      

      {
	  	map models/players/alpine/rally44_hglass_t.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map $dynamicreflection   
		blendFunc add         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/rally44_hglass.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/rally44_hlite
{
  	{
	  	map models/players/alpine/rally44_hlite.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/alpine/hliteglow.tga
            blendFunc add
		rgbGen identity
	}    
}

models/players/alpine/rally44
{      
	{
		map $dynamicreflection   
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/rally44.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

// ALPINE RALLY88 (helmet #88 livery)
models/players/alpine/rally88_glass
{
	{
		map $dynamicreflection   
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}

	{
		map models/players/alpine/rally88.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/alpine/rally88_hglass
{      

      {
	  	map models/players/alpine/rally88_hglass_t.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map $dynamicreflection   
		blendFunc add         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/rally88_hglass.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/rally88_hlite
{
  	{
	  	map models/players/alpine/rally88_hlite.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/alpine/hliteglow.tga
            blendFunc add
		rgbGen identity
	}    
}

models/players/alpine/rally88
{      
	{
		map $dynamicreflection   
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/rally88.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

// ALPINE RALLY01 (helmet #1 livery)
models/players/alpine/rally01_glass
{
	{
		map $dynamicreflection   
		tcGen environment
		blendFunc	add
		rgbGen lightingdiffuse
	}

	{
		map models/players/alpine/rally01.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/alpine/rally01_hglass
{      

      {
	  	map models/players/alpine/rally01_hglass_t.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
	{
		map $dynamicreflection   
		blendFunc add         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/rally01_hglass.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

models/players/alpine/rally01_hlite
{
  	{
	  	map models/players/alpine/rally01_hlite.tga
		depthWrite
		rgbGen lightingdiffuse
	}
      {
		map models/players/alpine/hliteglow.tga
            blendFunc add
		rgbGen identity
	}    
}

models/players/alpine/rally01
{      
	{
		map $dynamicreflection   
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/alpine/rally01.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}


         
}

// CAMARO RALLY13 (helmet #13 livery)
models/players/camaro/rally13_lines1
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally13_lines1.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally13_body
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally13_body.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally13_lines2
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally13_lines2.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally13_mirror
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally13_mirror.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

// CAMARO RALLY07 (helmet #7 livery)
models/players/camaro/rally07_lines1
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally07_lines1.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally07_body
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally07_body.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally07_lines2
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally07_lines2.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally07_mirror
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally07_mirror.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

// CAMARO RALLY27 (helmet #27 livery)
models/players/camaro/rally27_lines1
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally27_lines1.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally27_body
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally27_body.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally27_lines2
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally27_lines2.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally27_mirror
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally27_mirror.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

// CAMARO RALLY44 (helmet #44 livery)
models/players/camaro/rally44_lines1
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally44_lines1.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally44_body
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally44_body.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally44_lines2
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally44_lines2.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally44_mirror
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally44_mirror.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

// CAMARO RALLY88 (helmet #88 livery)
models/players/camaro/rally88_lines1
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally88_lines1.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally88_body
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally88_body.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally88_lines2
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally88_lines2.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally88_mirror
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally88_mirror.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

// CAMARO RALLY01 (helmet #1 livery)
models/players/camaro/rally01_lines1
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally01_lines1.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally01_body
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally01_body.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally01_lines2
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally01_lines2.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/camaro/rally01_mirror
{
	{
		map $dynamicreflection
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map models/players/camaro/rally01_mirror.tga
		alphaGen const 0.8
		blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}
}

// GRAVELORD RALLY13 (helmet #13 livery)
models/players/gravelord/rally13
{
	{
		map $dynamicreflection
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/rally13.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

// GRAVELORD RALLY07 (helmet #7 livery)
models/players/gravelord/rally07
{
	{
		map $dynamicreflection
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/rally07.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

// GRAVELORD RALLY27 (helmet #27 livery)
models/players/gravelord/rally27
{
	{
		map $dynamicreflection
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/rally27.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

// GRAVELORD RALLY44 (helmet #44 livery)
models/players/gravelord/rally44
{
	{
		map $dynamicreflection
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/rally44.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

// GRAVELORD RALLY88 (helmet #88 livery)
models/players/gravelord/rally88
{
	{
		map $dynamicreflection
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/rally88.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

// GRAVELORD RALLY01 (helmet #1 livery)
models/players/gravelord/rally01
{
	{
		map $dynamicreflection
		rgbGen identity
		tcGen environment 
	}
	{
		map models/players/gravelord/rally01.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

// GREMLIN RALLY13 (helmet #13 livery)
models/players/gremlin/rally13
{      
	{

		map $dynamicreflection         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/gremlin/rally13.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// GREMLIN RALLY07 (helmet #7 livery)
models/players/gremlin/rally07
{      
	{

		map $dynamicreflection         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/gremlin/rally07.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// GREMLIN RALLY27 (helmet #27 livery)
models/players/gremlin/rally27
{      
	{

		map $dynamicreflection         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/gremlin/rally27.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// GREMLIN RALLY44 (helmet #44 livery)
models/players/gremlin/rally44
{      
	{

		map $dynamicreflection         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/gremlin/rally44.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// GREMLIN RALLY88 (helmet #88 livery)
models/players/gremlin/rally88
{      
	{

		map $dynamicreflection         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/gremlin/rally88.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// GREMLIN RALLY01 (helmet #1 livery)
models/players/gremlin/rally01
{      
	{

		map $dynamicreflection         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/gremlin/rally01.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// MACDADDY RALLY13 (helmet #13 livery)
models/players/macdaddy/rally13_main
{      
	{
		map $dynamicreflection              
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/rally13_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// MACDADDY RALLY07 (helmet #7 livery)
models/players/macdaddy/rally07_main
{      
	{
		map $dynamicreflection              
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/rally07_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// MACDADDY RALLY27 (helmet #27 livery)
models/players/macdaddy/rally27_main
{      
	{
		map $dynamicreflection              
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/rally27_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// MACDADDY RALLY44 (helmet #44 livery)
models/players/macdaddy/rally44_main
{      
	{
		map $dynamicreflection              
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/rally44_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// MACDADDY RALLY88 (helmet #88 livery)
models/players/macdaddy/rally88_main
{      
	{
		map $dynamicreflection              
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/rally88_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// MACDADDY RALLY01 (helmet #1 livery)
models/players/macdaddy/rally01_main
{      
	{
		map $dynamicreflection              
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/macdaddy/rally01_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// MINI RALLY13 (helmet #13 livery)
models/players/mini/rally13_glass
{
	{
		map models/players/mini/rally13_glass.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
	{
		map $dynamicreflection  
		tcGen environment
		blendFunc add
	}
	{
		map models/players/mini/rally13_glass_r.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/mini/rally13_main
{      
      {
	  	map models/players/mini/rally13_main.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
		map $dynamicreflection  
		blendFunc add       
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally13_main.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/rally13_roof
{      
      {
	  	map models/players/mini/rally13_roof.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
		map $dynamicreflection  
            tcGen environment
		blendFunc add
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally13_roof.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/rally13_hlites
{

      {
	  	map models/players/mini/rally13_hlites.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
  	{
		map $dynamicreflection     
		blendFunc add             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally13_hlites.tga
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

// MINI RALLY07 (helmet #7 livery)
models/players/mini/rally07_glass
{
	{
		map models/players/mini/rally07_glass.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
	{
		map $dynamicreflection  
		tcGen environment
		blendFunc add
	}
	{
		map models/players/mini/rally07_glass_r.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/mini/rally07_main
{      
      {
	  	map models/players/mini/rally07_main.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
		map $dynamicreflection  
		blendFunc add       
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally07_main.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/rally07_roof
{      
      {
	  	map models/players/mini/rally07_roof.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
		map $dynamicreflection  
            tcGen environment
		blendFunc add
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally07_roof.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/rally07_hlites
{

      {
	  	map models/players/mini/rally07_hlites.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
  	{
		map $dynamicreflection     
		blendFunc add             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally07_hlites.tga
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

// MINI RALLY27 (helmet #27 livery)
models/players/mini/rally27_glass
{
	{
		map models/players/mini/rally27_glass.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
	{
		map $dynamicreflection  
		tcGen environment
		blendFunc add
	}
	{
		map models/players/mini/rally27_glass_r.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/mini/rally27_main
{      
      {
	  	map models/players/mini/rally27_main.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
		map $dynamicreflection  
		blendFunc add       
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally27_main.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/rally27_roof
{      
      {
	  	map models/players/mini/rally27_roof.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
		map $dynamicreflection  
            tcGen environment
		blendFunc add
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally27_roof.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/rally27_hlites
{

      {
	  	map models/players/mini/rally27_hlites.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
  	{
		map $dynamicreflection     
		blendFunc add             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally27_hlites.tga
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

// MINI RALLY44 (helmet #44 livery)
models/players/mini/rally44_glass
{
	{
		map models/players/mini/rally44_glass.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
	{
		map $dynamicreflection  
		tcGen environment
		blendFunc add
	}
	{
		map models/players/mini/rally44_glass_r.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/mini/rally44_main
{      
      {
	  	map models/players/mini/rally44_main.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
		map $dynamicreflection  
		blendFunc add       
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally44_main.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/rally44_roof
{      
      {
	  	map models/players/mini/rally44_roof.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
		map $dynamicreflection  
            tcGen environment
		blendFunc add
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally44_roof.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/rally44_hlites
{

      {
	  	map models/players/mini/rally44_hlites.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
  	{
		map $dynamicreflection     
		blendFunc add             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally44_hlites.tga
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

// MINI RALLY88 (helmet #88 livery)
models/players/mini/rally88_glass
{
	{
		map models/players/mini/rally88_glass.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
	{
		map $dynamicreflection  
		tcGen environment
		blendFunc add
	}
	{
		map models/players/mini/rally88_glass_r.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/mini/rally88_main
{      
      {
	  	map models/players/mini/rally88_main.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
		map $dynamicreflection  
		blendFunc add       
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally88_main.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/rally88_roof
{      
      {
	  	map models/players/mini/rally88_roof.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
		map $dynamicreflection  
            tcGen environment
		blendFunc add
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally88_roof.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/rally88_hlites
{

      {
	  	map models/players/mini/rally88_hlites.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
  	{
		map $dynamicreflection     
		blendFunc add             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally88_hlites.tga
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

// MINI RALLY01 (helmet #1 livery)
models/players/mini/rally01_glass
{
	{
		map models/players/mini/rally01_glass.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
	{
		map $dynamicreflection  
		tcGen environment
		blendFunc add
	}
	{
		map models/players/mini/rally01_glass_r.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/players/mini/rally01_main
{      
      {
	  	map models/players/mini/rally01_main.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
		map $dynamicreflection  
		blendFunc add       
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally01_main.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/rally01_roof
{      
      {
	  	map models/players/mini/rally01_roof.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
	{
		map $dynamicreflection  
            tcGen environment
		blendFunc add
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally01_roof.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/mini/rally01_hlites
{

      {
	  	map models/players/mini/rally01_hlites.tga
		depthWrite
		rgbGen lightingdiffuse
	}       
  	{
		map $dynamicreflection     
		blendFunc add             
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/mini/rally01_hlites.tga
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

// MURCIELAGO RALLY13 (helmet #13 livery)
models/players/murcielago/rally13_main
{
	{
		map $dynamicreflection
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/rally13_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/murcielago/rally13_under
{
	{
		map $dynamicreflection
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/rally13_under.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

// MURCIELAGO RALLY07 (helmet #7 livery)
models/players/murcielago/rally07_main
{
	{
		map $dynamicreflection
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/rally07_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/murcielago/rally07_under
{
	{
		map $dynamicreflection
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/rally07_under.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

// MURCIELAGO RALLY27 (helmet #27 livery)
models/players/murcielago/rally27_main
{
	{
		map $dynamicreflection
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/rally27_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/murcielago/rally27_under
{
	{
		map $dynamicreflection
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/rally27_under.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

// MURCIELAGO RALLY44 (helmet #44 livery)
models/players/murcielago/rally44_main
{
	{
		map $dynamicreflection
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/rally44_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/murcielago/rally44_under
{
	{
		map $dynamicreflection
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/rally44_under.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

// MURCIELAGO RALLY88 (helmet #88 livery)
models/players/murcielago/rally88_main
{
	{
		map $dynamicreflection
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/rally88_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/murcielago/rally88_under
{
	{
		map $dynamicreflection
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/rally88_under.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

// MURCIELAGO RALLY01 (helmet #1 livery)
models/players/murcielago/rally01_main
{
	{
		map $dynamicreflection
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/rally01_main.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

models/players/murcielago/rally01_under
{
	{
		map $dynamicreflection
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{
		map models/players/murcielago/rally01_under.tga
		blendfunc blend
		rgbGen lightingDiffuse
		depthWrite
	}
}

// RAPTOR RALLY13 (helmet #13 livery)
models/players/raptor/rally13
{      
	{
	
		map $dynamicreflection
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/rally13.tga
       
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/raptor/rally13_parts
{      
	{
	
		map $dynamicreflection
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/rally13_parts.tga
       
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

// RAPTOR RALLY07 (helmet #7 livery)
models/players/raptor/rally07
{      
	{
	
		map $dynamicreflection
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/rally07.tga
       
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/raptor/rally07_parts
{      
	{
	
		map $dynamicreflection
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/rally07_parts.tga
       
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

// RAPTOR RALLY27 (helmet #27 livery)
models/players/raptor/rally27
{      
	{
	
		map $dynamicreflection
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/rally27.tga
       
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/raptor/rally27_parts
{      
	{
	
		map $dynamicreflection
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/rally27_parts.tga
       
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

// RAPTOR RALLY44 (helmet #44 livery)
models/players/raptor/rally44
{      
	{
	
		map $dynamicreflection
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/rally44.tga
       
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/raptor/rally44_parts
{      
	{
	
		map $dynamicreflection
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/rally44_parts.tga
       
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

// RAPTOR RALLY88 (helmet #88 livery)
models/players/raptor/rally88
{      
	{
	
		map $dynamicreflection
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/rally88.tga
       
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/raptor/rally88_parts
{      
	{
	
		map $dynamicreflection
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/rally88_parts.tga
       
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

// RAPTOR RALLY01 (helmet #1 livery)
models/players/raptor/rally01
{      
	{
	
		map $dynamicreflection
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/rally01.tga
       
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/raptor/rally01_parts
{      
	{
	
		map $dynamicreflection
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/raptor/rally01_parts.tga
       
            blendFunc blend
		depthWrite
		rgbGen lightingdiffuse
	}       
}

// REAPER RALLY13 (helmet #13 livery)
models/players/reaper/rally13_main
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally13_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally13_parts
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally13_parts.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally13_hlites
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally13_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally13_tlites
{
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
  	{
	  	map models/players/reaper/rally13_tlites.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/rally13_net
{

	{
		map models/players/reaper/rally13_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/rally13_bumper
{
	{
		map models/players/reaper/rally13_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}

// REAPER RALLY07 (helmet #7 livery)
models/players/reaper/rally07_main
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally07_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally07_parts
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally07_parts.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally07_hlites
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally07_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally07_tlites
{
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
  	{
	  	map models/players/reaper/rally07_tlites.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/rally07_net
{

	{
		map models/players/reaper/rally07_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/rally07_bumper
{
	{
		map models/players/reaper/rally07_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}

// REAPER RALLY27 (helmet #27 livery)
models/players/reaper/rally27_main
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally27_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally27_parts
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally27_parts.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally27_hlites
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally27_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally27_tlites
{
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
  	{
	  	map models/players/reaper/rally27_tlites.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/rally27_net
{

	{
		map models/players/reaper/rally27_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/rally27_bumper
{
	{
		map models/players/reaper/rally27_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}

// REAPER RALLY44 (helmet #44 livery)
models/players/reaper/rally44_main
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally44_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally44_parts
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally44_parts.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally44_hlites
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally44_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally44_tlites
{
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
  	{
	  	map models/players/reaper/rally44_tlites.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/rally44_net
{

	{
		map models/players/reaper/rally44_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/rally44_bumper
{
	{
		map models/players/reaper/rally44_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}

// REAPER RALLY88 (helmet #88 livery)
models/players/reaper/rally88_main
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally88_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally88_parts
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally88_parts.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally88_hlites
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally88_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally88_tlites
{
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
  	{
	  	map models/players/reaper/rally88_tlites.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/rally88_net
{

	{
		map models/players/reaper/rally88_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/rally88_bumper
{
	{
		map models/players/reaper/rally88_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}

// REAPER RALLY01 (helmet #1 livery)
models/players/reaper/rally01_main
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally01_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally01_parts
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally01_parts.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally01_hlites
{      
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/reaper/rally01_main.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}       
}

models/players/reaper/rally01_tlites
{
	{
		map textures/reflect/chrometest2.jpg
            tcGen environment
            rgbGen lightingdiffuse
	}
  	{
	  	map models/players/reaper/rally01_tlites.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/rally01_net
{

	{
		map models/players/reaper/rally01_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}

models/players/reaper/rally01_bumper
{
	{
		map models/players/reaper/rally01_parts.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaFunc GT0
		depthWrite
		rgbGen lightingdiffuse
	}
}

// ROADSTER RALLY13 (helmet #13 livery)
models/players/roadster/rally13_main
{

	{
		map $dynamicreflection
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/roadster/rally13_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

// ROADSTER RALLY07 (helmet #7 livery)
models/players/roadster/rally07_main
{

	{
		map $dynamicreflection
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/roadster/rally07_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

// ROADSTER RALLY27 (helmet #27 livery)
models/players/roadster/rally27_main
{

	{
		map $dynamicreflection
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/roadster/rally27_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

// ROADSTER RALLY44 (helmet #44 livery)
models/players/roadster/rally44_main
{

	{
		map $dynamicreflection
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/roadster/rally44_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

// ROADSTER RALLY88 (helmet #88 livery)
models/players/roadster/rally88_main
{

	{
		map $dynamicreflection
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/roadster/rally88_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

// ROADSTER RALLY01 (helmet #1 livery)
models/players/roadster/rally01_main
{

	{
		map $dynamicreflection
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/roadster/rally01_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

// SIDEPIPE RALLY07 (helmet #7 livery)
models/players/sidepipe/rally07
{      
	{

		map $dynamicreflection         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/sidepipe/rally07.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// SIDEPIPE RALLY27 (helmet #27 livery)
models/players/sidepipe/rally27
{      
	{

		map $dynamicreflection         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/sidepipe/rally27.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// SIDEPIPE RALLY44 (helmet #44 livery)
models/players/sidepipe/rally44
{      
	{

		map $dynamicreflection         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/sidepipe/rally44.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// SIDEPIPE RALLY88 (helmet #88 livery)
models/players/sidepipe/rally88
{      
	{

		map $dynamicreflection         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/sidepipe/rally88.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// SIDEPIPE RALLY01 (helmet #1 livery)
models/players/sidepipe/rally01
{      
	{

		map $dynamicreflection         
            tcGen environment
            rgbGen lightingdiffuse
	}
      {
	  	map models/players/sidepipe/rally01.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
		rgbGen lightingdiffuse
	}
         
}

// SLINGSHOT RALLY13 (helmet #13 livery)
models/players/slingshot/rally13_main
{

	{
		map $dynamicreflection
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/slingshot/rally13_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

// SLINGSHOT RALLY07 (helmet #7 livery)
models/players/slingshot/rally07_main
{

	{
		map $dynamicreflection
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/slingshot/rally07_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

// SLINGSHOT RALLY27 (helmet #27 livery)
models/players/slingshot/rally27_main
{

	{
		map $dynamicreflection
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/slingshot/rally27_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

// SLINGSHOT RALLY44 (helmet #44 livery)
models/players/slingshot/rally44_main
{

	{
		map $dynamicreflection
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/slingshot/rally44_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

// SLINGSHOT RALLY88 (helmet #88 livery)
models/players/slingshot/rally88_main
{

	{
		map $dynamicreflection
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/slingshot/rally88_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

// SLINGSHOT RALLY01 (helmet #1 livery)
models/players/slingshot/rally01_main
{

	{
		map $dynamicreflection
		tcGen environment
          rgbGen lightingdiffuse
	}
{
		map models/players/slingshot/rally01_main.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            rgbGen lightingdiffuse

	}
}

// VIPER RALLY13 (helmet #13 livery)
models/players/viper/rally13
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally13.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/rally13_f_lites
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally13_f_lites.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/rally13_r_lites
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally13_r_lites.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

// VIPER RALLY07 (helmet #7 livery)
models/players/viper/rally07
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally07.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/rally07_f_lites
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally07_f_lites.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/rally07_r_lites
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally07_r_lites.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

// VIPER RALLY27 (helmet #27 livery)
models/players/viper/rally27
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally27.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/rally27_f_lites
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally27_f_lites.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/rally27_r_lites
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally27_r_lites.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

// VIPER RALLY44 (helmet #44 livery)
models/players/viper/rally44
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally44.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/rally44_f_lites
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally44_f_lites.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/rally44_r_lites
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally44_r_lites.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

// VIPER RALLY88 (helmet #88 livery)
models/players/viper/rally88
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally88.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/rally88_f_lites
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally88_f_lites.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/rally88_r_lites
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally88_r_lites.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

// VIPER RALLY01 (helmet #1 livery)
models/players/viper/rally01
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally01.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/rally01_f_lites
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally01_f_lites.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}

models/players/viper/rally01_r_lites
{
	{

		map $dynamicreflection          
		tcGen environment
		rgbGen lightingdiffuse
	}

	{
		map models/players/viper/rally01_r_lites.tga
		blendFunc	GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA

		depthWrite
		rgbGen lightingdiffuse
	}


}
