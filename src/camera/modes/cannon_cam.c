#include "camera/camera_math.h"
#include "camera/camera_modes.h"
#include "engine/math_util.h"
#include "game/game_init.h"
#include "game/camera.h"


/**
 * Used when Mario is in a cannon.
 */
s32 update_in_cannon(UNUSED struct Camera *c, Vec3f focus, Vec3f pos) {
    focus_on_mario(pos, focus, 125.f + sCannonYOffset, 125.f, 800.f,
                                    sMarioCamState->faceAngle[0], sMarioCamState->faceAngle[1]);
    return sMarioCamState->faceAngle[1];
}

/**
 * Updates the camera when Mario is in a cannon.
 * sCannonYOffset is used to make the camera rotate down when Mario has just entered the cannon
 */
void mode_cannon_camera(struct Camera *c) {
    sLakituPitch = 0;
    gCameraMovementFlags &= ~CAM_MOVING_INTO_MODE;
    c->nextYaw = update_in_cannon(c, c->focus, c->pos);
    if (gPlayer1Controller->buttonPressed & A_BUTTON) {
        set_camera_mode(c, CAMERA_MODE_BEHIND_MARIO, 1);
        sPanDistance = 0.f;
        sCannonYOffset = 0.f;
        sStatusFlags &= ~CAM_FLAG_BLOCK_SMOOTH_MOVEMENT;
    } else {
        sCannonYOffset = approach_f32(sCannonYOffset, 0.f, 100.f, 100.f);
    }
}
