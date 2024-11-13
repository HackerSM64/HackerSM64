#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Star dance cutscene.
 * The camera moves closer and rotates clockwise around Mario.
 */

/**
 * Jump the camera pos and focus to cvar 8 and 7.
 * Called every frame, starting after 10, so when these cvars are updated, the camera will jump.
 */
void cutscene_key_dance_jump_cvar(struct Camera *c) {
    offset_rotated(c->pos, sMarioCamState->pos, sCutsceneVars[8].point, sMarioCamState->faceAngle);
    offset_rotated(c->focus, sMarioCamState->pos, sCutsceneVars[7].point, sMarioCamState->faceAngle);
}

/**
 * Jump to a closeup view of Mario and the key.
 */
void cutscene_key_dance_jump_closeup(UNUSED struct Camera *c) {
    vec3f_set(sCutsceneVars[8].point, 38.f, 171.f, -248.f);
    vec3f_set(sCutsceneVars[7].point, -57.f, 51.f, 187.f);
}

/**
 * Jump to a view from the lower left (Mario's right).
 */
void cutscene_key_dance_jump_lower_left(UNUSED struct Camera *c) {
    vec3f_set(sCutsceneVars[8].point, -178.f, 62.f, -132.f);
    vec3f_set(sCutsceneVars[7].point, 299.f, 91.f, 58.f);
}

/**
 * Jump to a rotated view from above.
 */
void cutscene_key_dance_jump_above(UNUSED struct Camera *c) {
    gLakituState.keyDanceRoll = 0x2800;
    vec3f_set(sCutsceneVars[8].point, 89.f, 373.f, -304.f);
    vec3f_set(sCutsceneVars[7].point, 0.f, 127.f, 0.f);
}

/**
 * Finally, jump to a further view, slightly to Mario's left.
 */
void cutscene_key_dance_jump_last(UNUSED struct Camera *c) {
    gLakituState.keyDanceRoll = 0;
    vec3f_set(sCutsceneVars[8].point, 135.f, 158.f, -673.f);
    vec3f_set(sCutsceneVars[7].point, -20.f, 135.f, -198.f);
}

void cutscene_key_dance_shake_fov(UNUSED struct Camera *c) {
    set_fov_shake(0x180, 0x30, 0x8000);
}

void cutscene_key_dance_handheld_shake(UNUSED struct Camera *c) {
    set_handheld_shake(HAND_CAM_SHAKE_CUTSCENE);
}

void cutscene_key_dance_focus_mario(struct Camera *c) {
    focus_in_front_of_mario(c, 0, 0.2f);
}

/**
 * Cutscene that plays when Mario collects a key from bowser. It's basically a sequence of four jump
 * cuts.
 */
void cutscene_key_dance(struct Camera *c) {
    cutscene_event(cutscene_dance_move_to_mario, c, 0, 10);
    cutscene_event(cutscene_key_dance_focus_mario, c, 0, 10);
    cutscene_event(cutscene_key_dance_jump_closeup, c, 0, 0);
    cutscene_event(cutscene_key_dance_jump_lower_left, c, 20, 20);
    cutscene_event(cutscene_key_dance_jump_above, c, 35, 35);
    cutscene_event(cutscene_key_dance_jump_last, c, 52, 52);
    cutscene_event(cutscene_key_dance_jump_cvar, c, 11, -1);
    cutscene_event(cutscene_key_dance_shake_fov, c, 54, 54);
    cutscene_event(cutscene_key_dance_handheld_shake, c, 52, -1);
}

struct Cutscene sCutsceneKeyDance[] = {
    { cutscene_key_dance, CUTSCENE_LOOP }
};
