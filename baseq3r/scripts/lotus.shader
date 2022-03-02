//red
models/players/lotus/red
{
    {
        map models/players/lotus/red.tga
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        rgbGen lightingdiffuse
        depthWrite
    }
    {
        map textures/reflect/reflect.tga
        tcGen environment
        blendFunc add
    }
    {
        map models/players/lotus/red_spec_mask.tga
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        rgbGen lightingdiffuse
        
    }
}

//blue
models/players/lotus/blue
{
    {
        map models/players/lotus/blue.tga
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        rgbGen lightingdiffuse
        depthWrite
    }
    {
        map textures/reflect/reflect.tga
        tcGen environment
        blendFunc add
    }
    {
        map models/players/lotus/blue_spec_mask.tga
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        rgbGen lightingdiffuse
    }
}

//green
models/players/lotus/green
{
    {
        map models/players/lotus/green.tga
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        rgbGen lightingdiffuse
        depthWrite
    }
    {
        map textures/reflect/reflect.tga
        tcGen environment
        blendFunc add
    }
    {
        map models/players/lotus/green_spec_mask.tga
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        rgbGen lightingdiffuse
    }
}

//yellow
models/players/lotus/yellow
{
    {
        map models/players/lotus/yellow.tga
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        rgbGen lightingdiffuse
        depthWrite
    }
    {
        map textures/reflect/reflect.tga
        tcGen environment
        blendFunc add
    }
    {
        map models/players/lotus/yellow_spec_mask.tga
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        rgbGen lightingdiffuse
    }
}

models/players/lotus/chrome
{
    {
        map textures/reflect/chrometest2.jpg
        tcGen environment
        rgbGen lightingdiffuse
    }
    {
        map models/players/lotus/shine.tga
        blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        depthWrite
        rgbGen lightingdiffuse
    } 
}

models/players/lotus/wiper
{
	cull disable
	{
		map models/players/lotus/wiper.tga
		// opacity mask
        blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}

models/players/lotus/headLights
{
    {
        map models/players/lotus/shine.tga
        rgbGen lightingdiffuse
    }
    {
        map models/players/lotus/shineEmissive.tga
        blendFunc add
        rgbGen lightingdiffuse
    }
}

models/players/lotus/tailLights
{
    {
        map models/players/lotus/red.tga
        rgbGen lightingdiffuse
    }
    {
        map models/players/lotus/bodyEmissive.tga
        blendFunc add
        rgbGen lightingdiffuse
    }  
}