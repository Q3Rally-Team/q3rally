"""Paint the Bountyhunter skins: the original sheriff texture and recoloured liveries.

usage: gen_textures.py <source dir with sferiff.jpg> <outdir>

Only the unsaturated paint of sferiff.jpg is recoloured, by brightness: the
black body paint takes the scheme's dark colour, the white doors and roof its
light colour. Rust, the gold star and lettering, the DANGER plate, lamps,
tyre, rim and the chrome pipe keep their original colours.
"""
import colorsys
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
import numpy as np
from PIL import Image
import paint

SRC, OUT = sys.argv[1], sys.argv[2]
P_DIR = 'models/players/bountyhunter/'
# name: (dark paint colour, light panel colour); None = the original
SCHEMES = {
    'sheriff': None,
    'highway': ((18, 30, 70), (236, 238, 242)),
    'redline': ((92, 10, 12), (232, 222, 196)),
    'desert': ((96, 72, 44), (214, 196, 156)),
    'ghost': ((22, 22, 24), (96, 98, 104)),
}
# texture areas that keep their colours (u0, v0, u1, v1 with v from the top)
KEEP = [(0.75, 0.75, 1.00, 1.00),     # tyre and rim
        (0.84, 0.66, 1.00, 0.75),     # tread strip
        (0.72, 0.56, 0.82, 0.78),     # chrome pipe
        (0.30, 0.74, 0.39, 0.82), (0.61, 0.74, 0.70, 0.82),   # headlights
        (0.37, 0.75, 0.63, 0.81),     # chrome grille
        (0.31, 0.83, 0.55, 0.90),     # lower front lamps
        (0.44, 0.04, 0.50, 0.17), (0.44, 0.29, 0.50, 0.43),   # tail lights
        (0.40, 0.16, 0.46, 0.28)]     # DANGER plate

src = np.asarray(Image.open(os.path.join(SRC, 'sferiff.jpg')).convert('RGB')).astype(np.float32) / 255
H, W = src.shape[:2]
hsv = np.array([colorsys.rgb_to_hsv(*px) for px in src.reshape(-1, 3)]).reshape(H, W, 3)
sat, val = hsv[..., 1], hsv[..., 2]
keep = np.zeros((H, W), bool)
for u0, v0, u1, v1 in KEEP:
    keep[int(v0 * H):int(v1 * H), int(u0 * W):int(u1 * W)] = True
# weight of the recolouring: full for grey pixels, fading out towards saturated ones
w = np.clip((0.32 - sat) / 0.12, 0, 1) * ~keep
t = np.clip((val - 0.18) / 0.55, 0, 1)[..., None]           # 0 = dark paint, 1 = white panel

os.makedirs(OUT, exist_ok=True)
for name, scheme in SCHEMES.items():
    if scheme is None:
        img = src
    else:
        dark, light = (np.array(c, np.float32) / 255 for c in scheme)
        # keep the original shading: scale the scheme colour by the pixel's brightness
        # relative to the paint it belongs to
        base = dark * (1 - t) + light * t
        shade = np.clip(val / np.where(t[..., 0] > 0.5, 0.85, 0.12), 0.25, 1.4)[..., None]
        recol = np.clip(base * shade, 0, 1)
        img = src * (1 - w[..., None]) + recol * w[..., None]
    out = Image.fromarray((img * 255 + 0.5).astype(np.uint8))
    out.save(os.path.join(OUT, name + '_main.tga'), compression='tga_rle')
    lines = ['body,%s%s_main.tga' % (P_DIR, name), 'parts,%sparts.tga' % P_DIR]
    lines += [t_ + ',' for t_ in paint.TAGS]
    open(os.path.join(OUT, name + '.skin'), 'w').write('\n'.join(lines) + '\n')
print('ok')
