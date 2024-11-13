#include "camera/camera_math.h"
#include "camera/camera_modes.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Used in sModeTransitions for CLOSE and FREE_ROAM mode
 */
s32 update_mario_camera(UNUSED struct Camera *c, Vec3f focus, Vec3f pos) {
    s16 yaw = sMarioCamState->faceAngle[1] + sModeOffsetYaw + DEGREES(180);
    focus_on_mario(focus, pos, 125.f, 125.f, gCameraZoomDist, 0x05B0, yaw);

    return sMarioCamState->faceAngle[1];
}

/**
 * When no other mode is active and the current R button mode is Mario
 */
void mode_mario_camera(struct Camera *c) {
    gCameraZoomDist = 350.f;
    mode_default_camera(c);
}
