textures/acc_dm3/comptop
{
	surfaceparm metalsteps
	{
		map textures/acc_dm3/comptop.jpg
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/acc_dm3/cop
{
	surfaceparm metalsteps
	{
		map textures/acc_dm3/cop.jpg
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/acc_dm3/stripes_nonsolid
{
	qer_editorimage textures/acc_dm3/stripes.jpg
	surfaceparm nonsolid
	{
		map textures/acc_dm3/stripes.jpg
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/acc_dm3/grind
{
	qer_trans 0.5
	q3map_lightimage textures/acc_dm3/grind.tga
	surfaceparm trans
	surfaceparm alphashadow
	surfaceparm metalsteps
	cull none
	{
		map textures/acc_dm3/grind.tga
		blendfunc GL_ONE GL_ZERO
		alphafunc GE128
		depthwrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbgen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthfunc equal
	}
}

textures/acc_dm3/chrome
{
	q3map_nonplanar 
	q3map_shadeAngle 150
	surfaceparm nonsolid
	{
		map textures/acc_dm3/chrome.jpg
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/acc_dm3/transrumbles
{
	qer_editorimage textures/acc_dm3/wallbotcracksb.jpg
	surfaceparm nonsolid
	{
		map textures/acc_dm3/wallbotcracksb.jpg
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/acc_dm3/fx_dm3sky
{
	surfaceparm nomarks
	q3map_sun 0.5058 0.3843 0.2156 200 340 25
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
	q3map_globaltexture
	q3map_surfacelight 70
	skyparms full 512 -
	{
		map textures/acc_dm3/sp_flatsky.jpg
	}
	{
		map textures/acc_dm3/sp_flatsky2.tga
		blendfunc blend
		tcMod scroll 0.002 0.01
		tcMod scale 3 3
		rgbgen identity
	}
}

textures/acc_dm3/sp_ground
{
	q3map_nonplanar 
	q3map_shadeAngle 70
	surfaceparm nonsolid
	{
		map textures/acc_dm3/sp_ground.jpg
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/acc_dm3/brwnwater
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm water
	cull disable
	{
		map textures/acc_dm3/brwnwater.tga
		blendFunc blend
		tcmod scroll .025 -.01
	}
	{
		map textures/acc_dm3/brwnwater.tga
		blendFunc filter
		tcmod scroll .05 -.007
		tcmod turb .1 .1 .1 .1
	}
}

textures/acc_dm3/fx_portal
{
	qer_editorimage textures/acc_dm3/fx_portal.jpg
	portal
	surfaceparm nolightmap
	surfaceparm nomarks
	{
		map textures/acc_dm3/portal_sfx.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
	}
	{
		map textures/acc_dm3/portal_fog.jpg
		blendfunc gl_src_alpha gl_one_minus_src_alpha
		alphagen portal 1024
		rgbGen identity	
		tcmod rotate .1 
		tcmod scroll .04 .01
	}
	{
		map textures/acc_dm3/fx_tintedportal.jpg
		rgbgen identity
		blendFunc filter
	}
}

textures/acc_dm3/rustpipes
{
	surfaceparm nonsolid
	{
		map textures/acc_dm3/rustpipes.jpg
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/acc_dm3/sqrlight
{
	surfaceparm nomarks
	{
		map textures/acc_dm3/sqrlight_dn.jpg
	}
	{
		map textures/acc_dm3/fx_window.tga
		tcgen environment
		rgbgen identity
		blendFunc blend
		depthFunc equal
	}
	{
		map textures/acc_dm3/sqrlight_up.tga
		alphafunc GE128
		depthwrite
	}
}

textures/acc_dm3/comp
{
	surfaceparm nomarks
	surfaceparm metalsteps
	{
		map textures/acc_dm3/comp.jpg
	}
	{
		map textures/acc_dm3/fx_window.tga
		tcgen environment
		rgbgen identity
		blendFunc blend
		depthFunc equal
	}
	{
		map textures/acc_dm3/comp_up.tga
		alphafunc GE128
		depthwrite
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
	{
		map textures/acc_dm3/comp_lght.tga
		blendfunc blend
	}
}

textures/acc_dm3/decalcomputer
{
	surfaceparm trans
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm nonsolid
	polygonoffset
	{
		map textures/acc_dm3/decalcomputer.tga
		blendfunc blend
		rgbgen vertex
	}
}

textures/acc_dm3/wires1
{
	cull disable
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm alphashadow
	polygonoffset
	{
		map textures/acc_dm3/wires1.tga
		alphafunc GE128
		depthwrite
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/acc_dm3/wires2
{
	cull disable
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm alphashadow
	polygonoffset
	{
		map textures/acc_dm3/wires2.tga
		alphafunc GE128
		depthwrite
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/acc_dm3/decalitem
{
	surfaceparm trans
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm nonsolid
	polygonoffset
	{
		map textures/acc_dm3/decalitem.tga
		blendfunc blend
		rgbgen vertex
	}
}

textures/acc_dm3/ind_wall1
{
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nonsolid
	{
		map textures/acc_dm3/ind_wall1.tga
		alphafunc GE128
		depthwrite
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/acc_dm3/ind_x1
{
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm alphashadow
	{
		map textures/acc_dm3/ind_x1.tga
		alphafunc GE128
		depthwrite
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/acc_dm5/flame
{
        q3map_flare textures/sfx/flameflare
	qer_editorimage textures/acc_dm5/flame.tga
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nonsolid
	surfaceparm nolightmap
	cull none
	{
		map textures/acc_dm5/flame.tga
		tcMod Scroll 1 0
		blendFunc blend
	}
	{
		map textures/acc_dm5/flame.tga
		blendFunc blend
		tcMod Scroll -.6 0
		rgbGen wave sin 2 0 .1 1.5
	}
}

textures/acc_dm5/fx_waterfall
{
	qer_editorimage textures/acc_dm5/fx_waterfall.jpg
	surfaceparm noimpact
	surfaceparm nonsolid
	surfaceparm trans
	cull none
	deformVertexes wave 32 sin 1.5 1.7 .5 1
	{
		map textures/acc_dm5/WaterFall1B.tga
		blendFunc blend
		tcmod scale .5 .5
		tcmod scroll 0.1 -1
	}
	{
		map textures/acc_dm5/WaterFall1B.tga
		blendFunc blend
		tcmod turb .05 -0.5 0 0.02
		tcmod scale .5 .5
		tcmod scroll 0.1 -2
	}
	{
		map textures/acc_dm5/WaterFall1A.tga
		blendFunc blend
		tcmod scale .5 .5
		tcmod scroll .02 -3
	}
}

textures/acc_dm5/brwnwater
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm water
	cull disable
	deformVertexes wave 32 sin .7 .7 .7 .5
	{
		map textures/acc_dm5/brwnwater.tga
		blendFunc blend
		tcmod scroll .025 -.001
	}
	{
		map textures/acc_dm5/brwnwater.tga
		blendFunc blend
		tcmod scroll .04 -.002
		tcmod turb .05 -0.05 0.05 0.01
	}
}

textures/acc_dm5/fx_portal
{
	qer_editorimage textures/acc_dm5/fx_portal.jpg
	portal
	surfaceparm nolightmap
	surfaceparm noimpact
	{
		map textures/acc_dm5/portal_sfx.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		depthWrite
	}
	{
		map textures/acc_dm5/portal_fog.jpg
		blendfunc gl_src_alpha gl_one_minus_src_alpha
		alphagen portal 1024
		rgbGen identity	
		tcmod rotate .1 
		tcmod scroll .04 .01
	}
}

textures/acc_dm5/fx_sky
{
	qer_editorimage textures/acc_dm5/fx_sky.jpg
	q3map_sun 0.176 0.494 0.694 30 160 80
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nolightmap
	surfaceparm sky
	q3map_globaltexture
	q3map_surfacelight 20
	skyParms full 700 -
	{
		map textures/acc_dm5/sky.tga
		tcMod scroll -0.013 0.04
	}
}

textures/acc_dm5/fx_lums
{
	qer_editorimage textures/acc_dm5/fx_lums.jpg
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	cull none
	DeformVertexes autosprite

	{
		map textures/acc_dm5/lum1c.tga
		blendfunc blend
		rgbGen wave sin 1 .5 0 .1
		tcMod rotate 50
		tcmod turb .2 -0.4 0.1 0.2
	}
	{
		map textures/acc_dm5/lum1a.tga
		tcmod scale 1.2 1.2
		blendfunc blend
		rgbGen wave sin 1 .5 0 .1
		tcMod rotate -120
	}
	{
		map textures/acc_dm5/lum1b.tga
		blendfunc blend
		tcmod scale 1 1
		tcMod rotate 10
	}
	{
		map textures/acc_dm5/lum1c.tga
		blendfunc blend
		tcmod scale 1 1
		rgbGen wave sin 1 .5 0 .2
		tcmod turb .5 -0.5 0.2 0.2
	}
}

textures/acc_dm5/mud_trans
{
	qer_editorimage textures/acc_dm5/mud02.jpg
	surfaceparm nonsolid
	cull none
	{
		map textures/acc_dm5/mud02.jpg
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/acc_dm5/roots_moss
{
	qer_editorimage textures/acc_dm5/roots_moss.jpg
	surfaceparm nonsolid
	cull none
	{
		map textures/acc_dm5/roots_moss.jpg
	}
	{
		map $lightmap
		rgbgen identity
		blendfunc filter
		depthfunc equal
	}
}

textures/acc_dm5/watershore
{
	qer_editorimage textures/acc_dm5/watershore.tga
	surfaceparm noimpact
	surfaceparm nonsolid
	surfaceparm trans
	cull none

	{
		map textures/acc_dm5/watershore.tga
		blendFunc blend
		tcmod turb .05 -0.5 .1 0.02
		tcmod scroll 0 -1.5
		tcMod stretch sin 1 -0.01 0 0.1
	}
	{
		map textures/acc_dm5/watershore.tga
		blendFunc blend
		tcmod scroll .1 -2
	}
}

textures/acc_dm5/woodenbarrier
{
	qer_trans 0.5
	q3map_lightimage textures/acc_dm5/woodenbarrier.tga
	cull none
	surfaceparm alphashadow
	surfaceparm trans
	{
		map textures/acc_dm5/woodenbarrier.tga
		blendfunc GL_ONE GL_ZERO
		alphafunc GE128
		depthwrite
		rgbGen identity
	}
	{
		map $lightmap
		rgbGen identity
		blendFunc GL_DST_COLOR GL_ZERO
		depthFunc equal
	}
}
