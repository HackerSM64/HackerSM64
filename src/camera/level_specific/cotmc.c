#include "game/camera.h"

/**
 * Fix the camera in place as Mario gets exits out the MC cave into the waterfall.
 */
void cam_cotmc_exit_waterfall(UNUSED struct Camera *c) {
    gCameraMovementFlags |= CAM_MOVE_FIX_IN_PLACE;
}
