#include "seq_ids.h"
#include "audio/external.h"
#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * The intro of the game. Peach reads her letter and Lakitu flies down to Mario's warp pipe.
 */

/**
 * Plays the background music that starts while peach reads the intro message.
 */
void cutscene_intro_peach_play_message_music(void) {
    play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, SEQ_EVENT_PEACH_MESSAGE), 0);
}

/**
 * Plays the music that starts after peach fades and Lakitu appears.
 */
void cutscene_intro_peach_play_lakitu_flying_music(void) {
    play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(15, SEQ_EVENT_CUTSCENE_INTRO), 0);
}

/**
 * Lower the volume (US only) and start the peach letter background music
 */
void cutscene_intro_peach_start_letter_music(UNUSED struct Camera *c) {
#if defined(VERSION_US) || defined(VERSION_SH)
    seq_player_lower_volume(SEQ_PLAYER_LEVEL, 60, 40);
#endif
    cutscene_intro_peach_play_message_music();
}

/**
 * Raise the volume (not in JP) and start the flying music.
 */
void cutscene_intro_peach_start_flying_music(UNUSED struct Camera *c) {
    seq_player_unlower_volume(SEQ_PLAYER_LEVEL, 60);
    cutscene_intro_peach_play_lakitu_flying_music();
}

#ifdef VERSION_EU
/**
 * Lower the volume for the letter background music. In US, this happens on the same frame as the music
 * starts.
 */
void cutscene_intro_peach_eu_lower_volume(UNUSED struct Camera *c) {
    seq_player_lower_volume(SEQ_PLAYER_LEVEL, 60, 40);
}
#endif

/**
 * Move the camera along `positionSpline` and point its focus at the corresponding point along
 * `focusSpline`. sCutsceneSplineSegmentProgress is updated after pos and focus are calculated.
 */
s32 intro_peach_move_camera_start_to_pipe(struct Camera *c, struct CutsceneSplinePoint positionSpline[],
                  struct CutsceneSplinePoint focusSpline[]) {
    Vec3f offset;
    s32 posReturn = 0;
    s32 focusReturn = 0;

    /**
     * The position spline's speed parameters are all 0, so sCutsceneSplineSegmentProgress doesn't get
     * updated. Otherwise position would move two frames ahead, and c->focus would always be one frame
     * further along the spline than c->pos.
     */
    posReturn = move_point_along_spline(c->pos, positionSpline, &sCutsceneSplineSegment, &sCutsceneSplineSegmentProgress);
    focusReturn = move_point_along_spline(c->focus, focusSpline, &sCutsceneSplineSegment, &sCutsceneSplineSegmentProgress);

    // The two splines used by this function are reflected in the horizontal plane for some reason,
    // so they are rotated every frame. Why do this, Nintendo?
    rotate_in_xz(c->focus, c->focus, DEGREES(-180));
    rotate_in_xz(c->pos, c->pos, DEGREES(-180));

    vec3f_set(offset, -1328.f, 260.f, 4664.f);
    vec3f_add(c->focus, offset);
    vec3f_add(c->pos, offset);

    posReturn += focusReturn; // Unused
    return focusReturn;
}

/**
 * Create a dialog box with the letter text
 */
void peach_letter_text(UNUSED struct Camera *c) {
    create_dialog_box(DIALOG_020);
}

#ifndef VERSION_JP
void play_sound_peach_reading_letter(UNUSED struct Camera *c) {
    play_sound(SOUND_PEACH_DEAR_MARIO, gGlobalSoundSource);
}
#endif

/**
 * Move the camera from peach reading the letter all the way to Mario's warp pipe. Follow the
 * sIntroStartToPipe splines.
 */
void cutscene_intro_peach_start_to_pipe_spline(struct Camera *c) {
    static struct CutsceneSplinePoint sIntroStartToPipePosition[] = {
        { 0, 0, { 2122, 8762, 9114 } },  { 0, 0, { 2122, 8762, 9114 } },  { 1, 0, { 2122, 7916, 9114 } },
        { 1, 0, { 2122, 7916, 9114 } },  { 2, 0, { 957, 5166, 8613 } },   { 3, 0, { 589, 4338, 7727 } },
        { 4, 0, { 690, 3366, 6267 } },   { 5, 0, { -1600, 2151, 4955 } }, { 6, 0, { -1557, 232, 1283 } },
        { 7, 0, { -6962, -295, 2729 } }, { 8, 0, { -6979, 131, 3246 } },  { 9, 0, { -6360, -283, 4044 } },
        { 0, 0, { -5695, -334, 5264 } }, { 1, 0, { -5568, -319, 7933 } }, { 2, 0, { -3848, -200, 6278 } },
        { 3, 0, { -965, -263, 6092 } },  { 4, 0, { 1607, 2465, 6329 } },  { 5, 0, { 2824, 180, 3548 } },
        { 6, 0, { 1236, 136, 945 } },    { 0, 0, { 448, 136, 564 } },     { 0, 0, { 448, 136, 564 } },
        { 0, 0, { 448, 136, 564 } },     { -1, 0, { 448, 136, 564 } }
    };
    static struct CutsceneSplinePoint sIntroStartToPipeFocus[] = {
        { 0, 50, { 1753, 29800, 8999 } }, { 0, 50, { 1753, 29800, 8999 } },
        { 1, 50, { 1753, 8580, 8999 } },  { 1, 100, { 1753, 8580, 8999 } },
        { 2, 50, { 520, 5400, 8674 } },   { 3, 50, { 122, 4437, 7875 } },
        { 4, 50, { 316, 3333, 6538 } },   { 5, 36, { -1526, 2189, 5448 } },
        { 6, 50, { -1517, 452, 1731 } },  { 7, 50, { -6659, -181, 3109 } },
        { 8, 17, { -6649, 183, 3618 } },  { 9, 20, { -6009, -214, 4395 } },
        { 0, 50, { -5258, -175, 5449 } }, { 1, 36, { -5158, -266, 7651 } },
        { 2, 26, { -3351, -192, 6222 } }, { 3, 25, { -483, -137, 6060 } },
        { 4, 100, { 1833, 2211, 5962 } }, { 5, 26, { 3022, 207, 3090 } },
        { 6, 20, { 1250, 197, 449 } },    { 7, 50, { 248, 191, 227 } },
        { 7, 0, { 48, 191, 227 } },       { 7, 0, { 48, 191, 227 } },
        { -1, 0, { 48, 191, 227 } }
    };

    if (intro_peach_move_camera_start_to_pipe(c, sIntroStartToPipePosition, sIntroStartToPipeFocus) != 0) {
        gCameraMovementFlags &= ~CAM_MOVE_C_UP_MODE;
        gCutsceneTimer = CUTSCENE_LOOP;
    }
}

/**
 * Loop the cutscene until Mario exits the dialog.
 */
void cutscene_intro_peach_dialog(struct Camera *c) {
    if (get_dialog_id() == DIALOG_NONE) {
        vec3f_copy(gLakituState.goalPos, c->pos);
        vec3f_copy(gLakituState.goalFocus, c->focus);
        sStatusFlags |= (CAM_FLAG_SMOOTH_MOVEMENT | CAM_FLAG_UNUSED_CUTSCENE_ACTIVE);
        gCutsceneTimer = CUTSCENE_STOP;
        c->cutscene = 0;
    }
}

/**
 * Describes the spline the camera follows, starting when the camera jumps to Lakitu and ending after
 * Mario jumps out of the pipe when the first dialog opens.
 */
void cutscene_intro_peach_follow_pipe_spline(struct Camera *c) {
    static struct CutsceneSplinePoint sIntroPipeToDialogPosition[] = {
        { 0, 0, { -785, 625, 4527 } },  { 1, 0, { -785, 625, 4527 } },  { 2, 0, { -1286, 644, 4376 } },
        { 3, 0, { -1286, 623, 4387 } }, { 4, 0, { -1286, 388, 3963 } }, { 5, 0, { -1286, 358, 4093 } },
        { 6, 0, { -1386, 354, 4159 } }, { 7, 0, { -1477, 306, 4223 } }, { 8, 0, { -1540, 299, 4378 } },
        { 9, 0, { -1473, 316, 4574 } }, { 0, 0, { -1328, 485, 5017 } }, { 0, 0, { -1328, 485, 5017 } },
        { 0, 0, { -1328, 485, 5017 } }, { -1, 0, { -1328, 485, 5017 } }
    };

#ifdef VERSION_EU
    static struct CutsceneSplinePoint sIntroPipeToDialogFocus[] = {
        { 0, 25, { -1248, 450, 4596 } }, { 1, 71, { -1258, 485, 4606 } }, { 2, 71, { -1379, 344, 4769 } },
        { 3, 22, { -1335, 366, 4815 } }, { 4, 23, { -1315, 370, 4450 } }, { 5, 40, { -1322, 333, 4591 } },
        { 6, 25, { -1185, 329, 4616 } }, { 7, 21, { -1059, 380, 4487 } }, { 8, 14, { -1086, 421, 4206 } },
        { 9, 21, { -1321, 346, 4098 } }, { 0, 0, { -1328, 385, 4354 } },  { 0, 0, { -1328, 385, 4354 } },
        { 0, 0, { -1328, 385, 4354 } },  { -1, 0, { -1328, 385, 4354 } }
    };
#else
    static struct CutsceneSplinePoint sIntroPipeToDialogFocus[] = {
        { 0, 20, { -1248, 450, 4596 } }, { 1, 59, { -1258, 485, 4606 } }, { 2, 59, { -1379, 344, 4769 } },
        { 3, 20, { -1335, 366, 4815 } }, { 4, 23, { -1315, 370, 4450 } }, { 5, 40, { -1322, 333, 4591 } },
        { 6, 25, { -1185, 329, 4616 } }, { 7, 21, { -1059, 380, 4487 } }, { 8, 14, { -1086, 421, 4206 } },
        { 9, 21, { -1321, 346, 4098 } }, { 0, 0, { -1328, 385, 4354 } },  { 0, 0, { -1328, 385, 4354 } },
        { 0, 0, { -1328, 385, 4354 } },  { -1, 0, { -1328, 385, 4354 } }
    };
#endif

    move_point_along_spline(c->pos, sIntroPipeToDialogPosition, &sCutsceneSplineSegment, &sCutsceneSplineSegmentProgress);
    move_point_along_spline(c->focus, sIntroPipeToDialogFocus, &sCutsceneSplineSegment, &sCutsceneSplineSegmentProgress);
}

void cutscene_intro_peach_clear_cutscene_status(UNUSED struct Camera *c) {
    sMarioCamState->cameraEvent = CAM_EVENT_NONE;
}

/**
 * Set fov to 8 degrees, then zoom out to 30.
 */
void cutscene_intro_peach_zoom_fov(UNUSED struct Camera *c) {
    sFOVState.fov = 8.f;
    set_fov_function(CAM_FOV_ZOOM_30);
}

/**
 * Reset the spline progress, turn on handheld shake.
 */
void cutscene_intro_peach_reset_spline(UNUSED struct Camera *c) {
    sCutsceneSplineSegment = 0;
    sCutsceneSplineSegmentProgress = 0.1f;
    //! @bug since this event is only called for one frame, this handheld shake is turned off on the
    //! next frame.
    set_handheld_shake(HAND_CAM_SHAKE_HIGH);
}

/**
 * Turn off handheld shake. This was likely written before handheld shake was changed to turn off every
 * frame, as it's the only instance of HAND_CAM_SHAKE_OFF.
 */
void cutscene_intro_peach_handheld_shake_off(UNUSED struct Camera *c) {
    set_handheld_shake(HAND_CAM_SHAKE_OFF);
}

void intro_pipe_exit_text(UNUSED struct Camera *c) {
    create_dialog_box(DIALOG_033);
}

void play_sound_intro_turn_on_hud(UNUSED struct Camera *c) {
    play_sound_rbutton_changed();
}

/**
 * Fly to the pipe. Near the end, the camera jumps to Lakitu's position and the hud turns on.
 */
void cutscene_intro_peach_fly_to_pipe(struct Camera *c) {
    cutscene_event(play_sound_intro_turn_on_hud, c, 818, 818);
    cutscene_spawn_obj(6, 1);
    cutscene_event(cutscene_intro_peach_start_flying_music, c, 0, 0);
    cutscene_event(cutscene_intro_peach_start_to_pipe_spline, c, 0, -1);
    cutscene_event(cutscene_intro_peach_clear_cutscene_status, c, 717, 717);
    clamp_pitch(c->pos, c->focus, 0x3B00, -0x3B00);
    sCutsceneVars[1].point[1] = 400.f;
}

/**
 * Lakitu flies around the warp pipe, then Mario jumps out.
 */
void cutscene_intro_peach_mario_appears(struct Camera *c) {
    sMarioCamState->cameraEvent = 0;
    cutscene_event(cutscene_intro_peach_reset_spline, c, 0, 0);
    cutscene_event(cutscene_intro_peach_follow_pipe_spline, c, 0, -1);
    cutscene_event(cutscene_intro_peach_handheld_shake_off, c, 70, 70);
    cutscene_event(intro_pipe_exit_text, c, 250, 250);

    approach_f32_asymptotic_bool(&sCutsceneVars[1].point[1], 80.f + sMarioGeometry.currFloorHeight +
                                 (sMarioCamState->pos[1] - sMarioGeometry.currFloorHeight) * 1.1f, 0.4f);

    // Make the camera look up as Mario jumps out of the pipe
    if (c->focus[1] < sCutsceneVars[1].point[1]) {
        c->focus[1] = sCutsceneVars[1].point[1];
    }

    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
}

/**
 * Reset the fov. This gives the effect of peach zooming out as she fades.
 */
void cutscene_intro_peach_reset_fov(UNUSED struct Camera *c) {
    set_fov_function(CAM_FOV_DEFAULT);
}

/**
 * Peach reads the letter to Mario.
 */
void cutscene_intro_peach_letter(struct Camera *c) {
    cutscene_spawn_obj(CUTSCENE_OBJ_BEGINNING_PEACH, 0);
    cutscene_event(cutscene_intro_peach_zoom_fov, c, 0, 0);
    cutscene_event(cutscene_intro_peach_start_letter_music, c, 65, 65);
    cutscene_event(cutscene_intro_peach_start_to_pipe_spline, c, 0, 0);
    cutscene_event(peach_letter_text, c, 65, 65);
#ifndef VERSION_JP
    cutscene_event(play_sound_peach_reading_letter, c, 83, 83);
#endif

    if ((gCutsceneTimer > 120) && (get_dialog_id() == DIALOG_NONE)) {
        // Start the next scene
        gCutsceneTimer = CUTSCENE_LOOP;
    }

    clamp_pitch(c->pos, c->focus, 0x3B00, -0x3B00);
}

struct Cutscene sCutsceneIntroPeach[] = {
    { cutscene_intro_peach_letter, CUTSCENE_LOOP },
    { cutscene_intro_peach_reset_fov, 35 },
#ifdef VERSION_EU
    { cutscene_intro_peach_fly_to_pipe, 675 },
#else
    { cutscene_intro_peach_fly_to_pipe, 820 },
#endif
    { cutscene_intro_peach_mario_appears, 270 },
    { cutscene_intro_peach_dialog, CUTSCENE_LOOP }
};
