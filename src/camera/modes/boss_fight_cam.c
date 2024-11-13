#include "camera/camera_math.h"
#include "camera/camera_modes.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game/level_update.h"
#include "game/camera.h"


/**
 * Updates the camera during a boss fight
 */
s32 update_boss_fight_camera(struct Camera *c, Vec3f focus, Vec3f pos) {
    struct Object *obj;
    f32 focusDistance;
    // Floor normal values
    f32 nx;
    f32 ny;
    f32 nz;
    /// Floor originOffset
    f32 oo;
    s16 yaw;
    s16 heldState;
    struct Surface *floor;
    Vec3f secondFocus;
    Vec3f holdFocOffset = { 0.f, -150.f, -125.f };

    handle_c_button_movement(c);

    // Start camera shakes if bowser jumps or gets thrown.
    if (sMarioCamState->cameraEvent == CAM_EVENT_BOWSER_JUMP) {
        set_environmental_camera_shake(SHAKE_ENV_BOWSER_JUMP);
        sMarioCamState->cameraEvent = CAM_EVENT_NONE;
    }
    if (sMarioCamState->cameraEvent == CAM_EVENT_BOWSER_THROW_BOUNCE) {
        set_environmental_camera_shake(SHAKE_ENV_BOWSER_THROW_BOUNCE);
        sMarioCamState->cameraEvent = CAM_EVENT_NONE;
    }

    yaw = sModeOffsetYaw + DEGREES(45);
    // Get boss's position and whether Mario is holding it.
    if ((obj = gSecondCameraFocus) != NULL) {
        vec3f_copy(secondFocus, &obj->oPosVec);
        heldState = obj->oHeldState;
    } else {
    // If no boss is there, just rotate around the area's center point.
        secondFocus[0] = c->areaCenX;
        secondFocus[1] = sMarioCamState->pos[1];
        secondFocus[2] = c->areaCenZ;
        heldState = 0;
    }

    focusDistance = calc_abs_dist(sMarioCamState->pos, secondFocus) * 1.6f;
    if (focusDistance < 800.f) {
        focusDistance = 800.f;
    }
    if (focusDistance > 5000.f) {
        focusDistance = 5000.f;
    }

    // If holding the boss, add a slight offset to secondFocus so that the spinning is more pronounced.
    if (heldState == HELD_HELD) {
        offset_rotated(secondFocus, sMarioCamState->pos, holdFocOffset, sMarioCamState->faceAngle);
    }

    // Set the camera focus to the average of Mario and secondFocus
    focus[0] = (sMarioCamState->pos[0] + secondFocus[0]) / 2.f;
    focus[1] = (sMarioCamState->pos[1] + secondFocus[1]) / 2.f + 125.f;
    focus[2] = (sMarioCamState->pos[2] + secondFocus[2]) / 2.f;

    // Calculate the camera's position as an offset from the focus
    // When C-Down is not active, this
    vec3f_set_dist_and_angle(focus, pos, focusDistance, 0x1000, yaw);
    // Find the floor of the arena
    pos[1] = find_floor(c->areaCenX, CELL_HEIGHT_LIMIT, c->areaCenZ, &floor);
    if (floor != NULL) {
        nx = floor->normal.x;
        ny = floor->normal.y;
        nz = floor->normal.z;
        oo = floor->originOffset;
        pos[1] = 300.f - (nx * pos[0] + nz * pos[2] + oo) / ny;
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
        switch (gCurrLevelArea) {
            case AREA_BOB:
                pos[1] += 125.f;
                //! fall through, makes the BoB boss fight camera move up twice as high as it should
                FALL_THROUGH;
            case AREA_WF:
                pos[1] += 125.f;
        }
#endif
    }
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    // Prevent the camera from going to the ground in the outside boss fight
    if (gCurrLevelNum == LEVEL_BBH) {
        pos[1] = 2047.f;
    }
#endif
    // Rotate from C-Button input
    if (sCSideButtonYaw < 0) {
        sModeOffsetYaw += 0x200;
        if ((sCSideButtonYaw += 0x100) > 0) {
            sCSideButtonYaw = 0;
        }
    }
    if (sCSideButtonYaw > 0) {
        sModeOffsetYaw -= 0x200;
        if ((sCSideButtonYaw -= 0x100) < 0) {
            sCSideButtonYaw = 0;
        }
    }

    focus[1] = (sMarioCamState->pos[1] + secondFocus[1]) / 2.f + 100.f;
    if (heldState == 1) {
        focus[1] += 300.f * sins((gMarioStates[0].angleVel[1] > 0.f) ?  gMarioStates[0].angleVel[1]
                                                                     : -gMarioStates[0].angleVel[1]);
    }

    //! Unnecessary conditional, focusDistance is already bounded to 800
    // if (focusDistance < 400.f) {
    //     focusDistance = 400.f;
    // }

    // Set C-Down distance and pitch.
    // C-Down will essentially double the distance from the center.
    // sLakituPitch approaches 33.75 degrees.
    lakitu_zoom(focusDistance, 0x1800);

    // Move the camera position back as sLakituDist and sLakituPitch increase.
    // This doesn't zoom out of bounds because pos is set above each frame.
    // The constant 0x1000 doubles the pitch from the center when sLakituPitch is 0
    // When Lakitu is fully zoomed out, the pitch comes to 0x3800, or 78.75 degrees, up from the focus.
    vec3f_set_dist_and_angle(pos, pos, sLakituDist, sLakituPitch + 0x1000, yaw);

    return yaw;
}

void mode_boss_fight_camera(struct Camera *c) {
    c->nextYaw = update_boss_fight_camera(c, c->focus, c->pos);
}