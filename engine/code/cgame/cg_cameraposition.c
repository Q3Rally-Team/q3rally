#include "cg_local.h"

void CG_CameraPosition(void) {
    switch(cg_cameraposition.integer) {
        case 0:
            trap_Cvar_Set("cg_thirdPersonHeight", "60");
            trap_Cvar_Set("cg_thirdPersonRange", "170");
            break;
        case 1:
            trap_Cvar_Set("cg_thirdPersonHeight", "30");
            trap_Cvar_Set("cg_thirdPersonRange", "0");
            break;
        case 2:
            trap_Cvar_Set("cg_thirdPersonHeight", "30");
            trap_Cvar_Set("cg_thirdPersonRange", "110");
            break;
        case 3:
            trap_Cvar_Set("cg_thirdPersonHeight", "500");
            trap_Cvar_Set("cg_thirdPersonRange", "500");
            break;
        default:
            trap_Cvar_Set("cg_thirdPersonHeight", "170");
            trap_Cvar_Set("cg_thirdPersonRange", "60");
            break;
    }
}
