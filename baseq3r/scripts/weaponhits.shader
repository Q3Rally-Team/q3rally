railDic
{
	cull disable
	deformVertexes autosprite
	{
		clampmap textures/oafx/plastrail.tga
		blendfunc gl_one gl_src_alpha
		rgbGen wave inversesawtooth 0 1 0 1.3 
		tcMod rotate -460
	}
	{
		clampmap textures/oafx/plastrail.tga
		blendfunc gl_one gl_src_alpha
		rgbGen wave inversesawtooth 0 1 0 1.3 
		tcMod rotate 754
	}
}

oldbulletExplosion
{
	cull disable
	{
		map models/weaphits/bulletscroll.tga
		blendfunc add
		tcMod scroll -1.4 0
	}
}

bitoutofdatebulletExplosion
{
	cull disable
	{
		map models/weaphits/bulletscroll2.tga
		blendfunc gl_one gl_src_alpha
		rgbGen wave inversesawtooth 0 1 0 2.8 
		tcMod scroll -1.5 0
	}
	{
		map models/weaphits/bulletscroll2.tga
		blendfunc gl_one gl_src_alpha
		rgbGen wave inversesawtooth 0 1 0 2.8 
		tcMod scroll -1.5 0
	}
}

bulletExplosion
{
	cull disable
	{
		animmap 12 models/weaphits/bullet_0000.tga models/weaphits/bullet_0001.tga models/weaphits/bullet_0002.tga models/weaphits/bullet_0003.tga models/weaphits/bullet_0004.tga models/weaphits/bullet_0005.tga models/weaphits/bullet_0006.tga models/weaphits/bullet_0007.tga 
		blendfunc add
	}
}

lightningExplosion
{
	cull disable
	deformVertexes wave 9 sin 0 1 0 9 
	{
		map models/weaphits/elecscroll.tga
		blendfunc add
		tcMod scroll -8 0
	}
}

bfgExplocsion
{
	cull disable
	{
		map models/weaphits/bfgscroll.tga
		blendfunc add
		tcMod scroll -1.4 0
	}
	{
		map models/weaphits/bfgscroll.tga
		blendfunc add
		tcMod scroll -0.6 0
	}
}

bfgExplosion
{
	{
		clampmap textures/oa/bfgfiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1.6 
		tcMod rotate 77
		tcMod stretch sin 0.3 0.7 0 0.6 
	}
	{
		clampmap textures/oa/bfgfiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1.6 
		tcMod rotate -17
		tcMod stretch sin 0 1.3 0 0.8 
	}
	{
		clampmap textures/oa/bfgfiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1.6 
		tcMod rotate -77
		tcMod stretch sawtooth 0 1.3 0 0.8 
	}
}

plasmaExplosion
{
	cull disable
	deformVertexes wave 1 sin 0 1 0 2 
	{
		clampmap models/weaphits/plasring.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1.6 
		tcMod stretch sawtooth 0 4.3 0 1.4 
		tcMod rotate -37
	}
	{
		clampmap models/weaphits/plasring.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1.6 
		tcMod stretch sawtooth 0 4.5 0 1.4 
		tcMod rotate 66
	}
}

railExplosion
{
	cull disable
	{
		clampmap models/weaphits/smokering2.tga
		blendfunc blend
		rgbGen lightingDiffuse
		tcMod stretch sawtooth 0 4.5 0 1.3 
		tcMod rotate -80
		alphaGen wave inversesawtooth 0 1 0 1.65 
	}
	{
		clampmap models/weaphits/smokering2.tga
		blendfunc blend
		rgbGen lightingDiffuse
		tcMod stretch sawtooth 0 4.5 0 1.4 
		tcMod rotate 66
		alphaGen wave inversesawtooth 0 1 0 1.65 
	}
	{
		clampmap models/weaphits/spark.tga
		blendfunc gl_src_alpha gl_one
		rgbGen identity
		tcMod stretch sawtooth 1.5 -3.5 0 1.1 
		tcMod rotate -80
		alphaGen wave inversesawtooth 0 1 0 1.65 
	}
}

OLDplasmaExplosion
{
	cull disable
	{
		map models/weaphits/plasscroll2.tga
		blendfunc add
		tcMod scroll -1.2 7
	}
}

OLDrailExplosion
{
	cull disable
	{
		map models/weaphits/railscroll.tga
		blendfunc blend
		rgbGen lightingDiffuse
		tcMod scroll -1 -0.06
	}
	{
		map models/weaphits/plasscroll2.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 0.6 
		tcMod scroll 0 1
	}
}

OLDrocketExplosion
{
	{
		clampmap textures/oa/fiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate 77
		tcMod stretch sin 0.3 0.4 0 0.4 
	}
	{
		clampmap textures/oa/fiar2.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate 33
		tcMod stretch sin 0.8 -0.4 0 0.1 
	}
	{
		clampmap textures/oa/fiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate -17
	}
	{
		clampmap textures/oa/fiar2.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate -77
	}
}

bloodExplosion
{
	{
		clampmap gfx/2d/bloodspew.tga
		blendfunc blend
		tcMod rotate 77
		tcMod stretch sin 0 2 0 0.4 
		alphaGen wave inversesawtooth 0 1 0 1 
	}
	{
		clampmap gfx/2d/bloodspew.tga
		blendfunc blend
		tcMod rotate -43
		tcMod stretch sin 0 1.7 0 0.4 
		alphaGen wave inversesawtooth 0 1 0 1 
	}
}

olderrocketExplosion
{
	{
		clampmap textures/oa/fiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate 77
		tcMod stretch sin 0 1 0 0.4 
	}
	{
		clampmap textures/oa/fiar2.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate 33
		tcMod stretch sin 0 1 0 0.4 
	}
	{
		clampmap textures/oa/fiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate -17
		tcMod stretch sawtooth 0 1.8 0 1 
	}
	{
		clampmap textures/oa/fiar2.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate -77
		tcMod stretch sawtooth 0.3 1 0 1 
	}
}

rocketExplosion
{
	{
		animmap 8 models/weaphits/rlboom/rlboom_1.tga models/weaphits/rlboom/rlboom_2.tga models/weaphits/rlboom/rlboom_3.tga models/weaphits/rlboom/rlboom_4.tga models/weaphits/rlboom/rlboom_5.tga models/weaphits/rlboom/rlboom_6.tga models/weaphits/rlboom/rlboom_7.tga models/weaphits/rlboom/rlboom_8.tga 
		blendfunc gl_one gl_src_alpha
		rgbGen wave inversesawtooth 0 1 0 8 
	}
	{
		animmap 8 models/weaphits/rlboom/rlboom_2.tga models/weaphits/rlboom/rlboom_3.tga models/weaphits/rlboom/rlboom_4.tga models/weaphits/rlboom/rlboom_5.tga models/weaphits/rlboom/rlboom_6.tga models/weaphits/rlboom/rlboom_7.tga models/weaphits/rlboom/rlboom_8.tga models/weaphits/rlboom/rlboom_8.tga 
		blendfunc gl_one gl_src_alpha
		rgbGen wave sawtooth 0 1 0 8 
	}
	{
		clampmap textures/oa/fiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate 77
		tcMod stretch sawtooth 0 1 0 0.4 
	}
	{
		clampmap textures/oa/fiar2.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate 33
		tcMod stretch sin 0 1 0 0.4 
	}
	{
		clampmap textures/oa/fiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate -17
		tcMod stretch sawtooth 0 1.5 0 1 
	}
	{
		clampmap textures/oa/fiar2.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate -77
		tcMod stretch sawtooth 0.3 1 0 1 
	}
}

lasmaExplosion
{
	cull disable
	{
		clampmap textures/oa/fiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 4 
		tcMod rotate 300
		tcMod stretch sin 0.3 0.4 0 0.4 
	}
	{
		clampmap textures/oa/fiar2.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 4 
		tcMod rotate 122
		tcMod stretch sin 0.8 -0.7 0 0.1 
	}
}

ailExplosion
{
	cull disable
	{
		clampmap textures/oa/fiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate 12
		tcMod stretch sin 0.3 0.4 0 0.4 
	}
	{
		clampmap textures/oa/fiar2.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate 15
		tcMod stretch sin 0.8 -0.4 0 0.1 
	}
	{
		clampmap textures/oa/fiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate -12
	}
	{
		clampmap textures/oa/fiar2.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1 
		tcMod rotate -78
	}
}

OLDgrenadeExplosion
{
	{
		clampmap textures/oa/grenfiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1.6 
		tcMod rotate 77
		tcMod stretch sin 0.3 0.4 0 0.4 
	}
	{
		clampmap textures/oa/grenfiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1.6 
		tcMod rotate 33
		tcMod stretch sin 0.8 -0.4 0 0.4 
	}
	{
		clampmap textures/oa/grenfiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1.6 
		tcMod rotate -17
	}
	{
		clampmap textures/oa/grenfiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1.6 
		tcMod rotate -77
	}
}

grenadeExplosion
{
	{
		clampmap textures/oa/grenfiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1.6 
		tcMod rotate 77
		tcMod stretch sin 0 1 0 0.4 
	}
	{
		clampmap textures/oa/grenfiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1.6 
		tcMod rotate 33
		tcMod stretch sin 0 1 0 0.4 
	}
	{
		clampmap textures/oa/grenfiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1.6 
		tcMod rotate -17
		tcMod stretch sawtooth 0 1.8 0 1 
	}
	{
		clampmap textures/oa/grenfiar.tga
		blendfunc add
		rgbGen wave inversesawtooth 0 1 0 1.6 
		tcMod rotate -77
		tcMod stretch sawtooth 0.3 1 0 1 
	}
}

