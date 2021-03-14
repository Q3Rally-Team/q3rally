models/players/heads/quake3/crash_f
{

        {
                map textures/sfx/snow.tga
                tcmod scale .5 .5
                tcmod scroll  9 0.3
                rgbGen identity
        }
        {
                map textures/effects/tinfx2b.tga
                tcGen environment
                blendFunc add
                rgbGen lightingdiffuse
        }
}

models/players/heads/quake3/doom_f
{    
 
         {
		map textures/effects/tinfx.tga
                tcGen environment
                blendFunc GL_ONE GL_ZERO              
                rgbGen lightingDiffuse	
	}
        {
		
		map models/players/heads/quake3/doom_f.tga
		tcGen environment
		rgbGen lightingDiffuse
		blendfunc gl_ONE gl_ONE 		
	}
	
}

models/players/heads/quake3/razor_gogs
{
	{
		map models/players/heads/quake3/razor_gogs.tga	
                tcGen environment

	}

	
	{
		map	models/players/heads/quake3/razor_gogs_fx.tga	
		blendfunc GL_ONE GL_ONE
		tcMod scroll 1.2 9.3
		
	}
}

models/players/heads/quake3/uriel_h
{      
        {
		map models/players/heads/quake3/uriel_h.tga
                blendFunc GL_ONE GL_ZERO
		rgbGen lightingDiffuse
	} 
        {
                map textures/sfx/proto_zzzt.tga
                blendFunc GL_ONE GL_ONE
                tcmod scroll -0.2 1
                rgbGen identity
	}  
        {
		map models/players/heads/quake3/uriel_h.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingDiffuse
	}
       
         
}

models/players/heads/quake3/xaero_h
{      
        {
		map models/players/heads/quake3/xaero_h.tga
                blendFunc GL_ONE GL_ZERO
		rgbGen lightingDiffuse
	} 
        {
                map textures/sfx/firewalla.tga
                blendFunc GL_ONE GL_ONE
                tcmod scroll 0.1 1
                //rgbGen wave triangle .5 1 0 .4
	}  
        {
		map models/players/heads/quake3/xaero_h.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingDiffuse
	}
       
}

models/players/heads/quake3/hunter_f
{
     deformVertexes wave 100 sin 0 .3 0 .2
     cull disable
        {
                map models/players/heads/quake3/hunter_f.tga
                //blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
                //alphaFunc GT0
                alphaFunc GE128
		depthWrite
		rgbGen lightingDiffuse
        }


}

models/players/heads/quake3/mynx_shiny
{      
 
        {
                map textures/sfx/specular5.tga            
                blendFunc GL_ONE GL_ZERO
                tcGen environment
	}  
        {
		map models/players/heads/quake3/mynx_shiny.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingDiffuse
	}
                
}

models/players/heads/quake3/anarki_g

{
        {
                map models/players/heads/quake3/anarki_g.tga
                //rgbGen vertex
        }
        {
		
		map models/players/heads/quake3/anarki_g_fx.tga
		rgbGen lightingDiffuse
                rgbGen wave triangle 0 1 0 .5
		blendfunc gl_ONE gl_ONE 		
	}
}

models/players/heads/quake3/klesk_h
{
	{
		map models/players/heads/quake3/klesk_h.tga
                blendfunc GL_ONE GL_ZERO
                rgbGen lightingDiffuse
		
        }
	
	{
		map models/players/heads/quake3/klesk_g.tga
				blendfunc GL_ONE GL_ONE
				rgbGen wave triangle .5 1 0 .5
		
	}
}

models/players/heads/quake3/bones2_h
{
        {
                map models/players/heads/quake3/bones2_h.tga
                blendfunc add
                //rgbGen wave sin 1 .8 0 9
        }
        {
		
                map models/players/heads/quake3/bones2_fx.jpg
                tcMod scroll -6 -.2
                tcgen environment
		blendFunc GL_ONE GL_ONE
		rgbGen identity
	}    


}

models/players/heads/quake3/bones_h
{
    
    cull disable
        {
                map models/players/heads/quake3/bones_h.tga
                alphaFunc GE128
		depthWrite
		rgbGen lightingdiffuse
        }
}

models/players/heads/quake3/phobos_f
{    
 
         {
		map textures/effects/tinfx.tga
                tcGen environment
                blendFunc GL_ONE GL_ZERO              
                rgbGen lightingDiffuse
	} 
        {
		map models/players/heads/quake3/phobos_f.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingDiffuse
	}
}

models/players/heads/quake3/stripe_h
{      
 
       {
	        //map textures/effects/tinfx.tga
                map textures/sfx/electricgrade3.tga
		//tcmod scale 6 6
                //tcmod rotate 350
                tcmod scroll 0 -.5
		//tcGen environment
		blendfunc GL_ONE GL_ZERO
                rgbGen identity
        }
        {
		map models/players/heads/quake3/stripe_h.tga
                blendfunc gl_src_alpha gl_one_minus_src_alpha
		rgbGen lightingDiffuse
	}
       
}

models/players/heads/quake3/harpy_h
{
        {
                map textures/effects/envmaproc.tga
                tcGen environment
                rgbGen lightingdiffuse
        }
        {
                map models/players/heads/quake3/hunter_h.tga
                blendFunc blend
                rgbGen lightingdiffuse
        }
}

models/players/heads/quake3/flisk_h
{      
        {
		map models/players/heads/quake3/flisk_h.tga
                blendFunc GL_ONE GL_ZERO
		rgbGen lightingDiffuse
	} 
        {
                map textures/sfx/bolts.tga
                blendFunc GL_ONE GL_ONE
                tcmod scale 2 2
                tcmod scroll 2 2
	}   
        {
		map models/players/heads/quake3/flisk_h.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingDiffuse
	}
       
         
}

//models/players/heads/quake3/wrack
//{      
//        {
//		map $whiteimage
//		rgbGen lightingDiffuse
//	}
//	{
//        	map models/players/heads/quake3/wrack.tga
//		blendFunc GL_DST_COLOR GL_SRC_ALPHA
//		rgbGen identity
//		alphaGen lightingSpecular
//	}
//          
//}
models/players/heads/quake3/wrack
{
      
        {
		map $whiteimage
		rgbGen lightingDiffuse
	}
	{
        	map models/players/heads/quake3/wrack.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
          
}

models/players/heads/quake3/patriot_gogs
{
        {
		map textures/effects/tinfx2d.tga
                tcGen environment
                blendFunc GL_ONE GL_ZERO              
                rgbGen lightingDiffuse
	}   

          
}

models/players/heads/quake3/id_h
{
        {
                map textures/effects/tinfx2d.tga
                tcGen environment
                rgbGen lightingdiffuse
        }
        {
                map models/players/heads/quake3/id_h.tga
                blendFunc blend
                rgbGen lightingdiffuse
        }
}

models/players/heads/quake3/krusade_h
{       
        {
                map textures/sfx/firegorre.tga
                blendFunc GL_ONE GL_ZERO
                tcmod scroll 0 1
                tcMod turb 0 .25 0 1.6
                tcmod scale 4 4
                rgbGen identity
	}  
        {
		map models/players/heads/quake3/krusade_h.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingDiffuse
	}
       
         
}

models/players/heads/quake3/roderic_h
{       
        {
                map textures/sfx/firegorre.tga
                blendFunc GL_ONE GL_ZERO
                tcmod scroll 0 1
                tcMod turb 0 .25 0 1.6
                tcmod scale 4 4
                rgbGen identity
	}  
        {
		map models/players/heads/quake3/roderic_h.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingDiffuse
	}
       
         
}

models/players/heads/quake3/null
{       
    {
	map models/players/heads/quake3/null.tga
        blendFunc GL_ONE GL_ONE
    }
         
}

models/players/heads/quake3/grrl_h
{

        {
                map textures/effects/tinfx2d.tga
                tcGen environment
                rgbGen identity
        }
        {
                map models/players/heads/quake3/grrl_h.tga
                blendfunc blend
                rgbGen lightingdiffuse
        }
        
}

models/players/heads/quake3/yuriko_h
{
        {
                map textures/effects/envmaproc.jpg
                tcGen environment
                rgbGen lightingdiffuse
        }
        {
                map models/players/heads/quake3/yuriko_h.tga
                blendFunc blend
                rgbGen lightingdiffuse
        }
}

models/players/heads/quake3/gorre
{   
 
        {
                map textures/sfx/firegorre.tga
                blendFunc GL_ONE GL_ZERO
                tcmod scroll 0 1
                tcMod turb 0 .25 0 1.6
                tcmod scale 4 4
                rgbGen identity
	}  
        //{
        //        map textures/sfx/firewallb.tga
        //        blendFunc GL_ONE GL_ONE
        //        tcmod scale 3 3
        //        tcMod turb 0 .2 0 .1
        //        tcMod scroll 0 3
        //        //tcmod rotate 360
        //        rgbGen identity
        //}            
        {
		map models/players/heads/quake3/gorre.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingDiffuse
	}
}

models/players/heads/quake3/zael
{
      
        {
		map $whiteimage
		rgbGen lightingDiffuse
	}
	{
        	map models/players/heads/quake3/zael.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
          
}