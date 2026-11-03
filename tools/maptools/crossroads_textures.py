#!/usr/bin/env python3
"""Generated images and shader script of Crossroads (run once, Python 3 + Pillow).

- textures/q3r_crossroads/macro.jpg: tileable large-scale brightness noise.
  The terrain shaders multiply it in (2x modulate, 50 % grey is neutral) at a
  tile size of about 4300 units, which breaks up the repetition of the ground
  textures.
- scripts/q3r_crossroads.shader: the six ground layers of the index map and
  all their blends are written from one table, so names and stages always
  match make_crossroads.py.
"""
import random
from itertools import combinations
from pathlib import Path

from PIL import Image, ImageFilter

ROOT = Path(__file__).resolve().parents[2]
BASE = ROOT / "baseq3r"
TEX = "textures/q3r_crossroads/"

# index map layers: (name, image, repeats per 256 units)
LAYERS = (("grass", "textures/q3r_dirt_grass/grass07.tga", 1.5),
          ("dirt", "textures/q3r_dirt_grass/dirt02.tga", 2.0),
          ("gravel", "textures/q3r_dirt_grass/stones01.tga", 2.0),
          ("pebbles", "textures/q3r_ground/pebble01.jpg", 1.25),
          ("drygrass", "textures/q3r_dirt_grass/grass09.tga", 1.5),
          ("concrete", "textures/base_floor/concrete.jpg", 0.75))
# parent shaders on the brush sides: collision and tyre friction
PARENTS = (("grass", "surfaceparm SURF_GRASS", 0), ("dirt", "surfaceparm SURF_DIRT", 1),
           ("gravel", "surfaceparm SURF_GRAVEL", 2), ("plain", None, 5))
MACRO = 0.06
# Texture coordinates come from world x/y per stage (tcGen vector). Compile-time
# coordinates would be re-based per surface by q3map2, and any scale that is not
# an integer would then show seams between the merged surfaces.


def vector(scale):
    k = f"{scale / 256:.8f}".rstrip("0")
    return f"\t\ttcGen vector ( {k} 0 0 ) ( 0 {k} 0 )"


def macro_noise(size=512, seed=7):
    rnd = random.Random(seed)
    acc = [0.0] * (size * size)
    for cells, weight in ((4, 1.0), (8, 0.6), (16, 0.35), (32, 0.18)):
        small = Image.new("F", (cells, cells))
        small.putdata([rnd.random() for _ in range(cells * cells)])
        tiled = Image.new("F", (cells * 3, cells * 3))
        for i in range(3):
            for j in range(3):
                tiled.paste(small, (i * cells, j * cells))
        big = tiled.resize((size * 3, size * 3), Image.BICUBIC).crop((size, size, 2 * size, 2 * size))
        acc = [a + weight * v for a, v in zip(acc, big.get_flattened_data())]
    data = acc
    lo, hi = min(data), max(data)
    # 0.5 is neutral; +-22 % brightness
    out = Image.new("L", (size, size))
    out.putdata([int(128 + ((v - lo) / (hi - lo) - 0.5) * 2 * 56) for v in data])
    out = out.filter(ImageFilter.GaussianBlur(1.2))
    return out.convert("RGB")


def stage(image, scale, blend=None):
    lines = ["\t{", f"\t\tmap {image}"]
    if blend:
        lines += ["\t\tblendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA", "\t\talphaGen vertex"]
    lines += [vector(scale), "\t\trgbGen identity", "\t}"]
    return lines


def tail():
    return ["\t{", f"\t\tmap {TEX}macro.jpg", "\t\tblendFunc GL_DST_COLOR GL_SRC_COLOR",
            vector(MACRO), "\t\trgbGen identity", "\t}",
            "\t{", "\t\tmap $lightmap", "\t\tblendFunc GL_DST_COLOR GL_ZERO", "\t\trgbGen identity", "\t}"]


def ground_shaders():
    out = ["// ---------------------------------------------------------------- ground",
           "// Written by tools/maptools/crossroads_textures.py. The terrain brushes use",
           "// the t_* parents (collision, tyre friction). q3map2 swaps the drawn",
           "// surfaces for ter_<layer> or ter_<a>to<b> from maps/q3r_crossroads_index.pcx",
           "// and sets the vertex alpha of the blend. Layers: " +
           ", ".join(f"{i} {n}" for i, (n, _, _) in enumerate(LAYERS)), ""]
    for name, parm, layer in PARENTS:
        out += [f"{TEX}t_{name}", "{", f"\tqer_editorimage {LAYERS[layer][1]}",
                "\tq3map_indexed", "\tq3map_shadeAngle 120", "\tq3map_nonplanar", "\tq3map_lightmapAxis z"]
        if parm:
            out.append(f"\t{parm}")
        out += ["}", ""]
    for i, (name, image, scale) in enumerate(LAYERS):
        out += [f"{TEX}ter_{i}", "{", f"\t// {name}", f"\tqer_editorimage {image}",
                "\tq3map_shadeAngle 120", "\tq3map_nonplanar", "\tq3map_lightmapAxis z"]
        out += stage(image, scale) + tail() + ["}", ""]
    for i, j in combinations(range(len(LAYERS)), 2):
        a, b = LAYERS[i], LAYERS[j]
        out += [f"{TEX}ter_{i}to{j}", "{", f"\t// {a[0]} to {b[0]}", f"\tqer_editorimage {a[1]}", "\tq3map_shadeAngle 120", "\tq3map_nonplanar", "\tq3map_lightmapAxis z"]
        out += stage(a[1], a[2]) + stage(b[1], b[2], blend=True) + tail() + ["}", ""]
    return "\n".join(out)


def main():
    (BASE / TEX).mkdir(parents=True, exist_ok=True)
    macro_noise().save(BASE / TEX / "macro.jpg", quality=92)
    shader = BASE / "scripts" / "q3r_crossroads.shader"
    text = shader.read_text(encoding="utf-8")
    marker = "// ---------------------------------------------------------------- ground"
    head = text.split(marker)[0].rstrip() + "\n\n"
    shader.write_text(head + ground_shaders(), encoding="utf-8")
    print("wrote", shader, "and", BASE / TEX / "macro.jpg")


if __name__ == "__main__":
    main()
