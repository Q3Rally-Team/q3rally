//for q3rally,
//this will be the place for ctf decorations eg) banners, curtains, etc.
//used the q3r logo as base
//to be completed by devteam --thebigbuu

//////////
//decals//
//////////
textures/q3r_ctf/q3r_bluelong_decal
{
	qer_editorimage textures/q3r_ctf/q3r_bannerbluelong.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	polygonoffset
	{
		map textures/q3r_ctf/q3r_bannerbluelong.tga
	}
	
}

textures/q3r_ctf/q3r_redlong_decal
{
	qer_editorimage textures/q3r_ctf/q3r_bannerredlong.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	polygonoffset
	{
		map textures/q3r_ctf/q3r_bannerredlong.tga
	}
	
}

////////////////
//wavy banners//
////////////////
textures/q3r_ctf/q3r_bluelong_01
{
	qer_editorimage textures/q3r_ctf/q3r_bannerbluelong.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	polygonoffset
	{
		map textures/q3r_ctf/q3r_bannerbluelong.tga
		wave sin 0 1 0 .1
	}
	
}

textures/q3r_ctf/q3r_redlong_01
{}


///////////////////////////
//these have transparency//
///////////////////////////

textures/q3r_ctf/q3r_curtainredwide_01
{
	qer_editorimage textures/q3r_ctf/q3r_curtainredwide_shader.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	{
		map textures/q3r_ctf/q3r_curtainredwide_shader.tga
		alphagen lt_128
	}
}

textures/q3r_ctf/q3r_curtainbluewide_01
{
	qer_editorimage textures/q3r_ctf/q3r_curtainbluewide_shader.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	{
		map textures/q3r_ctf/q3r_curtainbluewide_shader.tga
	}
}

textures/q3r_ctf/q3r_curtainneuwide_01
{
	qer_editorimage textures/q3r_ctf/q3r_curtainneuwide.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	{
		map textures/q3r_ctf/q3r_curtainneuwide.tga
	}
}

/////////////////
//experimenting
/////////////////

textures/q3r_ctf/q3r_doorwideneu_01
{
	qer_editorimage textures/q3r_ctf/q3r_curtainneuwide.tga
	surfaceparm nolightmap
	surfaceparm nomarks
	{
		map textures/q3r_ctf/q3r_curtainneuwide.tga
	}
}