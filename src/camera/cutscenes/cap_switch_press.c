#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"
#include "game/save_file.h"

/**
 * Cutscene that plays when Mario presses a cap switch.
 */

void cap_switch_save(UNUSED s16 param) {
    save_file_do_save(gCurrSaveFileNum - 1);
}

/**
 * Set cvars:
 * cvar3 is an offset applied to the camera's rotation around Mario. It starts at 0x1200
 * cvar 1 is more complicated:
 *      First the yaw from Mario to the camera is calculated. cvar1 is the high byte of the difference
 *      between that yaw and Mario's faceAngle plus 0x1200. The reason for taking the high byte is
 *      because cvar1 rotates until is reaches 0, so it's important that it's a multiple of 0x100.
 */
void cutscene_cap_switch_press_start(struct Camera *c) {
    store_info_star(c);
    s16 yaw = calculate_yaw(sMarioCamState->pos, c->pos);
    sCutsceneVars[3].angle[1] = 0x1200;
    // Basically the amount of rotation to get from behind Mario to in front of Mario
    sCutsceneVars[1].angle[1] = (yaw - (sMarioCamState->faceAngle[1] + sCutsceneVars[3].angle[1])) & 0xFF00;
}

/**
 * Rotate around Mario. As each cvar stops updating, the rotation slows until the camera ends up in
 * front of Mario.
 */
void cutscene_cap_switch_press_rotate_around_mario(struct Camera *c) {
    f32 dist;
    s16 pitch, yaw;

    vec3f_get_dist_and_angle(sMarioCamState->pos, c->pos, &dist, &pitch, &yaw);

    // cvar3 wraps around until it reaches 0x1000
    if (sCutsceneVars[3].angle[1] != 0x1000) {
        sCutsceneVars[3].angle[1] += 0x100;
    }

    // cvar1 wraps until 0
    if (sCutsceneVars[1].angle[1] != 0) {
        sCutsceneVars[1].angle[1] += 0x100;
    }

    yaw = sMarioCamState->faceAngle[1] + sCutsceneVars[3].angle[1] + sCutsceneVars[1].angle[1];
    vec3f_set_dist_and_angle(sMarioCamState->pos, c->pos, dist, pitch, yaw);
}

/**
 * Move the camera slightly downwards.
 */
void cutscene_cap_switch_press_lower_cam(struct Camera *c) {
    rotate_and_move_vec3f(c->pos, sMarioCamState->pos, 0, -0x20, 0);
}

/**
 * Move the camera closer to Mario.
 */
void cutscene_cap_switch_press_approach_mario(struct Camera *c) {
    s16 pitch, yaw;
    f32 dist;

    vec3f_get_dist_and_angle(sMarioCamState->pos, c->pos, &dist, &pitch, &yaw);
    approach_f32_asymptotic_bool(&dist, 195.f, 0.2f);
    approach_s16_asymptotic_bool(&pitch, 0, 0x10);
    vec3f_set_dist_and_angle(sMarioCamState->pos, c->pos, dist, pitch, yaw);

    approach_f32_asymptotic_bool(&c->focus[0], sMarioCamState->pos[0], 0.1f);
    approach_f32_asymptotic_bool(&c->focus[1], sMarioCamState->pos[1] + 110.f, 0.1f);
    approach_f32_asymptotic_bool(&c->focus[2], sMarioCamState->pos[2], 0.1f);
}

/**
 * Pan the camera left so that Mario is on the right side of the screen when the camera stops spinning.
 */
void cutscene_cap_switch_press_pan_left(struct Camera *c) {
    vec3f_copy(c->focus, sMarioCamState->pos);
    c->focus[1] += 110.f;
    camera_approach_s16_symmetric_bool(&sCutsceneVars[0].angle[1], 0x800, 0x20);
    pan_camera(c, sCutsceneVars[0].angle[0], sCutsceneVars[0].angle[1]);
}

/**
 * Create a dialog box with the cap switch's text.
 */
void cutscene_cap_switch_press_create_dialog(UNUSED struct Camera *c) {
    create_dialog_box_with_response(gCutsceneFocus->oBehParams2ndByte + DIALOG_010);
}

static UNUSED void unused_cap_switch_retrieve_info(struct Camera *c) {
    retrieve_info_star(c);
    transition_next_state(c, 30);
}

void cutscene_cap_switch_press(struct Camera *c) {
    f32 dist;
    s16 pitch, yaw;

    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;

    cutscene_event(cutscene_cap_switch_press_start, c, 0, 0);
    cutscene_event(cutscene_cap_switch_press_approach_mario, c, 0, 30);
    cutscene_event(cutscene_cap_switch_press_pan_left, c, 0, -1);
    cutscene_event(cutscene_cap_switch_press_rotate_around_mario, c, 30, -1);
    cutscene_event(cutscene_cap_switch_press_lower_cam, c, 10, 70);
    cutscene_event(cutscene_cap_switch_press_create_dialog, c, 10, 10);
    vec3f_get_dist_and_angle(sMarioCamState->pos, c->pos, &dist, &pitch, &yaw);

    if (gDialogResponse != DIALOG_RESPONSE_NONE) {
        sCutsceneVars[4].angle[0] = gDialogResponse;
    }

    if ((get_dialog_id() == DIALOG_NONE) && (sCutsceneVars[4].angle[0] != 0)) {
        sCutsceneDialogResponse = sCutsceneVars[4].angle[0];
        if (sCutsceneVars[4].angle[0] == 1) {
            cap_switch_save(gCutsceneFocus->oBehParams2ndByte);
        }
        stop_cutscene_and_retrieve_stored_info(c);
        transition_next_state(c, 30);
    }
}

struct Cutscene sCutsceneCapSwitchPress[] = {
    { cutscene_cap_switch_press, CUTSCENE_LOOP }
};
