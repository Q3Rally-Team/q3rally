models/weapons2/shotgun/skinoa
{
    {
        map models/weapons2/shotgun/shotgun.tga
        rgbGen lightingDiffuse
    }
    {
        map models/weapons2/shotgun/shotgun_spec.jpg
        blendFunc GL_SRC_ALPHA GL_ONE
        detail
        alphaGen lightingSpecular
    }
}

models/weapons2/shotgun/shotgun_laser
{
	
	{
		map	models/weapons2/shotgun/shotgun_laser.tga
		blendfunc add
	}
}

models/weapons2/shells/sgunshell_2
{
	deformVertexes autosprite
    {
        map models/weapons2/shells/sgunshell_2
        rgbGen lightingDiffuse
	blendFunc blend
    }

}

