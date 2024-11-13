/**
 * The game's credits.
 */
#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"
#include "game/save_file.h"

/**
 * The current spline that controls the camera's position during the credits.
 */
static struct CutsceneSplinePoint sCurCreditsSplinePos[32];

/**
 * The current spline that controls the camera's focus during the credits.
 */
static struct CutsceneSplinePoint sCurCreditsSplineFocus[32];


void init_current_credits_spline() {    
    for (int i = 0; i < 32; i++) {
        sCurCreditsSplinePos[i].index = -1;
        sCurCreditsSplineFocus[i].index = -1;
    }
}

/*
 * credits spline paths.
 * TODO: Separate these into their own file(s)
 */
struct CutsceneSplinePoint sBobCreditsSplinePositions[] = {
    { 1, 0, { 5984, 3255, 4975 } },
    { 2, 0, { 4423, 3315, 1888 } },
    { 3, 0, { 776, 2740, -1825 } },
    { 4, 0, { -146, 3894, -3167 } },
    { -1, 0, { 741, 4387, -5474 } }
};

struct CutsceneSplinePoint sBobCreditsSplineFocus[] = {
    { 0, 30, { 5817, 3306, 4507 } },
    { 0, 40, { 4025, 3378, 1593 } },
    { 0, 50, { 1088, 2652, -2205 } },
    { 0, 60, { 205, 3959, -3517 } },
    { -1, 60, { 1231, 4400, -5649 } }
};

struct CutsceneSplinePoint sWfCreditsSplinePositions[] = {
    { 0, 0, { -301, 1399, 2643 } },
    { 0, 0, { -182, 2374, 4572 } },
    { 0, 0, { 4696, 3864, 413 } },
    { 0, 0, { 1738, 4891, -1516 } },
    { -1, 0, { 1783, 4891, -1516 } }
};

struct CutsceneSplinePoint sWfCreditsSplineFocus[] = {
    { 1, 30, { -249, 1484, 2153 } },
    { 2, 40, { -200, 2470, 4082 } },
    { 3, 40, { 4200, 3916, 370 } },
    { 4, 40, { 1523, 4976, -1072 } },
    { -1, 40, { 1523, 4976, -1072 } }
};

struct CutsceneSplinePoint sJrbCreditsSplinePositions[] = {
    { 0, 0, { 5538, -4272, 2376 } },
    { 0, 0, { 5997, -3303, 2261 } },
    { 0, 0, { 6345, -3255, 2179 } },
    { 0, 0, { 6345, -3255, 2179 } },
    { -1, 0, { 6694, -3203, 2116 } }
};

struct CutsceneSplinePoint sJrbCreditsSplineFocus[] = {
    { 0, 50, { 5261, -4683, 2443 } },
    { 0, 50, { 5726, -3675, 2456 } },
    { 0, 50, { 6268, -2817, 2409 } },
    { 0, 50, { 6596, -2866, 2369 } },
    { -1, 50, { 7186, -3153, 2041 } }
};

struct CutsceneSplinePoint sCcmSlideCreditsSplinePositions[] = {
    { 0, 0, { -6324, 6745, -5626 } },
    { 1, 0, { -6324, 6745, -5626 } },
    { 2, 0, { -6108, 6762, -5770 } },
    { 3, 0, { -5771, 6787, -5962 } },
    { -1, 0, { -5672, 6790, -5979 } }
};

struct CutsceneSplinePoint sCcmSlideCreditsSplineFocus[] = {
    { 0, 50, { -5911, 6758, -5908 } },
    { 1, 50, { -5911, 6758, -5908 } },
    { 2, 50, { -5652, 6814, -5968 } },
    { 3, 50, { -5277, 6801, -6043 } },
    { -1, 50, { -5179, 6804, -6060 } }
};

struct CutsceneSplinePoint sBbhCreditsSplinePositions[] = {
    { 1, 0, { 1088, 341, 2447 } },
    { 2, 0, { 1338, 610, 2808 } },
    { 3, 0, { 2267, 1612, 2966 } },
    { -1, 0, { 2296, 1913, 2990 } }
};

struct CutsceneSplinePoint sBbhCreditsSplineFocus[] = {
    { 1, 50, { 1160, 263, 1958 } },
    { 2, 50, { 1034, 472, 2436 } },
    { 3, 50, { 1915, 1833, 2688 } },
    { -1, 50, { 2134, 2316, 2742 } }
};

struct CutsceneSplinePoint sHmcCreditsSplinePositions[] = {
    { 1, 0, { -5952, 1807, -5882 } },
    { 2, 0, { -5623, 1749, -4863 } },
    { 3, 0, { -5472, 1955, -2520 } },
    { 4, 0, { -5544, 1187, -1085 } },
    { -1, 0, { -5547, 391, -721 } }
};

struct CutsceneSplinePoint sHmcCreditsSplineFocus[] = {
    { 1, 210, { -5952, 1884, -6376 } },
    { 2, 58, { -5891, 1711, -5283 } },
    { 3, 30, { -5595, 1699, -2108 } },
    { 4, 31, { -5546, 794, -777 } },
    { -1, 31, { -5548, -85, -572 } }
};

struct CutsceneSplinePoint sThiWigglerCreditsSplinePositions[] = {
    { 1, 0, { -1411, 2474, -1276 } },
    { 2, 0, { -1606, 2479, -434 } },
    { -1, 0, { -1170, 2122, 1337 } }
};

struct CutsceneSplinePoint sThiWigglerCreditsSplineFocus[] = {
    { 1, 50, { -1053, 2512, -928 } },
    { 2, 50, { -1234, 2377, -114 } },
    { -1, 50, { -758, 2147, 1054 } }
};

struct CutsceneSplinePoint sVolcanoCreditsSplinePositions[] = {
    { 0, 0, { -1445, 1094, 1617 } },
    { 0, 0, { -1509, 649, 871 } },
    { 0, 0, { -1133, 420, -248 } },
    { 0, 0, { -778, 359, -1052 } },
    { 0, 0, { -565, 260, -1730 } },
    { -1, 0, { 1274, 473, -275 } }
};

struct CutsceneSplinePoint sVolcanoCreditsSplineFocus[] = {
    { 0, 50, { -1500, 757, 1251 } },
    { 0, 50, { -1401, 439, 431 } },
    { 0, 50, { -749, 270, -532 } },
    { 0, 50, { -396, 270, -1363 } },
    { 0, 50, { -321, 143, -2151 } },
    { -1, 50, { 1002, 460, -694 } }
};

struct CutsceneSplinePoint sSslCreditsSplinePositions[] = {
    { 0, 0, { -4262, 4658, -5015 } },
    { 0, 0, { -3274, 2963, -4661 } },
    { 0, 0, { -2568, 812, -6528 } },
    { 0, 0, { -414, 660, -7232 } },
    { 0, 0, { 1466, 660, -6898 } },
    { -1, 0, { 2724, 660, -6298 } }
};

struct CutsceneSplinePoint sSslCreditsSplineFocus[] = {
    { 0, 50, { -4083, 4277, -4745 } },
    { 0, 50, { -2975, 2574, -4759 } },
    { 0, 50, { -2343, 736, -6088 } },
    { 0, 50, { -535, 572, -6755 } },
    { 0, 50, { 1311, 597, -6427 } },
    { -1, 50, { 2448, 612, -5884 } }
};

struct CutsceneSplinePoint sDddCreditsSplinePositions[] = {
    { 0, 0, { -874, -4933, 366 } },
    { 0, 0, { -1463, -4782, 963 } },
    { 0, 0, { -1893, -4684, 1303 } },
    { 0, 0, { -2818, -4503, 1583 } },
    { 0, 0, { -4095, -2924, 730 } },
    { 0, 0, { -4737, -1594, -63 } },
    { -1, 0, { -4681, -1084, -623 } }
};

struct CutsceneSplinePoint sDddCreditsSplineFocus[] = {
    { 0, 50, { -1276, -4683, 622 } },
    { 0, 50, { -1858, -4407, 1097 } },
    { 0, 50, { -2324, -4332, 1318 } },
    { 0, 50, { -3138, -4048, 1434 } },
    { 0, 50, { -4353, -2444, 533 } },
    { 0, 50, { -4807, -1169, -436 } },
    { -1, 50, { -4665, -664, -1007 } }
};

struct CutsceneSplinePoint sSlCreditsSplinePositions[] = {
    { 0, 0, { 939, 6654, 6196 } },
    { 0, 0, { 1873, 5160, 3714 } },
    { 0, 0, { 3120, 3564, 1314 } },
    { -1, 0, { 2881, 4231, 573 } }
};

struct CutsceneSplinePoint sSlCreditsSplineFocus[] = {
    { 0, 50, { 875, 6411, 5763 } },
    { 0, 50, { 1659, 4951, 3313 } },
    { 0, 50, { 2630, 3565, 1215 } },
    { -1, 50, { 2417, 4056, 639 } }
};

struct CutsceneSplinePoint sWdwCreditsSplinePositions[] = {
    { 0, 0, { 3927, 2573, 3685 } },
    { 0, 0, { 2389, 2054, 1210 } },
    { 0, 0, { 2309, 2069, 22 } },
    { -1, 0, { 2122, 2271, -979 } }
};

struct CutsceneSplinePoint sWdwCreditsSplineFocus[] = {
    { 0, 50, { 3637, 2460, 3294 } },
    { 0, 50, { 1984, 2067, 918 } },
    { 0, 50, { 1941, 2255, -261 } },
    { -1, 50, { 1779, 2587, -1158 } }
};

struct CutsceneSplinePoint sTtmCreditsSplinePositions[] = {
    { 0, 0, { 386, 2535, 644 } },
    { 0, 0, { 1105, 2576, 918 } },
    { 0, 0, { 3565, 2261, 2098 } },
    { 0, 0, { 6715, -2791, 4554 } },
    { 0, 0, { 3917, -3130, 3656 } },
    { -1, 0, { 3917, -3130, 3656 } }
};

struct CutsceneSplinePoint sTtmCreditsSplineFocus[] = {
    { 1, 50, { 751, 2434, 318 } },
    { 2, 50, { 768, 2382, 603 } },
    { 3, 60, { 3115, 2086, 1969 } },
    { 4, 30, { 6370, -3108, 4727 } },
    { 5, 50, { 4172, -3385, 4001 } },
    { -1, 50, { 4172, -3385, 4001 } }
};

struct CutsceneSplinePoint sThiHugeCreditsSplinePositions[] = {
    { 0, 0, { 6990, -1000, -4858 } },
    { 0, 0, { 7886, -1055, 2878 } },
    { 0, 0, { 1952, -1481, 10920 } },
    { 0, 0, { -1684, -219, 2819 } },
    { 0, 0, { -2427, -131, 2755 } },
    { 0, 0, { -3246, 416, 3286 } },
    { -1, 0, { -3246, 416, 3286 } }
};

struct CutsceneSplinePoint sThiHugeCreditsSplineFocus[] = {
    { 1, 70, { 7022, -965, -5356 } },
    { 2, 40, { 7799, -915, 2405 } },
    { 3, 60, { 1878, -1137, 10568 } },
    { 4, 50, { -1931, -308, 2394 } },
    { 5, 50, { -2066, -386, 2521 } },
    { 6, 50, { -2875, 182, 3045 } },
    { -1, 50, { -2875, 182, 3045 } }
};

struct CutsceneSplinePoint sTtcCreditsSplinePositions[] = {
    { 1, 0, { -1724, 277, -994 } },
    { 2, 0, { -1720, 456, -995 } },
    { 3, 0, { -1655, 810, -1014 } },
    { -1, 0, { -1753, 883, -1009 } }
};

struct CutsceneSplinePoint sTtcCreditsSplineFocus[] = {
    { 1, 50, { -1554, 742, -1063 } },
    { 2, 50, { -1245, 571, -1102 } },
    { 3, 50, { -1220, 603, -1151 } },
    { -1, 50, { -1412, 520, -1053 } }
};

struct CutsceneSplinePoint sRrCreditsSplinePositions[] = {
    { 0, 0, { -1818, 4036, 97 } },
    { 0, 0, { -575, 3460, -505 } },
    { 0, 0, { 1191, 3611, -1134 } },
    { -1, 0, { 2701, 3777, -3686 } }
};

struct CutsceneSplinePoint sRrCreditsSplineFocus[] = {
    { 0, 50, { -1376, 3885, -81 } },
    { 0, 50, { -146, 3343, -734 } },
    { 0, 50, { 1570, 3446, -1415 } },
    { -1, 50, { 2794, 3627, -3218 } }
};

struct CutsceneSplinePoint sSaCreditsSplinePositions[] = {
    { 0, 0, { -295, -396, -585 } },
    { 1, 0, { -295, -396, -585 } },
    { 2, 0, { -292, -856, -573 } },
    { 3, 0, { -312, -856, -541 } },
    { -1, 0, { 175, -856, -654 } }
};

struct CutsceneSplinePoint sSaCreditsSplineFocus[] = {
    { 0, 50, { -175, -594, -142 } },
    { 1, 50, { -175, -594, -142 } },
    { 2, 50, { -195, -956, -92 } },
    { 3, 50, { -572, -956, -150 } },
    { -1, 50, { -307, -956, -537 } }
};

struct CutsceneSplinePoint sCotmcCreditsSplinePositions[] = {
    { 0, 0, { -296, 495, 1607 } },
    { 0, 0, { -430, 541, 654 } },
    { 0, 0, { -466, 601, -359 } },
    { 0, 0, { -217, 433, -1549 } },
    { -1, 0, { -95, 366, -2922 } }
};

struct CutsceneSplinePoint sCotmcCreditsSplineFocus[] = {
    { 0, 50, { -176, 483, 2092 } },
    { 0, 50, { -122, 392, 1019 } },
    { 0, 50, { -268, 450, -792 } },
    { 0, 50, { -172, 399, -2046 } },
    { -1, 50, { -51, 355, -3420 } }
};

struct CutsceneSplinePoint sDddSubCreditsSplinePositions[] = {
    { 0, 0, { 4656, 2171, 5028 } },
    { 0, 0, { 4548, 1182, 4596 } },
    { 0, 0, { 5007, 813, 3257 } },
    { 0, 0, { 5681, 648, 1060 } },
    { -1, 0, { 4644, 774, 113 } }
};

struct CutsceneSplinePoint sDddSubCreditsSplineFocus[] = {
    { 0, 50, { 4512, 2183, 4549 } },
    { 0, 50, { 4327, 838, 4308 } },
    { 0, 50, { 4774, 749, 2819 } },
    { 0, 50, { 5279, 660, 763 } },
    { -1, 50, { 4194, 885, -75 } }
};

struct CutsceneSplinePoint sCcmOutsideCreditsSplinePositions[] = {
    { 1, 0, { 1427, -1387, 5409 } },
    { 2, 0, { -1646, -1536, 4526 } },
    { 3, 0, { -3852, -1448, 3913 } },
    { -1, 0, { -5199, -1366, 1886 } }
};

struct CutsceneSplinePoint sCcmOutsideCreditsSplineFocus[] = {
    { 1, 50, { 958, -1481, 5262 } },
    { 2, 50, { -2123, -1600, 4391 } },
    { 3, 50, { -3957, -1401, 3426 } },
    { -1, 50, { -4730, -1215, 1795 } }
};

void init_spline_point(struct CutsceneSplinePoint *splinePoint, s8 index, u8 speed, Vec3s point) {
    splinePoint->index = index;
    splinePoint->speed = speed;
    vec3s_copy(splinePoint->point, point);
}

// TODO: (Scrub C)
void copy_spline_segment(struct CutsceneSplinePoint dst[], struct CutsceneSplinePoint src[]) {
    s32 j = 0;
    s32 i = 0;

    init_spline_point(&dst[i], src[j].index, src[j].speed, src[j].point);
    i++;
    do {
        do {
            init_spline_point(&dst[i], src[j].index, src[j].speed, src[j].point);
            i++;
            j++;
        } while (src[j].index != -1);
    } while (j > 16);

    // Create the end of the spline by duplicating the last point
    init_spline_point(&dst[i + 0],  0, src[j].speed, src[j].point);
    init_spline_point(&dst[i + 1],  0,            0, src[j].point);
    init_spline_point(&dst[i + 2],  0,            0, src[j].point);
    init_spline_point(&dst[i + 3], -1,            0, src[j].point);
}

/**
 * Called on the first frame of the credits. Resets the spline progress.
 */
void cutscene_credits_reset_spline(UNUSED struct Camera *c) {
    cutscene_reset_spline();
}

/**
 * Follow splines through the courses of the game.
 */
void cutscene_credits(struct Camera *c) {
    struct CutsceneSplinePoint *focus, *pos;

    cutscene_event(cutscene_credits_reset_spline, c, 0, 0);

    switch (gCurrLevelArea) {
        case AREA_BOB:
            pos = sBobCreditsSplinePositions;
            focus = sBobCreditsSplineFocus;
            break;
        case AREA_WF:
            pos = sWfCreditsSplinePositions;
            focus = sWfCreditsSplineFocus;
            break;
        case AREA_JRB_MAIN:
            pos = sJrbCreditsSplinePositions;
            focus = sJrbCreditsSplineFocus;
            break;
        case AREA_CCM_SLIDE:
            pos = sCcmSlideCreditsSplinePositions;
            focus = sCcmSlideCreditsSplineFocus;
            break;
        case AREA_BBH:
            pos = sBbhCreditsSplinePositions;
            focus = sBbhCreditsSplineFocus;
            break;
        case AREA_HMC:
            pos = sHmcCreditsSplinePositions;
            focus = sHmcCreditsSplineFocus;
            break;
        case AREA_THI_WIGGLER:
            pos = sThiWigglerCreditsSplinePositions;
            focus = sThiWigglerCreditsSplineFocus;
            break;
        case AREA_LLL_VOLCANO:
            pos = sVolcanoCreditsSplinePositions;
            focus = sVolcanoCreditsSplineFocus;
            break;
        case AREA_SSL_OUTSIDE:
            pos = sSslCreditsSplinePositions;
            focus = sSslCreditsSplineFocus;
            break;
        case AREA_DDD_WHIRLPOOL:
            pos = sDddCreditsSplinePositions;
            focus = sDddCreditsSplineFocus;
            break;
        case AREA_SL_OUTSIDE:
            pos = sSlCreditsSplinePositions;
            focus = sSlCreditsSplineFocus;
            break;
        case AREA_WDW_MAIN:
            pos = sWdwCreditsSplinePositions;
            focus = sWdwCreditsSplineFocus;
            break;
        case AREA_TTM_OUTSIDE:
            pos = sTtmCreditsSplinePositions;
            focus = sTtmCreditsSplineFocus;
            break;
        case AREA_THI_HUGE:
            pos = sThiHugeCreditsSplinePositions;
            focus = sThiHugeCreditsSplineFocus;
            break;
        case AREA_TTC:
            pos = sTtcCreditsSplinePositions;
            focus = sTtcCreditsSplineFocus;
            break;
        case AREA_RR:
            pos = sRrCreditsSplinePositions;
            focus = sRrCreditsSplineFocus;
            break;
        case AREA_SA:
            pos = sSaCreditsSplinePositions;
            focus = sSaCreditsSplineFocus;
            break;
        case AREA_COTMC:
            pos = sCotmcCreditsSplinePositions;
            focus = sCotmcCreditsSplineFocus;
            break;
        case AREA_DDD_SUB:
            pos = sDddSubCreditsSplinePositions;
            focus = sDddSubCreditsSplineFocus;
            break;
        case AREA_CCM_OUTSIDE:
            //! Checks if the "Snowman's Lost His Head" star was collected. The credits likely would
            //! have avoided the snowman if the player didn't collect that star, but in the end the
            //! developers decided against it.
            if (save_file_get_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(gCurrCourseNum)) & (1 << 4)) {
                pos = sCcmOutsideCreditsSplinePositions;
                focus = sCcmOutsideCreditsSplineFocus;
            } else {
                pos = sCcmOutsideCreditsSplinePositions;
                focus = sCcmOutsideCreditsSplineFocus;
            }
            break;
        default:
            pos = sCcmOutsideCreditsSplinePositions;
            focus = sCcmOutsideCreditsSplineFocus;
    }

    copy_spline_segment(sCurCreditsSplinePos, pos);
    copy_spline_segment(sCurCreditsSplineFocus, focus);
    move_point_along_spline(c->pos, sCurCreditsSplinePos, &sCutsceneSplineSegment, &sCutsceneSplineSegmentProgress);
    move_point_along_spline(c->focus, sCurCreditsSplineFocus, &sCutsceneSplineSegment, &sCutsceneSplineSegmentProgress);
    player2_rotate_cam(c, -0x2000, 0x2000, -0x4000, 0x4000);
}

struct Cutscene sCutsceneCredits[] = {
    { cutscene_credits, CUTSCENE_LOOP }
};
