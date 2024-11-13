#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Cutscene that plays when entering bowser's arenas.
 */

void cutscene_bowser_area_shake_fov(UNUSED struct Camera *c) {
    cutscene_set_fov_shake_preset(2);
}

/**
 * Set oBowserCamAct to 1, which causes bowser to start walking.
 */
void cutscene_bowser_area_start_bowser_walking(UNUSED struct Camera *c) {
    gSecondCameraFocus->oBowserCamAct = BOWSER_CAM_ACT_WALK;
}

/**
 * Offset the camera from bowser using cvar2 and cvar3
 * @bug cvar2.point is (0,0,0) on the first frame, but because of the warp transition, this behavior
 *      isn't seen. After the first frame, cvar2.point is bowser's position.
 */
void cutscene_bowser_arena_set_pos(struct Camera *c) {
    vec3f_set_dist_and_angle(sCutsceneVars[2].point, c->pos, sCutsceneVars[3].point[2],
                                  sCutsceneVars[3].angle[0], sCutsceneVars[3].angle[1]);
    vec3f_set(sCutsceneVars[2].point, gSecondCameraFocus->oPosX, gSecondCameraFocus->oPosY,
              gSecondCameraFocus->oPosZ);
}

/**
 * Apply a sine wave to the focus's y coordinate.
 * The y offset starts at 120, then decreases to 0 before reaching ~240 on the last frame.
 */
void cutscene_bowser_arena_focus_sine(UNUSED struct Camera *c) {
    // cvar4 was zeroed when the cutscene started.
    f32 yOff = sins(sCutsceneVars[4].angle[1]) * 120.0f + 120.0f;
    sCutsceneVars[4].angle[1] -= 0x200;
    approach_f32_asymptotic_bool(&sCutsceneVars[0].point[1], yOff, 0.5f);
}

/**
 * Set the camera focus according to cvar0 and cvar2.
 */
void cutscene_bowser_arena_set_focus(struct Camera *c) {
    offset_rotated(c->focus, sCutsceneVars[2].point, sCutsceneVars[0].point, sCutsceneVars[2].angle);
}

/**
 * Adjust the cvar offsets, making the camera look up, move slightly further back, and focus a little
 * further in front of bowser.
 */
void cutscene_bowser_arena_adjust_offsets(UNUSED struct Camera *c) {
    approach_s16_asymptotic_bool(&sCutsceneVars[3].angle[0], 0x6C8, 30);
    approach_f32_asymptotic_bool(&sCutsceneVars[0].point[2], -200.f, 0.02f);
    approach_f32_asymptotic_bool(&sCutsceneVars[3].point[2], 550.f, 0.02f);
}

/**
 * Decrease cvar0's z offset, making the camera focus pan left towards bowser.
 */
void cutscene_bowser_arena_pan_left(UNUSED struct Camera *c) {
    approach_f32_asymptotic_bool(&sCutsceneVars[0].point[2], 0.f, 0.05f);
}

/**
 * Active for the first 5 frames of the cutscene.
 * cvar3 is the camera's polar offset from bowser
 * cvar2.angle is bowser's move angle
 *
 * cvar0 is the focus offset from bowser
 */
void cutscene_bowser_arena_start(struct Camera *c) {
    sCutsceneVars[3].point[2] = 430.f;
    sCutsceneVars[3].angle[1] = gSecondCameraFocus->oMoveAngleYaw - DEGREES(45);
    sCutsceneVars[3].angle[0] = 0xD90;

    //! Tricky math: Bowser starts at (0, 307, -1000), with a moveAngle of (0,0,0). A sane person would
    //! expect this offset to move the focus to (0, 427, -1800).
    //! BUT because offset_rotated() flips the Z direction (to match sm64's coordinate system), this
    //! offset actually moves the focus to (0, 427, -200)
    vec3f_set(sCutsceneVars[0].point, 0.f, 120.f, -800.f);
    vec3s_set(sCutsceneVars[2].angle, gSecondCameraFocus->oMoveAnglePitch,
              gSecondCameraFocus->oMoveAngleYaw, gSecondCameraFocus->oMoveAngleRoll);

    // Set the camera's position and focus.
    cutscene_bowser_arena_set_pos(c);
    cutscene_bowser_arena_set_focus(c);
}

/**
 * Create the dialog box depending on which bowser fight Mario is in.
 */
void bowser_fight_intro_dialog(UNUSED struct Camera *c) {
    s16 dialog;

    switch (gCurrLevelNum) {
        case LEVEL_BOWSER_1:
            dialog = DIALOG_067;
            break;
        case LEVEL_BOWSER_2:
            dialog = DIALOG_092;
            break;
        default: // LEVEL_BOWSER_3
            dialog = DIALOG_093;
    }

    create_dialog_box(dialog);
}

/**
 * Create the dialog box and wait until it's gone.
 */
void cutscene_bowser_arena_dialog(struct Camera *c) {
    cutscene_event(bowser_fight_intro_dialog, c, 0, 0);

    if (get_dialog_id() == DIALOG_NONE) {
        gCutsceneTimer = CUTSCENE_LOOP;
    }
}

/**
 * End the bowser arena cutscene.
 */
void cutscene_bowser_arena_end(struct Camera *c) {
    cutscene_stop_dialog(c);
    c->cutscene = 0;
    transition_next_state(c, 20);
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
    sModeOffsetYaw = sMarioCamState->faceAngle[1] + DEGREES(90);
    gSecondCameraFocus->oBowserCamAct = BOWSER_CAM_ACT_END;
}

/**
 * Cutscene that plays when Mario enters a bowser fight.
 */
void cutscene_bowser_arena(struct Camera *c) {
    //! This does nothing, but may have been used in development
    cutscene_spawn_obj(CUTSCENE_OBJ_UNUSED_2, 0);

    if (gSecondCameraFocus != NULL) {
        cutscene_event(cutscene_mario_dialog_look_front, c, 0, -1);
        cutscene_event(cutscene_bowser_arena_start, c, 0, 5);
        cutscene_event(cutscene_bowser_area_start_bowser_walking, c, 40, 40);
        cutscene_event(cutscene_bowser_area_shake_fov, c, 145, 145);
        cutscene_event(cutscene_bowser_arena_set_pos, c, 40, -1);
        cutscene_event(cutscene_bowser_arena_pan_left, c, 40, 99);
        cutscene_event(cutscene_bowser_arena_adjust_offsets, c, 100, -1);
        cutscene_event(cutscene_bowser_arena_focus_sine, c, 40, 140);
        cutscene_event(cutscene_bowser_arena_set_focus, c, 40, -1);
        cutscene_event(cutscene_shake_explosion, c, 60, 60);
        cutscene_event(cutscene_shake_explosion, c, 82, 82);
        cutscene_event(cutscene_shake_explosion, c, 109, 109);
        cutscene_event(cutscene_shake_explosion, c, 127, 127);
    }
}

struct Cutscene sCutsceneEnterBowserArena[] = {
    { cutscene_bowser_arena, 180 },
    { cutscene_bowser_arena_dialog, CUTSCENE_LOOP },
    { cutscene_bowser_arena_end, 0 }
};
