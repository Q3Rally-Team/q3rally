// ------------------------------------------------------------
// Shaders for Q3Rally common textures - rewritten by P3rlE
// based on the common.shader
// ------------------------------------------------------------

// ------------------------------------------------------------
// 1.mirror
// ------------------------------------------------------------

textures/common/mirrortut
{
	qer_editorimage textures/common/mirrortut.tga
	surfaceparm nolightmap
	portal
  
	{
		map textures/common/mirrortut.tga
		blendfunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		depthWrite
	}
       
        

}

// ------------------------------------------------------------
// 1b.mirror1
// ------------------------------------------------------------

textures/common/mirror1
{
	qer_editorimage textures/common/mirror1.tga
	surfaceparm nolightmap
	portal
	{
		map textures/common/mirror1.tga
		blendfunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		depthWrite
	}
}

// ------------------------------------------------------------
// 1c.mirror2
// ------------------------------------------------------------

textures/common/mirror2
{
	qer_editorimage textures/common/qer_mirror.tga
	surfaceparm nolightmap
	portal
	{
		map textures/common/mirror1.tga
		blendfunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		depthWrite
	}
	{
		map textures/sfx/mirror.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
	}
}

// ------------------------------------------------------------
// 2.rain
// ------------------------------------------------------------

textures/common/rain
{
	qer_trans	0.5
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
}

// ------------------------------------------------------------
// 3.snow
// ------------------------------------------------------------

textures/common/snow
{
	qer_trans	0.5
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
}

// ------------------------------------------------------------
// 4.checkpoint
// ------------------------------------------------------------

textures/common/checkpoint
{
	qer_trans	0.5
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
}

// ------------------------------------------------------------
// 5.startfinish
// ------------------------------------------------------------

textures/common/startfinish
{
	qer_trans	0.5
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
}

// ------------------------------------------------------------
// 6.areaportal
// ------------------------------------------------------------

textures/common/areaportal
{
	qer_trans 0.50
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm structural
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm areaportal
}

// ------------------------------------------------------------
// 7.caulk
// ------------------------------------------------------------

textures/common/caulk
{
	surfaceparm nodraw
    	surfaceparm nolightmap
	surfaceparm nomarks
}

// ------------------------------------------------------------
// 8.clip
// ------------------------------------------------------------

textures/common/clip
{
	qer_trans 0.40
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm noimpact
	surfaceparm playerclip
}

// ------------------------------------------------------------
// 9.clusterportal
// ------------------------------------------------------------

textures/common/clusterportal
{
	qer_trans 0.50
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm clusterportal
}

// ------------------------------------------------------------
// 10.cushion
// ------------------------------------------------------------

textures/common/cushion
{
	qer_nocarve
	qer_trans 0.50
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nodamage
}

// ------------------------------------------------------------
// 11.donotenter
// ------------------------------------------------------------

textures/common/donotenter
{
	qer_trans 0.50
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm donotenter
}

// ------------------------------------------------------------
// 12.full_clip
// ------------------------------------------------------------

textures/common/full_clip
{
	qer_trans 0.40
	surfaceparm nodraw
	surfaceparm playerclip
}

// ------------------------------------------------------------
// 13.hint
// ------------------------------------------------------------

textures/common/hint
{
	qer_nocarve
	qer_trans 0.30
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm structural
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm hint	
}

// ------------------------------------------------------------
// 13b.hintlocal
// ------------------------------------------------------------

textures/common/hintlocal
{
	qer_nocarve
	qer_trans 0.30
	qer_editorImage textures/common/hintlocal.tga
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm structural
	surfaceparm trans
	surfaceparm noimpact
}

// ------------------------------------------------------------
// 14.invisible (solid, transparent polygons, casts shadows)
// ------------------------------------------------------------

textures/common/invisible 
{
	surfaceparm nolightmap			
        {
                map textures/common/invisible.tga
                alphaFunc GE128
		depthWrite
		rgbGen vertex
        }
}

// ------------------------------------------------------------
// 15.missileclip
// ------------------------------------------------------------

textures/common/missileclip
{
	qer_trans 0.40
	surfaceparm nodamage
	surfaceparm nomarks
	surfaceparm nodraw
	//surfaceparm nonsolid
	surfaceparm playerclip
	surfaceparm trans
}

// ------------------------------------------------------------
// 16.nodraw
// ------------------------------------------------------------

textures/common/nodraw
{
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
}

// ------------------------------------------------------------
// 17.nodrawnonsolid
// ------------------------------------------------------------

textures/common/nodrawnonsolid
{
	surfaceparm nonsolid
	surfaceparm nolightmap
	surfaceparm nodraw
}

// ------------------------------------------------------------
// 18.nodrop
// ------------------------------------------------------------

textures/common/nodrop
{
	qer_nocarve
	qer_trans 0.5
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm nodrop
}

// ------------------------------------------------------------
// 19.noimpact
// ------------------------------------------------------------

textures/common/noimpact
{
	qer_editorimage textures/common/nolightmap.tga
	surfaceparm noimpact
}

// ------------------------------------------------------------
// 20.nolightmap
// ------------------------------------------------------------

textures/common/nolightmap
{
	surfaceparm nolightmap
}

// ------------------------------------------------------------
// 21.origin
// ------------------------------------------------------------

textures/common/origin
{
	qer_nocarve
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm origin
}

// ------------------------------------------------------------
// 22.portal
// ------------------------------------------------------------

textures/common/portal
{
	qer_editorimage textures/common/qer_portal.tga
	surfaceparm nolightmap
	portal
	{
		map textures/common/mirror1.tga
		tcMod turb 0 0.25 0 0.05
		blendfunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		depthWrite

	}
}

// ------------------------------------------------------------
// 23.slick
// ------------------------------------------------------------

textures/common/slick
{
	qer_trans 0.50
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm slick
}

// ------------------------------------------------------------
// 24.terrain
// ------------------------------------------------------------

textures/common/terrain
{
	q3map_terrain
	surfaceparm nodraw
    	surfaceparm nolightmap
	surfaceparm nomarks
}

// ------------------------------------------------------------
// 25.terrain2
// ------------------------------------------------------------

textures/common/terrain2
{
	q3map_terrain
	qer_editorimage textures/common/terrain.tga
	surfaceparm dust
	surfaceparm nodraw
	surfaceparm nomarks
	surfaceparm nolightmap
}

// ------------------------------------------------------------
// 26.timportal
// ------------------------------------------------------------

textures/common/timportal
{
	qer_editorimage textures/common/qer_portal.tga
	portal
	surfaceparm nolightmap
	{
		map textures/common/portal.tga
		tcMod turb 0 0.25 0 0.05
		blendFunc GL_ONE GL_SRC_ALPHA
		depthWrite
	}
}

// ------------------------------------------------------------
// 27.trigger
// ------------------------------------------------------------

textures/common/trigger
{
	qer_trans 0.50
	qer_nocarve
	surfaceparm nodraw
}

// ------------------------------------------------------------
// 28.weapclip
// ------------------------------------------------------------

textures/common/weapclip
{
	qer_trans 0.40
	surfaceparm nodraw
    	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nomarks
}

// ------------------------------------------------------------
// 29.metalclip
// ------------------------------------------------------------

textures/common/metalclip
{
	qer_trans 0.40
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm noimpact
	surfaceparm playerclip
	surfaceparm metalsteps
}

// ------------------------------------------------------------
// 30.botclip
// ------------------------------------------------------------

textures/common/botclip
{
	qer_trans 0.40
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm noimpact
	surfaceparm botclip
}

// ------------------------------------------------------------
// 31.lightgrid
// ------------------------------------------------------------
// ------------------------------------------------------------
// ydnar q3map lightgrid bounds
//
// the min/max bounds of brushes with this shader in a map
// will def the bounds of the map's lightgrid (model lighting)
// note: make it as small as possible around player space
// to minimize bsp size and compile time
// ------------------------------------------------------------

textures/common/lightgrid
{
	qer_trans 0.5
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm detail
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm lightgrid
}

// ------------------------------------------------------------
// 32.q3r_checkpoint_a
// ------------------------------------------------------------

textures/common/q3r_checkpoint_a
{
	qer_editorimage textures/common/q3r_checkpoint_a.tga
	cull disable
	{
		AnimMap 2 textures/common/q3r_checkpoint_a.tga textures/common/q3r_checkpoint_ainv.tga
	}
}

// ------------------------------------------------------------
// 33.energypad
// ------------------------------------------------------------

textures/common/energypad
{
	qer_editorimage textures/sfx/bluegoal.jpg
	surfaceparm nolightmap
	cull twosided
	{
		map textures/sfx/bluegoal.jpg
		blendFunc GL_ONE GL_SRC_ALPHA
		tcGen environment
		tcMod turb 0 0.25 0 0.05
	}
}

// ------------------------------------------------------------
// 34.antiportal (works like hint, but supresses portals)
// ------------------------------------------------------------

textures/common/antiportal
{
	qer_nocarve
	qer_trans 0.30
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm structural
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm antiportal
}

// ------------------------------------------------------------
// 35.skip
// ------------------------------------------------------------
// ydnar: skip works like quake 2 hint: it doesn't generate bsp splits
// use on sides of hint brushes where you don't want bsp splits or portals

textures/common/skip
{
	qer_nocarve
	qer_trans 0.30
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm structural
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm skip
}

// ------------------------------------------------------------
// 36.hintskip 
// ------------------------------------------------------------
// same as skip but name changed to allow CTRL+H to filter both hint and skip

textures/common/hintskip
{
	qer_nocarve
	qer_trans 0.30
	qer_editorImage textures/common/skip.tga
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm structural
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm skip
}

// ------------------------------------------------------------
// 37.watercaulk
// ------------------------------------------------------------
// 2 usages for watercaulk depending on water brush complexity
// SIMPLE WATER BRUSHES - use watercaulk on faces between water brushes
// COMPLEX WATER BRUSHES - overlap complex water brushes with watercaulk.
// Water shader should be nodraw, nonsolid, trans, *sans-water*

textures/common/watercaulk
{
	qer_trans 0.5
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm water
}

// ------------------------------------------------------------
// 38.slimecaulk
// ------------------------------------------------------------

textures/common/slimecaulk
{
	qer_trans 0.5
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm slime
}

// ------------------------------------------------------------
// 39.lavacaulk
// ------------------------------------------------------------

textures/common/lavacaulk
{
	qer_trans 0.5
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm lava
}

// ------------------------------------------------------------
// 40.alphafade
// ------------------------------------------------------------
// alpha fade shaders
// (c) 2004 randy reddig
// http://www.shaderlab.com

textures/common/alpha_100
{
	qer_trans 0.5
	q3map_alphaMod volume
	q3map_alphaMod scale 1.0
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
}

textures/common/alpha_75
{
	qer_trans 0.5
	q3map_alphaMod volume
	q3map_alphaMod scale 0.75
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
}

textures/common/alpha_50
{
	qer_trans 0.5
	q3map_alphaMod volume
	q3map_alphaMod scale 0.5
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
}

textures/common/alpha_25
{
	qer_trans 0.5
	q3map_alphaMod volume
	q3map_alphaMod scale 0.25
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
}

textures/common/alpha_0
{
	qer_trans 0.5
	q3map_alphaMod volume
	q3map_alphaMod scale 0
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
}