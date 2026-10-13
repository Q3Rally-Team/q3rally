"""Paint atlas layout shared by the model and texture builders."""
# ---------------- atlas layout (512x512 paint atlas, units -> pixels) ----------
A = 512.0; SC = 3.9
X0, X1 = -56.0, 56.0      # length
Y0, Y1 = -22.5, 22.5      # width
Z0, Z1 = -12.0, 15.0      # height
LX = 36                   # left margin of the long regions
R_LSIDE = (LX, 4)
R_RSIDE = (LX, 4 + (Z1-Z0)*SC + 6)
R_TOP   = (LX, R_RSIDE[1] + (Z1-Z0)*SC + 6)
R_FRONT = (4, R_TOP[1] + (Y1-Y0)*SC + 6)
R_BACK  = (4 + (Y1-Y0)*SC + 10, R_FRONT[1])
R_UNDER = (A-40, A-40)    # small dark patch for downward faces
def atlas(region, p):
    x, y, z = p
    if region == 'L':  u, v = R_LSIDE[0] + (X1-x)*SC, R_LSIDE[1] + (Z1-z)*SC
    elif region == 'R': u, v = R_RSIDE[0] + (x-X0)*SC, R_RSIDE[1] + (Z1-z)*SC
    elif region == 'T': u, v = R_TOP[0] + (x-X0)*SC,  R_TOP[1] + (Y1-y)*SC
    elif region == 'F': u, v = R_FRONT[0] + (y-Y0)*SC, R_FRONT[1] + (Z1-z)*SC
    elif region == 'B': u, v = R_BACK[0] + (Y1-y)*SC,  R_BACK[1] + (Z1-z)*SC
    else:               u, v = R_UNDER[0] + 16 + (x/X1)*12, R_UNDER[1] + 16 + (y/Y1)*12
    return [u/A, v/A]
