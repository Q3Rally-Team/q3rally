// powerups

//
// holdable
//

models/items/fuelcan_box
{
    {
        map models/items/envmapfuelcan.tga
        rgbGen identity
        tcMod rotate 12
        tcMod turb 0 0.5 0 0.5
        tcMod scroll 1 0.5
    }
    {
        map models/items/fuelcanfluid.tga
        blendfunc add
        rgbGen identity
        tcMod scroll 0.5 0
        tcGen environment
    }
    {
        map models/items/fuelcan_box.tga
        blendfunc blend
        rgbGen lightingDiffuse
    }
    {
        map models/items/fuelcan-glow.tga
        blendfunc gl_one_minus_dst_color gl_src_alpha
        rgbGen wave noise 0 1 0 5
    }
}


models/powerups/holdable/medkit_box
{
	{
		map models/powerups/holdable/envmapmedkit.tga
		rgbGen identity
		tcMod rotate 12
		tcMod turb 0 0.5 0 0.5
		tcMod scroll 1 0.5
	}
	{
		map models/powerups/holdable/medkitfluid.tga
		blendfunc add
		rgbGen identity
		tcMod scroll 0.5 0
		tcGen environment 
	}
	{
		map models/powerups/holdable/medkit_box.tga
		blendfunc blend
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/holdable/medkit-glow.tga
		blendfunc gl_one_minus_dst_color gl_src_alpha
		rgbGen wave noise 0 1 0 5 
	}
}

models/powerups/holdable/teleporter_box
{
	{
		map models/powerups/holdable/envmapteleporter.tga
		rgbGen identity
		tcMod rotate 12
		tcMod turb 0 0.5 0 0.5
		tcMod scroll 1 0.5
	}
	{
		map models/powerups/holdable/teleporterfluid.tga
		blendfunc add
		rgbGen identity
		tcMod scroll 0.5 0
		tcGen environment 
	}
	{
		map models/powerups/holdable/teleporter_box.tga
		blendfunc blend
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/holdable/teleporter-glow.tga
		blendfunc gl_one_minus_dst_color gl_src_alpha
		rgbGen wave noise 0 1 0 5 
	}
}

//
// ammo
//

models/powerups/ammo/bfgammo_new
{
	{
		map models/powerups/ammo/envmapbfg.tga
		rgbGen identity
		tcMod rotate 12
		tcMod turb 0 0.5 0 0.5
		tcMod scroll 1 0.5
	}
	{
		map models/powerups/ammo/bfgfluid.tga
		blendfunc add
		rgbGen identity
		tcMod scroll 0.5 0
		tcGen environment 
	}
	{
		map models/powerups/ammo/bfgammo_new.tga
		blendfunc blend
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/ammo/bfgammo-glow.tga
		blendfunc gl_one_minus_dst_color gl_src_alpha
		rgbGen wave noise 0 1 0 5 
	}
}
models/powerups/ammo/bfgammo1
{
        {
	        map models/powerups/ammo/purple.tga
		tcGen environment
		blendfunc GL_ONE GL_ZERO
                rgbGen lightingDiffuse
        }
}
models/powerups/ammo/bfgammo2
{
	{
		map models/powerups/ammo/bfgammo2.tga
		rgbGen identity
	}
	
}
models/powerups/ammo/grenammo_new
{
       {
	        map textures/effects/tinfx2d.tga
		tcGen environment
		blendfunc GL_ONE GL_ZERO
                rgbGen identity
       }
       {
	        map models/powerups/ammo/grenammo_new.tga
		blendfunc blend
                rgbGen lightingDiffuse
       }
}
models/powerups/ammo/grenammo1
{
        {
	        map models/powerups/ammo/green.tga
		tcGen environment
		blendfunc GL_ONE GL_ZERO
                rgbGen lightingDiffuse
        }
}
models/powerups/ammo/grenammo2
{
	{
		map models/powerups/ammo/grenammo2.tga
		rgbGen identity
	}
	
}
models/ammo/grenade
{
        {
                map models/ammo/grenade.tga
        }
	{
		AnimMap 7 models/ammo/grenade_r.tga models/ammo/grenade_g.tga
	       blendfunc GL_ONE GL_ONE
	}
}
models/powerups/ammo/lighammo_new
{
	{
		map models/powerups/ammo/envmapligh.tga
		rgbGen identity
		tcMod rotate 12
		tcMod turb 0 0.5 0 0.5
		tcMod scroll 1 0.5
	}
	{
		map models/powerups/ammo/lighfluid.tga
		blendfunc add
		rgbGen identity
		tcMod scroll 0.5 0
		tcGen environment 
	}
	{
		map models/powerups/ammo/lighammo_new.tga
		blendfunc blend
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/ammo/lighammo-glow.tga
		blendfunc gl_one_minus_dst_color gl_src_alpha
		rgbGen wave noise 0 1 0 5 
	}
}
models/powerups/ammo/lighammo1
{
        {
	        map models/powerups/ammo/pale_yellow.tga
		tcGen environment
		blendfunc GL_ONE GL_ZERO
                rgbGen lightingDiffuse
        }
}
models/powerups/ammo/lighammo2
{
	{
		map models/powerups/ammo/lighammo2.tga
		rgbGen identity
	}
	
}
models/powerups/ammo/machammo_new
{
       {
	        map textures/effects/tinfx2d.tga
		tcGen environment
		blendfunc GL_ONE GL_ZERO
                rgbGen identity
       }
       {
	        map models/powerups/ammo/machammo_new.tga
		blendfunc blend
                rgbGen lightingDiffuse
       }
}
models/powerups/ammo/machammo1
{
        {
	        map models/powerups/ammo/yellow.tga
		tcGen environment
		blendfunc GL_ONE GL_ZERO
                rgbGen lightingDiffuse
        }
}
models/powerups/ammo/machammo2
{
	{
		map models/powerups/ammo/machammo2.tga
		rgbGen identity
	}
	
}
models/powerups/ammo/plasammo_new
{
	{
		map models/powerups/ammo/envmapplas.tga
		rgbGen identity
		tcMod rotate 12
		tcMod turb 0 0.5 0 0.5
		tcMod scroll 1 0.5
	}
	{
		map models/powerups/ammo/plasfluid.tga
		blendfunc add
		rgbGen identity
		tcMod scroll 0.5 0
		tcGen environment 
	}
	{
		map models/powerups/ammo/plasammo_new.tga
		blendfunc blend
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/ammo/plasammo-glow.tga
		blendfunc gl_one_minus_dst_color gl_src_alpha
		rgbGen wave noise 0 1 0 5 
	}
}
models/powerups/ammo/plasammo1
{
        {
	        map models/powerups/ammo/blue.tga
		tcGen environment
		blendfunc GL_ONE GL_ZERO
                rgbGen lightingDiffuse
        }
}
models/powerups/ammo/plasammo2
{
	{
		map models/powerups/ammo/plasammo2.tga
		rgbGen identity
	}
	
}
models/powerups/ammo/railammo1
{
	{
		map textures/effects/envmaprail.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
}
models/powerups/ammo/railammo_new
{
	{
		map textures/effects/envmaprail.tga
		rgbGen identity
		tcMod rotate 12
		tcMod turb 0 0.5 0 0.5
		tcMod scroll 1 0.5
	}
	{
		map models/powerups/ammo/railfluid.tga
		blendfunc add
		rgbGen identity
		tcMod scroll 0.5 0
		tcGen environment 
	}
	{
		map models/powerups/ammo/railammo_new.tga
		blendfunc blend
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/ammo/railammo-glow.tga
		blendfunc gl_one_minus_dst_color gl_src_alpha
		rgbGen wave noise 0 1 0 5 
	}
}
models/powerups/ammo/railammo2
{
	{
		map models/powerups/ammo/railammo2.tga
		rgbGen identity
	}
	
}
models/powerups/ammo/rockammo_new
{
       {
	        map textures/effects/tinfx2d.tga
		tcGen environment
		blendfunc GL_ONE GL_ZERO
                rgbGen identity
       }
       {
	        map models/powerups/ammo/rockammo_new.tga
		blendfunc blend
                rgbGen lightingDiffuse
       }
}
models/powerups/ammo/rockammo1
{
        {
	        map models/powerups/ammo/red.tga
		tcGen environment
		blendfunc GL_ONE GL_ZERO
                rgbGen lightingDiffuse
        }
}
models/powerups/ammo/rockammo2
{
	{
		map models/powerups/ammo/rockammo2.tga
		rgbGen identity
	}
	
}
models/powerups/ammo/shotammo_new
{
       {
	        map textures/effects/tinfx2d.tga
		tcGen environment
		blendfunc GL_ONE GL_ZERO
                rgbGen identity
       }
       {
	        map models/powerups/ammo/shotammo_new.tga
		blendfunc blend
                rgbGen lightingDiffuse
       }
}
models/powerups/ammo/shotammo1
{
        {
	        map models/powerups/ammo/orange.tga
		tcGen environment
		blendfunc GL_ONE GL_ZERO
                rgbGen lightingDiffuse
        }
}
models/powerups/ammo/shotammo2
{
	{
		map models/powerups/ammo/shotammo2.tga
		rgbGen identity
	}
	
}

models/powerups/ammo/flameammo_new
{
	{
		map textures/effects/tinfx2d.tga
		rgbGen identity
		tcMod rotate 12
		tcMod turb 0 0.5 0 0.5
		tcMod scroll 1 0.5
	}
	{
		map textures/sfx/firegorre.tga
		blendfunc add
		rgbGen identity
		tcMod scroll 0.5 0
		tcGen environment 
	}
	{
		map models/powerups/ammo/flameammo_new.tga
		blendfunc blend
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/ammo/flameammo-glow.tga
		blendfunc gl_one_minus_dst_color gl_src_alpha
		rgbGen wave noise 0 1 0 5 
	}
}
models/powerups/ammo/flameammo1
{
        {
	        map models/powerups/ammo/red.tga
		tcGen environment
		blendfunc GL_ONE GL_ZERO
                rgbGen lightingDiffuse
        }
}
models/powerups/ammo/flameammo2
{
	{
		map models/powerups/ammo/flameammo2.tga
		rgbGen identity
	}
	
}
