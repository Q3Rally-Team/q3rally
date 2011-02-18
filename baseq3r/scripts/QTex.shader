// Quake texture subsitutions for easy converting
textures/water
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	fogparms ( 0.545098 0.545098 0.796079 ) 1337
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc blend
		rgbGen const ( 0.501961 0.501961 0.592157 )
		tcMod scroll -0.009 -0.005
		tcMod rotate 1
		alphaGen wave sin 0.5 0 0 0 
	}
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc add
		rgbGen const ( 0.317647 0.309804 0.458824 )
		tcMod scroll 0.02 -0.009
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
}

textures/04mwat2
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	fogparms ( 0.611765 0.709804 0.721569 ) 1337
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc blend
		rgbGen const ( 0.505882 0.588235 0.588235 )
		tcMod scroll -0.009 -0.005
		tcMod rotate 1
		alphaGen wave sin 0.5 0 0 0 
	}
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc add
		rgbGen const ( 0.329412 0.439216 0.423529 )
		tcMod scroll 0.02 -0.009
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
}

textures/water2
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	fogparms ( 0.611765 0.709804 0.721569 ) 1337
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc blend
		rgbGen const ( 0.505882 0.588235 0.588235 )
		tcMod scroll -0.009 -0.005
		tcMod rotate 1
		alphaGen wave sin 0.5 0 0 0 
	}
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc add
		rgbGen const ( 0.329412 0.439216 0.423529 )
		tcMod scroll 0.02 -0.009
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
}

textures/water0
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	fogparms ( 0.635294 0.701961 0.701961 ) 1337
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc blend
		rgbGen const ( 0.501961 0.592157 0.560784 )
		tcMod scroll -0.009 -0.005
		tcMod rotate 1
		alphaGen wave sin 0.5 0 0 0 
	}
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc add
		rgbGen const ( 0.34902 0.415686 0.396078 )
		tcMod scroll 0.02 -0.009
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
}

textures/04mwat1
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	fogparms ( 0.560784 0.435294 0.137255 ) 1337
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc add
		rgbGen const ( 0.333333 0.258824 0.0823529 )
		tcMod scroll 0.02 -0.009
	}
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc blend
		rgbGen const ( 0.560784 0.435294 0.137255 )
		tcMod scroll -0.009 -0.005
		tcMod rotate 1
		alphaGen wave sin 0.5 0 0 0 
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
}

textures/water1
{
	surfaceparm nomarks
	surfaceparm trans
	surfaceparm water
	cull disable
	fogparms ( 0.560784 0.435294 0.137255 ) 1337
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc add
		rgbGen const ( 0.333333 0.258824 0.0823529 )
		tcMod scroll 0.02 -0.009
	}
	{
		map textures/liquids/pool3d_3e.tga
		blendfunc blend
		rgbGen const ( 0.560784 0.435294 0.137255 )
		tcMod scroll -0.009 -0.005
		tcMod rotate 1
		alphaGen wave sin 0.5 0 0 0 
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
}

textures/ecop1_1
{
	surfaceparm metalsteps
	{
		map textures/gothic_floor/blocks17floor.tga
		rgbGen const ( 0.615686 0.533333 0.490196 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/ecop1_4
{
	surfaceparm metalsteps
	{
		map textures/gothic_floor/blocks17floor.tga
		rgbGen const ( 0.615686 0.533333 0.490196 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/ecop1_6
{
	surfaceparm metalsteps
	{
		map textures/gothic_floor/blocks17floor.tga
		rgbGen const ( 0.615686 0.533333 0.490196 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/ecop1_7
{
	surfaceparm metalsteps
	{
		map textures/base_trim/metal2_2.tga
		rgbGen const ( 0.67451 0.494118 0.435294 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/ecop1_8
{
	surfaceparm metalsteps
	{
		map textures/base_wall/patch10_beatup4.tga
		rgbGen const ( 1 1 1 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/ground1_2
{
	{
		map textures/stone/pjrock6.tga
		rgbGen const ( 0.678431 0.52549 0.427451 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_rock.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/ground1_6
{
	{
		map textures/stone/pjrock6.tga
		rgbGen const ( 0.611765 0.788235 0.560784 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_rock.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/rock1_2
{
	{
		map textures/stone/pjrock21.tga
		rgbGen const ( 0.447059 0.345098 0.196078 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_rock.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 16 16
		detail
	}
}

textures/sfloor3_2
{
	{
		map textures/base_trim/tin.tga
		rgbGen const ( 0.862745 0.803922 0.729412 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_rock.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 16 16
		detail
	}
}

textures/sfloor4_1
{
	surfaceparm metalsteps
	{
		map textures/base_floor/diamond2.tga
		rgbGen const ( 0.588235 0.627451 0.654902 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/sfloor4_2
{
	surfaceparm metalsteps
	{
		map textures/base_floor/diamond2.tga
		rgbGen const ( 0.643137 0.568627 0.466667 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/sfloor4_5
{
	surfaceparm metalsteps
	{
		map textures/base_floor/diamond2.tga
		rgbGen const ( 0.619608 0.541176 0.372549 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/sfloor4_6
{
	surfaceparm metalsteps
	{
		map textures/base_floor/diamond2.tga
		rgbGen const ( 0.619608 0.541176 0.372549 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/sfloor4_7
{
	surfaceparm metalsteps
	{
		map textures/base_floor/diamond2.tga
		rgbGen const ( 0.611765 0.403922 0.380392 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/sfloor4_8
{
	surfaceparm metalsteps
	{
		map textures/base_floor/diamond2.tga
		rgbGen const ( 0.619608 0.541176 0.372549 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/sfloor4_4
{
	q3map_lightimage textures/oquartz/slight2.tga
	surfaceparm noimpact
	q3map_surfacelight 300
	q3map_flare flareShader
	{
		map textures/oquartz/slight2.tga
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
}

textures/tech01_1
{
	surfaceparm metalsteps
	{
		map textures/base_support/cable.tga
		rgbGen const ( 0.619608 0.541176 0.372549 )
		tcMod scale 2 4
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech01_2
{
	surfaceparm metalsteps
	{
		map textures/base_support/cable.tga
		rgbGen const ( 0.803922 0.760784 0.670588 )
		tcMod scale 2 3
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech01_3
{
	surfaceparm metalsteps
	{
		map textures/base_support/cable.tga
		rgbGen const ( 0.803922 0.760784 0.670588 )
		tcMod scale 4 3
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech01_5
{
	surfaceparm metalsteps
	{
		map textures/base_support/cable.tga
		rgbGen const ( 0.647059 0.505882 0.294118 )
		tcMod scale 6 6
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech01_6
{
	surfaceparm metalsteps
	{
		map textures/base_support/cable.tga
		rgbGen const ( 0.74902 0.627451 0.45098 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech01_7
{
	surfaceparm metalsteps
	{
		map textures/base_support/cable.tga
		rgbGen const ( 0.752941 0.537255 0.447059 )
		tcMod scale 6 6
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech01_9
{
	surfaceparm metalsteps
	{
		map textures/base_floor/clanggrate2.tga
		rgbGen const ( 0.862745 0.380392 0.239216 )
		tcMod scale 6 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech02_1
{
	surfaceparm metalsteps
	{
		map textures/base_support/cable.tga
		rgbGen const ( 0.858824 0.823529 0.756863 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech02_2
{
	surfaceparm metalsteps
	{
		map textures/base_ceiling/metceil1d.tga
		rgbGen const ( 0.862745 0.380392 0.239216 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_conc.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech02_3
{
	surfaceparm metalsteps
	{
		map textures/base_ceiling/metceil1d.tga
		rgbGen const ( 0.862745 0.380392 0.239216 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_conc.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech02_5
{
	surfaceparm metalsteps
	{
		map textures/base_ceiling/metceil1d.tga
		rgbGen const ( 0.862745 0.380392 0.239216 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_conc.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech02_6
{
	surfaceparm metalsteps
	{
		map textures/base_ceiling/metceil1d.tga
		rgbGen const ( 0.862745 0.380392 0.239216 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_conc.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech02_7
{
	surfaceparm metalsteps
	{
		map textures/base_ceiling/metceil1d.tga
		rgbGen const ( 0.862745 0.380392 0.239216 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_conc.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech03_1
{
	surfaceparm metalsteps
	{
		map textures/base_wall/qbasewall.tga
		rgbGen const ( 1 1 1 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech03_2
{
	surfaceparm metalsteps
	{
		map textures/base_wall/qbasewall.tga
		rgbGen const ( 1 1 1 )
		tcMod scale 0.5 0.5
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech04_1
{
	surfaceparm metalsteps
	{
		map textures/base_trim/tin.tga
		rgbGen const ( 0.670588 0.517647 0.313726 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/tech04_2
{
	surfaceparm metalsteps
	{
		map textures/base_trim/tin.tga
		rgbGen const ( 0.670588 0.517647 0.313726 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/tech04_3
{
	surfaceparm metalsteps
	{
		map textures/base_trim/tin.tga
		rgbGen const ( 0.670588 0.517647 0.313726 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/tech04_4
{
	surfaceparm metalsteps
	{
		map textures/base_trim/tin.tga
		rgbGen const ( 0.670588 0.517647 0.313726 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/tech04_5
{
	surfaceparm metalsteps
	{
		map textures/base_trim/tin.tga
		rgbGen const ( 0.670588 0.517647 0.313726 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/tech04_6
{
	surfaceparm metalsteps
	{
		map textures/base_trim/tin.tga
		rgbGen const ( 0.670588 0.517647 0.313726 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/tech04_7
{
	surfaceparm metalsteps
	{
		map textures/base_trim/tin.tga
		rgbGen const ( 0.670588 0.517647 0.313726 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/tech04_8
{
	surfaceparm metalsteps
	{
		map textures/base_trim/tin.tga
		rgbGen const ( 0.670588 0.517647 0.313726 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 8 8
		detail
	}
}

textures/tech05_1
{
	surfaceparm metalsteps
	{
		map textures/base_wall/qbasewall.tga
		rgbGen const ( 0.803922 0.478431 0.321569 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech06_1
{
	surfaceparm metalsteps
	{
		map textures/base_wall/qbasewall.tga
		rgbGen const ( 0.803922 0.478431 0.321569 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tech08_1
{
	surfaceparm metalsteps
	{
		map textures/base_wall/qbasewall.tga
		rgbGen const ( 1 1 1 )
		tcMod scale 0.5 0.5
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 32 32
		detail
	}
}

textures/tlight01
{
	q3map_lightimage textures/oquartz/stlight1.tga
	surfaceparm noimpact
	q3map_surfacelight 126
	q3map_flare flareShader
	{
		map textures/oquartz/stlight1.tga
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 16 16
		detail
	}
}

textures/tlight02
{
	q3map_lightimage textures/oquartz/stlight1.tga
	surfaceparm noimpact
	q3map_surfacelight 32
	q3map_flare flareShader
	{
		map textures/oquartz/stlight1.tga
		rgbGen const ( 0.454902 0.454902 0.454902 )
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
	{
		map textures/detail/d_met2.tga
		blendfunc gl_dst_color gl_src_color
		tcMod scale 16 16
		detail
	}
}

textures/tlight03
{
	q3map_lightimage textures/oquartz/light.tga
	surfaceparm noimpact
	q3map_surfacelight 126
	q3map_flare flareShader
	{
		map textures/oquartz/light.tga
		tcMod scale 2 2
	}
	{
		map $lightmap 
		blendfunc filter
		tcGen lightmap 
	}
}

