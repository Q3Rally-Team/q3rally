extern int cg_thirdPersonHeight;
extern int cg_thirdPersonRange;

void cg_cameraposition(void) {

    switch(ui_cameraposition.integer) {

        case 0:

            // Default settings
            cg_thirdPersonHeight = 170;
            cg_thirdPersonRange = 60;

            break;

        case 1:

            cg_thirdPersonHeight = 30;
            cg_thirdPersonRange = 0;

            break;

        case 2:

            // Set the values for mode 2

            break;

        case 3:

            // Set the values for mode 3

            break;

        default:

            // Handle invalid mode

            break;

    }

}