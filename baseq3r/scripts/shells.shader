powerups/invisibility
{
	cull disable
	{
		animmap 30 sprites/multwake1.tga sprites/multwake2.tga sprites/multwake3.tga sprites/multwake4.tga sprites/multwake5.tga sprites/multwake6.tga sprites/multwake7.tga sprites/multwake8.tga 
		blendfunc gl_dst_color gl_src_color
		tcMod rotate 86
		tcGen environment 
	}
}

//too invisible
powerups/invisibilityOLD
{
	cull disable
	deformVertexes wave 64 sin 0 1 0 1 
	{
		map textures/oafx/inviso.tga
		blendfunc gl_dst_color gl_src_color
		tcMod rotate 53
	}
	{
		map textures/oafx/inviso.tga
		blendfunc gl_dst_color gl_src_color
		tcMod rotate -86
	}
}

mediumcrossEnv
{
	cull disable
	{
		map textures/effects/tinfx2.tga
		blendfunc add
		tcGen environment 
	}
}

smallcrossEnv
{
	cull disable
	{
		map textures/effects/envmaprail.tga
		blendfunc add
		tcMod scroll 1 0.1
		tcGen environment 
	}
}

largecrossEnv
{
	cull disable
	{
		map textures/effects/largehelth.tga
		blendfunc add
		tcMod scroll 1 0.2
		tcMod scale 0.5 0.5
		tcGen environment 
	}
}

megacrossEnv
{
	cull disable
	{
		map textures/effects/megahelth.tga
		blendfunc add
		tcMod scroll 1 0.2
		tcGen environment 
	}
}

powerups/quadi
{
	{
		map textures/oafx/quadmultshell.tga
		blendfunc gl_dst_color gl_src_color
		tcMod rotate 53
	}
}

powerups/quad
{
	deformVertexes wave 100 sin 0.5 0 0 0 
	{
		map textures/oafx/quadshell.tga
		blendfunc add
		tcMod rotate 75
		tcGen environment 
	}
}

powerups/quadWeapon
{
	deformVertexes wave 100 sin 0.5 0 0 0 
	{
		map textures/oafx/quadshell.tga
		blendfunc add
		tcMod rotate 75
		tcGen environment 
	}
}

powerups/battleSuit
{
	deformVertexes wave 100 sin 0.5 0 0 0 
	{
		map textures/oafx/suitshell.tga
		blendfunc add
		tcMod rotate 75
	}
}

powerups/battleWeapon
{
	deformVertexes wave 100 sin 0.5 0 0 0 
	{
		map textures/oafx/suitshell.tga
		blendfunc add
		tcMod rotate 75
	}
}

powerups/regen
{
	deformVertexes wave 100 sin 0.5 0 0 0 
	{
		map textures/oafx/regenshell.tga
		blendfunc add
		tcMod rotate 75
	}
}

