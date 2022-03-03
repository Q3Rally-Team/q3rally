// ------------------------------------------------------------
// Shaders for Q3Rally Atmosphere effects - rewritten by P3rlE
// based on the atmosphere.shader from stecki
// ------------------------------------------------------------

// ------------------------------------------------------------
// 1.Rain
// ------------------------------------------------------------

gfx/atmosphere/raindrop
{
	cull none
	polygonoffset
	sort 16
	{
		map gfx/atmosphere/raindrop.tga
		blendfunc blend
		alphagen vertex
	}
}

gfx/atmosphere/raindropwater
{
	cull none
	{
		map gfx/atmosphere/raindropwater.tga
		blendfunc blend
		alphagen vertex
	}
}

gfx/atmosphere/raindropsolid
{
	polygonoffset
	{
		map gfx/atmosphere/raindropsolid.tga
		blendfunc blend
		alphagen vertex
	}
}

// ------------------------------------------------------------
// 2.Snow
// ------------------------------------------------------------

gfx/atmosphere/snowflake00
{
	nopicmip
	cull none
	polygonoffset
	sort 16
	{
		map gfx/atmosphere/snowflake00.tga
		blendfunc blend
		alphagen vertex
	}
}

gfx/atmosphere/snowflake01
{
	nopicmip
	cull none
	polygonoffset
	sort 16
	{
		map gfx/atmosphere/snowflake01.tga
		blendfunc blend
		alphagen vertex
	}
}

gfx/atmosphere/snowflake02
{
	nopicmip
	cull none
	polygonoffset
	sort 16
	{
		map gfx/atmosphere/snowflake02.tga
		blendfunc blend
		alphagen vertex
	}
}

gfx/atmosphere/snowflake03
{
	nopicmip
	cull none
	polygonoffset
	sort 16
	{
		map gfx/atmosphere/snowflake03.tga
		blendfunc blend
		alphagen vertex
	}
}

gfx/atmosphere/snowflake04
{
	nopicmip
	cull none
	polygonoffset
	sort 16
	{
		map gfx/atmosphere/snowflake04.tga
		blendfunc blend
		alphagen vertex
	}
}

gfx/atmosphere/snowflake05
{
	nopicmip
	cull none
	polygonoffset
	sort 16
	{
		map gfx/atmosphere/snowflake05.tga
		blendfunc blend
		alphagen vertex
	}
}
