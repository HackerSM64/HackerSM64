#include "camera/camera_math.h"
#include "camera/camera_modes.h"
#include "engine/math_util.h"
#include "game/camera.h"


/**
 * Some functions called in this mode are located in radial_cam.c
 */

/**
 * Updates the camera in outward radial mode.
 * sModeOffsetYaw is calculated in radial_camera_move, which calls offset_yaw_outward_radial
 */
s32 update_outward_radial_camera(struct Camera *c, Vec3f focus, Vec3f pos) {
    f32 xDistFocToMario = sMarioCamState->pos[0] - c->areaCenX;
    f32 zDistFocToMario = sMarioCamState->pos[2] - c->areaCenZ;
    s16 camYaw = atan2s(zDistFocToMario, xDistFocToMario) + sModeOffsetYaw + DEGREES(180);
    s16 pitch = look_down_slopes(camYaw);
    f32 baseDist = 1000.f;
    // A base offset of 125.f is ~= Mario's eye height
    f32 yOff = 125.f;
    f32 posY;
    f32 focusY;

    sAreaYaw = camYaw - sModeOffsetYaw - DEGREES(180);
    calc_y_to_curr_floor(&posY, 1.f, 200.f, &focusY, 0.9f, 200.f);
    focus_on_mario(focus, pos, posY + yOff, focusY + yOff, sLakituDist + baseDist, pitch, camYaw);

    return camYaw;
}

/**
 * Input and updates for the outward radial mode.
 */
void mode_outward_radial_camera(struct Camera *c) {
    Vec3f pos;
    s16 oldAreaYaw = sAreaYaw;

    if (gCameraMovementFlags & CAM_MOVING_INTO_MODE) {
        update_yaw_and_dist_from_c_up(c);
    }
    radial_camera_input_default(c);
    radial_camera_move(c);
    lakitu_zoom(400.f, 0x900);
    c->nextYaw = update_outward_radial_camera(c, c->focus, pos);
    c->pos[0] = pos[0];
    c->pos[2] = pos[2];
    sAreaYawChange = sAreaYaw - oldAreaYaw;
    if (sMarioCamState->action == ACT_RIDING_HOOT) {
        pos[1] += 500.f;
    }
    set_camera_height(c, pos[1]);
    pan_ahead_of_player(c);
}
