#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Cutscene that plays when Mario unlocks the basement or upstairs key door.
 */

/**
 * Sets cvars:
 * cvar0 is the camera's position
 * cvar1 is the camera's focus
 * cvar2 is the goal position
 * cvar3 is the goal focus
 */
void cutscene_unlock_key_door_start(struct Camera *c) {
    Vec3f posOff, focusOff;

    vec3f_copy(sCutsceneVars[0].point, c->pos);
    vec3f_copy(sCutsceneVars[1].point, c->focus);
    vec3f_set(posOff, -206.f, 108.f, 234.f);
    vec3f_set(focusOff, 48.f, 104.f, -193.f);
    offset_rotated(sCutsceneVars[2].point, sMarioCamState->pos, posOff, sMarioCamState->faceAngle);
    offset_rotated(sCutsceneVars[3].point, sMarioCamState->pos, focusOff, sMarioCamState->faceAngle);
}

/**
 * Move the camera to the cvars position and focus, closer to Mario.
 * Gives a better view of the key.
 */
void cutscene_unlock_key_door_approach_mario(struct Camera *c) {
    approach_vec3f_asymptotic(c->pos, sCutsceneVars[2].point, 0.1f, 0.1f, 0.1f);
    approach_vec3f_asymptotic(c->focus, sCutsceneVars[3].point, 0.1f, 0.1f, 0.1f);
}

/**
 * Move the camera focus up a bit, focusing on the key in the lock.
 */
void cutscene_unlock_key_door_focus_lock(UNUSED struct Camera *c) {
    approach_f32_asymptotic_bool(&sCutsceneVars[3].point[1], sMarioCamState->pos[1] + 140.f, 0.07f);
}

void cutscene_unlock_key_door_stub(UNUSED struct Camera *c) {
}

/**
 * Move back to the previous pos and focus, stored in cvar0 and cvar1.
 */
void cutscene_unlock_key_door_fly_back(struct Camera *c) {
    approach_vec3f_asymptotic(c->pos, sCutsceneVars[0].point, 0.1f, 0.1f, 0.1f);
    approach_vec3f_asymptotic(c->focus, sCutsceneVars[1].point, 0.1f, 0.1f, 0.1f);
}

/**
 * Shake the camera's fov when the key is put in the lock.
 */
void cutscene_unlock_key_door_fov_shake(UNUSED struct Camera *c) {
    cutscene_set_fov_shake_preset(1);
}

/**
 * Cutscene that plays when Mario unlocks a key door.
 */
void cutscene_unlock_key_door(UNUSED struct Camera *c) {
    cutscene_event(cutscene_unlock_key_door_start, c, 0, 0);
    cutscene_event(cutscene_unlock_key_door_approach_mario, c, 0, 123);
    cutscene_event(cutscene_unlock_key_door_fly_back, c, 124, -1);
    cutscene_event(cutscene_unlock_key_door_fov_shake, c, 79, 79);
    cutscene_event(cutscene_unlock_key_door_focus_lock, c, 70, 110);
    cutscene_event(cutscene_unlock_key_door_stub, c, 112, 112);
}

struct Cutscene sCutsceneUnlockKeyDoor[] = {
    { cutscene_unlock_key_door, 200 },
    { cutscene_double_doors_end, 0 }
};
