// ------------------------------------------------------------
// Shaders for Q3Rally CTF - rewritten by P3rlE
// based on the ctf.shader
// ------------------------------------------------------------

// ------------------------------------------------------------
// 1.sprites (friend)
// ------------------------------------------------------------

sprites/friend
{
	nomipmaps
	nopicmip
	{
		map sprites/friend1.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}



// ------------------------------------------------------------
// 2.sprites (foe)
// ------------------------------------------------------------

sprites/foe
{
	nomipmaps
	nopicmip
	{
		map sprites/foe2.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

// ------------------------------------------------------------
// 3.flags (b_flag)
// ------------------------------------------------------------

models/flags/b_flag
{
	cull disable
	deformVertexes wave 80 sin 2 8 0 3 
	deformVertexes wave 20 square 0 2 0.5 0.2 
	{
		map textures/sfx/proto_zzztblu3.tga
		rgbGen identity
		tcMod turb 1 1 0.5 0.3
		tcGen environment 
	}
	{
		map models/flags/b_flag.tga
		rgbGen lightingDiffuse
		alphaFunc GE128
	}
	{
		map textures/effects/flagenv.tga
		blendfunc gl_dst_color gl_src_color
		tcGen environment 
	}
}

// ------------------------------------------------------------
// 4.flags (pole)
// ------------------------------------------------------------

models/flags/pole
{
	{
		map textures/base_wall/chrome_env.tga
		rgbGen lightingDiffuse
		tcMod scale 0.5 0.5
		tcGen environment 
	}
	{
		map models/flags/pole.tga
		blendfunc filter
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 5.flags (r_flag)
// ------------------------------------------------------------

models/flags/r_flag
{
	cull disable
	deformVertexes wave 80 sin 2 8 0 3 
	deformVertexes wave 20 square 0 2 0.5 0.2 
	{
		map textures/sfx/proto_zzzt.tga
		rgbGen identity
		tcMod turb 1 1 0.5 0.3
		tcGen environment 
	}
	{
		map models/flags/r_flag.tga
		rgbGen lightingDiffuse
		alphaFunc GE128
	}
	{
		map textures/effects/flagenv.tga
		blendfunc gl_dst_color gl_src_color
		tcGen environment 
	}
}

// ------------------------------------------------------------
// 6.ctf (blue_telep)
// ------------------------------------------------------------

textures/ctf/blue_telep
{ 
        cull disable	
        surfaceparm nomarks
        surfaceparm trans
        sort additive	

        {
	        
	        clampmap textures/ctf/blue_telep.tga
		blendFunc add
                //depthWrite
                //tcMod stretch sin .9 0.4 0 .5
                tcmod rotate 521
                rgbGen identity
	}
        {
	        clampmap textures/ctf/blue_telep2.tga
		blendFunc add
                //depthWrite
                //tcMod stretch sin .9 0.1 0 .1
                tcmod rotate -111
                rgbGen identity
        }
        {
	        clampmap textures/ctf/telep.tga
		alphaFunc GE128
                depthWrite
                tcmod rotate 20
	        rgbGen identity
	}
        {
	        clampmap textures/ctf/telep.tga
		alphaFunc GE128
                depthWrite
                tcMod stretch sin .7 0 0 0
                tcmod rotate -20
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
// 7.ctf (red_telep)
// ------------------------------------------------------------

textures/ctf/red_telep
{ 
        cull disable	
        surfaceparm nomarks
        surfaceparm trans

        {
	        
	        clampmap textures/ctf/red_telep.tga
		blendFunc add
                //tcMod stretch sin .9 0.4 0 .5
                tcmod rotate 521
                rgbGen identity
	}
        {
	        clampmap textures/ctf/red_telep2.tga
		blendFunc add
                //tcMod stretch sin .9 0.1 0 .1
                tcmod rotate -111
                rgbGen identity
        }
        {
	        clampmap textures/ctf/telep.tga
		alphaFunc GE128
                tcmod rotate 20
	        rgbGen identity
	}
        {
	        clampmap textures/ctf/telep.tga
		alphaFunc GE128
                tcMod stretch sin .7 0 0 0
                tcmod rotate -20
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
// 8.ctf (test2_trans)
// ------------------------------------------------------------

textures/ctf/test2_trans
{
	qer_editorimage textures/ctf/test2.tga
	surfaceparm nonsolid
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/ctf/test2.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}

}

// ------------------------------------------------------------
// 9.ctf (test2_r_trans)
// ------------------------------------------------------------

textures/ctf/test2_r_trans
{
	qer_editorimage textures/ctf/test2_r.tga
	surfaceparm nonsolid
	{
		map $lightmap
		rgbGen identity
	
	}
	{
		map textures/ctf/test2_r.tga
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

// ------------------------------------------------------------
// 10.ctf (killblockctf_r)
// ------------------------------------------------------------

textures/ctf/killblockctf_r
{
   qer_editorimage   textures/gothic_block/blocks15cgeomtrn.tga

        {
		map textures/sfx/fire_ctfred.tga
                tcmod scroll 0 1
                tcMod turb 0 .25 0 1.6
                tcmod scale 2 2
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
        {
	        map textures/gothic_block/blocks18cgeomtrn2.tga
		//blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
                blendFunc blend
                tcmod rotate 30
                tcMod stretch sin .8 0.2 0 .2
	        rgbGen identity
	}
        {
	        map textures/gothic_block/blocks18cgeomtrn2.tga
		blendFunc blend
                tcmod rotate 20
                tcMod stretch sin .8 0.2 0 .1
	        rgbGen identity
	}
	{
	        map textures/gothic_block/blocks15cgeomtrn.tga
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
// 11.ctf (killblockctf_b)
// ------------------------------------------------------------

textures/ctf/killblockctf_b
{
   qer_editorimage   textures/gothic_block/killblockgeomtrn.tga

        {
		map textures/sfx/fire_ctfblue.tga
                tcmod scroll 0 1
                tcMod turb 0 .25 0 1.6
                tcmod scale 2 2
                blendFunc GL_ONE GL_ZERO
                rgbGen identity
	}
        {
	        map textures/gothic_block/blocks18cgeomtrn2.tga
		//blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
                blendFunc blend
                tcmod rotate 30
                tcMod stretch sin .8 0.2 0 .2
	        rgbGen identity
	}
        {
	        map textures/gothic_block/blocks18cgeomtrn2.tga
		blendFunc blend
                tcmod rotate 20
                tcMod stretch sin .8 0.2 0 .1
	        rgbGen identity
	}
	{
	        map textures/gothic_block/killblockgeomtrn.tga
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
// 12.ctf (ctf_arch_b_shiny)
// ------------------------------------------------------------

textures/ctf/ctf_arch_b_shiny
{      
     qer_editorimage textures/ctf/ctf_arch_b.tga
	   {
		map textures/ctf/ctf_arch_b.tga
                blendFunc GL_ONE GL_ZERO
		rgbGen identity
	} 
        {
                map textures/sfx/specular.tga
                blendFunc GL_ONE GL_ONE
                tcGen environment
                tcmod scale 10 10
                rgbGen identity
	}  
        {
		map textures/ctf/ctf_arch_b.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}
         
}

// ------------------------------------------------------------
// 13.ctf (ctf_arch_r_shiny)
// ------------------------------------------------------------

textures/ctf/ctf_arch_r_shiny
{
	qer_editorimage textures/ctf/ctf_arch_r.tga      
        {
		map textures/ctf/ctf_arch_r.tga
                blendFunc GL_ONE GL_ZERO
		rgbGen identity
	} 
        {
                map textures/sfx/specular.tga
                blendFunc GL_ONE GL_ONE
                tcGen environment
                tcmod scale 10 10
                rgbGen identity
	}  
        {
		map textures/ctf/ctf_arch_r.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbGen identity
	}
         
}

// ------------------------------------------------------------
// 14.ctf (door02_bred2_shiny)
// ------------------------------------------------------------

textures/gothic_door/door02_bred2_shiny
{              
qer_editorimage textures/gothic_door/door02_bred2.tga 
	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/gothic_door/door02_bred2.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 15.ctf (door02_eblue2_shiny)
// ------------------------------------------------------------

textures/gothic_door/door02_eblue2_shiny
{              
qer_editorimage textures/gothic_door/door02_eblue2.tga

	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/gothic_door/door02_eblue2.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 16.ctf (red_scary4_d3fin)
// ------------------------------------------------------------

textures/ctf/red_scary4_d3fin
{              

	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/ctf/red_scary4_d3fin.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 17.ctf (ctf_tower_bluefin_shiny)
// ------------------------------------------------------------

textures/ctf/ctf_tower_bluefin_shiny
{              
qer_editorimage textures/ctf/ctf_tower_bluefin.tga 
	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/ctf/ctf_tower_bluefin.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 18.ctf (ctf_tower_redfin_shiny)
// ------------------------------------------------------------

textures/ctf/ctf_tower_redfin_shiny
{              
qer_editorimage textures/ctf/ctf_tower_redfin.tga 
	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/ctf/ctf_tower_redfin.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 19.ctf (metalbase09_bluekillblock_shiny)
// ------------------------------------------------------------

textures/ctf/metalbase09_bluekillblock_shiny
{              
qer_editorimage textures/ctf/metalbase09_bluekillblock.tga 
	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/ctf/metalbase09_bluekillblock.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 20.ctf (red_scary4_cfin)
// ------------------------------------------------------------

textures/ctf/red_scary4_cfin
{              

	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/ctf/red_scary4_cfin.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 21.ctf (red_scary4_csansfacefin)
// ------------------------------------------------------------

textures/ctf/red_scary4_csansfacefin
{              

	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/ctf/red_scary4_csansfacefin.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 22.ctf (tallblue_2_fin)
// ------------------------------------------------------------

textures/ctf/tallblue_2_fin
{              

	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/ctf/tallblue_2_fin.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 23.ctf (tallblue_1L_fin_shiny)
// ------------------------------------------------------------

textures/ctf/tallblue_1L_fin_shiny
{              
qer_editorimage textures/ctf/tallblue_1L_fin.tga 
	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/ctf/tallblue_1R_fin.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 24.ctf (tallblue_1R_fin_shiny)
// ------------------------------------------------------------

textures/ctf/tallblue_1R_fin_shiny
{              
qer_editorimage textures/ctf/tallblue_1R_fin.tga 
	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/ctf/tallblue_1R_fin.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 25.ctf (tallblue_2b_fin)
// ------------------------------------------------------------

textures/ctf/tallblue_2b_fin
{              

	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/ctf/tallblue_2b_fin.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 26.ctf (tallblue_4c_fin_shiny)
// ------------------------------------------------------------

textures/ctf/tallblue_4c_fin_shiny
{              
qer_editorimage textures/ctf/tallblue_4c_fin.tga 

	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/ctf/tallblue_4c_fin.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 27.ctf (tallblue_4bsmall_fin)
// ------------------------------------------------------------

textures/ctf/tallblue_4bsmall_fin
{              

	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/ctf/tallblue_4bsmall_fin.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 28.ctf (tallblue_5esmallb_fin)
// ------------------------------------------------------------

textures/ctf/tallblue_5esmallb_fin
{              

	{
		map $lightmap
		rgbGen identity
	}


        {
		map textures/ctf/tallblue_5esmallb_fin.tga
                blendFunc GL_dst_color GL_SRC_ALPHA
		alphagen lightingspecular
		rgbGen identity
	}

}

// ------------------------------------------------------------
// 29.ctf (ctf_blueflag)
// ------------------------------------------------------------

textures/ctf/ctf_blueflag
{
        tessSize 64
        deformVertexes wave 194 sin 0 0.1 0 .2
        deformVertexes normal .1 .2
        surfaceparm nomarks
        cull none
        
        {
		map textures/ctf/ctf_blueflag.tga
		rgbGen identity
	}
        {
		map textures/effects/redflagmap2.tga
                tcGen environment
                tcmod scroll .1 .2
                
                blendFunc GL_ONE GL_ONE
                rgbGen identity
	}
        {
		map textures/ctf/ctf_blueflag.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
        {
        	map textures/sfx/shadow.tga
                tcGen environment 
                //blendFunc GL_ONE GL_ONE            
                blendFunc GL_DST_COLOR GL_ZERO
               rgbGen identity
	}
}

// ------------------------------------------------------------
// 30.ctf (ctf_redflag)
// ------------------------------------------------------------

textures/ctf/ctf_redflag
{
        tessSize 64
        deformVertexes wave 194 sin 0 0.1 0 .2
        deformVertexes normal .1 .2
        surfaceparm nomarks
        cull none

        {
		map textures/ctf/ctf_redflag.tga
		rgbGen identity
	}
        {
		map textures/effects/blueflagmap2.tga
                 tcGen environment
                tcmod scroll .1 .2
                
                blendFunc GL_ONE GL_ONE
                rgbGen identity
	}
        {
		map textures/ctf/ctf_redflag.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
	}
        {
		map $lightmap
                blendFunc GL_DST_COLOR GL_ONE_MINUS_DST_ALPHA
		rgbGen identity
	}
        {

        	map textures/sfx/shadow.tga
                tcGen environment 
                //blendFunc GL_ONE GL_ONE            
                blendFunc GL_DST_COLOR GL_ZERO
               rgbGen identity
	}
}

// ------------------------------------------------------------
// 31.powerups (redflag)
// ------------------------------------------------------------

powerups/redflag
{
	deformVertexes wave 100 sin 3 0 0 0
	{
		map textures/effects/redflagmap.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
		tcMod turb 0 0.2 0 1
	}
}

// ------------------------------------------------------------
// 32.powerups (blueflag)
// ------------------------------------------------------------

powerups/blueflag
{
	deformVertexes wave 100 sin 3 0 0 0
	{
		map textures/effects/blueflagmap.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
		tcMod turb 0 0.2 0 1
	}
}

// ------------------------------------------------------------
// 33.ctf (q3r_bluelong_decal)
// ------------------------------------------------------------

textures/ctf/q3r_bluelong_decal
{
	qer_editorimage textures/ctf/q3r_bannerbluelong.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	polygonoffset
	{
		map textures/ctf/q3r_bannerbluelong.tga
	}
	
}

// ------------------------------------------------------------
// 34.ctf (q3r_redlong_decal)
// ------------------------------------------------------------

textures/ctf/q3r_redlong_decal
{
	qer_editorimage textures/ctf/q3r_bannerredlong.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	polygonoffset
	{
		map textures/ctf/q3r_bannerredlong.tga
	}
	
}

// ------------------------------------------------------------
// 35.ctf (q3r_bluelong_01)
// ------------------------------------------------------------

textures/ctf/q3r_bluelong_01
{
	qer_editorimage textures/ctf/q3r_bannerbluelong.tga
	deformVertexes wave sin 0 1 0 .1
	surfaceparm nolightmap
	surfaceparm nomarks
	polygonoffset
	{
		map textures/ctf/q3r_bannerbluelong.tga
	}
	
}

// ------------------------------------------------------------
// 36.ctf (q3r_redlong_01)
// ------------------------------------------------------------

textures/ctf/q3r_redlong_01
{
	qer_editorimage textures/ctf/q3r_bannerredlong.tga
	deformVertexes wave sin 0 1 0 .1
	surfaceparm nolightmap
	surfaceparm nomarks
	polygonoffset
	{
		map textures/ctf/q3r_bannerredlong.tga
	}
}

// ------------------------------------------------------------
// 37.ctf (blueteam01)
// ------------------------------------------------------------

textures/ctf2/blueteam01
{
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nolightmap
	polygonoffset
	cull none
	{
		map textures/ctf2/blueteam01.tga
		alphafunc GE128
		blendFunc GL_DST_COLOR GL_ONE
		rgbgen wave sin .9 1 0 .1
	}
}

// ------------------------------------------------------------
// 38.ctf (redteam01)
// ------------------------------------------------------------

textures/ctf2/redteam01
{
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nolightmap
	polygonoffset
	cull none
	{
		map textures/ctf2/redteam01.tga
		alphafunc GE128
		blendFunc GL_DST_COLOR GL_ONE
		rgbgen wave sin .9 1 0 .1
	}
}

// ------------------------------------------------------------
// 39.ctf (blueteam02)
// ------------------------------------------------------------

textures/ctf2/blueteam02
{
	qer_editorimage textures/ctf2/blueteam01.tga
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nolightmap
	polygonoffset
	cull none
	{
		map textures/ctf2/blueteam01.tga
		alphafunc GE128
		blendFunc GL_DST_COLOR GL_ONE
		rgbgen wave sin .9 1 0 .1
	}
}

// ------------------------------------------------------------
// 40.ctf (redteam02)
// ------------------------------------------------------------

textures/ctf2/redteam02
{
	qer_editorimage textures/ctf2/redteam01.tga
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nolightmap
	polygonoffset
	cull none
	{
		map textures/ctf2/redteam01.tga
		alphafunc GE128
		blendFunc GL_DST_COLOR GL_ONE
		rgbgen wave sin .9 1 0 .1
	}
}

// ------------------------------------------------------------
// 41.ctf (blue_banner02)
// ------------------------------------------------------------

textures/ctf2/blue_banner02
{
	cull none
	{
		map textures/ctf2/blue_banner02.tga
		rgbgen identity
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
	}
}

// ------------------------------------------------------------
// 42.ctf (red_banner02)
// ------------------------------------------------------------

textures/ctf2/red_banner02
{
	cull none
	{
		map textures/ctf2/red_banner02.tga
		rgbgen identity
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
	}
}

// ------------------------------------------------------------
// 43.ctf (blue_telep)
// ------------------------------------------------------------

textures/ctf/blue_telep
{
	q3map_lightimage textures/effects/jumpcircblue.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	cull disable
	q3map_surfacelight 455
	{
		clampmap textures/effects/jumpcircblue.tga
		blendfunc add
		tcMod rotate 46
	}
	{
		clampmap textures/effects/jumpcircblue.tga
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.6 -0.3 0 0.5 
	}
	{
		clampmap textures/effects/jumpcircblue.tga
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.5 0.3 0 0.5 
	}
}

// ------------------------------------------------------------
// 44.ctf (red_telep)
// ------------------------------------------------------------

textures/ctf/red_telep
{
	q3map_lightimage textures/effects/jumpcircred.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	cull disable
	q3map_surfacelight 455
	{
		clampmap textures/effects/jumpcircred.tga
		blendfunc add
		tcMod rotate 46
	}
	{
		clampmap textures/effects/jumpcircred.tga
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.6 -0.3 0 0.5 
	}
	{
		clampmap textures/effects/jumpcircred.tga
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.5 0.3 0 0.5 
	}
}

// ------------------------------------------------------------
// 45.ctf (weapfloor_red)
// ------------------------------------------------------------

textures/bubctf1/weapfloor_red
{
	cull disable
	{
		clampmap textures/flares/largeglow.tga
		blendfunc add
		rgbGen const ( 0.85098 0.701961 0.701961 )
		tcMod rotate -40
	}
	{
		clampmap textures/flares/wide.tga
		blendfunc add
		rgbGen const ( 1 0 0 )
		tcMod rotate 42
		tcMod stretch sin 1 0.1 0 1 
	}
}

// ------------------------------------------------------------
// 46.ctf (weapfloor_blue)
// ------------------------------------------------------------

textures/bubctf1/weapfloor_blue
{
	cull disable
	{
		clampmap textures/flares/largeglow.tga
		blendfunc add
		rgbGen const ( 0.705882 0.717647 0.847059 )
		tcMod rotate -40
	}
	{
		clampmap textures/flares/wide.tga
		blendfunc add
		rgbGen const ( 0 0 1 )
		tcMod rotate 42
		tcMod stretch sin 1 0.1 0 1 
	}
}

// ------------------------------------------------------------
// 47.ctf (ta_techspawn_blue)
// ------------------------------------------------------------

textures/ctf_unified/ta_techspawn_blue
{
	cull disable
	{
		clampmap textures/flares/largeglow.tga
		blendfunc add
		rgbGen const ( 0.705882 0.717647 0.847059 )
		tcMod rotate -40
	}
	{
		clampmap textures/flares/wide.tga
		blendfunc add
		rgbGen const ( 0 0 1 )
		tcMod rotate 42
		tcMod stretch sin 1 0.1 0 1 
	}
}

// ------------------------------------------------------------
// 48.ctf (ta_techspawn_red)
// ------------------------------------------------------------

textures/ctf_unified/ta_techspawn_red
{
	cull disable
	{
		clampmap textures/flares/largeglow.tga
		blendfunc add
		rgbGen const ( 0.85098 0.701961 0.701961 )
		tcMod rotate -40
	}
	{
		clampmap textures/flares/wide.tga
		blendfunc add
		rgbGen const ( 1 0 0 )
		tcMod rotate 42
		tcMod stretch sin 1 0.1 0 1 
	}
}

// ------------------------------------------------------------
// 49.ctf (weapfloor_neutral)
// ------------------------------------------------------------

textures/bubctf1/weapfloor_neutral
{
	cull disable
	{
		clampmap textures/flares/largeglow.tga
		blendfunc add
		tcMod rotate -40
	}
	{
		clampmap textures/flares/wide.tga
		blendfunc add
		tcMod rotate 42
		tcMod stretch sin 1 0.1 0 1 
	}
}

// ------------------------------------------------------------
// 50.flags (flagsphere)
// ------------------------------------------------------------

models/flags/flagsphere
{
	{
		map textures/base_wall/chrome_env.tga
		rgbGen lightingDiffuse
		tcMod scale 0.5 0.5
		tcGen environment 
		blendfunc GL_ONE GL_ONE
	}
}

// ------------------------------------------------------------
// 51.flags (y_flag)
// ------------------------------------------------------------

models/flags/y_flag
{
	cull disable
	deformVertexes wave 80 sin 2 8 0 3 
	deformVertexes wave 20 square 0 2 0.5 0.2 
	{
		map textures/sfx/proto_zzztyellow.tga
		rgbGen identity
		tcMod turb 1 1 0.5 0.3
		tcGen environment 
	}
	{
		map models/flags/y_flag.tga
		rgbGen lightingDiffuse
		alphaFunc GE128
	}
	{
		map textures/effects/flagenv.tga
		blendfunc gl_dst_color gl_src_color
		tcGen environment 
	}
}

// ------------------------------------------------------------
// 52.flags (g_flag)
// ------------------------------------------------------------

models/flags/g_flag
{
	cull disable
	deformVertexes wave 80 sin 2 8 0 3 
	deformVertexes wave 20 square 0 2 0.5 0.2 
	{
		map textures/sfx/proto_zzztgreen.tga
		rgbGen identity
		tcMod turb 1 1 0.5 0.3
		tcGen environment 
	}
	{
		map models/flags/g_flag.tga
		rgbGen lightingDiffuse
		alphaFunc GE128
	}
	{
		map textures/effects/flagenv.tga
		blendfunc gl_dst_color gl_src_color
		tcGen environment 
	}
}
