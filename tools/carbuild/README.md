# carbuild – Mustang asset pipeline

Python 3 + Pillow + NumPy scripts that turned the raw MilkShape export of the
Mustang (`mustang.pk3`: untextured `body.md3` with 100 identical frames, no UVs,
incomplete tags, the Mini's wheel) into the finished car in
`baseq3r/models/players/mustang/`.

| Script | Purpose |
| --- | --- |
| `md3.py` | Minimal MD3 reader/writer (frames, tags, surfaces, normals). |
| `layout.py` | Paint atlas layout (left/right side, top, front, back) in world units. |
| `build_model.py` | Collapses to one frame, centres the body on the origin, generates UVs (atlas projection for the paint, box projection for lights/glass), renames `tlights` -> `ilights`, writes all 14 tags, scales the wheel to fill the arches. |
| `gen_textures.py` | Paints the `*_main.tga` atlases (red/blue/green/yellow + rally01/07/13/27/44/88 in the house rally colours) and the shared glass, chrome, light and underside textures. |
| `render3d.py` | Small software renderer for previews and the menu icons (`icon_<skin>.tga`). |

## Rebuild

```sh
cd tools/carbuild
D=../../baseq3r/models/players/mustang
python3 build_model.py <src>/body.md3 <src>/wheel.md3 $D
python3 gen_textures.py $D/body.md3 $D
# icons (rally icons reuse the number badge of the matching Viper icon)
Q3R_FIT=1 python3 render3d.py $D red svt_cobra /tmp/icon_red.png 384 200,60,45
Q3R_FIT=1 python3 render3d.py $D rally13 svt_cobra /tmp/icon_rally13.png 384 200,60,45 \
    ../../baseq3r/models/players/viper/icon_rally13.tga
```

The icons are written as PNG; convert them to 32-bit TGA (e.g. with Pillow)
before copying them next to the skins.
