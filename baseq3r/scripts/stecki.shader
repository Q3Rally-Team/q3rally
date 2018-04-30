textures/stecki/haze
{
	qer_editorimage textures/stecki/haze_rt.tga
	surfaceparm sky
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_sun 1 .992 .879 80 38 30
	q3map_surfacelight 50
	skyparms textures/stecki/haze - -
}

//stecki haze fog
textures/stecki/fog_haze
{
qer_editorimage textures/sfx/fog_grey.tga
surfaceparm	trans
surfaceparm	nonsolid
surfaceparm	fog
surfaceparm	nolightmap

fogparms ( .477 .477 .479 ) 400

}

textures/stecki/cameraflash
{
	qer_editorimage textures/stecki/cameraflash_01.tga
      surfaceparm nomarks	
      surfaceparm nonsolid
	surfaceparm nolightmap
	deformVertexes autosprite

	{
	animMap 2 textures/stecki/cameraflash_01.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga
		blendFunc add
		rgbGen wave Inversesawtooth 0 2 0 2
//		rgbGen wave Inversesawtooth 1 1 0 1
	}
}

textures/stecki/cameraflash2
{
	qer_editorimage textures/stecki/cameraflash_01.tga
      surfaceparm nomarks	
      surfaceparm nonsolid
	surfaceparm nolightmap
	deformVertexes autosprite

	{
	animMap 2 textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_01.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga
		blendFunc add
		rgbGen wave Inversesawtooth 0 2 0 2
//		rgbGen wave Inversesawtooth 1 1 0 1
	}
}

textures/stecki/cameraflash3
{
	qer_editorimage textures/stecki/cameraflash_01.tga
      surfaceparm nomarks	
      surfaceparm nonsolid
	surfaceparm nolightmap
	deformVertexes autosprite

	{
	animMap 2 textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_01.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga
		blendFunc add
		rgbGen wave Inversesawtooth 0 2 0 2
//		rgbGen wave Inversesawtooth 1 1 0 1
	}
}

textures/stecki/cameraflash4
{
	qer_editorimage textures/stecki/cameraflash_01.tga
      surfaceparm nomarks	
      surfaceparm nonsolid
	surfaceparm nolightmap
	deformVertexes autosprite

	{
	animMap 2 textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_01.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga
		blendFunc add
		rgbGen wave Inversesawtooth 0 2 0 2
//		rgbGen wave Inversesawtooth 1 1 0 1
	}
}

textures/stecki/cameraflash5
{
	qer_editorimage textures/stecki/cameraflash_01.tga
      surfaceparm nomarks	
      surfaceparm nonsolid
	surfaceparm nolightmap
	deformVertexes autosprite

	{
	animMap 2 textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_02.tga textures/stecki/cameraflash_01.tga textures/stecki/cameraflash_02.tga
		blendFunc add
		rgbGen wave Inversesawtooth 0 2 0 2
//		rgbGen wave Inversesawtooth 1 1 0 1
	}
}


textures/stecki/night01
{
	qer_editorimage textures/stecki/night01_up.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm sky
//        q3map_lightimage textures/stecki/night01_up.tga
	q3map_sun	.9 .8 1 30 30 60
	q3map_surfacelight 5

        skyparms textures/stecki/night01 512 textures/stecki/night01

}

textures/stecki/decal01
{
	qer_editorimage gfx/damage/plasma_mrk.tga
	polygonOffset
	{
		map gfx/damage/plasma_mrk.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		alphaGen vertex
	}
}

