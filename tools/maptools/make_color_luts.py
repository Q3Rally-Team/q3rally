#!/usr/bin/env python3
"""
Q3Rally: builds the colour grading LUTs in baseq3r/gfx/luts/.

A LUT is a strip of N slices of N x N pixels (N*N wide, N high): red runs
across a slice, green down (top row = 0), blue from slice to slice. The
OpenGL2 renderer looks every pixel up in it after tone mapping
(r_colorGrade = strength, r_colorLut = force one look). A map picks its look
with the worldspawn key "colorlut" or with "q3r_colorLut <image>" in its sky
shader; without one it gets gfx/luts/q3r_default.

Usage:  python3 make_color_luts.py [output dir]   (needs numpy and Pillow)

To make a new look, add a function to LOOKS. It gets r, g, b (numpy arrays,
0..1, display colours) and returns the new r, g, b.
"""

import os
import sys

import numpy as np
from PIL import Image

N = 32  # LUT size: 32 x 32 x 32 (a 1024 x 32 image)


# ---------------------------------------------------------------- helpers

def luma(r, g, b):
    return 0.2126 * r + 0.7152 * g + 0.0722 * b


def saturation(r, g, b, amount):
    """amount 1 = unchanged, >1 more colour, <1 less"""
    l = luma(r, g, b)
    return l + (r - l) * amount, l + (g - l) * amount, l + (b - l) * amount


def contrast(r, g, b, amount):
    """S-curve around mid grey; amount 0 = unchanged, <0 flatter"""
    def s(x):
        x = np.clip(x, 0.0, 1.0)
        return x + (x * x * (3.0 - 2.0 * x) - x) * amount
    return s(r), s(g), s(b)


def balance(r, g, b, mr, mg, mb):
    """white balance: multiply each channel"""
    return r * mr, g * mg, b * mb


def split_tone(r, g, b, shadow, highlight, strength=1.0):
    """add a tint to the shadows and another to the highlights"""
    l = luma(r, g, b)
    ws = (1.0 - l) ** 2 * strength
    wh = l ** 2 * strength
    return (r + shadow[0] * ws + highlight[0] * wh,
            g + shadow[1] * ws + highlight[1] * wh,
            b + shadow[2] * ws + highlight[2] * wh)


def lift(r, g, b, black, white=1.0):
    """black: what black becomes, white: what white becomes"""
    return (black + r * (white - black), black + g * (white - black), black + b * (white - black))


# ------------------------------------------------------------------ looks

def neutral(r, g, b):
    return r, g, b


def q3r_default(r, g, b):
    # subtle: a little more contrast and colour, cool shadows, warm light
    r, g, b = contrast(r, g, b, 0.06)
    r, g, b = saturation(r, g, b, 1.03)
    r, g, b = split_tone(r, g, b, (-0.008, 0.000, 0.014), (0.014, 0.005, -0.010))
    return r, g, b


def warm(r, g, b):
    # desert, canyon, late afternoon sun
    r, g, b = balance(r, g, b, 1.06, 1.01, 0.88)
    r, g, b = contrast(r, g, b, 0.20)
    r, g, b = saturation(r, g, b, 1.12)
    r, g, b = split_tone(r, g, b, (0.006, -0.004, 0.012), (0.030, 0.014, -0.020))
    return r, g, b


def cold(r, g, b):
    # snow, winter, early morning
    r, g, b = balance(r, g, b, 0.93, 0.98, 1.07)
    r, g, b = saturation(r, g, b, 0.88)
    r, g, b = contrast(r, g, b, 0.10)
    r, g, b = lift(r, g, b, 0.018, 1.0)
    r, g, b = split_tone(r, g, b, (-0.006, 0.004, 0.020), (0.000, 0.006, 0.012))
    return r, g, b


def night(r, g, b):
    # night city: teal shadows, orange street lights
    r, g, b = contrast(r, g, b, 0.22)
    r, g, b = split_tone(r, g, b, (-0.020, 0.018, 0.040), (0.040, 0.012, -0.030))
    r, g, b = saturation(r, g, b, 1.06)
    return r, g, b


def vivid(r, g, b):
    # arcade: strong colours
    r, g, b = contrast(r, g, b, 0.25)
    r, g, b = saturation(r, g, b, 1.28)
    return r, g, b


def overcast(r, g, b):
    # grey sky, rain
    r, g, b = balance(r, g, b, 0.97, 0.99, 1.03)
    r, g, b = saturation(r, g, b, 0.80)
    r, g, b = contrast(r, g, b, -0.06)
    r, g, b = lift(r, g, b, 0.020, 0.98)
    return r, g, b


def dusk(r, g, b):
    # sunset: violet shadows, golden light
    r, g, b = contrast(r, g, b, 0.16)
    r, g, b = split_tone(r, g, b, (0.012, -0.008, 0.030), (0.040, 0.016, -0.030))
    r, g, b = saturation(r, g, b, 1.10)
    return r, g, b


LOOKS = {
    "neutral": neutral,
    "q3r_default": q3r_default,
    "warm": warm,
    "cold": cold,
    "night": night,
    "vivid": vivid,
    "overcast": overcast,
    "dusk": dusk,
}


def build(look):
    x = np.arange(N * N) % N
    s = np.arange(N * N) // N
    y = np.arange(N)
    r = np.tile((x / (N - 1.0))[None, :], (N, 1))
    b = np.tile((s / (N - 1.0))[None, :], (N, 1))
    g = np.tile((y / (N - 1.0))[:, None], (1, N * N))
    r, g, b = look(r, g, b)
    rgb = np.stack([r, g, b], axis=-1)
    return Image.fromarray((np.clip(rgb, 0.0, 1.0) * 255.0 + 0.5).astype(np.uint8), "RGB")


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    out = sys.argv[1] if len(sys.argv) > 1 else os.path.join(here, "..", "..", "baseq3r", "gfx", "luts")
    os.makedirs(out, exist_ok=True)
    for name, look in LOOKS.items():
        path = os.path.join(out, name + ".tga")
        build(look).save(path)
        print(path)


if __name__ == "__main__":
    main()
