//======================
//	Camera Flash	
//======================

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

//================
//	UFO GFX	
//================

textures/q3r_sfx/force_green

{
	qer_editorimage models/mapobjects/ufo/ufo01glow.tga
	cull disable
	surfaceparm alphashadow
	surfaceparm playerclip
       {
	        map models/mapobjects/ufo/ufo01glow.tga
		tcMod scroll 2.1 0
		blendFunc add
               rgbGen wave sin 1 1 .5 .1
       }
      {
	        map models/mapobjects/ufo/ufo01glow.tga
		tcMod scroll 1.4 0
		blendFunc add
               rgbGen wave square 1 1 .5 2
       }
      {
	        map models/mapobjects/ufo/ufo01glow.tga
		tcMod scroll -.9 0
		blendFunc add
               rgbGen wave square 1 1 .25 .5
       }


}
