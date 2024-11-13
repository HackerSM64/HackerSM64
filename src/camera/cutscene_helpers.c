#include "audio/external.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game/camera.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "cutscene_helpers.h"
#include "camera_math.h"

/**
 * A copy of gDialogID, the dialog displayed during the cutscene.
 */
s16 sCutsceneDialogID;

/**
 * Mario's response to a dialog.
 * 0 = No response yet
 * 1 = Yes
 * 2 = No
 * 3 = Dialog doesn't have a response
 */
u8 sCutsceneDialogResponse = DIALOG_RESPONSE_NONE;

/**
 * A cutscene that plays when the player interacts with an object
 */
u8 sObjectCutscene = CUTSCENE_NONE;

struct Object *gCutsceneFocus = NULL;

/**
 * The ID of the cutscene that ended. It's set to 0 if no cutscene ended less than 8 frames ago.
 *
 * It is only used to prevent the same cutscene from playing twice before 8 frames have passed.
 */
u8 gRecentCutscene = CUTSCENE_NONE;

/**
 * The current frame of the cutscene shot.
 */
s16 gCutsceneTimer;

/**
 * The currently playing shot in the cutscene.
 */
s16 sCutsceneShot;

/**
 * These structs are used by the cutscenes. Most of the fields are unused, and some (all?) of the used
 * ones have multiple uses.
 * Check the cutscene_start functions for documentation on the cvars used by a specific cutscene.
 */
struct CutsceneVariable sCutsceneVars[10];

/**
 * The progress (from 0 to 1) through the current spline segment.
 * When it becomes >= 1, 1.0 is subtracted from it and sCutsceneSplineSegment is increased.
 */
f32 sCutsceneSplineSegmentProgress;

/**
 * The current segment of the CutsceneSplinePoint[] being used.
 */
s16 sCutsceneSplineSegment;

/**
 * Direction controlled by player 2, moves the focus during the credits.
 */
Vec3f sPlayer2FocusOffset;
/**
 * The pitch used for the credits easter egg.
 */
s16 sCreditsPlayer2Pitch;
/**
 * The yaw used for the credits easter egg.
 */
s16 sCreditsPlayer2Yaw;

void init_cutscene_vars() {
    sCreditsPlayer2Pitch = 0;
    sCreditsPlayer2Yaw = 0;
    vec3_zero(sPlayer2FocusOffset);
}

void reset_cutscene_vars() {
    gCutsceneTimer = 0;
    sCutsceneShot = 0;
    gCutsceneObjSpawn = CUTSCENE_OBJ_NONE;
    gObjCutsceneDone = FALSE;
    gCutsceneFocus = NULL;
    sObjectCutscene = CUTSCENE_NONE;
    gRecentCutscene = CUTSCENE_NONE;
}

/**
 * If the camera's yaw is out of the range of `absYaw` +- `yawMax`, then set the yaw to `absYaw`
 */
void star_dance_bound_yaw(struct Camera *c, s16 absYaw, s16 yawMax) {
    s16 yaw;

    vec3f_get_yaw(sMarioCamState->pos, c->pos, &yaw);
    s16 yawFromAbs = yaw - absYaw;

    // Because angles are s16, this checks if yaw is negative
    if ((yawFromAbs & 0x8000) != 0) {
        yawFromAbs = -yawFromAbs;
    }
    if (yawFromAbs > yawMax) {
        yaw = absYaw;
        c->nextYaw = yaw;
        c->yaw = yaw;
    }
}

/**
 * Easter egg: the player 2 controller can move the camera's focus in the ending and credits.
 */
void player2_rotate_cam(struct Camera *c, s16 minPitch, s16 maxPitch, s16 minYaw, s16 maxYaw) {
    f32 distCamToFocus;
    s16 pitch, yaw, pitchCap;

    // Change the camera rotation to match the 2nd player's stick
    approach_s16_asymptotic_bool(&sCreditsPlayer2Yaw, -(s16)(gPlayer2Controller->stickX * 250.f), 4);
    approach_s16_asymptotic_bool(&sCreditsPlayer2Pitch, -(s16)(gPlayer2Controller->stickY * 265.f), 4);
    vec3f_get_dist_and_angle(c->pos, c->focus, &distCamToFocus, &pitch, &yaw);

    pitchCap = 0x3800 - pitch;
    if (pitchCap < 0) {
        pitchCap = 0;
    }
    if (maxPitch > pitchCap) {
        maxPitch = pitchCap;
    }

    pitchCap = -0x3800 - pitch;
    if (pitchCap > 0) {
        pitchCap = 0;
    }
    if (minPitch < pitchCap) {
        minPitch = pitchCap;
    }

    if (sCreditsPlayer2Pitch > maxPitch) {
        sCreditsPlayer2Pitch = maxPitch;
    }
    if (sCreditsPlayer2Pitch < minPitch) {
        sCreditsPlayer2Pitch = minPitch;
    }

    if (sCreditsPlayer2Yaw > maxYaw) {
        sCreditsPlayer2Yaw = maxYaw;
    }
    if (sCreditsPlayer2Yaw < minYaw) {
        sCreditsPlayer2Yaw = minYaw;
    }

    pitch += sCreditsPlayer2Pitch;
    yaw += sCreditsPlayer2Yaw;
    vec3f_set_dist_and_angle(c->pos, sPlayer2FocusOffset, distCamToFocus, pitch, yaw);
    vec3f_sub(sPlayer2FocusOffset, c->focus);
}

/**
 * Start a cutscene focusing on an object
 * This will play if nothing else happened in the same frame, like exiting or warping.
 */
void start_object_cutscene(u8 cutscene, struct Object *obj) {
    sObjectCutscene = cutscene;
    gRecentCutscene = CUTSCENE_NONE;
    gCutsceneFocus = obj;
    gObjCutsceneDone = FALSE;
}

/**
 * Start a low-priority cutscene without focusing on an object
 * This will play if nothing else happened in the same frame, like exiting or warping.
 */
void start_object_cutscene_without_focus(u8 cutscene) {
    sObjectCutscene = cutscene;
    sCutsceneDialogResponse = DIALOG_RESPONSE_NONE;
}

UNUSED s32 unused_dialog_cutscene_response(u8 cutscene) {
    // if not in a cutscene, start this one
    if ((gCamera->cutscene == 0) && (sObjectCutscene == 0)) {
        sObjectCutscene = cutscene;
    }

    // if playing this cutscene and Mario responded, return the response
    if ((gCamera->cutscene == cutscene) && (sCutsceneDialogResponse)) {
        return sCutsceneDialogResponse;
    } else {
        return 0;
    }
}

s16 cutscene_object_with_dialog(u8 cutscene, struct Object *obj, s16 dialogID) {
    s16 response = DIALOG_RESPONSE_NONE;

    if ((gCamera->cutscene == CUTSCENE_NONE) && (sObjectCutscene == CUTSCENE_NONE)) {
        if (gRecentCutscene != cutscene) {
            start_object_cutscene(cutscene, obj);
            if (dialogID != DIALOG_NONE) {
                sCutsceneDialogID = dialogID;
            } else {
                sCutsceneDialogID = DIALOG_001;
            }
        } else {
            response = sCutsceneDialogResponse;
        }

        gRecentCutscene = CUTSCENE_NONE;
    }
    return response;
}

s16 cutscene_object_without_dialog(u8 cutscene, struct Object *obj) {
    return cutscene_object_with_dialog(cutscene, obj, DIALOG_NONE);
}

/**
 * @return 0 if not started, 1 if started, and -1 if finished
 */
s16 cutscene_object(u8 cutscene, struct Object *obj) {
    s16 status = 0;

    if ((gCamera->cutscene == 0) && (sObjectCutscene == 0)) {
        if (gRecentCutscene != cutscene) {
            start_object_cutscene(cutscene, obj);
            status = 1;
        } else {
            status = -1;
        }
    }
    return status;
}

/**
 * Starts a cutscene dialog. Only has an effect when `trigger` is 1
 */
void trigger_cutscene_dialog(s32 trigger) {
    if (trigger == 1) start_object_cutscene_without_focus(CUTSCENE_READ_MESSAGE);
}

void reset_pan_distance(UNUSED struct Camera *c) {
    sPanDistance = 0;
}

/**
 * Triggers Mario to enter a dialog state. This is used to make Mario look at the focus of a cutscene,
 * for example, bowser.
 * @param state 0 = stop, 1 = start, 2 = start and look up, and 3 = start and look down
 *
 * @return if Mario left the dialog state, return CUTSCENE_LOOP, else return gCutsceneTimer
 */
s16 cutscene_common_set_dialog_state(s32 state) {
    s16 timer = gCutsceneTimer;
    // If the dialog ended, return CUTSCENE_LOOP, which would end the cutscene shot
    if (set_mario_npc_dialog(state) == MARIO_DIALOG_STATUS_SPEAK) {
        timer = CUTSCENE_LOOP;
    }
    return timer;
}

void cutscene_stop_dialog(UNUSED struct Camera *c) {
    cutscene_common_set_dialog_state(MARIO_DIALOG_STOP);
}

/**
 * Cutscene helpers for Mario to enter the dialog state and look in a direction
 */
void cutscene_mario_dialog_look_down(UNUSED struct Camera *c) {
    gCutsceneTimer = cutscene_common_set_dialog_state(MARIO_DIALOG_LOOK_DOWN);
}

void cutscene_mario_dialog_look_front(UNUSED struct Camera *c) {
    gCutsceneTimer = cutscene_common_set_dialog_state(MARIO_DIALOG_LOOK_FRONT);
}

void cutscene_mario_dialog_look_up(UNUSED struct Camera *c) {
    gCutsceneTimer = cutscene_common_set_dialog_state(MARIO_DIALOG_LOOK_UP);
}

void set_flag_post_door(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_BEHIND_MARIO_POST_DOOR;
    sCameraYawAfterDoorCutscene = calculate_yaw(c->focus, c->pos);
}

/**
 * Ends the double door cutscene.
 */
void cutscene_double_doors_end(struct Camera *c) {
    set_flag_post_door(c);
    c->cutscene = 0;
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
}

/**
 * End the cutscene, used by cutscenes that play when Mario exits a course to castle grounds.
 */
void cutscene_exit_to_castle_grounds_end(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    gCutsceneTimer = CUTSCENE_STOP;
    c->cutscene = 0;
    cutscene_update_camera_yaw(c);
}

/**
 * Start a preset fov shake. Used in cutscenes
 */
void cutscene_set_fov_shake_preset(u8 preset) {
    switch (preset) {
        case 1:
            set_fov_shake(0x100, 0x30, 0x8000);
            break;
        case 2:
            set_fov_shake(0x400, 0x20, 0x4000);
            break;
    }
}

void cutscene_shake_explosion(UNUSED struct Camera *c) {
    set_environmental_camera_shake(SHAKE_ENV_EXPLOSION);
    cutscene_set_fov_shake_preset(1);
}

/**
 * Update the camera's yaw and nextYaw. This is called from cutscenes to ignore the camera mode's yaw.
 */
void cutscene_update_camera_yaw(struct Camera *c) {
    c->nextYaw = calculate_yaw(c->focus, c->pos);
    c->yaw = c->nextYaw;
}

void cutscene_soften_music(UNUSED struct Camera *c) {
    seq_player_lower_volume(SEQ_PLAYER_LEVEL, 60, 40);
}

void cutscene_unsoften_music(UNUSED struct Camera *c) {
    seq_player_unlower_volume(SEQ_PLAYER_LEVEL, 60);
}

void cutscene_reset_spline(void) {
    sCutsceneSplineSegment = 0;
    sCutsceneSplineSegmentProgress = 0;
}

void stop_cutscene_and_retrieve_stored_info(struct Camera *c) {
    gCutsceneTimer = CUTSCENE_STOP;
    c->cutscene = 0;
    vec3f_copy(c->focus, sCameraStoreCutscene.focus);
    vec3f_copy(c->pos, sCameraStoreCutscene.pos);
}

/**
 * Store camera info for the cannon opening cutscene
 */
void store_info_cannon(struct Camera *c) {
    vec3f_copy(sCameraStoreCutscene.pos, c->pos);
    vec3f_copy(sCameraStoreCutscene.focus, c->focus);
    sCameraStoreCutscene.panDist = sPanDistance;
    sCameraStoreCutscene.cannonYOffset = sCannonYOffset;
}

/**
 * Retrieve camera info for the cannon opening cutscene
 */
void retrieve_info_cannon(struct Camera *c) {
    vec3f_copy(c->pos, sCameraStoreCutscene.pos);
    vec3f_copy(c->focus, sCameraStoreCutscene.focus);
    sPanDistance = sCameraStoreCutscene.panDist;
    sCannonYOffset = sCameraStoreCutscene.cannonYOffset;
}

/**
 * Store camera info for the star spawn cutscene
 */
void store_info_star(struct Camera *c) {
    reset_pan_distance(c);
    vec3f_copy(sCameraStoreCutscene.pos, c->pos);
    sCameraStoreCutscene.focus[0] = sMarioCamState->pos[0];
    sCameraStoreCutscene.focus[1] = c->focus[1];
    sCameraStoreCutscene.focus[2] = sMarioCamState->pos[2];
}

/**
 * Retrieve camera info for the star spawn cutscene
 */
void retrieve_info_star(struct Camera *c) {
    vec3f_copy(c->pos, sCameraStoreCutscene.pos);
    vec3f_copy(c->focus, sCameraStoreCutscene.focus);
}

/**
 * Calculates Mario's distance to the floor, or the water level if it is above the floor. Then:
 * `posOff` is set to the distance multiplied by posMul and bounded to [-posBound, posBound]
 * `focOff` is set to the distance multiplied by focMul and bounded to [-focBound, focBound]
 *
 * Notes:
 *      posMul is always 1.0f, focMul is always 0.9f
 *      both ranges are always 200.0f
 *          Since focMul is 0.9, `focOff` is closer to the floor than `posOff`
 *      posOff and focOff are sometimes the same address, which just ignores the pos calculation
 */
void calc_y_to_curr_floor(f32 *posOff, f32 posMul, f32 posBound, f32 *focOff, f32 focMul, f32 focBound) {
    f32 floorHeight = sMarioGeometry.currFloorHeight;
    f32 waterHeight;

    if (!(sMarioCamState->action & ACT_FLAG_METAL_WATER)) {
        //! @bug this should use sMarioGeometry.waterHeight
        if (floorHeight < (waterHeight = find_water_level(sMarioCamState->pos[0], sMarioCamState->pos[2]))) {
            floorHeight = waterHeight;
        }
    }

    if (sMarioCamState->action & ACT_FLAG_ON_POLE) {
        if (sMarioGeometry.currFloorHeight >= gMarioStates[0].usedObj->oPosY && sMarioCamState->pos[1]
                   < 0.7f * gMarioStates[0].usedObj->hitboxHeight + gMarioStates[0].usedObj->oPosY) {
            posBound = 1200;
        }
    }

    *posOff = (floorHeight - sMarioCamState->pos[1]) * posMul;

    if (*posOff > posBound) {
        *posOff = posBound;
    }

    if (*posOff < -posBound) {
        *posOff = -posBound;
    }

    *focOff = (floorHeight - sMarioCamState->pos[1]) * focMul;

    if (*focOff > focBound) {
        *focOff = focBound;
    }

    if (*focOff < -focBound) {
        *focOff = -focBound;
    }
}

/**
 * Set the camera's focus to Mario's position, and add several relative offsets.
 *
 * @param leftRight offset to Mario's left/right, relative to his faceAngle
 * @param yOff y offset
 * @param forwBack offset to Mario's front/back, relative to his faceAngle
 * @param yawOff offset to Mario's faceAngle, changes the direction of `leftRight` and `forwBack`
 */
void set_focus_rel_mario(struct Camera *c, f32 leftRight, f32 yOff, f32 forwBack, s16 yawOff) {
    s16 yaw;
    f32 focFloorYOff;

    calc_y_to_curr_floor(&focFloorYOff, 1.f, 200.f, &focFloorYOff, 0.9f, 200.f);
    yaw = sMarioCamState->faceAngle[1] + yawOff;
    c->focus[2] = sMarioCamState->pos[2] + forwBack * coss(yaw) - leftRight * sins(yaw);
    c->focus[0] = sMarioCamState->pos[0] + forwBack * sins(yaw) + leftRight * coss(yaw);
    c->focus[1] = sMarioCamState->pos[1] + yOff + focFloorYOff;
}

/**
 * Adjust the camera focus towards a point `dist` units in front of Mario.
 * @param dist distance in Mario's forward direction. Note that this is relative to Mario, so a negative
 *        distance will focus in front of Mario, and a positive distance will focus behind him.
 */
void focus_in_front_of_mario(struct Camera *c, f32 dist, f32 speed) {
    Vec3f goalFocus, offset;

    offset[0] = 0.f;
    offset[2] = dist;
    offset[1] = 100.f;

    offset_rotated(goalFocus, sMarioCamState->pos, offset, sMarioCamState->faceAngle);
    approach_vec3f_asymptotic(c->focus, goalFocus, speed, speed, speed);
}

/**
 * Call the event while `start` <= gCutsceneTimer <= `end`
 * If `end` is -1, call for the rest of the shot.
 */
void cutscene_event(CameraEvent event, struct Camera *c, s16 start, s16 end) {
    if (start <= gCutsceneTimer) {
        if (end == -1 || end >= gCutsceneTimer) {
            event(c);
        }
    }
}

/**
 * Set gCutsceneObjSpawn when gCutsceneTimer == `frame`.
 *
 * @see intro_scene.inc.c for details on which objects are spawned.
 */
void cutscene_spawn_obj(u32 obj, s16 frame) {
    if (frame == gCutsceneTimer) {
        gCutsceneObjSpawn = obj;
    }
}

/**
 * Moves the camera towards the cutscene's focus, stored in sCutsceneVars[3].point
 *
 * sCutsceneVars[3].point is used as the target point
 * sCutsceneVars[0].point is used as the current camera focus during the transition
 *
 * @param rotPitch constant pitch offset to add to the camera's focus
 * @param rotYaw constant yaw offset to add to the camera's focus
 */
void cutscene_goto_cvar_pos(struct Camera *c, f32 goalDist, s16 goalPitch, s16 rotPitch, s16 rotYaw) {
    f32 nextDist;
    s16 nextPitch, nextYaw;
    // The next 2 polar coord points are only used in CUTSCENE_PREPARE_CANNON
    f32 cannonDist;
    s16 cannonPitch, cannonYaw;
    f32 curDist;
    s16 curPitch, curYaw;

    vec3f_get_dist_and_angle(sCutsceneVars[3].point, c->pos, &nextDist, &nextPitch, &nextYaw);

    // If over 8000 units away from the cannon, just teleport there
    if ((nextDist > 8000.f) && (c->cutscene == CUTSCENE_PREPARE_CANNON)) {
        nextDist = goalDist * 4.f;
        nextPitch = goalPitch;
        vec3f_copy(sCutsceneVars[0].point, sCutsceneVars[3].point);
        sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
        if (gCurrLevelNum == LEVEL_TTM) {
            nextYaw = atan2s(sCutsceneVars[3].point[2] - c->areaCenZ,
                             sCutsceneVars[3].point[0] - c->areaCenX);
        }
#endif
    } else {
        if (c->cutscene == CUTSCENE_PREPARE_CANNON) {
            vec3f_get_dist_and_angle(c->pos, sCutsceneVars[0].point, &curDist, &curPitch, &curYaw);
            vec3f_get_dist_and_angle(c->pos, sCutsceneVars[3].point, &cannonDist, &cannonPitch, &cannonYaw);
            approach_f32_asymptotic_bool(&curDist, cannonDist, 0.1f);
            approach_s16_asymptotic_bool(&curPitch, cannonPitch, 15);
            approach_s16_asymptotic_bool(&curYaw, cannonYaw, 15);
            // Move the current focus, sCutsceneVars[0].point, in the direction towards the cannon
            vec3f_set_dist_and_angle(c->pos, sCutsceneVars[0].point, curDist, curPitch, curYaw);
        } else {
            approach_vec3f_asymptotic(sCutsceneVars[0].point, sCutsceneVars[3].point, 0.1f, 0.1f, 0.1f);
        }
    }

    approach_f32_asymptotic_bool(&nextDist, goalDist, 0.05f);
    approach_s16_asymptotic_bool(&nextPitch, goalPitch, 0x20);

    vec3f_set_dist_and_angle(sCutsceneVars[3].point, c->pos, nextDist, nextPitch, nextYaw);
    vec3f_copy(c->focus, sCutsceneVars[0].point);

    // Apply the constant rotation given
    pan_camera(c, rotPitch, rotYaw);
    vec3f_get_dist_and_angle(c->pos, c->focus, &nextDist, &nextPitch, &nextYaw);

    if (nextPitch < -0x3000) {
        nextPitch = -0x3000;
    }
    if (nextPitch > 0x3000) {
        nextPitch = 0x3000;
    }

    vec3f_set_dist_and_angle(c->pos, c->focus, nextDist, nextPitch, nextYaw);
}

/**
 * Zero the 10 cvars.
 */
void clear_cutscene_vars(UNUSED struct Camera *c) {
    s32 i;

    for (i = 0; i < 10; i++) {
        sCutsceneVars[i].unused1 = 0;
        vec3_zero(sCutsceneVars[i].point);
        vec3_zero(sCutsceneVars[i].unusedPoint);
        vec3_zero(sCutsceneVars[i].angle);
        sCutsceneVars[i].unused2 = 0;
    }
}

/**
 * Start the cutscene, `cutscene`, if it is not already playing.
 */
void start_cutscene(struct Camera *c, u8 cutscene) {
    if (c->cutscene != cutscene) {
        c->cutscene = cutscene;
        clear_cutscene_vars(c);
    }
}

/**
 * Play the current cutscene until either gCutsceneTimer reaches the max time, or c->cutscene is set to 0
 *
 * Note that CAM_FLAG_SMOOTH_MOVEMENT is cleared while a cutscene is playing, so cutscenes set it for
 * the duration they want the flag to be active.
 */
void play_cutscene(struct Camera *c) {
    s16 cutsceneDuration;
    u8 oldCutscene = c->cutscene;

    sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
    gCameraMovementFlags &= ~CAM_MOVING_INTO_MODE;

#define CUTSCENE(id, cutscene)                                                                            \
    case id:                                                                                              \
        cutsceneDuration = cutscene[sCutsceneShot].duration;                                              \
        cutscene[sCutsceneShot].shot(c);                                                                  \
        break;

    switch (c->cutscene) {
        CUTSCENE(CUTSCENE_STAR_SPAWN,           sCutsceneStarSpawn)
        CUTSCENE(CUTSCENE_RED_COIN_STAR_SPAWN,  sCutsceneRedCoinStarSpawn)
        CUTSCENE(CUTSCENE_ENDING,               sCutsceneEnding)
        CUTSCENE(CUTSCENE_GRAND_STAR,           sCutsceneGrandStar)
        CUTSCENE(CUTSCENE_DOOR_WARP,            sCutsceneDoorWarp)
        CUTSCENE(CUTSCENE_DOOR_PULL,            sCutsceneDoorPull)
        CUTSCENE(CUTSCENE_DOOR_PUSH,            sCutsceneDoorPush)
        CUTSCENE(CUTSCENE_DOOR_PULL_MODE,       sCutsceneDoorPullMode)
        CUTSCENE(CUTSCENE_DOOR_PUSH_MODE,       sCutsceneDoorPushMode)
        CUTSCENE(CUTSCENE_ENTER_CANNON,         sCutsceneEnterCannon)
        CUTSCENE(CUTSCENE_ENTER_PAINTING,       sCutsceneEnterPainting)
        CUTSCENE(CUTSCENE_DEATH_EXIT,           sCutsceneDeathExit)
        CUTSCENE(CUTSCENE_EXIT_PAINTING_SUCC,   sCutsceneExitPaintingSuccess)
        CUTSCENE(CUTSCENE_UNUSED_EXIT,          sCutsceneUnusedExit)
        CUTSCENE(CUTSCENE_INTRO_PEACH,          sCutsceneIntroPeach)
        CUTSCENE(CUTSCENE_ENTER_BOWSER_ARENA,   sCutsceneEnterBowserArena)
        CUTSCENE(CUTSCENE_DANCE_ROTATE,         sCutsceneDanceDefaultRotate)
        CUTSCENE(CUTSCENE_DANCE_DEFAULT,        sCutsceneDanceDefaultRotate)
        CUTSCENE(CUTSCENE_DANCE_FLY_AWAY,       sCutsceneDanceFlyAway)
        CUTSCENE(CUTSCENE_DANCE_CLOSEUP,        sCutsceneDanceCloseup)
        CUTSCENE(CUTSCENE_KEY_DANCE,            sCutsceneKeyDance)
        CUTSCENE(CUTSCENE_0F_UNUSED,            sCutsceneUnused)
        CUTSCENE(CUTSCENE_END_WAVING,           sCutsceneEndWaving)
        CUTSCENE(CUTSCENE_CREDITS,              sCutsceneCredits)
        CUTSCENE(CUTSCENE_CAP_SWITCH_PRESS,     sCutsceneCapSwitchPress)
        CUTSCENE(CUTSCENE_SLIDING_DOORS_OPEN,   sCutsceneSlidingDoorsOpen)
        CUTSCENE(CUTSCENE_PREPARE_CANNON,       sCutscenePrepareCannon)
        CUTSCENE(CUTSCENE_UNLOCK_KEY_DOOR,      sCutsceneUnlockKeyDoor)
        CUTSCENE(CUTSCENE_STANDING_DEATH,       sCutsceneStandingDeath)
        CUTSCENE(CUTSCENE_ENTER_POOL,           sCutsceneEnterPool)
        CUTSCENE(CUTSCENE_DEATH_ON_STOMACH,     sCutsceneDeathStomach)
        CUTSCENE(CUTSCENE_DEATH_ON_BACK,        sCutsceneDeathOnBack)
        CUTSCENE(CUTSCENE_QUICKSAND_DEATH,      sCutsceneQuicksandDeath)
        CUTSCENE(CUTSCENE_SUFFOCATION_DEATH,    sCutsceneSuffocation)
        CUTSCENE(CUTSCENE_EXIT_BOWSER_SUCC,     sCutsceneExitBowserSuccess)
        CUTSCENE(CUTSCENE_EXIT_BOWSER_DEATH,    sCutsceneExitBowserDeath)
        CUTSCENE(CUTSCENE_EXIT_SPECIAL_SUCC,    sCutsceneExitSpecialSuccess)
        CUTSCENE(CUTSCENE_EXIT_WATERFALL,       sCutsceneExitWaterfall)
        CUTSCENE(CUTSCENE_EXIT_FALL_WMOTR,      sCutsceneFallToCastleGrounds)
        CUTSCENE(CUTSCENE_NONPAINTING_DEATH,    sCutsceneNonPaintingDeath)
        CUTSCENE(CUTSCENE_DIALOG,               sCutsceneDialog)
        CUTSCENE(CUTSCENE_READ_MESSAGE,         sCutsceneReadMessage)
        CUTSCENE(CUTSCENE_RACE_DIALOG,          sCutsceneDialog)
        CUTSCENE(CUTSCENE_ENTER_PYRAMID_TOP,    sCutsceneEnterPyramidTop)
        CUTSCENE(CUTSCENE_SSL_PYRAMID_EXPLODE,  sCutscenePyramidTopExplode)
    }

#undef CUTSCENE

    if ((cutsceneDuration != 0) && !(gCutsceneTimer & CUTSCENE_STOP)) {
        if (gCutsceneTimer < CUTSCENE_LOOP) {
            gCutsceneTimer++;
        }
        //! Because gCutsceneTimer is often set to 0x7FFF (CUTSCENE_LOOP), this conditional can only
        //! check for == due to overflow
        if (gCutsceneTimer == cutsceneDuration) {
            sCutsceneShot++;
            gCutsceneTimer = 0;
        }
    } else {
        sMarioCamState->cameraEvent = CAM_EVENT_NONE;
        sCutsceneShot = 0;
        gCutsceneTimer = 0;
    }

    sAreaYawChange = 0;

    // The cutscene just ended
    if ((c->cutscene == CUTSCENE_NONE) && (oldCutscene != 0)) {
        gRecentCutscene = oldCutscene;
    }
}

