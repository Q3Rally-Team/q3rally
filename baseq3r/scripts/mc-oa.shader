textures/mc-oa-dm02/gc_lite1
{
	qer_editorimage textures/mc-oa-dm02/gc_lite1.blend.tga
	q3map_lightimage textures/mc-oa-dm02/gc_lite1.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 800
	q3map_flare flareShader-twilightflare
	{
		map textures/mc-oa-dm02/gc_lite1.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/mc-oa-dm02/gc_lite1.blend.tga
		blendfunc add
	}
}


textures/mc-oa-dm02/gc_lite2
{
	qer_editorimage textures/mc-oa-dm02/gc_lite1.blend.tga
	q3map_lightimage textures/mc-oa-dm02/gc_lite1.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 400
	q3map_flare flareShader-twilightflare
	{
		map textures/mc-oa-dm02/gc_lite2.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/mc-oa-dm02/gc_lite1.blend.tga
		blendfunc add
	}
}



textures/mc-oa-dm02/logo2
{       	
	cull none
        q3map_surfacelight 400
        surfaceparm trans
        surfaceparm nomarks
	q3map_flare flareShader-twilightflare
	{
		map textures/mc-oa-dm02/logo2.jpg
		blendFunc Add
		
                  
	}

}




textures/mc-oa-dm02/teleport
{       	
	cull none
        surfaceparm nolightmap
        surfaceparm trans
        surfaceparm nomarks
	{
		map textures/mc-oa-dm02/teleport.jpg
		blendFunc Add
		
                  
	}

}


textures/mc-oa-dm02/mc-oa-dm02
{
	qer_editorimage textures/mc-oa-dm02/mc-oa-dm02.jpg
	surfaceparm sky
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_lightsubdivide 256
	q3map_surfacelight 88
	q3map_sun 1 1 1 111 260 30
	skyparms env/mc-oa-dm02/mc-oa-dm02 - -
}


textures/mc-oa-dm04/ano-trim2
{
	qer_editorimage textures/mc-oa-dm04/ano-trim2.tga
	q3map_lightimage textures/mc-oa-dm04/ano-trim2.blend.tga
	surfaceparm nomarks
	q3map_lightsubdivide 32
	q3map_surfacelight 500
	{
		map textures/mc-oa-dm04/ano-trim2.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/mc-oa-dm04/ano-trim2.blend.tga
		blendfunc add
	}
}


textures/mc-oa-dm04/b_lamp_s_2k
{
	qer_editorimage textures/mc-oa-dm04/b_lamp.tga
	q3map_surfacelight 2000	
	q3map_flare flareShader-subtle
	{
		map textures/mc-oa-dm04/b_lamp.tga
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
	}
}


textures/mc-oa-dm04/anodm2_grey0_jumper
{
	qer_editorimage textures/mc-oa-dm04/anodm2_grey0_jumper.tga
	q3map_lightimage textures/mc-oa-dm04/anodm2_grey0_jumper_blend.tga
	q3map_surfacelight 400
	q3map_flare flareShader-subtle
	{
		map textures/mc-oa-dm04/anodm2_grey0_jumper.tga
		rgbGen identity
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/mc-oa-dm04/anodm2_grey0_jumper_blend.tga
		blendfunc add
		rgbGen wave sin 0 1 0 1 
	}
}

textures/anoice1/anoice1
{
	qer_editorimage textures/skies/anoice1.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_lightsubdivide 256
	q3map_surfacelight 25
	surfaceparm sky
	q3map_sun 0.65 0.75 0.85 195 295 14
	skyparms env/anoice1/anoice1 - -
	{
		map textures/skies/anoice1.jpg
		blendfunc filter
		tcmod scale 3 2
		tcmod scroll .01 .02
		rgbgen identity
	}
}


textures/e7/e7trimlight
{
	qer_editorimage textures/e7/e7trimlight.tga
	q3map_lightimage textures/e7/e7trimlight.blend.tga
	surfaceparm nomarks
	q3map_surfacelight 2000
	q3map_flare flareShader-subtle
	{
		map $lightmap 
		rgbGen identity
		tcGen lightmap 
	}
	{
		map textures/e7/e7trimlight.jpg
		blendfunc filter
		rgbGen identity
	}
	{
		map textures/e7/e7trimlight_glow.jpg
		blendfunc add
		rgbGen identity
	}
}

textures/amph-dm01/bloodsplat
{
	qer_editorimage gfx/damage/blood_stain.tga
	surfaceparm noimpact
	surfaceparm nonsolid
	surfaceparm trans
	nopicmip
	polygonoffset
	qer_trans 0.5
	{
		clampmap gfx/damage/blood_stain.tga
		blendfunc blend
		rgbGen identityLighting
		alphaGen wave sin 0.8 0 0 0 
	}
	{
		map $lightmap 
		blendfunc filter
		rgbGen identity
		tcGen lightmap 
		depthFunc equal
	}
}

// This file was automatically created by Particle Studio.
// For more information about Particle Studio, please
// visit http://www.quake3stuff.com/freebrief

// If this file was created with work-around code switched on,
// you will have to remove all occurences of the string
// "//-UNCMT4RELEASE-//" after you compile the map.
// 
// If you forget to do this,  your particles will look flat and 
// will have misaligned textures.

textures/amph-dm01/bigflame_1
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 14.487381 -13.910664 118.307266 sawtooth 0 1 0.998260 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.998260 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.998260 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.998260 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_2
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -7.016427 3.758277 119.735725 sawtooth 0 1 0.209784 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.209784 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.209784 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.209784 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_3
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -16.138309 -7.144148 118.695061 sawtooth 0 1 1.931211 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.931211 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 1.931211 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 1.931211 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_4
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 5.806557 18.766113 118.381241 sawtooth 0 1 0.566668 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.566668 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.566668 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.566668 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_5
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 10.687401 4.516825 119.437752 sawtooth 0 1 1.523728 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.523728 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 1.523728 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 1.523728 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_6
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -18.253271 -1.098753 118.598526 sawtooth 0 1 1.432966 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.432966 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 1.432966 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 1.432966 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_7
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -0.667288 15.046935 119.051018 sawtooth 0 1 0.169134 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.169134 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.169134 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.169134 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_8
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 8.815632 9.449194 119.302124 sawtooth 0 1 0.451979 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.451979 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.451979 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.451979 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_9
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 19.453594 -0.509069 118.411560 sawtooth 0 1 1.488632 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.488632 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 1.488632 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 1.488632 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_10
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -7.210326 4.673981 119.691963 sawtooth 0 1 1.764763 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.764763 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 1.764763 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 1.764763 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_11
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 12.416848 -19.805067 117.701233 sawtooth 0 1 1.394696 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.394696 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 1.394696 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 1.394696 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_12
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 16.189322 -6.563046 118.721657 sawtooth 0 1 0.060976 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.060976 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.060976 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.060976 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_13
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 5.122133 -6.991651 119.686592 sawtooth 0 1 0.831141 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.831141 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.831141 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.831141 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_14
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 7.857406 -17.997166 118.382271 sawtooth 0 1 1.543321 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.543321 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 1.543321 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 1.543321 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_15
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -6.321534 -12.502711 119.179367 sawtooth 0 1 1.092685 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.092685 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 1.092685 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 1.092685 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_16
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -19.660521 15.761874 117.324448 sawtooth 0 1 0.519425 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.519425 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.519425 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.519425 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_17
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -6.765626 -21.424593 117.877960 sawtooth 0 1 1.916013 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.916013 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 1.916013 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 1.916013 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_18
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -1.546627 14.717710 119.083984 sawtooth 0 1 0.169988 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.169988 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.169988 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.169988 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_19
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -7.622227 27.918009 116.458092 sawtooth 0 1 0.971892 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.971892 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.971892 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.971892 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_20
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 7.277077 -18.968523 118.267654 sawtooth 0 1 1.903012 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.903012 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 1.903012 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 1.903012 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_21
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -7.032858 -7.769308 119.541527 sawtooth 0 1 1.239479 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.239479 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 1.239479 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 1.239479 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_22
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -12.519408 13.984274 118.523018 sawtooth 0 1 0.848903 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.848903 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.848903 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.848903 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_23
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -7.723121 -15.031857 118.804024 sawtooth 0 1 0.242683 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.242683 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.242683 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.242683 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_24
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 4.892597 10.805710 119.412308 sawtooth 0 1 0.729026 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.729026 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.729026 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.729026 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_25
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 7.658578 10.057648 119.332268 sawtooth 0 1 0.545183 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.545183 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.545183 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.545183 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_26
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -19.965691 -4.723410 118.233078 sawtooth 0 1 0.783532 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.783532 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.783532 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.783532 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_27
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -5.651424 -25.341784 117.157394 sawtooth 0 1 0.460707 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.460707 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.460707 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.460707 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_28
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 4.900815 9.115550 119.552872 sawtooth 0 1 1.936460 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.936460 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 1.936460 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 1.936460 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_29
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -6.073101 14.794571 118.929550 sawtooth 0 1 1.912168 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.912168 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 1.912168 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 1.912168 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_30
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 19.466974 -15.948678 117.331490 sawtooth 0 1 0.142827 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.142827 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.142827 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.142827 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_31
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 0.384862 25.995123 125.331978 sawtooth 0 1 0.451247 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.451247 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 0.451247 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 0.451247 0.250000
blendfunc add
}
}

textures/amph-dm01/bigflame_32
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -1.402318 -5.186944 127.887177 sawtooth 0 1 1.746940 0.250000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.746940 0.250000
rgbGen wave sawtooth 1.000000 -1.000000 1.746940 0.250000
tcMod stretch sawtooth 0.500000 -0.200000 1.746940 0.250000
blendfunc add
}
}



textures/amph-dm01/smoke_1
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 1.358843 -37.477783 317.794861 sawtooth 0 1 -0.953490 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.953490 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.953490 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.953490 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_2
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -0.823239 -0.075231 319.998932 sawtooth 0 1 -0.787408 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.787408 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.787408 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.787408 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_3
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 42.185074 32.731373 315.513977 sawtooth 0 1 -0.207801 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.207801 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.207801 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.207801 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_4
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 35.399746 -40.397751 315.459778 sawtooth 0 1 0.139683 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.139683 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.139683 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.139683 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_5
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -1.701365 -34.428753 318.137970 sawtooth 0 1 -0.549242 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.549242 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.549242 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.549242 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_6
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -0.784748 -36.698318 317.887756 sawtooth 0 1 -0.348674 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.348674 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.348674 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.348674 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_7
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 4.421463 36.835098 317.842133 sawtooth 0 1 -0.026276 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.026276 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.026276 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.026276 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_8
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -40.892712 10.479141 317.203369 sawtooth 0 1 0.279336 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.279336 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.279336 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.279336 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_9
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 45.005230 -23.512926 315.945679 sawtooth 0 1 -0.408124 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.408124 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.408124 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.408124 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_10
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -18.191473 7.958837 319.383362 sawtooth 0 1 -0.099155 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.099155 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.099155 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.099155 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_11
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 40.507736 -25.955259 316.362854 sawtooth 0 1 -0.114231 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.114231 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.114231 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.114231 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_12
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 38.040527 10.251256 317.565491 sawtooth 0 1 -0.659413 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.659413 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.659413 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.659413 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_13
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -33.592258 23.976284 317.327423 sawtooth 0 1 0.558763 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.558763 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.558763 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.558763 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_14
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 2.903774 -0.896560 319.985565 sawtooth 0 1 0.104709 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.104709 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.104709 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.104709 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_15
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 18.134617 0.602679 319.485168 sawtooth 0 1 -0.332133 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.332133 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.332133 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.332133 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_16
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -18.084633 13.425112 319.206360 sawtooth 0 1 -0.804498 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.804498 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.804498 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.804498 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_17
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 48.572956 -9.800095 316.140198 sawtooth 0 1 -0.039399 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.039399 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.039399 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.039399 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_18
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 19.364388 -20.178679 318.775543 sawtooth 0 1 0.207739 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.207739 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.207739 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.207739 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_19
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -27.078072 38.832603 316.478760 sawtooth 0 1 -0.607410 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.607410 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.607410 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.607410 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_20
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -3.316522 27.102671 318.832947 sawtooth 0 1 0.067965 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.067965 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.067965 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.067965 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_21
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -18.958414 21.307333 318.726471 sawtooth 0 1 -0.891659 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.891659 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.891659 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.891659 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_22
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -17.469976 -11.749996 319.306671 sawtooth 0 1 -0.903134 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.903134 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.903134 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.903134 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_23
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -7.611027 41.972191 317.144135 sawtooth 0 1 0.404462 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.404462 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.404462 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.404462 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_24
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 0.795594 -51.218662 315.873413 sawtooth 0 1 0.793329 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.793329 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.793329 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.793329 0.100000
blendfunc blend
}
}

textures/amph-dm01/smoke_25
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 40.506279 14.404134 317.098969 sawtooth 0 1 0.714896 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.714896 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.714896 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.714896 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_1
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 31.612247 37.168587 316.258057 sawtooth 0 1 -0.545396 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.545396 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.545396 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.545396 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_2
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 4.662729 13.519415 319.680298 sawtooth 0 1 0.613392 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.613392 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.613392 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.613392 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_3
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 5.134696 -40.541519 317.379944 sawtooth 0 1 -0.967589 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.967589 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.967589 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.967589 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_4
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 32.009045 11.956154 318.170502 sawtooth 0 1 -0.528123 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.528123 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.528123 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.528123 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_5
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 5.528597 -8.680475 319.834473 sawtooth 0 1 -0.136509 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.136509 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.136509 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.136509 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_6
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -8.774223 -15.174442 319.519562 sawtooth 0 1 -0.002411 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.002411 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.002411 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.002411 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_7
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -15.742773 29.356747 318.261444 sawtooth 0 1 -0.110691 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.110691 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.110691 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.110691 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_8
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -21.931385 4.030158 319.222137 sawtooth 0 1 0.817316 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.817316 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.817316 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.817316 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_9
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -11.029301 32.350861 318.169434 sawtooth 0 1 0.043306 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.043306 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.043306 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.043306 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_10
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -10.657458 17.467329 319.345123 sawtooth 0 1 0.939695 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.939695 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.939695 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.939695 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_11
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -9.503635 14.395289 319.534760 sawtooth 0 1 0.851253 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.851253 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.851253 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.851253 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_12
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -51.839466 14.864049 315.423096 sawtooth 0 1 0.954711 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.954711 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.954711 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.954711 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_13
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -1.243763 -4.623316 319.964172 sawtooth 0 1 0.828242 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.828242 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.828242 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.828242 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_14
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 11.227793 5.515512 319.755402 sawtooth 0 1 0.594714 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.594714 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.594714 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.594714 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_15
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -6.854241 11.026029 319.736511 sawtooth 0 1 0.671560 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.671560 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.671560 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.671560 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_16
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 49.911999 4.185029 316.055817 sawtooth 0 1 0.591906 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.591906 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.591906 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.591906 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_17
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 32.961426 -17.873653 317.795654 sawtooth 0 1 0.282449 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.282449 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.282449 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.282449 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_18
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 8.780898 -6.144862 319.820496 sawtooth 0 1 -0.651906 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.651906 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.651906 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.651906 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_19
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 2.521881 -0.837303 319.988983 sawtooth 0 1 -0.157811 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.157811 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.157811 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.157811 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_20
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -3.657408 -6.301523 319.917053 sawtooth 0 1 -0.453841 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.453841 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.453841 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.453841 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_21
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -4.898691 36.754173 317.844513 sawtooth 0 1 -0.234779 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.234779 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.234779 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.234779 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_22
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -35.161621 -4.592112 318.029205 sawtooth 0 1 -0.272988 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.272988 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.272988 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.272988 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_23
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 7.775577 -51.699711 315.700317 sawtooth 0 1 -0.394269 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.394269 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.394269 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.394269 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_24
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -7.162114 -1.261538 319.917358 sawtooth 0 1 0.282205 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 0.282205 0.100000
rgbGen wave sawtooth 1.000000 0.000000 0.282205 0.100000
tcMod stretch sawtooth 0.250000 0.750000 0.282205 0.100000
blendfunc blend
}
}

textures/mc-sm19-amphetamine/smoke_25
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 2.125360 -3.279844 319.976135 sawtooth 0 1 -0.844783 0.100000
{
clampmap textures/amph-dm01/smoke.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 0.900000 -0.900000 -0.844783 0.100000
rgbGen wave sawtooth 1.000000 0.000000 -0.844783 0.100000
tcMod stretch sawtooth 0.250000 0.750000 -0.844783 0.100000
blendfunc blend
}
}


textures/mc-sm19-amphetamine/spark_1
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -11.925312 29.985764 146.490326 sawtooth 0 1 0.087225 0.666733
{
clampmap textures/mc-sm19-amphetamine/spark.jpg
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.087225 0.666733
rgbGen wave sawtooth 1.000000 -1.000000 0.087225 0.666733
tcMod stretch sawtooth 0.500000 -0.375000 0.087225 0.666733
blendfunc add
}
}

textures/mc-sm19-amphetamine/spark_2
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -104.517090 -89.074257 59.711781 sawtooth 0 1 -0.095013 0.668232
{
clampmap textures/mc-sm19-amphetamine/spark.jpg
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 -0.095013 0.668232
rgbGen wave sawtooth 1.000000 -1.000000 -0.095013 0.668232
tcMod stretch sawtooth 0.500000 -0.375000 -0.095013 0.668232
blendfunc add
}
}

textures/mc-sm19-amphetamine/spark_3
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -35.743641 66.884636 140.850006 sawtooth 0 1 -0.074236 0.626726
{
clampmap textures/mc-sm19-amphetamine/spark.jpg
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 -0.074236 0.626726
rgbGen wave sawtooth 1.000000 -1.000000 -0.074236 0.626726
tcMod stretch sawtooth 0.500000 -0.375000 -0.074236 0.626726
blendfunc add
}
}

textures/mc-sm19-amphetamine/spark_4
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -81.889450 -100.071449 93.164749 sawtooth 0 1 0.013425 0.627283
{
clampmap textures/mc-sm19-amphetamine/spark.jpg
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.013425 0.627283
rgbGen wave sawtooth 1.000000 -1.000000 0.013425 0.627283
tcMod stretch sawtooth 0.500000 -0.375000 0.013425 0.627283
blendfunc add
}
}

textures/mc-sm19-amphetamine/spark_5
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 5.242358 99.504318 108.932251 sawtooth 0 1 -0.055541 0.676629
{
clampmap textures/mc-sm19-amphetamine/spark.jpg
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 -0.055541 0.676629
rgbGen wave sawtooth 1.000000 -1.000000 -0.055541 0.676629
tcMod stretch sawtooth 0.500000 -0.375000 -0.055541 0.676629
blendfunc add
}
}

textures/mc-sm19-amphetamine/spark_6
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -75.135094 -18.878418 128.561081 sawtooth 0 1 -0.023869 0.665988
{
clampmap textures/mc-sm19-amphetamine/spark.jpg
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 -0.023869 0.665988
rgbGen wave sawtooth 1.000000 -1.000000 -0.023869 0.665988
tcMod stretch sawtooth 0.500000 -0.375000 -0.023869 0.665988
blendfunc add
}
}

textures/mc-sm19-amphetamine/spark_7
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -87.136635 -28.255167 118.201042 sawtooth 0 1 -0.048436 0.670002
{
clampmap textures/mc-sm19-amphetamine/spark.jpg
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 -0.048436 0.670002
rgbGen wave sawtooth 1.000000 -1.000000 -0.048436 0.670002
tcMod stretch sawtooth 0.500000 -0.375000 -0.048436 0.670002
blendfunc add
}
}

textures/amph-dm01/flame_1
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -3.138860 4.938580 31.460421 sawtooth 0 1 1.581347 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.581347 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 1.581347 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 1.581347 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_2
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -1.383416 -3.992448 31.719812 sawtooth 0 1 0.439833 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.439833 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 0.439833 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 0.439833 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_3
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -0.543314 4.947684 31.610525 sawtooth 0 1 0.857692 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.857692 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 0.857692 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 0.857692 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_4
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 2.346619 -3.449747 31.726843 sawtooth 0 1 1.227882 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.227882 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 1.227882 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 1.227882 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_5
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -2.889451 -4.092015 31.605482 sawtooth 0 1 1.756096 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.756096 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 1.756096 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 1.756096 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_6
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -0.956255 -2.203811 31.909697 sawtooth 0 1 0.415784 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.415784 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 0.415784 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 0.415784 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_7
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -4.829274 2.156717 31.559891 sawtooth 0 1 1.146092 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.146092 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 1.146092 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 1.146092 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_8
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -2.207697 -1.780512 31.874063 sawtooth 0 1 1.027436 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.027436 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 1.027436 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 1.027436 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_9
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 5.220461 -4.537048 31.243589 sawtooth 0 1 1.954955 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.954955 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 1.954955 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 1.954955 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_10
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -2.573873 -3.949968 31.650797 sawtooth 0 1 1.407453 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.407453 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 1.407453 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 1.407453 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_11
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -3.296790 1.819327 31.777683 sawtooth 0 1 1.949705 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.949705 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 1.949705 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 1.949705 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_12
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -3.148344 2.032721 31.779804 sawtooth 0 1 0.638203 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.638203 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 0.638203 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 0.638203 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_13
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move 4.288984 5.172560 31.286566 sawtooth 0 1 1.934873 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.934873 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 1.934873 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 1.934873 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_14
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -1.930119 -2.204002 31.865608 sawtooth 0 1 1.092502 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.092502 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 1.092502 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 1.092502 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_15
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -4.680904 -1.466531 31.621803 sawtooth 0 1 0.549028 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 0.549028 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 0.549028 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 0.549028 0.500000
blendfunc add
}
}

textures/amph-dm01/flame_16
{
qer_editorimage textures/particle_studio/editor.tga
surfaceparm noimpact
surfaceparm nolightmap
cull none
surfaceparm trans
surfaceparm nonsolid
surfaceparm nodlight
deformvertexes autosprite
deformvertexes move -4.112585 -4.023477 31.478537 sawtooth 0 1 1.492965 0.500000
{
clampmap textures/amph-dm01/flame.tga
tcMod rotate 0.000000
AlphaGen wave sawtooth 1.000000 0.000000 1.492965 0.500000
rgbGen wave sawtooth 1.000000 -1.000000 1.492965 0.500000
tcMod stretch sawtooth 0.200000 -0.100000 1.492965 0.500000
blendfunc add
}
}

textures/amph-dm01/fireball
{
	qer_editorimage textures/amph-dm01/spark.tga
	surfaceparm nodlight
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nonsolid
	surfaceparm trans
	cull disable
	deformVertexes autosprite
	qer_trans 0.5
	{
		clampmap textures/amph-dm01/spark.tga
		blendfunc add
		tcMod rotate 180
	}
}
