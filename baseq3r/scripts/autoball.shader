// Autoball ball (tools/autoball/make_ball.py)
// Stage 2 adds the seam/pentagon mask in the colour cgame sets on the
// entity: the team of the last car that touched the ball.
models/autoball/ball
{
	{
		map models/autoball/ball.tga
		rgbGen lightingDiffuse
	}
	{
		map models/autoball/ball_glow.tga
		blendFunc add
		rgbGen entity
	}
}

// Ball trail puffs (cg_autoball.c). Additive; cgame sets colour and fade.
autoballTrail
{
	nopicmip
	cull none
	entityMergable
	{
		map models/autoball/trail.tga
		blendFunc GL_SRC_ALPHA GL_ONE
		rgbGen vertex
		alphaGen vertex
	}
}

// Round ball shadow (the stock markShadow is the cars' rectangular one).
// Darkens by the white centre of the soft trail sprite; cgame sets the
// strength through the vertex colour.
autoballShadow
{
	polygonOffset
	{
		map models/autoball/trail.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_COLOR
		rgbGen exactVertex
	}
}
