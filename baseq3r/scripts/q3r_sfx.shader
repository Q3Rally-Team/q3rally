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

