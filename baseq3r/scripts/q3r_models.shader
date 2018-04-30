
//******************************************************//
//	models/powerups/health subfolder		//
//******************************************************//

models/powerups/health/blue
{
	{
		map textures/effects/envmapbfg.tga
		tcGen environment
		
	}
}

models/powerups/health/blue_sphere
{
	{
		map textures/reflect/reflect.jpg
		tcGen environment
		blendfunc GL_ONE GL_ONE
	}
}

models/powerups/health/green
{
	
	{
		map textures/effects/envmaprail.tga
		tcGen environment
		
	}
}

models/powerups/health/green_sphere
{
	{
		map textures/reflect/reflect.jpg
		tcGen environment
		blendfunc GL_ONE GL_ONE
	}
}

models/powerups/health/mega1
{
	{
		map textures/effects/envmapblue2.tga
                tcGen environment
                tcmod rotate 33
                tcmod scroll 1 1
		blendfunc GL_ONE GL_ONE
	}
}

models/powerups/health/mega2
{	
	
	{
		map textures/effects/envmapblue.tga
                tcGen environment
		blendfunc GL_ONE GL_ZERO
		
	}
        {
		map textures/sfx/kenelectric.tga
                tcmod scale 2 2
                tcmod rotate 333
                tcmod scroll 9 9
		blendfunc GL_ONE GL_ONE
		
	}
}

models/powerups/health/red
{	
	
	{
		map textures/effects/envmapgold.tga
                tcGen environment
		blendfunc GL_ONE GL_ZERO
		
	}
        {
		map textures/sfx/kenelectric.tga
                tcmod scale 2 2
                tcmod rotate 333
                tcmod scroll 9 9
		blendfunc GL_ONE GL_ONE
		
	}
}

models/powerups/health/red_sphere
{
	{
		map textures/reflect/reflect.jpg
                tcGen environment
                tcmod rotate 33
                tcmod scroll 1 1
		blendfunc GL_ONE GL_ONE
	}
}

models/powerups/health/yellow
{
	
	{
		map textures/effects/envmapyel.tga
                tcGen environment
		//blendfunc GL_ONE GL_ONE
		
	}
        
}

models/powerups/health/yellow_sphere
{
	{
		map textures/reflect/reflect.jpg
		tcGen environment
		blendfunc GL_ONE GL_ONE
	}
}
