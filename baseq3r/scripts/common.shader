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
// 14.invisible
// ------------------------------------------------------------

textures/common/invisible // solid, transparent polygons, casts shadows
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
