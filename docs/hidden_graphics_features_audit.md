# Versteckte Grafik-Features in der Q3Rally-Engine (Code-Audit)

## Methode
- Als **sichtbar im Menü** gelten Cvars, die in `ui_video.c` (Graphics) oder `ui_advanced_graphics.c` (Advanced Graphics) gesetzt/gelesen werden.
- Als **versteckt** gelten Renderer-Cvars aus `renderergl1/tr_init.c` und `renderergl2/tr_init.c`, die in diesen Menüs nicht auftauchen.

## Bereits im Menü sichtbar (nicht versteckt)
- Klassisches Grafikmenü: Auflösung/Fullscreen/Farbtiefe/Texturebits, LOD-Geometrie, Texture-Filter, Aniso, MSAA, Sun Shadows + Shadow Quality. 
- Advanced-Menü: HDR, PostProcess, ToneMap, AutoExposure, SSAO, Sun Rays, Dynamic Reflections.

## Versteckte Features (nicht im Grafikmenü)

### 1) Starke Upgrade-Hebel (visuelle Qualität)
- `r_pbr`
- `r_normalMapping`
- `r_specularMapping`
- `r_deluxeMapping`
- `r_parallaxMapping`
- `r_parallaxMapOffset`
- `r_parallaxMapShadows`
- `r_deluxeSpecular`
- `r_baseNormalX`, `r_baseNormalY`
- `r_baseParallax`
- `r_baseSpecular`
- `r_baseGloss`
- `r_glossType`
- `r_cubeMapping`
- `r_cubemapSize`
- `r_sunlightMode`
- `r_shadowBlur`
- `r_shadowCascadeZNear`, `r_shadowCascadeZFar`, `r_shadowCascadeZBias`
- `r_pshadowDist`
- `r_ext_framebuffer_object`
- `r_ext_texture_float`
- `r_ext_framebuffer_multisample` (wird teils intern gesetzt, aber nicht als eigenes UI-Feature geführt)
- `r_arb_seamless_cube_map`
- `r_floatLightmap`
- `r_depthPrepass`
- `r_mergeLightmaps`
- `r_imageUpsample`, `r_imageUpsampleMaxSize`, `r_imageUpsampleType`
- `r_genNormalMaps`

### 2) Mittlere Upgrade-Hebel / Feintuning
- `r_overBrightBits`
- `r_roundImagesDown`
- `r_simpleMipMaps`
- `r_detailtextures`
- `r_lodCurveError`, `r_lodbias`, `r_lodscale`
- `r_gamma`
- `r_ext_compressed_textures`
- `r_ext_multitexture`
- `r_ext_compiled_vertex_array`
- `r_ext_texture_env_add`
- `r_externalGLSL`
- `r_arb_vertex_array_object`
- `r_ext_direct_state_access`
- `r_vaoCache`

### 3) Atmosphärisch/„Look“ (situativ)
- `r_forceSun`, `r_forceSunLightScale`, `r_forceSunAmbientScale`
- `r_drawSun`
- `r_dynamiclight`, `r_dlightBacks`, `r_dlightMode`
- `r_fastsky`
- `r_greyscale`
- `r_inGameVideo`

### 4) Performance/Kompatibilität/Präsentation
- `r_swapInterval`
- `r_displayRefresh`
- `r_noborder`
- `r_customPixelAspect`
- `r_primitives`
- `r_ignoreFastPath`
- `r_ignoreGLErrors`
- `r_ignorehwgamma`
- `r_ignoreDstAlpha`
- `r_marksOnTriangleMeshes`
- `r_aviMotionJpegQuality`, `r_screenshotJpegQuality`
- `r_maxpolys`, `r_maxpolyverts`
- `r_stereoEnabled`, `r_stereoSeparation`, `r_anaglyphMode`

### 5) Debug-/Diagnose-Cvars (keine echten Upgrade-Hebel)
- `r_showImages`
- `r_debuglight`, `r_debugSort`
- `r_measureOverdraw`
- `r_logFile`, `r_verbose`
- `r_printShaders`, `r_saveFontData`
- `r_offsetfactor`, `r_offsetunits`, `r_drawBuffer`
- `r_ignore`
- `r_ambientScale`, `r_directedScale`
- `r_znear`, `r_zproj`
- `r_railWidth`, `r_railCoreWidth`, `r_railSegmentLength`
- `r_uifullscreen`


## Schatten-spezifischer Gap zum **ADVANCED GRAPHICS** Menü

### Bereits abgedeckt (aber im klassischen Graphics-Menü, nicht in Advanced)
- `r_sunShadows` (On/Off)
- `r_shadowFilter` (indirekt über Shadow Quality)
- `r_shadowMapSize` (indirekt über Shadow Quality)

### In Advanced Graphics **noch nicht integriert**
- `r_shadowBlur`
- `r_shadowCascadeZNear`
- `r_shadowCascadeZFar`
- `r_shadowCascadeZBias`
- `r_pshadowDist`
- `r_sunlightMode`

### Einordnung
- Derzeit steuert `ui_advanced_graphics.c` nur post-processing-nahe Features (`r_drawSunRays` etc.), aber keine dedizierten Shadow-Cascade/Blur-Parameter.
- Schatten-Controls liegen heute primär im klassischen `ui_video.c` (Sun Shadows + Quality-Mapping auf Filter/MapSize).

## Vollständige versteckte Cvar-Liste (Audit-Output)
`r_ambientScale`, `r_anaglyphMode`, `r_arb_seamless_cube_map`, `r_arb_vertex_array_object`, `r_aviMotionJpegQuality`, `r_baseGloss`, `r_baseNormalX`, `r_baseNormalY`, `r_baseParallax`, `r_baseSpecular`, `r_cameraExposure`, `r_cubeMapping`, `r_cubemapSize`, `r_customPixelAspect`, `r_debugSort`, `r_debuglight`, `r_deluxeMapping`, `r_deluxeSpecular`, `r_depthPrepass`, `r_detailtextures`, `r_directedScale`, `r_displayRefresh`, `r_dlightBacks`, `r_dlightMode`, `r_drawBuffer`, `r_drawSun`, `r_ext_compiled_vertex_array`, `r_ext_compressed_textures`, `r_ext_direct_state_access`, `r_ext_framebuffer_object`, `r_ext_multitexture`, `r_ext_texture_env_add`, `r_ext_texture_float`, `r_externalGLSL`, `r_floatLightmap`, `r_forceAutoExposure`, `r_forceAutoExposureMax`, `r_forceAutoExposureMin`, `r_forceSun`, `r_forceSunAmbientScale`, `r_forceSunLightScale`, `r_forceToneMap`, `r_forceToneMapAvg`, `r_forceToneMapMax`, `r_forceToneMapMin`, `r_genNormalMaps`, `r_glossType`, `r_greyscale`, `r_ignore`, `r_ignoreDstAlpha`, `r_ignoreFastPath`, `r_ignoreGLErrors`, `r_ignorehwgamma`, `r_imageUpsample`, `r_imageUpsampleMaxSize`, `r_imageUpsampleType`, `r_inGameVideo`, `r_lodCurveError`, `r_lodbias`, `r_lodscale`, `r_logFile`, `r_marksOnTriangleMeshes`, `r_maxpolys`, `r_maxpolyverts`, `r_measureOverdraw`, `r_mergeLightmaps`, `r_noborder`, `r_normalMapping`, `r_offsetfactor`, `r_offsetunits`, `r_parallaxMapOffset`, `r_parallaxMapShadows`, `r_parallaxMapping`, `r_pbr`, `r_primitives`, `r_printShaders`, `r_pshadowDist`, `r_railCoreWidth`, `r_railSegmentLength`, `r_railWidth`, `r_saveFontData`, `r_screenshotJpegQuality`, `r_shadowBlur`, `r_shadowCascadeZBias`, `r_shadowCascadeZFar`, `r_shadowCascadeZNear`, `r_showImages`, `r_simpleMipMaps`, `r_specularMapping`, `r_stereoEnabled`, `r_stereoSeparation`, `r_sunlightMode`, `r_swapInterval`, `r_uifullscreen`, `r_vaoCache`, `r_verbose`, `r_znear`, `r_zproj`.

## Kurzfazit
Die größten Hebel für ein sichtbares Grafik-Upgrade liegen im GL2-Feature-Block: Materialsystem (Normal/Specular/Parallax/PBR), Schatten-Kaskaden + Blur, Cubemaps/Reflections sowie Upsampling/Lightmap-Pfade.
