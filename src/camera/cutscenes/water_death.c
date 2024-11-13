#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"


/**
 * Moves the camera to Mario's side when Mario starts ACT_WATER_DEATH
 * Note that ACT_WATER_DEATH only starts when Mario gets hit by an enemy under water. It does not start
 * when Mario drowns.
 */
void water_death_move_to_mario_side(struct Camera *c) {
    f32 dist;
    s16 pitch, yaw;

    vec3f_get_dist_and_angle(sMarioCamState->pos, c->pos, &dist, &pitch, &yaw);
    approach_s16_asymptotic_bool(&yaw, (sMarioCamState->faceAngle[1] - 0x3000), 8);
    vec3f_set_dist_and_angle(sMarioCamState->pos, c->pos, dist, pitch, yaw);
}

/**
 * Unused cutscene for ACT_WATER_DEATH, which happens when Mario gets hit by an enemy under water.
 */
struct Cutscene sCutsceneWaterDeath[] = {
    { cutscene_quicksand_death, CUTSCENE_LOOP }
};
