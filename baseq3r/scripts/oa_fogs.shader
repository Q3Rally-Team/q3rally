textures/fogs/fogyou
{
		surfaceparm trans
		surfaceparm nonsolid
		surfaceparm fog
		surfaceparm nolightmap
	cull disable
	fogparms ( 0.458824 0.862745 0.411765 ) 666
}

textures/fogs/fogoff
{
		surfaceparm trans
		surfaceparm nonsolid
		surfaceparm fog
		surfaceparm nolightmap
	cull disable
	fogparms ( 0.458824 0.862745 0.411765 ) 666
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll 0.3 0.2
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -0.23 -0.3
	}
}

// added 1-19-2007 for backwards compat. with q3a
// and we needed a red fog :)
textures/sfx/hellfogdense
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.65098 0.07451 0.11372 ) 100
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -0.23 -0.3
	}
}

textures/sfx/kc_hellfog_1k
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.65098 0.07451 0.11372 ) 100
	q3map_surfacelight 1000
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -0.23 -0.3
	}
}

textures/sfx/hellfog_1k
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.65098 0.07451 0.11372 ) 666
	q3map_surfacelight 1000
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll 0.3 0.2
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -0.23 -0.3
	}
}

textures/sfx/hellfog
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.65098 0.07451 0.11372 ) 800
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -.02 .02
	}
}

//01-30-07 more of the same dmn_clown
textures/sfx/xredfog
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 1 0 0.01569 ) 50
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -.02 .02
	}
}

textures/sfx/xlightgreyfog
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.62745 0.62745 0.62745 ) 3000
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -.02 .02
	}
}

textures/sfx/xbluefog
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0 0.05490 0.54902 ) 50
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -.02 .02
	}
}

textures/sfx/q3tourney3fog
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.65098 0.07451 0.11372 ) 200
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -.02 .02
	}
}

textures/sfx/q3dm9fog
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.47843 0.086275 0.086275 ) 800
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -.02 .02
	}
}

textures/sfx/q3dm14fog
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.72157 0.13 0.13 ) 800
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -.02 .02
	}
}

textures/sfx/mkc_fog_ctfred
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.65098 0.07451 0.11372 ) 300

}

textures/sfx/mkc_fog_ctfblue
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.13 0 0.73 ) 300
	
}

textures/sfx/mkc_fog_ctfblue2
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.08627 0 0.47059 ) 300
	
}

textures/sfx/mkc_fog_tdm3
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.47058 0.13725 0.14509 ) 500
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -.02 .02
	}
}

textures/sfx/fog_timdm1
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.67059 0.48235 0.21961 ) 500
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -.02 .02
	}
}

textures/sfx/constfog
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.388235 0.388235 0.388235 ) 800
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	
}

textures/sfx/fog_pjdm1
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.388235 0.388235 0.388235 ) 600
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	
}

textures/sfx/fog_q3dm10
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.78431 0.82353 0.00392 ) 300
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	
}

textures/sfx/fog_timctf1
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.78431 0.82353 0.00392 ) 100
	
}

textures/sfx/fog_timdm2
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.78431 0.82353 0.00392 ) 300
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	
}

textures/sfx/fog_timdm8
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.14509 0.47843 0 ) 500
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	
}

textures/sfx/hellfog_mini_dm10
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.458824 0.862745 0.411765 ) 300
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -.02 .02
	}
}

textures/sfx/hellfog_pj_dm10
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.458824 0.862745 0.411765 ) 500
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -.02 .02
	}
}

textures/sfx/hellfog_tim_dm14
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.458824 0.862745 0.411765 ) 300
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -.02 .02
	}
}

textures/sfx/proto_hellfog
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.458824 0.862745 0.411765 ) 300
	
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -.02 .02
	}
}

textures/sfx/xblackfog
{
        surfaceparm trans
        surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nomarks
        cull disable
        fogparms ( 0.05 0.05 0.05 ) 500
}

textures/sfx/xbluefog
{
        surfaceparm trans
        surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nomarks
        cull disable
        fogparms ( 0.13 0 0.73 ) 500
}

textures/sfx/xbluefogx128
{
        surfaceparm trans
        surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nomarks
        cull disable
        fogparms ( 0.13 0 0.73 ) 128
}

textures/sfx/xdensegreyfog
{
        surfaceparm trans
        surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nomarks
        cull disable
        fogparms ( 0.4784 0.4784 0.4784 ) 128
}

textures/sfx/xfinalfog
{
        surfaceparm trans
        surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nomarks
        cull disable
        fogparms ( 0.4784 0.4784 0.4784 ) 800
}

textures/sfx/xfinalfoginvert
{
        surfaceparm trans
        surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nomarks
        cull disable
        fogparms ( 0 0 0 ) 100
}

textures/fogs/fog_orange
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.5 0.2 0.0 ) 512
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb 1 -0.3 0 0.07
		tcMod scale 0.2 0.2
	}
}

textures/sfx/fog_intel
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	cull disable
	fogparms ( 0.65098 0.07451 0.11372 ) 300
	
	
}

textures/sfx2/ouchfog
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	q3map_globaltexture
	cull none
	fogparms ( 1 0 0 ) 50
	{
		map textures/skies/s2.tga
		blendfunc filter
		tcmod scale 0.4 0.4
		tcmod turb .01 .5 0 .05
	}
}

textures/sfx2/dist_fog
{
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nolightmap
	q3map_globaltexture
	cull none
	fogparms ( .05 .04 .03 ) 256
}

textures/sfx/xnotsodensegreyfog
{
        surfaceparm trans
        surfaceparm nonsolid
	surfaceparm fog
	surfaceparm nomarks
        cull disable
        fogparms ( 0.4784 0.4784 0.4784 ) 600
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod turb .01 .5 0 .05
	}
	{
		map textures/liquids/kc_fogcloud3.tga
		blendfunc filter
		tcMod scroll -.02 .02
	}
}

