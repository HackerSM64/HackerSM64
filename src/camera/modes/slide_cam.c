#include "camera/camera_math.h"
#include "camera/camera_modes.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game/game_init.h"
#include "game/camera.h"

/**
 * Update the camera in slide and hoot mode.
 *
 * In slide mode, keep the camera 800 units from Mario
 */
s16 update_slide_camera(struct Camera *c) {
    struct Surface *floor;
    f32 floorHeight;
    Vec3f pos;
    f32 distCamToFocus;
    f32 maxCamDist;
    f32 pitchScale;
    s16 camPitch;
    s16 camYaw;
    s16 goalPitch = 0x1555;
    s16 goalYaw = sMarioCamState->faceAngle[1] + DEGREES(180);

    // Zoom in when inside the CCM shortcut
    if (sStatusFlags & CAM_FLAG_CCM_SLIDE_SHORTCUT) {
        sLakituDist = approach_f32(sLakituDist, -600.f, 20.f, 20.f);
    } else {
        sLakituDist = approach_f32(sLakituDist, 0.f, 20.f, 20.f);
    }

    // No C-Button input in this mode, notify the player with a buzzer
    play_camera_buzz_if_cbutton();

    // Focus on Mario
    vec3f_copy(c->focus, sMarioCamState->pos);
    c->focus[1] += 50.f;

    vec3f_get_dist_and_angle(c->focus, c->pos, &distCamToFocus, &camPitch, &camYaw);
    maxCamDist = 800.f;

    // In hoot mode, zoom further out and rotate faster
    if (sMarioCamState->action == ACT_RIDING_HOOT) {
        maxCamDist = 1000.f;
        goalPitch = 0x2800;
        camera_approach_s16_symmetric_bool(&camYaw, goalYaw, 0x100);
    } else {
        camera_approach_s16_symmetric_bool(&camYaw, goalYaw, 0x80);
    }
    camera_approach_s16_symmetric_bool(&camPitch, goalPitch, 0x100);

    // Hoot mode
    if (sMarioCamState->action != ACT_RIDING_HOOT && sMarioGeometry.currFloorType == SURFACE_DEATH_PLANE) {
        vec3f_set_dist_and_angle(c->focus, pos, maxCamDist + sLakituDist, camPitch, camYaw);
        c->pos[0] = pos[0];
        c->pos[2] = pos[2];
        camera_approach_f32_symmetric_bool(&c->pos[1], c->focus[1], 30.f);
        vec3f_get_dist_and_angle(c->pos, c->focus, &distCamToFocus, &camPitch, &camYaw);
        pitchScale = (distCamToFocus - maxCamDist + sLakituDist) / 10000.f;
        if (pitchScale > 1.f) {
            pitchScale = 1.f;
        }
        camPitch += 0x1000 * pitchScale;
        vec3f_set_dist_and_angle(c->pos, c->focus, distCamToFocus, camPitch, camYaw);

    // Slide mode
    } else {
        vec3f_set_dist_and_angle(c->focus, c->pos, maxCamDist + sLakituDist, camPitch, camYaw);
        sStatusFlags |= CAM_FLAG_BLOCK_SMOOTH_MOVEMENT;

        // Stay above the slide floor
        floorHeight = find_floor(c->pos[0], c->pos[1] + 200.f, c->pos[2], &floor) + 125.f;
        if (c->pos[1] < floorHeight) {
            c->pos[1] = floorHeight;
        }
        // Stay closer than maxCamDist
        vec3f_get_dist_and_angle(c->focus, c->pos, &distCamToFocus, &camPitch, &camYaw);
        if (distCamToFocus > maxCamDist + sLakituDist) {
            distCamToFocus = maxCamDist + sLakituDist;
            vec3f_set_dist_and_angle(c->focus, c->pos, distCamToFocus, camPitch, camYaw);
        }
    }

    camYaw = calculate_yaw(c->focus, c->pos);
    return camYaw;
}

/**
 * Slide/hoot mode.
 * In this mode, the camera is always at the back of Mario, because Mario generally only moves forward.
 */
void mode_slide_camera(struct Camera *c) {
    if (sMarioGeometry.currFloorType == SURFACE_CLOSE_CAMERA ||
        sMarioGeometry.currFloorType == SURFACE_NO_CAM_COL_SLIPPERY) {
        mode_lakitu_camera(c);
    } else {
        if (gPlayer1Controller->buttonPressed & U_CBUTTONS) {
            gCameraMovementFlags |= CAM_MOVE_C_UP_MODE;
        }
        c->nextYaw = update_slide_camera(c);
    }
}
