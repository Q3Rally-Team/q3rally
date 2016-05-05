//================
//	Mirror	
//================

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

//============================
//	rally_weather_rain	
//============================

textures/common/rain
{
	qer_trans	0.5
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
}

//============================
//	rally_weather_snow	
//============================

textures/common/snow
{
	qer_trans	0.5
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
}

//======================
//	q3r_checkpoint	
//======================

textures/common/checkpoint
{
	qer_trans	0.5
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
}

//======================
//	q3r_startfinish	
//======================

textures/common/startfinish
{
	qer_trans	0.5
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
}

//======================
//	Areaportal		
//======================

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

//================
//	Caulk		
//================

textures/common/caulk
{
	surfaceparm nodraw
    surfaceparm nolightmap
	surfaceparm nomarks
}

//================
//	Clip		
//================

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

//======================
//	Clusterportal	
//======================

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

//======================
//	Cushion		
//======================

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

//======================
//	DoNotEnter		
//======================

textures/common/donotenter
{
	qer_trans 0.50
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm donotenter
}

//======================
//	Full Clip		
//======================

textures/common/full_clip // maybe removing?
{
	qer_trans 0.40
	surfaceparm nodraw
	surfaceparm playerclip
}

//================
//	Hint		
//================

textures/common/hint // should NOT use surfaceparm hint.. strange but true
{
	qer_nocarve
	qer_trans 0.30
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm structural
	surfaceparm trans
	surfaceparm noimpact
	surfaceparm hint	// ydnar: yes it should.
}

//======================
//	Invisible		
//======================

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

//======================
//	Missile Clip	
//======================

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

//================
//	No Draw	
//================

textures/common/nodraw
{
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
}

//============================
//	No Draw Non Solid		
//============================

textures/common/nodrawnonsolid
{
	surfaceparm nonsolid
	surfaceparm nolightmap
	surfaceparm nodraw
}

//================
//	No Drop	
//================

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

//================
//	No Impact	
//================

textures/common/noimpact
{
	qer_editorimage textures/common/nolightmap.tga
	surfaceparm noimpact
}

//======================
//	No Lightmap		
//======================

textures/common/nolightmap
{
	surfaceparm nolightmap
}

//================
//	Origin	
//================

textures/common/origin
{
	qer_nocarve
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm origin
}

//================
//	Portal	
//================

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

//================
//	Slick		
//================

textures/common/slick
{
	qer_trans 0.50
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm slick
}

//================
//	Terrain	
//================

textures/common/terrain
{
	q3map_terrain
	surfaceparm nodraw
    surfaceparm nolightmap
	surfaceparm nomarks
}

//================
//	Terrain 2	
//================

textures/common/terrain2
{
	q3map_terrain
	qer_editorimage textures/common/terrain.tga
	surfaceparm dust
	surfaceparm nodraw
	surfaceparm nomarks
	surfaceparm nolightmap
}

//================
//	Tim Portal	
//================

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

//================
//	Trigger	
//================

textures/common/trigger
{
	qer_trans 0.50
	qer_nocarve
	surfaceparm nodraw
}

//================
//	Weapon Clip	
//================

textures/common/weapclip
{
	qer_trans 0.40
	surfaceparm nodraw
    	surfaceparm nolightmap
	surfaceparm trans
	surfaceparm nomarks
}

//================
//	Metal Clip	
//================

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

//================
//	Bot Clip	
//================

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


//=================================================================
// ydnar q3map lightgrid bounds
//
// the min/max bounds of brushes with this shader in a map
// will define the bounds of the map's lightgrid (model lighting)
// note: make it as small as possible around player space
// to minimize bsp size and compile time
//=================================================================

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

//================
// q3r checkpoint sign 1
//================

textures/common/q3r_checkpoint_a
{
	qer_editorimage textures/common/q3r_checkpoint_a.tga
	cull disable
	{
		AnimMap 2 textures/common/q3r_checkpoint_a.tga textures/common/q3r_checkpoint_ainv.tga
	}
}

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
