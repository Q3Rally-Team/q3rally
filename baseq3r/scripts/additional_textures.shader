// ------------------------------------------------------------
// Shaders for Q3Rally additional textures - rewritten by P3rlE
// based on several shaders e.g. proto
// ------------------------------------------------------------


// ------------------------------------------------------------
// 1.cheapfloor
// ------------------------------------------------------------

textures/proto2/cheapfloor
{
	qer_editorimage textures/proto2/marble02b_floor.tga
	{
		map textures/proto2/marble02b_floor.tga
		rgbgen identity
	}
	{
		map textures/effects/tinfx.tga
		tcgen environment
		rgbgen vertex
	}
	{
		map textures/proto2/marble02b_floor.tga
		blendfunc add
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 2.mirrorfloor
// ------------------------------------------------------------

textures/proto2/mirrorfloor
{
	qer_editorimage textures/proto2/marble02b_floor.tga
	portal
	{
		map textures/common/invisible.tga
		blendfunc gl_one gl_one_minus_src_alpha
		depthwrite
	}
	{
		map textures/proto2/marble02b_floor.tga
		blendfunc add
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 3.marble02b_s
// ------------------------------------------------------------

textures/proto2/marble02b_s
{
	qer_editorimage textures/proto2/marble02b_s.tga
	surfaceparm metalsteps
	{
		map textures/proto2/marble02b_s.tga
		rgbgen identity
	}
	{
		map textures/effects/tinfx.tga
		tcgen environment
		rgbgen vertex
	}
	{
		map textures/proto2/marble02b_s.tga
		blendfunc add
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 4.marble02b_s_green
// ------------------------------------------------------------

textures/proto2/marble02b_s_green
{
	qer_editorimage textures/proto2/marble02b_s.tga
	surfaceparm metalsteps
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/proto2/marble02b_s.tga
		blendfunc gl_dst_color gl_src_alpha
		rgbgen identitylighting
		alphagen lightingspecular
	}
}

// ------------------------------------------------------------
// 5.timbutton
// ------------------------------------------------------------

textures/gothic_button/timbutton
{
	q3map_surfacelight textures/gothic_button/timbutton.tga
	q3map_lightimage textures/gothic_button/timbutton2.jpg
	q3map_surfacelight 20
	
	{
		map textures/gothic_button/timbutton.tga
		rgbgen identity
	}
	{
		map textures/effects/tinfx.tga
		tcgen environment
		rgbgen vertex
	}
	{
		map textures/gothic_button/timbutton2.jpg
		blendfunc add
		rgbGen wave sin 1 1 0 0.5
	}
	{
		map textures/gothic_button/timbutton.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 6.steam01
// ------------------------------------------------------------

textures/proto2/steam01
{
	qer_editorimage textures/proto2/steam01.tga
	cull none
	surfaceparm trans
	deformvertexes autosprite2
	deformvertexes wave 30 sin 0.1 2 .5 .5
	{
		map textures/proto2/steam01.tga
		tcmod scroll 0 0.4
		blendfunc blend
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 7.zinc_shiny
// ------------------------------------------------------------

textures/gothic_trim/zinc_shiny
{
	qer_editorimage textures/gothic_trim/zinc_shiny.tga
	{	
		map textures/gothic_trim/zinc_shiny.tga
		rgbgen identity
	}
	{
		map textures/effects/tinfx.tga
		tcgen environment
		rgbgen identity
	}
	{
		map textures/gothic_trim/zinc_shiny.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 8.metalsupport4h_shiny
// ------------------------------------------------------------

textures/gothic_trim/metalsupport4h_shiny
{
	qer_editorimage textures/gothic_trim/metalsupport4h_shiny.tga
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_trim/metalsupport4h_shiny.tga
		blendfunc gl_dst_color gl_src_alpha
		rgbgen identitylighting
		alphagen lightingspecular
	}
}

// ------------------------------------------------------------
// 9.metalsupport4h_trans
// ------------------------------------------------------------

textures/gothic_trim/metalsupport4h_trans
{
	qer_editorimage textures/gothic_trim/metalsupport4h_shiny.tga
	surfaceparm nonsolid
	{
		map textures/gothic_trim/metalsupport4h_shiny.tga
		rgbgen identity
	}
	{
		map textures/effects/tinfx.tga
		tcgen environment
		rgbgen identity
	}
	{
		map textures/gothic_trim/metalsupport4h_shiny.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 10.gothgrate2
// ------------------------------------------------------------

textures/gothic_trim/gothgrate2
{
	surfaceparm metalsteps
	cull none
	{
		map textures/gothic_trim/gothgrate2.tga
		blendfunc blend
		rgbgen identity
		alphafunc GE128
		depthwrite
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
		depthfunc equal
	}
}

// ------------------------------------------------------------
// 11.metalfloor_wall_14sr
// ------------------------------------------------------------

textures/clown/metalfloor_wall_14sr
{
	qer_editorimage textures/clown/metalfloor_wall_14sr.tga

	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/clown/metalfloor_wall_14sr.tga
		blendfunc gl_dst_color gl_src_alpha
		rgbgen identitylighting
		alphagen lightingspecular
	}
}

// ------------------------------------------------------------
// 12.cheapfloor2
// ------------------------------------------------------------

textures/clown/cheapfloor2
{
	qer_editorimage textures/clown/stone_tile.jpg
	{
		map textures/clown/stone_tile.jpg
		rgbgen identity
	}
	{
		map textures/effects/tinfx.tga
		tcgen environment
		rgbgen vertex
	}
	{
		map textures/clown/stone_tile.jpg
		blendfunc add
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 13.c_zinc_shiny
// ------------------------------------------------------------

textures/proto2/c_zinc_shiny
{
	qer_editorimage textures/gothic_trim/zinc_shiny.tga
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/gothic_trim/zinc_shiny.tga
		blendfunc gl_dst_color gl_src_alpha
		rgbgen identitylighting
		alphagen lightingspecular
	}
}

// ------------------------------------------------------------
// 14.vertex_greenstone
// ------------------------------------------------------------

textures/proto2/vertex_greenstone
{
	qer_editorimage textures/gothic_block/blocks17i.jpg
	surfaceparm pointlight
	{
		map textures/gothic_block/blocks17i.jpg
		rgbgen oneminusvertex
	}
}

// ------------------------------------------------------------
// 15.vertex_cavestone
// ------------------------------------------------------------

textures/proto2/vertex_cavestone
{
	qer_editorimage textures/organics/pjrock7d.jpg
	surfaceparm pointlight
	{
		map textures/organics/pjrock7d.jpg
		rgbgen oneminusvertex
	}
}

// ------------------------------------------------------------
// 16.tin_panel
// ------------------------------------------------------------

textures/proto2/tin_panel
{
	surfaceparm metalsteps
	{
		map textures/effects/tinfx.tga
		blendfunc filter
		tcgen environment
		rgbgen identity
	}
	{
		map textures/proto2/tin_panel.tga
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
// 17.concrete_blue
// ------------------------------------------------------------

textures/proto2/concrete_blue
{
	surfaceparm metalsteps
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/proto2/concrete_blue.tga
		blendfunc gl_dst_color gl_src_alpha
		rgbgen identitylighting
		alphagen lightingspecular
	}
	{
		map $lightmap
		blendfunc gl_dst_color gl_zero
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 18.concrete_red
// ------------------------------------------------------------

textures/proto2/concrete_red
{
	surfaceparm metalsteps
	{
		map $lightmap
		rgbgen identity
	}
	{
		map textures/proto2/concrete_red.tga
		blendfunc gl_dst_color gl_src_alpha
		rgbgen identitylighting
		alphagen lightingspecular
	}

}

// ------------------------------------------------------------
// 19.concrete_bluenfx
// ------------------------------------------------------------

textures/proto2/concrete_bluenfx
{
	qer_editorimage textures/proto2/concrete_blue.tga
	surfaceparm metalsteps
	{
		map textures/proto2/concrete_blue.tga
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
	}
}

// ------------------------------------------------------------
// 20.concrete_rednfx
// ------------------------------------------------------------

textures/proto2/concrete_rednfx
{
	qer_editorimage textures/proto2/concrete_red.tga
	surfaceparm metalsteps
	{
		map textures/proto2/concrete_red.tga
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
	}
}

// ------------------------------------------------------------
// 21.concrete01wetwall
// ------------------------------------------------------------

textures/proto2/concrete01wetwall
{
	{
		map textures/liquids/proto_gruel3.tga
		tcmod scale 5 5
		tcmod scroll 0 -0.2
		rgbgen identity
	}
	{
		map textures/proto2/concrete01wetwall.tga
		blendfunc blend
		rgbgen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 22.blue_zot
// ------------------------------------------------------------

textures/proto2/blue_zot
{
	qer_editorimage textures/proto2/blue_zot.tga
	deformvertexes autosprite2
	deformvertexes move 0.23 0.2 0.3 sin 0 5 0 1
	deformvertexes move 0.3 0 0.3 sin 0 5 0 10
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nolightmap
	cull none
	{
		map textures/proto2/blue_zot.tga
		blendfunc add
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 23.red_zot
// ------------------------------------------------------------

textures/proto2/red_zot
{
	qer_editorimage textures/proto2/red_zot.tga
	deformvertexes autosprite2
	deformvertexes move 0.23 0.2 0.3 sin 0 5 0 1
	deformvertexes move 0.3 0 0.3 sin 0 5 0 10
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nolightmap
	cull none
	{
		map textures/proto2/red_zot.tga
		blendfunc add
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 24.blue_zot2
// ------------------------------------------------------------

textures/proto2/blue_zot2
{
	qer_editorimage textures/proto2/blue_zot2.tga
	q3map_lightimage textures/proto2/blue_zot2.tga
	q3map_surfacelight 100
	deformvertexes autosprite2
	deformvertexes move 0.23 0.2 0.3 sin 0 5 0 1
	deformvertexes move 0.3 0 0.3 sin 0 5 0 10
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nolightmap
	cull none
	{
		map textures/proto2/blue_zot2.tga
		blendfunc add
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 25.red_zot2
// ------------------------------------------------------------

textures/proto2/red_zot2
{
	qer_editorimage textures/proto2/red_zot2.tga
	q3map_lightimage textures/proto2/red_zot2.tga
	q3map_surfacelight 100
	deformvertexes autosprite2
	deformvertexes move 0.23 0.2 0.3 sin 0 5 0 1
	deformvertexes move 0.3 0 0.3 sin 0 5 0 10
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nolightmap
	cull none
	{
		map textures/proto2/red_zot2.tga
		blendfunc add
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 26.blueflare
// ------------------------------------------------------------

textures/proto2/blueflare
{
	deformvertexes autosprite
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm nolightmap
	cull none
	{
		clampmap textures/proto2/blueflare.tga
		tcmod rotate 25
		blendfunc add
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 27.blueflare2
// ------------------------------------------------------------

textures/proto2/blueflare2
{
	deformvertexes autosprite2
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm nolightmap
	cull none
	{
		map textures/proto2/blueflare2.tga
		blendfunc add
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 28.redflare
// ------------------------------------------------------------

textures/proto2/redflare
{
	deformvertexes autosprite
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm nolightmap
	cull none
	{
		clampmap textures/proto2/redflare.tga
		tcmod rotate 25
		blendfunc add
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 29.redflare2
// ------------------------------------------------------------

textures/proto2/redflare2
{
	deformvertexes autosprite2
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm nomarks
	surfaceparm nolightmap
	cull none
	{
		map textures/proto2/redflare2.tga
		blendfunc add
		rgbgen identity
	}
}

// ------------------------------------------------------------
// 30.flareGreen
// ------------------------------------------------------------

sfx/flareGreen
{
	q3map_flare flareShader
	{
		map textures/sfx/flarebox.tga
		rgbGen const ( 0 1 0 )
	}
}