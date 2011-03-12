// The new muzzleflash
// for The Shotgun
//The core cone - always seen
cmuz_def_cone1
{
	cull disable
	{
		map textures/oa/muzzle/muz3.tga
		blendfunc add
		rgbGen const ( 0.992157 0.533333 0.192157 )
		tcMod rotate 1024
	}
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.992157 0.533333 0.192157 )
		tcMod rotate -1024
	}
}

//The "fuzz" cone, used only for the _1 LOD mesh in subsitution
cmuz_def_cone2
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.980392 0.458824 0.0705882 )
		tcMod rotate 128
	}
}

//Only seen in highest LOD mesh for some more soft detail
cmuz_def_cone3
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.152941 0.0666667 0.00392157 )
		tcMod rotate 128
	}
}

//Muzzfuzzes for highest detail muzzleflash, four of them for maximum variance
cmuz_def_fuzz1
{
	deformVertexes autosprite
	{
		animmap 27 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz8.tga 
		blendfunc add
		rgbGen const ( 0.584314 0.352941 0.14902 )
	}
}

cmuz_def_fuzz2
{
	deformVertexes autosprite
	{
		animmap 36 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga 
		blendfunc add
		rgbGen const ( 0.584314 0.352941 0.14902 )
	}
}

cmuz_def_fuzz3
{
	deformVertexes autosprite
	{
		animmap 21 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.584314 0.352941 0.14902 )
	}
}

cmuz_def_fuzz4
{
	deformVertexes autosprite
	{
		animmap 52 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.584314 0.352941 0.14902 )
	}
}

// 
// The Grenade Launcher
// 
cmuz_grn_cone1
{
	cull disable
	{
		map textures/oa/muzzle/muz3.tga
		blendfunc add
		rgbGen const ( 0.517647 0.184314 0.184314 )
		tcMod rotate 1024
	}
	{
		map textures/oa/muzzle/muz2.tga
		blendfunc add
		rgbGen const ( 0.419608 0.0705882 0.0705882 )
		tcMod rotate -1024
	}
}

cmuz_grn_cone2
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.560784 0.121569 0.121569 )
		tcMod rotate 128
	}
}

cmuz_grn_cone3
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.0901961 0 0 )
		tcMod rotate 128
	}
}

cmuz_grn_fuzz1
{
	deformVertexes autosprite
	{
		animmap 27 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz8.tga 
		blendfunc add
		rgbGen const ( 0.505882 0.152941 0.152941 )
	}
}

cmuz_grn_fuzz2
{
	deformVertexes autosprite
	{
		animmap 36 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga 
		blendfunc add
		rgbGen const ( 0.478431 0.188235 0.188235 )
	}
}

cmuz_grn_fuzz3
{
	deformVertexes autosprite
	{
		animmap 21 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.388235 0.180392 0.180392 )
	}
}

cmuz_grn_fuzz4
{
	deformVertexes autosprite
	{
		animmap 52 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.388235 0.152941 0.152941 )
	}
}

// 
// The Rocket Launcher
// 
cmuz_rck_cone1
{
	cull disable
	{
		map textures/oa/muzzle/muz3.tga
		blendfunc add
		rgbGen const ( 0.67451 0.286275 0.156863 )
		tcMod rotate 1024
	}
	{
		map textures/oa/muzzle/muz2.tga
		blendfunc add
		rgbGen const ( 0.784314 0.317647 0.0901961 )
		tcMod rotate -1024
	}
}

cmuz_rck_cone2
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.576471 0.352941 0.192157 )
		tcMod rotate 128
	}
}

cmuz_rck_cone3
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.0352941 0.0196078 0.0156863 )
		tcMod rotate 128
	}
}

cmuz_rck_fuzz1
{
	deformVertexes autosprite
	{
		animmap 27 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz8.tga 
		blendfunc add
		rgbGen const ( 0.639216 0.203922 0.0941177 )
	}
}

cmuz_rck_fuzz2
{
	deformVertexes autosprite
	{
		animmap 36 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga 
		blendfunc add
		rgbGen const ( 0.466667 0.176471 0.0745098 )
	}
}

cmuz_rck_fuzz3
{
	deformVertexes autosprite
	{
		animmap 21 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.47451 0.133333 0.0745098 )
	}
}

cmuz_rck_fuzz4
{
	deformVertexes autosprite
	{
		animmap 52 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.337255 0.121569 0.0470588 )
	}
}

// 
// The Plasma Gun
// 
cmuz_pls_cone1
{
	cull disable
	{
		map textures/oa/muzzle/muz3.tga
		blendfunc add
		rgbGen const ( 0.133333 0.152941 0.176471 )
		tcMod rotate 1024
	}
	{
		map textures/oa/muzzle/muz2.tga
		blendfunc add
		rgbGen const ( 0.270588 0.427451 0.603922 )
		tcMod rotate -1024
	}
}

cmuz_pls_cone2
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.113725 0.145098 0.160784 )
		tcMod rotate 128
	}
}

cmuz_pls_cone3
{
	cull disable
	{
		map textures/oa/muzzle/muz6.tga
		blendfunc add
		rgbGen const ( 0.0745098 0.0862745 0.101961 )
		tcMod rotate 128
	}
}

cmuz_pls_fuzz1
{
	deformVertexes autosprite
	{
		animmap 27 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz8.tga 
		blendfunc add
		rgbGen const ( 0.196078 0.27451 0.352941 )
	}
}

cmuz_pls_fuzz2
{
	deformVertexes autosprite
	{
		animmap 36 textures/oa/muzzle/muz1.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga 
		blendfunc add
		rgbGen const ( 0.160784 0.180392 0.278431 )
	}
}

cmuz_pls_fuzz3
{
	deformVertexes autosprite
	{
		animmap 21 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.0862745 0.168627 0.313726 )
	}
}

cmuz_pls_fuzz4
{
	deformVertexes autosprite
	{
		animmap 52 textures/oa/muzzle/muz8.tga textures/oa/muzzle/muz6.tga textures/oa/muzzle/muz7.tga textures/oa/muzzle/muz5.tga textures/oa/muzzle/muz4.tga textures/oa/muzzle/muz3.tga textures/oa/muzzle/muz2.tga textures/oa/muzzle/muz1.tga 
		blendfunc add
		rgbGen const ( 0.196078 0.278431 0.372549 )
	}
}

