"""Paint the Cutlass textures and write its skins.

usage: gen_textures.py <outdir>

Run after build_model.py: it writes body.md3 and the part textures (trim,
lights from the source model, black) into the same directory. The paint
atlases follow layout.py.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
sys.path.insert(0, HERE)            # cutlass/layout.py before ../layout.py
import layout
import md3
import paint

OUT = sys.argv[1]
ALPHA = 238                         # satin paint like the other new cars
DECALS = {
    'flank': (-13.0, 16.0, 1.5, 4.5),
    'ends': (-11.0, 14.0),
    'stripes': [(-1.4, 7.6, 'stripe'), (-4.0, -2.6, 'edge')],
    'stripe_front': -8.0,
    'stripe_back': -6.0,
    # side stripe along both doors, between the wheel arches
    'side_stripes': [([(24.0, -4.6), (-21.0, -4.6), (-21.0, -6.8), (24.0, -6.8)], 'stripe'),
                     ([(24.0, -7.4), (-21.0, -7.4), (-21.0, -8.1), (24.0, -8.1)], 'edge')],
    'side_text': (-31.0, 0.8, 2.0),          # rear wing, above the wheel arch
    'door_roundel': (11.0, -0.8, 4.8),       # front door
    'roof_roundel': (-6.0, 0.0, 6.5),
    'hood_text': (40.0, 2.6),
}

paint.Painter(layout, DECALS, ALPHA).paint_all(OUT)
paint.shared_parts(OUT, ('glass', 'chrome'))
# skins list the surfaces of body.md3 (build_model.py splits big groups into group2, group3, ...)
TEX = {'glass': 'glass', 'chrome': 'chrome', 'trim': 'trim', 'black': 'black', 'lights': 'lights'}
names = [s.name for s in md3.load(os.path.join(OUT, 'body.md3')).surfs]
paint.write_skins(OUT, 'models/players/cutlass/', [],
                  [n for n in names if n.startswith('body_')],
                  [(n, TEX[n.rstrip('0123456789')]) for n in names if not n.startswith('body_')])
print('ok')
