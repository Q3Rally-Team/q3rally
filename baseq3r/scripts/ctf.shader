// ------------------------------------------------------------
// Shaders for Q3Rally CTF - rewritten by P3rlE
// based on the ctf.shader
// ------------------------------------------------------------

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

// powerups/redflag and powerups/blueflag are defined in
// powerups.shader and referenced by maps and gameplay code.

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
// 43.ctf (blue_telep)
// ------------------------------------------------------------

textures/ctf/blue_telep
{
	q3map_lightimage textures/effects/jumpcircblue.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	cull disable
	q3map_surfacelight 455
	{
		clampmap textures/effects/jumpcircblue.jpg
		blendfunc add
		tcMod rotate 46
	}
	{
		clampmap textures/effects/jumpcircblue.jpg
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.6 -0.3 0 0.5 
	}
	{
		clampmap textures/effects/jumpcircblue.jpg
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
	q3map_lightimage textures/effects/jumpcircred.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	cull disable
	q3map_surfacelight 455
	{
		clampmap textures/effects/jumpcircred.jpg
		blendfunc add
		tcMod rotate 46
	}
	{
		clampmap textures/effects/jumpcircred.jpg
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.6 -0.3 0 0.5 
	}
	{
		clampmap textures/effects/jumpcircred.jpg
		blendfunc add
		tcMod rotate -80
		tcMod stretch sin 0.5 0.3 0 0.5 
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
