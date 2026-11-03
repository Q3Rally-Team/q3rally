#!/usr/bin/env python3
"""Check compiled Crossroads collision against the editable terrain generator.

Reads Q3 IBSP collision planes and solid-content brushes directly. This catches
missing collision from shader contents, inverted brush planes, and startsolid
entities which source-only geometric checks cannot detect. No files are written.
"""
from __future__ import annotations

import importlib.util
import json
import math
import re
import struct
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EPSILON = 0.02


def load_generator():
    spec = importlib.util.spec_from_file_location("crossroads_source", Path(__file__).with_name("make_crossroads.py"))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    module.terrain()
    module.dressing()
    return module


class Collision:
    def __init__(self, path):
        data = path.read_bytes()
        assert data[:4] == b"IBSP", "Expected Q3 IBSP"
        assert struct.unpack_from("<i", data, 4)[0] == 46, "Expected Q3 BSP version 46"

        def lump(number):
            start, length = struct.unpack_from("<ii", data, 8 + number * 8)
            return data[start:start + length]

        self.entities = [dict(re.findall(r'"([^"\n]+)"\s+"([^"\n]*)"', block))
                         for block in re.findall(r"\{([^{}]*)\}", lump(0).decode("ascii"))]
        self.shaders = [(name.split(b"\0", 1)[0].decode("ascii"), surface, contents)
                        for name, surface, contents in struct.iter_unpack("<64sii", lump(1))]
        planes = list(struct.iter_unpack("<ffff", lump(2)))
        sides = list(struct.iter_unpack("<ii", lump(9)))
        self.brushes = []
        self.boundary_brushes = []
        self.solid_shader_counts = Counter()
        for first, count, shader in struct.iter_unpack("<iii", lump(8)):
            if not self.shaders[shader][2] & 1:
                continue
            equations = [planes[sides[index][0]] for index in range(first, first + count)]
            lo, hi = [-math.inf] * 3, [math.inf] * 3
            for equation in equations:
                normal, distance = equation[:3], equation[3]
                for axis in range(3):
                    if abs(normal[axis]) > 0.99999 and all(abs(normal[a]) < 0.00001 for a in range(3) if a != axis):
                        if normal[axis] > 0:
                            hi[axis] = min(hi[axis], distance)
                        else:
                            lo[axis] = max(lo[axis], -distance)
            self.brushes.append((lo, hi, equations))
            if self.shaders[shader][0] == "textures/q3r_crossroads/boundary":
                self.boundary_brushes.append((lo, hi, equations))
            self.solid_shader_counts[self.shaders[shader][0]] += 1

    def interval(self, x, y, half=(0, 0, 0)):
        """Solid intervals along a vertical line (optionally an axis-aligned box)."""
        for bounds_lo, bounds_hi, equations in self.brushes:
            if not (bounds_lo[0] - half[0] - EPSILON <= x <= bounds_hi[0] + half[0] + EPSILON
                    and bounds_lo[1] - half[1] - EPSILON <= y <= bounds_hi[1] + half[1] + EPSILON):
                continue
            lower, upper = -math.inf, math.inf
            for nx, ny, nz, distance in equations:
                support = abs(nx) * half[0] + abs(ny) * half[1] + abs(nz) * half[2]
                remaining = distance + support - nx * x - ny * y
                if abs(nz) < 1e-7:
                    if remaining < -EPSILON:
                        break
                elif nz > 0:
                    upper = min(upper, remaining / nz)
                else:
                    lower = max(lower, remaining / nz)
                if lower > upper + EPSILON:
                    break
            else:
                if lower <= upper + EPSILON:
                    yield lower, upper

    def floor(self, x, y, start_z=1000):
        heights = [upper for lower, upper in self.interval(x, y) if lower <= start_z and upper < start_z]
        assert heights, f"No solid floor under {x:g} {y:g}"
        return max(heights)

    def startsolid(self, x, y, z, half):
        return any(lower + EPSILON < z < upper - EPSILON for lower, upper in self.interval(x, y, half))

    def sweep(self, start, end, half, brushes=None):
        """First fraction where a swept car box enters compiled solid planes."""
        first_hit = 1.0
        for _, _, equations in self.brushes if brushes is None else brushes:
            enter, leave = 0.0, 1.0
            for nx, ny, nz, distance in equations:
                support = abs(nx) * half[0] + abs(ny) * half[1] + abs(nz) * half[2]
                a = nx * start[0] + ny * start[1] + nz * start[2] - distance - support
                b = nx * end[0] + ny * end[1] + nz * end[2] - distance - support
                if a > 0 and b > 0:
                    break
                if a > b and a > 0:
                    enter = max(enter, a / (a - b))
                elif b > a and b > 0:
                    leave = min(leave, a / (a - b))
                if enter > leave:
                    break
            else:
                first_hit = min(first_hit, enter)
        return first_hit


def main():
    source = load_generator()
    bsp = Collision(ROOT / "baseq3r" / "maps" / "q3r_crossroads.bsp")
    # Tire friction comes from these compiled BSP surface bits, not shader text.
    # The terrain brush sides use the t_* parent shaders (see make_crossroads.py).
    expected_materials = {"grass": 0x100000, "dirt": 0x4000000, "gravel": 0x1000000}
    material_flags = {}
    for material, expected in expected_materials.items():
        shaders = [surface for name, surface, _ in bsp.shaders if name == f"textures/q3r_crossroads/t_{material}"]
        assert shaders and all(flags & expected for flags in shaders), f"Missing compiled {material} friction bit {expected:#x}"
        material_flags[material] = hex(expected)
    boundary_shaders = [(surface, contents) for name, surface, contents in bsp.shaders
                        if name == "textures/q3r_crossroads/boundary"]
    assert boundary_shaders and all(contents & 1 for _, contents in boundary_shaders), "Boundary lost CONTENTS_SOLID"
    assert len(bsp.boundary_brushes) == 4, "Expected four solid perimeter brushes"
    boundary_sweeps = []
    e = source.EXTENT
    for axis in (0, 1):
        for sign in (-1, 1):
            point, outside, start = [0, 0, 512], [0, 0, 512], [0, 0, 512]
            point[axis], outside[axis], start[axis] = sign * (e - 48), sign * (e + 7), sign * (e - 98)
            half = (50, 25, 22) if axis == 0 else (25, 50, 22)
            assert not bsp.startsolid(*point, (0, 0, 0)), f"Inside boundary probe obstructed: {point}"
            assert not bsp.startsolid(*start, half), f"Outbound sweep begins in solid: {start}"
            assert bsp.startsolid(*outside, half), f"Car can cross outer boundary: {outside}"
            fraction = bsp.sweep(start, outside, half, bsp.boundary_brushes)
            assert 0 < fraction < 1, f"Perimeter fails outbound sweep: {outside}"
            assert abs(fraction - 48 / 105) < 0.0001, f"Unexpected perimeter contact plane: {outside}"
            assert abs(bsp.sweep(start, outside, half) - fraction) < 0.0001, "Unexpected obstruction before perimeter"
            boundary_sweeps.append({"side": ("x" if axis == 0 else "y") + ("-" if sign < 0 else "+"),
                                    "first_contact_car_center": sign * (e - 50),
                                    "fraction": round(fraction, 6)})
    # Sample both triangles per terrain cell. Skip scenery footprints because
    # its upper surfaces legitimately sit above the analytic terrain floor.
    comparisons, worst_error = 0, 0.0
    step = source.STEP
    for y in range(-e, e, step):
        for x in range(-e, e, step):
            for u, v in ((0.23, 0.31), (0.74, 0.69), (0.47, 0.81)):
                px, py = x + u * step, y + v * step
                if any(lo[0] - 1 <= px <= hi[0] + 1 and lo[1] - 1 <= py <= hi[1] + 1 for lo, hi in source.SOLIDS):
                    continue
                expected = source.ground(px, py)
                actual = bsp.floor(px, py)
                error = abs(actual - expected)
                assert error < 0.15, f"Floor mismatch at {(px, py)}: BSP={actual:g} source={expected:g}"
                worst_error = max(worst_error, error)
                comparisons += 1

    counts = Counter(e.get("classname") for e in bsp.entities)
    starts, drops = 0, 0
    for entity in bsp.entities:
        classname = entity.get("classname", "")
        if "origin" not in entity:
            continue
        x, y, z = map(float, entity["origin"].split())
        if classname.endswith(("player", "spawn")) or classname == "info_player_deathmatch":
            # Server adds 9 units to team/DM starts. The car has two 50x50x44
            # collision boxes offset 25 units along its forward vector.
            angle = math.radians(float(entity.get("angle", "0")))
            for offset in (-25, 25):
                cx, cy = x + offset * math.cos(angle), y + offset * math.sin(angle)
                assert not bsp.startsolid(cx, cy, z + 9, (25, 25, 22)), f"Car starts in compiled solid: {entity}"
            starts += 1
        if classname.startswith(("team_CTF_", "weapon_", "ammo_", "item_", "rally_item_")) and not classname.endswith(("player", "spawn")):
            assert not bsp.startsolid(x, y, z, (15, 15, 15)), f"Item starts in compiled solid: {entity}"
            # A dropped box can rest slightly above a point ray on a slope.
            floor = max(upper for lower, upper in bsp.interval(x, y, (15, 15, 15)) if lower <= z and upper < z)
            assert abs(floor - source.ground(x, y)) <= 25, f"Item floor is obstructed: {entity}"
            drops += 1
    for color in ("red", "blue", "green", "yellow"):
        assert counts[f"team_CTF_{color}player"] >= 2
        assert counts[f"team_CTF_{color}spawn"] >= 4
        assert counts[f"team_CTF_{color}flag"] == 1
    print(json.dumps({"bsp": "q3r_crossroads.bsp", "solid_brushes": len(bsp.brushes),
                      "solid_brush_shader_counts": dict(bsp.solid_shader_counts),
                      "terrain_material_surface_bits": material_flags,
                      "boundary_solid_brushes": len(bsp.boundary_brushes),
                      "boundary_contents": boundary_shaders[0][1],
                      "boundary_car_sweeps": boundary_sweeps,
                      "terrain_floor_comparisons": comparisons,
                      "max_floor_error_units": round(worst_error, 6),
                      "safe_car_start_positions": starts, "safe_item_drop_positions": drops,
                      "ctf4_flags": 4, "status": "compiled collision checks passed"}, indent=2))


if __name__ == "__main__":
    main()
