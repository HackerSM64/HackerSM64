#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Cutscene that plays when a cannon door is opened.
 */

/**
 * Store the camera's pos and focus, and copy the cannon's position to cvars.
 */
void cutscene_prepare_cannon_start(struct Camera *c) {
    store_info_cannon(c);
    vec3f_copy(sCutsceneVars[0].point, c->focus);
    sCutsceneVars[2].point[0] = 30.f;
    // Store the cannon door's position in sCutsceneVars[3]'s point
    object_pos_to_vec3f(sCutsceneVars[3].point, gCutsceneFocus);
    vec3s_set(sCutsceneVars[5].angle, 0, 0, 0);
}

/**
 * Fly towards the cannon door.
 */
void cutscene_prepare_cannon_fly_to_cannon(struct Camera *c) {
    cutscene_goto_cvar_pos(c, 300.f, 0x2000, 0, sCutsceneVars[5].angle[1]);
    camera_approach_s16_symmetric_bool(&sCutsceneVars[5].angle[1], 0x400, 17);
    set_handheld_shake(HAND_CAM_SHAKE_CUTSCENE);
}

/**
 * Used in the cannon opening cutscene to fly back to the camera's last position and focus
 */
void cannon_approach_prev(f32 *value, f32 target) {
    f32 inc = absf(target - *value) / sCutsceneVars[2].point[0];
    camera_approach_f32_symmetric_bool(value, target, inc);
}

/**
 * Fly or warp back to the previous pos and focus, stored in sCameraStoreCutscene.
 */
void cutscene_prepare_cannon_fly_back(struct Camera *c) {
    f32 distToPrevPos = calc_abs_dist_squared(c->pos, sCameraStoreCutscene.pos);

    if (distToPrevPos < sqr(8000.f)) {
        cannon_approach_prev(&c->pos[0], sCameraStoreCutscene.pos[0]);
        cannon_approach_prev(&c->pos[1], sCameraStoreCutscene.pos[1]);
        cannon_approach_prev(&c->pos[2], sCameraStoreCutscene.pos[2]);
        cannon_approach_prev(&c->focus[0], sCameraStoreCutscene.focus[0]);
        cannon_approach_prev(&c->focus[1], sCameraStoreCutscene.focus[1]);
        cannon_approach_prev(&c->focus[2], sCameraStoreCutscene.focus[2]);
    } else {
        // If too far away, just warp back
        vec3f_copy(c->focus, sCameraStoreCutscene.focus);
        vec3f_copy(c->pos, sCameraStoreCutscene.pos);
        sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
    }
    if (sCutsceneVars[2].point[0] > 1.f) {
        sCutsceneVars[2].point[0] -= 1.f;
    }
}

/**
 * Cutscene that plays when the cannon is opened.
 */
void cutscene_prepare_cannon(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    cutscene_event(cutscene_prepare_cannon_start, c, 0, 0);
    cutscene_event(cutscene_prepare_cannon_fly_to_cannon, c, 0, 140);
    cutscene_event(cutscene_prepare_cannon_fly_back, c, 141, -1);
}

/**
 * Stop the cannon opening cutscene.
 */
void cutscene_prepare_cannon_end(struct Camera *c) {
    gCutsceneTimer = CUTSCENE_STOP;
    c->cutscene = 0;
    retrieve_info_cannon(c);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
}

struct Cutscene sCutscenePrepareCannon[] = {
    { cutscene_prepare_cannon, 170 },
    { cutscene_prepare_cannon_end, 0 }
};
