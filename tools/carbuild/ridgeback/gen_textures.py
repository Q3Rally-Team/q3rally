"""Paint the Ridgeback textures and write its skins.

usage: gen_textures.py <outdir>

Run after build_model.py: it writes body.md3 and the part textures (chassis,
interior, lights, from the source model) into the same directory. The paint
atlases follow layout.py.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
sys.path.insert(0, HERE)            # ridgeback/layout.py before ../layout.py
import layout
import md3
import paint

OUT = sys.argv[1]
ALPHA = 238                         # satin paint like the Skyline and the GT-R
DECALS = {
    'flank': (-12.0, 18.0, 4.0, 8.0),
    'ends': (-10.0, 16.0),
    'stripes': [(-1.4, 7.6, 'stripe'), (-4.0, -2.6, 'edge')],
    'stripe_front': -6.0,
    'stripe_back': -5.0,
    # side stripe along the doors and the bed, between the wheel arches
    'side_stripes': [([(26.0, -2.4), (-20.0, -2.4), (-20.0, -4.6), (26.0, -4.6)], 'stripe'),
                     ([(26.0, -5.2), (-20.0, -5.2), (-20.0, -5.9), (26.0, -5.9)], 'edge')],
    'side_text': (-32.0, 4.0, 2.2),          # bed side, above the rear wheel
    'door_roundel': (10.0, 3.0, 5.0),
    'roof_roundel': (3.0, 0.0, 6.5),
    'hood_text': (38.0, 2.6),
}

paint.Painter(layout, DECALS, ALPHA).paint_all(OUT)
paint.shared_parts(OUT, ('glass',))
# skins list the surfaces of body.md3 (build_model.py splits big groups into group2, group3, ...)
TEX = {'glass': 'glass', 'chassis': 'chassis', 'interior': 'interior', 'lights': 'lights'}
names = [s.name for s in md3.load(os.path.join(OUT, 'body.md3')).surfs]
paint.write_skins(OUT, 'models/players/ridgeback/', [],
                  [n for n in names if n.startswith('body_')],
                  [(n, TEX[n.rstrip('0123456789')]) for n in names if not n.startswith('body_')])
print('ok')
