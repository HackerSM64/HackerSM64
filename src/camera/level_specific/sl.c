#include "game/camera.h"

/**
 * Sets 8 directional mode and blocks the next trigger from processing.
 * Activated when Mario is walking in front of the snowman's head.
 */
void cam_sl_snowman_head_8dir(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_BLOCK_AREA_PROCESSING;
    transition_to_camera_mode(c, CAMERA_MODE_8_DIRECTIONS, 60);
    s8DirModeBaseYaw = 0x1D27;
}

/**
 * Sets free roam mode in SL, called by a trigger that covers a large area and surrounds the 8 direction
 * trigger.
 */
void cam_sl_free_roam(struct Camera *c) {
    transition_to_camera_mode(c, CAMERA_MODE_FREE_ROAM, 60);
}
