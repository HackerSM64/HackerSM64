
/**
 * @file intro_lakitu.inc.c
 * This file implements lakitu's behvaior during the intro cutscene.
 * It's also used during the ending cutscene.
 */

/**
 * Add the camera's position to `offset`, rotate the point to be relative to the camera's focus, then
 * set lakitu's location.
 */
void intro_lakitu_set_offset_from_camera(struct Object *obj, Vec3f offset) {
    f32 dist;
    Vec3s focusAngles;
    s16 offsetPitch, offsetYaw;

    vec3f_add(offset, gCamera->pos);
    vec3f_get_dist_and_angle(gCamera->pos, gCamera->focus,
                             &dist, &focusAngles[0], &focusAngles[1]);
    vec3f_get_dist_and_angle(gCamera->pos, offset, &dist, &offsetPitch, &offsetYaw);
    vec3f_set_dist_and_angle(gCamera->pos, offset, dist,
                             focusAngles[0] + offsetPitch, focusAngles[1] + offsetYaw);
    vec3f_copy(&obj->oPosVec, offset);
}

void intro_lakitu_set_focus(struct Object *obj, Vec3f newFocus) {
    f32 dist;
    s16 pitch, yaw;

    // newFocus is an offset from lakitu's origin, not a point in the world.
    vec3f_get_dist_and_angle(gVec3fZero, newFocus, &dist, &pitch, &yaw);

    obj->oFaceAnglePitch = pitch;
    obj->oFaceAngleYaw = yaw;
}

/**
 * Move lakitu along the spline `offset`, relative to the camera, and face him towards the corresponding
 * location along the spline `focus`.
 */
s32 intro_lakitu_set_pos_and_focus(struct Object *obj, struct CutsceneSplinePoint offset[],
                                   struct CutsceneSplinePoint focus[]) {
    Vec3f newOffset, newFocus;
    s32 splineFinished = 0;
    s16 splineSegment = obj->oIntroLakituSplineSegment;

    if ((move_point_along_spline(newFocus, offset, &splineSegment,
                                 &obj->oIntroLakituSplineSegmentProgress) == 1)
        || (move_point_along_spline(newOffset, focus, &splineSegment,
                                    &obj->oIntroLakituSplineSegmentProgress) == 1)) {
        splineFinished++;
    }

    obj->oIntroLakituSplineSegment = splineSegment;

    intro_lakitu_set_offset_from_camera(obj, newOffset);
    intro_lakitu_set_focus(obj, newFocus);

    return splineFinished;
}

#ifdef VERSION_EU
#define TIMER1 599
#define TIMER2 74
#else
#define TIMER1 720
#define TIMER2 98
#endif

struct CutsceneSplinePoint gIntroLakituStartToPipeFocus[] = {
    { 0, 32, { 58, -250, 346 } },    { 1, 50, { -159, -382, 224 } }, { 2, 37, { 0, -277, 237 } },
    { 3, 15, { 1, -44, 245 } },      { 4, 35, { 0, -89, 228 } },     { 5, 15, { 28, 3, 259 } },
    { 6, 25, { -38, -201, 371 } },   { 7, 20, { -642, 118, 652 } },  { 8, 25, { 103, -90, 861 } },
    { 9, 25, { 294, 145, 579 } },    { 10, 30, { 220, -42, 500 } },  { 11, 20, { 10, -134, 200 } },
    { 12, 20, { -143, -145, 351 } }, { 13, 14, { -256, -65, 528 } }, { 14, 20, { -251, -52, 459 } },
    { 15, 25, { -382, 520, 395 } },  { 16, 25, { -341, 240, 653 } }, { 17, 5, { -262, 700, 143 } },
    { 18, 15, { -760, 32, 27 } },    { 19, 20, { -756, -6, -26 } },  { 20, 20, { -613, 5, 424 } },
    { 21, 20, { -22, -100, 312 } },  { 22, 25, { 212, 80, 61 } },    { 23, 20, { 230, -28, 230 } },
    { 24, 35, { -83, -51, 303 } },   { 25, 17, { 126, 90, 640 } },   { 26, 9, { 158, 95, 763 } },
    { 27, 8, { 113, -25, 1033 } },   { 28, 20, { 57, -53, 1291 } },  { 29, 15, { 73, -34, 1350 } },
    { 30, 7, { 0, 96, 1400 } },      { 31, 8, { -59, 269, 1450 } },  { 32, 15, { 57, 1705, 1500 } },
    { 0, 15, { -227, 511, 1550 } },  { -1, 15, { -227, 511, 1600 } }
};

struct CutsceneSplinePoint gIntroLakituStartToPipeOffsetFromCamera[] = {
    { 0, 0, { -46, 87, -15 } },   { 1, 0, { -38, 91, -11 } },  { 2, 0, { -31, 93, -13 } },
    { 3, 0, { -50, 84, -16 } },   { 4, 0, { -52, 83, -17 } },  { 5, 0, { -10, 99, 3 } },
    { 6, 0, { -54, 83, -10 } },   { 7, 0, { -31, 85, -40 } },  { 8, 0, { -34, 91, 19 } },
    { 9, 0, { -9, 95, 28 } },     { 10, 0, { 17, 72, 66 } },   { 11, 0, { 88, -7, 45 } },
    { 12, 0, { 96, -6, -26 } },   { 13, 0, { 56, -1, -82 } },  { 14, 0, { 40, 65, -63 } },
    { 15, 0, { -26, -3, -96 } },  { 16, 0, { 92, 82, 19 } },   { 17, 0, { 92, 32, 19 } },
    { 18, 0, { 92, 32, 19 } },    { 19, 0, { 92, 102, 19 } },  { 20, 0, { -69, 59, -70 } },
    { 21, 0, { -77, 109, -61 } }, { 22, 0, { -87, 59, -46 } }, { 23, 0, { -99, -3, 11 } },
    { 24, 0, { -99, -11, 5 } },   { 25, 0, { -97, -6, 19 } },  { 26, 0, { -97, 22, -7 } },
    { 27, 0, { -98, -11, -13 } }, { 28, 0, { -97, -11, 19 } }, { 29, 0, { -91, -11, 38 } },
    { 30, 0, { -76, -11, 63 } },  { 31, 0, { -13, 33, 93 } },  { 32, 0, { 51, -11, 84 } },
    { 33, 0, { 51, -11, 84 } },   { -1, 0, { 51, -11, 84 } }
};

void bhv_intro_lakitu_loop(void) {
    Vec3f offset, fromPoint, toPoint;
	s16 yawToCam;

    switch (o->oAction) {
        case INTRO_LAKITU_ACT_INIT:
            cur_obj_disable_rendering();

            o->oIntroLakituSplineSegment = 0.0f;
            o->oIntroLakituSplineSegmentProgress = 0.0f;
            o->oIntroLakituCloud =
                spawn_object_relative_with_scale(CLOUD_BP_LAKITU_CLOUD, 0, 0, 0, 2.0f, o, MODEL_MIST, bhvCloud);

            if (gCamera->cutscene == CUTSCENE_END_WAVING) {
                o->oAction = INTRO_LAKITU_ACT_CUTSCENE_END_WAVING_1;
            } else {
                o->oAction++;
            }
            break;

        case INTRO_LAKITU_ACT_CUTSCENE_INTRO_1:
            cur_obj_enable_rendering();

            if ((gCutsceneTimer > 350) && (gCutsceneTimer < 458)) {
                vec3f_copy_y_off(&o->oPosVec, gCamera->pos, 500.0f);
            }

            if (gCutsceneTimer > 52) {
                cur_obj_play_sound_1(SOUND_AIR_LAKITU_FLY_HIGHPRIO);
            }

            if (intro_lakitu_set_pos_and_focus(o, gIntroLakituStartToPipeOffsetFromCamera, gIntroLakituStartToPipeFocus)) {
                o->oAction++;
            }

            switch (o->oTimer) {
#if defined(VERSION_US) || defined(VERSION_SH)
                case 534:
                    cur_obj_play_sound_2(SOUND_ACTION_FLYING_FAST);
                    break;
                case 581:
                    cur_obj_play_sound_2(SOUND_ACTION_INTRO_UNK45E);
                    break;
#endif
                case 73:
                    o->oAnimState++;
                    break;
                case 74:
                    o->oAnimState--;
                    break;
                case 82:
                    o->oAnimState++;
                    break;
                case 84:
                    o->oAnimState--;
                    break;
            }
#ifdef VERSION_EU
            if (o->oTimer == 446) {
                cur_obj_play_sound_2(SOUND_ACTION_FLYING_FAST);
            }
            if (o->oTimer == 485) {
                cur_obj_play_sound_2(SOUND_ACTION_INTRO_UNK45E);
            }
#endif
            break;

        case INTRO_LAKITU_ACT_CUTSCENE_INTRO_2:
            if (gCutsceneTimer > TIMER1) {
                o->oAction++;

                o->oIntroLakituDistToBirdsX   = 1400.0f;
                o->oIntroLakituDistToBirdsZ   = -4096.0f;
                o->oIntroLakituEndBirds1DestZ = 2048.0f;
                o->oIntroLakituEndBirds1DestY = -200.0f;

                o->oMoveAngleYaw = 0x8000;
                o->oFaceAngleYaw = o->oMoveAngleYaw + 0x4000;
                o->oMoveAnglePitch = 0x800;
            }

            cur_obj_play_sound_1(SOUND_AIR_LAKITU_FLY_HIGHPRIO);
            break;

        case INTRO_LAKITU_ACT_CUTSCENE_INTRO_3:
            cur_obj_play_sound_1(SOUND_AIR_LAKITU_FLY_HIGHPRIO);
            vec3f_set(fromPoint, -1128.0f, 560.0f, 4664.0f);
            o->oMoveAngleYaw += 0x200;
            o->oIntroLakituDistToBirdsX = approach_f32_asymptotic(o->oIntroLakituDistToBirdsX, 100.0f, 0.03f);
            o->oFaceAnglePitch = atan2s(200.0f, o->oPosY - 400.0f);
            o->oFaceAngleYaw = approach_s16_asymptotic(o->oFaceAngleYaw, o->oMoveAngleYaw + 0x8000, 4);
            vec3f_set_dist_and_angle(fromPoint, toPoint, o->oIntroLakituDistToBirdsX, 0,  o->oMoveAngleYaw);
            toPoint[1] += 150.0f * coss((s16) o->oIntroLakituDistToBirdsZ);
            o->oIntroLakituDistToBirdsZ += o->oIntroLakituEndBirds1DestZ;
            o->oIntroLakituEndBirds1DestZ = approach_f32_asymptotic(o->oIntroLakituEndBirds1DestZ, 512.0f, 0.05f);
            toPoint[0] += o->oIntroLakituEndBirds1DestY;
            o->oIntroLakituEndBirds1DestY = approach_f32_asymptotic(o->oIntroLakituEndBirds1DestY, 0.0f, 0.05f);
            vec3f_copy(&o->oPosVec, toPoint);

            if (o->oTimer == 31) {
                o->oPosY -= 158.0f;
                // Spawn white ground particles
                spawn_mist_from_global();
                o->oPosY += 158.0f;
            }

            if (o->oTimer == TIMER2) {
                obj_mark_for_deletion(o);
                obj_mark_for_deletion(o->oIntroLakituCloud);
            }

            if (o->oTimer == 14) {
                cur_obj_play_sound_2(SOUND_ACTION_INTRO_UNK45F);
            }
            break;
        case INTRO_LAKITU_ACT_CUTSCENE_END_WAVING_1:
            cur_obj_enable_rendering();
            vec3f_set(offset, -100.0f, 100.0f, 300.0f);
            offset_rotated(toPoint, gCamera->pos, offset, sMarioCamState->faceAngle);
            vec3f_copy(&o->oPosVec, toPoint);

            o->oMoveAnglePitch = 0x1000;
            o->oMoveAngleYaw = 0x9000;
            o->oFaceAnglePitch = o->oMoveAnglePitch / 2;
            o->oFaceAngleYaw = o->oMoveAngleYaw;

            o->oAction = INTRO_LAKITU_ACT_CUTSCENE_END_WAVING_2;
            break;

        case INTRO_LAKITU_ACT_CUTSCENE_END_WAVING_2:
            vec3f_copy(toPoint, &o->oPosVec);

            if (o->oTimer > 60) {
                o->oForwardVel = approach_f32_asymptotic(o->oForwardVel, -10.0f, 0.05f);
                o->oMoveAngleYaw += 0x78;
                o->oMoveAnglePitch += 0x40;
                o->oFaceAngleYaw = camera_approach_s16_symmetric(
                                       o->oFaceAngleYaw, (s16) calculate_yaw(toPoint, gCamera->pos), 0x200);
            }

            if (o->oTimer > 105) {
                o->oAction++;
                o->oMoveAnglePitch = 0xE00;
            }

            o->oFaceAnglePitch = 0;

            cur_obj_set_pos_via_transform();
            break;

        case INTRO_LAKITU_ACT_CUTSCENE_END_WAVING_3:
            vec3f_copy(toPoint, &o->oPosVec);

            o->oForwardVel = approach_f32_asymptotic(o->oForwardVel, 60.0f, 0.05f);
            vec3f_get_yaw(toPoint, gCamera->pos, &yawToCam);
            o->oFaceAngleYaw = camera_approach_s16_symmetric(
                                   o->oFaceAngleYaw, yawToCam, 0x200);

            if (o->oTimer < 62) {
                o->oMoveAngleYaw = approach_s16_asymptotic(o->oMoveAngleYaw, 0x1800, 0x1E);
            }

            o->oMoveAnglePitch = camera_approach_s16_symmetric(o->oMoveAnglePitch, -0x2000, 0x5A);
            o->oFaceAnglePitch = 0;

            cur_obj_set_pos_via_transform();
            break;
    }
}

#undef TIMER1
#undef TIMER2
