"""Replace the centre badge of a 64x64 rim texture with a plain hub cap.
usage: debadge_rim.py <in.tga> <out.tga>"""
import sys
import numpy as np
from PIL import Image
a = np.asarray(Image.open(sys.argv[1]).convert('RGBA')).astype(np.float32)
h, w = a.shape[:2]; cy, cx = 31.4, 31.2                 # badge centre
yy, xx = np.mgrid[:h, :w]; d = np.hypot(yy - cy, xx - cx)
cap = d <= 3.6
near = d <= 5.5                                         # drop the badge's yellow fringe
a[near, :3] = a[near, :3].mean(-1, keepdims=True)
g = 175 - 55*(d/3.6) - 20*((yy - cy)/3.6)                 # lit from above
a[cap, 0] = g[cap]; a[cap, 1] = g[cap]; a[cap, 2] = g[cap] + 6
ring = (d > 3.6) & (d <= 4.4)
a[ring, :3] = a[ring, :3]*0.8
Image.fromarray(np.clip(a, 0, 255).astype(np.uint8), 'RGBA').save(sys.argv[2])
