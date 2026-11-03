"""Paint atlas layout of the GT-R (1024x1024, world units -> pixels).

Shared by build_model.py (UVs) and gen_textures.py (painting). The body is
projected from five sides: left, right, top, front and back; faces pointing
down get a small dark patch.
"""
A = 1024.0
SC = 6.8                  # pixels per world unit
X0, X1 = -57.5, 57.5      # length
Y0, Y1 = -25.0, 25.0      # width
Z0, Z1 = -16.0, 15.5      # height
LX = 40                   # left margin of the long regions
R_LSIDE = (LX, 6)
R_RSIDE = (LX, R_LSIDE[1] + (Z1 - Z0) * SC + 8)
R_TOP = (LX, R_RSIDE[1] + (Z1 - Z0) * SC + 8)
R_FRONT = (6, R_TOP[1] + (Y1 - Y0) * SC + 8)
R_BACK = (6 + (Y1 - Y0) * SC + 16, R_FRONT[1])
R_UNDER = (A - 72, A - 72)


def atlas(region, p):
    x, y, z = p
    if region == 'L':
        u, v = R_LSIDE[0] + (X1 - x) * SC, R_LSIDE[1] + (Z1 - z) * SC
    elif region == 'R':
        u, v = R_RSIDE[0] + (x - X0) * SC, R_RSIDE[1] + (Z1 - z) * SC
    elif region == 'T':
        u, v = R_TOP[0] + (x - X0) * SC, R_TOP[1] + (Y1 - y) * SC
    elif region == 'F':
        u, v = R_FRONT[0] + (y - Y0) * SC, R_FRONT[1] + (Z1 - z) * SC
    elif region == 'B':
        u, v = R_BACK[0] + (Y1 - y) * SC, R_BACK[1] + (Z1 - z) * SC
    else:
        u, v = R_UNDER[0] + 32 + (x / X1) * 24, R_UNDER[1] + 32 + (y / Y1) * 24
    return [u / A, v / A]


def region(n):
    """paint region of a face from its normal"""
    nx, ny, nz = n
    ax = max(abs(nx), abs(ny), abs(nz))
    if nz < -0.55:
        return 'U'
    if ax == abs(nz):
        return 'T'
    if ax == abs(ny):
        return 'L' if ny > 0 else 'R'
    return 'F' if nx > 0 else 'B'
