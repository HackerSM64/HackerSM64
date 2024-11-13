#include <ultra64.h>

#include "sm64.h"
#include "camera.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "audio/external.h"
#include "mario_misc.h"
#include "game_init.h"
#include "hud.h"
#include "engine/math_util.h"
#include "area.h"
#include "engine/surface_collision.h"
#include "engine/behavior_script.h"
#include "level_update.h"
#include "ingame_menu.h"
#include "mario_actions_cutscene.h"
#include "save_file.h"
#include "object_helpers.h"
#include "print.h"
#include "spawn_sound.h"
#include "behavior_actions.h"
#include "behavior_data.h"
#include "object_list_processor.h"
#include "paintings.h"
#include "engine/graph_node.h"
#include "level_table.h"
#include "config.h"
#include "puppyprint.h"
#include "profiling.h"

#include "camera/camera_math.h"
#include "camera/vanilla_trigger_code.h"
#include "camera/camera_modes.h"
#include "camera/cutscene_helpers.h"

#define CBUTTON_MASK (U_CBUTTONS | D_CBUTTONS | L_CBUTTONS | R_CBUTTONS)

/**
 * Stores Lakitu's position from the last frame, used for transitioning in next_lakitu_state()
 */
Vec3f sOldPosition;
/**
 * Stores Lakitu's focus from the last frame, used for transitioning in next_lakitu_state()
 */
Vec3f sOldFocus;
/**
 * Global array of PlayerCameraState.
 * L is real.
 */
struct PlayerCameraState gPlayerCameraState[2];

/**
 * Used to decide when to zoom out in the pause menu.
 */
u8 sFramesPaused;

/**
 * Lakitu's position and focus.
 * @see LakituState
 */
struct LakituState gLakituState;
struct CameraFOVStatus sFOVState;
struct TransitionInfo sModeTransition;
struct PlayerGeometry sMarioGeometry;
struct Camera *gCamera;
s16 sAvoidYawVel;
s16 sCameraYawAfterDoorCutscene;

// Shaky Hand-held Camera effect variables
struct HandheldShakePoint sHandheldShakeSpline[4];
s16 sHandheldShakeMag;
f32 sHandheldShakeTimer;
f32 sHandheldShakeInc;
s16 sHandheldShakePitch;
s16 sHandheldShakeYaw;
s16 sHandheldShakeRoll;

/**
 * Controls which object to spawn in the intro and ending cutscenes.
 */
u32 gCutsceneObjSpawn;
/**
 * Controls when an object-based cutscene should end. It's only used in the star spawn cutscenes, but
 * Yoshi also toggles this.
 */
s32 gObjCutsceneDone;

/**
 * Determines which R-Trigger mode is selected in the pause menu.
 */
s16 sSelectionFlags;

/**
 * Flags that determine what movements the camera should start / do this frame.
 */
s16 gCameraMovementFlags;

/**
 * Flags that change how modes operate and how Lakitu moves.
 * The most commonly used flag is CAM_FLAG_SMOOTH_MOVEMENT, which makes Lakitu fly to the next position,
 * instead of warping.
 */
s16 sStatusFlags;
/**
 * Flags that determine whether the player has already rotated left or right. Used in radial mode to
 * determine whether to rotate all the way, or just to 60 degrees.
 */
s16 s2ndRotateFlags;
/**
 * Flags that control buzzes and sounds that play, mostly for C-button input.
 */
s16 sCameraSoundFlags;
/**
 * Stores what C-Buttons are pressed this frame.
 */
u16 sCButtonsPressed;

/**
 * The angle of the direction vector from the area's center to Mario's position.
 */
s16 sAreaYaw;

/**
 * How much sAreaYaw changed when Mario moved.
 */
s16 sAreaYawChange;

/**
 * Lakitu's distance from Mario in C-Down mode
 */
s16 sLakituDist;

/**
 * How much Lakitu looks down in C-Down mode
 */
s16 sLakituPitch;

/**
 * The amount of distance left to zoom out
 */
f32 sZoomAmount;

s16 sCSideButtonYaw;

/**
 * Sound timer used to space out sounds in behind Mario mode
 */
s16 sBehindMarioSoundTimer;

/**
 * Virtually unused aside being set to 0 and compared with gCameraZoomDist (which is never < 0)
 */
f32 sZeroZoomDist;

/**
 * The camera's pitch in C-Up mode. Mainly controls Mario's head rotation.
 */
s16 sCUpCameraPitch;
/**
 * The current mode's yaw, which gets added to the camera's yaw.
 */
s16 sModeOffsetYaw;

/**
 * The constant offset to 8-direction mode's yaw.
 */
s16 s8DirModeBaseYaw;
/**
 * Player-controlled yaw offset in 8-direction mode, a multiple of 45 degrees.
 */
s16 s8DirModeYawOffset;

/**
 * The distance that the camera will look ahead of Mario in the direction Mario is facing.
 */
f32 sPanDistance;

/**
 * When Mario gets in the cannon, it is pointing straight up and rotates down.
 * This is used to make the camera start up and rotate down, like the cannon.
 */
f32 sCannonYOffset;

struct ModeTransitionInfo sModeInfo;
/**
 * Offset added to sFixedModeBasePosition when Mario is inside, near the castle lobby entrance
 */
Vec3f sCastleEntranceOffset;

/**
 * The information stored when C-Up is active, used to update Lakitu's rotation when exiting C-Up
 */
struct CameraStoredInfo sCameraStoreCUp;

/**
 * The information stored during cutscenes
 */
struct CameraStoredInfo sCameraStoreCutscene;

/**
 * The information of a second focus camera used by some objects
 */
struct Object *gSecondCameraFocus = NULL;

/**
 * How fast the camera's yaw should approach the next yaw.
 */
s16 sYawSpeed = 0x400;

s32 gCurrLevelArea = 0;
u32 gPrevLevel = 0;
f32 gCameraZoomDist = 800.0f;

/**
 * A timer that increments for 8 frames when a cutscene ends.
 * When it reaches 8, it sets gRecentCutscene to 0.
 */
u8 sFramesSinceCutsceneEnded = 0;

struct PlayerCameraState *sMarioCamState = &gPlayerCameraState[0];
Vec3f sFixedModeBasePosition    = { 646.0f, 143.0f, -1513.0f };

typedef s32 (*CameraTransition)(struct Camera *c, Vec3f focus, Vec3f pos);
CameraTransition sModeTransitions[] = {
    NULL,
    update_radial_camera,
    update_outward_radial_camera,
    update_behind_mario_camera,
    update_mario_camera,
    unused_update_mode_5_camera,
    update_c_up,
    update_mario_camera,
    nop_update_water_camera,
    update_slide_or_0f_camera,
    update_in_cannon,
    update_boss_fight_camera,
    update_parallel_tracking_camera,
    update_fixed_camera,
    update_8_directions_camera,
    update_slide_or_0f_camera,
    update_mario_camera,
    update_spiral_stairs_camera
};

/**
 * Starts a camera shake triggered by an interaction
 */
void set_camera_shake_from_hit(s16 shake) {
    switch (shake) {
        // Makes the camera stop for a bit
        case SHAKE_ATTACK:
            gLakituState.focHSpeed = 0;
            gLakituState.posHSpeed = 0;
            break;

        case SHAKE_FALL_DAMAGE:
            set_camera_pitch_shake(0x60, 0x3, 0x8000);
            set_camera_roll_shake(0x60, 0x3, 0x8000);
            break;

        case SHAKE_GROUND_POUND:
            set_camera_pitch_shake(0x60, 0xC, 0x8000);
            break;

        case SHAKE_SMALL_DAMAGE:
            if (sMarioCamState->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER)) {
                set_camera_yaw_shake(0x200, 0x10, 0x1000);
                set_camera_roll_shake(0x400, 0x20, 0x1000);
                set_fov_shake(0x100, 0x30, 0x8000);
            } else {
                set_camera_yaw_shake(0x80, 0x8, 0x4000);
                set_camera_roll_shake(0x80, 0x8, 0x4000);
                set_fov_shake(0x100, 0x30, 0x8000);
            }

            gLakituState.focHSpeed = 0;
            gLakituState.posHSpeed = 0;
            break;

        case SHAKE_MED_DAMAGE:
            if (sMarioCamState->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER)) {
                set_camera_yaw_shake(0x400, 0x20, 0x1000);
                set_camera_roll_shake(0x600, 0x30, 0x1000);
                set_fov_shake(0x180, 0x40, 0x8000);
            } else {
                set_camera_yaw_shake(0x100, 0x10, 0x4000);
                set_camera_roll_shake(0x100, 0x10, 0x4000);
                set_fov_shake(0x180, 0x40, 0x8000);
            }

            gLakituState.focHSpeed = 0;
            gLakituState.posHSpeed = 0;
            break;

        case SHAKE_LARGE_DAMAGE:
            if (sMarioCamState->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER)) {
                set_camera_yaw_shake(0x600, 0x30, 0x1000);
                set_camera_roll_shake(0x800, 0x40, 0x1000);
                set_fov_shake(0x200, 0x50, 0x8000);
            } else {
                set_camera_yaw_shake(0x180, 0x20, 0x4000);
                set_camera_roll_shake(0x200, 0x20, 0x4000);
                set_fov_shake(0x200, 0x50, 0x8000);
            }

            gLakituState.focHSpeed = 0;
            gLakituState.posHSpeed = 0;
            break;

        case SHAKE_HIT_FROM_BELOW:
            gLakituState.focHSpeed = 0.07f;
            gLakituState.posHSpeed = 0.07f;
            break;

        case SHAKE_SHOCK:
            set_camera_pitch_shake(random_float() * 64.0f, 0x8, 0x8000);
            set_camera_yaw_shake(random_float() * 64.0f, 0x8, 0x8000);
            break;
    }
}

/**
 * Start a shake from the environment
 */
void set_environmental_camera_shake(s16 shake) {
    switch (shake) {
        case SHAKE_ENV_EXPLOSION:
            set_camera_pitch_shake(0x60, 0x8, 0x4000);
            break;

        case SHAKE_ENV_BOWSER_THROW_BOUNCE:
            set_camera_pitch_shake(0xC0, 0x8, 0x4000);
            break;

        case SHAKE_ENV_BOWSER_JUMP:
            set_camera_pitch_shake(0x100, 0x8, 0x3000);
            break;

        case SHAKE_ENV_UNUSED_6:
            set_camera_roll_shake(0x80, 0x10, 0x3000);
            break;

        case SHAKE_ENV_UNUSED_7:
            set_camera_pitch_shake(0x20, 0x8, 0x8000);
            break;

        case SHAKE_ENV_PYRAMID_EXPLODE:
            set_camera_pitch_shake(0x40, 0x8, 0x8000);
            break;

        case SHAKE_ENV_JRB_SHIP_DRAIN:
            set_camera_pitch_shake(0x20, 0x8, 0x8000);
            set_camera_roll_shake(0x400, 0x10, 0x100);
            break;

        case SHAKE_ENV_FALLING_BITS_PLAT:
            set_camera_pitch_shake(0x40, 0x2, 0x8000);
            break;

        case SHAKE_ENV_UNUSED_5:
            set_camera_yaw_shake(-0x200, 0x80, 0x200);
            break;
    }
}

/**
 * Starts a camera shake, but scales the amplitude by the point's distance from the camera
 */
void set_camera_shake_from_point(s16 shake, f32 posX, f32 posY, f32 posZ) {
    switch (shake) {
        case SHAKE_POS_BOWLING_BALL:
            set_pitch_shake_from_point(0x28, 0x8, 0x4000, 2000.0f, posX, posY, posZ);
            break;

        case SHAKE_POS_SMALL:
            set_pitch_shake_from_point(0x80, 0x8, 0x4000, 4000.0f, posX, posY, posZ);
            set_fov_shake_from_point_preset(SHAKE_FOV_SMALL, posX, posY, posZ);
            break;

        case SHAKE_POS_MEDIUM:
            set_pitch_shake_from_point(0xC0, 0x8, 0x4000, 6000.0f, posX, posY, posZ);
            set_fov_shake_from_point_preset(SHAKE_FOV_MEDIUM, posX, posY, posZ);
            break;

        case SHAKE_POS_LARGE:
            set_pitch_shake_from_point(0x100, 0x8, 0x3000, 8000.0f, posX, posY, posZ);
            set_fov_shake_from_point_preset(SHAKE_FOV_LARGE, posX, posY, posZ);
            break;
    }
}

/**
 * Start a camera shake from an environmental source, but only shake the camera's pitch.
 */
void unused_set_camera_pitch_shake_env(s16 shake) {
    switch (shake) {
        case SHAKE_ENV_EXPLOSION:
            set_camera_pitch_shake(0x60, 0x8, 0x4000);
            break;

        case SHAKE_ENV_BOWSER_THROW_BOUNCE:
            set_camera_pitch_shake(0xC0, 0x8, 0x4000);
            break;

        case SHAKE_ENV_BOWSER_JUMP:
            set_camera_pitch_shake(0x100, 0x8, 0x3000);
            break;
    }
}

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
s16 find_in_bounds_yaw_wdw_bob_thi(Vec3f pos, Vec3f origin, s16 yaw) {
    switch (gCurrLevelArea) {
        case AREA_WDW_MAIN:
            yaw = clamp_positions_and_find_yaw(pos, origin, 4508.f, -3739.f, 4508.f, -3739.f);
            break;
        case AREA_BOB:
            yaw = clamp_positions_and_find_yaw(pos, origin, 8000.f, -8000.f, 7050.f, -8000.f);
            break;
        case AREA_THI_HUGE:
            yaw = clamp_positions_and_find_yaw(pos, origin, 8192.f, -8192.f, 8192.f, -8192.f);
            break;
        case AREA_THI_TINY:
            yaw = clamp_positions_and_find_yaw(pos, origin, 2458.f, -2458.f, 2458.f, -2458.f);
            break;
    }
    return yaw;
}
#endif

/**
 * Moves Lakitu from zoomed in to zoomed out and vice versa.
 * When C-Down mode is not active, sLakituDist and sLakituPitch decrease to 0.
 */
void lakitu_zoom(f32 rangeDist, s16 rangePitch) {
    if (sLakituDist < 0) {
        if ((sLakituDist += 30) > 0) {
            sLakituDist = 0;
        }
    } else if (rangeDist < sLakituDist) {
        if ((sLakituDist -= 30) < rangeDist) {
            sLakituDist = rangeDist;
        }
    } else if (gCameraMovementFlags & CAM_MOVE_ZOOMED_OUT) {
        if ((sLakituDist += 30) > rangeDist) {
            sLakituDist = rangeDist;
        }
    } else {
        if ((sLakituDist -= 30) < 0) {
            sLakituDist = 0;
        }
    }

    if (gCurrLevelArea == AREA_SSL_PYRAMID && gCamera->mode == CAMERA_MODE_OUTWARD_RADIAL) {
        rangePitch /= 2;
    }

    if (gCameraMovementFlags & CAM_MOVE_ZOOMED_OUT) {
        if ((sLakituPitch += rangePitch / 13) > rangePitch) {
            sLakituPitch = rangePitch;
        }
    } else {
        if ((sLakituPitch -= rangePitch / 13) < 0) {
            sLakituPitch = 0;
        }
    }
}

s32 snap_to_45_degrees(s16 angle) {
    if (angle % DEGREES(45)) {
        s16 d1 = ABS(angle) % DEGREES(45);
        s16 d2 = DEGREES(45) - d1;
        if (angle > 0) {
            if (d1 < d2) return angle - d1;
            else return angle + d2;
        } else {
            if (d1 < d2) return angle + d1;
            else return angle - d2;
        }
    }
    return angle;
}

s32 unused_update_mode_5_camera(UNUSED struct Camera *c, UNUSED Vec3f focus, UNUSED Vec3f pos) {
   return 0;
}

s32 nop_update_water_camera(UNUSED struct Camera *c, UNUSED Vec3f focus, UNUSED Vec3f pos) {
   return 0;
}

s32 update_slide_or_0f_camera(UNUSED struct Camera *c, Vec3f focus, Vec3f pos) {
    s16 yaw = sMarioCamState->faceAngle[1] + sModeOffsetYaw + DEGREES(180);

    focus_on_mario(focus, pos, 125.f, 125.f, 800.f, DEGREES(30), yaw);
    return sMarioCamState->faceAngle[1];
}


void store_lakitu_cam_info_for_c_up(struct Camera *c) {
    vec3f_copy(sCameraStoreCUp.pos, c->pos);
    vec3f_sub(sCameraStoreCUp.pos, sMarioCamState->pos);
    // Only store the y value, and as an offset from Mario, for some reason
    vec3f_set(sCameraStoreCUp.focus, 0.f, c->focus[1] - sMarioCamState->pos[1], 0.f);
}

/**
 * Start C-Up mode. The actual mode change is handled in update_mario_inputs() in mario.c
 *
 * @see update_mario_inputs
 */
void set_mode_c_up(struct Camera *c) {
    if (!(gCameraMovementFlags & CAM_MOVE_C_UP_MODE)) {
        gCameraMovementFlags |= CAM_MOVE_C_UP_MODE;
        store_lakitu_cam_info_for_c_up(c);
        sCameraSoundFlags &= ~CAM_SOUND_C_UP_PLAYED;
    }
}

/**
 * Cause Lakitu to fly to the next Camera position and focus over a number of frames.
 *
 * At the end of each frame, Lakitu's position and focus ("state") are stored.
 * Calling this function makes next_lakitu_state() fly from the last frame's state to the
 * current frame's calculated state.
 *
 * @see next_lakitu_state()
 */
void transition_next_state(UNUSED struct Camera *c, s16 frames) {
    if (!(sStatusFlags & CAM_FLAG_FRAME_AFTER_CAM_INIT)) {
        sStatusFlags |= (CAM_FLAG_START_TRANSITION | CAM_FLAG_TRANSITION_OUT_OF_C_UP);
        sModeTransition.framesLeft = frames;
    }
}

/**
 * Sets the camera mode to `newMode` and initializes sModeTransition with `numFrames` frames
 *
 * Used to change the camera mode to 'level-oriented' modes
 *      namely: RADIAL/OUTWARD_RADIAL, 8_DIRECTIONS, FREE_ROAM, CLOSE, SPIRAL_STAIRS, and SLIDE_HOOT
 */
void transition_to_camera_mode(struct Camera *c, s16 newMode, s16 numFrames) {
    if (c->mode != newMode) {
        sModeInfo.newMode = (newMode != -1) ? newMode : sModeInfo.lastMode;
        sModeInfo.lastMode = c->mode;
        c->mode = sModeInfo.newMode;

        // Clear movement flags that would affect the transition
        gCameraMovementFlags &= (u16)~(CAM_MOVE_RESTRICT | CAM_MOVE_ROTATE);
        if (!(sStatusFlags & CAM_FLAG_FRAME_AFTER_CAM_INIT)) {
            transition_next_state(c, numFrames);
            sCUpCameraPitch = 0;
            sModeOffsetYaw = 0;
            sLakituDist = 0;
            sLakituPitch = 0;
            sAreaYawChange = 0;
            sPanDistance = 0.f;
            sCannonYOffset = 0.f;
        }
    }
}

/**
 * Used to change the camera mode between its default/previous and certain Mario-oriented modes,
 *      namely: C_UP, WATER_SURFACE, CLOSE, and BEHIND_MARIO
 *
 * Stores the current pos and focus in sModeInfo->transitionStart, and
 * stores the next pos and focus into sModeInfo->transitionEnd. These two fields are used in
 * move_into_c_up().
 *
 * @param mode the mode to change to, or -1 to switch to the previous mode
 * @param frames number of frames the transition should last, only used when entering C_UP
 */
void set_camera_mode(struct Camera *c, s16 mode, s16 frames) {
    struct LinearTransitionPoint *start = &sModeInfo.transitionStart;
    struct LinearTransitionPoint *end = &sModeInfo.transitionEnd;

#ifdef ENABLE_VANILLA_CAM_PROCESSING
    if (mode == CAMERA_MODE_WATER_SURFACE && gCurrLevelArea == AREA_TTM_OUTSIDE) {
    } else {
#endif
        // Clear movement flags that would affect the transition
        gCameraMovementFlags &= (u16)~(CAM_MOVE_RESTRICT | CAM_MOVE_ROTATE);
        gCameraMovementFlags |= CAM_MOVING_INTO_MODE;
        if (mode == CAMERA_MODE_NONE) {
            mode = CAMERA_MODE_CLOSE;
        }
        sCUpCameraPitch = 0;
        sModeOffsetYaw = 0;
        sLakituDist = 0;
        sLakituPitch = 0;
        sAreaYawChange = 0;

        sModeInfo.newMode = (mode != -1) ? mode : sModeInfo.lastMode;
        sModeInfo.lastMode = c->mode;
        sModeInfo.max = frames;
        sModeInfo.frame = 1;

        c->mode = sModeInfo.newMode;
        gLakituState.mode = c->mode;

        vec3f_copy(end->focus, c->focus);
        vec3f_sub(end->focus, sMarioCamState->pos);

        vec3f_copy(end->pos, c->pos);
        vec3f_sub(end->pos, sMarioCamState->pos);

#ifndef ENABLE_VANILLA_CAM_PROCESSING
        if (mode == CAMERA_MODE_8_DIRECTIONS) {
            // Helps transition from any camera mode to 8dir
            s8DirModeYawOffset = snap_to_45_degrees(c->yaw);
        }
#endif

        sAreaYaw = sModeTransitions[sModeInfo.newMode](c, end->focus, end->pos);

        // End was updated by sModeTransitions
        vec3f_sub(end->focus, sMarioCamState->pos);
        vec3f_sub(end->pos, sMarioCamState->pos);

        vec3f_copy(start->focus, gLakituState.curFocus);
        vec3f_sub(start->focus, sMarioCamState->pos);

        vec3f_copy(start->pos, gLakituState.curPos);
        vec3f_sub(start->pos, sMarioCamState->pos);

        vec3f_get_dist_and_angle(start->focus, start->pos, &start->dist, &start->pitch, &start->yaw);
        vec3f_get_dist_and_angle(end->focus, end->pos, &end->dist, &end->pitch, &end->yaw);
#ifdef ENABLE_VANILLA_CAM_PROCESSING
    }
#endif
}

/**
 * Updates Lakitu's position/focus and applies camera shakes.
 */
void update_lakitu(struct Camera *c) {
    struct Surface *floor = NULL;
    Vec3f newPos;
    Vec3f newFoc;
    f32 distToFloor;
    s16 newYaw;

    if (!(gCameraMovementFlags & CAM_MOVE_PAUSE_SCREEN)) {
        newYaw = next_lakitu_state(newPos, newFoc, c->pos, c->focus, sOldPosition, sOldFocus,
                                   c->nextYaw);
        set_or_approach_s16_symmetric(&c->yaw, newYaw, sYawSpeed);
        sStatusFlags &= ~CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;

        // Update old state
        vec3f_copy(sOldPosition, newPos);
        vec3f_copy(sOldFocus, newFoc);

        gLakituState.yaw = c->yaw;
        gLakituState.nextYaw = c->nextYaw;
        vec3f_copy(gLakituState.goalPos, c->pos);
        vec3f_copy(gLakituState.goalFocus, c->focus);

        // Simulate Lakitu flying to the new position and turning towards the new focus
        set_or_approach_vec3f_asymptotic(gLakituState.curPos, newPos,
                                         gLakituState.posHSpeed, gLakituState.posVSpeed,
                                         gLakituState.posHSpeed);
        set_or_approach_vec3f_asymptotic(gLakituState.curFocus, newFoc,
                                         gLakituState.focHSpeed, gLakituState.focVSpeed,
                                         gLakituState.focHSpeed);
        // Adjust Lakitu's speed back to normal
        set_or_approach_f32_asymptotic(&gLakituState.focHSpeed, 0.8f, 0.05f);
        set_or_approach_f32_asymptotic(&gLakituState.focVSpeed, 0.3f, 0.05f);
        set_or_approach_f32_asymptotic(&gLakituState.posHSpeed, 0.3f, 0.05f);
        set_or_approach_f32_asymptotic(&gLakituState.posVSpeed, 0.3f, 0.05f);

        // Turn on smooth movement when it hasn't been blocked for 2 frames
        if (sStatusFlags & CAM_FLAG_BLOCK_SMOOTH_MOVEMENT) {
            sStatusFlags &= ~CAM_FLAG_BLOCK_SMOOTH_MOVEMENT;
        } else {
            sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
        }

        vec3f_copy(gLakituState.pos, gLakituState.curPos);
        vec3f_copy(gLakituState.focus, gLakituState.curFocus);

        if (c->cutscene) {
            vec3f_add(gLakituState.focus, sPlayer2FocusOffset);
            vec3_zero(sPlayer2FocusOffset);
        }

        vec3f_get_dist_and_angle(gLakituState.pos, gLakituState.focus, &gLakituState.focusDistance,
                                 &gLakituState.oldPitch, &gLakituState.oldYaw);

        gLakituState.roll = 0;

        // Apply camera shakes
        shake_camera_pitch(gLakituState.pos, gLakituState.focus);
        shake_camera_yaw(gLakituState.pos, gLakituState.focus);
        shake_camera_roll(&gLakituState.roll);
        shake_camera_handheld(gLakituState.pos, gLakituState.focus);

        if (sMarioCamState->action == ACT_DIVE && gLakituState.lastFrameAction != ACT_DIVE) {
            set_camera_shake_from_hit(SHAKE_HIT_FROM_BELOW);
        }

        gLakituState.roll += sHandheldShakeRoll;
        gLakituState.roll += gLakituState.keyDanceRoll;

        if (c->mode != CAMERA_MODE_C_UP && c->cutscene == CUTSCENE_NONE) {
            gCollisionFlags |= COLLISION_FLAG_CAMERA;
            distToFloor = find_floor(gLakituState.pos[0],
                                     gLakituState.pos[1] + 20.0f,
                                     gLakituState.pos[2], &floor);
            if (distToFloor != FLOOR_LOWER_LIMIT) {
                if (gLakituState.pos[1] < (distToFloor += 100.0f)) {
                    gLakituState.pos[1] = distToFloor;
                } else {
                    gCollisionFlags &= ~COLLISION_FLAG_CAMERA;
                }
            }
        }

        vec3f_copy(sModeTransition.marioPos, sMarioCamState->pos);
    }
    clamp_pitch(gLakituState.pos, gLakituState.focus, 0x3E00, -0x3E00);
    gLakituState.mode = c->mode;
    gLakituState.defMode = c->defMode;
}

/**
 * Reset all the camera variables to their arcane defaults
 */
void reset_camera(struct Camera *c) {
    gCamera = c;
    s2ndRotateFlags = 0;
    sStatusFlags = 0;

    reset_cutscene_vars();
    gSecondCameraFocus = NULL;
    sCButtonsPressed = 0;
    vec3f_copy(sModeTransition.marioPos, sMarioCamState->pos);
    sModeTransition.framesLeft = 0;
    gCameraMovementFlags = CAM_MOVE_INIT_CAMERA;
    sStatusFlags = 0;
    sCameraSoundFlags = 0;
    sCUpCameraPitch = 0;
    sModeOffsetYaw = 0;
    sSpiralStairsYawOffset = 0;
    sLakituDist = 0;
    sLakituPitch = 0;
    sAreaYaw = 0;
    sAreaYawChange = 0.f;
    sPanDistance = 0.f;
    sCannonYOffset = 0.f;
    sZoomAmount = 0.f;
    sZeroZoomDist = 0.f;
    sBehindMarioSoundTimer = 0;
    sCSideButtonYaw = 0;
    s8DirModeBaseYaw = 0;
    s8DirModeYawOffset = 0;
    c->doorStatus = DOOR_DEFAULT;
    sMarioCamState->headRotation[0] = 0;
    sMarioCamState->headRotation[1] = 0;
    sMarioCamState->cameraEvent = CAM_EVENT_NONE;
    sMarioCamState->usedObj = NULL;
    gLakituState.shakeMagnitude[0] = 0;
    gLakituState.shakeMagnitude[1] = 0;
    gLakituState.shakeMagnitude[2] = 0;
    gLakituState.lastFrameAction = 0;
    set_fov_function(CAM_FOV_DEFAULT);
    sFOVState.fov = 45.f;
    sFOVState.fovOffset = 0.f;
    sFOVState.unusedIsSleeping = 0;
    sFOVState.shakeAmplitude = 0.f;
    sFOVState.shakePhase = 0;
}

void init_camera(struct Camera *c) {
    struct Surface *floor = NULL;
    Vec3f marioOffset;
    s32 i;

    init_cutscene_vars();
    gPrevLevel = gCurrLevelArea / 16;
    gCurrLevelArea = gCurrLevelNum * 16 + gCurrentArea->index;
    sSelectionFlags &= CAM_MODE_MARIO_SELECTED;
    sFramesPaused = 0;
    gLakituState.mode = c->mode;
    gLakituState.defMode = c->defMode;
    gLakituState.posHSpeed = 0.3f;
    gLakituState.posVSpeed = 0.3f;
    gLakituState.focHSpeed = 0.8f;
    gLakituState.focVSpeed = 0.3f;
    gLakituState.roll = 0;
    gLakituState.keyDanceRoll = 0;
    sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
    vec3_zero(sCastleEntranceOffset);
    find_mario_floor_and_ceil(&sMarioGeometry);
    sMarioGeometry.prevFloorHeight = sMarioGeometry.currFloorHeight;
    sMarioGeometry.prevCeilHeight = sMarioGeometry.currCeilHeight;
    sMarioGeometry.prevFloor = sMarioGeometry.currFloor;
    sMarioGeometry.prevCeil = sMarioGeometry.currCeil;
    sMarioGeometry.prevFloorType = sMarioGeometry.currFloorType;
    sMarioGeometry.prevCeilType = sMarioGeometry.currCeilType;
    init_current_credits_spline();
    cutscene_reset_spline();
    sHandheldShakeInc = 0.f;
    sHandheldShakeTimer = 0.f;
    sHandheldShakeMag = 0;
    for (i = 0; i < 4; i++) {
        sHandheldShakeSpline[i].index = -1;
    }
    sHandheldShakePitch = 0;
    sHandheldShakeYaw = 0;
    sHandheldShakeRoll = 0;
    c->cutscene = CUTSCENE_NONE;
    marioOffset[0] = 0.f;
    marioOffset[1] = 125.f;
    marioOffset[2] = 400.f;

    // Set the camera's starting position or start a cutscene for certain levels
    switch (gCurrLevelNum) {
        // Calls the initial cutscene when you enter Bowser battle levels
        // Note: This replaced an "old" way to call these cutscenes using
        // a camEvent value: CAM_EVENT_BOWSER_INIT
        case LEVEL_BOWSER_1:
            // Since Bowser 1 has a demo entry, check for it
            // If it is, then set CamAct to the end to directly activate Bowser
            // If it isn't, then start cutscene
            if (gCurrDemoInput == NULL) {
                start_cutscene(c, CUTSCENE_ENTER_BOWSER_ARENA);
            } else if (gSecondCameraFocus != NULL) {
                gSecondCameraFocus->oBowserCamAct = BOWSER_CAM_ACT_END;
            }
            break;
        case LEVEL_BOWSER_2:
            start_cutscene(c, CUTSCENE_ENTER_BOWSER_ARENA);
            break;
        case LEVEL_BOWSER_3:
            start_cutscene(c, CUTSCENE_ENTER_BOWSER_ARENA);
            break;

#ifdef ENABLE_VANILLA_CAM_PROCESSING
        //! Hardcoded position checks determine which cutscene to play when Mario enters castle grounds.
        case LEVEL_CASTLE_GROUNDS:
            if (is_within_100_units_of_mario(-1328.f, 260.f, 4664.f) != 1) {
                marioOffset[0] = -400.f;
                marioOffset[2] = -800.f;
            }
            if (is_within_100_units_of_mario(-6901.f, 2376.f, -6509.f) == 1) {
                start_cutscene(c, CUTSCENE_EXIT_WATERFALL);
            }
            if (is_within_100_units_of_mario(5408.f, 4500.f, 3637.f) == 1) {
                start_cutscene(c, CUTSCENE_EXIT_FALL_WMOTR);
            }
            gLakituState.mode = CAMERA_MODE_FREE_ROAM;
            break;
        case LEVEL_SA:
            marioOffset[2] = 200.f;
            break;
        case LEVEL_CASTLE_COURTYARD:
            marioOffset[2] = -300.f;
            break;
        case LEVEL_LLL:
            gCameraMovementFlags |= CAM_MOVE_ZOOMED_OUT;
            break;
        case LEVEL_CASTLE:
            marioOffset[2] = 150.f;
            break;
        case LEVEL_RR:
            vec3f_set(sFixedModeBasePosition, -2985.f, 478.f, -5568.f);
            break;
#endif
    }
    if (c->mode == CAMERA_MODE_8_DIRECTIONS) {
        gCameraMovementFlags |= CAM_MOVE_ZOOMED_OUT;
    }
    switch (gCurrLevelArea) {
#ifdef ENABLE_VANILLA_CAM_PROCESSING
        case AREA_SSL_EYEROK:
            vec3f_set(marioOffset, 0.f, 500.f, -100.f);
            break;
        case AREA_CCM_SLIDE:
            marioOffset[2] = -300.f;
            break;
        case AREA_THI_WIGGLER:
            marioOffset[2] = -300.f;
            break;
        case AREA_SL_IGLOO:
            marioOffset[2] = -300.f;
            break;
        case AREA_SL_OUTSIDE:
            if (is_within_100_units_of_mario(257.f, 2150.f, 1399.f) == 1) {
                marioOffset[2] = -300.f;
            }
            break;
        case AREA_CCM_OUTSIDE:
            gCameraMovementFlags |= CAM_MOVE_ZOOMED_OUT;
            break;
        case AREA_TTM_OUTSIDE:
            gLakituState.mode = CAMERA_MODE_RADIAL;
            break;
#endif
    }

    // Set the camera pos to marioOffset (relative to Mario), added to Mario's position
    offset_rotated(c->pos, sMarioCamState->pos, marioOffset, sMarioCamState->faceAngle);
    if (c->mode != CAMERA_MODE_BEHIND_MARIO) {
        c->pos[1] = find_floor(sMarioCamState->pos[0], sMarioCamState->pos[1] + 100.f,
                               sMarioCamState->pos[2], &floor) + 125.f;
    }
    vec3f_copy(c->focus, sMarioCamState->pos);
    vec3f_copy(gLakituState.curPos, c->pos);
    vec3f_copy(gLakituState.curFocus, c->focus);
    vec3f_copy(gLakituState.goalPos, c->pos);
    vec3f_copy(gLakituState.goalFocus, c->focus);
    vec3f_copy(gLakituState.pos, c->pos);
    vec3f_copy(gLakituState.focus, c->focus);
    if (c->mode == CAMERA_MODE_FIXED) {
        set_fixed_cam_axis_sa_lobby(c->mode);
    }
    store_lakitu_cam_info_for_c_up(c);
    gLakituState.yaw = calculate_yaw(c->focus, c->pos);
    gLakituState.nextYaw = gLakituState.yaw;
    c->yaw = gLakituState.yaw;
    c->nextYaw = gLakituState.yaw;
#ifdef PUPPYCAM
    puppycam_init();
#endif
}

void select_mario_cam_mode(void) {
    sSelectionFlags = CAM_MODE_MARIO_SELECTED;
}
/**
 * If `selection` is 0, just get the current selection
 * If `selection` is 1, select 'Mario' as the alt mode.
 * If `selection` is 2, select 'fixed' as the alt mode.
 *
 * @return the current selection
 */
s32 cam_select_alt_mode(s32 selection) {
    s32 mode = CAM_SELECTION_FIXED;

    if (selection == CAM_SELECTION_MARIO) {
        if (!(sSelectionFlags & CAM_MODE_MARIO_SELECTED)) {
            sSelectionFlags |= CAM_MODE_MARIO_SELECTED;
        }
        sCameraSoundFlags |= CAM_SOUND_UNUSED_SELECT_MARIO;
    }

    // The alternate mode is up-close, but the player just selected fixed in the pause menu
    if (selection == CAM_SELECTION_FIXED && (sSelectionFlags & CAM_MODE_MARIO_SELECTED)) {
        // So change to normal mode in case the user paused in up-close mode
        set_cam_angle(CAM_ANGLE_LAKITU);
        sSelectionFlags &= ~CAM_MODE_MARIO_SELECTED;
        sCameraSoundFlags |= CAM_SOUND_UNUSED_SELECT_FIXED;
    }

    if (sSelectionFlags & CAM_MODE_MARIO_SELECTED) {
        mode = CAM_SELECTION_MARIO;
    }
    return mode;
}

/**
 * Sets the camera angle to either Lakitu or Mario mode. Returns the current mode.
 *
 * If `mode` is 0, just returns the current mode.
 * If `mode` is 1, start Mario mode
 * If `mode` is 2, start Lakitu mode
 */
s32 set_cam_angle(s32 mode) {
    s32 curMode = CAM_ANGLE_LAKITU;

    // Switch to Mario mode
    if (mode == CAM_ANGLE_MARIO && !(sSelectionFlags & CAM_MODE_MARIO_ACTIVE)) {
        sSelectionFlags |= CAM_MODE_MARIO_ACTIVE;
        if (gCameraMovementFlags & CAM_MOVE_ZOOMED_OUT) {
            sSelectionFlags |= CAM_MODE_LAKITU_WAS_ZOOMED_OUT;
            gCameraMovementFlags &= ~CAM_MOVE_ZOOMED_OUT;
        }
        sCameraSoundFlags |= CAM_SOUND_MARIO_ACTIVE;
    }

    // Switch back to normal mode
    if (mode == CAM_ANGLE_LAKITU && (sSelectionFlags & CAM_MODE_MARIO_ACTIVE)) {
        sSelectionFlags &= ~CAM_MODE_MARIO_ACTIVE;
        if (sSelectionFlags & CAM_MODE_LAKITU_WAS_ZOOMED_OUT) {
            sSelectionFlags &= ~CAM_MODE_LAKITU_WAS_ZOOMED_OUT;
            gCameraMovementFlags |= CAM_MOVE_ZOOMED_OUT;
        } else {
            gCameraMovementFlags &= ~CAM_MOVE_ZOOMED_OUT;
        }
        sCameraSoundFlags |= CAM_SOUND_NORMAL_ACTIVE;
    }
    if (sSelectionFlags & CAM_MODE_MARIO_ACTIVE) {
        curMode = CAM_ANGLE_MARIO;
    }
    return curMode;
}

/**
 * Enables the handheld shake effect for this frame.
 *
 * @see shake_camera_handheld()
 */
void set_handheld_shake(u8 mode) {
    switch (mode) {
        // They're not in numerical order because that would be too simple...
        case HAND_CAM_SHAKE_CUTSCENE: // Lowest increment
            sHandheldShakeMag = 0x600;
            sHandheldShakeInc = 0.04f;
            break;
        case HAND_CAM_SHAKE_LOW: // Lowest magnitude
            sHandheldShakeMag = 0x300;
            sHandheldShakeInc = 0.06f;
            break;
        case HAND_CAM_SHAKE_HIGH: // Highest mag and inc
            sHandheldShakeMag = 0x1000;
            sHandheldShakeInc = 0.1f;
            break;
        case HAND_CAM_SHAKE_UNUSED: // Never used
            sHandheldShakeMag = 0x600;
            sHandheldShakeInc = 0.07f;
            break;
        case HAND_CAM_SHAKE_HANG_OWL: // exactly the same as UNUSED...
            sHandheldShakeMag = 0x600;
            sHandheldShakeInc = 0.07f;
            break;
        case HAND_CAM_SHAKE_STAR_DANCE: // Slightly steadier than HANG_OWL and UNUSED
            sHandheldShakeMag = 0x400;
            sHandheldShakeInc = 0.07f;
            break;
        default:
            sHandheldShakeMag = 0x0;
            sHandheldShakeInc = 0.f;
    }
}

/**
 * When sHandheldShakeMag is nonzero, this function adds small random offsets to `focus` every time
 * sHandheldShakeTimer increases above 1.0, simulating the camera shake caused by unsteady hands.
 *
 * This function must be called every frame in order to actually apply the effect, since the effect's
 * mag and inc are set to 0 every frame at the end of this function.
 */
void shake_camera_handheld(Vec3f pos, Vec3f focus) {
    s32 i;
    Vec3f shakeOffset;
    Vec3f shakeSpline[4];
    f32 dist;
    s16 pitch, yaw;

    if (sHandheldShakeMag == 0) {
        vec3_zero(shakeOffset);
    } else {
        for (i = 0; i < 4; i++) {
            shakeSpline[i][0] = sHandheldShakeSpline[i].point[0];
            shakeSpline[i][1] = sHandheldShakeSpline[i].point[1];
            shakeSpline[i][2] = sHandheldShakeSpline[i].point[2];
        }
        evaluate_cubic_spline(sHandheldShakeTimer, shakeOffset, shakeSpline[0],
                              shakeSpline[1], shakeSpline[2], shakeSpline[3]);
        if (1.f <= (sHandheldShakeTimer += sHandheldShakeInc)) {
            // The first 3 control points are always (0,0,0), so the random spline is always just a
            // straight line
            for (i = 0; i < 3; i++) {
                vec3s_copy(sHandheldShakeSpline[i].point, sHandheldShakeSpline[i + 1].point);
            }
            random_vec3s(sHandheldShakeSpline[3].point, sHandheldShakeMag, sHandheldShakeMag, sHandheldShakeMag / 2);
            sHandheldShakeTimer -= 1.f;

            // Code dead, this is set to be 0 before it is used.
            sHandheldShakeInc = random_float() * 0.5f;
            if (sHandheldShakeInc < 0.02f) {
                sHandheldShakeInc = 0.02f;
            }
        }
    }

    approach_s16_asymptotic_bool(&sHandheldShakePitch, shakeOffset[0], 0x08);
    approach_s16_asymptotic_bool(&sHandheldShakeYaw, shakeOffset[1], 0x08);
    approach_s16_asymptotic_bool(&sHandheldShakeRoll, shakeOffset[2], 0x08);

    if (sHandheldShakePitch | sHandheldShakeYaw) {
        vec3f_get_dist_and_angle(pos, focus, &dist, &pitch, &yaw);
        pitch += sHandheldShakePitch;
        yaw += sHandheldShakeYaw;
        vec3f_set_dist_and_angle(pos, focus, dist, pitch, yaw);
    }

    // Unless called every frame, the effect will stop after the first time.
    sHandheldShakeMag = 0;
    sHandheldShakeInc = 0.0f;
}

/**
 * Updates C Button input state and stores it in `currentState`
 */
s32 find_c_buttons_pressed(u16 currentState, u16 buttonsPressed, u16 buttonsDown) {
    buttonsPressed &= CBUTTON_MASK;
    buttonsDown &= CBUTTON_MASK;

    if (buttonsPressed & L_CBUTTONS) {
        currentState |= L_CBUTTONS;
        currentState &= ~R_CBUTTONS;
    }
    if (!(buttonsDown & L_CBUTTONS)) {
        currentState &= ~L_CBUTTONS;
    }

    if (buttonsPressed & R_CBUTTONS) {
        currentState |= R_CBUTTONS;
        currentState &= ~L_CBUTTONS;
    }
    if (!(buttonsDown & R_CBUTTONS)) {
        currentState &= ~R_CBUTTONS;
    }

    if (buttonsPressed & U_CBUTTONS) {
        currentState |= U_CBUTTONS;
        currentState &= ~D_CBUTTONS;
    }
    if (!(buttonsDown & U_CBUTTONS)) {
        currentState &= ~U_CBUTTONS;
    }

    if (buttonsPressed & D_CBUTTONS) {
        currentState |= D_CBUTTONS;
        currentState &= ~U_CBUTTONS;
    }
    if (!(buttonsDown & D_CBUTTONS)) {
        currentState &= ~D_CBUTTONS;
    }

    return currentState;
}

/**
 * Determine which icon to show on the HUD
 */
s32 update_camera_hud_status(struct Camera *c) {
    s16 status = CAM_STATUS_NONE;

    if (c->cutscene != CUTSCENE_NONE
        || ((gPlayer1Controller->buttonDown & R_TRIG) && cam_select_alt_mode(0) == CAM_SELECTION_FIXED)) {
        status |= CAM_STATUS_FIXED;
    } else if (set_cam_angle(0) == CAM_ANGLE_MARIO) {
        status |= CAM_STATUS_MARIO;
    } else {
        status |= CAM_STATUS_LAKITU;
    }
    if (gCameraMovementFlags & CAM_MOVE_ZOOMED_OUT) {
        status |= CAM_STATUS_C_DOWN;
    }
    if (gCameraMovementFlags & CAM_MOVE_C_UP_MODE) {
        status |= CAM_STATUS_C_UP;
    }
    set_hud_camera_status(status);
    return status;
}

void play_camera_buzz_if_cdown(void) {
    if (gPlayer1Controller->buttonPressed & D_CBUTTONS) {
        play_sound_button_change_blocked();
    }
}

void play_camera_buzz_if_cbutton(void) {
    if (gPlayer1Controller->buttonPressed & CBUTTON_MASK) {
        play_sound_button_change_blocked();
    }
}

void play_camera_buzz_if_c_sideways(void) {
    if (gPlayer1Controller->buttonPressed & (L_CBUTTONS | R_CBUTTONS)) {
        play_sound_button_change_blocked();
    }
}

void play_sound_cbutton_up(void) {
    play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
}

void play_sound_cbutton_down(void) {
    play_sound(SOUND_MENU_CAMERA_ZOOM_OUT, gGlobalSoundSource);
}

void play_sound_cbutton_side(void) {
    play_sound(SOUND_MENU_CAMERA_TURN, gGlobalSoundSource);
}

void play_sound_button_change_blocked(void) {
    play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
}

void play_sound_rbutton_changed(void) {
    play_sound(SOUND_MENU_CLICK_CHANGE_VIEW, gGlobalSoundSource);
}

void play_sound_if_cam_switched_to_lakitu_or_mario(void) {
    if (sCameraSoundFlags & CAM_SOUND_MARIO_ACTIVE) {
        play_sound_rbutton_changed();
    }
    if (sCameraSoundFlags & CAM_SOUND_NORMAL_ACTIVE) {
        play_sound_rbutton_changed();
    }
    sCameraSoundFlags &= ~(CAM_SOUND_MARIO_ACTIVE | CAM_SOUND_NORMAL_ACTIVE);
}

/**
 * Moves the camera when Mario has triggered a warp
 */
void warp_camera(f32 displacementX, f32 displacementY, f32 displacementZ) {
    Vec3f displacement;
    struct MarioState *marioStates = &gMarioStates[0];
    struct LinearTransitionPoint *start = &sModeInfo.transitionStart;
    struct LinearTransitionPoint *end = &sModeInfo.transitionEnd;

    gCurrLevelArea = gCurrLevelNum * 16 + gCurrentArea->index;
    displacement[0] = displacementX;
    displacement[1] = displacementY;
    displacement[2] = displacementZ;
    vec3f_add(gLakituState.curPos, displacement);
    vec3f_add(gLakituState.curFocus, displacement);
    vec3f_add(gLakituState.goalPos, displacement);
    vec3f_add(gLakituState.goalFocus, displacement);
    marioStates->waterLevel += displacementY;

    vec3f_add(start->focus, displacement);
    vec3f_add(start->pos, displacement);
    vec3f_add(end->focus, displacement);
    vec3f_add(end->pos, displacement);
}

/**
 * Make the camera's y coordinate approach `goal`,
 * unless smooth movement is off, in which case the y coordinate is simply set to `goal`
 */
void approach_camera_height(struct Camera *c, f32 goal, f32 inc) {
    if (sStatusFlags & CAM_FLAG_SMOOTH_MOVEMENT) {
        if (c->pos[1] < goal) {
            if ((c->pos[1] += inc) > goal) {
                c->pos[1] = goal;
            }
        } else {
            if ((c->pos[1] -= inc) < goal) {
                c->pos[1] = goal;
            }
        }
    } else {
        c->pos[1] = goal;
    }
}

/**
 * Set the camera's position to Mario's position, and add several relative offsets. Unused.
 *
 * @param leftRight offset to Mario's left/right, relative to his faceAngle
 * @param yOff y offset
 * @param forwBack offset to Mario's front/back, relative to his faceAngle
 * @param yawOff offset to Mario's faceAngle, changes the direction of `leftRight` and `forwBack`
 */
UNUSED static void unused_set_pos_rel_mario(struct Camera *c, f32 leftRight, f32 yOff, f32 forwBack, s16 yawOff) {
    u16 yaw = sMarioCamState->faceAngle[1] + yawOff;

    c->pos[0] = sMarioCamState->pos[0] + forwBack * sins(yaw) + leftRight * coss(yaw);
    c->pos[1] = sMarioCamState->pos[1] + yOff;
    c->pos[2] = sMarioCamState->pos[2] + forwBack * coss(yaw) - leftRight * sins(yaw);
}

/**
 * Calculate Lakitu's next position and focus, according to gCamera's state,
 * and store them in `newPos` and `newFoc`.
 *
 * @param newPos where Lakitu should fly towards this frame
 * @param newFoc where Lakitu should look towards this frame
 *
 * @param curPos gCamera's pos this frame
 * @param curFoc gCamera's foc this frame
 *
 * @param oldPos gCamera's pos last frame
 * @param oldFoc gCamera's foc last frame
 *
 * @return Lakitu's next yaw, which is the same as the yaw passed in if no transition happened
 */
s16 next_lakitu_state(Vec3f newPos, Vec3f newFoc, Vec3f curPos, Vec3f curFoc,
                      Vec3f oldPos, Vec3f oldFoc, s16 yaw) {
    s16 yawVelocity;
    s16 pitchVelocity;
    f32 distVelocity;
    f32 goalDist;
    s16 goalPitch;
    s16 goalYaw;
    f32 distTimer = sModeTransition.framesLeft;
    s16 angleTimer = sModeTransition.framesLeft;
    Vec3f nextPos;
    Vec3f nextFoc;
    Vec3f startPos;
    Vec3f startFoc;
    s32 i;
    f32 floorHeight;
    struct Surface *floor;

    // If not transitioning, just use gCamera's current pos and foc
    vec3f_copy(newPos, curPos);
    vec3f_copy(newFoc, curFoc);

    if (sStatusFlags & CAM_FLAG_START_TRANSITION) {
        for (i = 0; i < 3; i++) {
            // Add Mario's displacement from this frame to the last frame's pos and focus
            // Makes the transition start from where the camera would have moved
            startPos[i] = oldPos[i] + sMarioCamState->pos[i] - sModeTransition.marioPos[i];
            startFoc[i] = oldFoc[i] + sMarioCamState->pos[i] - sModeTransition.marioPos[i];
        }


        vec3f_get_dist_and_angle(curFoc, startFoc, &sModeTransition.focDist, &sModeTransition.focPitch,
                                 &sModeTransition.focYaw);
        vec3f_get_dist_and_angle(curFoc, startPos, &sModeTransition.posDist, &sModeTransition.posPitch,
                                 &sModeTransition.posYaw);
        sStatusFlags &= ~CAM_FLAG_START_TRANSITION;
    }

    // Transition from the last mode to the current one
    if (sModeTransition.framesLeft > 0) {
        vec3f_get_dist_and_angle(curFoc, curPos, &goalDist, &goalPitch, &goalYaw);
        distVelocity = abss(goalDist - sModeTransition.posDist) / distTimer;
        pitchVelocity = abss(goalPitch - sModeTransition.posPitch) / angleTimer;
        yawVelocity = abss(goalYaw - sModeTransition.posYaw) / angleTimer;

        camera_approach_f32_symmetric_bool(&sModeTransition.posDist, goalDist, distVelocity);
        camera_approach_s16_symmetric_bool(&sModeTransition.posYaw, goalYaw, yawVelocity);
        camera_approach_s16_symmetric_bool(&sModeTransition.posPitch, goalPitch, pitchVelocity);
        vec3f_set_dist_and_angle(curFoc, nextPos, sModeTransition.posDist, sModeTransition.posPitch,
                                 sModeTransition.posYaw);

        vec3f_get_dist_and_angle(curPos, curFoc, &goalDist, &goalPitch, &goalYaw);
        pitchVelocity = sModeTransition.focPitch / (s16) sModeTransition.framesLeft;
        yawVelocity = sModeTransition.focYaw / (s16) sModeTransition.framesLeft;
        distVelocity = sModeTransition.focDist / sModeTransition.framesLeft;

        camera_approach_s16_symmetric_bool(&sModeTransition.focPitch, goalPitch, pitchVelocity);
        camera_approach_s16_symmetric_bool(&sModeTransition.focYaw, goalYaw, yawVelocity);
        camera_approach_f32_symmetric_bool(&sModeTransition.focDist, 0, distVelocity);
        vec3f_set_dist_and_angle(curFoc, nextFoc, sModeTransition.focDist, sModeTransition.focPitch,
                                 sModeTransition.focYaw);

        vec3f_copy(newFoc, nextFoc);
        vec3f_copy(newPos, nextPos);

        if (gCamera->cutscene != 0 || !(gCameraMovementFlags & CAM_MOVE_C_UP_MODE)) {
            floorHeight = find_floor(newPos[0], newPos[1], newPos[2], &floor);
            if (floorHeight != FLOOR_LOWER_LIMIT) {
                if ((floorHeight += 125.f) > newPos[1]) {
                    newPos[1] = floorHeight;
                }
            }
            f32_find_wall_collision(&newPos[0], &newPos[1], &newPos[2], 0.f, 100.f);
        }
        sModeTransition.framesLeft--;
        yaw = calculate_yaw(newFoc, newPos);
    } else {
        sModeTransition.posDist = 0.f;
        sModeTransition.posPitch = 0;
        sModeTransition.posYaw = 0;
        sStatusFlags &= ~CAM_FLAG_TRANSITION_OUT_OF_C_UP;
    }
    vec3f_copy(sModeTransition.marioPos, sMarioCamState->pos);
    return yaw;
}

static UNUSED void stop_transitional_movement(void) {
    sStatusFlags &= ~(CAM_FLAG_START_TRANSITION | CAM_FLAG_TRANSITION_OUT_OF_C_UP);
    sModeTransition.framesLeft = 0;
}

/**
 * Start fixed camera mode, setting the base position to (`x`, `y`, `z`)
 *
 * @return TRUE if the base pos was updated
 */
s32 set_camera_mode_fixed(struct Camera *c, s16 x, s16 y, s16 z) {
    s32 basePosSet = FALSE;
    f32 posX = x;
    f32 posY = y;
    f32 posZ = z;

    if (sFixedModeBasePosition[0] != posX || sFixedModeBasePosition[1] != posY
        || sFixedModeBasePosition[2] != posZ) {
        basePosSet = TRUE;
        sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
    }
    vec3f_set(sFixedModeBasePosition, posX, posY, posZ);
    if (c->mode != CAMERA_MODE_FIXED) {
        sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
        c->mode = CAMERA_MODE_FIXED;
        vec3f_set(c->pos, sFixedModeBasePosition[0], sMarioCamState->pos[1],
                  sFixedModeBasePosition[2]);
    }
    return basePosSet;
}

void set_camera_mode_8_directions(struct Camera *c) {
    if (c->mode != CAMERA_MODE_8_DIRECTIONS) {
        c->mode = CAMERA_MODE_8_DIRECTIONS;
        sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
        s8DirModeBaseYaw = 0;
        s8DirModeYawOffset = 0;
    }
}

/**
 * If the camera mode is not already the boss fight camera (camera with two foci)
 * set it to be so.
 */
void set_camera_mode_boss_fight(struct Camera *c) {
    if (c->mode != CAMERA_MODE_BOSS_FIGHT) {
        transition_to_camera_mode(c, CAMERA_MODE_BOSS_FIGHT, 15);
        sModeOffsetYaw = c->nextYaw - DEGREES(45);
    }
}

void set_camera_mode_close_cam(u8 *mode) {
    if (*mode != CAMERA_MODE_CLOSE) {
        sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
        *mode = CAMERA_MODE_CLOSE;
    }
}

/**
 * Change to radial mode.
 * If the difference in yaw between pos -> Mario and pos > focus is < 90 degrees, transition.
 * Otherwise jump to radial mode.
 */
void set_camera_mode_radial(struct Camera *c, s16 transitionTime) {
    Vec3f focus;
    s16 yaw;

    focus[0] = c->areaCenX;
    focus[1] = sMarioCamState->pos[1];
    focus[2] = c->areaCenZ;
    if (c->mode != CAMERA_MODE_RADIAL) {
        yaw = calculate_yaw(focus, sMarioCamState->pos) - calculate_yaw(c->focus, c->pos) + DEGREES(90);
        if (yaw > 0) {
            transition_to_camera_mode(c, CAMERA_MODE_RADIAL, transitionTime);
        } else {
            c->mode = CAMERA_MODE_RADIAL;
            sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
        }
        sModeOffsetYaw = 0;
    }
}

/**
 * Set the fixed camera base pos depending on the current level area
 */
void set_fixed_cam_axis_sa_lobby(UNUSED s16 preset) {
    switch (gCurrLevelArea) {
        case AREA_SA:
            vec3f_set(sFixedModeBasePosition, 646.f, 143.f, -1513.f);
            break;

        case AREA_CASTLE_LOBBY:
            vec3f_set(sFixedModeBasePosition, -577.f, 143.f, 1443.f);
            break;
    }
}

/**
 * Block area-specific CameraTrigger and special surface modes.
 * Generally, block area mode changes if:
 *      Mario is wearing the metal cap, or at the water's surface, or the camera is in Mario mode
 *
 * However, if the level is WDW, DDD, or COTMC (levels that have metal cap and water):
 *      Only block area mode changes if Mario is in a cannon,
 *      or if the camera is in Mario mode and Mario is not swimming or in water with the metal cap
 */
#ifdef ENABLE_VANILLA_CAM_PROCESSING
void check_blocking_area_processing(const u8 *mode) {
    if (sMarioCamState->action & ACT_FLAG_METAL_WATER ||
                        *mode == DEEP_WATER_CAMERA_MODE || *mode == WATER_SURFACE_CAMERA_MODE) {
        sStatusFlags |= CAM_FLAG_BLOCK_AREA_PROCESSING;
    }

    if (gCurrLevelNum == LEVEL_DDD || gCurrLevelNum == LEVEL_WDW || gCurrLevelNum == LEVEL_COTMC) {
        sStatusFlags &= ~CAM_FLAG_BLOCK_AREA_PROCESSING;
    }

    if ((*mode == DEEP_WATER_CAMERA_MODE &&
            !(sMarioCamState->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER))) ||
         *mode == CAMERA_MODE_INSIDE_CANNON) {
        sStatusFlags |= CAM_FLAG_BLOCK_AREA_PROCESSING;
    }
#else
void check_blocking_area_processing(UNUSED const u8 *mode) {
    sStatusFlags |= CAM_FLAG_BLOCK_AREA_PROCESSING;
#endif
}

/**
 * Apply any modes that are triggered by special floor surface types
 */
u32 surface_type_modes(struct Camera *c) {
    u32 modeChanged = 0;

    switch (sMarioGeometry.currFloorType) {
        case SURFACE_CLOSE_CAMERA:
            transition_to_camera_mode(c, CAMERA_MODE_CLOSE, 90);
            modeChanged++;
            break;

        case SURFACE_CAMERA_FREE_ROAM:
            transition_to_camera_mode(c, CAMERA_MODE_FREE_ROAM, 90);
            modeChanged++;
            break;

        case SURFACE_NO_CAM_COL_SLIPPERY:
            transition_to_camera_mode(c, CAMERA_MODE_CLOSE, 90);
            modeChanged++;
            break;
    }
    return modeChanged;
}

/**
 * Set the camera mode to `mode` if Mario is not standing on a special surface
 */
u32 set_mode_if_not_set_by_surface(struct Camera *c, u8 mode) {
    u32 modeChanged = surface_type_modes(c);

    if ((modeChanged == 0) && (mode != 0)) {
        transition_to_camera_mode(c, mode, 90);
    }

    return modeChanged;
}

/**
 * Used in THI, check if Mario is standing on any of the special surfaces in that area
 */
void surface_type_modes_thi(struct Camera *c) {
    switch (sMarioGeometry.currFloorType) {
        case SURFACE_CLOSE_CAMERA:
            if (c->mode != CAMERA_MODE_CLOSE) {
                transition_to_camera_mode(c, CAMERA_MODE_FREE_ROAM, 90);
            }
            break;

        case SURFACE_CAMERA_FREE_ROAM:
            if (c->mode != CAMERA_MODE_CLOSE) {
                transition_to_camera_mode(c, CAMERA_MODE_FREE_ROAM, 90);
            }
            break;

        case SURFACE_NO_CAM_COL_SLIPPERY:
            if (c->mode != CAMERA_MODE_CLOSE) {
                transition_to_camera_mode(c, CAMERA_MODE_FREE_ROAM, 90);
            }
            break;

        case SURFACE_CAMERA_8_DIR:
            transition_to_camera_mode(c, CAMERA_MODE_8_DIRECTIONS, 90);
            break;

        default:
            transition_to_camera_mode(c, CAMERA_MODE_RADIAL, 90);
    }
}

/**
 * Terminates a list of CameraTriggers.
 */
#define NULL_TRIGGER                                                                                    \
    { 0, NULL, 0, 0, 0, 0, 0, 0, 0 }

/**
 * The SL triggers operate camera behavior in front of the snowman who blows air.
 * The first sets a 8 direction mode, while the latter (which encompasses the former)
 * sets free roam mode.
 *
 * This behavior is exploitable, since the ranges assume that Mario must pass through the latter on
 * exit. Using hyperspeed, the earlier area can be directly exited from, keeping the changes it applies.
 */
struct CameraTrigger sCamSL[] = {
    { 1, cam_sl_snowman_head_8dir, 1119, 3584, 1125, 1177, 358, 358, -0x1D27 },
    // This trigger surrounds the previous one
    { 1, cam_sl_free_roam, 1119, 3584, 1125, 4096, 4096, 4096, -0x1D27 },
    NULL_TRIGGER
};

/**
 * The THI triggers are specifically for the tunnel near the start of the Huge Island.
 * The first helps the camera from getting stuck on the starting side, the latter aligns with the
 * tunnel. Both sides achieve their effect by editing the camera yaw.
 */
struct CameraTrigger sCamTHI[] = {
    { 1, cam_thi_move_cam_through_tunnel, -4609, -2969, 6448, 100, 300, 300, 0 },
    { 1, cam_thi_look_through_tunnel,     -4809, -2969, 6448, 100, 300, 300, 0 },
    NULL_TRIGGER
};

/**
 * The HMC triggers are mostly for warping the camera below platforms, but the second trigger is used to
 * start the cutscene for entering the CotMC pool.
 */
struct CameraTrigger sCamHMC[] = {
    { 1, cam_hmc_enter_maze, 1996, 102, 0, 205, 100, 205, 0 },
    { 1, cam_castle_hmc_start_pool_cutscene, 3350, -4689, 4800, 600, 50, 600, 0 },
    { 1, cam_hmc_elevator_black_hole, -3278, 1236, 1379, 358, 200, 358, 0 },
    { 1, cam_hmc_elevator_maze_emergency_exit, -2816, 2055, -2560, 358, 200, 358, 0 },
    { 1, cam_hmc_elevator_lake, -3532, 1543, -7040, 358, 200, 358, 0 },
    { 1, cam_hmc_elevator_maze, -972, 1543, -7347, 358, 200, 358, 0 },
    NULL_TRIGGER
};

/**
 * The SSL triggers are for starting the enter pyramid top cutscene,
 * setting close mode in the middle of the pyramid, and setting the boss fight camera mode to outward
 * radial.
 */
struct CameraTrigger sCamSSL[] = {
    { 1, cam_ssl_enter_pyramid_top, -2048, 1080, -1024, 150, 150, 150, 0 },
    { 2, cam_ssl_pyramid_center, 0, -104, -104, 1248, 1536, 2950, 0 },
    { 2, cam_ssl_pyramid_center, 0, 2500, 256, 515, 5000, 515, 0 },
    { 3, cam_ssl_boss_room, 0, -1534, -2040, 1000, 800, 1000, 0 },
    NULL_TRIGGER
};

/**
 * The RR triggers are for changing between fixed and 8 direction mode when entering / leaving the building at
 * the end of the ride.
 */
struct CameraTrigger sCamRR[] = {
    { 1, cam_rr_exit_building_side, -4197, 3819, -3087, 1769, 1490, 342, 0 },
    { 1, cam_rr_enter_building_side, -4197, 3819, -3771, 769, 490, 342, 0 },
    { 1, cam_rr_enter_building_window, -5603, 4834, -5209, 300, 600, 591, 0 },
    { 1, cam_rr_enter_building, -2609, 3730, -5463, 300, 650, 577, 0 },
    { 1, cam_rr_exit_building_top, -4196, 7343, -5155, 4500, 1000, 4500, 0 },
    { 1, cam_rr_enter_building, -4196, 6043, -5155, 500, 300, 500, 0 },
    NULL_TRIGGER,
};

/**
 * These triggers are unused, but because the first trigger surrounds the BoB tower and activates radial
 * mode (which is called "tower mode" in the patent), it's speculated they belonged to BoB.
 *
 * This table contains the only instance of a CameraTrigger with an area set to -1, and it sets the mode
 * to free_roam when Mario is not walking up the tower.
 */
struct CameraTrigger sCamBOB[] = {
    {  1, cam_bob_tower, 2468, 2720, -4608, 3263, 1696, 3072, 0 },
    { -1, cam_bob_default_free_roam, 0, 0, 0, 0, 0, 0, 0 },
    NULL_TRIGGER
};

/**
 * The CotMC trigger is only used to prevent fix Lakitu in place when Mario exits through the waterfall.
 */
struct CameraTrigger sCamCotMC[] = {
    { 1, cam_cotmc_exit_waterfall, 0, 1500, 3500, 550, 10000, 1500, 0 },
    NULL_TRIGGER
};

/**
 * The CCM triggers are used to set the flag that says when Mario is in the slide shortcut.
 */
struct CameraTrigger sCamCCM[] = {
    { 2, cam_ccm_enter_slide_shortcut, -4846, 2061, 27, 1229, 1342, 396, 0 },
    { 2, cam_ccm_leave_slide_shortcut, -6412, -3917, -6246, 307, 185, 132, 0 },
    NULL_TRIGGER
};

/**
 * The Castle triggers are used to set the camera to fixed mode when entering the lobby, and to set it
 * to close mode when leaving it. They also set the mode to spiral staircase.
 *
 * There are two triggers for looking up and down straight staircases when Mario is at the start,
 * and one trigger that starts the enter pool cutscene when Mario enters HMC.
 */
struct CameraTrigger sCamCastle[] = {
    { 1, cam_castle_close_mode, -1100, 657, -1346, 300, 150, 300, 0 },
    { 1, cam_castle_enter_lobby, -1099, 657, -803, 300, 150, 300, 0 },
    { 1, cam_castle_close_mode, -2304, -264, -4072, 140, 150, 140, 0 },
    { 1, cam_castle_close_mode, -2304, 145, -1344, 140, 150, 140, 0 },
    { 1, cam_castle_enter_lobby, -2304, 145, -802, 140, 150, 140, 0 },
    //! Sets the camera mode when leaving secret aquarium
    { 1, cam_castle_close_mode, 2816, 1200, -256, 100, 100, 100, 0 },
    { 1, cam_castle_close_mode, 256, -161, -4226, 140, 150, 140, 0 },
    { 1, cam_castle_close_mode, 256, 145, -1344, 140, 150, 140, 0 },
    { 1, cam_castle_enter_lobby, 256, 145, -802, 140, 150, 140, 0 },
    { 1, cam_castle_close_mode, -1023, 44, -4870, 140, 150, 140, 0 },
    { 1, cam_castle_close_mode, -459, 145, -1020, 140, 150, 140, 0x6000 },
    { 1, cam_castle_enter_lobby, -85, 145, -627, 140, 150, 140, 0 },
    { 1, cam_castle_close_mode, -1589, 145, -1020, 140, 150, 140, -0x6000 },
    { 1, cam_castle_enter_lobby, -1963, 145, -627, 140, 150, 140, 0 },
    { 1, cam_castle_leave_lobby_sliding_door, -2838, 657, -1659, 200, 150, 150, 0x2000 },
    { 1, cam_castle_enter_lobby_sliding_door, -2319, 512, -1266, 300, 150, 300, 0x2000 },
    { 1, cam_castle_close_mode, 844, 759, -1657, 40, 150, 40, -0x2000 },
    { 1, cam_castle_enter_lobby, 442, 759, -1292, 140, 150, 140, -0x2000 },
    { 2, cam_castle_enter_spiral_stairs, -1000, 657, 1740, 200, 300, 200, 0 },
    { 2, cam_castle_enter_spiral_stairs, -996, 1348, 1814, 200, 300, 200, 0 },
    { 2, cam_castle_close_mode, -946, 657, 2721, 50, 150, 50, 0 },
    { 2, cam_castle_close_mode, -996, 1348, 907, 50, 150, 50, 0 },
    { 2, cam_castle_close_mode, -997, 1348, 1450, 140, 150, 140, 0 },
    { 1, cam_castle_close_mode, -4942, 452, -461, 140, 150, 140, 0x4000 },
    { 1, cam_castle_close_mode, -3393, 350, -793, 140, 150, 140, 0x4000 },
    { 1, cam_castle_enter_lobby, -2851, 350, -792, 140, 150, 140, 0x4000 },
    { 1, cam_castle_enter_lobby, 803, 350, -228, 140, 150, 140, -0x4000 },
    //! Duplicate camera trigger outside JRB door
    { 1, cam_castle_enter_lobby, 803, 350, -228, 140, 150, 140, -0x4000 },
    { 1, cam_castle_close_mode, 1345, 350, -229, 140, 150, 140, 0x4000 },
    { 1, cam_castle_close_mode, -946, -929, 622, 300, 150, 300, 0 },
    { 2, cam_castle_look_upstairs, -205, 1456, 2508, 210, 928, 718, 0 },
    { 1, cam_castle_basement_look_downstairs, -1027, -587, -718, 318, 486, 577, 0 },
    { 1, cam_castle_lobby_entrance, -1023, 376, 1830, 300, 400, 300, 0 },
    { 3, cam_castle_hmc_start_pool_cutscene, 2485, -1689, -2659, 600, 50, 600, 0 },
    NULL_TRIGGER
};

/**
 * The BBH triggers are the most complex, they cause the camera to enter fixed mode for each room,
 * transition between rooms, and enter free roam when outside.
 *
 * The triggers are also responsible for warping the camera below platforms.
 */
struct CameraTrigger sCamBBH[] = {
    { 1, cam_bbh_enter_front_door, 742, 0, 2369, 200, 200, 200, 0 },
    { 1, cam_bbh_leave_front_door, 741, 0, 1827, 200, 200, 200, 0 },
    { 1, cam_bbh_room_1, 222, 0, 1458, 200, 200, 200, 0 },
    { 1, cam_bbh_room_1, 222, 0, 639, 200, 200, 200, 0 },
    { 1, cam_bbh_room_1, 435, 0, 222, 200, 200, 200, 0 },
    { 1, cam_bbh_room_1, 1613, 0, 222, 200, 200, 200, 0 },
    { 1, cam_bbh_room_1, 1827, 0, 1459, 200, 200, 200, 0 },
    { 1, cam_bbh_room_1, -495, 819, 1407, 200, 200, 200, 0 },
    { 1, cam_bbh_room_1, -495, 819, 640, 250, 200, 200, 0 },
    { 1, cam_bbh_room_1, 179, 819, 222, 200, 200, 200, 0 },
    { 1, cam_bbh_room_1, 1613, 819, 222, 200, 200, 200, 0 },
    { 1, cam_bbh_room_1, 1827, 819, 486, 200, 200, 200, 0 },
    { 1, cam_bbh_room_1, 1827, 819, 1818, 200, 200, 200, 0 },
    { 1, cam_bbh_room_2_lower, 2369, 0, 1459, 200, 200, 200, 0 },
    { 1, cam_bbh_room_2_lower, 3354, 0, 1347, 200, 200, 200, 0 },
    { 1, cam_bbh_room_2_lower, 2867, 514, 1843, 512, 102, 409, 0 },
    { 1, cam_bbh_room_4, 3354, 0, 804, 200, 200, 200, 0 },
    { 1, cam_bbh_room_4, 1613, 0, -320, 200, 200, 200, 0 },
    { 1, cam_bbh_room_8, 435, 0, -320, 200, 200, 200, 0 },
    { 1, cam_bbh_room_5_library, -2021, 0, 803, 200, 200, 200, 0 },
    { 1, cam_bbh_room_5_library, -320, 0, 640, 200, 200, 200, 0 },
    { 1, cam_bbh_room_5_library_to_hidden_transition, -1536, 358, -254, 716, 363, 102, 0 },
    { 1, cam_bbh_room_5_hidden_to_library_transition, -1536, 358, -459, 716, 363, 102, 0 },
    { 1, cam_bbh_room_5_hidden, -1560, 0, -1314, 200, 200, 200, 0 },
    { 1, cam_bbh_room_3, -320, 0, 1459, 200, 200, 200, 0 },
    { 1, cam_bbh_room_3, -2021, 0, 1345, 200, 200, 200, 0 },
    { 1, cam_bbh_room_2_library, 2369, 819, 486, 200, 200, 200, 0 },
    { 1, cam_bbh_room_2_library, 2369, 1741, 486, 200, 200, 200, 0 },
    { 1, cam_bbh_room_2_library_to_trapdoor_transition, 2867, 1228, 1174, 716, 414, 102, 0 },
    { 1, cam_bbh_room_2_trapdoor_transition, 2867, 1228, 1378, 716, 414, 102, 0 },
    { 1, cam_bbh_room_2_trapdoor, 2369, 819, 1818, 200, 200, 200, 0 },
    { 1, cam_bbh_room_9_attic, 1829, 1741, 486, 200, 200, 200, 0 },
    { 1, cam_bbh_room_9_attic, 741, 1741, 1587, 200, 200, 200, 0 },
    { 1, cam_bbh_room_9_attic_transition, 102, 2048, -191, 100, 310, 307, 0 },
    { 1, cam_bbh_room_9_mr_i_transition, 409, 2048, -191, 100, 310, 307, 0 },
    { 1, cam_bbh_room_13_balcony, 742, 1922, 2164, 200, 200, 200, 0 },
    { 1, cam_bbh_fall_off_roof, 587, 1322, 2677, 1000, 400, 600, 0 },
    { 1, cam_bbh_room_3, -1037, 819, 1408, 200, 200, 200, 0 },
    { 1, cam_bbh_room_3, -1970, 1024, 1345, 200, 200, 200, 0 },
    { 1, cam_bbh_room_8, 179, 819, -320, 200, 200, 200, 0 },
    { 1, cam_bbh_room_7_mr_i, 1613, 819, -320, 200, 200, 200, 0 },
    { 1, cam_bbh_room_7_mr_i_to_coffins_transition, 2099, 1228, -819, 102, 414, 716, 0 },
    { 1, cam_bbh_room_7_coffins_to_mr_i_transition, 2304, 1228, -819, 102, 414, 716, 0 },
    { 1, cam_bbh_room_6, -1037, 819, 640, 200, 200, 200, 0 },
    { 1, cam_bbh_room_6, -1970, 1024, 803, 200, 200, 200, 0 },
    { 1, cam_bbh_room_1, 1827, 819, 1818, 200, 200, 200, 0 },
    { 1, cam_bbh_fall_into_pool, 2355, -1112, -193, 1228, 500, 1343, 0 },
    { 1, cam_bbh_fall_into_pool, 2355, -1727, 1410, 1228, 500, 705, 0 },
    { 1, cam_bbh_elevator_room_lower, 0, -2457, 1827, 250, 200, 250, 0 },
    { 1, cam_bbh_elevator_room_lower, 0, -2457, 2369, 250, 200, 250, 0 },
    { 1, cam_bbh_elevator_room_lower, 0, -2457, 4929, 250, 200, 250, 0 },
    { 1, cam_bbh_elevator_room_lower, 0, -2457, 4387, 250, 200, 250, 0 },
    { 1, cam_bbh_room_0_back_entrance, 1887, -2457, 204, 250, 200, 250, 0 },
    { 1, cam_bbh_room_0, 1272, -2457, 204, 250, 200, 250, 0 },
    { 1, cam_bbh_room_0, -1681, -2457, 204, 250, 200, 250, 0 },
    { 1, cam_bbh_room_0_back_entrance, -2296, -2457, 204, 250, 200, 250, 0 },
    { 1, cam_bbh_elevator, -2939, -605, 5367, 800, 100, 800, 0 },
    { 1, cam_bbh_room_12_upper, -2939, -205, 5367, 300, 100, 300, 0 },
    { 1, cam_bbh_room_12_upper, -2332, -204, 4714, 250, 200, 250, 0x6000 },
    { 1, cam_bbh_room_0_back_entrance, -1939, -204, 4340, 250, 200, 250, 0x6000 },
    NULL_TRIGGER
};

#define _ NULL
#define STUB_LEVEL(_0, _1, _2, _3, _4, _5, _6, _7, cameratable) cameratable,
#define DEFINE_LEVEL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, cameratable) cameratable,

/*
 * This table has an extra 2 levels after the last unknown_38 stub level. What I think
 * the programmer was thinking was that the table is null terminated and so used the
 * level count as a correspondence to the ID of the final level, but the enum represents
 * an ID *after* the last stub level, not before or during it.
 *
 * Each table is terminated with NULL_TRIGGER
 */
struct CameraTrigger *sCameraTriggers[LEVEL_COUNT + 1] = {
    NULL,
    #include "levels/level_defines.h"
};
#undef _
#undef STUB_LEVEL
#undef DEFINE_LEVEL

void area_specific_camera_processing(struct Camera *c) {
    // Area-specific camera processing
    if (!(sStatusFlags & CAM_FLAG_BLOCK_AREA_PROCESSING)) {
        switch (gCurrLevelArea) {
            case AREA_WF:
                if (sMarioCamState->action == ACT_RIDING_HOOT) {
                    transition_to_camera_mode(c, CAMERA_MODE_SLIDE_HOOT, 60);
                } else {
                    switch (sMarioGeometry.currFloorType) {
                        case SURFACE_CAMERA_8_DIR:
                            transition_to_camera_mode(c, CAMERA_MODE_8_DIRECTIONS, 90);
                            s8DirModeBaseYaw = DEGREES(90);
                            break;

                        case SURFACE_BOSS_FIGHT_CAMERA:
                            if (gCurrActNum == 1) {
                                set_camera_mode_boss_fight(c);
                            } else {
                                set_camera_mode_radial(c, 60);
                            }
                            break;
                        default:
                            set_camera_mode_radial(c, 60);
                    }
                }
                break;

            case AREA_BBH:
                // if camera is fixed at bbh_room_13_balcony_camera (but as floats)
                if (vec3f_compare(sFixedModeBasePosition, 210.f, 420.f, 3109.f) == TRUE) {
                    if (sMarioCamState->pos[1] < 1800.f) {
                        transition_to_camera_mode(c, CAMERA_MODE_CLOSE, 30);
                    }
                }
                break;

            case AREA_SSL_PYRAMID:
                set_mode_if_not_set_by_surface(c, CAMERA_MODE_OUTWARD_RADIAL);
                break;

            case AREA_SSL_OUTSIDE:
                set_mode_if_not_set_by_surface(c, CAMERA_MODE_RADIAL);
                break;

            case AREA_THI_HUGE:
                break;

            case AREA_THI_TINY:
                surface_type_modes_thi(c);
                break;

            case AREA_TTC:
                set_mode_if_not_set_by_surface(c, CAMERA_MODE_OUTWARD_RADIAL);
                break;

            case AREA_BOB:
                if (set_mode_if_not_set_by_surface(c, CAMERA_MODE_NONE) == 0) {
                    if (sMarioGeometry.currFloorType == SURFACE_BOSS_FIGHT_CAMERA) {
                        set_camera_mode_boss_fight(c);
                    } else {
                        if (c->mode == CAMERA_MODE_CLOSE) {
                            transition_to_camera_mode(c, CAMERA_MODE_RADIAL, 60);
                        } else {
                            set_camera_mode_radial(c, 60);
                        }
                    }
                }
                break;

            case AREA_WDW_MAIN:
                switch (sMarioGeometry.currFloorType) {
                    case SURFACE_INSTANT_WARP_1B:
                        c->defMode = CAMERA_MODE_RADIAL;
                        break;
                }
                break;

            case AREA_WDW_TOWN:
                switch (sMarioGeometry.currFloorType) {
                    case SURFACE_INSTANT_WARP_1C:
                        c->defMode = CAMERA_MODE_CLOSE;
                        break;
                }
                break;

            case AREA_DDD_WHIRLPOOL:
                //! @bug this does nothing
                gLakituState.defMode = CAMERA_MODE_OUTWARD_RADIAL;
                break;

            case AREA_DDD_SUB:
                if ((c->mode != CAMERA_MODE_BEHIND_MARIO)
                    && (c->mode != CAMERA_MODE_WATER_SURFACE)) {
                    if (((sMarioCamState->action & ACT_FLAG_ON_POLE) != 0)
                        || (sMarioGeometry.currFloorHeight > 800.f)) {
                        transition_to_camera_mode(c, CAMERA_MODE_8_DIRECTIONS, 60);

                    } else {
                        if (sMarioCamState->pos[1] < 800.f) {
                            transition_to_camera_mode(c, CAMERA_MODE_FREE_ROAM, 60);
                        }
                    }
                }
                //! @bug this does nothing
                gLakituState.defMode = CAMERA_MODE_FREE_ROAM;
                break;
        }
    }
}

/**
 * Activates any CameraTriggers that Mario is inside.
 * Then, applies area-specific processing to the camera, such as setting the default mode, or changing
 * the mode based on the terrain type Mario is standing on.
 *
 * @return the camera's mode after processing, although this is unused in the code
 */
s16 camera_course_processing(struct Camera *c) {
    s16 level = gCurrLevelNum;
    s8 area = gCurrentArea->index;
    // Bounds iterator
    u32 b;
    // Camera trigger's bounding box
    Vec3f center, bounds;
    u32 insideBounds = FALSE;
    u8 oldMode = c->mode;

    if (c->mode == CAMERA_MODE_C_UP) {
        c->mode = sModeInfo.lastMode;
    }
    check_blocking_area_processing(&c->mode);
    if (level > LEVEL_COUNT + 1) {
        level = LEVEL_COUNT + 1;
    }

    if (sCameraTriggers[level] != NULL) {
        b = 0;

        // Process positional triggers.
        // All triggered events are called, not just the first one.
        while (sCameraTriggers[level][b].event != NULL) {

            // Check only the current area's triggers
            if (sCameraTriggers[level][b].area == area) {
                // Copy the bounding box into center and bounds
                vec3f_set(center, sCameraTriggers[level][b].centerX,
                                  sCameraTriggers[level][b].centerY,
                                  sCameraTriggers[level][b].centerZ);
                vec3f_set(bounds, sCameraTriggers[level][b].boundsX,
                                  sCameraTriggers[level][b].boundsY,
                                  sCameraTriggers[level][b].boundsZ);

                // Check if Mario is inside the bounds
                if (is_pos_in_bounds(sMarioCamState->pos, center, bounds,
                                                   sCameraTriggers[level][b].boundsYaw) == TRUE) {
                    //! This should be checked before calling is_pos_in_bounds. (It doesn't belong
                    //! outside the while loop because some events disable area processing)
                    if (!(sStatusFlags & CAM_FLAG_BLOCK_AREA_PROCESSING)) {
                        sCameraTriggers[level][b].event(c);
                        insideBounds = TRUE;
                    }
                }
            }

            if ((sCameraTriggers[level])[b].area == -1) {
                // Default triggers are only active if Mario is not already inside another trigger
                if (!insideBounds) {
                    if (!(sStatusFlags & CAM_FLAG_BLOCK_AREA_PROCESSING)) {
                        sCameraTriggers[level][b].event(c);
                    }
                }
            }

            b++;
        }
    }
#if defined(ENABLE_VANILLA_CAM_PROCESSING) && !defined(FORCED_CAMERA_MODE) && !defined(USE_COURSE_DEFAULT_MODE)
    area_specific_camera_processing(c);
#endif

    sStatusFlags &= ~CAM_FLAG_BLOCK_AREA_PROCESSING;
    if (oldMode == CAMERA_MODE_C_UP) {
        sModeInfo.lastMode = c->mode;
        c->mode = oldMode;
    }
    return c->mode;
}

/**
 * Stores type and height of the nearest floor and ceiling to Mario in `pg`
 *
 * Note: Also finds the water level, but waterHeight is unused
 */
void find_mario_floor_and_ceil(struct PlayerGeometry *pg) {
    struct Surface *surf;
    s32 tempCollisionFlags = gCollisionFlags;
    gCollisionFlags |= COLLISION_FLAG_CAMERA;

    if (find_floor(sMarioCamState->pos[0], sMarioCamState->pos[1] + 10.f,
                   sMarioCamState->pos[2], &surf) != FLOOR_LOWER_LIMIT) {
        pg->currFloorType = surf->type;
    } else {
        pg->currFloorType = 0;
    }

    if (find_ceil(sMarioCamState->pos[0], sMarioCamState->pos[1] - 10.f,
                  sMarioCamState->pos[2], &surf) != CELL_HEIGHT_LIMIT) {
        pg->currCeilType = surf->type;
    } else {
        pg->currCeilType = 0;
    }

    gCollisionFlags &= ~COLLISION_FLAG_CAMERA;
    pg->currFloorHeight = find_floor(sMarioCamState->pos[0],
                                     sMarioCamState->pos[1] + 10.f,
                                     sMarioCamState->pos[2], &pg->currFloor);
    pg->currCeilHeight = find_ceil(sMarioCamState->pos[0],
                                   sMarioCamState->pos[1] - 10.f,
                                   sMarioCamState->pos[2], &pg->currCeil);
    pg->waterHeight = find_water_level(sMarioCamState->pos[0], sMarioCamState->pos[2]);
    gCollisionFlags = tempCollisionFlags;
}

/**
 * Rotate the camera's focus around the camera's position by incYaw and incPitch
 */
void pan_camera(struct Camera *c, s16 incPitch, s16 incYaw) {
    f32 distCamToFocus;
    s16 pitch, yaw;

    vec3f_get_dist_and_angle(c->pos, c->focus, &distCamToFocus, &pitch, &yaw);
    pitch += incPitch;
    yaw += incYaw;
    vec3f_set_dist_and_angle(c->pos, c->focus, distCamToFocus, pitch, yaw);
}


static UNUSED void unused_start_bowser_bounce_shake(UNUSED struct Camera *c) {
    set_environmental_camera_shake(SHAKE_ENV_BOWSER_THROW_BOUNCE);
}

/* TODO:
 * The next two arrays are both related to levels, and they look generated.
 * These should be split into their own file.
 */

/**
 * These masks set whether or not the camera zooms out when game is paused.
 *
 * Each entry is used by two levels. Even levels use the low 4 bits, odd levels use the high 4 bits
 * Because areas are 1-indexed, a mask of 0x1 will make area 1 (not area 0) zoom out.
 *
 * In zoom_out_if_paused_and_outside(), the current area is converted to a shift.
 * Then the value of (1 << shift) is &'d with the level's mask,
 * and if the result is non-zero, the camera will zoom out.
 */
u8 sZoomOutAreaMasks[] = {
    ZOOMOUT_AREA_MASK(0,0,0,0, 0,0,0,0), // Unused         | Unused
    ZOOMOUT_AREA_MASK(0,0,0,0, 0,0,0,0), // Unused         | Unused
    ZOOMOUT_AREA_MASK(0,0,0,0, 1,0,0,0), // BBH            | CCM
    ZOOMOUT_AREA_MASK(0,0,0,0, 0,0,0,0), // CASTLE_INSIDE  | HMC
    ZOOMOUT_AREA_MASK(1,0,0,0, 1,0,0,0), // SSL            | BOB
    ZOOMOUT_AREA_MASK(1,0,0,0, 1,0,0,0), // SL             | WDW
    ZOOMOUT_AREA_MASK(0,0,0,0, 1,1,0,0), // JRB            | THI
    ZOOMOUT_AREA_MASK(0,0,0,0, 1,0,0,0), // TTC            | RR
    ZOOMOUT_AREA_MASK(1,0,0,0, 1,0,0,0), // CASTLE_GROUNDS | BITDW
    ZOOMOUT_AREA_MASK(0,0,0,0, 1,0,0,0), // VCUTM          | BITFS
    ZOOMOUT_AREA_MASK(0,0,0,0, 1,0,0,0), // SA             | BITS
    ZOOMOUT_AREA_MASK(1,0,0,0, 0,0,0,0), // LLL            | DDD
    ZOOMOUT_AREA_MASK(1,0,0,0, 0,0,0,0), // WF             | ENDING
    ZOOMOUT_AREA_MASK(0,0,0,0, 0,0,0,0), // COURTYARD      | PSS
    ZOOMOUT_AREA_MASK(0,0,0,0, 1,0,0,0), // COTMC          | TOTWC
    ZOOMOUT_AREA_MASK(1,0,0,0, 1,0,0,0), // BOWSER_1       | WMOTR
    ZOOMOUT_AREA_MASK(0,0,0,0, 1,0,0,0), // Unused         | BOWSER_2
    ZOOMOUT_AREA_MASK(1,0,0,0, 0,0,0,0), // BOWSER_3       | Unused
    ZOOMOUT_AREA_MASK(1,0,0,0, 0,0,0,0), // TTM            | Unused
    ZOOMOUT_AREA_MASK(0,0,0,0, 0,0,0,0), // Unused         | Unused
};

STATIC_ASSERT(ARRAY_COUNT(sZoomOutAreaMasks) - 1 == LEVEL_MAX / 2, "Make sure you edit sZoomOutAreaMasks when adding / removing courses.");

/**
 * Start shaking the camera's field of view.
 *
 * @param shakeSpeed How fast the shake should progress through its period. The shake offset is
 *                   calculated from coss(), so this parameter can be thought of as an angular velocity.
 */
void set_fov_shake(s16 amplitude, s16 decay, s16 shakeSpeed) {
    if (amplitude > sFOVState.shakeAmplitude) {
        sFOVState.shakeAmplitude = amplitude;
        sFOVState.decay = decay;
        sFOVState.shakeSpeed = shakeSpeed;
    }
}

/**
 * Start shaking the camera's field of view, but reduce `amplitude` by distance from camera
 */
void set_fov_shake_from_point(s16 amplitude, s16 decay, s16 shakeSpeed, f32 maxDist, f32 posX, f32 posY, f32 posZ) {
    amplitude = reduce_by_dist_from_camera(amplitude, maxDist, posX, posY, posZ);

    if (amplitude != 0) {
        set_fov_shake(amplitude, decay, shakeSpeed);
    }
}

static UNUSED void unused_deactivate_sleeping_camera(UNUSED struct MarioState *m) {
    sStatusFlags &= ~CAM_FLAG_SLEEPING;
}

/**
 * Change the camera's FOV mode.
 *
 * @see geo_camera_fov
 */
void set_fov_function(u8 func) {
    sFOVState.fovFunc = func;
}

/**
 * Start a preset fov shake that is reduced by the point's distance from the camera.
 * Used in set_camera_shake_from_point
 *
 * @see set_camera_shake_from_point
 */
void set_fov_shake_from_point_preset(u8 preset, f32 posX, f32 posY, f32 posZ) {
    switch (preset) {
        case SHAKE_FOV_SMALL:
            set_fov_shake_from_point(0x100, 0x30, 0x8000, 3000.f, posX, posY, posZ);
            break;
        case SHAKE_FOV_MEDIUM:
            set_fov_shake_from_point(0x200, 0x30, 0x8000, 4000.f, posX, posY, posZ);
            break;
        case SHAKE_FOV_LARGE:
            set_fov_shake_from_point(0x300, 0x30, 0x8000, 6000.f, posX, posY, posZ);
            break;
        case SHAKE_FOV_UNUSED:
            set_fov_shake_from_point(0x800, 0x20, 0x4000, 3000.f, posX, posY, posZ);
            break;
    }
}

/**
 * Offset an object's position in a random direction within the given bounds.
 */
static UNUSED void unused_displace_obj_randomly(struct Object *obj, f32 xRange, f32 yRange, f32 zRange) {
    f32 rnd = random_float();

    obj->oPosX += (rnd * xRange - xRange / 2.f);
    obj->oPosY += (rnd * yRange - yRange / 2.f);
    obj->oPosZ += (rnd * zRange - zRange / 2.f);
}

/**
 * Rotate an object in a random direction within the given bounds.
 */
static UNUSED void unused_rotate_obj_randomly(struct Object *obj, f32 pitchRange, f32 yawRange) {
    f32 rnd = random_float();

    obj->oMoveAnglePitch += (s16)(rnd * pitchRange - pitchRange / 2.f);
    obj->oMoveAngleYaw   += (s16)(rnd *   yawRange -   yawRange / 2.f);
}

/**
 * Rotate the object towards the point `point`.
 */
void obj_rotate_towards_point(struct Object *obj, Vec3f point, s16 pitchOff, s16 yawOff, s16 pitchDiv, s16 yawDiv) {
    f32 dist;
    s16 pitch, yaw;
    Vec3f oPos;

    object_pos_to_vec3f(oPos, obj);
    vec3f_get_dist_and_angle(oPos, point, &dist, &pitch, &yaw);
    obj->oMoveAnglePitch = approach_s16_asymptotic(obj->oMoveAnglePitch, pitchOff - pitch, pitchDiv);
    obj->oMoveAngleYaw = approach_s16_asymptotic(obj->oMoveAngleYaw, yaw + yawOff, yawDiv);
}

/**
 * The main camera update function.
 * Gets controller input, checks for cutscenes, handles mode changes, and moves the camera
 */
void update_camera(struct Camera *c) {
    PROFILER_GET_SNAPSHOT_TYPE(PROFILER_DELTA_COLLISION);
    gCamera = c;
    update_camera_hud_status(c);
    if (c->cutscene == CUTSCENE_NONE
#ifdef PUPPYCAM
        && !gPuppyCam.enabled
#endif
        && gCurrentArea->camera->mode != CAMERA_MODE_INSIDE_CANNON) {
        // Only process R_TRIG if 'fixed' is not selected in the menu
        if (cam_select_alt_mode(CAM_SELECTION_NONE) == CAM_SELECTION_MARIO) {
            if (gPlayer1Controller->buttonPressed & R_TRIG) {
                if (set_cam_angle(0) == CAM_ANGLE_LAKITU) {
                    set_cam_angle(CAM_ANGLE_MARIO);
                } else {
                    set_cam_angle(CAM_ANGLE_LAKITU);
                }
            }
        }
        play_sound_if_cam_switched_to_lakitu_or_mario();
    }

    // Initialize the camera
    sStatusFlags &= ~CAM_FLAG_FRAME_AFTER_CAM_INIT;
    if (gCameraMovementFlags & CAM_MOVE_INIT_CAMERA) {
        init_camera(c);
        gCameraMovementFlags &= ~CAM_MOVE_INIT_CAMERA;
        sStatusFlags |= CAM_FLAG_FRAME_AFTER_CAM_INIT;
    }

#ifdef PUPPYCAM
    if (!gPuppyCam.enabled || c->cutscene != CUTSCENE_NONE || gCurrentArea->camera->mode == CAMERA_MODE_INSIDE_CANNON) {
#endif
    // Store previous geometry information
    sMarioGeometry.prevFloorHeight = sMarioGeometry.currFloorHeight;
    sMarioGeometry.prevCeilHeight = sMarioGeometry.currCeilHeight;
    sMarioGeometry.prevFloor = sMarioGeometry.currFloor;
    sMarioGeometry.prevCeil = sMarioGeometry.currCeil;
    sMarioGeometry.prevFloorType = sMarioGeometry.currFloorType;
    sMarioGeometry.prevCeilType = sMarioGeometry.currCeilType;

    find_mario_floor_and_ceil(&sMarioGeometry);
    gCollisionFlags |= COLLISION_FLAG_CAMERA;
    vec3f_copy(c->pos, gLakituState.goalPos);
    vec3f_copy(c->focus, gLakituState.goalFocus);

    c->yaw = gLakituState.yaw;
    c->nextYaw = gLakituState.nextYaw;
    c->mode = gLakituState.mode;
    c->defMode = gLakituState.defMode;
#ifdef ENABLE_VANILLA_CAM_PROCESSING
    camera_course_processing(c);
#else
    if (gCurrDemoInput != NULL) camera_course_processing(c);
#endif
    sCButtonsPressed = find_c_buttons_pressed(sCButtonsPressed, gPlayer1Controller->buttonPressed, gPlayer1Controller->buttonDown);

    if (c->cutscene != CUTSCENE_NONE) {
        sYawSpeed = 0;
        play_cutscene(c);
        sFramesSinceCutsceneEnded = 0;
    } else {
        // Clear the recent cutscene after 8 frames
        if (gRecentCutscene != CUTSCENE_NONE && sFramesSinceCutsceneEnded < 8) {
            sFramesSinceCutsceneEnded++;
            if (sFramesSinceCutsceneEnded >= 8) {
                gRecentCutscene = CUTSCENE_NONE;
                sFramesSinceCutsceneEnded = 0;
            }
        }
    }
    // If not in a cutscene, do mode processing
    if (c->cutscene == CUTSCENE_NONE) {
        sYawSpeed = 0x400;

        if (sSelectionFlags & CAM_MODE_MARIO_ACTIVE) {
            switch (c->mode) {
                case CAMERA_MODE_BEHIND_MARIO:
                    mode_behind_mario_camera(c);
                    break;

                case CAMERA_MODE_C_UP:
                    mode_c_up_camera(c);
                    break;

                case CAMERA_MODE_WATER_SURFACE:
                    mode_water_surface_camera(c);
                    break;

                case CAMERA_MODE_INSIDE_CANNON:
                    mode_cannon_camera(c);
                    break;

                default:
                    mode_mario_camera(c);
            }
        } else {
            switch (c->mode) {
                case CAMERA_MODE_BEHIND_MARIO:
                    mode_behind_mario_camera(c);
                    break;

                case CAMERA_MODE_C_UP:
                    mode_c_up_camera(c);
                    break;

                case CAMERA_MODE_WATER_SURFACE:
                    mode_water_surface_camera(c);
                    break;

                case CAMERA_MODE_INSIDE_CANNON:
                    mode_cannon_camera(c);
                    break;

                case CAMERA_MODE_8_DIRECTIONS:
                    mode_8_directions_camera(c);
                    break;

                case CAMERA_MODE_RADIAL:
                    mode_radial_camera(c);
                    break;

                case CAMERA_MODE_OUTWARD_RADIAL:
                    mode_outward_radial_camera(c);
                    break;

                case CAMERA_MODE_CLOSE:
                    mode_lakitu_camera(c);
                    break;

                case CAMERA_MODE_FREE_ROAM:
                    mode_lakitu_camera(c);
                    break;

                case CAMERA_MODE_BOSS_FIGHT:
                    mode_boss_fight_camera(c);
                    break;

                case CAMERA_MODE_PARALLEL_TRACKING:
                    mode_parallel_tracking_camera(c);
                    break;

                case CAMERA_MODE_SLIDE_HOOT:
                    mode_slide_camera(c);
                    break;

                case CAMERA_MODE_FIXED:
                    mode_fixed_camera(c);
                    break;

                case CAMERA_MODE_SPIRAL_STAIRS:
                    mode_spiral_stairs_camera(c);
                    break;
            }
        }
    }
#ifdef PUPPYCAM
    }
#endif
    // Start any Mario-related cutscenes
    start_cutscene(c, get_cutscene_from_mario_status(c));
    gCollisionFlags &= ~COLLISION_FLAG_CAMERA;
#ifdef PUPPYCAM
    if (!gPuppyCam.enabled || c->cutscene != 0 || gCurrentArea->camera->mode == CAMERA_MODE_INSIDE_CANNON) {
#endif
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    if (gCurrLevelNum != LEVEL_CASTLE) {
#endif
        // If fixed camera is selected as the alternate mode, then fix the camera as long as the right
        // trigger is held
        if ((c->cutscene == CUTSCENE_NONE &&
            (gPlayer1Controller->buttonDown & R_TRIG) && cam_select_alt_mode(0) == CAM_SELECTION_FIXED)
            || (gCameraMovementFlags & CAM_MOVE_FIX_IN_PLACE)
            || (sMarioCamState->action) == ACT_GETTING_BLOWN) {

            // If this is the first frame that R_TRIG is held, play the "click" sound
            if (c->cutscene == CUTSCENE_NONE && (gPlayer1Controller->buttonPressed & R_TRIG)
                && cam_select_alt_mode(0) == CAM_SELECTION_FIXED) {
                sCameraSoundFlags |= CAM_SOUND_FIXED_ACTIVE;
                play_sound_rbutton_changed();
            }

            // Fixed mode only prevents Lakitu from moving. The camera pos still updates, so
            // Lakitu will fly to his next position as normal whenever R_TRIG is released.
            gLakituState.posHSpeed = 0.f;
            gLakituState.posVSpeed = 0.f;

            vec3f_get_yaw(gLakituState.focus, gLakituState.pos, &c->nextYaw);
            c->yaw = c->nextYaw;
            gCameraMovementFlags &= ~CAM_MOVE_FIX_IN_PLACE;
        } else {
            // Play the "click" sound when fixed mode is released
            if (sCameraSoundFlags & CAM_SOUND_FIXED_ACTIVE) {
                play_sound_rbutton_changed();
                sCameraSoundFlags &= ~CAM_SOUND_FIXED_ACTIVE;
            }
        }
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    } else {
        if ((gPlayer1Controller->buttonPressed & R_TRIG) && (cam_select_alt_mode(0) == CAM_SELECTION_FIXED)) {
            play_sound_button_change_blocked();
        }
    }
#endif

    update_lakitu(c);
#ifdef PUPPYCAM
    }
    // Just a cute little bit that syncs puppycamera up to vanilla when playing a vanilla cutscene :3
    if (c->cutscene != CUTSCENE_NONE) {
        gPuppyCam.yawTarget = gCamera->yaw;
        gPuppyCam.yaw = gCamera->yaw;
        if (gMarioState->action == ACT_ENTERING_STAR_DOOR) { // god this is stupid and the fact I have to continue doing this is testament to the idiocy of the star door cutscene >:(
            gPuppyCam.yawTarget = gMarioState->faceAngle[1] + 0x8000;
            gPuppyCam.yaw = gMarioState->faceAngle[1] + 0x8000;
        }
    }
    if (c->cutscene == CUTSCENE_NONE
        && gPuppyCam.enabled
        && gCurrentArea->camera->mode != CAMERA_MODE_INSIDE_CANNON) {
        // Clear the recent cutscene after 8 frames
        if (gRecentCutscene != CUTSCENE_NONE && sFramesSinceCutsceneEnded < 8) {
            sFramesSinceCutsceneEnded++;
            if (sFramesSinceCutsceneEnded >= 8) {
                gRecentCutscene = CUTSCENE_NONE;
                sFramesSinceCutsceneEnded = 0;
            }
        }
        puppycam_loop();
        // Apply camera shakes
        shake_camera_pitch(gLakituState.pos, gLakituState.focus);
        shake_camera_yaw(gLakituState.pos, gLakituState.focus);
        shake_camera_roll(&gLakituState.roll);
        shake_camera_handheld(gLakituState.pos, gLakituState.focus);

        if ((sMarioCamState->action == ACT_DIVE)
         && (gLakituState.lastFrameAction != ACT_DIVE)) {
            set_camera_shake_from_hit(SHAKE_HIT_FROM_BELOW);
        }
        gLakituState.roll += sHandheldShakeRoll;
        gLakituState.roll += gLakituState.keyDanceRoll;
    }
#endif
    gLakituState.lastFrameAction = sMarioCamState->action;
    profiler_update(PROFILER_TIME_CAMERA, profiler_get_delta(PROFILER_DELTA_COLLISION) - first);
}

#include "behaviors/intro_peach.inc.c"
#include "behaviors/intro_lakitu.inc.c"
#include "behaviors/end_birds_1.inc.c"
#include "behaviors/end_birds_2.inc.c"
#include "behaviors/intro_scene.inc.c"
