// ------------------------------------------------------------
// Shaders for Q3Rally Ammo Boxes - rewritten by P3rlE
// based on the ammo.shader from jzero --> johnzero@verizon.net
// ------------------------------------------------------------


// ------------------------------------------------------------
// 1.Machinegun ammo
// ------------------------------------------------------------

models/powerups/ammo/machammo
{
	{
		map models/powerups/ammo/ammobox.tga
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/ammo/ammolights.tga
		blendfunc blend
		rgbGen const ( 1 1 0 )
		alphaGen wave sawtooth 0 1 0 1 
	}
}

models/powerups/ammo/machammo2
{
	{
		map models/powerups/ammo/machammo2.tga
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 2.Shotgun ammo
// ------------------------------------------------------------

models/powerups/ammo/shotammo
{
	{
		map models/powerups/ammo/ammobox.tga
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/ammo/ammolights.tga
		blendfunc blend
		rgbGen const ( 1 0.5 0 )
		alphaGen wave sawtooth 0 1 0 1 
	}
}

models/powerups/ammo/shotammo2
{
	{
		map models/powerups/ammo/shotammo2.tga
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 3.Grenadelauncher ammo
// ------------------------------------------------------------

models/powerups/ammo/grenammo
{
	{
		map models/powerups/ammo/ammobox.tga
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/ammo/ammolights.tga
		blendfunc blend
		rgbGen const ( 0 0.56 0 )
		alphaGen wave sawtooth 0 1 0 1 
	}
}

models/powerups/ammo/grenammo2
{
	{
		map models/powerups/ammo/grenammo2.tga
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 4.Lightninggun ammo
// ------------------------------------------------------------

models/powerups/ammo/lighammo
{
	{
		map models/powerups/ammo/ammobox.tga
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/ammo/ammolights.tga
		blendfunc blend
		rgbGen const ( 0.50 1 1 )
		alphaGen wave sawtooth 0 1 0 1 
	}
}

models/powerups/ammo/lighammo2
{
	{
		map models/powerups/ammo/lighammo2.tga
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 5.Plasmagun ammo
// ------------------------------------------------------------

models/powerups/ammo/plasammo
{
	{
		map models/powerups/ammo/ammobox.tga
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/ammo/ammolights.tga
		blendfunc blend
		rgbGen const ( 0 0 1 )
		alphaGen wave sawtooth 0 1 0 1 
	}
}

models/powerups/ammo/plasammo2
{
	{
		map models/powerups/ammo/plasammo2.tga
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 6.Rocketlauncher ammo
// ------------------------------------------------------------

models/powerups/ammo/rockammo
{
	{
		map models/powerups/ammo/ammobox.tga
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/ammo/ammolights.tga
		blendfunc blend
		rgbGen const ( 1 0 0 )
		alphaGen wave sawtooth 0 1 0 1 
	}
}

models/powerups/ammo/rockammo2
{
	{
		map models/powerups/ammo/rockammo2.tga
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 7.Railgun ammo
// ------------------------------------------------------------

models/powerups/ammo/railammo
{
	{
		map models/powerups/ammo/ammobox.tga
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/ammo/ammolights.tga
		blendfunc blend
		rgbGen const ( 0 1 0 )
		alphaGen wave sawtooth 0 1 0 1 
	}
}

models/powerups/ammo/railammo2
{
	{
		map models/powerups/ammo/railammo2.tga
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 8.BigFuckingGun ammo
// ------------------------------------------------------------

models/powerups/ammo/bfgammo
{
	{
		map models/powerups/ammo/ammobox.tga
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/ammo/ammolights.tga
		blendfunc blend
		rgbGen const ( 0 1 0 )
		alphaGen wave sawtooth 0 1 0 1 
	}
}

models/powerups/ammo/bfgammo2
{
	{
		map models/powerups/ammo/bfgammo2.tga
		rgbGen identity
	}
}

// ------------------------------------------------------------
// 9.Flamethrower ammo
// ------------------------------------------------------------

models/powerups/ammo/flameammo
{
	{
		map models/powerups/ammo/ammobox.tga
		rgbGen lightingDiffuse
	}
	{
		map models/powerups/ammo/ammolights.tga
		blendfunc blend
		rgbGen const ( 1 0.5 0 )
		alphaGen wave sawtooth 0 1 0 1 
	}
}

models/powerups/ammo/flameammo2
{
	{
		map models/powerups/ammo/flameammo2.tga
		rgbGen identity
	}
}

