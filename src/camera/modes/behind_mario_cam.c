#include "camera/camera_math.h"
#include "camera/camera_modes.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/camera.h"

/**
 * Move `pos` between the nearest floor and ceiling
 * REVIEWERS: is this a better fit for camera_math? it's only used here
 */
static void set_floor_ceiling_midpoint(Vec3f pos) {
    struct Surface *surf;

    f32_find_wall_collision(&pos[0], &pos[1], &pos[2], 0.f, 100.f);
    f32 floorY = find_floor(pos[0], pos[1] + 50.f, pos[2], &surf);
    f32 ceilY = find_ceil(pos[0], pos[1] - 50.f, pos[2], &surf);

    if ((FLOOR_LOWER_LIMIT != floorY) && (CELL_HEIGHT_LIMIT == ceilY)) {
        if (pos[1] < (floorY += 125.f)) {
            pos[1] = floorY;
        }
    }

    if ((FLOOR_LOWER_LIMIT == floorY) && (CELL_HEIGHT_LIMIT != ceilY)) {
        if (pos[1] > (ceilY -= 125.f)) {
            pos[1] = ceilY;
        }
    }

    if ((FLOOR_LOWER_LIMIT != floorY) && (CELL_HEIGHT_LIMIT != ceilY)) {
        floorY += 125.f;
        ceilY -= 125.f;

        if ((pos[1] <= floorY) && (pos[1] < ceilY)) {
            pos[1] = floorY;
        }
        if ((pos[1] > floorY) && (pos[1] >= ceilY)) {
            pos[1] = ceilY;
        }
        if ((pos[1] <= floorY) && (pos[1] >= ceilY)) {
            pos[1] = (floorY + ceilY) * 0.5f;
        }
    }
}

/**
 * Updates the camera in BEHIND_MARIO mode.
 *
 * The C-Buttons rotate the camera 90 degrees left/right and 67.5 degrees up/down.
 */
s32 update_behind_mario_camera(struct Camera *c, Vec3f focus, Vec3f pos) {
    f32 dist;
    s16 absPitch;
    s16 pitch;
    s16 yaw;
    s16 goalPitch = -sMarioCamState->faceAngle[0];
    s16 marioYaw = sMarioCamState->faceAngle[1] + DEGREES(180);
    s16 goalYawOff = 0;
    s16 yawSpeed;
    s16 pitchInc = 32;
    f32 maxDist = 800.f;
    f32 focYOff = 125.f;

    // Zoom in when Mario R_TRIG mode is active
    if (sSelectionFlags & CAM_MODE_MARIO_ACTIVE) {
        maxDist = 350.f;
        focYOff = 120.f;
    }
    if (!(sMarioCamState->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER))) {
        pitchInc = 128;
    }

    // Focus on Mario
    vec3f_copy(focus, sMarioCamState->pos);
    c->focus[1] += focYOff;
    vec3f_get_dist_and_angle(focus, pos, &dist, &pitch, &yaw);
    if (dist > maxDist) {
        dist = maxDist;
    }
    if ((absPitch = pitch) < 0) {
        absPitch = -absPitch;
    }

    // Determine the yaw speed based on absPitch. A higher absPitch (further away from looking straight)
    // translates to a slower speed
    // Note: Pitch is always within +- 90 degrees or +-0x4000, and 0x4000 / 0x200 = 32
    yawSpeed = 32 - absPitch / 0x200;
    if (yawSpeed < 1) {
        yawSpeed = 1;
    }
    if (yawSpeed > 32) {
        yawSpeed = 32;
    }

    if (sCSideButtonYaw != 0) {
        camera_approach_s16_symmetric_bool(&sCSideButtonYaw, 0, 1);
        yawSpeed = 8;
    }
    if (sBehindMarioSoundTimer != 0) {
        goalPitch = 0;
        camera_approach_s16_symmetric_bool(&sBehindMarioSoundTimer, 0, 1);
        pitchInc = 0x800;
    }

    if (sBehindMarioSoundTimer == 28) {
        if (sCSideButtonYaw < 5 || sCSideButtonYaw > 28) {
            play_sound_cbutton_up();
        }
    }
    if (sCSideButtonYaw == 28) {
        if (sBehindMarioSoundTimer < 5 || sBehindMarioSoundTimer > 28) {
            play_sound_cbutton_up();
        }
    }

    // C-Button input. Note: Camera rotates in the opposite direction of the button (airplane controls)
    //! @bug C-Right and C-Up take precedence due to the way input is handled here

    // Rotate right
    if (sCButtonsPressed & L_CBUTTONS) {
        if (gPlayer1Controller->buttonPressed & L_CBUTTONS) {
            play_sound_cbutton_side();
        }
        if (dist < maxDist) {
            camera_approach_f32_symmetric_bool(&dist, maxDist, 5.f);
        }
        goalYawOff = -0x3FF8;
        sCSideButtonYaw = 30;
        yawSpeed = 2;
    }
    // Rotate left
    if (sCButtonsPressed & R_CBUTTONS) {
        if (gPlayer1Controller->buttonPressed & R_CBUTTONS) {
            play_sound_cbutton_side();
        }
        if (dist < maxDist) {
            camera_approach_f32_symmetric_bool(&dist, maxDist, 5.f);
        }
        goalYawOff = 0x3FF8;
        sCSideButtonYaw = 30;
        yawSpeed = 2;
    }
    // Rotate up
    if (sCButtonsPressed & D_CBUTTONS) {
        if (gPlayer1Controller->buttonPressed & (U_CBUTTONS | D_CBUTTONS)) {
            play_sound_cbutton_side();
        }
        if (dist < maxDist) {
            camera_approach_f32_symmetric_bool(&dist, maxDist, 5.f);
        }
        goalPitch = -0x3000;
        sBehindMarioSoundTimer = 30;
        pitchInc = 0x800;
    }
    // Rotate down
    if (sCButtonsPressed & U_CBUTTONS) {
        if (gPlayer1Controller->buttonPressed & (U_CBUTTONS | D_CBUTTONS)) {
            play_sound_cbutton_side();
        }
        if (dist < maxDist) {
            camera_approach_f32_symmetric_bool(&dist, maxDist, 5.f);
        }
        goalPitch = 0x3000;
        sBehindMarioSoundTimer = 30;
        pitchInc = 0x800;
    }

    approach_s16_asymptotic_bool(&yaw, marioYaw + goalYawOff, yawSpeed);
    camera_approach_s16_symmetric_bool(&pitch, goalPitch, pitchInc);
    if (dist < 300.f) {
        dist = 300.f;
    }
    vec3f_set_dist_and_angle(focus, pos, dist, pitch, yaw);
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    if (gCurrLevelArea == AREA_WDW_MAIN) {
        yaw = clamp_positions_and_find_yaw(pos, focus, 4508.f, -3739.f, 4508.f, -3739.f);
    }
    if (gCurrLevelArea == AREA_THI_HUGE) {
        yaw = clamp_positions_and_find_yaw(pos, focus, 8192.f, -8192.f, 8192.f, -8192.f);
    }
    if (gCurrLevelArea == AREA_THI_TINY) {
        yaw = clamp_positions_and_find_yaw(pos, focus, 2458.f, -2458.f, 2458.f, -2458.f);
    }
#endif

    return yaw;
}

/**
 * "Behind Mario" mode: used when Mario is flying, on the water's surface, or shot from a cannon
 */
s32 mode_behind_mario(struct Camera *c) {
    struct MarioState *marioState = &gMarioStates[0];
    struct Surface *floor;
    Vec3f newPos;
    f32 waterHeight;
    f32 floorHeight;
    f32 distCamToFocus;
    s16 camPitch;
    s16 camYaw;
    s16 yaw;

    gCameraMovementFlags &= ~CAM_MOVING_INTO_MODE;
    vec3f_copy(newPos, c->pos);
    yaw = update_behind_mario_camera(c, c->focus, newPos);
    c->pos[0] = newPos[0];
    c->pos[2] = newPos[2];

    // Keep the camera above the water surface if swimming
    if (c->mode == WATER_SURFACE_CAMERA_MODE) {
        floorHeight = find_floor(c->pos[0], c->pos[1], c->pos[2], &floor);
        newPos[1] = marioState->waterLevel + 120;
        if (newPos[1] < (floorHeight += 120.f)) {
            newPos[1] = floorHeight;
        }
    }
    approach_camera_height(c, newPos[1], 50.f);
    waterHeight = find_water_level(c->pos[0], c->pos[2]) + 100.f;
    if (c->pos[1] <= waterHeight) {
        gCameraMovementFlags |= CAM_MOVE_SUBMERGED;
    } else {
        gCameraMovementFlags &= ~CAM_MOVE_SUBMERGED;
    }

    set_floor_ceiling_midpoint(c->pos);
    // Prevent camera getting too far away
    vec3f_get_dist_and_angle(c->focus, c->pos, &distCamToFocus, &camPitch, &camYaw);
    if (distCamToFocus > 800.f) {
        distCamToFocus = 800.f;
        vec3f_set_dist_and_angle(c->focus, c->pos, distCamToFocus, camPitch, camYaw);
    }
    pan_ahead_of_player(c);

    return yaw;
}


void mode_behind_mario_camera(struct Camera *c) {
    c->nextYaw = mode_behind_mario(c);
}
