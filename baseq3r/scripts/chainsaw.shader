models/weapons2/gauntlet/blade
{
//	{
//		map textures/sfx/specular.tga
//		blendFunc GL_ONE GL_ZERO
//		tcGen environment
//		rgbGeb identity
//	}
	{
		map models/weapons2/gauntlet/blade.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
//		rgbGen lightingDiffuse
//		rgbGen identity
	}
}
models/weapons2/gauntlet/chain
{      
	{
		map models/weapons2/gauntlet/main.tga
		tcMod scroll -3 0
	//	blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	//	rgbGen lightingDiffuse
	}
}
models/weapons2/gauntlet/wheel
{      
        {
                map models/weapons2/gauntlet/env_map.tga
                blendFunc GL_ONE GL_ZERO
                tcGen environment
                //rgbGen lightingDiffuse
                rgbGen identity
	}  
        {
		map models/weapons2/gauntlet/wheel.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingDiffuse
	}
}
models/weapons2/gauntlet/tube
{      
        {
                map models/weapons2/gauntlet/tube2.tga
                blendFunc GL_ONE GL_ZERO
        //	tcGen environment
        //	rgbGen lightingDiffuse
		tcMod scroll .1 .75
                rgbGen identity
	}  
        {
		map models/weapons2/gauntlet/tube.tga
                blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingDiffuse
	}
}
models/weapons2/gauntlet/main
{
	sort additive
	{
		map models/weapons2/gauntlet/main.tga
	}
	{
		map models/weapons2/gauntlet/mainlite.jpg
		blendFunc add
		rgbgen wave sin .575 .25 0 10
	}
}