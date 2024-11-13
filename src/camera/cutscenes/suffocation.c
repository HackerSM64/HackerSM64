#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game/camera.h"

/**
 * Cutscene that plays when Mario suffocates.
 */

/**
 * Fly away from Mario near the end of the cutscene.
 */
void cutscene_suffocation_fly_away(UNUSED struct Camera *c) {
    Vec3f target;
    Vec3f offset = { 0, 20.f, 120.f };

    offset_rotated(target, sMarioCamState->pos, offset, sMarioCamState->faceAngle);
    approach_vec3f_asymptotic(sCutsceneVars[3].point, target, 0.1f, 0.1f, 0.1f);
}

/**
 * Keep Lakitu above the gas level.
 */
void cutscene_suffocation_stay_above_gas(struct Camera *c) {
    cutscene_goto_cvar_pos(c, 400.f, 0x2800, 0x200, 0);
    f32 gasLevel = find_poison_gas_level(sMarioCamState->pos[0], sMarioCamState->pos[2]);

    if (gasLevel != FLOOR_LOWER_LIMIT) {
        if ((gasLevel += 130.f) > c->pos[1]) {
            c->pos[1] = gasLevel;
        }
    }
}

/**
 * Quickly rotate around Mario.
 */
void cutscene_suffocation_rotate(struct Camera *c) {
    f32 dist;
    s16 pitch, yaw;

    vec3f_get_dist_and_angle(sMarioCamState->pos, c->pos, &dist, &pitch, &yaw);
    yaw += 0x100;
    vec3f_set_dist_and_angle(sMarioCamState->pos, c->pos, dist, pitch, yaw);
}

/**
 * Cutscene that plays when Mario dies from suffocation (ie due to HMC gas).
 */
void cutscene_suffocation(struct Camera *c) {
    cutscene_event(cutscene_death_stomach_start, c, 0, 0);
    cutscene_event(cutscene_suffocation_rotate, c, 0, -1);
    cutscene_event(cutscene_suffocation_stay_above_gas, c, 0, -1);
    cutscene_event(cutscene_suffocation_fly_away, c, 50, -1);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    set_handheld_shake(HAND_CAM_SHAKE_HIGH);
}

struct Cutscene sCutsceneSuffocation[] = {
    { cutscene_suffocation, CUTSCENE_LOOP }
};
