#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Cutscene that plays when Mario beats the game.
 */

/**
 * Set the camera position and focus for when Mario falls from the sky.
 */
void cutscene_ending_mario_fall_start(struct Camera *c) {
    vec3f_set(c->focus, -26.f, 0.f, -137.f);
    vec3f_set(c->pos, 165.f, 4725.f, 324.f);
}

/**
 * Focus on Mario when he's falling from the sky.
 */
void cutscene_ending_mario_fall_focus_mario(struct Camera *c) {
    Vec3f offset;
    vec3f_set(offset, 0.f, 80.f, 0.f);

    offset[2] = ABS(sMarioCamState->pos[1] - c->pos[1]) * -0.1f;
    if (offset[2] > -100.f) {
        offset[2] = -100.f;
    }

    offset_rotated(c->focus, sMarioCamState->pos, offset, sMarioCamState->faceAngle);
}

/**
 * Mario falls from the sky after the grand star cutscene.
 */
void cutscene_ending_mario_fall(struct Camera *c) {
    cutscene_event(cutscene_ending_mario_fall_start, c, 0, 0);
    cutscene_event(cutscene_ending_mario_fall_focus_mario, c, 0, -1);
    player2_rotate_cam(c, -0x2000, 0x2000, -0x2000, 0x2000);
}

/**
 * Closeup of Mario as the wing cap fades and Mario looks up.
 */
void cutscene_ending_mario_land_closeup(struct Camera *c) {
    vec3f_set(c->focus, 85.f, 826.f, 250.f);
    vec3f_set(c->pos, -51.f, 988.f, -202.f);
    player2_rotate_cam(c, -0x2000, 0x2000, -0x2000, 0x2000);
}

/**
 * Reset the spline progress and cvar9.
 */
void cutscene_ending_reset_spline(UNUSED struct Camera *c) {
    sCutsceneVars[9].point[0] = 0.f;
    cutscene_reset_spline();
}

/**
 * Follow sEndingFlyToWindowPos/Focus up to the window.
 */
static struct CutsceneSplinePoint sEndingFlyToWindowPos[] = {
    { 0, 0, { -86, 876, 640 } },   { 1, 0, { -86, 876, 610 } },   { 2, 0, { -66, 945, 393 } },
    { 3, 0, { -80, 976, 272 } },   { 4, 0, { -66, 1306, -36 } },  { 5, 0, { -70, 1869, -149 } },
    { 6, 0, { -10, 2093, -146 } }, { 7, 0, { -10, 2530, -248 } }, { 8, 0, { -10, 2530, -263 } },
    { 9, 0, { -10, 2530, -273 } }
};
static struct CutsceneSplinePoint sEndingFlyToWindowFocus[] = {
    { 0, 50, { -33, 889, -7 } },    { 1, 35, { -33, 889, -7 } },    { 2, 31, { -17, 1070, -193 } },
    { 3, 25, { -65, 1182, -272 } }, { 4, 20, { -64, 1559, -542 } }, { 5, 25, { -68, 2029, -677 } },
    { 6, 25, { -9, 2204, -673 } },  { 7, 25, { -8, 2529, -772 } },  { 8, 0, { -8, 2529, -772 } },
    { 9, 0, { -8, 2529, -772 } },   { -1, 0, { -8, 2529, -772 } }
};
void cutscene_ending_fly_up_to_window(struct Camera *c) {

    move_point_along_spline(c->pos, sEndingFlyToWindowPos, &sCutsceneSplineSegment, &sCutsceneSplineSegmentProgress);
    move_point_along_spline(c->focus, sEndingFlyToWindowFocus, &sCutsceneSplineSegment, &sCutsceneSplineSegmentProgress);
}

/**
 * Move the camera up to the window as the star power frees peach.
 */
void cutscene_ending_stars_free_peach(struct Camera *c) {
    cutscene_event(cutscene_ending_reset_spline, c, 0, 0);
    cutscene_event(cutscene_ending_fly_up_to_window, c, 0, -1);
    player2_rotate_cam(c, -0x2000, 0x2000, -0x2000, 0x2000);
}

/**
 * Move the camera to the ground as Mario lands.
 */
void cutscene_ending_mario_land(struct Camera *c) {
    vec3f_set(c->focus, sEndingFlyToWindowFocus[0].point[0], sEndingFlyToWindowFocus[0].point[1] + 80.f, sEndingFlyToWindowFocus[0].point[2]);
    vec3f_set(c->pos, sEndingFlyToWindowPos[0].point[0], sEndingFlyToWindowPos[0].point[1], sEndingFlyToWindowPos[0].point[2] + 150.f);
    player2_rotate_cam(c, -0x800, 0x2000, -0x2000, 0x2000);
}

/**
 * Move the camera closer to peach appearing.
 */
void cutscene_ending_peach_appear_closeup(struct Camera *c) {
    vec3f_set(c->pos, 179.f, 2463.f, -1216.f);
    c->pos[1] = gCutsceneFocus->oPosY + 35.f;
    vec3f_set(c->focus, gCutsceneFocus->oPosX, gCutsceneFocus->oPosY + 125.f, gCutsceneFocus->oPosZ);
}

/**
 * Peach fades in, the camera focuses on her.
 */
void cutscene_ending_peach_appears(struct Camera *c) {
    cutscene_event(cutscene_ending_peach_appear_closeup, c, 0, 0);
    approach_f32_asymptotic_bool(&c->pos[1], gCutsceneFocus->oPosY + 35.f, 0.02f);
    approach_f32_asymptotic_bool(&c->focus[1], gCutsceneFocus->oPosY + 125.f, 0.15f);
    player2_rotate_cam(c, -0x2000, 0x2000, -0x2000, 0x2000);
}

/**
 * Reset spline progress, set cvar2 y offset.
 */
void cutscene_ending_peach_descends_start(UNUSED struct Camera *c) {
    cutscene_reset_spline();
    sCutsceneVars[2].point[1] = 150.f;
}

static struct CutsceneSplinePoint sEndingPeachDescentCamPos[] = {
    { 0, 50, { 1, 120, -1150 } },    { 1, 50, { 1, 120, -1150 } },    { 2, 40, { 118, 121, -1199 } },
    { 3, 40, { 147, 74, -1306 } },   { 4, 40, { 162, 95, -1416 } },   { 5, 40, { 25, 111, -1555 } },
    { 6, 40, { -188, 154, -1439 } }, { 7, 40, { -203, 181, -1242 } }, { 8, 40, { 7, 191, -1057 } },
    { 9, 40, { 262, 273, -1326 } },  { 0, 40, { -4, 272, -1627 } },   { 1, 35, { -331, 206, -1287 } },
    { 2, 30, { -65, 219, -877 } },   { 3, 25, { 6, 216, -569 } },     { 4, 25, { -8, 157, 40 } },
    { 5, 25, { -4, 106, 200 } },     { 6, 25, { -6, 72, 574 } },      { 7, 0, { -6, 72, 574 } },
    { 8, 0, { -6, 72, 574 } },       { -1, 0, { -6, 72, 574 } }
};

/**
 * Follow the sEndingPeachDescentCamPos spline, which rotates around peach.
 */
void cutscene_ending_follow_peach_descent(struct Camera *c) {
    move_point_along_spline(c->pos, sEndingPeachDescentCamPos, &sCutsceneSplineSegment, &sCutsceneSplineSegmentProgress);
    c->pos[1] += gCutsceneFocus->oPosY + sCutsceneVars[3].point[1];
}

/**
 * Decrease cvar2's y offset while the camera flies backwards to Mario.
 */
void cutscene_ending_peach_descent_lower_focus(UNUSED struct Camera *c) {
    camera_approach_f32_symmetric_bool(&(sCutsceneVars[2].point[1]), 90.f, 0.5f);
}

/**
 * Keep following the sEndingPeachDescentCamPos spline, which leads back to Mario.
 */
void cutscene_ending_peach_descent_back_to_mario(struct Camera *c) {
    Vec3f pos;

    move_point_along_spline(pos, sEndingPeachDescentCamPos, &sCutsceneSplineSegment, &sCutsceneSplineSegmentProgress);
    c->pos[0] = pos[0];
    c->pos[2] = pos[2];
    approach_f32_asymptotic_bool(&c->pos[1], (pos[1] += gCutsceneFocus->oPosY), 0.07f);
}

/**
 * Peach starts floating to the ground. Rotate the camera around her, then fly backwards to Mario when
 * she lands.
 */
void cutscene_ending_peach_descends(struct Camera *c) {
    cutscene_event(cutscene_ending_peach_descends_start, c, 0, 0);
    cutscene_event(cutscene_ending_follow_peach_descent, c, 0, 299);
    cutscene_event(cutscene_ending_peach_descent_back_to_mario, c, 300, -1);
    cutscene_event(cutscene_ending_peach_descent_lower_focus, c, 300, -1);
    vec3f_set(c->focus, gCutsceneFocus->oPosX, sCutsceneVars[2].point[1] + gCutsceneFocus->oPosY,
              gCutsceneFocus->oPosZ);
    player2_rotate_cam(c, -0x2000, 0x2000, -0x2000, 0x2000);
}

/**
 * Mario runs across the bridge to peach, and takes off his cap.
 * Follow the sEndingMarioToPeach* splines while Mario runs across.
 */
void cutscene_ending_mario_to_peach(struct Camera *c) {    
    static struct CutsceneSplinePoint sEndingMarioToPeachPos[] = {
        { 0, 0, { -130, 1111, -1815 } }, { 1, 0, { -131, 1052, -1820 } }, { 2, 0, { -271, 1008, -1651 } },
        { 3, 0, { -439, 1043, -1398 } }, { 4, 0, { -433, 1040, -1120 } }, { 5, 0, { -417, 1040, -1076 } },
        { 6, 0, { -417, 1040, -1076 } }, { 7, 0, { -417, 1040, -1076 } }, { -1, 0, { -417, 1040, -1076 } }
    };
    static struct CutsceneSplinePoint sEndingMarioToPeachFocus[] = {
        { 0, 50, { -37, 1020, -1332 } }, { 1, 20, { -36, 1012, -1330 } }, { 2, 20, { -24, 1006, -1215 } },
        { 3, 20, { 28, 1002, -1224 } },  { 4, 24, { 45, 1013, -1262 } },  { 5, 35, { 34, 1000, -1287 } },
        { 6, 0, { 34, 1000, -1287 } },   { 7, 0, { 34, 1000, -1287 } },   { -1, 0, { 34, 1000, -1287 } }
    };

    cutscene_event(cutscene_ending_reset_spline, c, 0, 0);
    move_point_along_spline(c->pos, sEndingMarioToPeachPos, &sCutsceneSplineSegment, &sCutsceneSplineSegmentProgress);
    move_point_along_spline(c->focus, sEndingMarioToPeachFocus, &sCutsceneSplineSegment, &sCutsceneSplineSegmentProgress);
    player2_rotate_cam(c, -0x2000, 0x2000, -0x2000, 0x2000);
}

/**
 * Make the focus follow the sEndingLookUpAtCastle spline.
 */
void cutscene_ending_look_up_at_castle(UNUSED struct Camera *c) {
    static struct CutsceneSplinePoint sEndingLookUpAtCastle[] = {
        { 0, 50, { 200, 1066, -1414 } }, { 0, 50, { 200, 1066, -1414 } }, { 0, 30, { 198, 1078, -1412 } },
        { 0, 33, { 15, 1231, -1474 } },  { 0, 39, { -94, 1381, -1368 } }, { 0, 0, { -92, 1374, -1379 } },
        { 0, 0, { -92, 1374, -1379 } },  { -1, 0, { -92, 1374, -1379 } }
    };

    move_point_along_spline(c->focus, sEndingLookUpAtCastle, &sCutsceneSplineSegment, &sCutsceneSplineSegmentProgress);
}

/**
 * Peach opens her eyes and the camera looks at the castle window again.
 */
void cutscene_ending_peach_wakeup(struct Camera *c) {
    cutscene_event(cutscene_ending_reset_spline, c, 0, 0);
    cutscene_event(cutscene_ending_look_up_at_castle, c, 0, 0);
#ifdef VERSION_EU
    cutscene_event(cutscene_ending_look_up_at_castle, c, 265, -1);
    cutscene_spawn_obj(7, 315);
    cutscene_spawn_obj(9, 355);
#else
    cutscene_event(cutscene_ending_look_up_at_castle, c, 250, -1);
    cutscene_spawn_obj(7, 300);
    cutscene_spawn_obj(9, 340);
#endif
    vec3f_set(c->pos, -163.f, 978.f, -1082.f);
    player2_rotate_cam(c, -0x800, 0x2000, -0x2000, 0x2000);
}

/**
 * Side view of peach and Mario. Peach thanks Mario for saving her.
 */
void cutscene_ending_dialog(struct Camera *c) {
    vec3f_set(c->focus, 11.f, 983.f, -1273.f);
    vec3f_set(c->pos, -473.f, 970.f, -1152.f);
    player2_rotate_cam(c, -0x800, 0x2000, -0x2000, 0x2000);
}

/**
 * Zoom in and move the camera close to Mario and peach.
 */
void cutscene_ending_kiss_closeup(struct Camera *c) {
    set_fov_function(CAM_FOV_SET_29);
    vec3f_set(c->focus, 350.f, 1034.f, -1216.f);
    vec3f_set(c->pos, -149.f, 1021.f, -1216.f);
}

/**
 * Fly back and zoom out for Mario's spin after the kiss.
 */
void cutscene_ending_kiss_here_we_go(struct Camera *c) {
    Vec3f pos, foc;

    set_fov_function(CAM_FOV_DEFAULT);
    vec3f_set(foc, 233.f, 1068.f, -1298.f);
    vec3f_set(pos, -250.f, 966.f, -1111.f);
    approach_vec3f_asymptotic(c->pos, pos, 0.2f, 0.1f, 0.2f);
    approach_vec3f_asymptotic(c->focus, foc, 0.2f, 0.1f, 0.2f);
}

/**
 * Peach kisses Mario on the nose.
 */
void cutscene_ending_kiss(struct Camera *c) {
    cutscene_event(cutscene_ending_kiss_closeup, c, 0, 0);
    cutscene_event(cutscene_ending_kiss_here_we_go, c, 155, -1);
    player2_rotate_cam(c, -0x800, 0x2000, -0x2000, 0x2000);
}

/**
 * Make the focus follow sEndingLookAtSkyFocus.
 */
void cutscene_ending_look_at_sky(struct Camera *c) {
    static struct CutsceneSplinePoint sEndingLookAtSkyFocus[] = {
#ifdef VERSION_EU
        { 0, 50, { 484, 1368, -868 } }, { 0, 72, { 479, 1372, -872 } }, { 0, 50, { 351, 1817, -918 } },
#else
        { 0, 50, { 484, 1368, -888 } }, { 0, 72, { 479, 1372, -892 } }, { 0, 50, { 351, 1817, -918 } },
#endif
        { 0, 50, { 351, 1922, -598 } }, { 0, 0, { 636, 2027, -415 } },  { 0, 0, { 636, 2027, -415 } },
        { -1, 0, { 636, 2027, -415 } }
    };

    move_point_along_spline(c->focus, sEndingLookAtSkyFocus, &sCutsceneSplineSegment, &sCutsceneSplineSegmentProgress);
    vec3f_set(c->pos, 699.f, 1680.f, -703.f);
}

/**
 * Zoom in the fov. The fovFunc was just set to default, so it wants to approach 45. But while this is
 * called, it will stay at about 37.26f
 */
void cutscene_ending_zoom_fov(UNUSED struct Camera *c) {
    sFOVState.fov = 37.f;
}

/**
 * Peach suggests baking a cake for Mario. Mario looks back at the camera before going inside the castle.
 */
void cutscene_ending_cake_for_mario(struct Camera *c) {
    cutscene_event(cutscene_ending_reset_spline, c, 0, 0);
    cutscene_event(cutscene_ending_look_at_sky, c, 0, 0);
    cutscene_event(cutscene_ending_zoom_fov, c, 0, 499);
    cutscene_event(cutscene_ending_look_at_sky, c, 500, -1);
    cutscene_spawn_obj(8, 600);
    cutscene_spawn_obj(8, 608);
    cutscene_spawn_obj(8, 624);
    cutscene_spawn_obj(8, 710);
}

/**
 * Stop the ending cutscene, reset the fov.
 */
void cutscene_ending_stop(struct Camera *c) {
    set_fov_function(CAM_FOV_SET_45);
    c->cutscene = 0;
    gCutsceneTimer = CUTSCENE_STOP;
}

struct Cutscene sCutsceneEnding[] = {
    { cutscene_ending_mario_fall, 170 },
    { cutscene_ending_mario_land, 70 },
    { cutscene_ending_mario_land_closeup, 75 },
#ifdef VERSION_SH
    { cutscene_ending_stars_free_peach, 431 },
#else
    { cutscene_ending_stars_free_peach, 386 },
#endif
    { cutscene_ending_peach_appears, 139 },
    { cutscene_ending_peach_descends, 590 },
    { cutscene_ending_mario_to_peach, 95 },
#ifdef VERSION_SH
    { cutscene_ending_peach_wakeup, 455 },
    { cutscene_ending_dialog, 286 },
#else
    { cutscene_ending_peach_wakeup, 425 },
    { cutscene_ending_dialog, 236 },
#endif
    { cutscene_ending_kiss, 245 },
    { cutscene_ending_cake_for_mario, CUTSCENE_LOOP },
    { cutscene_ending_stop, 0 }
};
