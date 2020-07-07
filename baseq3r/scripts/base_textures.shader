// ------------------------------------------------------------
// Shaders for base textures - rewritten by P3rlE
// based on base-xxx.shader from id software
// ------------------------------------------------------------

// ------------------------------------------------------------
// 1.base_light
// ------------------------------------------------------------

textures/base_light/btactmach0
{
	q3map_surfacelight 1000
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/btactmach0.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/btactmach0.blend.tga
		rgbGen wave sin 0.5 0.5 0 .2
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 2.base_button
// ------------------------------------------------------------

textures/base_button/shootme2
{
	qer_editorimage textures/base_button/shootme1.tga
	q3map_lightimage textures/base_button/shootme_glow.tga
	q3map_surfacelight 1000
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_support/metal3_3.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_button/shootme_glow.tga
		rgbGen wave sin 0.5 1.0 0 .3
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 3.base_door (shinymetaldoor)
// ------------------------------------------------------------

textures/base_door/shinymetaldoor
{

	{
		//map $lightmap
		map textures/base_wall/chrome_env.tga
                tcGen environment
		tcmod scale .25 .25 
		rgbGen identity   
	}

		
	{
		map textures/base_door/shinymetaldoor.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}

	{
		map textures/base_door/quake3sign.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}


	{
		map $lightmap
		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}	
		
} 

// ------------------------------------------------------------
// 4.base_door (shinymetaldoor_outside)
// ------------------------------------------------------------

textures/base_door/shinymetaldoor_outside
{

	{
		map textures/base_wall/chrome_env.tga
                tcGen environment
		tcmod scale .25 .25 
		rgbGen identity   
	}

		
	{
		map textures/base_door/shinymetaldoor_outside.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}

	{
		map textures/base_door/quake3sign_outside.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}


	{
		map $lightmap
		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}	
		
} 

// ------------------------------------------------------------
// 5.base_door (shinymetaldoor_outside3a2)
// ------------------------------------------------------------

textures/base_door/shinymetaldoor_outside3a2
{

	{
		map textures/base_wall/chrome_env.tga
                tcGen environment
		tcmod scale .25 .25 
		rgbGen identity   
	}

		
	{
		map textures/base_door/shinymetaldoor_outside3a2.tga
		blendFunc blend
		rgbGen identity
	}


	{
		map $lightmap
		rgbgen identity
		blendFunc filter
	}

	{
		map textures/base_door/shinymetaldoor_outside3glow.tga
		blendFunc add
		rgbGen wave sin .9 .1 0 5
	}
		
}

// ------------------------------------------------------------
// 6.base_floor (techfloor2)
// ------------------------------------------------------------

textures/base_floor/techfloor2
{ 
        cull disable
        //surfaceparm trans	
        surfaceparm nomarks	
        //nomipmaps

        {
	        clampmap textures/base_floor/techfloor2.tga
		alphaFunc GE128
                tcmod rotate 70
                tcMod stretch sin .8 0.2 0 .3
	        rgbGen identity
	}
        {
                clampmap textures/base_floor/techfloor2.tga
                alphaFunc GE128
                tcmod rotate -50
                tcMod stretch sin .8 0.2 0 .2
                rgbGen identity
	}
        {
	        clampmap textures/base_floor/techfloor2.tga
		alphaFunc GE128
                tcmod rotate 30
                tcMod stretch sin .8 0.2 0 .1
	        rgbGen identity
	}
        {
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}

}

// ------------------------------------------------------------
// 7.base_floor (techfloor)
// ------------------------------------------------------------

textures/base_floor/techfloor
{    
     surfaceparm	metalsteps	   
        
        {
		map textures/sfx/proto_zzztpink.tga
                tcMod turb 0 .5 0 9.6
                tcmod scale 2 2
                tcmod scroll 9 5
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
        {
	        //clampmap textures/gothic_block/blocks18cgeomtrn2.tga
                clampmap textures/base_floor/techfloor2.tga
		blendFunc blend
                tcmod rotate 170
                tcMod stretch sin .8 0.2 0 .4
	        rgbGen identity
	}
        {
		map textures/base_floor/techfloor.tga
                blendFunc blend
		rgbGen identity
	}
        
        {
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
                depthFunc equal
       }
}

// ------------------------------------------------------------
// 8.base_floor (metalbridge04dbroke)
// ------------------------------------------------------------

textures/base_floor/metalbridge04dbroke
{    
     surfaceparm	metalsteps	   
        
        {
		map textures/sfx/proto_zzztblu2.tga
                tcMod turb 0 .5 0 9.6
                tcmod scale 2 2
                tcmod scroll 9 5
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
        {
		map textures/base_floor/metalbridge04dbroke.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 9.base_floor (clang_floor_ow3)
// ------------------------------------------------------------

textures/base_floor/clang_floor_ow3
{    
     surfaceparm	metalsteps	   
        
        {
		map textures/sfx/proto_zzztblu2.tga
                tcMod turb 0 .5 0 9.6
                tcmod scale 2 2
                tcmod scroll 9 5
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
        {
		map textures/base_floor/clang_floor_ow3.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 10.base_floor (floor3_3dark_ow)
// ------------------------------------------------------------


textures/base_floor/floor3_3dark_ow
{

        {
		map textures/effects/envmapblue.tga
                tcmod scroll 0 1
                tcMod turb 0 .25 0 5.6
                tcmod scale 1.5 1.5
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
       
	{
	        map textures/base_floor/floor3_3dark_ow.tga
		blendFunc blend
	        rgbGen identity
	}
        {
		map $lightmap
                blendFunc filter
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 11.base_floor (proto_rustygrate2)
// ------------------------------------------------------------


textures/base_floor/proto_rustygrate2
{
	surfaceparm	metalsteps	
        surfaceparm trans	
	surfaceparm alphashadow
	cull none
        nopicmip
	// A RUSTY GRATE OR GRILL THAT CAN BE SEEN FROM BOTH SIDES
	{
		map textures/base_floor/proto_rustygrate2.tga
		// tcMod scale 2 2
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 12.base_floor (cybergrate3)
// ------------------------------------------------------------


textures/base_floor/cybergrate3
{
    cull disable
    surfaceparm alphashadow
    surfaceparm	metalsteps	
    surfaceparm nomarks
        {
                map textures/sfx/hologirl.tga
                blendFunc add
                tcmod scale  1.2 .5
                tcmod scroll 3.1 1.1
		
        }
        {
                map textures/base_floor/cybergrate3.tga
                alphaFunc GE128
		depthWrite
        }
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
		depthFunc equal
	}


}

// ------------------------------------------------------------
// 13.base_floor (cybergrate2)
// ------------------------------------------------------------

textures/base_floor/cybergrate2
{
    cull disable
    surfaceparm alphashadow
    surfaceparm	metalsteps	
    surfaceparm nomarks
        {
                map textures/base_floor/cybergrate2.tga
                alphaFunc GE128
		depthWrite
		rgbGen vertex
        }
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
		depthFunc equal
	}


}

// ------------------------------------------------------------
// 14.base_floor (clang_floor_ow)
// ------------------------------------------------------------

textures/base_floor/clang_floor_ow
{    
     surfaceparm	metalsteps	   
        
        {
		map textures/sfx/proto_zzztblu2.tga
                tcMod turb 0 .5 0 9.6
                tcmod scale 2 2
                tcmod scroll 9 5
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
        {
		map textures/base_floor/clang_floor_ow.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 15.base_floor (clangdark_ow3)
// ------------------------------------------------------------

textures/base_floor/clangdark_ow3
{    
     surfaceparm	metalsteps	   
        
        {
		map textures/sfx/proto_zzztblu2.tga
                tcMod turb 0 .5 0 9.6
                tcmod scale 2 2
                tcmod scroll 9 5
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
        {
		map textures/base_floor/clangdark_ow3.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 16.base_floor (cybergrate)
// ------------------------------------------------------------

textures/base_floor/cybergrate
{
    cull disable
    surfaceparm alphashadow
    surfaceparm	metalsteps	
    surfaceparm nomarks
        {
                map textures/base_floor/cybergrate.tga
                alphaFunc GE128
		depthWrite
		rgbGen vertex
        }
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
		depthFunc equal
	}


}

// ------------------------------------------------------------
// 16.base_floor (diamond2c_ow)
// ------------------------------------------------------------

textures/base_floor/diamond2c_ow
{    
     surfaceparm	metalsteps	   
        
        {
		map textures/sfx/proto_zzztblu2.tga
                tcMod turb 0 .5 0 9.6
                tcmod scale 2 2
                tcmod scroll 9 5
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
        {
		map textures/base_floor/diamond2c_ow.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 17.base_floor (diamond2cspot)
// ------------------------------------------------------------

textures/base_floor/diamond2cspot
{    
     surfaceparm	metalsteps	   
        
        {
		map textures/liquids/proto_grueldark2.tga
                blendFunc GL_ONE GL_ZERO
                tcmod scale 2 2
                tcMod scroll .01 .03
                tcMod turb 0 0.05 0 .05
	}
        
        {
		map textures/effects/tinfx3.tga
                blendFunc GL_ONE GL_ONE
                tcGen environment
        }
       
        {
		map textures/base_floor/diamond2cspot.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 18.base_floor (clang_floor3blava)
// ------------------------------------------------------------

textures/base_floor/clang_floor3blava
{
	surfaceparm nolightmap
        {
		map textures/liquids/protolava2.tga
                tcmod scale .2 .2
                tcmod scroll .04 .03
                tcMod turb 0 .1 0 .01
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
	{
		map textures/base_floor/clang_floor3blava.tga
                blendfunc blend
		
	}
      
}

// ------------------------------------------------------------
// 19.base_floor (clang_floor3blava)
// ------------------------------------------------------------

textures/base_floor/pool_side2
{

	
       	{
		map textures/liquids/pool3d_5e.tga
		tcmod scale -.5 -.25
		tcmod scroll .025 .025
		rgbgen wave sin .75 0 0 0
	}
       
	{
		map textures/liquids/pool3d_6e.tga
		blendFunc GL_one GL_ONE
		tcmod scale .5 .25
		tcmod scroll .025 .025
		rgbgen wave sin .75 0 0 0
	}

	{
		map textures/base_floor/pool_side2.tga
		blendFunc GL_one GL_src_color
		tcmod scale .5 .5
		rgbgen identity
	}


	{
		rgbGen identity
		map $lightmap
		blendfunc gl_dst_color gl_zero
	}
       
              
}

// ------------------------------------------------------------
// 20.base_floor (pool_side3)
// ------------------------------------------------------------

textures/base_floor/pool_side3
{
	qer_editorimage textures/base_wall/patch10rusty5.tga
	
       	{
		map textures/liquids/pool3d_5e.tga
		tcmod scale -.5 -.25
		tcmod scroll .025 .025
		rgbgen wave sin .75 0 0 0
	}
       
	{
		map textures/liquids/pool3d_6e.tga
		blendFunc GL_one GL_ONE
		tcmod scale .5 .25
		tcmod scroll .025 .025
		rgbgen wave sin .75 0 0 0
	}

	{
		map textures/base_wall/patch10rusty5.tga
		blendFunc GL_one GL_src_color
		tcmod scale .5 .5
		rgbgen identity
	}


	{
		rgbGen identity
		map $lightmap
		blendfunc gl_dst_color gl_zero
	}
       
              
}

// ------------------------------------------------------------
// 21.base_floor (pool_floor2)
// ------------------------------------------------------------

textures/base_floor/pool_floor2
{
	
	
        	{
		map textures/liquids/pool3d_5e.tga
		tcmod scale -.25 -.25
		tcmod scroll .025 .025
		rgbgen wave sin .75 0 0 0
	
	}
       
	{
		map textures/liquids/pool3d_6e.tga
		blendFunc GL_one GL_ONE
		tcmod scale .25 .25
		tcmod scroll .025 .025
		rgbgen wave sin .75 0 0 0
	}

	{
		map textures/base_floor/pool_floor2.tga
		blendFunc GL_one GL_src_color
		rgbgen identity
	}


	{
		rgbGen identity
		map $lightmap
		blendfunc gl_dst_color gl_zero
	}
       
              
}

// ------------------------------------------------------------
// 22.base_floor (pool_floor3)
// ------------------------------------------------------------

textures/base_floor/pool_floor3
{
	qer_editorimage textures/base_wall/patch10rusty5.tga
	
        	{
		map textures/liquids/pool3d_5e.tga
		tcmod scale -.25 -.25
		tcmod scroll .025 .025
		rgbgen wave sin .75 0 0 0
	
	}
       
	{
		map textures/liquids/pool3d_6e.tga
		blendFunc GL_one GL_ONE
		tcmod scale .25 .25
		tcmod scroll .025 .025
		rgbgen wave sin .75 0 0 0
	}

	{
		map textures/base_wall/patch10rusty5.tga
		blendFunc GL_one GL_src_color
		rgbgen identity
	}


	{
		rgbGen identity
		map $lightmap
		blendfunc gl_dst_color gl_zero
	}
       
              
}

// ------------------------------------------------------------
// 23.base_floor (hfloor3)
// ------------------------------------------------------------

textures/base_floor/hfloor3
{
	//deformVertexes wave 100 sin 3 2 .1 3
        {
		rgbGen identity
		map $lightmap
	}
	{
		map textures/base_floor/hfloor3.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
        {
		map textures/effects/tinfx.tga
                tcgen environment
		blendFunc GL_ONE GL_ONE
		rgbGen identity
	}
       
        {
		map textures/base_floor/hfloor3.tga
		//blendFunc GL_ONE GL_ONE
                blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
       
              
}

// ------------------------------------------------------------
// 24.base_floor (clangspot)
// ------------------------------------------------------------

textures/base_floor/clangspot
{    
     surfaceparm	metalsteps	
     qer_editorimage textures/base_floor/clangdarkspot.tga 
        //{
	//	map textures/base_floor/clangspot.tga
	//	rgbGen identity
	//}
        
        {
		map textures/liquids/proto_grueldark.tga
                //tcGen environment
                blendFunc GL_ONE GL_ZERO
                tcmod scale 2 2
                tcMod scroll .01 .03
                
		tcMod turb 0 0.05 0 .05
                //rgbGen identity
	}
        
        {
		map textures/effects/tinfx3.tga
                //tcGen environment
                blendFunc GL_ONE GL_ONE
                tcGen environment
		//tcMod turb 0 0.01 0 9
                //rgbGen identity
	}
       
        {
		map textures/base_floor/clangspot.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 25.base_floor (clangspot2)
// ------------------------------------------------------------

textures/base_floor/clangspot2
{    
     surfaceparm	metalsteps	
        
        {
		map textures/liquids/proto_grueldark.tga
                //tcGen environment
                blendFunc GL_ONE GL_ZERO
                tcmod scale 2 2
                tcMod scroll .01 .03
                
		tcMod turb 0 0.05 0 .05
                //rgbGen identity
	}
        
        {
		map textures/effects/tinfx3.tga
                //tcGen environment
                blendFunc GL_ONE GL_ONE
                tcGen environment
		//tcMod turb 0 0.01 0 9
                //rgbGen identity
	}
       
        {
		map textures/base_floor/clangspot2.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 26.base_floor (clangdarkspot)
// ------------------------------------------------------------

textures/base_floor/clangdarkspot
{    
     surfaceparm	metalsteps	   
        //{
	//	map textures/base_floor/clangdarkspot.tga
	//	rgbGen identity
	//}
        
        {
		map textures/liquids/proto_grueldark.tga
                //tcGen environment
                blendFunc GL_ONE GL_ZERO
                tcmod scale 2 2
                tcMod scroll .01 .03
                
		tcMod turb 0 0.05 0 .05
                //rgbGen identity
	}
        
        {
		map textures/effects/tinfx3.tga
                //tcGen environment
                blendFunc GL_ONE GL_ONE
                tcGen environment
		//tcMod turb 0 0.01 0 9
                //rgbGen identity
	}
       
        {
		map textures/base_floor/clangdarkspot.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 27.base_floor (tilefloor5)
// ------------------------------------------------------------

textures/base_floor/tilefloor5
{
	{
		rgbGen identity
		map $lightmap
	}
	{
		map textures/base_floor/tilefloor5.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
}

// ------------------------------------------------------------
// 28.base_floor (tile5_plain)
// ------------------------------------------------------------


textures/base_floor/tile5_plain
{
	qer_editorimage textures/base_floor/tilefloor5.tga
	{
		rgbGen identity
		map $lightmap
	}
	{
		map textures/base_floor/tilefloor5.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 29.base_floor (concretefloor1)
// ------------------------------------------------------------

textures/base_floor/concretefloor1
{

	{
		map textures/base_wall/chrome_env.tga
		tcgen environment
		rgbgen wave sin .25 0 0 0
	}


	{
		map textures/base_floor/concretefloor1.tga
		blendFunc GL_ZERO GL_SRC_ALPHA
		tcmod scale .1 .1
		rgbgen identity	
	}

	{
		map textures/base_floor/concretefloor1.tga
		blendFunc GL_ONE GL_SRC_ALPHA
		tcmod turb sin .5 1 0 1
		rgbgen identity	
	}


	{
		map $lightmap
        	blendfunc gl_dst_color gl_zero
		rgbgen identity 
	}




}

// ------------------------------------------------------------
// 30.base_floor (pjgrate1kc)
// ------------------------------------------------------------

textures/base_floor/pjgrate1kc
{
	qer_editorimage textures/base_floor/proto_grate5.tga
	surfaceparm	metalsteps		
	cull none

	// A GRATE OR GRILL THAT CAN BE SEEN FROM BOTH SIDES
	
	
	{
		map textures/base_floor/proto_grate5.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}


	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 31.base_floor (clangfloor_noisy)
// ------------------------------------------------------------

textures/base_floor/clangfloor_noisy
{
	qer_editorimage textures/base_floor/clang_floor.tga
	surfaceparm metalsteps
	{
		map $lightmap
		rgbGen identity
	}

	{
		map textures/base_floor/clang_floor.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbgen identity

	}

}

// ------------------------------------------------------------
// 32.base_floor (clang_floorshiny2)
// ------------------------------------------------------------


textures/base_floor/clang_floorshiny2
{
	qer_editorimage textures/base_floor/clang_floor.tga

	{
		map $lightmap
               	tcGen environment
		tcmod scale .25 .25 
		rgbgen identity 
	}


	{
		map textures/base_floor/clang_floorshiny_alpha2.tga
		blendFunc GL_ONE GL_SRC_ALPHA
		alphagen wave triangle .98 .02 0 10
		rgbGen identity
	}
	{
		map $lightmap
		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}

}

// ------------------------------------------------------------
// 33.base_floor (clang_floorwet)
// ------------------------------------------------------------

textures/base_floor/clang_floorwet
{
	qer_editorimage textures/base_floor/clang_floor.tga

	{
		map textures/effects/tinfx3.tga
               	tcGen environment
		//tcmod scale .25 .25 
		rgbgen identity 
	}


	{
		map textures/base_floor/clang_floorwet.tga
		blendFunc GL_ONE GL_SRC_ALPHA
		//alphagen wave triangle .98 .02 0 10
		rgbGen identity
	}
	{
		map $lightmap
		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}

}

// ------------------------------------------------------------
// 34.base_floor (clang_floorshiny)
// ------------------------------------------------------------

textures/base_floor/clang_floorshiny
{
	qer_editorimage textures/base_floor/clang_floor.tga

	{
		map $lightmap
               	tcGen environment
		tcmod scale .25 .25  
	}


	{
		map textures/base_floor/clang_floorshiny_alpha.tga
		blendFunc GL_ONE GL_SRC_ALPHA
		rgbGen identity
	}
	{
		map $lightmap
		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}

}

// ------------------------------------------------------------
// 35.base_floor (tilefloor7_ow)
// ------------------------------------------------------------

textures/base_floor/tilefloor7_ow
{
	q3map_lightimage textures/base_floor/tilefloor7_owfx.tga
	q3map_surfacelight 100
	
        {
		map $lightmap
		rgbGen identity
	}
        {
		map textures/base_floor/tilefloor7_ow.tga
		blendfunc gl_dst_color gl_zero
                rgbGen identity
	}	
        {
		map textures/base_floor/tilefloor7_owfx.tga
		blendfunc GL_ONE GL_ONE
                rgbgen wave triangle 1 2 0 7
	}
        {
		map textures/base_floor/tilefloor7_owfx.tga
		blendfunc GL_ONE GL_ONE
                rgbgen wave triangle 1 5 1 3
	}
    	
}

// ------------------------------------------------------------
// 36.base_floor (skylight1)
// ------------------------------------------------------------

textures/base_floor/skylight1
{
	q3map_lightimage textures/base_floor/skylight1_lm.tga
	q3map_surfacelight 100
	
        {
		map $lightmap
		rgbGen identity
	}
        {
		map textures/base_floor/skylight1.tga
		blendfunc gl_dst_color gl_zero
                rgbGen identity
	}	
        {
		map textures/base_floor/skylight1_lm.tga
		blendfunc GL_ONE GL_ONE
                rgbgen wave triangle .2 2 0 7.7
	}
        {
		map textures/base_floor/skylight1_lm.tga
		blendfunc GL_ONE GL_ONE
                rgbgen wave triangle .2 5 1 5.1
	}
    	
}

// ------------------------------------------------------------
// 37.base_floor (skylight_spec)
// ------------------------------------------------------------
 
textures/base_floor/skylight_spec
{
	{
		rgbGen identity
		map $lightmap
	}
	{
		map textures/base_floor/skylight_spec.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
}

// ------------------------------------------------------------
// 38.base_floor (diamond_dirty)
// ------------------------------------------------------------

// ------------------------------------------------------------
//  This texture has a detail layer that is used to apply a 
//  smeary, dirty overlay to the diamond metal floor texture
// ------------------------------------------------------------

textures/base_floor/diamond_dirty
{
	qer_editorimage textures/base_floor/diamond2.tga
	q3map_globaltexture
	surfaceparm metalsteps
 {
       map $lightmap
       rgbGen identity
 }
	{
		map textures/base_floor/grunge2.tga
                blendfunc GL_DST_COLOR GL_SRC_COLOR
		rgbGen identity
                detail
                tcMod scale 0.0693 0.0712
        }
	{
		map textures/base_floor/diamond2.tga
		tcMod scale 0.5 0.5
		blendfunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 39.base_floor (clangdark)
// ------------------------------------------------------------

textures/base_floor/clangdark
{
	surfaceparm	metalsteps		

	// metal that makes metalsteps sound
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_floor/clangdark.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}


}

// ------------------------------------------------------------
// 40.base_floor (clangdark_ow)
// ------------------------------------------------------------

textures/base_floor/clangdark_ow
{
	surfaceparm	metalsteps		

	// metal that makes metalsteps sound
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_floor/clangdark_ow.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}


}

// ------------------------------------------------------------
// 41.base_floor (diamond_noisy)
// ------------------------------------------------------------

textures/base_floor/diamond_noisy
{
	surfaceparm	metalsteps		

	// metal that makes metalsteps sound
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_floor/diamond2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 42.base_floor (proto_grate)
// ------------------------------------------------------------

textures/base_floor/proto_grate
{
	surfaceparm	metalsteps	
        surfaceparm trans		
	cull none
        nopicmip

	// A GRATE OR GRILL THAT CAN BE SEEN FROM BOTH SIDES
	{
		map textures/base_floor/proto_grate.tga
		//tcMod scale 2 2
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 43.base_floor (proto_grate2)
// ------------------------------------------------------------

textures/base_floor/proto_grate2
{
	surfaceparm	metalsteps	
        surfaceparm trans		
	cull none
        nopicmip

	// A GRATE OR GRILL THAT CAN BE SEEN FROM BOTH SIDES
	{
		map textures/base_floor/proto_grate2.tga
		//tcMod scale 2 2
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 44.base_floor (proto_grate3)
// ------------------------------------------------------------

textures/base_floor/proto_grate3
{
	surfaceparm	metalsteps	
        surfaceparm trans		
	cull none
        nopicmip
	// A GRATE OR GRILL THAT CAN BE SEEN FROM BOTH SIDES
	{
		map textures/base_floor/proto_grate3.tga
		//tcMod scale 2 2
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 45.base_floor (proto_grate4)
// ------------------------------------------------------------

textures/base_floor/proto_grate4
{
	surfaceparm	metalsteps	
        surfaceparm trans		
	surfaceparm nonsolid  //Tim's fuckage
	cull none
        nopicmip

	// A GRATE OR GRILL THAT CAN BE SEEN FROM BOTH SIDES
	{
		map textures/base_floor/proto_grate4.tga
		//tcMod scale 2 2
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 46.base_floor (kc_grate4b)
// ------------------------------------------------------------

textures/base_floor/kc_grate4b
{
	surfaceparm	metalsteps
	surfaceparm alphashadow	
        surfaceparm trans		
	surfaceparm nonsolid
	cull none
       // nopicmip

	// A GRATE OR GRILL THAT CAN BE SEEN FROM BOTH SIDES
	{
		map textures/base_floor/kc_grate4b.tga
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc filter
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 47.base_floor (proto_grate5)
// ------------------------------------------------------------

textures/base_floor/proto_grate5
{
	surfaceparm	metalsteps	
        surfaceparm trans		
	cull none
        nopicmip
	// A GRATE OR GRILL THAT CAN BE SEEN FROM BOTH SIDES
	{
		map textures/base_floor/proto_grate5.tga
		//tcMod scale 2 2
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 48.base_floor (proto_skullgrate)
// ------------------------------------------------------------

textures/base_floor/proto_skullgrate
{
	surfaceparm	metalsteps	
        surfaceparm trans		
	cull none
        nopicmip
	// A GRATE OR GRILL THAT CAN BE SEEN FROM BOTH SIDES
	{
		map textures/base_floor/proto_skullgrate.tga
		//tcMod scale 2 2
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 49.base_floor (proto_rustygrate)
// ------------------------------------------------------------

textures/base_floor/proto_rustygrate
{
	surfaceparm	metalsteps	
    surfaceparm trans	
	surfaceparm alphashadow
	cull none
        nopicmip
	// A RUSTY GRATE OR GRILL THAT CAN BE SEEN FROM BOTH SIDES
	{
		map textures/base_floor/proto_rustygrate.tga
		// tcMod scale 2 2
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 50.base_floor (rusty_pentagrate)
// ------------------------------------------------------------

textures/base_floor/rusty_pentagrate
{
	surfaceparm	metalsteps	
    surfaceparm trans		
    surfaceparm alphashadow

	cull none
    nopicmip
	// A RUSTY and Eeevil grate THAT CAN BE SEEN FROM BOTH SIDES
	{
		map textures/base_floor/rusty_pentagrate.tga
		// tcMod scale 2 2
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 51.base_floor (rusty_baphograte)
// ------------------------------------------------------------

textures/base_floor/rusty_baphograte
{
	surfaceparm	metalsteps	
        surfaceparm trans		
	cull none
        nopicmip
	// A RUSTY and Eeevil grate THAT CAN BE SEEN FROM BOTH SIDES
	{
		map textures/base_floor/rusty_baphograte.tga
		// tcMod scale 2 2
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
        {
		map textures/base_floor/rusty_baphograte2.tga
		blendFunc GL_ONE GL_ONE
		rgbgen wave inversesawtooth 0 1 .2 .5
	}
        {
		map textures/base_floor/rusty_baphograte3.tga
		blendFunc GL_ONE GL_ONE
		rgbgen wave inversesawtooth 0 1 .4 .5
	}
    
}

// ------------------------------------------------------------
// 52.base_floor (pjgrate1)
// ------------------------------------------------------------

textures/base_floor/pjgrate1
{
	surfaceparm	metalsteps		
	cull none

	// A GRATE OR GRILL THAT CAN BE SEEN FROM BOTH SIDES
	{
		map textures/base_floor/pjgrate1.tga
		tcMod scale 2 2
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 53.base_floor (pjgrate2)
// ------------------------------------------------------------

textures/base_floor/pjgrate2
{
	surfaceparm	metalsteps		
	cull none

	// A RUSTED GRATE THAT CAN BE SEEN FROM BOTH SIDES
	{
		map textures/base_floor/pjgrate2.tga
		tcMod scale 2.0 2.0
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 54.base_floor (glowfloor2)
// ------------------------------------------------------------

textures/base_floor/glowfloor2
{
	q3map_surfacelight 800
	q3map_lightimage base_floor/glowfloor2.tga


	// GLOWING RED Floor Color on metalfloor_wall_15
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_wall/metalfloor_wall_15.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_floor/glowred1.blend.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sin 0.5 0.1 1 0.5
	}
}

// ------------------------------------------------------------
// 55.base_floor (glowfloor3)
// ------------------------------------------------------------

textures/base_floor/glowfloor3
{
	q3map_surfacelight 500

	// GLOWING RED Floor Color on Diamond2 floor
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_floor/diamond2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_floor/glowred1.blend.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sin 0.5 0.1 1 0.5
	}
}

// ------------------------------------------------------------
// 56.base_floor (nomarkstone_1)
// ------------------------------------------------------------

textures/base_floor/nomarkstone_1
{
	qer_editorimage textures/base_floor/nomarkstone.tga
	q3map_lightimage textures/base_floor/smallstone.tga
	q3map_globaltexture
	surfaceparm nomarks
	surfaceparm noimpact

	// A stone that isn't marked by weapons. Used below red fog in pj_dm10
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_floor/smallstone.tga
		tcMod scale 2 2
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 57.base_light (baslt4_1_2k)
// ------------------------------------------------------------

textures/base_light/baslt4_1_2k
{
	qer_editorimage textures/base_light/baslt4_1.jpg
	q3map_lightimage textures/base_light/baslt4_1.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2000
	
	
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/baslt4_1.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/base_light/baslt4_1.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 58.base_light (light1_1500)
// ------------------------------------------------------------

textures/base_light/light1_1500
{
	qer_editorimage textures/base_light/light1.jpg
	q3map_lightimage textures/base_light/light1.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 59.base_light (light1_3000)
// ------------------------------------------------------------

textures/base_light/light1_3000
{
	qer_editorimage textures/base_light/light1.jpg
	q3map_lightimage textures/base_light/light1.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 3000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 60.base_light (light1_5000)
// ------------------------------------------------------------

textures/base_light/light1_5000
{
	qer_editorimage textures/base_light/light1.jpg
	q3map_lightimage textures/base_light/light1.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 61.base_light (light1blue_800)
// ------------------------------------------------------------

textures/base_light/light1blue_800
{
	qer_editorimage textures/base_light/light1blue.jpg
	q3map_lightimage textures/base_light/light1blue.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 800
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 62.base_light (light1blue_1500)
// ------------------------------------------------------------

textures/base_light/light1blue_1500
{
	qer_editorimage textures/base_light/light1blue.jpg
	q3map_lightimage textures/base_light/light1blue.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 63.base_light (light1blue_2000)
// ------------------------------------------------------------

textures/base_light/light1blue_2000
{
	qer_editorimage textures/base_light/light1blue.jpg
	q3map_lightimage textures/base_light/light1blue.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2000
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 64.base_light (light1blue_5000)
// ------------------------------------------------------------

textures/base_light/light1blue_5000
{
	qer_editorimage textures/base_light/light1blue.jpg
	q3map_lightimage textures/base_light/light1blue.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 5000
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 65.base_light (light1red_2000)
// ------------------------------------------------------------

textures/base_light/light1red_2000
{
	qer_editorimage textures/base_light/light1red.jpg
	q3map_lightimage textures/base_light/light1red.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1red.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1red.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 66.base_light (light1red_5000)
// ------------------------------------------------------------

textures/base_light/light1red_5000
{
	qer_editorimage textures/base_light/light1red.jpg
	q3map_lightimage textures/base_light/light1red.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1red.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1red.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 67.base_light (light5_3000)
// ------------------------------------------------------------

textures/base_light/light5_3000
{
	qer_editorimage textures/base_light/light5.jpg
	q3map_lightimage textures/base_light/xlight5.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 3000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 68.base_light (light5_20k)
// ------------------------------------------------------------

textures/base_light/light5_20k
{
	qer_editorimage textures/base_light/light5.jpg
	q3map_lightimage textures/base_light/xlight5.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 20000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 69.base_light (light5_5k)
// ------------------------------------------------------------

textures/base_light/light5_5k
{
	qer_editorimage textures/base_light/light5.jpg
	q3map_lightimage textures/base_light/xlight5.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 69.base_light (light5_10k)
// ------------------------------------------------------------

textures/base_light/light5_10k
{
	qer_editorimage textures/base_light/light5.jpg
	q3map_lightimage textures/base_light/xlight5.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 70.base_light (light5_15k)
// ------------------------------------------------------------

textures/base_light/light5_15k
{
	qer_editorimage textures/base_light/light5.jpg
	q3map_lightimage textures/base_light/xlight5.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 15000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/xlight5.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 71.base_light (light1dark)
// ------------------------------------------------------------

textures/base_light/light1dark
{
	qer_editorimage textures/base_light/light1.jpg
	q3map_lightimage textures/base_light/light1.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 500
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 72.base_light (patch10_pj_lite)
// ------------------------------------------------------------

textures/base_light/patch10_pj_lite
{
	qer_editorimage textures/base_light/patch10_pj_lite.jpg
	q3map_lightimage textures/base_light/patch10_pj_lite.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2500
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/patch10_pj_lite.jpg
		blendfunc filter
		rgbgen identity
	}	
	{
		map textures/base_light/patch10_pj_lite.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 73.base_light (patch10_pj_lite2)
// ------------------------------------------------------------

textures/base_light/patch10_pj_lite2
{
	qer_editorimage textures/base_light/patch10_pj_lite2.jpg
	q3map_lightimage textures/base_light/patch10_pj_lite2.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/patch10_pj_lite2.jpg
		blendfunc filter
		rgbgen identity
	}	
	{
		map textures/base_light/patch10_pj_lite2.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 74.base_light (patch10_pj_lite2_1000)
// ------------------------------------------------------------

textures/base_light/patch10_pj_lite2_1000
{
	qer_editorimage textures/base_light/patch10_pj_lite2.jpg
	q3map_lightimage textures/base_light/patch10_pj_lite2.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 1000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/patch10_pj_lite2.jpg
		blendfunc filter
		rgbgen identity
	}	
	{
		map textures/base_light/patch10_pj_lite2.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 75.base_light (proto_light_2k)
// ------------------------------------------------------------

textures/base_light/proto_light_2k
{
	qer_editorimage textures/base_light/proto_light.tga
	q3map_lightimage textures/base_light/proto_lightmap.tga
	surfaceparm metalsteps
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map textures/base_light/proto_light.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/proto_lightmap.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 76.base_light (runway)
// ------------------------------------------------------------

textures/base_light/runway
{
	qer_editorimage textures/base_light/runway.tga
	q3map_lightimage textures/base_light/runwayb.jpg
	surfaceparm nomarks
	q3map_surfacelight 500
	
	{
		map textures/base_light/runway_glow.jpg
		rgbgen wave square 1 4 0 8
		tcmod scale 1 .3
		tcmod scroll 0 1
	}
	{
		map textures/base_light/runway.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
	}
	{
		map textures/base_light/runwayb.tga
		blendFunc add
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 77.base_light (ceil_white5k)
// ------------------------------------------------------------

textures/base_light/ceil_white5k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 78.base_light (proto_lightred2_flare)
// ------------------------------------------------------------

textures/base_light/proto_lightred2_flare
{
	qer_editorimage textures/base_light/proto_lightred.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	q3map_flare flareShader-twilightflare
	
	{
		map $lightmap
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen wave square  1 1 .5 1
	}
}

// ------------------------------------------------------------
// 79.base_light (proto_lightred_flare)
// ------------------------------------------------------------

textures/base_light/proto_lightred_flare
{
	qer_editorimage textures/base_light/proto_lightred.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	q3map_flare flareShader-twilightflare
	
	{
	
		map $lightmap
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 80.base_light (proto_lightblue_flare)
// ------------------------------------------------------------

textures/proto2/proto_lightblue_flare
{
	qer_editorimage textures/proto2/bluelight_on.tga
	surfaceparm nomarks
	q3map_surfacelight 1500
	q3map_flare flareShader-twilightflare
	
	{
		map $lightmap
		rgbGen identity
	}
	{	
		map textures/proto2/bluelight_on.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/proto2/bluelight_on.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave square  1 1 .5 1
	}
}

// ------------------------------------------------------------
// 81.base_light (geolight)
// ------------------------------------------------------------

textures/base_light/geolight
{
	qer_editorimage textures/base_light/geolight.tga
	surfaceparm nomarks
	q3map_surfacelight 3000
	q3map_flare flareShader-wide
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/geolight.tga
		rgbgen identity
	}
	{
		map textures/base_light/geolight_glow.jpg
		blendfunc add
		rgbgen wave sin .1 .1 0 2
	}
}

// ------------------------------------------------------------
// 82.base_light (jaildr1_3)
// ------------------------------------------------------------

textures/base_light/jaildr1_3
{
	qer_editorimage textures/base_light/jaildr1_3.jpg
	q3map_lightimage textures/base_light/jaildr1_3.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/jaildr1_3.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/jaildr1_3.blend.jpg
		blendfunc add
		rgbgen wave sin .1 .1 0 2
	}
}

// ------------------------------------------------------------
// 83.base_light (jaildr03_2)
// ------------------------------------------------------------

textures/base_light/jaildr03_2
{
	qer_editorimage textures/base_light/jaildr03_2.jpg
	q3map_lightimage textures/base_light/jaildr03_2.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2000
	q3map_flare flareShader
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/jaildr03_2.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/jaildr03_2.blend.jpg
		blendfunc add
		rgbgen wave sin .9 .9 .5 2
	}
}

// ------------------------------------------------------------
// 84.base_light (proto_lightblue)
// ------------------------------------------------------------

textures/base_light/proto_lightblue
{
	qer_editorimage textures/base_light/proto_lightblue.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	{
	
		map $lightmap
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightblue.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightblue.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen wave square  1 1 .5 1
	}
}

// ------------------------------------------------------------
// 85.base_light (proto_lightblue_flare)
// ------------------------------------------------------------

textures/base_light/proto_lightblue_flare
{
	qer_editorimage textures/base_light/proto_lightblue.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	q3map_flare flareShader-twilightflare
	
	{
	
		map $lightmap
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightblue.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightblue.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen wave square  1 1 .5 1
	}
}

// ------------------------------------------------------------
// 86.base_light (baslt4_1_4k)
// ------------------------------------------------------------

textures/base_light/baslt4_1_4k
{
	qer_editorimage textures/base_light/baslt4_1.jpg
	surfaceparm nomarks
	q3map_surfacelight 4000
	
	
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/baslt4_1.jpg
		blendFunc filter
		rgbGen identity
	}
	{
		map textures/base_light/baslt4_1.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 87.base_light (ceil1_39)
// ------------------------------------------------------------

textures/base_light/ceil1_39
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 300
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 88.base_light (proto_light)
// ------------------------------------------------------------

textures/base_light/proto_light
{
	qer_editorimage textures/base_light/proto_light.tga
	q3map_lightimage textures/base_light/proto_lightmap.tga
	surfaceparm metalsteps
	q3map_surfacelight 666
	q3map_flare flareShader-wide
	{
		map textures/base_light/proto_light.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/proto_lightmap.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 89.base_light (proto_lightred)
// ------------------------------------------------------------

textures/base_light/proto_lightred
{
	surfaceparm nomarks
	q3map_surfacelight 400
	
	{
	
		map $lightmap
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 90.base_light (proto_lightred2)
// ------------------------------------------------------------

textures/base_light/proto_lightred2
{
	qer_editorimage textures/base_light/proto_lightred.jpg
	surfaceparm nomarks
	q3map_surfacelight 1500
	
	{
		map $lightmap
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightred.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen wave square  1 1 .5 1
	}
}

// ------------------------------------------------------------
// 91.base_light (xlight5)
// ------------------------------------------------------------

textures/base_light/xlight5
{
	qer_editorimage textures/base_light/xlight5.tga
	q3map_lightimage textures/base_light/xlight5.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 1000
	q3map_flare flareShader-glow
	{
		map textures/effects/envmap2.tga
		rgbGen identity
		tcGen environment 
	}
	{
		map textures/base_light/xlight5.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xlight5.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 92.base_light (wsupprt1_12)
// ------------------------------------------------------------


textures/base_light/wsupprt1_12
{
	q3map_lightimage textures/base_light/wsupprt1_12.tga
	q3map_surfacelight 600
	q3map_flare flareShader
	{
		map textures/base_light/wsupprt1_12.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/wsupprt1_12.tga
		blendfunc add
		rgbGen wave noise 0.8 0.2 0 1 
	}
}

// ------------------------------------------------------------
// 93.base_light (light2blue_2500)
// ------------------------------------------------------------

textures/base_light/light2blue_2500
{
	qer_editorimage textures/base_light/light1blue.jpg
	q3map_lightimage textures/base_light/light1blue.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2500
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light1blue.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 94.base_light (lt2_20k)
// ------------------------------------------------------------

textures/base_light/lt2_20k
{
	qer_editorimage textures/base_light/light2.jpg
	q3map_lightimage textures/base_light/light2.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 20000
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light2.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light2.blend.jpg
		rgbGen wave sin .6 .1 .1 .1
		blendfunc add
	}
}

// ------------------------------------------------------------
// 95.base_light (lt2_8000)
// ------------------------------------------------------------

textures/base_light/lt2_8000
{
	qer_editorimage textures/base_light/light2.jpg
	q3map_lightimage textures/base_light/light2.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 8000
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light2.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light2.blend.jpg
		rgbGen wave sin .6 .1 .1 .1
		blendfunc add
	}
}

// ------------------------------------------------------------
// 96.base_light (lt2_4000)
// ------------------------------------------------------------

textures/base_light/lt2_4000
{
	qer_editorimage textures/base_light/light2.jpg
	q3map_lightimage textures/base_light/light2.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 4000
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light2.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light2.blend.jpg
		rgbGen wave sin .6 .1 .1 .1
		blendfunc add
	}
}

// ------------------------------------------------------------
// 97.base_light (lt2_2000)
// ------------------------------------------------------------

textures/base_light/lt2_2000
{
	qer_editorimage textures/base_light/light2.jpg
	q3map_lightimage textures/base_light/light2.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2000
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/light2.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/light2.blend.jpg
		rgbGen wave sin .6 .1 .1 .1
		blendfunc add
	}
}

// ------------------------------------------------------------
// 98.base_light (sm_light1blue)
// ------------------------------------------------------------

textures/base_light/sm_light1blue
{
	qer_editorimage textures/base_light/sm-light1blue.jpg
	q3map_lightimage textures/base_light/sm-light1blue.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2500
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/sm-light1blue.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/sm-light1blue.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 99.base_light (sm_light1red)
// ------------------------------------------------------------

textures/base_light/sm_light1red
{
	qer_editorimage textures/base_light/sm-light1red.jpg
	q3map_lightimage textures/base_light/sm-light1red.blend.jpg
	surfaceparm nomarks
	q3map_surfacelight 2500
	q3map_flare flareShader-subtle
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_light/sm-light1red.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_light/sm-light1red.blend.jpg
		blendfunc add
	}
}

// ------------------------------------------------------------
// 100.base_light (trianglelight)
// ------------------------------------------------------------

textures/base_light/trianglelight
{
	q3map_lightimage textures/base_light/trianglelight.blend.tga
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_surfacelight 2500
	{
		map textures/base_light/trianglelight.tga
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}
	{	
		map textures/base_light/trianglelight.blend.tga
		blendfunc GL_one gl_one
	}

}

// ------------------------------------------------------------
// 101.base_light (ceil1_22a)
// ------------------------------------------------------------

textures/base_light/ceil1_22a
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 102.base_light (ceil1_22a_trans)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_trans
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	surfaceparm nomarks
	surfaceparm trans
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 103.base_light (ceil1_22a_1k)
// ------------------------------------------------------------


textures/base_light/ceil1_22a_1k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 1000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 104.base_light (ceil1_22a_2k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_2k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 105.base_light (ceil1_22a_4k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_4k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 4000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 106.base_light (ceil1_22a_5k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_5k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 107.base_light (ceil1_22a_8k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_8k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 8000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 108.base_light (ceil1_22a_10k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_10k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 109.base_light (ceil1_22a_15k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_15k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 15000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 110.base_light (ceil1_22a_16k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_16k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 16000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 111.base_light (ceil1_22a_20k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_20k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 20000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 112.base_light (ceil1_22a_22k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_22k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 22000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 113.base_light (ceil1_22a_30k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_30k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 30000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 114.base_light (ceil1_22a_40k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_40k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 40000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 115.base_light (ceil1_22a_50k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_50k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 50000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 116.base_light (ceil1_22a_70k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_70k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 70000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 117.base_light (ceil1_22a_90k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_90k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 90000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 118.base_light (ceil1_22a_110k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_110k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 110000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 119.base_light (ceil1_22a_500k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_500k
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	q3map_lightimage textures/base_light/ceil1_22a.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 500000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_22a.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 120.base_light (ceil1_30)
// ------------------------------------------------------------

textures/base_light/ceil1_30
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 121.base_light (ceil1_30_trans)
// ------------------------------------------------------------

textures/base_light/ceil1_30_trans
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	surfaceparm nomarks
	surfaceparm trans
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 122.base_light (ceil1_30_1k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_1k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 1000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 123.base_light (ceil1_30_2k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_2k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 124.base_light (ceil1_30_4k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_4k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 4000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 125.base_light (ceil1_30_5k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_5k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 126.base_light (ceil1_30_8k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_8k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 8000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 127.base_light (ceil1_30_10k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_10k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 128.base_light (ceil1_30_15k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_15k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 15000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 129.base_light (ceil1_30_16k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_16k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 16000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 130.base_light (ceil1_30_20k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_20k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 20000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 131.base_light (ceil1_30_22k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_22k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 22000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 132.base_light (ceil1_30_30k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_30k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 30000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 133.base_light (ceil1_30_40k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_40k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 40000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 134.base_light (ceil1_30_50k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_50k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 50000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 135.base_light (ceil1_30_70k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_70k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 70000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 136.base_light (ceil1_30_90k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_90k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 90000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 137.base_light (ceil1_30_110k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_110k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 110000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 138.base_light (ceil1_30_500k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_500k
{
	qer_editorimage textures/base_light/ceil1_30.tga
	q3map_lightimage textures/base_light/ceil1_30.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 500000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_30.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 139.base_light (ceil1_34)
// ------------------------------------------------------------

textures/base_light/ceil1_34
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 140.base_light (ceil1_34_trans)
// ------------------------------------------------------------

textures/base_light/ceil1_34_trans
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	surfaceparm nomarks
	surfaceparm trans
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 141.base_light (ceil1_34_1k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_1k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 1000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 142.base_light (ceil1_34_2k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_2k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 143.base_light (ceil1_34_4k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_4k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 4000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 144.base_light (ceil1_34_5k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_5k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 145.base_light (ceil1_34_8k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_8k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 8000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 146.base_light (ceil1_34_10k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_10k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 147.base_light (ceil1_34_15k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_15k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 15000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 148.base_light (ceil1_34_16k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_16k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 16000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 149.base_light (ceil1_34_20k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_20k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 20000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 150.base_light (ceil1_34_22k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_22k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 22000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 151.base_light (ceil1_34_30k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_30k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 30000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 152.base_light (ceil1_34_40k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_40k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 40000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 153.base_light (ceil1_34_50k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_50k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 50000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 154.base_light (ceil1_34_70k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_70k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 70000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 155.base_light (ceil1_34_90k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_90k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 90000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 156.base_light (ceil1_34_110k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_110k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 110000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 157.base_light (ceil1_34_500k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_500k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 500000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_34.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 158.base_light (ceil1_37)
// ------------------------------------------------------------

textures/base_light/ceil1_37
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 159.base_light (ceil1_37_trans)
// ------------------------------------------------------------

textures/base_light/ceil1_37_trans
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	surfaceparm nomarks
	surfaceparm trans
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 160.base_light (ceil1_37_1k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_1k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 1000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 161.base_light (ceil1_37_2k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_2k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 162.base_light (ceil1_37_4k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_4k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 4000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 163.base_light (ceil1_37_5k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_5k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 164.base_light (ceil1_37_8k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_8k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 8000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 165.base_light (ceil1_37_10k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_10k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 166.base_light (ceil1_37_15k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_15k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 15000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 167.base_light (ceil1_37_16k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_16k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 16000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 168.base_light (ceil1_37_20k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_20k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 20000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 169.base_light (ceil1_37_22k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_22k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 22000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 170.base_light (ceil1_37_30k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_30k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 30000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 171.base_light (ceil1_37_40k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_40k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 40000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 172.base_light (ceil1_37_50k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_50k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 50000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 173.base_light (ceil1_37_70k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_70k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 70000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 174.base_light (ceil1_37_90k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_90k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 90000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 175.base_light (ceil1_37_110k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_110k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 110000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 176.base_light (ceil1_37_500k)
// ------------------------------------------------------------

textures/base_light/ceil1_37_500k
{
	qer_editorimage textures/base_light/ceil1_37.tga
	q3map_lightimage textures/base_light/ceil1_37.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 500000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_37.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 177.base_light (ceil1_38)
// ------------------------------------------------------------

textures/base_light/ceil1_38
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 178.base_light (ceil1_38_trans)
// ------------------------------------------------------------

textures/base_light/ceil1_38_trans
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	surfaceparm nomarks
	surfaceparm trans
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 179.base_light (ceil1_38_1k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_1k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 1000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 180.base_light (ceil1_38_2k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_2k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 181.base_light (ceil1_38_4k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_4k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 4000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 182.base_light (ceil1_38_5k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_5k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 183.base_light (ceil1_38_8k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_8k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 8000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 184.base_light (ceil1_38_10k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_10k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 185.base_light (ceil1_38_15k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_15k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 15000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 186.base_light (ceil1_38_16k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_16k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 16000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 187.base_light (ceil1_38_20k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_20k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 20000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 188.base_light (ceil1_38_22k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_22k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 22000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 189.base_light (ceil1_38_30k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_30k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 30000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 190.base_light (ceil1_38_40k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_40k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 40000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 191.base_light (ceil1_38_50k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_50k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 50000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 192.base_light (ceil1_38_70k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_70k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 70000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 193.base_light (ceil1_38_90k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_90k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 90000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 194.base_light (ceil1_38_110k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_110k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 110000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 195.base_light (ceil1_38_500k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_500k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	q3map_lightimage textures/base_light/ceil1_38.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 500000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_38.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 196.base_light (ceil1_39)
// ------------------------------------------------------------

textures/base_light/ceil1_39
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 197.base_light (ceil1_39_trans)
// ------------------------------------------------------------

textures/base_light/ceil1_39_trans
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	surfaceparm nomarks
	surfaceparm trans
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 198.base_light (ceil1_39_1k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_1k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 1000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 199.base_light (ceil1_39_2k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_2k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 200.base_light (ceil1_39_4k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_4k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 4000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 201.base_light (ceil1_39_5k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_5k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 202.base_light (ceil1_39_8k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_8k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 8000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 203.base_light (ceil1_39_10k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_10k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 204.base_light (ceil1_39_15k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_15k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 15000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 205.base_light (ceil1_39_16k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_16k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 16000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 206.base_light (ceil1_39_20k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_20k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 20000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 207.base_light (ceil1_39_22k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_22k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 22000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 208.base_light (ceil1_39_30k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_30k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 30000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 209.base_light (ceil1_39_40k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_40k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 40000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 210.base_light (ceil1_39_50k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_50k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 50000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 211.base_light (ceil1_39_70k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_70k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 70000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 212.base_light (ceil1_39_90k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_90k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 90000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 213.base_light (ceil1_39_110k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_110k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 110000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 214.base_light (ceil1_39_500k)
// ------------------------------------------------------------

textures/base_light/ceil1_39_500k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 500000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 215.base_light (ceil1_4)
// ------------------------------------------------------------

textures/base_light/ceil1_4
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 216.base_light (ceil1_4_trans)
// ------------------------------------------------------------

textures/base_light/ceil1_4_trans
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	surfaceparm nomarks
	surfaceparm trans
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 217.base_light (ceil1_4_1k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_1k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 1000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 218.base_light (ceil1_4_2k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_2k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 219.base_light (ceil1_4_4k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_4k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 4000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 220.base_light (ceil1_4_5k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_5k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 221.base_light (ceil1_4_8k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_8k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 8000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 222.base_light (ceil1_4_10k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_10k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 223.base_light (ceil1_4_15k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_15k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 15000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 224.base_light (ceil1_4_16k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_16k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 16000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 225.base_light (ceil1_4_20k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_20k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 20000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 226.base_light (ceil1_4_22k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_22k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 22000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 227.base_light (ceil1_4_30k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_30k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 30000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 228.base_light (ceil1_4_40k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_40k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 40000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 229.base_light (ceil1_4_50k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_50k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 50000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 230.base_light (ceil1_4_70k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_70k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 70000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 231.base_light (ceil1_4_90k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_90k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 90000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 232.base_light (ceil1_4_110k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_110k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 110000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 233.base_light (ceil1_4_500k)
// ------------------------------------------------------------

textures/base_light/ceil1_4_500k
{
	qer_editorimage textures/base_light/ceil1_4.tga
	q3map_lightimage textures/base_light/ceil1_4.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 500000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_4.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 234.base_light (xceil1_39)
// ------------------------------------------------------------

textures/base_light/xceil1_39
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 235.base_light (xceil1_39_trans)
// ------------------------------------------------------------

textures/base_light/xceil1_39_trans
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	surfaceparm nomarks
	surfaceparm trans
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		blendfunc gl_one gl_one_minus_src_alpha
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 236.base_light (xceil1_39_1k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_1k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 1000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 237.base_light (xceil1_39_2k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_2k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 2000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 238.base_light (xceil1_39_4k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_4k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 4000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 239.base_light (xceil1_39_5k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_5k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 5000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 240.base_light (xceil1_39_8k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_8k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 8000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 241.base_light (xceil1_39_10k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_10k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 10000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 242.base_light (xceil1_39_15k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_15k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 15000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 243.base_light (xceil1_39_16k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_16k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 16000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 244.base_light (xceil1_39_20k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_20k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 20000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 245.base_light (xceil1_39_22k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_22k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 22000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 246.base_light (xceil1_39_30k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_30k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 30000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 247.base_light (xceil1_39_40k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_40k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 40000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 248.base_light (xceil1_39_50k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_50k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 50000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 249.base_light (xceil1_39_70k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_70k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 70000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 250.base_light (xceil1_39_90k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_90k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm metalsteps
	q3map_surfacelight 90000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 251.base_light (xceil1_39_110k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_110k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 110000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 252.base_light (xceil1_39_500k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_500k
{
	qer_editorimage textures/base_light/xceil1_39.tga
	q3map_lightimage textures/base_light/xceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 500000
	q3map_flare flareShader-wide
	{
		map textures/base_light/xceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/xceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 253.base_light (xceil1_39_a)
// ------------------------------------------------------------

textures/base_light/xceil1_39_a
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 4000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 254.base_light (xceil1_39_b)
// ------------------------------------------------------------

textures/base_light/xceil1_39_b
{
	qer_editorimage textures/base_light/ceil1_39.tga
	q3map_lightimage textures/base_light/ceil1_39.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 8000
	q3map_flare flareShader-wide
	{
		map textures/base_light/ceil1_39.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc add
	}
}

// ------------------------------------------------------------
// 255.base_light (runway)
// ------------------------------------------------------------

textures/base_light/runway
{
	q3map_lightimage textures/base_light/runway_glow.tga
	surfaceparm nomarks
	q3map_surfacelight 400
	{
		map textures/base_light/runway_glow.tga
		tcmod scale 1 .25
		rgbgen wave square -1 2 .5 8
		tcmod scroll 0 .5
	}	

	{
		map textures/base_light/runway.tga
		blendFunc blend
		rgbGen identity
	}
        
	{
		map $lightmap
		rgbGen identity
		blendfunc filter
	}

	{
		map textures/base_light/runwayb.tga
		blendFunc add
		rgbGen identity
	}

	
}

// ------------------------------------------------------------
// 256.base_light (geolight)
// ------------------------------------------------------------

textures/base_light/geolight
{
	q3map_lightimage textures/base_light/geolight_glow.tga
	surfaceparm nomarks
	q3map_surfacelight 4000
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/geolight.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
        {
		map textures/base_light/geolight_glow.tga
		blendFunc add
		rgbgen wave sin .575 .025 0 10
	}
	
}

// ------------------------------------------------------------
// 257.base_light (dm18twr)
// ------------------------------------------------------------

textures/base_light/dm18twr
{
	q3map_lightimage textures/base_light/dm18twr_glow.tga
	surfaceparm nomarks
	q3map_surfacelight 4000
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/dm18twr.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
        {
		map textures/base_light/dm18twr_glow.tga
		blendFunc add
		rgbgen wave sin .575 .025 0 10
	}
	
}

// ------------------------------------------------------------
// 258.base_light (proto_lightgreen)
// ------------------------------------------------------------

textures/base_light/proto_lightgreen
{
	q3map_lightimage textures/base_light/proto_lightgreen.tga
	surfaceparm nomarks
	q3map_surfacelight 300
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightgreen.tga
		blendFunc filter
		rgbGen identity
	}
        {
		map textures/base_light/proto_lightgreen.tga
		blendFunc add
	}
	
}

// ------------------------------------------------------------
// 259.base_light (cornerlight)
// ------------------------------------------------------------

textures/base_light/cornerlight
{
	q3map_lightimage textures/base_light/cornerlight.glow.tga
	surfaceparm nomarks
	q3map_surfacelight 2500
	{
		map textures/base_light/cornerlight.tga
		rgbGen identity
	}

	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}

		
	{	
		map textures/base_light/cornerlight.glow.tga
		blendfunc GL_one gl_one
		rgbgen wave sawtooth .6 .1 0 7
	}		
}

// ------------------------------------------------------------
// 260.base_light (trianglelight)
// ------------------------------------------------------------

textures/base_light/trianglelight
{
	q3map_lightimage textures/base_light/trianglelight.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 2500
	{
		map textures/base_light/trianglelight.tga
		rgbGen identity
	}

	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}

		
	{	
		map textures/base_light/trianglelight.blend.tga
		blendfunc GL_one gl_one
		rgbgen wave sawtooth .6 .1 0 7
	}

		
}

// ------------------------------------------------------------
// 261.base_light (trianglelight)
// ------------------------------------------------------------

textures/base_light/proto_bulb
{
    deformVertexes autoSprite2
    surfaceparm	trans
	surfaceparm nomarks
	surfaceparm nolightmap
	cull none
    
        {
                map textures/base_light/proto_bulb.tga
                blendFunc GL_ONE GL_ONE
                
        }


}

// ------------------------------------------------------------
// 262.base_light (shi)
// ------------------------------------------------------------

textures/base_light/shi
{
        surfaceparm	trans
	surfaceparm nomarks
	surfaceparm nolightmap
	cull disable
	{
		map textures/base_light/shi.tga
		blendFunc GL_ONE GL_ONE
                tcmod scale .5 .5
		rgbgen wave triangle 1 5.1 1 3
	}
	
        {	
		map textures/base_light/shi.tga
		blendfunc GL_ONE GL_ONE
                tcmod scale .5 .5
                rgbgen wave triangle 1 2 0 7.3
	}
}

// ------------------------------------------------------------
// 263.base_light (proto_light)
// ------------------------------------------------------------

textures/base_light/proto_light
{
	q3map_lightimage textures/base_light/proto_lightmap.tga
	surfaceparm nomarks
	q3map_surfacelight 1000
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/proto_light.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightmap.tga
		blendfunc GL_ONE GL_ONE
                rgbGen wave sin .5 0.5 1 .1
	}
        {	
		map textures/base_light/proto_light2.tga
		blendfunc GL_ONE GL_ONE
                rgbgen wave triangle 1 5 1 3
	}
        {	
		map textures/base_light/proto_light2.tga
		blendfunc GL_ONE GL_ONE
                tcmod scale -1 -1
                 rgbgen wave triangle 1 2 0 7
	}
}

// ------------------------------------------------------------
// 264.base_light (proto_lightred)
// ------------------------------------------------------------

textures/base_light/proto_lightred
{
	q3map_lightimage textures/base_light/proto_lightred.tga
	surfaceparm nomarks
	q3map_surfacelight 300
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightred.tga
		blendFunc filter
		rgbGen identity
	}
        {
		map textures/base_light/proto_lightred.tga
		blendFunc add
	}
	
}

// ------------------------------------------------------------
// 265.base_light (proto_lightred2)
// ------------------------------------------------------------

textures/base_light/proto_lightred2
{
	q3map_lightimage textures/base_light/proto_lightred.tga
	qer_editorimage textures/base_light/proto_lightred.tga
	surfaceparm nomarks
	q3map_surfacelight 1000
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/proto_lightred.tga
		blendFunc filter
		rgbGen identity
	}
        {
		map textures/base_light/proto_lightred.tga
		blendFunc add
		rgbGen wave square .5 .5 0 1
	}
	
}

// ------------------------------------------------------------
// 266.base_light (proto_light_2k)
// ------------------------------------------------------------

textures/base_light/proto_light_2k
{
	qer_editorimage textures/base_light/proto_light.tga
	q3map_lightimage textures/base_light/proto_lightmap.tga
	surfaceparm nomarks
	q3map_surfacelight 2000
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/proto_light.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightmap.tga
		blendfunc GL_ONE GL_ONE
                rgbGen wave sin .5 0.5 1 .1
	}
        {	
		map textures/base_light/proto_light2.tga
		blendfunc GL_ONE GL_ONE
                rgbgen wave triangle 1 5 1 3
	}
        {	
		map textures/base_light/proto_light2.tga
		blendfunc GL_ONE GL_ONE
                tcmod scale -1 -1
                 rgbgen wave triangle 1 2 0 7
	}
}

// ------------------------------------------------------------
// 267.base_light (s_proto_light)
// ------------------------------------------------------------

textures/base_light/s_proto_light
{
	q3map_lightimage textures/base_light/s_proto_lightmap.tga
	surfaceparm nomarks
	q3map_surfacelight 100
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/proto_light.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{	
		map textures/base_light/proto_lightmap.tga
		blendfunc GL_ONE GL_ONE
                rgbGen wave sin .5 0.5 1 .1
	}
        {	
		map textures/base_light/proto_light2.tga
		blendfunc GL_ONE GL_ONE
                rgbgen wave triangle 1 5 1 3
	}
        {	
		map textures/base_light/proto_light2.tga
		blendfunc GL_ONE GL_ONE
                tcmod scale -1 -1
                 rgbgen wave triangle 1 2 0 7
	}
}

// ------------------------------------------------------------
// 268.base_light (scrolllight)
// ------------------------------------------------------------

textures/base_light/scrolllight
{
	q3map_lightimage textures/sfx/scrolllight2.tga
	q3map_surfacelight 100
	
        {
		map $lightmap
		rgbGen identity
	}
        {
		Map textures/base_light/scrolllight.tga
		blendfunc gl_dst_color gl_zero
                rgbGen identity
	}	
        {    
		Map textures/base_light/scrolllight2.tga
		blendFunc GL_ONE GL_ONE
                tcmod scroll -1 0
	}	
        {    
      	        Map textures/base_light/scrolllight2.tga
      	        blendFunc GL_ONE GL_ONE
                tcmod scroll -2 0
        }	
    
	
}

// ------------------------------------------------------------
// 269.base_light (lightflare_wht500)
// ------------------------------------------------------------

textures/base_light/lightflare_wht500
{
	qer_editorimage textures/base_light/light_flare.tga
	q3map_lightimage textures/base_light/light1.blend.tga 
	deformVertexes autoSprite
	q3map_surfacelight	500
	surfaceparm	trans
	surfaceparm nomarks
	surfaceparm nolightmap
	cull none
	
	{
		Map textures/base_light/light_flare.tga
		blendFunc GL_ONE GL_ONE
	}	
	
}

// ------------------------------------------------------------
// 270.base_light (lightflare_500)
// ------------------------------------------------------------

textures/base_light/lightflare_500
{
	qer_editorimage textures/base_light/light_flare.tga
	deformVertexes autoSprite
	q3map_surfacelight	500
	surfaceparm	trans
	surfaceparm nomarks
	surfaceparm nolightmap
	cull none
	
	{
		Map textures/base_light/light_flare.tga
		blendFunc GL_ONE GL_ONE
	}	
	
}

// ------------------------------------------------------------
// 271.base_light (border7_ceil50)
// ------------------------------------------------------------

textures/base_light/border7_ceil50
{
	qer_editorimage textures/base_light/border7_ceil50.tga
	q3map_surfacelight 100
	// Square dirty white llight
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/border7_ceil50.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/border7_ceil50glow.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 272.base_light (ceil1_38_10k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_10k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	// Square dirty white llight
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 273.base_light (ceil1_38_20k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_20k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	surfaceparm nomarks
	q3map_surfacelight 20000
	light 1
	// Square dirty white llight
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 274.base_light (ceil1_38_30k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_30k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	surfaceparm nomarks
	q3map_surfacelight 30000
	light 1
	// Square dirty white llight
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 275.base_light (ceil1_38_40k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_40k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	surfaceparm nomarks
	q3map_surfacelight 40000
	light 1
	// Square dirty white light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 276.base_light (ceil1_38_50k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_50k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	surfaceparm nomarks
	q3map_surfacelight 50000
	light 1
	// Square dirty white light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 277.base_light (ceil1_38_70k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_70k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	surfaceparm nomarks
	q3map_surfacelight 70000
	light 1
	// Square dirty white llight
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 278.base_light (ceil1_38_90k)
// ------------------------------------------------------------

textures/base_light/ceil1_38_90k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	surfaceparm nomarks
	q3map_surfacelight 90000
	light 1
	// Square dirty white llight
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 279.base_light (light1dark)
// ------------------------------------------------------------

textures/base_light/light1dark
{
	qer_editorimage textures/base_light/light1.tga
	q3map_lightimage textures/base_light/light1.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 6000
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{	
		map textures/base_light/light1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 280.base_light (light1red_2000)
// ------------------------------------------------------------

textures/base_light/light1red_2000
{
	qer_editorimage textures/base_light/light1red.tga
	q3map_lightimage textures/base_light/light1red.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 2000
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light1red.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{	
		map textures/base_light/light1red.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 281.base_light (light1red_5000)
// ------------------------------------------------------------

textures/base_light/light1red_5000
{
	qer_editorimage textures/base_light/light1red.tga
	q3map_lightimage textures/base_light/light1red.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 5000
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light1red.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{	
		map textures/base_light/light1red.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 282.base_light (light1blue_5000)
// ------------------------------------------------------------

textures/base_light/light1blue_5000
{
	qer_editorimage textures/base_light/light1blue.tga
	q3map_lightimage textures/base_light/light1blue.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 5000
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light1blue.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{	
		map textures/base_light/light1blue.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 283.base_light (light1blue_2000)
// ------------------------------------------------------------

textures/base_light/light1blue_2000
{
	qer_editorimage textures/base_light/light1blue.tga
	q3map_lightimage textures/base_light/light1blue.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 2000
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light1blue.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{	
		map textures/base_light/light1blue.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 284.base_light (light2blue_2500)
// ------------------------------------------------------------

textures/base_light/light2blue_2500
{
	qer_editorimage textures/base_light/light1blue.tga
	q3map_lightimage textures/base_light/light1blue1.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 2500
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light1blue.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{	
		map textures/base_light/light1blue1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 285.base_light (light1blue_800)
// ------------------------------------------------------------

textures/base_light/light1blue_800
{
	qer_editorimage textures/base_light/light1blue.tga
	q3map_lightimage textures/base_light/light1blue.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 800
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light1blue.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{	
		map textures/base_light/light1blue.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 286.base_light (light1blue_1500)
// ------------------------------------------------------------

textures/base_light/light1blue_1500
{
	qer_editorimage textures/base_light/light1blue.tga
	q3map_lightimage textures/base_light/light1blue.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 1500
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light1blue.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{	
		map textures/base_light/light1blue.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 287.base_light (light1_5000)
// ------------------------------------------------------------

textures/base_light/light1_5000
{
	qer_editorimage textures/base_light/light1.tga
	q3map_lightimage textures/base_light/light1.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 5000
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{	
		map textures/base_light/light1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 288.base_light (light1_3000)
// ------------------------------------------------------------

textures/base_light/light1_3000
{
	qer_editorimage textures/base_light/light1.tga
	light 1
	surfaceparm nomarks
	q3map_surfacelight 1500
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/light1.blend.tga
		//rgbGen wave sin 0.5 0.5 0 3
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 289.base_light (light1_1500)
// ------------------------------------------------------------

textures/base_light/light1_1500
{
	qer_editorimage textures/base_light/light1.tga
	light 1
	surfaceparm nomarks
	q3map_surfacelight 1500
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/light1.blend.tga
		//rgbGen wave sin 0.5 0.5 0 3
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 290.base_light (lt2_8000)
// ------------------------------------------------------------

textures/base_light/lt2_8000
{
	qer_editorimage textures/base_light/light2.tga
	light 1
	surfaceparm nomarks
	q3map_surfacelight 8000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/light2.blend.tga
		rgbGen wave sin .6 .1 .1 .1
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 291.base_light (lt2_4000)
// ------------------------------------------------------------

textures/base_light/lt2_4000
{
	qer_editorimage textures/base_light/light2.tga
	//light 1
	surfaceparm nomarks
	q3map_surfacelight 4000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/light2.blend.tga
		rgbGen wave sin .6 .1 .1 .1
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 292.base_light (lt2_2000)
// ------------------------------------------------------------

textures/base_light/lt2_2000
{
	qer_editorimage textures/base_light/light2.tga
	light 1
	surfaceparm nomarks
	q3map_surfacelight 2000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/light2.blend.tga
		rgbGen wave sin 0.6 0.1 .1 .1
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 293.base_light (light3b_2500)
// ------------------------------------------------------------

textures/base_light/light3b_2500
{
	qer_editorimage textures/base_light/light3b.tga
	light 1
	surfaceparm nomarks
	q3map_surfacelight 2500
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light3b.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/light3b.blend.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 294.base_light (light4_2500)
// ------------------------------------------------------------

textures/base_light/light4_2500
{
	qer_editorimage textures/base_light/light4.tga
	light 1
	surfaceparm nomarks
	q3map_surfacelight 2500
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light4.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/light4.blend.tga
		rgbGen wave sin 0.5 0.5 0 3
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 295.base_light (light5_3000)
// ------------------------------------------------------------

textures/base_light/light5_3000
{
	qer_editorimage textures/base_light/light5.tga
	light 1
	surfaceparm nomarks
	q3map_surfacelight 3000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/xlight5.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/xlight5.blend.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 296.base_light (light5_5k)
// ------------------------------------------------------------

textures/base_light/light5_5k
{
	qer_editorimage textures/base_light/light5.tga
	//light 1
	surfaceparm nomarks
	surfaceparm nolightmap
	q3map_backsplash 0 0
	q3map_surfacelight 5000
	{
		map textures/base_light/light5.tga
		rgbGen identity
	}
//	{
//		map textures/base_light/light5.blend.tga
//		blendFunc GL_ONE GL_ONE
//	}
}

// ------------------------------------------------------------
// 297.base_light (light5_10k)
// ------------------------------------------------------------

textures/base_light/light5_10k
{
	qer_editorimage textures/base_light/light5.tga
	q3map_lightimage textures/base_light/xlight5.blend.tga
	light 1
	surfaceparm nomarks
	q3map_surfacelight 10000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/xlight5.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/xlight5.blend.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 298.base_light (light5_15k)
// ------------------------------------------------------------

textures/base_light/light5_15k
{
	qer_editorimage textures/base_light/light5.tga
	q3map_lightimage textures/base_light/light5.blend.tga
	light 1
	surfaceparm nomarks
	q3map_surfacelight 15000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light5.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/light5.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 299.base_light (light5_20k)
// ------------------------------------------------------------

textures/base_light/light5_20k
{
	qer_editorimage textures/base_light/light5.tga
	light 1
	surfaceparm nomarks
	q3map_surfacelight 20000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light5.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/light5.blend.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 300.base_light (lt6_2500)
// ------------------------------------------------------------

textures/base_light/lt6_2500
{
	qer_editorimage textures/base_light/light6.tga
	light 1
	surfaceparm nomarks
	q3map_surfacelight 2500
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light6.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/light6.blend.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 301.base_light (light6_2500)
// ------------------------------------------------------------

textures/base_light/light6_2500
{
	qer_editorimage textures/base_light/light6.tga
	light 1
	surfaceparm nomarks
	q3map_surfacelight 2500
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light6.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/light6.blend.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 302.base_light (lt6_2p5k)
// ------------------------------------------------------------

textures/base_light/lt6_2p5k
{
	qer_editorimage textures/base_light/light6.tga
	light 1
	surfaceparm nomarks
	q3map_surfacelight 5000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/light6.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/light6.blend.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 303.base_light (patch10_pj_lite)
// ------------------------------------------------------------

textures/base_light/patch10_pj_lite
{
	q3map_surfacelight 2500
	surfaceparm nomarks
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/patch10_pj_lite.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/patch10_pj_lite.blend.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 304.base_light (patch10_pj_lite2)
// ------------------------------------------------------------

textures/base_light/patch10_pj_lite2
{
	q3map_surfacelight 2000
	surfaceparm nomarks
	//light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/patch10_pj_lite2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/patch10_pj_lite2.blend.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 305.base_light (patch10_pj_lite2_1000)
// ------------------------------------------------------------

textures/base_light/patch10_pj_lite2_1000
{
	qer_editorimage textures/base_light/patch10_pj_lite2.tga
	q3map_surfacelight 1000
	surfaceparm nomarks
	//light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/patch10_pj_lite2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/patch10_pj_lite2.blend.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 306.base_light (steedmetallight)
// ------------------------------------------------------------

textures/base_light/steedmetallight
{
	q3map_surfacelight 2000	
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/steedmetallight.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/steedmetallight.blend.tga
		rgbGen wave sin 0.5 0.5 0 .2
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 307.base_light (border11light)
// ------------------------------------------------------------

textures/base_light/border11light
{
	q3map_surfacelight 500	
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/border11light.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/border11light.blend.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 308.base_light (border11light_300)
// ------------------------------------------------------------

textures/base_light/border11light_300
{
	qer_editorimage textures/base_light/border11light.tga
	q3map_lightimage textures/base_light/border11light.blend.tga

	q3map_surfacelight 235	
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/border11light.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/border11light.blend.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 309.base_light (sm_light1red)
// ------------------------------------------------------------

textures/base_light/sm_light1red
{
	light 1
	surfaceparm nomarks
	q3map_surfacelight 2500
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/sm_light1red.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/sm_light1red.blend.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 310.base_light (sm_light1blue)
// ------------------------------------------------------------

textures/base_light/sm_light1blue
{
	light 1
	surfaceparm nomarks
	q3map_surfacelight 2500
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/sm_light1blue.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/sm_light1blue.blend.tga
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 311.base_light (wstlt1_5)
// ------------------------------------------------------------

textures/base_light/wstlt1_5
{
	q3map_surfacelight 2000
	surfaceparm nomarks
	light 1

	// older wall lamp
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/wstlt1_5.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/wstlt1_5.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 312.base_light (pj_wstlt1_3k)
// ------------------------------------------------------------

textures/base_light/pj_wstlt1_3k
{
	qer_editorimage textures/base_light/wstlt1_5.tga
	q3map_lightimage textures/base_light/wstlt1_5.blend.tga
	q3map_surfacelight 3000
	surfaceparm nomarks
	light 1

	// older wall lamp
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/wstlt1_5.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/wstlt1_5.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 313.base_light (baslt3_1_2K)
// ------------------------------------------------------------

textures/base_light/baslt3_1_2K
{
	qer_editorimage textures/base_light/baslt3_1.tga
	surfaceparm nomarks
	q3map_surfacelight 2000
	//light 1

	// Fluorescent light from Q2
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/baslt3_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/baslt3_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 314.base_light (baslt4_1_2K)
// ------------------------------------------------------------

textures/base_light/baslt4_1_2k
{
	qer_editorimage textures/base_light/baslt4_1.tga
	surfaceparm nomarks
	q3map_surfacelight 2000
	//light 1

	// New Fluorescent light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/baslt4_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/baslt4_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 315.base_light (baslt4_1_4K)
// ------------------------------------------------------------

textures/base_light/baslt4_1_4k
{
	qer_editorimage textures/base_light/baslt4_1.tga
	surfaceparm nomarks
	q3map_surfacelight 4000
	//light 1

	// New Fluorescent light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/baslt4_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/baslt4_1.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 316.base_light (ceil1_37)
// ------------------------------------------------------------

textures/base_light/ceil1_37
{
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	// Modified light blue ceil light from Q2
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_37.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_37.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 317.base_light (ceil1_2)
// ------------------------------------------------------------

textures/base_light/ceil1_2
{
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_2.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 318.base_light (ceil1_22a)
// ------------------------------------------------------------

textures/base_light/ceil1_22a
{
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	//	A red-orange ceiling light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_22a.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 319.base_light (ceil1_22a_trans)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_trans
{
	qer_editorimage textures/base_light/ceil1_22a.tga
	surfaceparm nomarks
	surfaceparm nonsolid
	q3map_surfacelight 10000
	light 1
	//	A red-orange ceiling light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_22a.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 320.base_light (ceil1_22a_8k)
// ------------------------------------------------------------

textures/base_light/ceil1_22a_8k
{
	surfaceparm nomarks
	qer_editorimage textures/base_light/ceil1_22a
	q3map_surfacelight 8000
	light 1
	//	A red-orange ceiling light (wattage 8k)
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_22a.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_22a.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 321.base_light (ceil1_4)
// ------------------------------------------------------------

textures/base_light/ceil1_4
{
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	//	A yellow ceiling light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_4.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_4.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 322.base_light (ceil1_3)
// ------------------------------------------------------------

textures/base_light/ceil1_3
{
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	//	A red ceiling light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_3.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_3.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 323.base_light (ceil1_35)
// ------------------------------------------------------------

textures/base_light/ceil1_35
{
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	// Square green light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_35.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_35.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 324.base_light (ceil1_30)
// ------------------------------------------------------------

textures/base_light/ceil1_30
{
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	// Square rich blue light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_30.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 325.base_light (ceil1_30_trans)
// ------------------------------------------------------------

textures/base_light/ceil1_30_trans
{
	qer_editorimage textures/base_light/ceil1_30.tga
	surfaceparm nomarks
	surfaceparm nonsolid
	q3map_surfacelight 10000
	light 1
	// Square rich blue light non-solid
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_30.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 326.base_light (ceil1_30_8k)
// ------------------------------------------------------------

textures/base_light/ceil1_30_8k
{
	surfaceparm nomarks
	qer_editorimage textures/base_light/ceil1_30
	q3map_surfacelight 8000
	light 1
	// Square rich blue light (8k light wattage
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_30.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_30.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 327.base_light (ceil1_31)
// ------------------------------------------------------------

textures/base_light/ceil1_31
{
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	// Square magenta light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_31.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_31.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 328.base_light (ceil1_32)
// ------------------------------------------------------------

textures/base_light/ceil1_32
{
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	// Square violet light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_32.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_32.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 329.base_light (ceil1_33)
// ------------------------------------------------------------

textures/base_light/ceil1_33
{
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	// Square golden yellow light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_33.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_33.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 330.base_light (ceil1_34)
// ------------------------------------------------------------

textures/base_light/ceil1_34
{
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	// Square tourquoise light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_34.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 331.base_light (ceil1_34_5k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_5k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	surfaceparm nomarks
	q3map_surfacelight 5000
	light 1
	// Square tourquoise light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_34.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 332.base_light (ceil1_34_10k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_10k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	// Square tourquoise light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_34.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 333.base_light (ceil1_34_2k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_2k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 2000
	light 1
	// Square tourquoise light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_34.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 334.base_light (ceil1_34_1k)
// ------------------------------------------------------------

textures/base_light/ceil1_34_1k
{
	qer_editorimage textures/base_light/ceil1_34.tga
	q3map_lightimage textures/base_light/ceil1_34.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 1000
	light 1
	// Square tourquoise light
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_34.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_34.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 335.base_light (ceil1_white5k)
// ------------------------------------------------------------

textures/base_light/ceil_white5k
{
	qer_editorimage textures/base_light/ceil1_38.tga
	surfaceparm nomarks
	q3map_surfacelight 5000
	light 1
	// Square dirty white llight
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 336.base_light (ceil1_38)
// ------------------------------------------------------------

textures/base_light/ceil1_38
{
	surfaceparm nomarks
	q3map_surfacelight 2500
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_38.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 337.base_light (pjceil1_39_1K)
// ------------------------------------------------------------

textures/base_light/pjceil1_39_1K
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 1000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 338.base_light (ceil1_39)
// ------------------------------------------------------------

textures/base_light/ceil1_39
{
	surfaceparm nomarks
	q3map_surfacelight 7500
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 339.base_light (pjceil1_39_1p5k)
// ------------------------------------------------------------

textures/base_light/pjceil1_39_1p5k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 1500
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 340.base_light (xceil1_39_2k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_2k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 2000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 341.base_light (xceil1_39_5k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_5k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 5000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 342.base_light (xceil1_39_10k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_10k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 343.base_light (xceil1_39_15k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_15k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 15000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 344.base_light (xceil1_39_20k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_20k
{
//	q3map_backsplash 0 0
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 20000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 345.base_light (xceil1_39_30k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_30k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 30000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 346.base_light (xceil1_39_50k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_50k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 50000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 347.base_light (xceil1_39_70k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_70k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 70000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 348.base_light (xceil1_39_90k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_90k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 90000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 349.base_light (xceil1_39_110k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_110k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 110000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 350.base_light (xceil1_39_500k)
// ------------------------------------------------------------

textures/base_light/xceil1_39_500k
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 500000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 351.base_light (xceil1_39_1)
// ------------------------------------------------------------

textures/base_light/xceil1_39_1
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 10000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 352.base_light (xceil1_39)
// ------------------------------------------------------------

textures/base_light/xceil1_39
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 20000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 353.base_light (xceil1_39_a)
// ------------------------------------------------------------

textures/base_light/xceil1_39_a
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 30000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 354.base_light (xceil1_39_b)
// ------------------------------------------------------------

textures/base_light/xceil1_39_b
{
	qer_editorimage textures/base_light/ceil1_39.tga
	surfaceparm nomarks
	q3map_surfacelight 40000
	light 1
	// Square dirty white
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/ceil1_39.blend.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 355.base_light (jaildr01_3)
// ------------------------------------------------------------

textures/base_light/jaildr01_3
{
	surfaceparm nomarks
	q3map_surfacelight 1000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/jaildr01_3.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/jaildr01_3.blend.tga
		rgbGen wave sin 0.5 0.5 1 1
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 356.base_light (jaildr02_3)
// ------------------------------------------------------------

textures/base_light/jaildr02_3
{
	surfaceparm nomarks
	q3map_surfacelight 1000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/jaildr02_3.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/jaildr02_3.blend.tga
		rgbGen wave sin 0.5 0.5 1 1
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 357.base_light (jaildr03_1)
// ------------------------------------------------------------

textures/base_light/jaildr03_1
{
	surfaceparm nomarks
	q3map_surfacelight 1000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/jaildr03_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/jaildr03_1.tga
		rgbGen wave sin 0.5 0.5 1 1
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 358.base_light (jaildr03_2)
// ------------------------------------------------------------

textures/base_light/jaildr03_2
{
	surfaceparm nomarks
	q3map_surfacelight 1000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/jaildr03_2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/jaildr03_2.blend.tga
		rgbGen wave sin 0.5 0.5 1 1
		blendfunc GL_ONE GL_ONE
	}

}

// ------------------------------------------------------------
// 359.base_light (jaildr1_3)
// ------------------------------------------------------------

textures/base_light/jaildr1_3
{
	surfaceparm nomarks
	q3map_surfacelight 1000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/jaildr1_3.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/jaildr1_3.blend.tga
		rgbGen wave sin 0.5 0.5 1 1
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 360.base_light (jaildr2_3)
// ------------------------------------------------------------

textures/base_light/jaildr2_3
{
	surfaceparm nomarks
	q3map_surfacelight 1000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/jaildr2_3.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/jaildr2_3.blend.tga
//		rgbGen wave sin 0.5 0.5 1 1
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 361.base_light (wsupprt1_12)
// ------------------------------------------------------------

textures/base_light/wsupprt1_12
{
	surfaceparm nomarks
	q3map_surfacelight 1000
	//light1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/wsupprt1_12.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/wsupprt1_12.blend.tga
		rgbGen wave sin 0.5 0.5 1 1
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 362.base_light (wsupprt1_12h)
// ------------------------------------------------------------

textures/base_light/wsupprt1_12h
{
	surfaceparm nomarks
	q3map_surfacelight 1000
	//light1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_light/wsupprt1_12h.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_light/wsupprt1_12h.blend.tga
		rgbGen wave sin 0.5 0.5 1 1
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 363.base_light (ceil1_26)
// ------------------------------------------------------------

textures/base_light/ceil1_26
{
	surfaceparm nomarks
	q3map_surfacelight 10000
	//light1
}

// ------------------------------------------------------------
// 364.base_light (ceil1_28)
// ------------------------------------------------------------

textures/base_light/ceil1_28
{
	surfaceparm nomarks
	q3map_surfacelight 10000
	//light1
}

// ------------------------------------------------------------
// 365.base_light (ceil1_6)
// ------------------------------------------------------------

textures/base_light/ceil1_6
{
	surfaceparm nomarks
	q3map_surfacelight 10000
	//light1
}

// ------------------------------------------------------------
// 366.base_light (tlight05)
// ------------------------------------------------------------

textures/base_light/tlight05
{
	surfaceparm nomarks
	q3map_surfacelight 5000
	//light1
}

// ------------------------------------------------------------
// 367.base_light (invisi_light1)
// ------------------------------------------------------------

textures/base_light/invisi_light1
{
	qer_editorimage base_light/ceil1_30.tga
	qer_trans 50
	//surfaceparm nodraw
	surfaceparm nomarks
	q3map_surfacelight 50000
	light 1
	// Square blue light
	
}

// ------------------------------------------------------------
// 368.base_object (plasmalogo_1)
// ------------------------------------------------------------

textures/base_object/plasmalogo_1
{
	q3map_lightimage textures/base_object/plasmalogo.tga
	//surfaceparm nomarks
	q3map_surfacelight 1000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_object/plasmalogo_1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_object/plasmalogo.tga
		rgbGen wave triangle 0.5 0.5 0.5 0.15
		blendfunc GL_ONE GL_ONE
	}

}

// ------------------------------------------------------------
// 369.base_object (quadsign_1)
// ------------------------------------------------------------

textures/base_object/quadsign_1
{
	qer_editorimage base_object/quadsign1.tga
	q3map_lightimage textures/base_object/quadsign1.blend.tga
	//surfaceparm nomarks
	q3map_surfacelight 1000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_object/quadsign1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_object/quadsign1.blend.tga
		rgbGen wave triangle 0.5 0.5 0.5 0.15
		blendfunc GL_ONE GL_ONE
	}

}

// ------------------------------------------------------------
// 370.base_object (hastesign_1)
// ------------------------------------------------------------

textures/base_object/hastesign_1
{
	qer_editorimage base_object/hastesign1.tga
	q3map_lightimage textures/base_object/hastesign1.blend.tga
	//surfaceparm nomarks
	q3map_surfacelight 1000
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_object/hastesign1.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_object/hastesign1.blend.tga
		rgbGen wave triangle 0.5 0.5 0.5 0.15
		blendfunc GL_ONE GL_ONE
	}

}

// ------------------------------------------------------------
// 371.base_object (boxQ3_1)
// ------------------------------------------------------------

textures/base_object/boxQ3_1
{
	// A green box with a specular map on it
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_object/boxQ3_1.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
}

// ------------------------------------------------------------
// 372.base_object (boxQ3_2)
// ------------------------------------------------------------

textures/base_object/boxQ3_2
{
	// A silver box with a specular and a lightglow map on it
	
	q3map_surfacelight 200
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_object/boxQ3_2.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
	{
		map textures/base_object/boxQ3_2.blend.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sin 0.5 0.5 0 .2
	}
}

// ------------------------------------------------------------
// 373.base_object (boxQ3_3)
// ------------------------------------------------------------

textures/base_object/boxQ3_3
{
	//	A red box side with a specular map on it
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_object/boxQ3_3.tga
		rgbGen identity
		alphaGen lightingSpecular
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
	}
}

// ------------------------------------------------------------
// 374.base_object (boxQ3_4)
// ------------------------------------------------------------

textures/base_object/boxq3_4
{
	//	A golden yellow box with a specular map on it
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_object/boxq3_4.tga
		rgbGen identity
		alphaGen lightingSpecular
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
	}
}

// ------------------------------------------------------------
// 375.base_support (x_support4)
// ------------------------------------------------------------

textures/base_support/x_support4
{
	surfaceparm	metalsteps	
	surfaceparm trans	
	surfaceparm alphashadow
    surfaceparm nomarks	
	cull none
        nopicmip
	{
		map textures/base_support/x_support4.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 376.base_support (x_support3)
// ------------------------------------------------------------

textures/base_support/x_support3
{
	surfaceparm	metalsteps	
	surfaceparm trans	
	surfaceparm alphashadow
    surfaceparm nomarks	
	cull none
        nopicmip
	{
		map textures/base_support/x_support3.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 377.base_support (support1shiny)
// ------------------------------------------------------------

textures/base_support/support1shiny
{   
    
        {
                map textures/effects/tinfx.tga       
                tcGen environment
                rgbGen identity
	}   
        {
		map textures/base_support/support1shiny.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	} 
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 378.base_support (support1rust)
// ------------------------------------------------------------

textures/base_support/support1rust
{
	//deformVertexes wave 100 sin 3 2 .1 3
        {
		rgbGen identity
		map $lightmap
	}
	{
		map textures/base_support/support1rust.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
        {
		map textures/base_trim/tinfx.tga
                tcgen environment
		blendFunc GL_ONE GL_ONE
		rgbGen identity
	}
       
        {
		map textures/base_support/support1rust.tga
		//blendFunc GL_ONE GL_ONE
                blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 379.base_support (support2rust)
// ------------------------------------------------------------
      
textures/base_support/support2rust
{
	qer_editorimage textures/base_support/support1rust.tga
        {
		rgbGen identity
		map $lightmap
	}
	{
		map textures/base_support/support1rust.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 380.base_support (grate1_3)
// ------------------------------------------------------------

textures/base_support/grate1_3
{

	{
		map $lightmap
		rgbgen identity      
	}

	{
		map textures/base_support/grate1_3.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		alphaGen lightingSpecular
		rgbGen identity
	}
				
}

// ------------------------------------------------------------
// 381.base_support (shinysupport2)
// ------------------------------------------------------------

textures/base_support/shinysupport2
{

	{
		map textures/base_wall/chrome_env.tga
		rgbgen identity      
	}

	{
		map textures/base_support/shinysupport2.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		alphaGen lightingSpecular
		rgbGen identity
	}

	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbgen identity      
	}

				
}

// ------------------------------------------------------------
// 382.base_support (shinysupport1)
// ------------------------------------------------------------

textures/base_support/shinysupport1
{

	{
		map $lightmap
		tcgen environment
		tcmod scale .25 .25
		rgbgen identity      
	}
		
	
	{
		map textures/base_support/shinysupport1.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}


	{
		map $lightmap
    		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}

			
}

// ------------------------------------------------------------
// 383.base_support (x_support)
// ------------------------------------------------------------

textures/base_support/x_support
{
	surfaceparm	metalsteps	
	surfaceparm trans	
	surfaceparm alphashadow
    surfaceparm nomarks	
	cull none
        nopicmip
	{
		map textures/base_support/x_support.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 384.base_support (z_support)
// ------------------------------------------------------------

textures/base_support/z_support
{
	surfaceparm	metalsteps	
    surfaceparm trans	
	surfaceparm alphashadow
    surfaceparm nomarks	
	cull none
        nopicmip
	{
		map textures/base_support/z_support.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 385.base_support (x_support2)
// ------------------------------------------------------------

textures/base_support/x_support2
{
	surfaceparm	metalsteps	
    	surfaceparm trans	
	surfaceparm alphashadow
	surfaceparm playerclip
   	surfaceparm nonsolid
	surfaceparm nomarks	
	cull none
        nopicmip
	{
		map textures/base_support/x_support2.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 386.base_support (metal1_5)
// ------------------------------------------------------------

textures/base_support/metal1_5
{
	surfaceparm	metalsteps
}

textures/base_support/pj_metal14_1
//Paul Jaquays uses this on the tops of metal frameworks
{
	surfaceparm	metalsteps		

	// metal that makes metalsteps sound
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_support/metal14_1.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

// ------------------------------------------------------------
// 387.base_support (cable_trans)
// ------------------------------------------------------------

textures/base_support/cable_trans
{
	
	surfaceparm nonsolid
	{
		map $lightmap
		rgbGen identity
	
	}
	{
		map textures/base_support/cable_trans.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO

	
	}
}

// ------------------------------------------------------------
// 388.base_trim (pj_launcher)
// ------------------------------------------------------------

textures/base_trim/pj_launcher
{
        {
        	 map textures/base_trim/pj_launcher2.tga
        	 tcmod scroll 0 2
                 rgbGen identity
        }      
        {
        	 map textures/base_trim/pj_launcher.tga
        	 blendfunc blend
                 rgbGen identity
        }
        {
		map $lightmap
                blendFunc filter
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 389.base_trim (pewter_nonsolid)
// ------------------------------------------------------------

textures/base_trim/pewter_nonsolid
{
	qer_editorimage textures/base_trim/dirty_pewter.tga
	surfaceparm nonsolid
	{
		map $lightmap
		rgbGen identity
	
	}
	{
		map textures/base_trim/dirty_pewter.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO

	
	}
}

// ------------------------------------------------------------
// 390.base_trim (proto_comp)
// ------------------------------------------------------------

textures/base_trim/proto_comp
{
        {
        	 map textures/base_trim/proto_comp2.tga
        	 tcmod scroll 0 -.2
        	 rgbGen wave sin .5 .5 0 3
                 rgbGen identity
        }
        {
        	 clampmap textures/sfx/console03.tga
        	 tcmod rotate -33
                 tcMod stretch sin .8 0 0 0
                 blendfunc add
                 rgbGen wave sin .6 0 0 0
        }
        {
        	 clampmap textures/sfx/console02.tga
        	 tcmod rotate -333
                 tcMod stretch sin .8 0 0 0
                 blendfunc add
                 rgbGen wave sin .3 0 0 0
        }
        {
        	 map textures/effects/tinfx2c.tga
        	 tcGen environment
                 blendfunc add
                 rgbGen identity
        }
        {
        	 map textures/base_trim/proto_comp.tga
        	 blendfunc blend
        }
        {
		map $lightmap
                blendFunc filter
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 391.base_trim (pewter_shiney)
// ------------------------------------------------------------

textures/base_trim/pewter_shiney
{   
 
        {
                map textures/effects/tinfx.tga       
                tcGen environment
                rgbGen identity
	}   
        {
		map textures/base_trim/pewter_shiney.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	} 
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 392.base_trim (panel_glo)
// ------------------------------------------------------------

textures/base_trim/panel_glo
{   
    
        {
                map textures/sfx/firegorre2.tga       
                
                tcMod turb 0 .6 0 0.06
                tcmod scale .53 .61
                tcmod scroll 0 4
                blendFunc GL_ONE GL_ZERO
                rgbGen wave sin 1 1 0 .1
	}   
        {
		map textures/base_trim/panel_glo.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	} 
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 393.base_trim (spidertrim4)
// ------------------------------------------------------------

textures/base_trim/spidertrim4
{   
    
        {
                map textures/effects/tinfx.tga       
                tcGen environment
                rgbGen identity
	}   
        {
		map textures/base_trim/spidertrim4.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	} 
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 394.base_trim (panel_shiny)
// ------------------------------------------------------------

textures/base_trim/panel_shiny
{   
    
        {
                map textures/effects/tinfx.tga       
                tcGen environment
                rgbGen identity
	}   
        {
		map textures/base_trim/panel_shiny.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	} 
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 395.base_trim (spidertrim)
// ------------------------------------------------------------

textures/base_trim/spidertrim3
{   
    
        {
                map textures/effects/tinfx.tga       
                tcGen environment
                rgbGen identity
	}   
        {
		map textures/base_trim/spidertrim3.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	} 
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 396.base_trim (wires02)
// ------------------------------------------------------------

textures/base_trim/wires02
{
    cull disable
    surfaceparm alphashadow
    surfaceparm trans	
        {
                map textures/base_trim/wires02.tga
                alphaFunc GE128
		depthWrite
		rgbGen vertex
        }
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
		depthFunc equal
	}


}

// ------------------------------------------------------------
// 397.base_trim (wires01)
// ------------------------------------------------------------

textures/base_trim/wires01
{
    cull disable
    surfaceparm alphashadow
    deformVertexes autoSprite2
    surfaceparm trans	
        {
                map textures/base_trim/wires01.tga
                alphaFunc GE128
		depthWrite
		rgbGen vertex
        }
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
		depthFunc equal
	}


}

// ------------------------------------------------------------
// 398.base_trim (wires01_ass)
// ------------------------------------------------------------

textures/base_trim/wires01_ass
{
    cull disable
    surfaceparm alphashadow
    deformVertexes autoSprite2
        {
                map textures/base_trim/wires01_ass.tga
                alphaFunc GE128
		depthWrite
		rgbGen vertex
        }
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
		depthFunc equal
	}


}

// ------------------------------------------------------------
// 399.base_trim (spiderbite)
// ------------------------------------------------------------

textures/base_trim/spiderbite
{

        {
		map textures/sfx/proto_zzztblu2.tga
                tcMod turb 0 .3 0 .2
                tcmod scroll 6 .7
                tcmod scale .4 1
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
        {
	        map textures/base_trim/spiderbite.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
                tcMod stretch sin .8 0.4 0 .2
                tcmod scroll .3 0
	        rgbGen identity
	}
        {
	        map textures/base_trim/spiderbite.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
                tcMod stretch sin .8 0.2 0 .1
                tcmod scroll -.5 0
	        rgbGen identity
	}
	{
	        map textures/base_trim/spiderbite.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	        rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 400.base_trim (dark_tin2)
// ------------------------------------------------------------

textures/base_trim/dark_tin2
{   
   
        {
                map textures/effects/tinfx.tga       
                tcGen environment
                rgbGen vertex
	}  
        {
		map textures/base_trim/dark_tin2.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
         
}

// ------------------------------------------------------------
// 401.base_trim (dark_tin2_trans)
// ------------------------------------------------------------

textures/base_trim/dark_tin2_trans
{  
	qer_editorimage textures/base_trim/dark_tin2
	surfaceparm nonsolid
   
        {
                map textures/effects/tinfx.tga       
                tcGen environment
                rgbGen vertex
	}  
        {
		map textures/base_trim/dark_tin2.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
         
}

// ------------------------------------------------------------
// 402.base_trim (skull_monitor)
// ------------------------------------------------------------

textures/base_trim/skull_monitor
{   
    
        {
                map textures/base_trim/skull_monitor.tga 
                tcMod turb 0 .05 0 .2
                rgbGen identity
	} 
        { 
                 map textures/sfx/text.tga
                 blendFunc GL_ONE GL_ONE
                 tcmod scroll 1 0
                 rgbGen identity
	}  
        { 
                 map textures/effects/tinfx2.tga
                 blendFunc GL_ONE GL_ONE
                 tcGen environment
                 rgbGen identity
	}  
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
         
}

// ------------------------------------------------------------
// 403.base_trim (deeprust_trans)
// ------------------------------------------------------------

textures/base_trim/deeprust_trans
{
	qer_editorimage textures/base_trim/deeprust.tga
	surfaceparm nonsolid
	{
		map $lightmap
		rgbGen identity
	
	}
	{
		map textures/base_trim/deeprust.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO

	
	}
}

// ------------------------------------------------------------
// 404.base_trim (yellow_rustc)
// ------------------------------------------------------------

textures/base_trim/yellow_rustc
{
	
	{
		map textures/base_wall/chrome_env.tga
		tcgen environment
		tcmod scale .25 .25
		rgbGen identity
	}
	{
		map textures/base_trim/yellow_rustc.tga
		blendFunc gl_one_minus_src_alpha gl_src_alpha
	//	alphagen lightingspecular
		rgbGen identity
	}

	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}

	
       
}

// ------------------------------------------------------------
// 405.base_trim (panelbrn010w)
// ------------------------------------------------------------

textures/base_trim/panelbrn010w
{
	q3map_lightimage textures/base_trim/panelbrn010wfx.tga
	
        {
		map textures/base_trim/panelbrn010w.tga
        }
	
        {
		map textures/sfx/hologirl2.tga
                tcmod scale .4 .4
                tcmod scroll 6 2
                blendFunc GL_ONE GL_ONE
                rgbGen identity
	}
	{
		map textures/base_trim/panelbrn010w.tga
                blendfunc blend
		
	}
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
       
}

// ------------------------------------------------------------
// 406.base_trim (pewter_spec)
// ------------------------------------------------------------

textures/base_trim/pewter_spec
{
	{
		rgbGen identity
		map $lightmap
	}
	{
		map textures/base_trim/pewter_spec.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
      
}

// ------------------------------------------------------------
// 407.base_trim (tin)
// ------------------------------------------------------------

textures/base_trim/tin
{
	//deformVertexes wave 100 sin 3 2 .1 3
        {
		rgbGen identity
		map $lightmap
	}
	{
		map textures/base_trim/tin.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
        {
		map textures/base_trim/tinfx.tga
                tcgen environment
		blendFunc GL_ONE GL_ONE
		rgbGen identity
	}
       
        {
		map textures/base_trim/tin.tga
		//blendFunc GL_ONE GL_ONE
                blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 408.base_trim (dark_tin)
// ------------------------------------------------------------
  
textures/base_trim/dark_tin
{
	//deformVertexes wave 100 sin 3 2 .1 3
        {
		rgbGen identity
		map $lightmap
	}
	{
		map textures/base_trim/dark_tin.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
        {
		map textures/base_trim/tinfx.tga
                tcgen environment
		blendFunc GL_ONE GL_ONE
		rgbGen identity
	}
       
        {
		map textures/base_trim/dark_tin.tga
		//blendFunc GL_ONE GL_ONE
                blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
       
              
}

// ------------------------------------------------------------
// 409.base_trim (techborder)
// ------------------------------------------------------------

textures/base_trim/techborder
{
	q3map_surfacelight 300	
        q3map_lightimage textures/base_trim/techborder_fx.tga
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_trim/techborder.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/base_trim/techborder_fx.tga
                //tcmod scale  .1 1
                tcmod scroll .1 0
                
		blendfunc GL_ONE GL_ONE
	}
        {
		map textures/base_trim/techborder_fx.tga
                tcmod scale  .1 1
                tcmod scroll .1 0
                
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 410.base_trim (spiderbit)
// ------------------------------------------------------------

textures/base_trim/spiderbit
{
	q3map_lightimage textures/base_trim/spiderbit_fx.tga
	q3map_surfacelight 100
	
        {
		map $lightmap
		rgbGen identity
	}
        {
		map textures/base_trim/spiderbit.tga
		blendfunc gl_dst_color gl_zero
                rgbGen identity
	}	
        {
		map textures/base_trim/spiderbit_fx.tga
		blendfunc GL_ONE GL_ONE
                rgbgen wave triangle .1 .5 0 7
	}
    	
}

// ------------------------------------------------------------
// 411.base_trim (proto_fence)
// ------------------------------------------------------------

textures/base_trim/proto_fence
{
	surfaceparm trans		
	cull none
        nopicmip

	{
		map textures/base_trim/proto_fence.tga
		tcMod scale 3 3
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 412.base_trim (border11light)
// ------------------------------------------------------------

textures/base_trim/border11light
{
	q3map_surfacelight 500
	Q3map_lightimage textures/base_trim/border11light.glow.tga	
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_trim/border11light.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/base_trim/border11light.glow.tga
		blendfunc GL_ONE GL_ONE
		rgbgen wave sin .9 .1 0 10
	}
}

// ------------------------------------------------------------
// 413.base_trim (border11_250)
// ------------------------------------------------------------

textures/base_trim/border11_250
{
	qer_editorimage textures/base_trim/border11light.tga
	q3map_surfacelight 250
	q3map_lightimage textures/base_trim/border11light.glow.tga	
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_trim/border11light.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/base_trim/border11light.glow.tga
		blendfunc GL_ONE GL_ONE
		rgbgen wave sin .9 .1 0 10
	}
}

// ------------------------------------------------------------
// 414.base_trim (border12b)
// ------------------------------------------------------------

textures/base_trim/border12b
{
	q3map_surfacelight 1000	
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_trim/border12b.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/base_trim/border12bfx.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 415.base_trim (border12b_pj)
// ------------------------------------------------------------

textures/base_trim/border12b_pj
{
	qer_editorimage textures/base_trim/border12b.tga
	q3map_surfacelight 150	
	q3map_lightimage textures/base_trim/border12bfx.tga	
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_trim/border12b.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/base_trim/border12bfx.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 416.base_trim (xborder12b)
// ------------------------------------------------------------

textures/base_trim/xborder12b
{
	qer_editorimage textures/base_trim/border12b.tga
	q3map_surfacelight 100	
	q3map_lightimage textures/base_trim/border12bfx.tga
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_trim/border12b.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/base_trim/border12bfx.tga
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 417.base_trim (drag_glass1)
// ------------------------------------------------------------

textures/base_trim/drag_glass1
{

	surfaceparm trans
	surfaceparm nolightmap
	{
		map textures/base_trim/drag_glass1.tga
		blendFunc GL_ONE_MINUS_DST_COLOR GL_ZERO
	}
	{
		map textures/base_trim/drag_glass1.tga
		alphaFunc LT128
		blendfunc GL_DST_COLOR GL_ONE
	}
}

// ------------------------------------------------------------
// 418.base_trim (electricwall)
// ------------------------------------------------------------

textures/base_trim/electricwall	
{
	qer_editorimage textures/organics/wire02a_f.tga
	light 1	
	q3map_surfacelight 400

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/organics/wire02a_f.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/organics/wire02a_f.tga
		blendFunc GL_ONE GL_ONE
	}
	{
		map textures/sfx/electric2.blend.tga
		rgbGen wave sin 0.6 1 0 2
	//	tcMod rotate 54
		tcMod scroll 0.53 0.43
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 419.base_trim (electric_blue)
// ------------------------------------------------------------

textures/base_trim/electric_blue
{
	qer_editorimage textures/base_trim/electricwall_blue.tga
	q3map_lightimage textures/sfx/electric2.blend.tga
	q3map_globaltexture	
	q3map_surfacelight 200

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_trim/electricwall_blue.tga
		tcMod scale 0.5 0.5
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/base_trim/electricwall_blue.tga
		blendFunc GL_ONE GL_ONE
	}
	{
		map textures/sfx/electric2.blend.tga
		rgbGen wave sin 0.6 1 0 2
		tcMod scale 0.5 0.5
		tcMod rotate 54
		tcMod scroll 0.53 0.43
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 420.base_wall (bluemetalsupport2eye)
// ------------------------------------------------------------

textures/base_wall/bluemetalsupport2eye
{
	
        surfaceparm nomarks
	q3map_surfacelight 300
	q3map_lightimage textures/base_wall/bluemetalsupport2eyel.tga
	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetalsupport2eye.tga
		blendFunc blend	
		rgbGen identity
	}

	{
		map $lightmap
		blendfunc filter
		rgbGen identity
	}
        {
		map textures/base_wall/bluemetalsupport2eyel.tga
		blendFunc GL_ONE GL_ONE
		rgbgen wave triangle .5 .5 0 .5
	}
}

// ------------------------------------------------------------
// 421.base_wall (atechengine_ax)
// ------------------------------------------------------------

textures/base_wall/atechengine_ax
{       
      q3map_lightimage textures/base_wall/atechengine_ax2.tga
      q3map_surfacelight 300  
        {
		map textures/sfx/proto_zzztpink.tga
                tcmod rotate 60
                tcMod turb 0 .5 0 9.6
                tcmod scale 2 2
                tcmod scroll 9 5
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
        {
		map textures/base_wall/atechengine_ax.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 422.base_wall (concrete_ow)
// ------------------------------------------------------------

textures/base_wall/concrete_ow
{       
        
        {
		map textures/sfx/proto_zzztblu2.tga
                tcMod turb 0 .5 0 9.6
                tcmod scale 2 2
                tcmod scroll 9 5
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
        {
		map textures/base_wall/concrete_ow.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 423.base_wall (symbol_bluelt1)
// ------------------------------------------------------------

textures/base_wall/symbol_bluelt1   
{
	q3map_lightimage textures/base_wall/symbol_bluelt1_mask.tga
	q3map_surfacelight 100
	light 1

	{
		map $lightmap
		rgbGen identity
	}

	
	{
		map textures/base_wall/symbol_bluelt1.tga
		blendfunc filter
		rgbgen identity
	}
	
	{
		map textures/base_wall/symbol_bluelt1_mask.tga
		blendfunc add
		rgbgen wave sawtooth .2 .05 0 7
	}
}
	
// ------------------------------------------------------------
// 424.base_wall (metaltechgeomtrn)
// ------------------------------------------------------------

textures/base_wall/metaltechgeomtrn
{


        {
		map textures/sfx/fire_ctfblue.tga
                tcmod scroll 0 1
                tcMod turb 0 .25 0 1.6
                tcmod scale 2 2
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
        {
	        map textures/base_wall/metaltechgeomtrn.tga
                blendFunc blend
                tcmod rotate 30
                tcMod stretch sin .8 0.2 0 .2
	        rgbGen identity
	}
        {
	        map textures/base_wall/metaltechgeomtrn2.tga
		blendFunc blend
                tcmod rotate 20
                tcMod stretch sin .8 0.2 0 .1
	        rgbGen identity
	}
	{
	        map textures/base_wall/metaltechgeomtrn.tga
		blendFunc blend
	        rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 425.base_wall (green_con04)
// ------------------------------------------------------------

textures/base_wall/green_con04
{
     {
		map textures/sfx/specular.tga
                tcGen environment
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
	{
		map textures/base_wall/green_con04.tga
                blendfunc blend
                rgbGen identity
	}
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
	//{
	//	map $lightmap
	//	rgbgen identity      
	//}
	//
	//{
	//	map textures/base_wall/green_con04.tga
	//	blendFunc GL_DST_COLOR GL_SRC_ALPHA
	//	rgbGen identity
	//	alphaGen lightingSpecular
	//}
}

// ------------------------------------------------------------
// 426.base_wall (glass_frame)
// ------------------------------------------------------------

textures/base_wall/glass_frame
{
     cull none
     surfaceparm alphashadow

        {
		map textures/effects/tinfx2.tga
                blendFunc GL_ONE GL_ONE
                tcgen environment
                rgbGen identity
	}
	{
		map textures/base_wall/glass_frame.tga
                alphaFunc GE128
                depthWrite
		//rgbGen vertex
                //blendfunc blend	
	}
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
	}

}

// ------------------------------------------------------------
// 427.base_wall (metalfloor_wall_15ow)
// ------------------------------------------------------------

textures/base_wall/metalfloor_wall_15ow
{
        {
		map textures/base_wall/metalfloor_wall_15ow.tga
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
        }
	
        {
		map textures/sfx/hologirl.tga
                tcmod scale 2 .4
                tcmod scroll 6 .6
                //tcMod turb 0 .1 0 .01
                blendFunc GL_ONE GL_ONE
                rgbGen identity
                
	}
	{
		map textures/base_wall/metalfloor_wall_15ow.tga
                blendfunc blend
		rgbGen identity
	}
        {
		map $lightmap
		blendFunc filter
                //blendfunc dst_color gl_zero                                             
	}

}

// ------------------------------------------------------------
// 428.base_wall (atech1_alpha)
// ------------------------------------------------------------

textures/base_wall/atech1_alpha
{
	
        {
		map textures/sfx/hologirl.tga
                tcmod scale 2 .4
                tcmod scroll 6 .6
                //tcMod turb 0 .1 0 .01
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
	{
		map textures/base_wall/atech1_alpha.tga
                blendfunc blend
                rgbGen identity
	}
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
	}

}

// ------------------------------------------------------------
// 429.base_wall (concrete4)
// ------------------------------------------------------------

textures/base_wall/concrete4
{
	
        {
		map textures/liquids/protolava2.tga
                tcmod scale .2 .2
                tcmod scroll .04 .03
                tcMod turb 0 .1 0 .01
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
	{
		map textures/base_wall/concrete4.tga
                blendfunc blend
                rgbGen identity
		
	}
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
	}

}

// ------------------------------------------------------------
// 430.base_wall (protobanner_ow)
// ------------------------------------------------------------

textures/base_wall/protobanner_ow
{
     cull disable
     surfaceparm alphashadow
     surfaceparm trans	
     surfaceparm nomarks
     deformVertexes wave 100 sin 0 1 0 .1
     deformVertexes wave 100 sin 0 1 0 .2
     
        {
                map textures/base_wall/protobanner_ow.tga
                alphaFunc GE128
		depthWrite
		rgbGen vertex
        }
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
		depthFunc equal
	}


}

// ------------------------------------------------------------
// 431.base_wall (girders1i_yellofin)
// ------------------------------------------------------------

textures/base_wall/girders1i_yellofin
{
	surfaceparm	metalsteps	
    	surfaceparm trans	
	surfaceparm alphashadow
	surfaceparm playerclip
   	surfaceparm nonsolid
	surfaceparm nomarks	
	cull none
        nopicmip
	{
		map textures/base_wall/girders1i_yellodark_fin.tga
		blendFunc GL_ONE GL_ZERO
		alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 432.base_wall (protobanner)
// ------------------------------------------------------------

textures/base_wall/protobanner
{
     cull disable
     surfaceparm alphashadow
     surfaceparm trans	
     surfaceparm nomarks
     tessSize 64
     deformVertexes wave 30 sin 0 3 0 .2
     deformVertexes wave 100 sin 0 3 0 .7
     
        {
                map textures/base_wall/protobanner.tga
                alphaFunc GE128
		depthWrite
		rgbGen vertex
        }
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
		depthFunc equal
	}


}

// ------------------------------------------------------------
// 433.base_wall (protobanner_still)
// ------------------------------------------------------------

textures/base_wall/protobanner_still
{
     qer_editorimage textures/base_wall/protobanner.tga
     cull disable
     surfaceparm alphashadow
     surfaceparm trans	
     surfaceparm nomarks
     tessSize 64
     //deformVertexes wave 30 sin 0 3 0 .2
     //deformVertexes wave 100 sin 0 3 0 .7
     
        {
                map textures/base_wall/protobanner.tga
                alphaFunc GE128
		depthWrite
		rgbGen vertex
        }
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
		depthFunc equal
	}


}

// ------------------------------------------------------------
// 434.base_wall (medal_impressive)
// ------------------------------------------------------------

textures/base_wall/medal_impressive
{
	qer_editorimage textures/base_wall/medal_impressive.tga
        q3map_surfacelight 100
	
	
	{
		animMap .25 textures/base_wall/medal_impressive.tga textures/base_wall/medal_gauntlet.tga textures/base_wall/medal_excellent.tga textures/base_wall/medal_accuracy.tga textures/base_wall/medal_victory.tga textures/base_wall/medal_frags.tga
		rgbGen wave sawtooth 0 1 0 .25
	
	}

	{
		map textures/base_wall/comp3textd.tga
		blendfunc add
		tcmod scroll 5 .25
		rgbGen wave inversesawtooth 0 1 0 .25
	}

	{
		map textures/base_wall/chrome_env.tga
		tcgen environment
		tcmod scale .5 .5
		blendfunc add
		rgbgen wave triangle .5 .05 0 10
	}

	{
		map textures/base_wall/comp3frame.tga
		blendfunc blend
		rgbgen identity
	}

	{
		map $lightmap
	        rgbGen identity
		blendfunc gl_dst_color gl_zero
	}

	          		
} 

// ------------------------------------------------------------
// 435.base_wall (q3tourneyscreen)
// ------------------------------------------------------------

textures/base_wall/q3tourneyscreen
{
	qer_editorimage textures/base_wall/q3tourney1.tga
        q3map_surfacelight 100
	
	

	{
		animMap .25 textures/base_wall/q3tourney1.tga textures/base_wall/q3tourney2.tga textures/base_wall/q3tourney3.tga textures/base_wall/q3tourney4.tga textures/base_wall/q3tourney5.tga
		rgbGen wave sawtooth 0 1 0 .25
	
	}

	{
		map textures/base_wall/comp3textd.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 .25
		tcmod scroll 5 .25 
	}

	{
		map textures/base_wall/chrome_env.tga
		tcgen environment
		tcmod scale .5 .5
		blendfunc add
		rgbgen wave triangle .5 .05 0 10
	}

	{
		map textures/base_wall/comp3frame.tga
		blendfunc blend
		rgbgen identity
	}

	{
		map $lightmap
	        blendfunc gl_dst_color gl_zero
		rgbgen identity
	}
	          		
}     

// ------------------------------------------------------------
// 436.base_wall (main_q3abanner)
// ------------------------------------------------------------

textures/base_wall/main_q3abanner
{
	q3map_lightimage textures/base_wall/main_q3abanner.tga
        q3map_surfacelight 100


	{
		map textures/base_wall/main_q3abanner.tga
	        rgbGen wave square 0 1 0 .5
	}

	{
		map textures/base_wall/comp3text.tga
		blendfunc add
	        rgbGen identity
		tcmod scroll 3 3
	}

	{
		map textures/base_wall/comp3textb.tga
		blendfunc add
	        rgbGen identity
		tcmod scroll 3 3
	}


	{
		map $lightmap
	        rgbGen identity
		blendfunc gl_dst_color gl_zero
	}

	{
		map $lightmap
		tcgen environment
		tcmod scale .5 .5
	        rgbGen wave sin .25 0 0 0
		blendfunc add
	}	          		
}     

// ------------------------------------------------------------
// 437.base_wall (metalfloor_wall_14_specular)
// ------------------------------------------------------------

textures/base_wall/metalfloor_wall_14_specular
{
	qer_editorimage textures/base_wall/metalfloor_wall_14_specular.tga
	{
		map $lightmap
		rgbgen identity      
	}
	
	{
		map textures/base_wall/metalfloor_wall_14_specular.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
}

// ------------------------------------------------------------
// 438.base_wall (shinygrate1_4_glow)
// ------------------------------------------------------------

textures/base_wall/shinygrate1_4_glow
{
	q3map_lightimage textures/base_wall/shinygrate1_4_glow_light2.tga
	q3map_surfacelight 100
	q3map_lightsubdivide 32 
	
	{
		map textures/sfx/electricslime.tga
		rgbGen wave sin .5 .5 0 1
		tcmod scale .25 .25
		tcMod scroll -10 -1
	}
	
	
	{
		map textures/base_wall/shinygrate1_4_glow.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}


	{
		map $lightmap
    		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}

	{
		map textures/base_wall/shinygrate1_4_glow_light2.tga
		blendfunc gl_one gl_one
		rgbGen wave triangle .2 .01 0 10
	}

			
}

// ------------------------------------------------------------
// 439.base_wall (bluemetal1b_chrome)
// ------------------------------------------------------------

textures/base_wall/bluemetal1b_chrome
{
	qer_editorimage textures/base_wall/bluemetal1b.tga

	{
		map textures/base_wall/chrome_env2.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetal1b_shiny.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 440.base_wall (bluemetal1c)
// ------------------------------------------------------------

textures/base_wall/bluemetal1c
{
	

	{
		map textures/base_wall/chrome_env2.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetal1c.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 441.base_wall (bluemetalsupport_shiny)
// ------------------------------------------------------------

textures/base_wall/bluemetalsupport_shiny
{
	

	{
		map textures/base_wall/chrome_env2.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetalsupport_shiny.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 442.base_wall (grill)
// ------------------------------------------------------------

textures/base_wall/grill
{
	

	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/grill.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 443.base_wall (grill2)
// ------------------------------------------------------------

textures/base_wall/grill2
{
	//deformVertexes wave 100 sin 3 2 .1 3
        {
		rgbGen identity
		map $lightmap
	}
	{
		map textures/base_wall/grill2.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
        {
		map textures/effects/tinfx.tga
                tcgen environment
		blendFunc GL_ONE GL_ONE
		rgbGen identity
	}
       
        {
		map textures/base_wall/grill2.tga
		//blendFunc GL_ONE GL_ONE
                blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}       

// ------------------------------------------------------------
// 444.base_wall (bluemetalsupport2)
// ------------------------------------------------------------

textures/base_wall/bluemetalsupport2
{
	

	{
		map textures/base_wall/chrome_env2.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetalsupport2.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 445.base_wall (bluemetalsupport2c_1)
// ------------------------------------------------------------

textures/base_wall/bluemetalsupport2c_1
{
	

	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetalsupport2c_1.tga
		blendFunc blend	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 446.base_wall (bluemetalsupport2c_1vent)
// ------------------------------------------------------------

textures/base_wall/bluemetalsupport2c_1vent
{
	

	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetalsupport2c_1vent.tga
		blendFunc blend	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 447.base_wall (bluemetalsupport2c)
// ------------------------------------------------------------

textures/base_wall/bluemetalsupport2c
{
	

	{
		map textures/base_wall/chrome_env2.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetalsupport2c.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 448.base_wall (bluemetalsupport2clight)
// ------------------------------------------------------------

textures/base_wall/bluemetalsupport2clight
{
	surfaceparm nomarks
	q3map_surfacelight 500

	

	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetalsupport2clight.tga
		blendFunc blend	
		rgbGen identity
	}
	
	{
		map $lightmap
		blendfunc filter
		rgbGen identity
	}


	{
		map textures/base_wall/bluemetalsupport2clight.glow.tga
		blendFunc add
		rgbgen wave sawtooth .6 .1 0 4
	}
}

// ------------------------------------------------------------
// 449.base_wall (bluemetalsupport2f)
// ------------------------------------------------------------

textures/base_wall/bluemetalsupport2f
{
	

	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetalsupport2f.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA	
	}
	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 450.base_wall (bluemetalsupport2e)
// ------------------------------------------------------------

textures/base_wall/bluemetalsupport2e
{
	

	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetalsupport2e.tga
		blendFunc blend	
		rgbGen identity
	}

	{
		map $lightmap
		blendfunc filter
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 451.base_wall (bluemetalsupport2e2)
// ------------------------------------------------------------

textures/base_wall/bluemetalsupport2e2
{
	

	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetalsupport2e2.tga
		blendFunc blend	
		rgbGen identity
	}

	{
		map $lightmap
		blendfunc filter
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 452.base_wall (bluemetalsupport2fline)
// ------------------------------------------------------------

textures/base_wall/bluemetalsupport2fline
{
	q3map_surfacelight 100
	q3map_lightimage textures/base_wall/bluemetalsupport2fline_glow.tga

	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetalsupport2fline.tga
		blendFunc blend	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbGen identity
	}

	{
		map textures/base_wall/bluemetalsupport2fline_glow.tga
		blendFunc add	
		rgbGen wave sin .5 .5 0 1	
	}


}

// ------------------------------------------------------------
// 453.base_wall (bluemetalsupport2ftv)
// ------------------------------------------------------------

textures/base_wall/bluemetalsupport2ftv
{
	q3map_surfacelight 100
	q3map_lightimage textures/base_wall/bluemetalsupport2ftv_glow.tga

	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetalsupport2ftv.tga
		blendFunc blend	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbGen identity
	}

	{
		map textures/base_wall/bluemetalsupport2ftv_glow.tga
		blendFunc add	
		rgbGen wave sawtooth .4 .05 0 10
	}


}

// ------------------------------------------------------------
// 454.base_wall (bluemetalsupport2d_1)
// ------------------------------------------------------------

textures/base_wall/bluemetalsupport2d_1
{
	

	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetalsupport2d_1.tga
		blendFunc blend	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 455.base_wall (shinymetaldoor_outside3a2)
// ------------------------------------------------------------

textures/base_door/shinymetaldoor_outside3a2
{

	{
		map textures/base_wall/chrome_env.tga
                tcGen environment
		tcmod scale .25 .25 
		rgbGen identity   
	}

		
	{
		map textures/base_door/shinymetaldoor_outside3a2.tga
		blendFunc blend
		rgbGen identity
	}


	{
		map $lightmap
		rgbgen identity
		blendFunc filter
	}

	{
		map textures/base_door/shinymetaldoor_outside3glow.tga
		blendFunc add
		rgbGen wave sin .9 .1 0 5
	}
		
} 

// ------------------------------------------------------------
// 456.base_wall (shinymetaldoor_outside3a)
// ------------------------------------------------------------

textures/base_door/shinymetaldoor_outside3a
{

	{
		map textures/base_wall/chrome_env.tga
                tcGen environment
		tcmod scale .25 .25 
		rgbGen identity   
	}

		
	{
		map textures/base_door/shinymetaldoor_outside3a.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}


	{
		map $lightmap
		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}	
		
} 

// ------------------------------------------------------------
// 457.base_wall (shinymetaldoor_outside3b)
// ------------------------------------------------------------

textures/base_door/shinymetaldoor_outside3b
{

	{
		map textures/base_wall/chrome_env.tga
                tcGen environment
		tcmod scale .25 .25 
		rgbGen identity   
	}

		
	{
		map textures/base_door/shinymetaldoor_outside3b.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}


	{
		map $lightmap
		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}	
		
} 

// ------------------------------------------------------------
// 458.base_wall (shinymetaldoor_outside)
// ------------------------------------------------------------

textures/base_door/shinymetaldoor_outside
{

	{
		map textures/base_wall/chrome_env.tga
                tcGen environment
		tcmod scale .25 .25 
		rgbGen identity   
	}

		
	{
		map textures/base_door/shinymetaldoor_outside.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}

	{
		map textures/base_door/quake3sign_outside.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}


	{
		map $lightmap
		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}	
		
} 

// ------------------------------------------------------------
// 459.base_wall (steedmetal2)
// ------------------------------------------------------------

textures/base_wall/steedmetal2
{

	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/steedmetal2.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}

			
}

// ------------------------------------------------------------
// 460.base_wall (steed1gf)
// ------------------------------------------------------------

textures/base_wall/steed1gf
{
	{
		map $lightmap
		rgbgen identity      
	}
	
	{
		map textures/base_wall/steed1gf.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}

			
}

// ------------------------------------------------------------
// 461.base_wall (steed1gd)
// ------------------------------------------------------------

textures/base_wall/steed1gd
{
	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/steed1gd.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}

			
}

// ------------------------------------------------------------
// 462.base_wall (bluemetal1b_shiny)
// ------------------------------------------------------------

textures/base_wall/bluemetal1b_shiny
{
	{
		map $lightmap
		rgbgen identity      
	}
	
	{
		map textures/base_wall/bluemetal1b_shiny.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}

			
}

// ------------------------------------------------------------
// 463.base_wall (bluemetal2_shiny)
// ------------------------------------------------------------

textures/base_wall/bluemetal2_shiny
{
	{
		map $lightmap
		rgbgen identity      
	}
	
	{
		map textures/base_wall/bluemetal2_shiny.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}		
}

// ------------------------------------------------------------
// 464.base_wall (bluemetal2_shiny_trans)
// ------------------------------------------------------------

textures/base_wall/bluemetal2_shiny_trans
{
	qer_editorimage textures/base_wall/bluemetal2_shiny.tga
	surfaceparm nonsolid
	{
		map $lightmap
		rgbgen identity      
	}
	
	{
		map textures/base_wall/bluemetal2_shiny.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}		
}

// ------------------------------------------------------------
// 465.base_wall (bluemetal2_noisy)
// ------------------------------------------------------------

textures/base_wall/bluemetal2_noisy
{
	qer_editorimage textures/base_wall/bluemetal2_shiny
	surfaceparm metalsteps
	{
		map $lightmap
		rgbgen identity      
	}
	
	{
		map textures/base_wall/bluemetal2_shiny.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}			
}

// ------------------------------------------------------------
// 466.base_wall (train1)
// ------------------------------------------------------------

textures/base_wall/train1
{

	{
		map textures/base_wall/chrome_env.tga
                tcGen environment
		tcmod scale .25 .1 
		rgbGen identity   
	}

		
	{
		map textures/base_wall/train1.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}

	{
		map $lightmap
		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}	
		
} 

// ------------------------------------------------------------
// 467.base_wall (bloodstain)
// ------------------------------------------------------------

textures/base_wall/bloodstain
{
	{
		map $lightmap
		rgbgen identity      
	}
	
	{
		map textures/base_wall/bloodstain.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}
			
}

// ------------------------------------------------------------
// 468.base_wall (shinymetaldoor)
// ------------------------------------------------------------

textures/base_door/shinymetaldoor
{

	{
		//map $lightmap
		map textures/base_wall/chrome_env.tga
                tcGen environment
		tcmod scale .25 .25 
		rgbGen identity   
	}

		
	{
		map textures/base_door/shinymetaldoor.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}

	{
		map textures/base_door/quake3sign.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}


	{
		map $lightmap
		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}	
		
}

// ------------------------------------------------------------
// 469.base_wall (dooreye)
// ------------------------------------------------------------

textures/base_wall/dooreye
{
	surfaceparm nomarks
	q3map_surfacelight 300
	q3map_lightimage textures/base_wall/dooreyelight.tga

	
	{
		map $lightmap
		rgbGen identity
	}

	{
		map textures/base_wall/dooreye.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}

	{
		map textures/base_wall/dooreyelight.tga
		blendFunc GL_ONE GL_ONE
		rgbgen wave triangle .5 .5 0 .5
	}


}

// ------------------------------------------------------------
// 470.base_wall (dooreye2)
// ------------------------------------------------------------

textures/base_wall/dooreye2
{
	surfaceparm nomarks
	q3map_surfacelight 300
	q3map_lightimage textures/base_wall/dooreyelight.tga

	
	{
		map $lightmap
		rgbGen identity
	}

	{
		map textures/base_wall/dooreye2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}

	{
		map textures/base_wall/dooreyelight.tga
		blendFunc GL_ONE GL_ONE
		rgbgen wave triangle .5 .5 0 .5
	}


}

// ------------------------------------------------------------
// 471.base_wall (dooreye3)
// ------------------------------------------------------------

textures/base_wall/dooreye3
{
	surfaceparm nomarks
	q3map_surfacelight 300
	q3map_lightimage textures/base_wall/dooreyelight.tga

	
	{
		map $lightmap
		rgbGen identity
	}

	{
		map textures/base_wall/dooreye3.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}

	{
		map textures/base_wall/dooreyelight.tga
		blendFunc GL_ONE GL_ONE
		rgbgen wave triangle .5 .5 0 .5
	}


}

// ------------------------------------------------------------
// 472.base_wall (glass01)
// ------------------------------------------------------------

textures/base_wall/glass01
{
        surfaceparm trans	
	cull none
	qer_trans 	0.5
     
        {
		map textures/effects/tinfx.tga
                tcgen environment
		blendFunc GL_ONE GL_ONE
		rgbGen identity
	}
        {
		map $lightmap
		rgbGen identity
		blendFunc filter
	}
           
}

// ------------------------------------------------------------
// 473.base_wall (patch10_beat4_drip)
// ------------------------------------------------------------

textures/base_wall/patch10_beat4_drip
{
	qer_editorimage textures/base_wall/patch10_beatup4_stain.tga
	
	{
		map $lightmap
		rgbgen identity      
	}
	
	{
		map textures/base_wall/waterdrip.tga
		blendFunc GL_ONE gl_src_alpha
		tcmod scroll 0 -.001
	}

	{
		map textures/base_wall/patch10_beatup4_stain.tga
		blendFunc GL_ONE GL_SRC_ALPHA
		rgbGen identity
	}

	{
		map $lightmap
		rgbgen identity 
		blendfunc gl_dst_color gl_zero     
	}

			
}

// ------------------------------------------------------------
// 474.base_wall (patch10_beatup4_shiny2)
// ------------------------------------------------------------

textures/base_wall/patch10_beatup4_shiny2
{
	
	{
		map $lightmap
		rgbgen identity      
	}
	
	{
		map textures/base_wall/patch10_beatup4_shiny2.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}

			
}

// ------------------------------------------------------------
// 475.base_wall (patch10shiny)
// ------------------------------------------------------------

textures/base_wall/patch10shiny
{
	qer_editorimage textures/base_wall/patch10_beatup4_alpha.tga
	{
		map $lightmap
		rgbgen identity      
	}
	
	{
		map textures/base_wall/patch10_beatup4_alpha.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}

			
}

// ------------------------------------------------------------
// 476.base_wall (patch10shiny_trans)
// ------------------------------------------------------------

textures/base_wall/patch10shiny_trans
{
	qer_editorimage textures/base_wall/patch10_beatup4_alpha.tga
	surfaceparm nonsolid
	{
		map $lightmap
		rgbgen identity      
	}
	
	{
		map textures/base_wall/patch10_beatup4_alpha.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}

			
}

// ------------------------------------------------------------
// 477.base_wall (patch11_shiny)
// ------------------------------------------------------------

textures/base_wall/patch11_shiny
{
	qer_editorimage textures/base_wall/patch11_shiny.tga
	{
		map $lightmap
		rgbgen identity      
	}
	
	{
		map textures/base_wall/patch11_shiny.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}

			
}

// ------------------------------------------------------------
// 478.base_wall (bluecover)
// ------------------------------------------------------------

textures/base_wall/bluecover
{

	{
		map $lightmap	
		tcgen environment
		tcmod scale .25 .25 
		rgbgen identity      
	}

		
	{
		map textures/base_wall/bluecover.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
		//alphaGen lightingSpecular
	}

	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbgen identity      
	}

			
}

// ------------------------------------------------------------
// 479.base_wall (shinybluemetal1b)
// ------------------------------------------------------------

textures/base_wall/shinybluemetal1b
{

	
	{
		map textures/base_wall/chrome_env.tga
		tcgen environment
		tcmod scale .25 .25 
		rgbgen identity      
	}

	{
		map textures/base_wall/shinybluemetal1b.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
		//alphaGen lightingSpecular
	}
	
	{	
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbgen identity      
	}


				
}

// ------------------------------------------------------------
// 480.base_wall (pjrock18)
// ------------------------------------------------------------

textures/base_wall/pjrock18
{
	qer_editorimage textures/stone/pjrock18.tga
	{
		map $lightmap
		tcgen environment
		tcmod scale .25 .25
		rgbgen identity      
	}
		
	
	{
		map textures/stone/pjrock18.tga
		blendFunc GL_ONE GL_SRC_ALPHA
	//	alphaGen lightingSpecular
		rgbGen identity
	}


	{
		map $lightmap
    		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}

			
}

// ------------------------------------------------------------
// 481.base_wall (shinybluemetal1)
// ------------------------------------------------------------

textures/base_wall/shinybluemetal1
{

	{
		map $lightmap
		rgbgen identity      
	}
	
	{
		map textures/base_wall/shinybluemetal1.tga
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
		rgbGen identity
		alphaGen lightingSpecular
	}

			
}

// ------------------------------------------------------------
// 482.base_wall (shinygrate1_4)
// ------------------------------------------------------------

textures/base_wall/shinygrate1_4
{

	{
		map $lightmap
		tcgen environment
		tcmod scale .25 .25
		rgbgen identity      
	}
		
	
	{
		map textures/base_wall/shinygrate1_4.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}


	{
		map $lightmap
    		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}

			
}

// ------------------------------------------------------------
// 483.base_wall (chrome_metal)
// ------------------------------------------------------------

textures/base_wall/chrome_metal
{
	{
		map textures/base_wall/chrome_env.tga
		tcGen environment
		tcmod scale .25 .25 
		
	}

	{
		map textures/base_wall/chrome_metal.tga
		blendFunc GL_ONE GL_ONE
		detail
        	tcMod scale 0.0693 0.0712
		rgbGen identity
	}


	{
		map $lightmap
                rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	   
	}

	
			
}

// ------------------------------------------------------------
// 484.base_wall (blupaintedmetalcomp)
// ------------------------------------------------------------

textures/base_wall/blupaintedmetalcomp
{

	{
		map $lightmap
                tcGen environment
		tcmod scale .25 .25    
	}
		
	{
		map textures/base_wall/blupaintedmetalcomp.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}

	{
		map $lightmap
		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}		
}

// ------------------------------------------------------------
// 485.base_wall (blupaintedmetal)
// ------------------------------------------------------------

textures/base_wall/blupaintedmetal
{

	{
		map $lightmap
                tcGen environment
		tcmod scale .25 .25    
	}
		
	{
		map textures/base_wall/blupaintedmetal.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA
		rgbGen identity
	}

	{
		map $lightmap
		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}		
}

// ------------------------------------------------------------
// 486.base_wall (shinymetal2)
// ------------------------------------------------------------
       
textures/base_wall/shinymetal2
{

	{
		map $lightmap
                tcGen environment
		tcmod scale .25 .25    
	}

	{
		map $lightmap
		rgbgen identity
		blendFunc GL_ONE GL_SRC_COLOR
	}

		
	{
		map textures/base_wall/shinymetal2.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
		
		
} 

// ------------------------------------------------------------
// 487.base_wall (comp3c)
// ------------------------------------------------------------

textures/base_wall/comp3c
{
        {
		map textures/base_wall/comp3env.tga
	        rgbGen identity
		tcGen environment
	}
	      
        {
		map $lightmap
                tcGen environment
                blendFunc GL_ONE GL_ONE
	}

	{
		map textures/base_wall/comp3c.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}

	{
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}

}
      
// ------------------------------------------------------------
// 488.base_wall (comp3b)
// ------------------------------------------------------------

textures/base_wall/comp3b
{
	q3map_lightimage textures/base_wall/comp3textb.tga
        q3map_surfacelight 1000
        {
		map textures/base_wall/comp3textb.tga
	        rgbGen identity
		tcmod scroll 3 3
	}
	      
        {
		map textures/base_wall/comp3env.tga
                tcGen environment
                blendFunc GL_ONE GL_ONE
                rgbGen wave sin .98 .02 0 5
	}
	
	{
		map $lightmap
                tcGen environment
                blendFunc GL_DST_COLOR GL_ONE
	}


	{
		map textures/base_wall/comp3b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
       
	{
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 489.base_wall (comp3b_dark)
// ------------------------------------------------------------
 
textures/base_wall/comp3b_dark
{
	q3map_lightimage textures/base_wall/comp3textb.tga
        q3map_surfacelight 1
        {
		map textures/base_wall/comp3textb.tga
	        rgbGen identity
		tcmod scroll 3 3
	}
	      
        {
		map textures/base_wall/comp3env.tga
                tcGen environment
                blendFunc GL_ONE GL_ONE
                rgbGen wave sin .98 .02 0 5
	}
	
	{
		map $lightmap
                tcGen environment
                blendFunc GL_DST_COLOR GL_ONE
	}


	{
		map textures/base_wall/comp3b.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
       
	{
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 490.base_wall (comp3)
// ------------------------------------------------------------
     
textures/base_wall/comp3
{
	q3map_lightimage textures/base_wall/comp3env.tga
        q3map_surfacelight 1000
        {
		map textures/base_wall/comp3text.tga
	        rgbGen identity
		tcmod scroll 3 1
	}
	      
        {
		map textures/base_wall/comp3env.tga
                tcGen environment
                blendFunc GL_ONE GL_ONE
                rgbGen wave sin .98 .02 0 5
	}
	
	{
		map $lightmap
                tcGen environment
                blendFunc GL_DST_COLOR GL_ONE
	}


	{
		map textures/base_wall/comp3.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
       
	{
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
	
}       

// ------------------------------------------------------------
// 491.base_wall (basewall01bit)
// ------------------------------------------------------------

textures/base_wall/basewall01bit
{
	q3map_lightimage textures/base_wall/basewall01bitfx.tga
        q3map_surfacelight 300
        {
		map textures/base_wall/basewall01bit.tga
	        rgbGen identity
	}
	      
	{
		map textures/base_wall/basewall01bitfx.tga
		blendfunc GL_ONE GL_ONE
                tcmod scroll 3.3 1
                rgbGen wave sin .5 0.5 0 5
	}
        {
		map textures/effects/envmap2.tga
                tcGen environment
                tcmod scale 4 4
                blendFunc GL_ONE GL_ONE
               rgbGen identity
	}
	{
		map textures/base_wall/basewall01bit.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	        rgbGen identity
	}
       
	{
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
	
       
}

// ------------------------------------------------------------
// 492.base_wall (basewall01_ow)
// ------------------------------------------------------------

textures/base_wall/basewall01_ow
{
	q3map_lightimage textures/base_wall/basewall01_owfx.tga
	
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_wall/basewall01_ow.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{	
		map textures/base_wall/basewall01_owfx.tga
		blendfunc GL_ONE GL_ONE
                rgbGen wave sin .5 0.5 0 .5
	}
       
}

// ------------------------------------------------------------
// 493.base_wall (blumtlspt2b)
// ------------------------------------------------------------

textures/base_wall/blumtlspt2b
{
	
	q3map_surfacelight 100
	q3map_lightimage textures/base_wall/blumtlspt2bglow.tga


	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/blumtlspt2b.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}


	{
		map textures/base_wall/bluemetalsupport2bglow.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sin 0.5 0.5 0 .2
	}
}

// ------------------------------------------------------------
// 494.base_wall (bluemetalsupport2b)
// ------------------------------------------------------------

textures/base_wall/bluemetalsupport2b
{
	
	q3map_surfacelight 100
	q3map_lightimage textures/base_wall/bluemetalsupport2bglow.tga


	{
		map textures/base_wall/chrome_env.tga
	        rgbGen identity
		tcGen environment
		tcmod scale .25 .25
	}
	
	{
		map textures/base_wall/bluemetalsupport2b.tga
		blendFunc GL_ONE_MINUS_SRC_ALPHA GL_SRC_ALPHA	
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}


	{
		map textures/base_wall/bluemetalsupport2bglow.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sin 0.5 0.5 0 .2
	}
}

// ------------------------------------------------------------
// 495.base_wall (kcgrate1)
// ------------------------------------------------------------

textures/base_wall/kcgrate1
{		
	cull disable

	// A GRATE OR GRILL THAT CAN BE SEEN FROM BOTH SIDES
	{
		map textures/base_wall/kcgrate1.tga
		tcMod scale 2 2
		blendFunc GL_ONE GL_ZERO
		// alphaFunc GT0
                   alphaFunc GE128
		depthWrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}

// ------------------------------------------------------------
// 496.base_wall (future9light)
// ------------------------------------------------------------

textures/base_wall/future9light
{
	q3map_lightimage textures/base_wall/future9light.color.tga
	q3map_surfacelight 400	
	light 1
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_wall/future9light.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/base_wall/future9light.blend.tga
		rgbGen wave sin 0.5 0.1 1 0.5
		blendFunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 497.base_wall (bluemetal1light)
// ------------------------------------------------------------

textures/base_wall/bluemetal1light
{
	//surfaceparm nomarks
	q3map_surfacelight 1000
	q3map_lightimage textures/base_wall/bluemetal1light_bl.tga
	light 1
	// blue light in a metal wall
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures//base_wall/bluemetal1light.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/base_wall/bluemetal1light_bl.tga
		rgbGen wave sin 0.5 0.1 1 0.5
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 498.base_wall (green_contrim_noisey)
// Paul Jaquays uses this on the tops of metal frameworks
// ------------------------------------------------------------

textures/base_wall/green_contrim_noisey
{
	qer_editorimage textures/base_wall/green_contrim.tga
	surfaceparm	metalsteps		

	// metal that makes metalsteps sound
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/base_wall/green_contrim.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

// ------------------------------------------------------------
// 499.base_wall (c_met5_)
// ------------------------------------------------------------

textures/base_wall/c_met5_
{
	{
		map $lightmap 
		tcGen lightmap 
	}
	{
		map textures/base_wall/c_met5_2.tga
	}
	{
		map textures/detail/d_conc.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 4 4
		detail
	}
}

// ------------------------------------------------------------
// 500.base_wall (baslt4_1_2k)
// ------------------------------------------------------------

textures/base_light/baslt4_1_2k
{
	qer_editorimage textures/base_light/baslt4_1_2k.jpg
	surfaceparm nomarks
	surfaceparm nodlight
	q3map_surfacelight 2000
	cull disable
	{	
		map textures/base_light/baslt4_1_2k.jpg
	}
}

// ------------------------------------------------------------
// 501.base_wall (baslt4_1_4k)
// ------------------------------------------------------------

textures/base_light/baslt4_1_4k
{
	qer_editorimage textures/base_light/baslt4_1_4k.jpg
	surfaceparm nomarks
	surfaceparm nodlight
	q3map_surfacelight 4000
	cull disable
	{	
		map textures/base_light/baslt4_1_4k.jpg
	}
}

// ------------------------------------------------------------
// 502.base_wall (proto_lightmap)
// ------------------------------------------------------------

textures/base_light/proto_lightmap
{
	qer_editorimage textures/base_light/proto_lightmap.jpg
	surfaceparm nomarks
	surfaceparm nodlight
	q3map_surfacelight 4000
	cull disable
	{	
		map textures/base_light/proto_lightmap.jpg
	}
}