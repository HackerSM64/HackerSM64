#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Moves the camera to through the tunnel by forcing sModeOffsetYaw
 */
void cam_thi_move_cam_through_tunnel(UNUSED struct Camera *c) {
    if (sModeOffsetYaw < DEGREES(60)) {
        sModeOffsetYaw = DEGREES(60);
    }
}

/**
 * Aligns the camera to look through the tunnel
 */
void cam_thi_look_through_tunnel(UNUSED struct Camera *c) {
    // ~82.5 degrees
    if (sModeOffsetYaw > 0x3AAA) {
        sModeOffsetYaw = 0x3AAA;
    }
}
