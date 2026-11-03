"""Paint the Skyline textures and convert the lamp/grill textures of the source.

usage: gen_textures.py <source textures dir> <outdir>

The paint atlases follow layout.py. Panel gaps, vents and lamps are modelled,
so the paint only carries colour, shading and the rally liveries.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
sys.path.insert(0, HERE)            # r34/layout.py before ../layout.py
from PIL import Image
import layout
import paint

SRC, OUT = sys.argv[1], sys.argv[2]
ALPHA = 238                         # paint over the reflection; satin, less glossy than the other cars
DECALS = {
    'flank': (-13.0, 16.0, 2.5, 6.0),
    'ends': (-10.0, 14.0),
    # twin stripes over nose, bonnet, roof, boot and tail, offset like the Mustang's
    'stripes': [(-1.4, 7.2, 'stripe'), (-3.8, -2.4, 'edge')],
    'stripe_front': -8.0,
    'stripe_back': -6.0,
    # side stripe from the front wheel arch to the rear wheel arch, rising to the back
    'side_stripes': [([(25.0, -6.2), (-24.0, -5.2), (-24.0, -7.6), (25.0, -8.6)], 'stripe'),
                     ([(25.0, -9.2), (-24.0, -8.2), (-24.0, -8.9), (25.0, -9.9)], 'edge')],
    'side_text': (-31.0, 1.2, 2.0),          # rear wing, above the wheel arch
    'door_roundel': (2.0, -0.5, 5.6),
    'roof_roundel': (-7.0, 0.0, 7.0),
    'hood_text': (36.0, 2.6),
}

paint.Painter(layout, DECALS, ALPHA).paint_all(OUT)
paint.shared_parts(OUT)
# lamps and grille from the source model
for src, name, size in (('lights_texture.png', 'lights', 512), ('backlights.png', 'tlite', 512),
                        ('grill-texture-png-11.png', 'grill', 256)):
    im = Image.open(os.path.join(SRC, src)).convert('RGBA').resize((size, size), Image.LANCZOS)
    paint.save(OUT, im, name)
paint.write_skins(OUT, 'models/players/skyline/',
                  [('bottom', 'under'), ('glass', 'glass'), ('trim', 'trim'), ('chrome', 'chrome'), ('grill', 'grill')],
                  ('body_fl', 'body_fr', 'body_rl', 'body_rr'),
                  [('hlights', 'lights'), ('ilights', 'lights'), ('rlights', 'tlite')])
print('ok')
