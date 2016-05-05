textures/skies/hellsky
{
	qer_editorimage textures/skies/dimclouds.tga
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nolightmap

	q3map_sun 4 3 3 65 290 75
	q3map_surfacelight 50
	skyparms - 512 -

	{
		map textures/skies/dimclouds.tga
		tcMod scroll 0.05 0
		tcMod scale 2 2
		depthWrite
	}
}

textures/skies/nightsky_xian_dm1
{
	surfaceparm noimpact
	surfaceparm nolightmap

	q3map_surfacelight 300
	q3map_sun 1 1 1 100 -41 58

	qer_editorimage textures/skies/xnight2_up.tga


	skyparms env/xnight2 - -
}

textures/skies/nightsky_xian_dm15
{
	qer_editorimage textures/skies/xnight2_up.tga
	surfaceparm nolightmap
	surfaceparm noimpact

	q3map_surfacelight 900
	q3map_sun 1 1 1 100 -58 58
	skyparms env/xnight2 - -


}

textures/skies/kc_dm10sky
{
	q3map_lightimage textures/skies/pjbasesky.tga
	q3map_lightsubdivide 512 
	q3map_globaltexture
	surfaceparm noimpact
	surfaceparm nolightmap
	
	q3map_sun	.5 .6 .8 90 225 55
	q3map_surfacelight 150

	qer_editorimage textures/skies/bluetopclouds.tga

	skyparms - 512 -

	{
		map textures/skies/bluedimclouds.tga
		tcMod scale 3 2
		tcMod scroll 0.15 0.15
	}
	{
		map textures/skies/topclouds.tga
		blendFunc GL_ONE GL_ONE
		tcMod scale 3 3
		tcMod scroll 0.05 0.05
	}


}

textures/q3r_skies/env/trainyard/trainyard
{
	qer_editorimage textures/q3r_skies/env/trainyard/trainyard_lf.tga
	surfaceparm sky
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	q3map_surfacelight 500
	q3map_lightsubdivide 512
	q3map_sun 1 1 1 32	90 90	
	skyparms textures/q3r_skies/env/trainyard/trainyard - -
}

textures/q3r_skies/clouds
{
	qer_editorimage textures/q3r_skies/clouds.tga
	surfaceparm noimpact
	surfaceparm nomarks
	surfaceparm nolightmap

	q3map_sun 4 3 3 65 290 75
	q3map_surfacelight 50
	skyparms - 512 -

	{
		map textures/q3r_skies/clouds.tga
		tcMod scroll 0.05 0
		tcMod scale 2 2
		depthWrite
	}
}

textures/skies/pjbasesky_arena1_sky
{
	qer_editorimage textures/skies/pjbasesky.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_lightsubdivide 256 
	q3map_sun	0.266383 0.274632 0.358662 150 60 85
	q3map_surfacelight 100

	skyparms - 512 -
	
	{
		map textures/skies/dimclouds.tga
		tcMod scroll 0.01 0.01
		tcMod scale 3 3
		depthWrite
	}
	{
		map textures/skies/pjbasesky.tga
		blendfunc GL_ONE GL_ONE
		tcMod scroll -0.01 -0.01
		tcMod scale 5 5
	}
}


textures/q3r_skies/env/mp_us/mp_us
{
	qer_editorimage textures/q3r_skies/env/mp_us/urbansp_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_lightsubdivide 256
	q3map_surfacelight 100
	surfaceparm sky
	q3map_sun 1 1 1 100 0 90
	skyparms textures/q3r_skies/env/mp_us/urbansp - -
}

textures/q3r_skies/env/sincity/sincity
{
	qer_editorimage textures/q3r_skies/env/sincity/sincity_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_lightsubdivide 256
	q3map_surfacelight 100
	surfaceparm sky
	q3map_sun 1 1 1 100 0 90
	skyparms textures/q3r_skies/env/sincity/sincity - -
}

textures/stecki/night01
{
	qer_editorimage textures/stecki/night01_up.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
//        q3map_lightimage textures/stecki/night01_up.tga
	q3map_sun	.5 .8 1 30 30 85
	q3map_surfacelight 5

        skyparms textures/stecki/night01 512 textures/stecki/night01

}

textures/stecki/desert
{
	qer_editorimage textures/stecki/desert_rt.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
//        q3map_lightimage textures/stecki/desert_up.tga
	q3map_sun	1 1 1 100 30 60
	q3map_surfacelight 80

        skyparms textures/stecki/desert 512 textures/stecki/desert


}

textures/stecki/sub01
{
	qer_editorimage textures/stecki/sub01_rt.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
//        q3map_lightimage textures/stecki/sub01_up.tga
	q3map_sun	1 1 1 120 180 60
	q3map_surfacelight 320

        skyparms textures/stecki/sub01 512 textures/stecki/sub01


}

////////////////////////////////////////////
///	thebigbuu's mkart-map series skyboxes //
////////////////////////////////////////////

textures/q3r_skies/env/mkart1/mkart1
{
	qer_editorimage textures/q3r_skies/env/mkart1/mkart1_ft.tga
	surfaceparm sky
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	q3map_surfacelight 32
	q3map_lightsubdivide 512
	q3map_sun 1 1 1 64 90 90	
	skyparms textures/q3r_skies/env/mkart1/mkart1 - -
}

textures/q3r_skies/env/mkart2/mkart2
{
	qer_editorimage textures/q3r_skies/env/mkart2/mkart2_ft.tga
	surfaceparm sky
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	q3map_surfacelight 32
	q3map_lightsubdivide 512
	q3map_sun .1 .1 1 100 90 90	
	skyparms textures/q3r_skies/env/mkart2/mkart2 - -
}

textures/q3r_skies/env/mkart3/mkart3
{
	qer_editorimage textures/q3r_skies/env/mkart3/mkart3_ft.tga
	surfaceparm sky
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	q3map_surfacelight 32
	q3map_lightsubdivide 512
	q3map_sun 1 1 1 92	90 90	
	skyparms textures/q3r_skies/env/mkart3/mkart3 - -
}

textures/q3r_skies/env/mkart4/mkart4
{
	qer_editorimage textures/q3r_skies/env/mkart4/mkart4_ft.tga
	surfaceparm sky
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	q3map_surfacelight 32
	q3map_lightsubdivide 512
	q3map_sun 1 1 1 64 90 90	
	skyparms textures/q3r_skies/env/mkart4/mkart4 - -
}

textures/q3r_skies/env/mkart5/mkart5
{
	qer_editorimage textures/q3r_skies/env/mkart5/mkart5_bk.tga
	surfaceparm sky
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nomarks
	//q3map_surfacelight 32
	q3map_lightsubdivide 512
	//q3map_sun .36 .28 .13 64 90 90	//brown
	q3map_sun 0 .5 .39 64 90 90	//teal? dark greenish?
	skyparms textures/q3r_skies/env/mkart5/mkart5 - -
}

//q3rally ctf skybox, sorry for the NES style look, lol ;
//NOTICE:red section facing 180deg(_lf = left);
//blue section facing 0deg(_rt = right);
//plan accordingly or this next skybox WILL DISORIENT players. --thebigbuu;

textures/q3r_skies/env/q3r_curtainwide/q3r_curtainwide
{
	qer_editorimage textures/q3r_skies/env/q3r_curtainwide/q3r_curtainwide_ft.tga
	q3map_globaltexture
	q3map_lightsubdivide 1024
	q3map_sun 1 1 1 32 90 90
	surfaceparm nomarks
	surfaceparm nodlight
	surfaceparm nolightmap
	surfaceparm sky
	skyparms textures/q3r_skies/env/q3r_curtainwide/q3r_curtainwide - -
}

textures/q3r_skies/env/ame_nebula
{
	qer_editorimage textures/q3r_skies/env/ame_nebula/purplenebula_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_sun 1.00 0.90 1.00 70 90 90
	q3map_surfacelight 70
	skyparms textures/q3r_skies/env/ame_nebula/purplenebula - -
}