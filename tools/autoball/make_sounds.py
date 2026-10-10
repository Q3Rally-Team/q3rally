#!/usr/bin/env python3
"""Synthesise the Autoball sound effects (no samples, no licences needed).

    python3 tools/autoball/make_sounds.py [--out baseq3r/sound/autoball]

Writes mono 44.1 kHz Ogg Vorbis files (needs numpy and ffmpeg with libvorbis):
    hit_soft.ogg    car touches the ball
    hit_hard.ogg    hard car hit / shot
    bounce.ogg      ball bounces off a wall, the floor or the ceiling
    goal_horn.ogg   stadium horn after a goal
    whistle.ogg     referee whistle at kick-off
    crowd_cheer.ogg goal cheer: sound/world/crowds.ogg cut to 7 s with a
                    3 s fade-out, so it dies away before the kick-off
                    whistle instead of stopping hard (needs that file)
Every sound is generated from a fixed random seed, so re-running the script
reproduces the same files.
"""
import argparse
import os
import subprocess
import tempfile
import wave

import numpy as np

SR = 44100
RNG = np.random.default_rng(23)


def t_axis(seconds):
    return np.arange(int(SR * seconds)) / SR


def decay(t, tau):
    return np.exp(-t / tau)


def sweep_sine(t, f0, f1, tau):
    """sine whose pitch falls exponentially from f0 to f1"""
    f = f1 + (f0 - f1) * np.exp(-t / tau)
    return np.sin(2 * np.pi * np.cumsum(f) / SR)


def band_noise(n, lo, hi):
    spec = np.fft.rfft(RNG.standard_normal(n))
    freqs = np.fft.rfftfreq(n, 1 / SR)
    spec[(freqs < lo) | (freqs > hi)] = 0
    out = np.fft.irfft(spec, n)
    return out / (np.max(np.abs(out)) + 1e-9)


def lowpass(x, cutoff):
    a = np.exp(-2 * np.pi * cutoff / SR)
    y = np.empty_like(x)
    acc = 0.0
    for i, v in enumerate(x):
        acc = (1 - a) * v + a * acc
        y[i] = acc
    return y


def fade(x, attack=0.002, release=0.02):
    n = len(x)
    env = np.ones(n)
    a = max(1, int(attack * SR))
    r = max(1, int(release * SR))
    env[:a] = np.linspace(0, 1, a)
    env[-r:] *= np.linspace(1, 0, r)
    return x * env


def normalise(x, peak_db):
    peak = 10 ** (peak_db / 20)
    return x / (np.max(np.abs(x)) + 1e-9) * peak


def hit_soft():
    t = t_axis(0.35)
    thump = sweep_sine(t, 190, 105, 0.03) * decay(t, 0.06)
    slap = band_noise(len(t), 300, 1400) * decay(t, 0.012) * 0.45
    hollow = (np.sin(2 * np.pi * 420 * t) * decay(t, 0.05) * 0.22 +
              np.sin(2 * np.pi * 655 * t) * decay(t, 0.035) * 0.12)
    return normalise(fade(thump + slap + hollow), -2)


def hit_hard():
    t = t_axis(0.6)
    thump = sweep_sine(t, 150, 58, 0.05) * decay(t, 0.11)
    crack = band_noise(len(t), 150, 6000) * decay(t, 0.022) * 0.7
    shell = (np.sin(2 * np.pi * 330 * t) * decay(t, 0.08) * 0.28 +
             np.sin(2 * np.pi * 515 * t) * decay(t, 0.06) * 0.16 +
             np.sin(2 * np.pi * 860 * t) * decay(t, 0.03) * 0.08)
    x = np.tanh(1.8 * (thump + crack + shell))
    return normalise(fade(x), -1)


def bounce():
    t = t_axis(0.3)
    thump = sweep_sine(t, 260, 150, 0.02) * decay(t, 0.045)
    tick = band_noise(len(t), 900, 5000) * decay(t, 0.006) * 0.35
    ring = np.sin(2 * np.pi * 720 * t) * decay(t, 0.03) * 0.15
    return normalise(fade(thump + tick + ring), -3)


def goal_horn():
    dur = 2.9
    t = t_axis(dur)
    vib = 1 + 0.003 * np.sin(2 * np.pi * 5.2 * t)
    x = np.zeros_like(t)
    # buzzy stadium horn: three detuned sawtooth voices, band limited
    for f0, gain in ((155.6, 1.0), (196.0, 0.7), (233.1, 0.55), (156.4, 0.6)):
        phase = 2 * np.pi * np.cumsum(f0 * vib) / SR
        k = 1
        while f0 * k < 5000:
            x += gain * np.sin(k * phase) / k
            k += 1
    x = lowpass(x, 2200)
    x += band_noise(len(t), 400, 3000) * 0.015      # a bit of air
    env = np.ones_like(t)
    a, r = int(0.08 * SR), int(0.45 * SR)
    env[:a] = np.linspace(0, 1, a) ** 0.6
    env[-r:] *= np.linspace(1, 0, r) ** 1.5
    x = np.tanh(1.4 * x / np.max(np.abs(x))) * env
    return normalise(x, -2)


def whistle():
    dur = 0.85
    t = t_axis(dur)
    trill = np.sin(2 * np.pi * 29 * t)               # the pea rattling
    f = 2950 + 110 * trill
    tone = np.sin(2 * np.pi * np.cumsum(f) / SR)
    tone *= 0.65 + 0.35 * (0.5 + 0.5 * trill)
    tone += 0.12 * np.sin(2 * np.pi * 2 * np.cumsum(f) / SR)
    breath = band_noise(len(t), 2200, 4200) * 0.12
    env = np.ones_like(t)
    a, r = int(0.025 * SR), int(0.12 * SR)
    env[:a] = np.linspace(0, 1, a)
    env[-r:] *= np.linspace(1, 0, r)
    return normalise((tone + breath) * env, -3)


SOUNDS = {
    "hit_soft": hit_soft,
    "hit_hard": hit_hard,
    "bounce": bounce,
    "goal_horn": goal_horn,
    "whistle": whistle,
}


def write_ogg(samples, path):
    pcm = (np.clip(samples, -1, 1) * 32767).astype(np.int16)
    with tempfile.NamedTemporaryFile(suffix=".wav", delete=False) as tmp:
        wav_path = tmp.name
    try:
        with wave.open(wav_path, "wb") as w:
            w.setnchannels(1)
            w.setsampwidth(2)
            w.setframerate(SR)
            w.writeframes(pcm.tobytes())
        subprocess.run(["ffmpeg", "-y", "-loglevel", "error", "-i", wav_path,
                        "-c:a", "libvorbis", "-q:a", "5", path], check=True)
    finally:
        os.unlink(wav_path)


CROWD_LENGTH = 7.0      # goal celebration (4 s) + kick-off countdown (3 s)
CROWD_FADE = 3.0


def crowd_cheer(path):
    """fade the stock crowd sample out instead of letting it end at full volume"""
    here = os.path.dirname(os.path.abspath(__file__))
    source = os.path.normpath(os.path.join(here, "..", "..", "baseq3r", "sound", "world", "crowds.ogg"))
    if not os.path.exists(source):
        print(f"skipped {path}: {source} not found")
        return
    fade_start = CROWD_LENGTH - CROWD_FADE
    subprocess.run(["ffmpeg", "-y", "-loglevel", "error", "-i", source,
                    "-af", f"atrim=0:{CROWD_LENGTH},afade=t=in:d=0.08,"
                           f"afade=t=out:st={fade_start}:d={CROWD_FADE}:curve=qsin",
                    "-c:a", "libvorbis", "-q:a", "4", path], check=True)
    print(f"{path}  ({os.path.getsize(path)} bytes)")


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    default_out = os.path.normpath(os.path.join(here, "..", "..", "baseq3r", "sound", "autoball"))
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--out", default=default_out)
    args = parser.parse_args()
    os.makedirs(args.out, exist_ok=True)
    for name, make in SOUNDS.items():
        path = os.path.join(args.out, name + ".ogg")
        write_ogg(make(), path)
        print(f"{path}  ({os.path.getsize(path)} bytes)")
    crowd_cheer(os.path.join(args.out, "crowd_cheer.ogg"))


if __name__ == "__main__":
    main()
