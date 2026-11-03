#!/usr/bin/env python3
"""Create a diagnostic-only .bpd candidate from checkpoint respawn origins.

Q3Rally's checkpoint `origin` is the respawn point, not its Bezier route anchor.
The generated path is therefore unsuitable as a production racing line. Use it
only to probe the effect of guessed handles; a proper fitter must use gate
crossings from the checkpoint brushes or a reference Ghost.
"""
import argparse
from pathlib import Path
import re
import struct


LUMP_COUNT = 17
LUMP_ENTITIES = 0
LUMP_MODELS = 7


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


def vector(text, context):
    try:
        values = tuple(float(part) for part in text.split())
    except (AttributeError, ValueError) as error:
        raise ValueError(f'{context}: invalid vector {text!r}') from error
    if len(values) != 3:
        raise ValueError(f'{context}: expected three coordinates')
    return values


def subtract(a, b):
    return tuple(x - y for x, y in zip(a, b))


def length(value):
    return sum(component * component for component in value) ** 0.5


def scale(value, factor):
    return tuple(component * factor for component in value)


def normalize(value):
    magnitude = length(value)
    if magnitude < 1e-5:
        return None
    return scale(value, 1.0 / magnitude)


def add(a, b):
    return tuple(x + y for x, y in zip(a, b))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('bsp', type=Path, help='compiled map BSP')
    parser.add_argument('output', type=Path, help='destination .bpd file')
    parser.add_argument('--handle-scale', type=float, default=1.0 / 3.0,
                        help='handle length as a fraction of the shorter adjacent gate spacing')
    parser.add_argument('--max-handle', type=float, default=900.0,
                        help='maximum handle length in game units')
    parser.add_argument('--force', action='store_true', help='overwrite output if it exists')
    args = parser.parse_args()
    if not 0.05 <= args.handle_scale <= 0.5 or args.max_handle <= 0:
        parser.error('handle scale must be in [0.05, 0.5] and max handle must be positive')
    if args.output.exists() and not args.force:
        parser.error(f'{args.output} already exists; pass --force to replace it')

    lumps = read_lumps(args.bsp.read_bytes())
    entities = parse_entities(lumps[LUMP_ENTITIES])
    checkpoints = {}
    startfinish = []
    for entity in entities:
        classname = entity.get('classname', '').lower()
        if classname == 'rally_checkpoint':
            try:
                number = int(entity['number'])
            except (KeyError, ValueError) as error:
                raise ValueError('rally_checkpoint is missing a valid number') from error
            if number in checkpoints:
                raise ValueError(f'duplicate checkpoint number {number}')
            checkpoints[number] = vector(entity.get('origin'), f'checkpoint {number}')
        elif classname == 'rally_startfinish':
            startfinish.append(vector(entity.get('origin'), 'start/finish'))

    if len(startfinish) != 1:
        raise ValueError(f'expected one rally_startfinish entity, found {len(startfinish)}')
    count = len(checkpoints)
    if set(checkpoints) != set(range(1, count + 1)):
        raise ValueError('checkpoint numbering must be contiguous from 1')

    # A lap route starts just after the start/finish gate, then visits checkpoints 1..N.
    # Runtime numbers the start/finish entity N+1 after it converts it to a checkpoint.
    ordered = [(count + 1, startfinish[0])] + [
        (number, checkpoints[number]) for number in range(1, count + 1)
    ]
    anchors = [point for _, point in ordered]
    records = []
    handle_lengths = []
    for index, (number, point) in enumerate(ordered):
        previous = anchors[(index - 1) % len(anchors)]
        following = anchors[(index + 1) % len(anchors)]
        incoming = normalize(subtract(point, previous))
        outgoing = normalize(subtract(following, point))
        if incoming is None or outgoing is None:
            raise ValueError(f'checkpoint route has duplicate adjacent anchors near {number}')
        tangent = normalize(add(incoming, outgoing)) or outgoing
        adjacent = min(length(subtract(point, previous)), length(subtract(following, point)))
        handle_length = min(adjacent * args.handle_scale, args.max_handle)
        handle = scale(tangent, handle_length)
        records.append((number, point, handle))
        handle_lengths.append(handle_length)

    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open('w', encoding='ascii', newline='\n') as stream:
        for number, point, handle in sorted(records):
            stream.write(f'{number} : {point[0]:.3f} {point[1]:.3f} {point[2]:.3f} : '
                         f'{handle[0]:.3f} {handle[1]:.3f} {handle[2]:.3f}\n')
    print(f'Wrote {len(records)} diagnostic Bezier anchors to {args.output}')
    print('WARNING: BSP checkpoint origins are respawn points, not Bezier route anchors.')
    print(f'Handle lengths: min={min(handle_lengths):.1f}, '
          f'max={max(handle_lengths):.1f}, mean={sum(handle_lengths) / len(handle_lengths):.1f}')


if __name__ == '__main__':
    main()
