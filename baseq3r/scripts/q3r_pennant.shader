// Q3Rally: pennant on a whip antenna for cars that carry a CTF flag.
// Drawn by cgame as polygons (CG_CarPennant); colour and fade come from the
// vertex colours, so one shader serves all teams.
gfx/misc/q3r_pennant
{
	nopicmip
	cull none
	sort additive
	{
		map $whiteimage
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		alphaGen vertex
	}
}
