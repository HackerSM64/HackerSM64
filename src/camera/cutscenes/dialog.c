#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"
#include "game/object_list_processor.h"

/**
 * Cutscene that plays when Mario talks to a creature.
 */

/**
 * cvar8 is Mario's position and faceAngle
 *
 * cvar9.point is gCutsceneFocus's position
 * cvar9.angle[1] is the yaw between Mario and the gCutsceneFocus
 */
void cutscene_dialog_start(struct Camera *c) {
    s16 yaw;

    cutscene_soften_music(c);
    set_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_DIALOG);

    if (c->mode == CAMERA_MODE_BOSS_FIGHT) {
        vec3f_copy(sCameraStoreCutscene.focus, c->focus);
        vec3f_copy(sCameraStoreCutscene.pos, c->pos);
    } else {
        store_info_star(c);
    }

    // Store Mario's position and faceAngle
    sCutsceneVars[8].angle[0] = 0;
    vec3f_copy(sCutsceneVars[8].point, sMarioCamState->pos);
    sCutsceneVars[8].point[1] += 125.f;

    // Store gCutsceneFocus's position and yaw
    object_pos_to_vec3f(sCutsceneVars[9].point, gCutsceneFocus);
    sCutsceneVars[9].point[1] += gCutsceneFocus->hitboxHeight + 200.f;
    sCutsceneVars[9].angle[1] = calculate_yaw(sCutsceneVars[8].point, sCutsceneVars[9].point);

    yaw = calculate_yaw(sMarioCamState->pos, gLakituState.curPos);
    if ((yaw - sCutsceneVars[9].angle[1]) & 0x8000) {
        sCutsceneVars[9].angle[1] -= 0x6000;
    } else {
        sCutsceneVars[9].angle[1] += 0x6000;
    }
}

/**
 * Move closer to Mario and the object, adjusting to their difference in height.
 * The camera's generally ends up looking over Mario's shoulder.
 */
void cutscene_dialog_move_mario_shoulder(struct Camera *c) {
    f32 dist;
    s16 pitch, yaw;
    Vec3f focus, pos;

    scale_along_line(focus, sCutsceneVars[9].point, sMarioCamState->pos, 0.7f);
    vec3f_get_dist_and_angle(c->pos, focus, &dist, &pitch, &yaw);
    pitch = calculate_pitch(c->pos, sCutsceneVars[9].point);
    vec3f_set_dist_and_angle(c->pos, pos, dist, pitch, yaw);

    focus[1] = focus[1] + (sCutsceneVars[9].point[1] - focus[1]) * 0.1f;
    approach_vec3f_asymptotic(c->focus, focus, 0.2f, 0.2f, 0.2f);

    vec3f_copy(pos, c->pos);

    // Set y pos to cvar8's y (top of focus object)
    pos[1] = sCutsceneVars[8].point[1];
    vec3f_get_dist_and_angle(sCutsceneVars[8].point, pos, &dist, &pitch, &yaw);
    approach_s16_asymptotic_bool(&yaw, sCutsceneVars[9].angle[1], 0x10);
    approach_f32_asymptotic_bool(&dist, 180.f, 0.05f);
    vec3f_set_dist_and_angle(sCutsceneVars[8].point, pos, dist, pitch, yaw);

    // Move up if Mario is below the focus object, down is Mario is above
    pos[1] = sCutsceneVars[8].point[1]
              + sins(calculate_pitch(sCutsceneVars[9].point, sCutsceneVars[8].point)) * 100.f;

    approach_f32_asymptotic_bool(&c->pos[1], pos[1], 0.05f);
    c->pos[0] = pos[0];
    c->pos[2] = pos[2];
}

/**
 * Create the dialog with sCutsceneDialogID
 */
void cutscene_dialog_create_dialog_box(struct Camera *c) {
    if (c->cutscene == CUTSCENE_RACE_DIALOG) {
        create_dialog_box_with_response(sCutsceneDialogID);
    } else {
        create_dialog_box(sCutsceneDialogID);
    }

    //! Unused. This may have been used before sCutsceneDialogResponse was implemented.
    sCutsceneVars[8].angle[0] = DIALOG_RESPONSE_NOT_DEFINED;
}

/**
 * Cutscene that plays when Mario talks to an object.
 */
void cutscene_dialog(struct Camera *c) {
    cutscene_event(cutscene_dialog_start, c, 0, 0);
    cutscene_event(cutscene_dialog_move_mario_shoulder, c, 0, -1);
    cutscene_event(cutscene_dialog_create_dialog_box, c, 10, 10);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;

    if (gDialogResponse != DIALOG_RESPONSE_NONE) {
        sCutsceneDialogResponse = gDialogResponse;
    }

    if ((get_dialog_id() == DIALOG_NONE) && (sCutsceneVars[8].angle[0] != 0)) {
        if (c->cutscene != CUTSCENE_RACE_DIALOG) {
            sCutsceneDialogResponse = DIALOG_RESPONSE_NOT_DEFINED;
        }

        gCutsceneTimer = CUTSCENE_LOOP;
        retrieve_info_star(c);
        transition_next_state(c, 15);
        sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
        cutscene_unsoften_music(c);
    }
}

/**
 * Sets the CAM_FLAG_UNUSED_CUTSCENE_ACTIVE flag, which does nothing.
 */
void cutscene_dialog_set_flag(UNUSED struct Camera *c) {
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
}

/**
 * Ends the dialog cutscene.
 */
void cutscene_dialog_end(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
    c->cutscene = 0;
    clear_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_DIALOG);
}

struct Cutscene sCutsceneDialog[] = {
    { cutscene_dialog, CUTSCENE_LOOP },
    { cutscene_dialog_set_flag, 12 },
    { cutscene_dialog_end, 0 }
};
