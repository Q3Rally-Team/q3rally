models/gibs/muffler

{

	{
            map textures/sfx/specular.tga
		tcGen environment
	}
	{
		map models/gibs/gibs.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

models/gibs/piston

{

	{
            map textures/sfx/specular.tga
		tcGen environment
	}
	{
		map models/gibs/gibs.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}


models/gibs/shock

{
	cull disable
	{
		map models/gibs/gibs.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
	}
}

models/gibs/steer

{
	{
		map models/gibs/gibs.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
	}
}

