#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Star dance cutscene.
 * The camera moves closer and rotates clockwise around Mario.
 */

/**
 * cvar8.point[2] is the amount to increase distance from Mario
 */
void cutscene_dance_fly_away_start(struct Camera *c) {
    Vec3f areaCenter;

    vec3f_copy(sCutsceneVars[9].point, c->focus);
    sCutsceneVars[8].point[2] = 65.f;

    if (c->mode == CAMERA_MODE_RADIAL) {
        vec3f_set(areaCenter, c->areaCenX, c->areaCenY, c->areaCenZ);
        c->yaw = calculate_yaw(areaCenter, c->pos);
        c->nextYaw = c->yaw;
    }

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    // Restrict the camera yaw in tight spaces
    if ((gLastCompletedStarNum == 6) && (gCurrCourseNum == COURSE_CCM)) {
        star_dance_bound_yaw(c, 0x5600, 0x800);
    }
    if ((gLastCompletedStarNum == 2) && (gCurrCourseNum == COURSE_TTM)) {
        star_dance_bound_yaw(c, 0x0,    0x800);
    }
    if ((gLastCompletedStarNum == 1) && (gCurrCourseNum == COURSE_SL)) {
        star_dance_bound_yaw(c, 0x2000, 0x800);
    }
    if ((gLastCompletedStarNum == 3) && (gCurrCourseNum == COURSE_RR)) {
        star_dance_bound_yaw(c, 0x0,    0x800);
    }
#endif
}

void cutscene_dance_fly_away_approach_mario(struct Camera *c) {
    s16 pitch, yaw;
    f32 dist;

    vec3f_get_dist_and_angle(sMarioCamState->pos, c->pos, &dist, &pitch, &yaw);
    approach_f32_asymptotic_bool(&dist, 600.f, 0.3f);
    approach_s16_asymptotic_bool(&pitch, 0x1000, 16);
    approach_s16_asymptotic_bool(&yaw, c->yaw, 8);
    vec3f_set_dist_and_angle(sMarioCamState->pos, c->pos, dist, pitch, yaw);
}

void cutscene_dance_fly_away_focus_mario(struct Camera *c) {
    Vec3f marioPos;

    vec3f_set(marioPos, sMarioCamState->pos[0], sMarioCamState->pos[1] + 125.f, sMarioCamState->pos[2]);
    approach_vec3f_asymptotic(sCutsceneVars[9].point, marioPos, 0.2f, 0.2f, 0.2f);
    vec3f_copy(c->focus, sCutsceneVars[9].point);
}

/**
 * Slowly pan the camera downwards and to the camera's right, using cvar9's angle.
 */
void cutscene_pan_cvar9(struct Camera *c) {
    vec3f_copy(c->focus, sCutsceneVars[9].point);
    sCutsceneVars[9].angle[0] -= 29;
    sCutsceneVars[9].angle[1] += 29;
    pan_camera(c, sCutsceneVars[9].angle[0], sCutsceneVars[9].angle[1]);
}

/**
 * Move backwards and rotate slowly around Mario.
 */
void cutscene_dance_fly_rotate_around_mario(struct Camera *c) {
    cutscene_pan_cvar9(c);
    rotate_and_move_vec3f(c->pos, sMarioCamState->pos, sCutsceneVars[8].point[2], 0, 0);
}

/**
 * Rotate quickly while Lakitu flies up.
 */
void cutscene_dance_fly_away_rotate_while_flying(struct Camera *c) {
    rotate_and_move_vec3f(c->pos, sMarioCamState->pos, 0, 0, 0x80);
}

void cutscene_dance_fly_away_shake_fov(UNUSED struct Camera *c) {
    set_fov_shake(0x400, 0x30, 0x8000);
}

/**
 * After collecting the star, Lakitu flies upwards out of the course.
 */
void cutscene_dance_fly_away(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    cutscene_event(cutscene_dance_fly_away_start, c, 0, 0);
    cutscene_event(cutscene_dance_fly_away_focus_mario, c, 0, 30);
    cutscene_event(cutscene_dance_fly_away_approach_mario, c, 0, 30);
    cutscene_event(cutscene_dance_fly_rotate_around_mario, c, 55, 124);
    cutscene_event(cutscene_dance_fly_away_rotate_while_flying, c, 55, 124);
    cutscene_event(cutscene_dance_fly_away_shake_fov, c, 40, 40);
    set_fov_function(CAM_FOV_DEFAULT);
    set_handheld_shake(HAND_CAM_SHAKE_STAR_DANCE);
}


struct Cutscene sCutsceneDanceFlyAway[] = {
    { cutscene_dance_fly_away, CUTSCENE_LOOP }
};
