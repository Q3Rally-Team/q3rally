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
| `md3info.py` | Per-surface vertex/triangle counts against the engine limit (exit code 1 if a model would be rejected). |
| `subdivide.py` | Splits every triangle into four per pass; smooth edges follow PN-triangle curves, hard edges stay sharp. For high-poly tests and smoothing low-poly bodies. |

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

## Polygon limit

The renderer rejects a whole model if one surface has 10000 or more vertices
or 20000 or more triangles (`SHADER_MAX_VERTEXES` in
`engine/code/qcommon/qfiles.h`). Engines before that change stopped at 1000
vertices / 2000 triangles, and fall back to the default car. There is no
limit for the whole model, but every surface is one draw call, so prefer few
large surfaces over many small ones.

```sh
python3 md3info.py ../../baseq3r/models/players/*/body.md3
# high-poly test version of a car, 2 passes = 16x the triangles
python3 subdivide.py $D/body.md3 /tmp/body_hp.md3 2
```

## Skyline (R34) – OBJ to MD3

`r34/` builds `baseq3r/models/players/skyline/` from the Sketchfab model
"Nissan Skyline R34 GT-R" by Lexyc16 (https://skfb.ly/6TxFX, CC BY 4.0, see
the `CREDITS.txt` in the model folder). The source zip is not in the repo.

| Script | Purpose |
| --- | --- |
| `obj.py` | Minimal OBJ reader (objects, materials, uvs, normals). |
| `r34/layout.py` | 1024x1024 paint atlas layout of the Skyline. |
| `r34/build_model.py` | OBJ -> `body.md3` (full detail, paint split into four surfaces, tags) and `wheel.md3` (the R34 tyre with UVs for the shared rim/tread textures). |
| `r34/gen_textures.py` | Paint atlases, trim/glass/chrome textures, lamp and grille textures from the source, skins. |

```sh
cd tools/carbuild
D=../../baseq3r/models/players/skyline
unzip nissan-skyline-r34-gt-r.zip -d /tmp/r34 && unzip /tmp/r34/source/*.zip -d /tmp/r34
python3 r34/build_model.py /tmp/r34/r34.obj $D
python3 r34/gen_textures.py /tmp/r34/textures $D
Q3R_FIT=1 python3 render3d.py $D red amg_163 /tmp/icon_red.png 384 200,60,45
Q3R_FIT=1 python3 render3d.py $D rally13 amg_163 /tmp/icon_rally13.png 384 200,60,45 \
    ../../baseq3r/models/players/viper/icon_rally13.tga
python3 md3info.py $D/body.md3 $D/wheel.md3
```

## GT-R (R35) – FBX to MD3 with culling and decimation

`gtr/` builds `baseq3r/models/players/gtr/` from the project's NissanGT-R.fbx
(757,016 triangles incl. interior and engine; not in the repo).

| Script | Purpose |
| --- | --- |
| `fbx.py` | Binary FBX 7.x reader (meshes, materials, model transforms). |
| `cull.py` | Keeps the triangles visible from outside (z-buffer from 81 directions). Slow on big models (~25 min here), so `build_model.py` caches the result. |
| `mesh.py` | Welding, quadric decimation via pyfqmr with fixed borders, crease-angle normals. |
| `carlib.py` | md3 surface builder/writer and the wheel builder for the shared rim selection (also used by `r34/`). |
| `paint.py` | Projection-atlas painter with per-car decal positions, shared part textures, skins (also used by `r34/`). |
| `gtr/build_model.py` | FBX -> `body.md3` (~100k triangles, big surface groups split below the engine limit), `wheel.md3`, `parts.tga` (material colour palette), tags. |
| `gtr/gen_textures.py` | Paint atlases and skins; run after `build_model.py`. |

```sh
pip install pyfqmr
cd tools/carbuild
D=../../baseq3r/models/players/gtr
python3 gtr/build_model.py NissanGT-R.fbx $D /tmp/gtr_cache
python3 gtr/gen_textures.py $D
python3 md3info.py $D/body.md3 $D/wheel.md3
# icons as for the Skyline (render3d.py with Q3R_FIT=1)
```

## Ridgeback – FBX with textures

`ridgeback/` builds `baseq3r/models/players/ridgeback/` from the project's
"lightbody lifted 85" model (`source/krmlgtbdy.fbx` + `textures/`; not in the
repo). Same steps as the GT-R, but small enough to keep all visible triangles
(no decimation), and every part except the paint keeps its own UVs and
textures (chassis, interior, lights).

```sh
cd tools/carbuild
D=../../baseq3r/models/players/ridgeback
python3 ridgeback/build_model.py <zip>/source/krmlgtbdy.fbx <zip>/textures $D /tmp/rb_cache
python3 ridgeback/gen_textures.py $D
python3 ground_fix.py --dry-run $D          # reports "ok"
```

`ground_fix.py` puts the tyres of any car on the ground (see `carlib.wheel_tag_z`).

## Cutlass – FBX with textures

`cutlass/` builds `baseq3r/models/players/cutlass/` from the project's
`cutlass-sketchfab.fbx` + `textures/` (not in the repo), like the Ridgeback:
all visible triangles of the body, the wheel decimated to 3,000 triangles
(needs pyfqmr), chrome and glass with the shared textures, mouldings and lamps
with their own.

```sh
cd tools/carbuild
D=../../baseq3r/models/players/cutlass
python3 cutlass/build_model.py <zip>/source/cutlass-sketchfab.fbx <zip>/textures $D /tmp/cl_cache
python3 cutlass/gen_textures.py $D
python3 tagview.py $D red /tmp/cutlass_tags.png   # check the light tags
```

## Bountyhunter – 3DS, original low-poly model

`bountyhunter/` builds `baseq3r/models/players/bountyhunter/` from the project's
`PoliceCar.3DS` + `sferiff.jpg`/`sferiffi.jpg` (not in the repo; `tds.py` reads
.3ds files). The car keeps its geometry and painted textures; the lamp, plate
and exhaust tags are placed where the painted parts sit on the model (uv -> 3d).
The wheel is a tyre turned from the source wheel's outline with the shared
tread and rim textures (`carlib.build_wheel`), so the rim selection works. The
extra liveries recolour only the unsaturated paint.

```sh
cd tools/carbuild
D=../../baseq3r/models/players/bountyhunter
python3 bountyhunter/build_model.py <zip>/PoliceCar.3DS <zip> $D
python3 bountyhunter/gen_textures.py <zip> $D
```

## GT500 – FBX with culling, without textures

`gt500/` builds `baseq3r/models/players/gt500/` from the project's Shelby GT500
model (`upload.fbx`, 348,009 triangles with interior, engine and suspension;
not in the repo), the same way as the GT-R: only what is visible from outside
is kept, the paint gets the projection atlas, the other parts a colour
palette, the wheel the shared rim textures. The visible part fits the budget,
so nothing is decimated; the tyre, whose separate tread blocks do not
decimate, is turned on a lathe from the source tyre's outline. The source's ground plane is left out
before culling, the manufacturer badges and lettering are dropped, and panels
wound inside out in the source (the top of the front wings) are turned the
right way round with `cull.visible_facing`.

```sh
pip install pyfqmr
cd tools/carbuild
D=../../baseq3r/models/players/gt500
python3 gt500/build_model.py upload.fbx $D /tmp/gt500_cache
python3 gt500/gen_textures.py $D
# icons as for the Skyline (render3d.py with Q3R_FIT=1, rim svt_cobra)
```

## Secret paints of the new cars

The 13 original cars carry seven secret paints in `baseq3r/zz_secret_skins.pk3`
(Retro, Camo, Carbon, Chrome, Neon, Hot Rod, Gold; unlocked by the profile
rank, see `uiSecretSkins` in `q3_ui/ui_rally_profiles.c`). `secret_paints.py`
makes the same seven paints for the cars built here.

The patterns are painted in world space: the projection cars get the world
position of every texel from their atlas layout, the Bountyhunter from its
body surface rasterised in texture space, so stripes and camouflage run on
across the atlas seams. The plain red atlas (the Bountyhunter: `sheriff_main`)
is the reference: its red is paint, everything else (panel gaps, grilles,
markers, rust, the underside patch) is kept, and the brightness of the red
carries the shading over. Chrome and Gold get a tint texture for the
reflection, Neon a pulsing glow, Hot Rod six flame frames, as on the old cars;
the icons get the star badge of the matching Sidepipe icon.

```sh
cd tools/carbuild
# badges: the secret icons of the Sidepipe, e.g. unpacked from the pk3
unzip ../../baseq3r/zz_secret_skins.pk3 'models/players/sidepipe/icon_*' -d /tmp/sec
Q3R_BADGES=/tmp/sec/models/players/sidepipe python3 secret_paints.py ../../baseq3r /tmp/secret_new
# Q3R_NO_ICONS=1 skips the (slow) icon renders; car names limit the run: ... /tmp/secret_new gtr
python3 merge_secret_pk3.py ../../baseq3r/zz_secret_skins.pk3 /tmp/secret_new
```

`merge_secret_pk3.py` adds the files to the pk3 and appends
`scripts/cars_secret_newcars.shader` to `scripts/cars_secret.shader` inside
it (shaders of an earlier run are replaced).

## DB12 – OBJ with culling and decimation

`db12/` builds `baseq3r/models/players/db12/` from `Aston Martin DB12 2024.obj`
(1,859,182 triangles with interior, brakes and very detailed wheels; not in the
repo; `db12/objnp.py` reads the OBJ into numpy arrays and caches them). Like the
GT-R: the wheels are dropped (shared rims, lathe-turned tyre), badges, plates,
the lamp lenses and the inner window glass are left out of the culling (the
lenses are one glass shell over all lamps, so the lamps behind them count as
visible). Parts seen only through narrow openings (grille, gills, vents, lamp
housings) are missed by the culling at its resolution and kept whole
(`KEEP_ALL`), otherwise the openings show the background. The paint keeps about
a third of its triangles, the rest shares what is left of about 85,000. Black
trim that culling cut into many bordered pieces is decimated a second time with
moving borders. Thin parts seen from both sides (grille, gills, vents, diffuser:
`TWO_SIDED`) get a reversed copy of their triangles with the normals turned round
(about 139,000 triangles in all); `cull none` would light their backs with the
front's normal, and undersides facing up flash white in game. `render3d.py`
with Q3R_CULL=1 draws like the game (no back faces; Q3R_TWOSIDED=parts exempts
two-sided textures), which shows such holes. The source's textures are ambient-occlusion maps; the parts take their
material colour (or the flat colour texture) from a palette.

`cull.py` works through the triangles in blocks (bounded memory) and takes
`views=` so `db12/build_model.py` can split the 81 views over CULL_JOBS
processes (default 2; ~1 h with two cores).

```sh
pip install pyfqmr
cd tools/carbuild
D=../../baseq3r/models/players/db12
python3 db12/build_model.py "<zip>/Aston Martin DB12 2024.obj" $D /tmp/db12_cache
python3 db12/gen_textures.py $D
# icons as for the Skyline (render3d.py with Q3R_FIT=1, rim amg_163)
```
