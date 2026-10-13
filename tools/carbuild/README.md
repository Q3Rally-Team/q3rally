# carbuild – car asset pipeline

Python 3 + Pillow + NumPy scripts that turned raw MilkShape exports into the
finished cars in `baseq3r/models/players/`:

* **mustang** (`mustang.pk3`: untextured `body.md3` with 100 identical frames,
  no UVs, incomplete tags, the Mini's wheel)
* **vw_typ_82** (`vw_typ_82.pk3`: Kuebelwagen with a flat colour template
  texture, mirrored UVs, uncentred body, asymmetric wheel tags, a 30-frame
  wheel and only a red skin)

| Script | Purpose |
| --- | --- |
| `md3.py` | Minimal MD3 reader/writer (frames, tags, surfaces, normals). |
| `layout.py` | Paint atlas layout (left/right side, top, front, back) in world units. |
| `build_model.py` | Collapses to one frame, centres the body on the origin, generates UVs (atlas projection for the paint, box projection for lights/glass), renames `tlights` -> `ilights`, writes all 14 tags, scales the wheel to fill the arches. |
| `gen_textures.py` | Paints the `*_main.tga` atlases (red/blue/green/yellow + rally01/07/13/27/44/88 in the house rally colours) and the shared glass, chrome, light and underside textures. |
| `paintlib.py` | Rally colour schemes, number roundels, lettering and atlas drawing helpers shared by the texture scripts. |
| `build_vw.py` | Kuebelwagen: sorts the triangles into paint / trim / headlight lens / overall / hands by sampling the template texture, atlas-projects the paint, centres the body, rewrites all 15 tags, reduces the wheel to one frame. |
| `gen_vw_textures.py` | Kuebelwagen paint atlases (plain, sand, olive, rally), livery coloured driver overalls, trim, headlight and see-through windscreen textures. |
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

Kuebelwagen: `build_vw.py <dir of the pk3's vw_typ_82 folder> $D`, then
`gen_vw_textures.py $D`; its icons use the camera `160,130,75` so the livery is
not hidden behind the spare wheel.

The icons are written as PNG; convert them to 32-bit TGA (e.g. with Pillow)
before copying them next to the skins.
