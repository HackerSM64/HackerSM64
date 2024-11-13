#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Cutscene that plays when Mario opens a sliding star door.
 */

/**
 * Set the camera pos relative to Mario.
 */
void cutscene_sliding_doors_open_start(struct Camera *c) {
    f32 dist;
    s16 pitch, yaw;

    vec3f_get_dist_and_angle(sMarioCamState->pos, c->pos, &dist, &pitch, &yaw);

    // If the camera is too close, warp it backwards set it to a better angle.
    if (dist < 500.f) {
        dist = 500.f;
        yaw = sMarioCamState->faceAngle[1] + 0x8800;
        pitch = 0x800;
    }

    vec3f_set_dist_and_angle(sMarioCamState->pos, c->pos, dist, pitch, yaw);
}

/**
 * cvar1: Mario's position
 * cvar0.angle: Mario's angle
 * cvar0.point: offset from Mario
 */
void cutscene_sliding_doors_open_set_cvars(UNUSED struct Camera *c) {
    vec3f_copy(sCutsceneVars[1].point, sMarioCamState->pos);
    vec3s_copy(sCutsceneVars[0].angle, sMarioCamState->faceAngle);
    vec3f_set(sCutsceneVars[0].point, 80.f, 325.f, 200.f);
}

/**
 * Decrease the cvar0 y offset to 75, which would simulate Lakitu flying under the doorway.
 * However, the initial y offset is too high for Lakitu to reach 75 in time.
 */
void cutscene_sliding_doors_go_under_doorway(UNUSED struct Camera *c) {
    camera_approach_f32_symmetric_bool(&sCutsceneVars[0].point[1], 75.f, 10.f);
}

/**
 * Approach a y offset of 125 again.
 */
void cutscene_sliding_doors_fly_back_up(UNUSED struct Camera *c) {
    camera_approach_f32_symmetric_bool(&sCutsceneVars[0].point[1], 125.f, 10.f);
}

/**
 * Follow Mario through the door, by approaching cvar1.point.
 */
void cutscene_sliding_doors_follow_mario(struct Camera *c) {
    Vec3f pos;
    vec3f_copy(pos, c->pos);

    // Update cvar1 with Mario's position (the y value doesn't change)
    sCutsceneVars[1].point[0] = sMarioCamState->pos[0];
    sCutsceneVars[1].point[2] = sMarioCamState->pos[2];

    // Decrease cvar0's offsets, moving the camera behind Mario at his eye height.
    approach_f32_asymptotic_bool(&sCutsceneVars[0].point[0], 0, 0.1f);
    camera_approach_f32_symmetric_bool(&sCutsceneVars[0].point[2], 125.f, 50.f);
    // Update cvar0's angle
    approach_vec3s_asymptotic(sCutsceneVars[0].angle, sMarioCamState->faceAngle, 16, 16, 16);

    // Apply the offset to the camera's position
    offset_rotated(pos, sCutsceneVars[1].point, sCutsceneVars[0].point, sCutsceneVars[0].angle);
    approach_vec3f_asymptotic(c->pos, pos, 0.15f, 0.05f, 0.15f);

    // Focus on Mario's eye height
    set_focus_rel_mario(c, 0, 125.f, 0, 0);
}

/**
 * Plays when Mario opens the sliding doors.
 * Note: the star door unlocking event is not a cutscene, it's handled by Mario separately.
 */
void cutscene_sliding_doors_open(struct Camera *c) {
    reset_pan_distance(c);
    cutscene_event(cutscene_sliding_doors_open_start, c, 0, 8);
    cutscene_event(cutscene_sliding_doors_open_set_cvars, c, 8, 8);
    cutscene_event(cutscene_sliding_doors_go_under_doorway, c, 8, 28);
    cutscene_event(cutscene_sliding_doors_fly_back_up, c, 29, -1);
    cutscene_event(cutscene_sliding_doors_follow_mario, c, 8, -1);
}

struct Cutscene sCutsceneSlidingDoorsOpen[] = {
    { cutscene_sliding_doors_open, 50 },
    { cutscene_double_doors_end, 0 }
};
