#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Star dance cutscene.
 * The camera moves in for a closeup on Mario. Used in tight spaces and underwater.
 */

/**
 * Start the closeup dance cutscene by restricting the camera's yaw in certain areas.
 * Store the camera's focus in cvar9.
 */
void cutscene_dance_closeup_start(struct Camera *c) {
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    if ((gLastCompletedStarNum == 4) && (gCurrCourseNum == COURSE_JRB)) {
        star_dance_bound_yaw(c, 0x0, 0x4000);
    }
    if ((gLastCompletedStarNum == 1) && (gCurrCourseNum == COURSE_DDD)) {
        star_dance_bound_yaw(c, 0x8000, 0x5000);
    }
    if ((gLastCompletedStarNum == 5) && (gCurrCourseNum == COURSE_WDW)) {
        star_dance_bound_yaw(c, 0x8000, 0x800);
    }
#endif

    vec3f_copy(sCutsceneVars[9].point, c->focus);
    //! cvar8 is unused in the closeup cutscene
    sCutsceneVars[8].angle[0] = 0x2000;
}

/**
 * Focus the camera on Mario eye height.
 */
void cutscene_dance_closeup_focus_mario(struct Camera *c) {
    Vec3f marioPos;

    vec3f_set(marioPos, sMarioCamState->pos[0], sMarioCamState->pos[1] + 125.f, sMarioCamState->pos[2]);
    approach_vec3f_asymptotic(sCutsceneVars[9].point, marioPos, 0.2f, 0.2f, 0.2f);
    vec3f_copy(c->focus, sCutsceneVars[9].point);
}

/**
 * Fly above Mario, looking down.
 */
void cutscene_dance_closeup_fly_above(struct Camera *c) {
    s16 pitch, yaw;
    f32 dist;
    s16 goalPitch = 0x1800;
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    if ((gLastCompletedStarNum == 6 && gCurrCourseNum == COURSE_SL) ||
        (gLastCompletedStarNum == 4 && gCurrCourseNum == COURSE_TTC)) {
        goalPitch = 0x800;
    }
#endif
    vec3f_get_dist_and_angle(sMarioCamState->pos, c->pos, &dist, &pitch, &yaw);
    approach_f32_asymptotic_bool(&dist, 800.f, 0.05f);
    approach_s16_asymptotic_bool(&pitch, goalPitch, 16);
    approach_s16_asymptotic_bool(&yaw, c->yaw, 8);
    vec3f_set_dist_and_angle(sMarioCamState->pos, c->pos, dist, pitch, yaw);
}

/**
 * Fly closer right when Mario gives the peace sign.
 */
void cutscene_dance_closeup_fly_closer(struct Camera *c) {
    s16 pitch, yaw;
    f32 dist;

    vec3f_get_dist_and_angle(sMarioCamState->pos, c->pos, &dist, &pitch, &yaw);
    approach_f32_asymptotic_bool(&dist, 240.f, 0.4f);
    approach_s16_asymptotic_bool(&yaw, c->yaw, 8);
    approach_s16_asymptotic_bool(&pitch, 0x1000, 5);
    vec3f_set_dist_and_angle(sMarioCamState->pos, c->pos, dist, pitch, yaw);
}

/**
 * Zoom in by increasing fov to 80 degrees. Most dramatic zoom in the game.
 */
void cutscene_dance_closeup_zoom(UNUSED struct Camera *c) {
    set_fov_function(CAM_FOV_APP_80);
}

/**
 * Shake fov, starts on the first frame Mario has the peace sign up.
 */
void cutscene_dance_closeup_shake_fov(UNUSED struct Camera *c) {
    set_fov_shake(0x300, 0x30, 0x8000);
}

/**
 * The camera moves in for a closeup on Mario. Used for stars that are underwater or in tight places.
 */
void cutscene_dance_closeup(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;

    if (sMarioCamState->action == ACT_STAR_DANCE_WATER) {
        cutscene_event(cutscene_dance_closeup_start, c, 0, 0);
        cutscene_event(cutscene_dance_closeup_focus_mario, c, 0, -1);
        cutscene_event(cutscene_dance_closeup_fly_above, c, 0, 62);
        cutscene_event(cutscene_dance_closeup_fly_closer, c, 63, -1);
        cutscene_event(cutscene_dance_closeup_zoom, c, 63, 63);
        cutscene_event(cutscene_dance_closeup_shake_fov, c, 70, 70);
    } else {
        cutscene_event(cutscene_dance_closeup_start, c, 0, 0);
        cutscene_event(cutscene_dance_closeup_focus_mario, c, 0, -1);
        // Almost twice as fast as under water
        cutscene_event(cutscene_dance_closeup_fly_above, c, 0, 32);
        cutscene_event(cutscene_dance_closeup_fly_closer, c, 33, -1);
        cutscene_event(cutscene_dance_closeup_zoom, c, 33, 33);
        cutscene_event(cutscene_dance_closeup_shake_fov, c, 40, 40);
    }
    set_handheld_shake(HAND_CAM_SHAKE_CUTSCENE);
}

struct Cutscene sCutsceneDanceCloseup[] = {
    { cutscene_dance_closeup, CUTSCENE_LOOP }
};
