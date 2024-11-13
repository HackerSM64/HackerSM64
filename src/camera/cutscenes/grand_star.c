#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Cutscene that plays when Mario collects the grand star from bowser.
 */

/**
 * Start the grand star cutscene.
 * cvar0 is a relative offset from Mario.
 * cvar1 is the is the camera's goal position.
 */
void cutscene_grand_star_start(UNUSED struct Camera *c) {
    vec3f_set(sCutsceneVars[0].point, 0.f, 150.f, -600.f);
    offset_rotated(sCutsceneVars[1].point, sMarioCamState->pos, sCutsceneVars[0].point, sMarioCamState->faceAngle);
    sCutsceneVars[1].point[1] = 457.f;
}

/**
 * Make the camera fly to the front of Mario.
 */
void cutscene_grand_star_front_of_mario(struct Camera *c) {
    f32 goalDist;
    s16 goalPitch, goalYaw;
    f32 dist;
    s16 pitch, yaw;

    vec3f_get_dist_and_angle(sMarioCamState->pos, sCutsceneVars[1].point, &goalDist, &goalPitch, &goalYaw);
    vec3f_get_dist_and_angle(sMarioCamState->pos, c->pos, &dist, &pitch, &yaw);
    approach_f32_asymptotic_bool(&dist, goalDist, 0.1f);
    approach_s16_asymptotic_bool(&pitch, goalPitch, 32);
    approach_s16_asymptotic_bool(&yaw, goalYaw + 0x1200, 20);
    vec3f_set_dist_and_angle(sMarioCamState->pos, c->pos, dist, pitch, yaw);
}

/**
 * Started shortly after Mario starts the triple jump. Stores Mario's face angle and zeros cvar2.
 */
void cutscene_grand_star_mario_jump(UNUSED struct Camera *c) {
    vec3s_set(sCutsceneVars[0].angle, 0, sMarioCamState->faceAngle[1], 0);
    vec3_zero(sCutsceneVars[2].point);
}

/**
 * Accelerate cvar2 to point back and to the left (relative to the camera).
 */
void cutscene_grand_star_accel_cvar2(UNUSED struct Camera *c) {
    camera_approach_f32_symmetric_bool(&sCutsceneVars[2].point[2], -40.f, 2.0f);
    sCutsceneVars[2].point[0] = 5.0f;
}

/**
 * Decrease cvar2 offset, follow Mario by directly updating the camera's pos.
 */
void cutscene_grand_star_approach_mario(struct Camera *c) {
    camera_approach_f32_symmetric_bool(&sCutsceneVars[2].point[2], 0.f, 2.f);
    sCutsceneVars[2].point[0] = 0.f;
    approach_f32_asymptotic_bool(&c->pos[0], sMarioCamState->pos[0], 0.01f);
    approach_f32_asymptotic_bool(&c->pos[2], sMarioCamState->pos[2], 0.01f);
}

/**
 * Offset the camera's position by cvar2. Before Mario triple jumps, this moves back and to the left.
 * After the triple jump, cvar2 decelerates to 0.
 */
void cutscene_grand_star_move_cvar2(struct Camera *c) {
    offset_rotated(c->pos, c->pos, sCutsceneVars[2].point, sCutsceneVars[0].angle);
}

void cutscene_grand_star_focus_mario(struct Camera *c) {
    Vec3f foc;

    vec3f_set(foc, sMarioCamState->pos[0], (sMarioCamState->pos[1] - 307.f) * 0.5f + 407.f, sMarioCamState->pos[2]);
    approach_vec3f_asymptotic(c->focus, foc, 0.5f, 0.8f, 0.5f);
}

/**
 * The first part of the grand star cutscene, after Mario has collected the grand star.
 */
void cutscene_grand_star(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    cutscene_event(cutscene_grand_star_start, c, 0, 0);
    cutscene_event(cutscene_grand_star_front_of_mario, c, 0, 109);
    cutscene_event(cutscene_grand_star_focus_mario, c, 0, -1);
    cutscene_event(cutscene_grand_star_mario_jump, c, 110, 110);
    cutscene_event(cutscene_grand_star_accel_cvar2, c, 110, 159);
    cutscene_event(cutscene_grand_star_approach_mario, c, 160, -1);
    cutscene_event(cutscene_grand_star_move_cvar2, c, 110, -1);
}

/**
 * Zero the cvars that are used when Mario is flying.
 */
void cutscene_grand_star_fly_start(struct Camera *c) {
    //! cvar7 is unused in grand star
    vec3f_set(sCutsceneVars[7].point, 0.5f, 0.5f, 0.5f);
    //! cvar6 is unused in grand star
    vec3f_set(sCutsceneVars[6].point, 0.01f, 0.01f, 0.01f);
    vec3f_set(sCutsceneVars[4].point, 0.f, 0.f, 0.f);
    vec3f_set(sCutsceneVars[5].point, 0.f, c->focus[1] - sMarioCamState->pos[1], 0.f);
    sCutsceneVars[8].point[2] = 0.f;
    sCutsceneVars[8].point[0] = 0.f;
}

/**
 * Decrease the cvar offsets so that Lakitu flies closer to Mario.
 */
void cutscene_grand_star_fly_move_to_mario(UNUSED struct Camera *c) {
    Vec3f posOff;

    vec3f_set(posOff, -600.f, 0.f, -400.f);
    approach_vec3f_asymptotic(sCutsceneVars[4].point, posOff, 0.05f, 0.05f, 0.05f);
    camera_approach_f32_symmetric_bool(&sCutsceneVars[5].point[1], 0.f, 2.f);
    camera_approach_f32_symmetric_bool(&sCutsceneVars[5].point[2], -200.f, 6.f);
}

/**
 * Gradually increase the cvar offsets so Lakitu flies away. Mario flies offscreen to the right.
 *
 * cvar4 is the position offset from Mario.
 * cvar5 is the focus offset from Mario.
 * cvar8.point[0] is the approach velocity.
 */
void cutscene_grand_star_fly_mario_offscreen(UNUSED struct Camera *c) {
    camera_approach_f32_symmetric_bool(&sCutsceneVars[8].point[0], 15.f, 0.1f);

    camera_approach_f32_symmetric_bool(&sCutsceneVars[4].point[0], -2000.f, sCutsceneVars[8].point[0]);
    camera_approach_f32_symmetric_bool(&sCutsceneVars[4].point[1], 1200.f, sCutsceneVars[8].point[0] / 10.f);
    camera_approach_f32_symmetric_bool(&sCutsceneVars[4].point[2], 1000.f, sCutsceneVars[8].point[0] / 10.f);

    camera_approach_f32_symmetric_bool(&sCutsceneVars[5].point[0], 0.f, sCutsceneVars[8].point[0]);
    camera_approach_f32_symmetric_bool(&sCutsceneVars[5].point[1], 1200.f, sCutsceneVars[8].point[0] / 2);
    camera_approach_f32_symmetric_bool(&sCutsceneVars[5].point[2], 1000.f, sCutsceneVars[8].point[0] / 1.5f);
}

/**
 * Make Lakitu approach the cvars.
 * cvar4 is the position offset.
 * cvar5 is the focus offset.
 */
void cutscene_grand_star_fly_app_cvars(struct Camera *c) {
    Vec3f goalPos, goalFoc;
    f32 dist;
    s16 pitch, yaw;

    camera_approach_f32_symmetric_bool(&sCutsceneVars[8].point[2], 90.f, 2.5f);
    offset_rotated(goalPos, sMarioCamState->pos, sCutsceneVars[4].point, sMarioCamState->faceAngle);
    offset_rotated(goalFoc, sMarioCamState->pos, sCutsceneVars[5].point, sMarioCamState->faceAngle);

    // Move towards goalPos by cvar8's Z speed
    vec3f_get_dist_and_angle(goalPos, c->pos, &dist, &pitch, &yaw);
    camera_approach_f32_symmetric_bool(&dist, 0, sCutsceneVars[8].point[2]);
    vec3f_set_dist_and_angle(goalPos, c->pos, dist, pitch, yaw);

    approach_vec3f_asymptotic(c->pos, goalPos, 0.01f, 0.01f, 0.01f);
    approach_vec3f_asymptotic(c->focus, goalFoc, 0.5f, 0.8f, 0.5f);
}

/**
 * Part of the grand star cutscene, starts after Mario is flying.
 *
 * cvar4 and cvar5 are directions, relative to Mario:
 * cvar4 is used as the camera position's offset from Mario.
 * cvar5 is used as the camera focus's offset from Mario.
 *
 * cvar8.point[2] is Lakitu's speed.
 */
void cutscene_grand_star_fly(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    cutscene_event(cutscene_grand_star_fly_start, c, 0, 0);
    cutscene_event(cutscene_grand_star_fly_move_to_mario, c, 0, 140);
    cutscene_event(cutscene_grand_star_fly_mario_offscreen, c, 141, -1);
    cutscene_event(cutscene_grand_star_fly_app_cvars, c, 0, -1);
}

struct Cutscene sCutsceneGrandStar[] = {
    { cutscene_grand_star, 360 },
    { cutscene_grand_star_fly, CUTSCENE_LOOP }
};
