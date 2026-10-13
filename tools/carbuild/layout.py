"""Paint atlas layout shared by the model and texture builders.

The defaults fit the Mustang and the Kuebelwagen. A bigger car calls
configure() before importing paintlib (which copies these names)."""
# ---------------- atlas layout (512x512 paint atlas, units -> pixels) ----------
def configure(x0=-56.0, x1=56.0, y0=-22.5, y1=22.5, z0=-12.0, z1=15.0, sc=3.9, lx=36):
    global A, SC, X0, X1, Y0, Y1, Z0, Z1, LX, R_LSIDE, R_RSIDE, R_TOP, R_FRONT, R_BACK, R_UNDER
    A = 512.0; SC = sc
    X0, X1 = x0, x1           # length
    Y0, Y1 = y0, y1           # width
    Z0, Z1 = z0, z1           # height
    LX = lx                   # left margin of the long regions
    R_LSIDE = (LX, 4)
    R_RSIDE = (LX, 4 + (Z1-Z0)*SC + 6)
    R_TOP   = (LX, R_RSIDE[1] + (Z1-Z0)*SC + 6)
    R_FRONT = (4, R_TOP[1] + (Y1-Y0)*SC + 6)
    R_BACK  = (4 + (Y1-Y0)*SC + 10, R_FRONT[1])
    R_UNDER = (A-40, A-40)    # small dark patch for downward faces
configure()
def atlas(region, p):
    x, y, z = p
    if region == 'L':  u, v = R_LSIDE[0] + (X1-x)*SC, R_LSIDE[1] + (Z1-z)*SC
    elif region == 'R': u, v = R_RSIDE[0] + (x-X0)*SC, R_RSIDE[1] + (Z1-z)*SC
    elif region == 'T': u, v = R_TOP[0] + (x-X0)*SC,  R_TOP[1] + (Y1-y)*SC
    elif region == 'F': u, v = R_FRONT[0] + (y-Y0)*SC, R_FRONT[1] + (Z1-z)*SC
    elif region == 'B': u, v = R_BACK[0] + (Y1-y)*SC,  R_BACK[1] + (Z1-z)*SC
    else:               u, v = R_UNDER[0] + 16 + (x/X1)*12, R_UNDER[1] + 16 + (y/Y1)*12
    return [u/A, v/A]
