gfx/flares/blite
{
	deformVertexes autosprite

	{
		map gfx/flares/blite.tga
		blendFunc add
		rgbGen identity
	}
}

gfx/flares/hlite
{
	deformVertexes autosprite

	{
		map gfx/flares/hlite.tga
		blendFunc add
		rgbGen identity
	}
}

gfx/flares/rlite
{
	deformVertexes autosprite

	{
		map gfx/flares/rlite.tga
		blendFunc add
		rgbGen identity
	}
}
gfx/skidmarks/asphalt
{
	polygonOffset
	{
		map gfx/skidmarks/asphalt.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
}
gfx/skidmarks/grass
{
	polygonOffset
	{
		map gfx/skidmarks/grass.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
}
gfx/skidmarks/dirt
{
	polygonOffset
	{
		map gfx/skidmarks/dirt.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
}
gfx/skidmarks/flesh
{
	polygonOffset
	{
		map gfx/skidmarks/flesh.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
}
gfx/damage/spark
{
	sort nearest
	cull none
	{
		map gfx/damage/spark.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen vertex
	}
}

gfx/damage/oil_mark
{
	polygonOffset


	{
		map gfx/damage/oil_mark.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
	{	
	      map textures/reflect/oilreflect.tga
		blendFunc add
	      rgbGen exactVertex
	}
	{
		map gfx/damage/oil_mark_reflect.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
}
