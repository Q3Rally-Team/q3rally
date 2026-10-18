// ------------------------------------------------------------
// Shaders for Q3Rally additional textures - rewritten by P3rlE
// based on several shaders e.g. proto
// ------------------------------------------------------------

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

