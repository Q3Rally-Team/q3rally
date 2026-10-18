// ------------------------------------------------------------
// Shaders for base textures - rewritten by P3rlE
// based on base-xxx.shader from id software
// ------------------------------------------------------------

// ------------------------------------------------------------
// 1.base_light
// ------------------------------------------------------------

// ------------------------------------------------------------
// 2.base_button
// ------------------------------------------------------------

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
// 19.base_floor (clang_floor3blava)
// ------------------------------------------------------------

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
// 38.base_floor (diamond_dirty)
// ------------------------------------------------------------

// ------------------------------------------------------------
//  This texture has a detail layer that is used to apply a 
//  smeary, dirty overlay to the diamond metal floor texture
// ------------------------------------------------------------

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
// 261.base_light (trianglelight)
// ------------------------------------------------------------

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

// ------------------------------------------------------------
// 503.base_floor (tilefloor7_ow)
// ------------------------------------------------------------

textures/base_floor/tilefloor7_ow
{
	qer_editorimage textures/base_floor/tilefloor7_ow.jpg
	q3map_lightimage textures/base_floor/tilefloor7_owfx.jpg
	q3map_surfacelight 50
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/base_floor/tilefloor7_ow.jpg
		blendfunc filter
		rgbgen identity
	}
	{
		map textures/base_floor/tilefloor7_owfx.jpg
		blendfunc add
		rgbgen wave sin 2 1 .5 6
	}
	
}

// ------------------------------------------------------------
// 504.base_wall (metalfloor_wall_10)
// ------------------------------------------------------------

textures/base_wall/metalfloor_wall_10
{
	qer_editorimage textures/base_wall/metalfloor_wall_10.jpg
	{
		map textures/base_wall/metalfloor_wall_10.jpg
		rgbgen identity
	}
	{
		map textures/effects/tinfx.tga
		tcgen environment
		rgbgen vertex
	}
	{
		map textures/base_wall/metalfloor_wall_10.jpg
		blendfunc add
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

