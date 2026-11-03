#!/usr/bin/env python3
"""Fit Q3Rally Bezier anchors and handles to a recorded lap.

The BSP checkpoint entity origin is a respawn position. This tool instead
intersects the Ghost samples with each inline checkpoint brush and uses those
actual gate crossings as Bezier anchors.
"""
import argparse
from pathlib import Path
import math
import re
import struct


LUMP_COUNT = 17
LUMP_ENTITIES = 0
LUMP_SHADERS = 1
LUMP_PLANES = 2
LUMP_MODELS = 7
LUMP_BRUSHES = 8
LUMP_BRUSHSIDES = 9
LUMP_DRAWVERTS = 10
LUMP_DRAWINDEXES = 11
LUMP_SURFACES = 13


def read_lumps(data):
    if len(data) < 8 + LUMP_COUNT * 8 or data[:4] != b'IBSP':
        raise ValueError('not a Quake 3 IBSP file')
    version = struct.unpack_from('<i', data, 4)[0]
    if version != 46:
        raise ValueError(f'unsupported BSP version {version}; expected 46')
    lumps = []
    for index in range(LUMP_COUNT):
        offset, length = struct.unpack_from('<ii', data, 8 + index * 8)
        if offset < 0 or length < 0 or offset + length > len(data):
            raise ValueError(f'invalid BSP lump {index}')
        lumps.append(data[offset:offset + length])
    return lumps


def parse_entities(blob):
    entities = []
    entity_re = re.compile(rb'\{([^}]*)\}')
    pair_re = re.compile(rb'"([^"\\]*)"\s+"([^"\\]*)"')
    for block in entity_re.finditer(blob):
        entity = {key.decode('ascii'): value.decode('ascii')
                  for key, value in pair_re.findall(block.group(1))}
        if entity:
            entities.append(entity)
    return entities


def parse_vec(text, context):
    try:
        values = tuple(float(part) for part in text.split())
    except (AttributeError, ValueError) as error:
        raise ValueError(f'{context}: invalid vector {text!r}') from error
    if len(values) != 3 or not all(math.isfinite(value) for value in values):
        raise ValueError(f'{context}: expected three finite coordinates')
    return values


def parse_bsp_gates(lumps):
    shader_data = lumps[LUMP_SHADERS]
    if len(shader_data) % 72:
        raise ValueError('BSP shader lump has invalid size')
    shaders = [shader_data[i:i + 64].split(b'\0', 1)[0].decode('ascii')
               for i in range(0, len(shader_data), 72)]
    models, brushes = lumps[LUMP_MODELS], lumps[LUMP_BRUSHES]
    sides, planes = lumps[LUMP_BRUSHSIDES], lumps[LUMP_PLANES]
    if len(models) % 40 or len(brushes) % 12 or len(sides) % 8 or len(planes) % 16:
        raise ValueError('BSP model/brush/plane lump has invalid size')
    model_count = len(models) // 40
    brush_count = len(brushes) // 12
    side_count = len(sides) // 8
    plane_count = len(planes) // 16
    entities = parse_entities(lumps[LUMP_ENTITIES])
    checkpoints = {}
    starts = []
    for entity in entities:
        classname = entity.get('classname', '').lower()
        if classname not in ('rally_checkpoint', 'rally_startfinish'):
            continue
        try:
            model_index = int(entity['model'][1:])
        except (KeyError, ValueError, IndexError) as error:
            raise ValueError(f'{classname} has no valid inline BSP model') from error
        if not 0 < model_index < model_count:
            raise ValueError(f'{classname} references invalid BSP model *{model_index}')
        origin = parse_vec(entity.get('origin'), f'{classname} origin')
        model = struct.unpack_from('<6f4i', models, model_index * 40)
        first_brush, num_brushes = model[8], model[9]
        gate_brushes = []
        for brush_index in range(first_brush, first_brush + num_brushes):
            if not 0 <= brush_index < brush_count:
                raise ValueError(f'{classname} model has an invalid brush index')
            first_side, num_sides, shader_index = struct.unpack_from('<3i', brushes, brush_index * 12)
            if not 0 <= shader_index < len(shaders):
                raise ValueError(f'{classname} brush has an invalid shader index')
            shader_name = shaders[shader_index].lower()
            wanted_shader = 'common/startfinish' if classname == 'rally_startfinish' else 'common/checkpoint'
            if wanted_shader not in shader_name:
                continue
            brush_planes = []
            for side_index in range(first_side, first_side + num_sides):
                if not 0 <= side_index < side_count:
                    raise ValueError(f'{classname} brush has an invalid side index')
                plane_index, _ = struct.unpack_from('<2i', sides, side_index * 8)
                if not 0 <= plane_index < plane_count:
                    raise ValueError(f'{classname} brush has an invalid plane index')
                nx, ny, nz, distance = struct.unpack_from('<4f', planes, plane_index * 16)
                brush_planes.append(((nx, ny, nz), distance))
            if len(brush_planes) >= 4:
                gate_brushes.append(brush_planes)
        if len(gate_brushes) != 1:
            raise ValueError(f'{classname} model *{model_index} has {len(gate_brushes)} checkpoint brushes; expected 1')
        gate = {'origin': origin, 'planes': gate_brushes[0]}
        if classname == 'rally_startfinish':
            starts.append(gate)
        else:
            try:
                number = int(entity['number'])
            except (KeyError, ValueError) as error:
                raise ValueError('rally_checkpoint has no valid number') from error
            if number in checkpoints:
                raise ValueError(f'duplicate checkpoint number {number}')
            checkpoints[number] = gate
    if len(starts) != 1:
        raise ValueError(f'expected one rally_startfinish entity, found {len(starts)}')
    count = len(checkpoints)
    if not count or set(checkpoints) != set(range(1, count + 1)):
        raise ValueError('checkpoint numbering must be contiguous from 1')
    return starts[0], [checkpoints[index] for index in range(1, count + 1)]


def quadratic_point(points, t):
    u = 1.0 - t
    return tuple(u*u*points[0][axis] + 2.0*u*t*points[1][axis] + t*t*points[2][axis]
                 for axis in range(3))


def patch_point(grid, u, v):
    along_u = [quadratic_point(row, u) for row in grid]
    return quadratic_point(along_u, v)


def parse_road_triangles(lumps, patch_resolution=6):
    """Read visible road-material triangles as a track-surface preview proxy."""
    shaders = lumps[LUMP_SHADERS]
    if len(shaders) % 72:
        raise ValueError('BSP shader lump has invalid size')
    shader_names = [shaders[i:i + 64].split(b'\0', 1)[0].decode('ascii')
                    for i in range(0, len(shaders), 72)]
    verts_blob, indexes_blob, surfaces_blob = (
        lumps[LUMP_DRAWVERTS], lumps[LUMP_DRAWINDEXES], lumps[LUMP_SURFACES])
    if len(verts_blob) % 44 or len(indexes_blob) % 4 or len(surfaces_blob) % 104:
        raise ValueError('BSP draw-surface lump has invalid size')
    vertex_count, index_count = len(verts_blob) // 44, len(indexes_blob) // 4
    verts = [struct.unpack_from('<3f', verts_blob, index * 44) for index in range(vertex_count)]
    indexes = [struct.unpack_from('<i', indexes_blob, index * 4)[0]
               for index in range(index_count)]
    triangles = []
    road_surface_count = 0

    def append_triangle(a, b, c):
        if math.dist(a, b) > 0.01 and math.dist(b, c) > 0.01 and math.dist(c, a) > 0.01:
            triangles.append((a, b, c))

    for offset in range(0, len(surfaces_blob), 104):
        shader_index, _, surface_type, first_vert, num_verts, first_index, num_indexes = \
            struct.unpack_from('<7i', surfaces_blob, offset)
        if not 0 <= shader_index < len(shader_names):
            raise ValueError('BSP draw surface has an invalid shader index')
        material = shader_names[shader_index].lower()
        if 'road' not in material and 'asphalt' not in material:
            continue
        road_surface_count += 1
        if surface_type in (1, 3):  # MST_PLANAR / MST_TRIANGLE_SOUP
            if first_index < 0 or first_index + num_indexes > index_count:
                raise ValueError('BSP road surface references invalid draw indexes')
            for index in range(first_index, first_index + num_indexes - 2, 3):
                ids = [first_vert + indexes[index + delta] for delta in range(3)]
                if all(0 <= vertex < vertex_count for vertex in ids):
                    append_triangle(*(verts[vertex] for vertex in ids))
        elif surface_type == 2:  # MST_PATCH, quadratic Bezier control grid
            patch_width, patch_height = struct.unpack_from('<2i', surfaces_blob, offset + 96)
            if patch_width < 3 or patch_height < 3 or patch_width * patch_height > num_verts:
                continue
            if first_vert < 0 or first_vert + patch_width * patch_height > vertex_count:
                raise ValueError('BSP road patch references invalid draw vertices')
            for row in range(0, patch_height - 2, 2):
                for col in range(0, patch_width - 2, 2):
                    grid = [[verts[first_vert + (row + dy) * patch_width + col + dx]
                             for dx in range(3)] for dy in range(3)]
                    sampled = [[patch_point(grid, u / patch_resolution, v / patch_resolution)
                                for u in range(patch_resolution + 1)]
                               for v in range(patch_resolution + 1)]
                    for v in range(patch_resolution):
                        for u in range(patch_resolution):
                            p00, p10 = sampled[v][u], sampled[v][u + 1]
                            p01, p11 = sampled[v + 1][u], sampled[v + 1][u + 1]
                            append_triangle(p00, p10, p11)
                            append_triangle(p00, p11, p01)
    return triangles, road_surface_count


def point_on_road_mesh(point, road_triangles, height_tolerance):
    px, py, pz = point
    for triangle in road_triangles:
        a, b, c = triangle
        min_x, max_x = min(a[0], b[0], c[0]), max(a[0], b[0], c[0])
        min_y, max_y = min(a[1], b[1], c[1]), max(a[1], b[1], c[1])
        if px < min_x - 0.1 or px > max_x + 0.1 or py < min_y - 0.1 or py > max_y + 0.1:
            continue
        v0x, v0y = b[0] - a[0], b[1] - a[1]
        v1x, v1y = c[0] - a[0], c[1] - a[1]
        v2x, v2y = px - a[0], py - a[1]
        denominator = v0x * v1y - v1x * v0y
        if abs(denominator) < 1e-6:
            continue
        u = (v2x * v1y - v1x * v2y) / denominator
        v = (v0x * v2y - v2x * v0y) / denominator
        if u < -1e-4 or v < -1e-4 or u + v > 1.0001:
            continue
        z = a[2] + u * (b[2] - a[2]) + v * (c[2] - a[2])
        if abs(pz - z) <= height_tolerance:
            return True
    return False


def sample_bezier_route(anchors, handles, spacing=72.0):
    route = []
    for index, anchor in enumerate(anchors):
        next_index = (index + 1) % len(anchors)
        p0, p1 = anchor['point'], anchors[next_index]['point']
        h0, h1 = handles[index], handles[next_index]
        coarse = [bezier_point(p0, p1, h0, h1, step / 16.0) for step in range(17)]
        length = sum(math.dist(a, b) for a, b in zip(coarse, coarse[1:]))
        steps = max(8, math.ceil(length / spacing))
        if not route:
            route.append(p0)
        route.extend(bezier_point(p0, p1, h0, h1, step / steps)
                     for step in range(1, steps + 1))
    return route


def write_route_preview_svg(path, map_name, road_triangles, ghost_samples, route_points, anchors,
                            offroad_points, metadata, fit_rms, road_coverage):
    width, height, pad = 1500.0, 1050.0, 44.0
    xs = [point[0] for triangle in road_triangles for point in triangle]
    ys = [point[1] for triangle in road_triangles for point in triangle]
    xs.extend(point[0] for point in route_points)
    ys.extend(point[1] for point in route_points)
    if not xs:
        raise ValueError('cannot create preview without road surfaces')
    min_x, max_x = min(xs), max(xs)
    min_y, max_y = min(ys), max(ys)
    span_x, span_y = max(max_x - min_x, 1.0), max(max_y - min_y, 1.0)
    scale = min((width - 2 * pad) / span_x, (height - 2 * pad) / span_y)
    draw_w, draw_h = span_x * scale, span_y * scale
    offset_x, offset_y = (width - draw_w) * 0.5, (height - draw_h) * 0.5

    def xy(point):
        return offset_x + (point[0] - min_x) * scale, height - offset_y - (point[1] - min_y) * scale

    def polyline(points, color, stroke_width, opacity=1.0):
        coords = ' '.join(f'{xy(point)[0]:.1f},{xy(point)[1]:.1f}' for point in points)
        return f'<polyline points="{coords}" fill="none" stroke="{color}" stroke-width="{stroke_width}" stroke-opacity="{opacity}" stroke-linejoin="round" stroke-linecap="round"/>'

    svg = [f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {width:.0f} {height:.0f}">',
           '<rect width="100%" height="100%" fill="#141a20"/>',
           '<g id="road-surfaces" fill="#66717b" fill-opacity="0.55" stroke="#8a969f" stroke-opacity="0.12" stroke-width="0.6">']
    for triangle in road_triangles:
        points = ' '.join(f'{x:.1f},{y:.1f}' for x, y in (xy(p) for p in triangle))
        svg.append(f'<polygon points="{points}"/>')
    svg.extend(['</g>', '<g id="ghost">', polyline([point for _, point in ghost_samples], '#48a9ff', 3.0, 0.75),
                '</g>', '<g id="bpd">', polyline(route_points, '#ffb52e', 4.0), '</g>',
                '<g id="gates" fill="#65df9a" stroke="#102016" stroke-width="2">'])
    for anchor in anchors:
        x, y = xy(anchor['point'])
        svg.append(f'<circle cx="{x:.1f}" cy="{y:.1f}" r="5"/><title>{anchor["label"]}</title>')
        if anchor['number'] != len(anchors):
            svg.append(f'<text x="{x + 7:.1f}" y="{y - 7:.1f}" fill="#d9f7e6" font-size="12">{anchor["number"]}</text>')
    svg.append('</g><g id="off-road-samples" fill="#ff4c78" stroke="#3a0716" stroke-width="1">')
    for point in offroad_points:
        x, y = xy(point)
        svg.append(f'<circle cx="{x:.1f}" cy="{y:.1f}" r="4"/>')
    svg.extend(['</g>', '<g font-family="sans-serif" fill="#eef3f5">',
                f'<text x="24" y="28" font-size="20" font-weight="bold">{map_name} — fitted BPD vs Ghost</text>',
                f'<text x="24" y="50" font-size="13">Ghost {metadata.get("best_time_ms", "?")} ms · clean={metadata.get("clean", "unknown")} · fit RMS {fit_rms:.1f} units · road-mesh coverage {road_coverage:.1f}%</text>',
                '<line x1="24" y1="78" x2="70" y2="78" stroke="#48a9ff" stroke-width="4"/><text x="78" y="83" font-size="13">Ghost</text>',
                '<line x1="160" y1="78" x2="206" y2="78" stroke="#ffb52e" stroke-width="4"/><text x="214" y="83" font-size="13">BPD fit</text>',
                '<circle cx="320" cy="78" r="5" fill="#65df9a"/><text x="332" y="83" font-size="13">Gate anchor</text>',
                '<circle cx="458" cy="78" r="4" fill="#ff4c78"/><text x="470" y="83" font-size="13">BPD sample with no road mesh under it</text>',
                '</g>', '</svg>'])
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text('\n'.join(svg) + '\n', encoding='utf-8')


def parse_ghost(path):
    metadata = {}
    samples = []
    for line_number, line in enumerate(path.read_text(encoding='ascii').splitlines(), 1):
        stripped = line.strip()
        if not stripped or stripped.startswith('#'):
            continue
        parts = stripped.split()
        if len(parts) >= 2 and not parts[0][0].isdigit():
            metadata[parts[0]] = ' '.join(parts[1:])
            continue
        try:
            time_ms = int(parts[0])
            point = tuple(float(value) for value in parts[1:4])
        except (ValueError, IndexError) as error:
            raise ValueError(f'{path}:{line_number}: malformed Ghost sample') from error
        if len(point) != 3 or not all(math.isfinite(value) for value in point):
            raise ValueError(f'{path}:{line_number}: invalid Ghost coordinates')
        if samples and time_ms <= samples[-1][0]:
            if time_ms == samples[-1][0] and math.dist(point, samples[-1][1]) < 0.5:
                continue
            raise ValueError(f'{path}:{line_number}: Ghost times must increase')
        samples.append((time_ms, point))
    if len(samples) < 4:
        raise ValueError('Ghost contains fewer than four usable samples')
    return metadata, samples


def brush_interval(a, b, gate, margin=0.0):
    """Return the [enter, leave] fraction of a center-point segment in a BSP brush."""
    enter, leave = 0.0, 1.0
    origin = gate['origin']
    for normal, distance in gate['planes']:
        da = sum(normal[k] * (a[k] - origin[k]) for k in range(3)) - distance - margin
        db = sum(normal[k] * (b[k] - origin[k]) for k in range(3)) - distance - margin
        if da > 0.0 and db > 0.0:
            return None
        if da <= 0.0 and db <= 0.0:
            continue
        fraction = da / (da - db)
        if da > 0.0:
            enter = max(enter, fraction)
        else:
            leave = min(leave, fraction)
        if enter > leave:
            return None
    return enter, leave


def find_crossing(samples, gate, start_time=0, margin=24.0):
    for index in range(len(samples) - 1):
        t0, a = samples[index]
        t1, b = samples[index + 1]
        if t1 <= start_time:
            continue
        interval = brush_interval(a, b, gate, margin)
        if interval is None:
            continue
        enter, leave = interval
        fraction = (enter + leave) * 0.5
        time_ms = t0 + (t1 - t0) * fraction
        if time_ms <= start_time:
            continue
        point = tuple(a[k] + (b[k] - a[k]) * fraction for k in range(3))
        return time_ms, point, index
    return None


def interpolate_samples(samples, crossing):
    time_ms, point, index = crossing
    result = [(time_ms, point)]
    for time, sample in samples[index + 1:]:
        if time >= time_ms:
            break
        result.append((time, sample))
    result.append((time_ms, point))
    result.sort(key=lambda item: item[0])
    deduped = []
    for item in result:
        if deduped and item[0] <= deduped[-1][0]:
            continue
        deduped.append(item)
    return deduped


def solve_linear(matrix, rhs):
    n = len(rhs)
    augmented = [matrix[row][:] + [rhs[row]] for row in range(n)]
    for column in range(n):
        pivot = max(range(column, n), key=lambda row: abs(augmented[row][column]))
        if abs(augmented[pivot][column]) < 1e-12:
            raise ValueError('handle fit is singular; Ghost does not cover the full route')
        augmented[column], augmented[pivot] = augmented[pivot], augmented[column]
        scale = augmented[column][column]
        for j in range(column, n + 1):
            augmented[column][j] /= scale
        for row in range(n):
            if row == column:
                continue
            factor = augmented[row][column]
            if factor == 0.0:
                continue
            for j in range(column, n + 1):
                augmented[row][j] -= factor * augmented[column][j]
    return [augmented[row][n] for row in range(n)]


def bezier_point(p0, p1, h0, h1, t):
    u = 1.0 - t
    b0, b1, b2, b3 = u**3, 3.0*u*u*t, 3.0*u*t*t, t**3
    return tuple((b0 + b1) * p0[k] + (b2 + b3) * p1[k] + b1 * h0[k] - b2 * h1[k]
                 for k in range(3))


def fit_handles(anchors, segments, regularization, max_handle):
    count = len(anchors)
    matrix = [[0.0] * count for _ in range(count)]
    rhs = [[0.0] * count for _ in range(3)]
    parameterized = []
    for segment_index, points in enumerate(segments):
        distances = [0.0]
        for previous, current in zip(points, points[1:]):
            distances.append(distances[-1] + math.dist(previous, current))
        total = distances[-1]
        if total < 1.0:
            raise ValueError(f'Ghost has no usable distance between gates {segment_index} and {(segment_index + 1) % count}')
        next_index = (segment_index + 1) % count
        p0, p1 = anchors[segment_index]['point'], anchors[next_index]['point']
        observations = []
        for point, distance in zip(points, distances):
            t = distance / total
            u = 1.0 - t
            c0, c1 = 3.0*u*u*t, -3.0*u*t*t
            fixed0, fixed1 = u**3 + c0, 3.0*u*t*t + t**3
            fixed = tuple(fixed0*p0[k] + fixed1*p1[k] for k in range(3))
            error = tuple(point[k] - fixed[k] for k in range(3))
            terms = ((segment_index, c0), (next_index, c1))
            for ia, ca in terms:
                for ib, cb in terms:
                    matrix[ia][ib] += ca * cb
                for axis in range(3):
                    rhs[axis][ia] += ca * error[axis]
            observations.append((point, t))
        parameterized.append((segment_index, observations))
    for index in range(count):
        matrix[index][index] += regularization
    handles = [tuple(solve_linear(matrix, rhs[axis])[i] for axis in range(3))
               for i in range(count)]
    limited = []
    for handle in handles:
        magnitude = math.sqrt(sum(value * value for value in handle))
        if magnitude > max_handle:
            handle = tuple(value * max_handle / magnitude for value in handle)
        limited.append(handle)
    squared_error, sample_count, max_error = 0.0, 0, 0.0
    for segment_index, observations in parameterized:
        next_index = (segment_index + 1) % count
        for point, t in observations:
            predicted = bezier_point(anchors[segment_index]['point'], anchors[next_index]['point'],
                                     limited[segment_index], limited[next_index], t)
            error = math.dist(point, predicted)
            squared_error += error * error
            sample_count += 1
            max_error = max(max_error, error)
    rms = math.sqrt(squared_error / max(1, sample_count))
    return limited, rms, max_error, sample_count


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('bsp', type=Path, help='compiled map BSP')
    parser.add_argument('ghost', type=Path, help='recorded Ghost for the same map/variant')
    parser.add_argument('output', type=Path, help='destination .bpd file')
    parser.add_argument('--regularization', type=float, default=0.001,
                        help='least-squares handle regularization (default: 0.001)')
    parser.add_argument('--max-handle', type=float, default=900.0,
                        help='maximum handle magnitude in game units')
    parser.add_argument('--vehicle-margin', type=float, default=24.0,
                        help='expand checkpoint brushes for the vehicle touch extent (default: 24 units)')
    parser.add_argument('--preview-svg', type=Path,
                        help='write a route and visible-road-surface preview SVG (default: alongside the BPD)')
    parser.add_argument('--road-height-tolerance', type=float, default=96.0,
                        help='vertical tolerance for the approximate road-mesh coverage check (default: 96 units)')
    parser.add_argument('--force', action='store_true', help='overwrite output if it exists')
    args = parser.parse_args()
    if args.output.exists() and not args.force:
        parser.error(f'{args.output} already exists; pass --force to replace it')
    if (args.regularization < 0.0 or args.max_handle <= 0.0 or args.vehicle_margin < 0.0
            or args.road_height_tolerance < 0.0):
        parser.error('regularization, vehicle-margin and road-height-tolerance must be non-negative; max-handle must be positive')
    bsp_map = args.bsp.stem.lower()
    preview_path = args.preview_svg or args.output.with_suffix('.svg')
    if preview_path.exists() and not args.force:
        parser.error(f'{preview_path} already exists; pass --force to replace it')
    lumps = read_lumps(args.bsp.read_bytes())
    start_gate, checkpoint_gates = parse_bsp_gates(lumps)
    metadata, raw_samples = parse_ghost(args.ghost)
    if metadata.get('map', '').lower() != bsp_map:
        parser.error(f"Ghost map {metadata.get('map')!r} does not match BSP {bsp_map!r}")
    if metadata.get('track_length') not in (None, '1') or metadata.get('track_reversed') not in (None, '0'):
        parser.error('prototype currently supports normal medium-length lap tracks only')

    start_crossing = find_crossing(raw_samples, start_gate, 0.0, args.vehicle_margin)
    if start_crossing is None:
        raise ValueError('Ghost does not cross the start/finish brush before checkpoint 1')
    anchors = [{'number': len(checkpoint_gates) + 1, 'gate': start_gate,
                'point': start_crossing[1], 'time': start_crossing[0], 'label': 'start/finish'}]
    crossings = []
    last_time = 0.0
    for number, gate in enumerate(checkpoint_gates, 1):
        crossing = find_crossing(raw_samples, gate, last_time, args.vehicle_margin)
        if crossing is None:
            raise ValueError(f'Ghost does not cross checkpoint {number} after {last_time:.0f} ms')
        crossings.append(crossing)
        last_time = crossing[0]
        anchors.append({'number': number, 'gate': gate, 'point': crossing[1],
                        'time': crossing[0], 'label': f'checkpoint {number}'})
    finish_crossing = find_crossing(raw_samples, start_gate, last_time, args.vehicle_margin)
    if finish_crossing is None:
        raise ValueError('Ghost does not cross the start/finish brush after the last checkpoint')
    # The Ghost starts on the grid, which can be outside the start/finish gate.
    # Average the first and final gate crossings so the closed BPD loop remains
    # inside the actual trigger and passes the runtime gate-order validator.
    sf_point = tuple((start_crossing[1][axis] + finish_crossing[1][axis]) * 0.5 for axis in range(3))
    anchors[0]['point'] = sf_point
    anchors[0]['time'] = start_crossing[0]
    anchors.append({'number': len(checkpoint_gates) + 1, 'gate': start_gate,
                    'point': sf_point, 'time': finish_crossing[0], 'label': 'finish'})

    # Anchors are ordered [start/finish, CP1..CPn, finish]. Fit one loop with a
    # shared start/finish handle; combine the two observations for that node.
    route_anchors = anchors[:-1]
    segments = []
    bounds = [start_crossing[0]] + [crossing[0] for crossing in crossings] + [finish_crossing[0]]
    points_by_time = raw_samples
    for index in range(len(route_anchors)):
        start_time, end_time = bounds[index], bounds[index + 1]
        sequence = [route_anchors[index]['point']]
        sequence.extend(point for time_ms, point in points_by_time if start_time < time_ms < end_time)
        sequence.append(route_anchors[(index + 1) % len(route_anchors)]['point'])
        compact = [sequence[0]]
        compact.extend(point for point in sequence[1:] if math.dist(point, compact[-1]) > 0.01)
        segments.append(compact)

    handles, rms, max_error, sample_count = fit_handles(
        route_anchors, segments, args.regularization, args.max_handle)
    road_triangles, road_surface_count = parse_road_triangles(lumps)
    if not road_triangles:
        raise ValueError('BSP contains no visible surfaces with a road/asphalt material name')
    route_points = sample_bezier_route(route_anchors, handles)
    covered = [point_on_road_mesh(point, road_triangles, args.road_height_tolerance)
               for point in route_points]
    covered_points = [point for point, is_covered in zip(route_points, covered) if is_covered]
    offroad_points = [point for point, is_covered in zip(route_points, covered) if not is_covered]
    road_coverage = 100.0 * len(covered_points) / max(1, len(route_points))
    args.output.parent.mkdir(parents=True, exist_ok=True)
    records = sorted(zip(route_anchors, handles), key=lambda item: item[0]['number'])
    with args.output.open('w', encoding='ascii', newline='\n') as stream:
        for anchor, handle in records:
            point = anchor['point']
            stream.write(f"{anchor['number']} : {point[0]:.3f} {point[1]:.3f} {point[2]:.3f} : "
                         f"{handle[0]:.3f} {handle[1]:.3f} {handle[2]:.3f}\n")
    print(f'Wrote {len(records)} Ghost-fitted anchors to {args.output}')
    print(f'Gate crossings: start 1/1, checkpoints {len(crossings)}/{len(checkpoint_gates)}, finish 1/1; '
          f'Ghost clean={metadata.get("clean", "unknown")}; best_time_ms={metadata.get("best_time_ms", "unknown")}')
    print(f'Start-line sample: t={start_crossing[0]:.0f} ms at '
          f'({start_crossing[1][0]:.1f}, {start_crossing[1][1]:.1f}, {start_crossing[1][2]:.1f}); '
          f'finish: t={finish_crossing[0]:.0f} ms')
    print(f'Fit samples={sample_count}, RMS={rms:.1f}, max={max_error:.1f} game units; '
          f'vehicle gate margin={args.vehicle_margin:.1f}')
    print(f'Visible road mesh: {road_surface_count} surfaces, {len(road_triangles)} triangles; '
          f'route coverage={road_coverage:.1f}% ({len(covered_points)}/{len(route_points)} samples, '
          f'height tolerance={args.road_height_tolerance:.1f})')
    for point in offroad_points[:12]:
        print(f'  no road mesh under route sample: ({point[0]:.1f}, {point[1]:.1f}, {point[2]:.1f})')
    if len(offroad_points) > 12:
        print(f'  ... and {len(offroad_points) - 12} more; see preview')
    write_route_preview_svg(preview_path, bsp_map, road_triangles, raw_samples,
                            route_points, route_anchors, offroad_points, metadata, rms,
                            road_coverage)
    print(f'Wrote route preview to {preview_path}')
    for crossing, anchor in zip(crossings, route_anchors[1:]):
        print(f"{anchor['label']}: t={crossing[0]:.0f} ms, anchor="
              f"({anchor['point'][0]:.1f}, {anchor['point'][1]:.1f}, {anchor['point'][2]:.1f})")


if __name__ == '__main__':
    main()
