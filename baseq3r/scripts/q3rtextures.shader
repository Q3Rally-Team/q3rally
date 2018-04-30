//
// Textures from killaz
//

textures/qrally2/sand3
{
	surfaceparm	dust		

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/qrally2/sand3.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/qrally2/sandwet
{
	surfaceparm	dust
	surfaceparm	wet

	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/qrally2/sandwet.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/qrally2/qrally2asphalt
{
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/qrally2/qrally2asphalt.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}

textures/qrally2/qrally2grass0_1
{
	surfaceparm grass
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/qrally2/qrally2grass0_1.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
}
