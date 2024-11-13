#include "game/camera.h"


/**
 * Unused. Changes the camera to radial mode when Mario is on the tower.
 *
 * @see sCamBOB for bounds.
 */
void cam_bob_tower(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_BLOCK_AREA_PROCESSING;
    transition_to_camera_mode(c, CAMERA_MODE_RADIAL, 90);
}

/**
 * Unused. Changes the camera to free roam mode when Mario is not climbing the tower.
 *
 * This is the only CameraTrigger event that uses the area == -1 feature:
 * If this was used, it would be called by default in BoB.
 *
 * @see sCamBOB
 */
void cam_bob_default_free_roam(struct Camera *c) {
    transition_to_camera_mode(c, CAMERA_MODE_FREE_ROAM, 90);
}
