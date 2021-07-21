//TELEPORT MAP OBJECT BY STECKI - stecki@quakerally.com

//THESE ARE THE INSIDE LIGHTS OF THE MAIN RING

models/mapobjects/telestecki/telelite04
{

        {
                map textures/effects/tinfx2c.tga
                tcGen environment
                rgbGen identity
        }
	{
		map models/mapobjects/telestecki/telelite01.tga
		blendFunc blend
		rgbGen vertex

	}
		{
		map models/mapobjects/telestecki/telelite_g.tga
		blendFunc add
		rgbGen identity

	}
	{
		map models/mapobjects/telestecki/telelite_i.tga
		blendFunc add
		rgbGen wave Sin 0 1 0 .5
	}
}

models/mapobjects/telestecki/telelite03
{

        {
                map textures/effects/tinfx2c.tga
                tcGen environment
                rgbGen identity
        }
	{
		map models/mapobjects/telestecki/telelite01.tga
		blendFunc blend
		rgbGen vertex

	}
		{
		map models/mapobjects/telestecki/telelite_g.tga
		blendFunc add
		rgbGen identity

	}
	{
		map models/mapobjects/telestecki/telelite_i.tga
		blendFunc add
		rgbGen wave Sin 0 1 .25 .5

	}
}

models/mapobjects/telestecki/telelite02
{
        {
                map textures/effects/tinfx2c.tga
                tcGen environment
                rgbGen identity
        }
	{
		map models/mapobjects/telestecki/telelite01.tga
		blendFunc blend
		rgbGen vertex

	}
		{
		map models/mapobjects/telestecki/telelite_g.tga
		blendFunc add
		rgbGen identity

	}
	
	{
		map models/mapobjects/telestecki/telelite_i.tga
		blendFunc add
		rgbGen wave Sin 0 1 .5 .5

	}
}

models/mapobjects/telestecki/telelite01
{
        {
                map textures/effects/tinfx2c.tga
                tcGen environment
                rgbGen identity
        }
	{
		map models/mapobjects/telestecki/telelite01.tga
		blendFunc blend
		rgbGen vertex

	}
		{
		map models/mapobjects/telestecki/telelite_g.tga
		blendFunc add
		rgbGen identity

	}
	
	{
		map models/mapobjects/telestecki/telelite_i.tga
		blendFunc add
		rgbGen wave Sin 0 1 .75 .5

	}
}

//GLOWING CENTER FAUX PARTICLES

models/mapobjects/telestecki/teleglow01
{
	deformVertexes autosprite
     	surfaceparm	trans
     	nomipmaps

	{
		clampmap models/mapobjects/telestecki/teleglow01.tga
		blendfunc gl_one gl_one
		tcMod stretch sawtooth 1 -.9 .5 1.15
		tcMod rotate 15
		rgbGen wave sawtooth .5 .5 .25 1.15
	}
	{
		clampmap models/mapobjects/telestecki/teleglow01.tga
		blendfunc gl_one gl_one
		tcMod stretch sawtooth 1 -.9 .5 1.5
		tcMod rotate -60
		rgbGen wave sawtooth .75 .5 .5 1.5
	}
}


models/mapobjects/telestecki/teleglow02
{
	deformVertexes autosprite
	surfaceparm	trans
     	nomipmaps


	{
		clampmap models/mapobjects/telestecki/teleglow01.tga
		blendfunc gl_one gl_one
		tcMod stretch sawtooth 1 -.9 0 1.5
		tcMod rotate 60
		rgbGen wave sawtooth .75 .5 .6 1.5
	}
	{
		clampmap models/mapobjects/telestecki/teleglow01.tga
		blendfunc gl_one gl_one
		tcMod stretch sawtooth 1 -.9 .5 1.20
		tcMod rotate -20
		rgbGen wave sawtooth .75 .5 .1 1.20
	}
}


//MAIN TELEPORT SURFACES

models/mapobjects/telestecki/telemain
{
        {
                map textures/effects/tinfx2c.tga
                tcGen environment
                rgbGen identity
        }
        {
                map models/mapobjects/telestecki/telemain.tga
                blendFunc blend
                rgbGen vertex
        }
}

//TELEPORTER RAILING

models/mapobjects/telestecki/telerail

{
     cull disable
        {
                map models/mapobjects/telestecki/telemain.tga
                //blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
                //alphaFunc GT0
                alphaFunc GE128
		depthWrite
		rgbGen vertex
        }


}

//TELEPAD

models/mapobjects/telestecki/telepad
{
        {
                map textures/effects/tinfx2c.tga
                tcGen environment
                rgbGen identity
        }
        {
                map models/mapobjects/telestecki/telepad.tga
                blendFunc blend
                rgbGen vertex
        }
}
