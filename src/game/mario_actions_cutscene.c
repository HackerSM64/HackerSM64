#include <PR/ultratypes.h>

#include "sm64.h"
#include "mario_actions_cutscene.h"
#include "mario_actions_moving.h"
#include "area.h"
#include "audio/external.h"
#include "behavior_data.h"
#include "camera.h"
#include "dialog_ids.h"
#include "camera/camera_math.h"
#include "camera/cutscene_helpers.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game_init.h"
#include "gfx_dimensions.h"
#include "ingame_menu.h"
#include "interaction.h"
#include "level_table.h"
#include "level_update.h"
#include "mario.h"
#include "mario_step.h"
#include "moving_texture.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "save_file.h"
#include "seq_ids.h"
#include "sound_init.h"
#include "rumble_init.h"

static struct Object *sIntroWarpPipeObj;
static struct Object *sEndPeachObj;
static struct Object *sEndRightToadObj;
static struct Object *sEndLeftToadObj;
static struct Object *sEndJumboStarObj;
static s16 sEndPeachAnimation;
static s16 sEndToadAnims[2];
ModelID32 gStarModelLastCollected = MODEL_STAR;

Vp sEndCutsceneVp = {
    {
        { (SCREEN_WIDTH  * 2), (SCREEN_HEIGHT * 2), 511, 0 },
        { (SCREEN_WIDTH  * 2), (SCREEN_HEIGHT * 2), 511, 0 }
    }
};
struct CreditsEntry *sDispCreditsEntry = NULL;

// related to peach gfx?
static s8 sPeachManualBlinkTime = 0;
static s8 sPeachIsBlinking = FALSE;
static s8 sPeachBlinkTimes[7] = { 2, 3, 2, 1, 2, 3, 2 };

static u8 sStarsNeededForDialog[] = { 1, 3, 8, 30, 50, 70 };

/**
 * Data for the jumbo star cutscene. It specifies the flight path after triple
 * jumping. Each entry is one keyframe.
 * The first number is playback speed, 1000 is the maximum and means it lasts
 * 1 frame. 20 means that it lasts 1000/20 = 50 frames.
 * Speed 0 marks the last keyframe. Since the cubic spline looks 3 keyframes
 * ahead, there should be at least 2 more entries afterwards.
 * The last three numbers of each entry are x, y and z coordinates of points
 * that define the curve.
 */
static Vec4s sJumboStarKeyframes[27] = {
    { 20, 0, 678, -2916 },      { 30, 0, 680, -3500 },      { 40, 1000, 700, -4000 },
    { 50, 2500, 750, -3500 },   { 50, 3500, 800, -2000 },   { 50, 4000, 850, 0 },
    { 50, 3500, 900, 2000 },    { 50, 2000, 950, 3500 },    { 50, 0, 1000, 4000 },
    { 50, -2000, 1050, 3500 },  { 50, -3500, 1100, 2000 },  { 50, -4000, 1150, 0 },
    { 50, -3500, 1200, -2000 }, { 50, -2000, 1250, -3500 }, { 50, 0, 1300, -4000 },
    { 50, 2000, 1350, -3500 },  { 50, 3500, 1400, -2000 },  { 50, 4000, 1450, 0 },
    { 50, 3500, 1500, 2000 },   { 50, 2000, 1600, 3500 },   { 50, 0, 1700, 4000 },
    { 50, -2000, 1800, 3500 },  { 50, -3500, 1900, 2000 },  { 30, -4000, 2000, 0 },
    { 0, -3500, 2100, -2000 },  { 0, -2000, 2200, -3500 },  { 0, 0, 2300, -4000 },
};

#define CREDIT_TEXT_MARGIN_X ((s32)(GFX_DIMENSIONS_ASPECT_RATIO * 21))
#define CREDIT_TEXT_X_LEFT GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(CREDIT_TEXT_MARGIN_X)
#define CREDIT_TEXT_X_RIGHT GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(CREDIT_TEXT_MARGIN_X)


/**
 * Converts the u32 given in DEFINE_COURSE to a u8 with the odd and even digits rotated into the right
 * order for sDanceCutsceneIndexTable
 */
#define DROT(value, index) ((value >> (32 - (index + 1) * 8)) & 0xF0) >> 4 | \
                           ((value >> (32 - (index + 1) * 8)) & 0x0F) << 4

#define DANCE_ENTRY(c) { DROT(c, 0), DROT(c, 1), DROT(c, 2), DROT(c, 3) },

#define DEFINE_COURSE(_0, cutscenes) DANCE_ENTRY(cutscenes)
#define DEFINE_COURSES_END()
#define DEFINE_BONUS_COURSE(_0, cutscenes) DANCE_ENTRY(cutscenes)

/**
 * Each hex digit is an index into sDanceCutsceneTable.
 *
 * 0: Lakitu flies away after the dance
 * 1: Only rotates the camera, doesn't zoom out
 * 2: The camera goes to a close up of Mario
 * 3: Bowser keys and the grand star
 * 4: Default, used for 100 coin stars, 8 red coin stars in bowser levels, and secret stars
 */
u8 sDanceCutsceneIndexTable[][4] = {
    #include "levels/course_defines.h"
    { 0x44, 0x44, 0x44, 0x04 }, // (26) Why go to all this trouble to save bytes and do this?!
};
#undef DEFINE_COURSE
#undef DEFINE_COURSES_END
#undef DEFINE_BONUS_COURSE

#undef DANCE_ENTRY
#undef DROT

/**
 * Maps cutscene to numbers in [0,4]. Used in determine_dance_cutscene() with sDanceCutsceneIndexTable.
 *
 * Only the first 5 entries are used. Perhaps the last 5 were bools used to indicate whether the star
 * type exits the course or not.
 */
u8 sDanceCutsceneTable[] = {
    CUTSCENE_DANCE_FLY_AWAY, CUTSCENE_DANCE_ROTATE, CUTSCENE_DANCE_CLOSEUP, CUTSCENE_KEY_DANCE, CUTSCENE_DANCE_DEFAULT,
    CUTSCENE_NONE,           CUTSCENE_NONE,         CUTSCENE_NONE,          CUTSCENE_NONE,      CUTSCENE_NONE,
};

/**
 * print_displaying_credits_entry: Print the current displaying Credits Entry
 * Called in render_game. This function checks if sDispCreditsEntry points to a
 * credits entry (see act_credits_cutscene), and if so, display it. The reason
 * this is called every frame in render_game is because the credits need to
 * display on top of everything else.
 * To print a credits entry, we take the first character of the first string,
 * subtract the value of the 0 character, and use that as the number of lines to
 * print, excluding the title. Then, we print the title (after advancing the
 * pointer by 1) at X 28, Y either 28 or 172, depending on the print at bottom
 * flag. Finally, we print each line using the number of lines previously stored
 * as a counter, and increase the Y value by either the constant 16 (JP only) or
 * by the value of lineHeight.
 */
void print_displaying_credits_entry(void) {
    if (sDispCreditsEntry != NULL) {
        char **currStrPtr = (char **) sDispCreditsEntry->string;
        char *titleStr = *currStrPtr++;
        s16 numLines = *titleStr++ - '0';

        s16 strY = (sDispCreditsEntry->actNum & 0x20 ? 28 : 172) + (numLines == 1) * 16;
        s16 lineHeight = 16;

        dl_rgba16_begin_cutscene_msg_fade();
        print_credits_string(CREDIT_TEXT_X_LEFT, strY, titleStr);

        switch (numLines) {
            case 4:
                print_credits_string(CREDIT_TEXT_X_LEFT, strY + 24, *currStrPtr++);
                numLines = 2;
                lineHeight = 24;
                break;
            case 5:
                print_credits_string(CREDIT_TEXT_X_LEFT, strY + 16, *currStrPtr++);
                numLines = 3;
                break;
#ifdef VERSION_EU
            case 6:
                print_credits_string(CREDIT_TEXT_X_LEFT, strY + 32, *currStrPtr++);
                numLines = 3;
                break;
            case 7:
                print_credits_string(CREDIT_TEXT_X_LEFT, strY + 16, *currStrPtr++);
                print_credits_string(CREDIT_TEXT_X_LEFT, strY + 32, *currStrPtr++);
                numLines = 3;
                break;
#endif
        }

        while (numLines-- > 0) {
            print_credits_string_aligned(CREDIT_TEXT_X_RIGHT, strY, *currStrPtr, TEXT_ALIGN_RIGHT);

            strY += lineHeight;

            currStrPtr++;
        }

        dl_rgba16_stop_cutscene_msg_fade();
        sDispCreditsEntry = NULL;
    }
}

void bhv_end_peach_loop(void) {
    cur_obj_init_animation_with_sound(sEndPeachAnimation);
    if (cur_obj_check_if_near_animation_end()) {
        // anims: 0-3, 4, 5, 6-8, 9, 10, 11
        if (sEndPeachAnimation <  PEACH_ANIM_3 || sEndPeachAnimation == PEACH_ANIM_DIALOG_1_PART_1 || sEndPeachAnimation == PEACH_ANIM_DIALOG_1_PART_2) {
            sEndPeachAnimation++;
        }
    }
}

void bhv_end_toad_loop(void) {
    s32 toadAnimIndex = (gCurrentObject->oPosX >= 0.0f); // 0: West, 1: East

    cur_obj_init_animation_with_sound(sEndToadAnims[toadAnimIndex]);
    if (cur_obj_check_if_near_animation_end()) {
        // 0-1, 2-3, 4, 5, 6, 7
        if (sEndToadAnims[toadAnimIndex] == TOAD_ANIM_WEST_WAVE_THEN_TURN || sEndToadAnims[toadAnimIndex] == TOAD_ANIM_EAST_NOD_THEN_TURN) {
            sEndToadAnims[toadAnimIndex]++;
        }
    }
}

// Geo switch case function for controlling Peach's eye state.
Gfx *geo_switch_peach_eyes(s32 callContext, struct GraphNode *node, UNUSED s32 context) {
    struct GraphNodeSwitchCase *switchCase = (struct GraphNodeSwitchCase *) node;
    s16 timer;

    if (callContext == GEO_CONTEXT_RENDER) {
        if (sPeachManualBlinkTime == 0) {
            timer = (gAreaUpdateCounter + 0x20) >> 1 & 0x1F;
            if (timer < 7) {
                switchCase->selectedCase = sPeachIsBlinking * 4 + sPeachBlinkTimes[timer];
            } else {
                switchCase->selectedCase = sPeachIsBlinking * 4 + 1;
            }
        } else {
            switchCase->selectedCase = sPeachIsBlinking * 4 + sPeachManualBlinkTime - 1;
        }
    }

    return NULL;
}

/**
 * @return `pullResult` or `pushResult` depending on Mario's door action
 */
u8 open_door_cutscene(u8 pullResult, u8 pushResult) {
    if (sMarioCamState->action == ACT_PULLING_DOOR) {
        return pullResult;
    }
    if (sMarioCamState->action == ACT_PUSHING_DOOR) {
        return pushResult;
    }
    return CUTSCENE_NONE;
}

/**
 * Look up the victory dance cutscene in sDanceCutsceneTable
 *
 * First the index entry is determined based on the course and the star that was just picked up
 * Like the entries in sZoomOutAreaMasks, each entry represents two stars
 * The current courses's 4 bits of the index entry are used as the actual index into sDanceCutsceneTable
 *
 * @return the victory cutscene to use
 */
s32 determine_dance_cutscene(UNUSED struct Camera *c) {
#ifdef NON_STOP_STARS
    return CUTSCENE_DANCE_DEFAULT;
#else
    u8 cutscene = CUTSCENE_NONE;
    u8 cutsceneIndex = 0;
    u8 starIndex = (gLastCompletedStarNum - 1) / 2;
    u8 courseNum = gCurrCourseNum;

    if (starIndex > 3) {
        starIndex = 0;
    }
    if (courseNum > COURSE_MAX) {
        courseNum = COURSE_NONE;
    }
    cutsceneIndex = sDanceCutsceneIndexTable[courseNum][starIndex];

    if (gLastCompletedStarNum & 1) {
        // Odd stars take the lower four bytes
        cutsceneIndex &= 0xF;
    } else {
        // Even stars use the upper four bytes
        cutsceneIndex = cutsceneIndex >> 4;
    }
    cutscene = sDanceCutsceneTable[cutsceneIndex];
    return cutscene;
#endif
}

/**
 * If no cutscenes are playing, determines if a cutscene should play based on Mario's action and
 * cameraEvent
 *
 * @return the cutscene that should start, 0 if none
 */
u8 get_cutscene_from_mario_status(struct Camera *c) {
    u8 cutscene = c->cutscene;

    if (cutscene == CUTSCENE_NONE) {
        // A cutscene started by an object, if any, will start if nothing else happened
        cutscene = sObjectCutscene;
        sObjectCutscene = CUTSCENE_NONE;
        if (sMarioCamState->cameraEvent == CAM_EVENT_DOOR) {
            switch (gCurrLevelArea) {
                case AREA_CASTLE_LOBBY:
                    //! doorStatus is never DOOR_ENTER_LOBBY when cameraEvent == 6, because
                    //! doorStatus is only used for the star door in the lobby, which uses
                    //! ACT_ENTERING_STAR_DOOR
                    if (c->mode == CAMERA_MODE_SPIRAL_STAIRS || c->mode == CAMERA_MODE_CLOSE || c->doorStatus == DOOR_ENTER_LOBBY) {
                        cutscene = open_door_cutscene(CUTSCENE_DOOR_PULL_MODE, CUTSCENE_DOOR_PUSH_MODE);
                    } else {
                        cutscene = open_door_cutscene(CUTSCENE_DOOR_PULL, CUTSCENE_DOOR_PUSH);
                    }
                    break;
                case AREA_BBH:
                    //! Castle Lobby uses 0 to mean 'no special modes', but BBH uses 1...
                    if (c->doorStatus == DOOR_LEAVING_SPECIAL) {
                        cutscene = open_door_cutscene(CUTSCENE_DOOR_PULL, CUTSCENE_DOOR_PUSH);
                    } else {
                        cutscene = open_door_cutscene(CUTSCENE_DOOR_PULL_MODE, CUTSCENE_DOOR_PUSH_MODE);
                    }
                    break;
                default:
                    cutscene = open_door_cutscene(CUTSCENE_DOOR_PULL, CUTSCENE_DOOR_PUSH);
                    break;
            }
        }
        if (sMarioCamState->cameraEvent == CAM_EVENT_DOOR_WARP) {
            cutscene = CUTSCENE_DOOR_WARP;
        }
        if (sMarioCamState->cameraEvent == CAM_EVENT_CANNON) {
            cutscene = CUTSCENE_ENTER_CANNON;
        }
        if (SURFACE_IS_PAINTING_WARP(sMarioGeometry.currFloorType)) {
            cutscene = CUTSCENE_ENTER_PAINTING;
        }
        switch (sMarioCamState->action) {
            case ACT_DEATH_EXIT:
                cutscene = CUTSCENE_DEATH_EXIT;
                break;
            case ACT_EXIT_AIRBORNE:
                cutscene = CUTSCENE_EXIT_PAINTING_SUCC;
                break;
            case ACT_SPECIAL_EXIT_AIRBORNE:
                if (gPrevLevel == LEVEL_BOWSER_1 || gPrevLevel == LEVEL_BOWSER_2
                    || gPrevLevel == LEVEL_BOWSER_3) {
                    cutscene = CUTSCENE_EXIT_BOWSER_SUCC;
                } else {
                    cutscene = CUTSCENE_EXIT_SPECIAL_SUCC;
                }
                break;
            case ACT_SPECIAL_DEATH_EXIT:
                if (gPrevLevel == LEVEL_BOWSER_1 || gPrevLevel == LEVEL_BOWSER_2
                    || gPrevLevel == LEVEL_BOWSER_3) {
                    cutscene = CUTSCENE_EXIT_BOWSER_DEATH;
                } else {
                    cutscene = CUTSCENE_NONPAINTING_DEATH;
                }
                break;
            case ACT_ENTERING_STAR_DOOR:
                if (c->doorStatus == DOOR_DEFAULT) {
                    cutscene = CUTSCENE_SLIDING_DOORS_OPEN;
                } else {
                    cutscene = CUTSCENE_DOOR_PULL_MODE;
                }
                break;
            case ACT_UNLOCKING_KEY_DOOR:
                cutscene = CUTSCENE_UNLOCK_KEY_DOOR;
                break;
            case ACT_WATER_DEATH:
                cutscene = CUTSCENE_WATER_DEATH;
                break;
            case ACT_DEATH_ON_BACK:
                cutscene = CUTSCENE_DEATH_ON_BACK;
                break;
            case ACT_DEATH_ON_STOMACH:
                cutscene = CUTSCENE_DEATH_ON_STOMACH;
                break;
            case ACT_STANDING_DEATH:
                cutscene = CUTSCENE_STANDING_DEATH;
                break;
            case ACT_SUFFOCATION:
                cutscene = CUTSCENE_SUFFOCATION_DEATH;
                break;
            case ACT_QUICKSAND_DEATH:
                cutscene = CUTSCENE_QUICKSAND_DEATH;
                break;
            case ACT_ELECTROCUTION:
                cutscene = CUTSCENE_STANDING_DEATH;
                break;
            case ACT_STAR_DANCE_EXIT:
                cutscene = determine_dance_cutscene(c);
                break;
            case ACT_STAR_DANCE_WATER:
                cutscene = determine_dance_cutscene(c);
                break;
            case ACT_STAR_DANCE_NO_EXIT:
                cutscene = CUTSCENE_DANCE_DEFAULT;
                break;
        }
        switch (sMarioCamState->cameraEvent) {
            case CAM_EVENT_START_INTRO:
                cutscene = CUTSCENE_INTRO_PEACH;
                break;
            case CAM_EVENT_START_GRAND_STAR:
                cutscene = CUTSCENE_GRAND_STAR;
                break;
            case CAM_EVENT_START_ENDING:
                cutscene = CUTSCENE_ENDING;
                break;
            case CAM_EVENT_START_END_WAVING:
                cutscene = CUTSCENE_END_WAVING;
                break;
            case CAM_EVENT_START_CREDITS:
                cutscene = CUTSCENE_CREDITS;
                break;
        }
    }
    //! doorStatus is reset every frame. CameraTriggers need to constantly set doorStatus
    c->doorStatus = DOOR_DEFAULT;

    return cutscene;
}

/**
 * get_star_collection_dialog: Determine what dialog should show when Mario
 * collects a star.
 * Determines if Mario has collected enough stars to get a dialog for it, and
 * if so, return the dialog ID. Otherwise, return 0. A dialog is returned if
 * numStars has reached a milestone and prevNumStarsForDialog has not reached it.
 */
s32 get_star_collection_dialog(struct MarioState *m) {
    s32 i;
    s32 dialogID = 0;
    s32 numStarsRequired;

    for (i = 0; i < ARRAY_COUNT(sStarsNeededForDialog); i++) {
        numStarsRequired = sStarsNeededForDialog[i];
        if (m->prevNumStarsForDialog < numStarsRequired && m->numStars >= numStarsRequired) {
            dialogID = i + DIALOG_141;
            break;
        }
    }

    m->prevNumStarsForDialog = m->numStars;
    return dialogID;
}

// save menu handler
void handle_save_menu(struct MarioState *m) {
    s32 dialogID;
    // wait for the menu to show up
    if (is_anim_past_end(m) && gSaveOptSelectIndex != MENU_OPT_NONE) {
        // save and continue / save and quit
        if (gSaveOptSelectIndex == MENU_OPT_SAVE_AND_CONTINUE || gSaveOptSelectIndex == MENU_OPT_SAVE_AND_QUIT) {
            save_file_do_save(gCurrSaveFileNum - 1);

            if (gSaveOptSelectIndex == MENU_OPT_SAVE_AND_QUIT) {
                fade_into_special_warp(WARP_SPECIAL_MARIO_HEAD_REGULAR, 0); // reset game
            }
        }

        // not quitting
        if (gSaveOptSelectIndex != MENU_OPT_SAVE_AND_QUIT) {
            disable_time_stop();
            m->faceAngle[1] += 0x8000;
            // figure out what dialog to show, if we should
            dialogID = get_star_collection_dialog(m);
            if (dialogID) {
                play_peachs_jingle();
                // look up for dialog
                set_mario_action(m, ACT_READING_AUTOMATIC_DIALOG, dialogID);
            } else {
                set_mario_action(m, ACT_IDLE, 0);
            }
        }
    }
}

/**
 * spawn_obj_at_mario_rel_yaw: Spawns object at Mario with relative yaw.
 * Spawns object with given behavior and model and copies over Mario's position
 * and yaw plus relative yaw.
 */
struct Object *spawn_obj_at_mario_rel_yaw(struct MarioState *m, ModelID32 model, const BehaviorScript *behavior, s16 relYaw) {
    struct Object *obj = spawn_object(m->marioObj, model, behavior);

    obj->oFaceAngleYaw = m->faceAngle[1] + relYaw;
    obj->oPosX = m->pos[0];
    obj->oPosY = m->pos[1];
    obj->oPosZ = m->pos[2];

    return obj;
}

/**
 * cutscene_take_cap_off: Put Mario's cap on.
 * Clears "cap on head" flag, sets "cap in hand" flag, plays sound
 * SOUND_ACTION_TAKE_OFF_CAP.
 */
void cutscene_take_cap_off(struct MarioState *m) {
    m->flags &= ~MARIO_CAP_ON_HEAD;
    m->flags |= MARIO_CAP_IN_HAND;
    play_sound(SOUND_ACTION_TAKE_OFF_CAP, m->marioObj->header.gfx.cameraToObject);
}

/**
 * cutscene_put_cap_on: Put Mario's cap on.
 * Clears "cap in hand" flag, sets "cap on head" flag, plays sound
 * SOUND_ACTION_PUT_ON_CAP.
 */
void cutscene_put_cap_on(struct MarioState *m) {
    m->flags &= ~MARIO_CAP_IN_HAND;
    m->flags |= MARIO_CAP_ON_HEAD;
    play_sound(SOUND_ACTION_PUT_ON_CAP, m->marioObj->header.gfx.cameraToObject);
}

/**
 * mario_ready_to_speak: Determine if Mario is able to speak to a NPC
 * The following conditions must be met in order for Mario to be considered
 * ready to speak.
 * 1: Mario's action must be in the stationary or moving action groups, or if
 *    not, he must be in the "waiting for dialog" state.
 * 2: Mario mat not be riding a shell or be invulnerable.
 * 3: Mario must not be in first person mode.
 */
s32 mario_ready_to_speak(void) {
    u32 actionGroup = gMarioState->action & ACT_GROUP_MASK;
    s32 isReadyToSpeak = FALSE;

    if ((gMarioState->action == ACT_WAITING_FOR_DIALOG || actionGroup == ACT_GROUP_STATIONARY
         || actionGroup == ACT_GROUP_MOVING)
        && (!(gMarioState->action & (ACT_FLAG_RIDING_SHELL | ACT_FLAG_INVULNERABLE))
            && gMarioState->action != ACT_FIRST_PERSON)) {
        isReadyToSpeak = TRUE;
    }

    return isReadyToSpeak;
}

// (can) place Mario in dialog?
// initiate dialog?
// return values:
// 0 = not in dialog
// 1 = starting dialog
// 2 = speaking
s32 set_mario_npc_dialog(s32 actionArg) {
    s32 dialogState = MARIO_DIALOG_STATUS_NONE;

    // in dialog
    if (gMarioState->action == ACT_READING_NPC_DIALOG) {
        if (gMarioState->actionState < 8) {
            dialogState = MARIO_DIALOG_STATUS_START; // starting dialog
        }
        if (gMarioState->actionState == 8) {
            if (actionArg == MARIO_DIALOG_STOP) {
                gMarioState->actionState++; // exit dialog
            } else {
                dialogState = MARIO_DIALOG_STATUS_SPEAK;
            }
        }
    } else if (actionArg != MARIO_DIALOG_STOP && mario_ready_to_speak()) {
        gMarioState->usedObj = gCurrentObject;
        set_mario_action(gMarioState, ACT_READING_NPC_DIALOG, actionArg);
        dialogState = MARIO_DIALOG_STATUS_START; // starting dialog
    }

    return dialogState;
}

// actionargs:
// 1 : no head turn
// 2 : look up
// 3 : look down
// actionstate values:
// 0 - 7: looking toward npc
// 8: in dialog
// 9 - 22: looking away from npc
// 23: end
s32 act_reading_npc_dialog(struct MarioState *m) {
    s32 headTurnAmount = 0;

    if (m->actionArg == MARIO_DIALOG_LOOK_UP) {
        headTurnAmount = -1024;
    }
    if (m->actionArg == MARIO_DIALOG_LOOK_DOWN) {
        headTurnAmount = 384;
    }

    if (m->actionState < 8) {
        // turn to NPC
        m->faceAngle[1] = approach_angle(m->faceAngle[1], mario_obj_angle_to_object(m, m->usedObj), 0x800);
        // turn head to npc
        m->actionTimer += headTurnAmount;
        // set animation
        set_mario_animation(m, m->heldObj == NULL ? MARIO_ANIM_FIRST_PERSON
                                                  : MARIO_ANIM_IDLE_WITH_LIGHT_OBJ);
    } else if (m->actionState >= 9 && m->actionState < 17) {
        // look back from facing NPC
        m->actionTimer -= headTurnAmount;
    } else if (m->actionState == 23) {
        if (m->flags & MARIO_CAP_IN_HAND) {
            set_mario_action(m, ACT_PUTTING_ON_CAP, 0);
        } else {
            set_mario_action(m, m->heldObj == NULL ? ACT_IDLE : ACT_HOLD_IDLE, 0);
        }
    }
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
    vec3s_set(m->marioBodyState->headAngle, m->actionTimer, 0, 0);

    if (m->actionState != 8) {
        m->actionState++;
    }

    return FALSE;
}

// puts Mario in a state where he's waiting for (npc) dialog; doesn't do much
s32 act_waiting_for_dialog(struct MarioState *m) {
    set_mario_animation(m, m->heldObj == NULL ? MARIO_ANIM_FIRST_PERSON
                                              : MARIO_ANIM_IDLE_WITH_LIGHT_OBJ);
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
    return FALSE;
}

// makes Mario disappear and triggers warp
s32 act_disappeared(struct MarioState *m) {
    set_mario_animation(m, MARIO_ANIM_A_POSE);
    stop_and_set_height_to_floor(m);
    m->marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
    if (m->actionArg) {
        m->actionArg--;
        if ((m->actionArg & 0xFFFF) == 0) {
            level_trigger_warp(m, m->actionArg >> 16);
        }
    }
    return FALSE;
}

s32 act_reading_automatic_dialog(struct MarioState *m) {
    u32 actionArg;

    m->actionState++;
    if (m->actionState == 2) {
        enable_time_stop();
    }
    if (m->actionState < 9) {
        set_mario_animation(m, m->prevAction == ACT_STAR_DANCE_WATER ? MARIO_ANIM_WATER_IDLE
                                                                     : MARIO_ANIM_FIRST_PERSON);
        // always look up for automatic dialogs
        m->actionTimer -= 1024;
    } else {
        // set Mario dialog
        if (m->actionState == 9) {
            actionArg = m->actionArg;
            if (GET_HIGH_U16_OF_32(actionArg) == 0) {
                create_dialog_box(GET_LOW_U16_OF_32(actionArg));
            } else {
                create_dialog_box_with_int_var(GET_HIGH_U16_OF_32(actionArg), GET_LOW_U16_OF_32(actionArg));
            }
        }
        // wait until dialog is done
        else if (m->actionState == 10) {
            if (get_dialog_id() >= 0) {
                m->actionState--;
            }
        }
        // look back down
        else if (m->actionState < 19) {
            m->actionTimer += 1024;
        }
        // finished action
        else if (m->actionState == 25) {
            disable_time_stop();
            if (gNeverEnteredCastle) {
                gNeverEnteredCastle = FALSE;
                play_cutscene_music(SEQUENCE_ARGS(0, SEQ_LEVEL_INSIDE_CASTLE));
            }
            if (m->prevAction == ACT_STAR_DANCE_WATER) {
                set_mario_action(m, ACT_WATER_IDLE, 0); // 100c star?
            } else {
                // make Mario walk into door after star dialog
                set_mario_action(m, m->prevAction == ACT_UNLOCKING_STAR_DOOR ? ACT_WALKING : ACT_IDLE,
                                 0);
            }
        }
    }
    // apply head turn
    vec3s_set(m->marioBodyState->headAngle, m->actionTimer, 0, 0);
    return FALSE;
}

s32 act_reading_sign(struct MarioState *m) {
    if (m->pos[1] < (m->floorHeight + m->usedObj->hitboxHeight)) {
        m->pos[1] = m->floorHeight; // Fixes sign on slopes
    }

    struct Object *marioObj = m->marioObj;

    play_sound_if_no_flag(m, SOUND_ACTION_READ_SIGN, MARIO_ACTION_SOUND_PLAYED);

    switch (m->actionState) {
        // start dialog
        case 0:
            trigger_cutscene_dialog(1);
            enable_time_stop();
            // reading sign
            set_mario_animation(m, MARIO_ANIM_FIRST_PERSON);
            m->actionState = ACT_STATE_READING_SIGN_TURN_TOWARD_SIGN;
            // intentional fallthrough
        case ACT_STATE_READING_SIGN_TURN_TOWARD_SIGN:
            m->faceAngle[1] += marioObj->oMarioReadingSignDYaw  / 11;
            m->pos[0] += marioObj->oMarioReadingSignDPosX / 11.0f;
            m->pos[2] += marioObj->oMarioReadingSignDPosZ / 11.0f;
            // create the text box
            if (m->actionTimer++ == 10) {
                create_dialog_inverted_box(m->usedObj->oBehParams2ndByte);
                m->actionState = ACT_STATE_READING_SIGN_IN_DIALOG;
            }
            break;
        case ACT_STATE_READING_SIGN_IN_DIALOG:
            // dialog finished
            if (gCamera->cutscene == CUTSCENE_NONE) {
                disable_time_stop();
                set_mario_action(m, ACT_IDLE, 0);
            }
            break;
    }

    vec3f_copy(marioObj->header.gfx.pos, m->pos);
    vec3s_set(marioObj->header.gfx.angle, 0x0, m->faceAngle[1], 0x0);
    return FALSE;
}

s32 act_debug_free_move(struct MarioState *m) {
    struct WallCollisionData wallData;
    struct Surface *floor, *ceil;
    Vec3f pos;

    if (gPlayer1Controller->buttonPressed & L_TRIG) {
        m->health = 0x880;
    }

    f32 speed = (gPlayer1Controller->buttonDown & B_BUTTON) ? 4.0f : 1.0f;
    if (gPlayer1Controller->buttonDown & Z_TRIG) speed = 0.01f;
    if (m->area->camera->mode != CAMERA_MODE_8_DIRECTIONS) set_camera_mode(m->area->camera, CAMERA_MODE_8_DIRECTIONS, 1);

    set_mario_animation(m, MARIO_ANIM_A_POSE);
    vec3f_copy(pos, m->pos);

    if (gPlayer1Controller->buttonDown & U_JPAD) {
        pos[1] += 16.0f * speed;
    }
    if (gPlayer1Controller->buttonDown & D_JPAD) {
        pos[1] -= 16.0f * speed;
    }
    if (gPlayer1Controller->buttonPressed & A_BUTTON) {
        vec3_zero(m->vel);
        m->forwardVel = 0.0f;

        set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
        m->input &= ~INPUT_A_PRESSED;
        if (m->pos[1] <= (m->waterLevel - 100)) {
            return set_mario_action(m, ACT_WATER_IDLE, 0);
        } else if (m->pos[1] <= m->floorHeight) {
            return set_mario_action(m, ACT_IDLE, 0);
        } else {
            // slight upwards boost to get you some hover time
            m->vel[1] = 20.0f;
            gPlayer1Controller->buttonDown &= ~U_JPAD;
            return set_mario_action(m, ACT_FREEFALL, 0);
        }
    }

    if (m->intendedMag > 0) {
        speed *= m->intendedMag * 2.0f;
        pos[0] += speed * sins(m->intendedYaw);
        pos[2] += speed * coss(m->intendedYaw);
    }

    // TODO: Add ability to ignore collision
    //      - spawn pseudo floor object to prevent OOB death
    resolve_and_return_wall_collisions(pos, 60.0f, 50.0f, &wallData);

    set_mario_wall(m, ((wallData.numWalls > 0) ? wallData.walls[0] : NULL));
    f32 floorHeight = find_floor(pos[0], pos[1], pos[2], &floor);
    f32 ceilHeight = find_mario_ceil(pos, floorHeight, &ceil);

    if (floor == NULL) return FALSE;

    if (ceilHeight - floorHeight >= 160.0f) {
        if (floor != NULL && pos[1] < floorHeight) {
            pos[1] = floorHeight;
        }
        if (ceil != NULL && pos[1] + 160.0f > ceilHeight) {
            pos[1] = ceilHeight - 160.0f;
        }
        vec3f_copy(m->pos, pos);
    }

    m->faceAngle[1] = m->intendedYaw;
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);

    return FALSE;
}

void general_star_dance_handler(struct MarioState *m, s32 isInWater) {
    struct Object *celebStar = NULL;

    if (m->actionState == ACT_STATE_STAR_DANCE_CUTSCENE) {
        switch (++m->actionTimer) {
            case 1:
                celebStar = spawn_object(m->marioObj, MODEL_STAR, bhvCelebrationStar);
#ifdef STAR_DANCE_USES_STARS_MODEL
                obj_set_model(celebStar, gStarModelLastCollected);
#else
                if (gStarModelLastCollected == MODEL_BOWSER_KEY) {
                    obj_set_model(celebStar, gStarModelLastCollected);
                }
#endif
                disable_background_sound();
                //! TODO: Is this check necessary? Both seem to do the exact same thing.
                if (m->actionArg & 1) {
                    // No exit
                    play_course_clear(obj_has_model(celebStar, MODEL_BOWSER_KEY));
                } else {
                    // Exit
                    if (obj_has_model(celebStar, MODEL_BOWSER_KEY)) {
                        play_music(SEQ_PLAYER_ENV, SEQUENCE_ARGS(15, SEQ_EVENT_CUTSCENE_COLLECT_KEY), 0);
                    } else {
                        play_music(SEQ_PLAYER_ENV, SEQUENCE_ARGS(15, SEQ_EVENT_CUTSCENE_COLLECT_STAR), 0);
                    }
                }
                break;

            case 42:
                play_sound(SOUND_MARIO_HERE_WE_GO, m->marioObj->header.gfx.cameraToObject);
                break;

            case 80:
                if (!(m->actionArg & 1)) {
                    level_trigger_warp(m, WARP_OP_STAR_EXIT);
                } else {
                    enable_time_stop();
                    create_dialog_box_with_response(gLastCompletedStarNum == 7 ? DIALOG_013 : DIALOG_014);
                    m->actionState = ACT_STATE_STAR_DANCE_DO_SAVE;
                }
                break;
        }
    } else if (m->actionState == ACT_STATE_STAR_DANCE_DO_SAVE && gDialogResponse != DIALOG_RESPONSE_NONE) {
        if (gDialogResponse == DIALOG_RESPONSE_YES) {
            save_file_do_save(gCurrSaveFileNum - 1);
        }
        m->actionState = ACT_STATE_STAR_DANCE_RETURN;
    } else if (m->actionState == ACT_STATE_STAR_DANCE_RETURN && is_anim_at_end(m)) {
        disable_time_stop();
        enable_background_sound();
        s32 dialogID = get_star_collection_dialog(m);
        if (dialogID) {
            // look up for dialog
            set_mario_action(m, ACT_READING_AUTOMATIC_DIALOG, dialogID);
        } else {
            set_mario_action(m, isInWater ? ACT_WATER_IDLE : ACT_IDLE, 0);
        }
    }
}

s32 act_star_dance(struct MarioState *m) {
    m->faceAngle[1] = m->area->camera->yaw;
    set_mario_animation(m, m->actionState == ACT_STATE_STAR_DANCE_RETURN ? MARIO_ANIM_RETURN_FROM_STAR_DANCE
                                                                         : MARIO_ANIM_STAR_DANCE);
    general_star_dance_handler(m, FALSE);
    if (m->actionState != ACT_STATE_STAR_DANCE_RETURN && m->actionTimer >= 40) {
        m->marioBodyState->handState = MARIO_HAND_PEACE_SIGN;
    }
    stop_and_set_height_to_floor(m);
    return FALSE;
}

s32 act_star_dance_water(struct MarioState *m) {
    m->faceAngle[1] = m->area->camera->yaw;
    set_mario_animation(m, m->actionState == ACT_STATE_STAR_DANCE_RETURN ? MARIO_ANIM_RETURN_FROM_WATER_STAR_DANCE
                                                                         : MARIO_ANIM_WATER_STAR_DANCE);
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
    general_star_dance_handler(m, TRUE);
    if (m->actionState != ACT_STATE_STAR_DANCE_RETURN && m->actionTimer >= 62) {
        m->marioBodyState->handState = MARIO_HAND_PEACE_SIGN;
    }
    return FALSE;
}

s32 act_fall_after_star_grab(struct MarioState *m) {
    if (m->pos[1] < m->waterLevel - 130) {
        play_sound(SOUND_ACTION_WATER_PLUNGE, m->marioObj->header.gfx.cameraToObject);
        m->particleFlags |= PARTICLE_WATER_SPLASH;
        return set_mario_action(m, ACT_STAR_DANCE_WATER, m->actionArg);
    }
    if (perform_air_step(m, AIR_STEP_CHECK_LEDGE_GRAB) == AIR_STEP_LANDED) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
        set_mario_action(m, m->actionArg & 1 ? ACT_STAR_DANCE_NO_EXIT : ACT_STAR_DANCE_EXIT,
                         m->actionArg);
    }
    set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
    return FALSE;
}

s32 common_death_handler(struct MarioState *m, s32 animation, s32 frameToDeathWarp) {
    s32 animFrame = set_mario_animation(m, animation);
    if (animFrame == frameToDeathWarp) {
        level_trigger_warp(m, WARP_OP_DEATH);
    }
    m->marioBodyState->eyeState = MARIO_EYES_DEAD;
    stop_and_set_height_to_floor(m);
#ifdef PREVENT_DEATH_LOOP
    m->isDead = TRUE;
#endif
    return animFrame;
}

s32 act_standing_death(struct MarioState *m) {
    if (m->input & INPUT_IN_POISON_GAS) {
        return set_mario_action(m, ACT_SUFFOCATION, 0);
    }

    play_sound_if_no_flag(m, SOUND_MARIO_DYING, MARIO_ACTION_SOUND_PLAYED);
    common_death_handler(m, MARIO_ANIM_DYING_FALL_OVER, 80);
    if (m->marioObj->header.gfx.animInfo.animFrame == 77) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);
    }
    return FALSE;
}

s32 act_electrocution(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_MARIO_DYING, MARIO_ACTION_SOUND_PLAYED);
    common_death_handler(m, MARIO_ANIM_ELECTROCUTION, 43);
    return FALSE;
}

s32 act_suffocation(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_MARIO_DYING, MARIO_ACTION_SOUND_PLAYED);
    common_death_handler(m, MARIO_ANIM_SUFFOCATING, 86);
    return FALSE;
}

s32 act_death_on_back(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_MARIO_DYING, MARIO_ACTION_SOUND_PLAYED);
    if (common_death_handler(m, MARIO_ANIM_DYING_ON_BACK, 54) == 40) {
        play_mario_heavy_landing_sound(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);
    }
    return FALSE;
}

s32 act_death_on_stomach(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_MARIO_DYING, MARIO_ACTION_SOUND_PLAYED);
    if (common_death_handler(m, MARIO_ANIM_DYING_ON_STOMACH, 37) == 37) {
        play_mario_heavy_landing_sound(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);
    }
    return FALSE;
}

s32 act_quicksand_death(struct MarioState *m) {
    if (m->actionState == ACT_STATE_QUICKSAND_DEATH_INIT) {
        set_mario_animation(m, MARIO_ANIM_DYING_IN_QUICKSAND);
        set_anim_to_frame(m, 60);
        m->actionState = ACT_STATE_QUICKSAND_DEATH_SINK;
    }
    if (m->actionState == ACT_STATE_QUICKSAND_DEATH_SINK) {
        if (m->quicksandDepth >= 100.0f) {
            play_sound_if_no_flag(m, SOUND_MARIO_WAAAOOOW, MARIO_ACTION_SOUND_PLAYED);
        }
        if ((m->quicksandDepth += 5.0f) >= 180.0f) {
            level_trigger_warp(m, WARP_OP_DEATH);
            m->actionState = ACT_STATE_QUICKSAND_DEATH_DEAD;
        }
    }
    stationary_ground_step(m);
    play_sound(SOUND_MOVING_QUICKSAND_DEATH, m->marioObj->header.gfx.cameraToObject);
    return FALSE;
}

s32 act_eaten_by_bubba(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_MARIO_DYING, MARIO_ACTION_SOUND_PLAYED);
    set_mario_animation(m, MARIO_ANIM_A_POSE);
    m->marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
#ifdef BREATH_METER
    m->breath = 0xFF;
#endif
    m->health = 0xFF;
    if (m->actionTimer++ == 60) {
        level_trigger_warp(m, WARP_OP_DEATH);
    }
    return FALSE;
}

// set animation and forwardVel; when perform_air_step returns AIR_STEP_LANDED,
// set the new action
s32 launch_mario_until_land(struct MarioState *m, s32 endAction, s32 animation, f32 forwardVel) {
    s32 airStepLanded;
    mario_set_forward_vel(m, forwardVel);
    set_mario_animation(m, animation);
    airStepLanded = (perform_air_step(m, 0) == AIR_STEP_LANDED);
    if (airStepLanded) {
        set_mario_action(m, endAction, 0);
    }
    return airStepLanded;
}

s32 act_unlocking_key_door(struct MarioState *m) {
    m->faceAngle[1] = m->usedObj->oMoveAngleYaw;

    m->pos[0] = m->usedObj->oPosX + coss(m->faceAngle[1]) * 75.0f;
    m->pos[2] = m->usedObj->oPosZ + sins(m->faceAngle[1]) * 75.0f;

    if (m->actionArg & WARP_FLAG_DOOR_FLIP_MARIO) {
        m->faceAngle[1] += 0x8000;
    }

    if (m->actionTimer == 0) {
        spawn_obj_at_mario_rel_yaw(m, MODEL_BOWSER_KEY_CUTSCENE, bhvBowserKeyUnlockDoor, 0);
        set_mario_animation(m, MARIO_ANIM_UNLOCK_DOOR);
    }

    switch (m->marioObj->header.gfx.animInfo.animFrame) {
        case 79:
            play_sound(SOUND_GENERAL_DOOR_INSERT_KEY, m->marioObj->header.gfx.cameraToObject);
            break;
        case 111:
            play_sound(SOUND_GENERAL_DOOR_TURN_KEY, m->marioObj->header.gfx.cameraToObject);
            break;
    }

    update_mario_pos_for_anim(m);
    stop_and_set_height_to_floor(m);

    if (is_anim_at_end(m)) {
        if (GET_BPARAM1(m->usedObj->oBehParams) == KEY_DOOR_BP1_UPSTAIRS) {
            save_file_set_flags(SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR);
            save_file_clear_flags(SAVE_FLAG_HAVE_KEY_2);
        } else {
            save_file_set_flags(SAVE_FLAG_UNLOCKED_BASEMENT_DOOR);
            save_file_clear_flags(SAVE_FLAG_HAVE_KEY_1);
        }
        set_mario_action(m, ACT_WALKING, 0);
    }

    m->actionTimer++;
    return FALSE;
}

s32 act_unlocking_star_door(struct MarioState *m) {
    switch (m->actionState) {
        case ACT_STATE_UNLOCKING_STAR_DOOR_MOVE_POS:
            m->faceAngle[1] = m->usedObj->oMoveAngleYaw;
            if (m->actionArg & WARP_FLAG_DOOR_FLIP_MARIO) {
                m->faceAngle[1] += 0x8000;
            }
            m->marioObj->oMarioReadingSignDPosX = m->pos[0];
            m->marioObj->oMarioReadingSignDPosZ = m->pos[2];
            set_mario_animation(m, MARIO_ANIM_SUMMON_STAR);
            m->actionState = ACT_STATE_UNLOCKING_STAR_DOOR_SUMMON_STAR;
            break;
        case ACT_STATE_UNLOCKING_STAR_DOOR_SUMMON_STAR:
            if (is_anim_at_end(m)) {
                spawn_object(m->marioObj, MODEL_STAR, bhvUnlockDoorStar);
                m->actionState = ACT_STATE_UNLOCKING_STAR_DOOR_APPROACH_DOOR;
            }
            break;
        case ACT_STATE_UNLOCKING_STAR_DOOR_APPROACH_DOOR:
            if (m->actionTimer++ == 70) {
                set_mario_animation(m, MARIO_ANIM_RETURN_STAR_APPROACH_DOOR);
                m->actionState = ACT_STATE_UNLOCKING_STAR_DOOR_IN_DIALOG;
            }
            break;
        case ACT_STATE_UNLOCKING_STAR_DOOR_IN_DIALOG:
            if (is_anim_at_end(m)) {
                save_file_set_flags(get_door_save_file_flag(m->usedObj));
                set_mario_action(m, ACT_READING_AUTOMATIC_DIALOG, DIALOG_038);
            }
            break;
    }

    m->pos[0] = m->marioObj->oMarioReadingSignDPosX;
    m->pos[2] = m->marioObj->oMarioReadingSignDPosZ;

    update_mario_pos_for_anim(m);
    stop_and_set_height_to_floor(m);

    return FALSE;
}

s32 act_entering_star_door(struct MarioState *m) {
    f32 targetDX;
    f32 targetDZ;
    s16 targetAngle;

    if (m->actionTimer++ == 0) {
        m->interactObj->oInteractStatus = INT_STATUS_DOOR_PULLED;

        // ~30 degrees / 1/12 rot
        targetAngle = m->usedObj->oMoveAngleYaw + DEGREES(30);
        if (m->actionArg & WARP_FLAG_DOOR_FLIP_MARIO) {
            targetAngle += 0x5556; // ~120 degrees / 1/3 rot (total 150d / 5/12)
        }

        // targetDX and targetDZ are the offsets to add to Mario's position to
        // have Mario stand 150 units in front of the door

        targetDX = m->usedObj->oPosX + 150.0f * sins(targetAngle) - m->pos[0];
        targetDZ = m->usedObj->oPosZ + 150.0f * coss(targetAngle) - m->pos[2];

        m->marioObj->oMarioReadingSignDPosX = targetDX / 20.0f;
        m->marioObj->oMarioReadingSignDPosZ = targetDZ / 20.0f;

        m->faceAngle[1] = atan2s(targetDZ, targetDX);
    }

    // set Mario's animation
    if (m->actionTimer < 15) {
        set_mario_animation(m, MARIO_ANIM_FIRST_PERSON);
    }

    // go through door? for 20 frames
    else if (m->actionTimer < 35) {
        m->pos[0] += m->marioObj->oMarioReadingSignDPosX;
        m->pos[2] += m->marioObj->oMarioReadingSignDPosZ;

        set_mario_anim_with_accel(m, MARIO_ANIM_WALKING, 0x00028000);
    }

    else {
        m->faceAngle[1] = m->usedObj->oMoveAngleYaw;

        if (m->actionArg & WARP_FLAG_DOOR_FLIP_MARIO) {
            m->faceAngle[1] += 0x8000;
        }

        m->pos[0] += 12.0f * sins(m->faceAngle[1]);
        m->pos[2] += 12.0f * coss(m->faceAngle[1]);

        set_mario_anim_with_accel(m, MARIO_ANIM_WALKING, 0x00028000);
    }

    stop_and_set_height_to_floor(m);

    if (m->actionTimer == 48) {
        set_mario_action(m, ACT_IDLE, 0);
    }

    return FALSE;
}

s32 act_going_through_door(struct MarioState *m) {
    if (m->actionTimer == 0) {
        if (m->actionArg & WARP_FLAG_DOOR_PULLED) {
            m->interactObj->oInteractStatus = INT_STATUS_DOOR_PULLED;
            set_mario_animation(m, MARIO_ANIM_PULL_DOOR_WALK_IN);
        } else {
            m->interactObj->oInteractStatus = INT_STATUS_DOOR_PUSHED;
            set_mario_animation(m, MARIO_ANIM_PUSH_DOOR_WALK_IN);
        }
    }
    m->faceAngle[1] = m->usedObj->oMoveAngleYaw;
    m->pos[0] = m->usedObj->oPosX;
    m->pos[2] = m->usedObj->oPosZ;

    update_mario_pos_for_anim(m);
    stop_and_set_height_to_floor(m);

    if (m->actionArg & WARP_FLAG_DOOR_IS_WARP) {
        if (m->actionTimer == 16) {
            level_trigger_warp(m, WARP_OP_WARP_DOOR);
        }
    } else if (is_anim_at_end(m)) {
        if (m->actionArg & WARP_FLAG_DOOR_FLIP_MARIO) {
            m->faceAngle[1] += 0x8000;
        }
        set_mario_action(m, ACT_IDLE, 0);
    }

    m->actionTimer++;
    return FALSE;
}

s32 act_warp_door_spawn(struct MarioState *m) {
    if (m->actionState == ACT_STATE_WARP_DOOR_SPAWN_SET_INTERACT_STATUS) {
        m->actionState = ACT_STATE_WARP_DOOR_EXIT_DOOR;
        if (m->actionArg & WARP_FLAG_DOOR_PULLED) {
            m->usedObj->oInteractStatus = INT_STATUS_WARP_DOOR_PULLED;
        } else {
            m->usedObj->oInteractStatus = INT_STATUS_WARP_DOOR_PUSHED;
        }
    } else if (m->usedObj->oAction == DOOR_ACT_CLOSED) {
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
        if (gNeverEnteredCastle && gCurrLevelNum == LEVEL_CASTLE) {
            set_mario_action(m, ACT_READING_AUTOMATIC_DIALOG, DIALOG_021);
        } else {
            set_mario_action(m, ACT_IDLE, 0);
        }
#else
        set_mario_action(m, ACT_IDLE, 0);
#endif
    }
    set_mario_animation(m, MARIO_ANIM_FIRST_PERSON);
    stop_and_set_height_to_floor(m);
    return FALSE;
}

s32 act_emerge_from_pipe(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;

    if (m->actionTimer++ < 11) {
        marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
        return FALSE;
    }

    marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;

    play_sound_if_no_flag(m, SOUND_MARIO_YAHOO, MARIO_MARIO_SOUND_PLAYED);
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    if (gCurrLevelNum == LEVEL_THI) {
        if (gCurrAreaIndex == 2) {
            play_sound_if_no_flag(m, SOUND_MENU_EXIT_PIPE, MARIO_ACTION_SOUND_PLAYED);
        } else {
            play_sound_if_no_flag(m, SOUND_MENU_ENTER_PIPE, MARIO_ACTION_SOUND_PLAYED);
        }
    }
#endif

    if (launch_mario_until_land(m, ACT_JUMP_LAND_STOP, MARIO_ANIM_SINGLE_JUMP, 8.0f)) {
        mario_set_forward_vel(m, 0.0f);
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
    }
    return FALSE;
}

s32 act_spawn_spin_airborne(struct MarioState *m) {
    // entered water, exit action
    if (m->pos[1] < m->waterLevel - 100) {
        load_level_init_text(0);
        return set_water_plunge_action(m);
    }

    // updates all velocity variables based on m->forwardVel
    mario_set_forward_vel(m, m->forwardVel);

    // landed on floor, play spawn land animation
    if (perform_air_step(m, AIR_STEP_CHECK_NONE) == AIR_STEP_LANDED) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
        set_mario_action(m, ACT_SPAWN_SPIN_LANDING, 0);
    }

    // is 300 units above floor, spin and play woosh sounds
    if (m->actionState == ACT_STATE_SPAWN_SPIN_AIRBORNE_SPINNING && m->pos[1] - m->floorHeight > 300.0f) {
        if (set_mario_animation(m, MARIO_ANIM_FORWARD_SPINNING) == 0) { // first anim frame
            play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
        }
    }

    // under 300 units above floor, enter freefall animation
    else {
        m->actionState = ACT_STATE_SPAWN_SPIN_AIRBORNE_FALLING;
        set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
    }

    return FALSE;
}

s32 act_spawn_spin_landing(struct MarioState *m) {
    stop_and_set_height_to_floor(m);
    set_mario_animation(m, MARIO_ANIM_GENERAL_LAND);
    if (is_anim_at_end(m)) {
        load_level_init_text(0);
        set_mario_action(m, ACT_IDLE, 0);
    }
    return FALSE;
}

/**
 * act_exit_airborne: Jump out of a level after collecting a Power Star (no
 ** sparkles)
 * Mario always faces a level entrance when he launches out of it, whether he
 * died or he collected a star/key. Because of that, we need him to move away
 * from the painting by setting his speed to -32.0f and have him face away from
 * the painting by adding 0x8000 (180 deg) to his graphics angle. We also set
 * his heal counter to 31 to restore 7.75 units of his health, and enable the
 * particle flag that generates sparkles.
 */
s32 act_exit_airborne(struct MarioState *m) {
#ifdef BREATH_METER
    m->breath = 0x880;
#endif
    if (15 < m->actionTimer++
        && launch_mario_until_land(m, ACT_EXIT_LAND_SAVE_DIALOG, MARIO_ANIM_GENERAL_FALL, -32.0f)) {
        // heal Mario
        m->healCounter = 31;
    }
    // rotate him to face away from the entrance
    m->marioObj->header.gfx.angle[1] += 0x8000;
    m->particleFlags |= PARTICLE_SPARKLES;
    return FALSE;
}

s32 act_falling_exit_airborne(struct MarioState *m) {
#ifdef BREATH_METER
    m->breath = 0x880;
#endif
    if (launch_mario_until_land(m, ACT_EXIT_LAND_SAVE_DIALOG, MARIO_ANIM_GENERAL_FALL, 0.0f)) {
        // heal Mario
        m->healCounter = 31;
    }
    // rotate Mario to face away from the entrance
    m->marioObj->header.gfx.angle[1] += 0x8000;
    m->particleFlags |= PARTICLE_SPARKLES;
    return FALSE;
}

s32 act_exit_land_save_dialog(struct MarioState *m) {
    s32 animFrame;
    stationary_ground_step(m);
    play_mario_landing_sound_once(m, SOUND_ACTION_TERRAIN_LANDING);
    switch (m->actionState) {
        case ACT_STATE_EXIT_LAND_SAVE_DIALOG_DETERMINE_TYPE:
            set_mario_animation(m, m->actionArg == 0 ? MARIO_ANIM_GENERAL_LAND
                                                     : MARIO_ANIM_LAND_FROM_SINGLE_JUMP);
            if (is_anim_past_end(m)) {
                if (gLastCompletedCourseNum != COURSE_BITDW
                    && gLastCompletedCourseNum != COURSE_BITFS) {
                    enable_time_stop();
                }

                set_menu_mode(MENU_MODE_RENDER_COURSE_COMPLETE_SCREEN);
                gSaveOptSelectIndex = MENU_OPT_NONE;

                m->actionState = ACT_STATE_EXIT_LAND_SAVE_DIALOG_WITH_CAP; // star exit with cap
                if (!(m->flags & MARIO_CAP_ON_HEAD)) {
                    m->actionState = ACT_STATE_EXIT_LAND_SAVE_DIALOG_NO_CAP; // star exit without cap
                }
                if (gLastCompletedCourseNum == COURSE_BITDW
                 || gLastCompletedCourseNum == COURSE_BITFS) {
                    m->actionState = ACT_STATE_EXIT_LAND_SAVE_DIALOG_KEY; // key exit
                }
            }
            break;
        case ACT_STATE_EXIT_LAND_SAVE_DIALOG_KEY:
            animFrame = set_mario_animation(m, MARIO_ANIM_THROW_CATCH_KEY);
            switch (animFrame) {
                case -1:
                    spawn_obj_at_mario_rel_yaw(m, MODEL_BOWSER_KEY_CUTSCENE, bhvBowserKeyCourseExit, -0x8000);
                    //! fallthrough
                case 67:
                    play_sound(SOUND_ACTION_KEY_SWISH, m->marioObj->header.gfx.cameraToObject);
                    //! fallthrough
                case 83:
                    play_sound(SOUND_ACTION_PAT_BACK, m->marioObj->header.gfx.cameraToObject);
                    //! fallthrough
                case 111:
                    play_sound(SOUND_ACTION_KEY_UNKNOWN45C, m->marioObj->header.gfx.cameraToObject);
                    // no break
            }
            handle_save_menu(m);
            break;
        case ACT_STATE_EXIT_LAND_SAVE_DIALOG_NO_CAP:
            animFrame = set_mario_animation(m, MARIO_ANIM_MISSING_CAP);
            if ((animFrame >= 18 && animFrame < 55) || (animFrame >= 112 && animFrame < 134)) {
                m->marioBodyState->handState = MARIO_HAND_OPEN;
            }
            if (!(animFrame < 109) && animFrame < 154) {
                m->marioBodyState->eyeState = MARIO_EYES_HALF_CLOSED;
            }

            handle_save_menu(m);
            break;
        case ACT_STATE_EXIT_LAND_SAVE_DIALOG_WITH_CAP:
            animFrame = set_mario_animation(m, MARIO_ANIM_TAKE_CAP_OFF_THEN_ON);
            switch (animFrame) {
                case 12:
                    cutscene_take_cap_off(m);
                    break;
                case 37:
                // fallthrough
                case 53:
                    play_sound(SOUND_ACTION_BRUSH_HAIR, m->marioObj->header.gfx.cameraToObject);
                    break;
                case 82:
                    cutscene_put_cap_on(m);
                    break;
            }
            handle_save_menu(m);
            break;
    }

    m->marioObj->header.gfx.angle[1] += 0x8000;
    return FALSE;
}

s32 act_death_exit(struct MarioState *m) {
    if (15 < m->actionTimer++
        && launch_mario_until_land(m, ACT_DEATH_EXIT_LAND, MARIO_ANIM_GENERAL_FALL, -32.0f)) {
        play_sound(SOUND_MARIO_OOOF2, m->marioObj->header.gfx.cameraToObject);
#if ENABLE_RUMBLE
        queue_rumble_data(5, 80);
#endif
#ifdef ENABLE_LIVES
        m->numLives--;
#endif
        // restore 7.75 units of health
        m->healCounter = 31;
    }
    // one unit of health
    m->health = 0x0100;
#ifdef BREATH_METER
    m->breath = 0x880;
#endif
    return FALSE;
}

s32 act_unused_death_exit(struct MarioState *m) {
    if (launch_mario_until_land(m, ACT_FREEFALL_LAND_STOP, MARIO_ANIM_GENERAL_FALL, 0.0f)) {
        play_sound(SOUND_MARIO_OOOF2, m->marioObj->header.gfx.cameraToObject);
#ifdef ENABLE_LIVES
        m->numLives--;
#endif
        // restore 7.75 units of health
        m->healCounter = 31;
    }
    // one unit of health
    m->health = 0x0100;
#ifdef BREATH_METER
    m->breath = 0x880;
#endif
    return FALSE;
}

s32 act_falling_death_exit(struct MarioState *m) {
    if (launch_mario_until_land(m, ACT_DEATH_EXIT_LAND, MARIO_ANIM_GENERAL_FALL, 0.0f)) {
        play_sound(SOUND_MARIO_OOOF2, m->marioObj->header.gfx.cameraToObject);
#if ENABLE_RUMBLE
        queue_rumble_data(5, 80);
#endif
#ifdef ENABLE_LIVES
        m->numLives--;
#endif
        // restore 7.75 units of health
        m->healCounter = 31;
    }
    // one unit of health
    m->health = 0x0100;
#ifdef BREATH_METER
    m->breath = 0x880;
#endif
    return FALSE;
}

// waits 11 frames before actually executing, also has reduced fvel
s32 act_special_exit_airborne(struct MarioState *m) {
#ifdef BREATH_METER
    m->breath = 0x880;
#endif
    struct Object *marioObj = m->marioObj;

    play_sound_if_no_flag(m, SOUND_MARIO_YAHOO, MARIO_MARIO_SOUND_PLAYED);

    if (m->actionTimer++ < 11) {
        marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
        return FALSE;
    }

    if (launch_mario_until_land(m, ACT_EXIT_LAND_SAVE_DIALOG, MARIO_ANIM_SINGLE_JUMP, -24.0f)) {
        // heal Mario
        m->healCounter = 31;
        m->actionArg = 1;
    }

    m->particleFlags |= PARTICLE_SPARKLES;
    // rotate Mario to face away from the entrance
    marioObj->header.gfx.angle[1] += 0x8000;
    // show Mario
    marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;

    return FALSE;
}

s32 act_special_death_exit(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;

    if (m->actionTimer++ < 11) {
        marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
        return FALSE;
    }

    if (launch_mario_until_land(m, ACT_HARD_BACKWARD_GROUND_KB, MARIO_ANIM_BACKWARD_AIR_KB, -24.0f)) {
#if ENABLE_RUMBLE
        queue_rumble_data(5, 80);
#endif
#ifdef ENABLE_LIVES
        m->numLives--;
#endif
        m->healCounter = 31;
    }
    // show Mario
    marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    // one unit of health
    m->health = 0x0100;
#ifdef BREATH_METER
    m->breath = 0x880;
#endif

    return FALSE;
}

s32 act_spawn_no_spin_airborne(struct MarioState *m) {
    launch_mario_until_land(m, ACT_SPAWN_NO_SPIN_LANDING, MARIO_ANIM_GENERAL_FALL, 0.0f);
    if (m->pos[1] < m->waterLevel - 100) {
        set_water_plunge_action(m);
    }
    return FALSE;
}

s32 act_spawn_no_spin_landing(struct MarioState *m) {
    play_mario_landing_sound_once(m, SOUND_ACTION_TERRAIN_LANDING);
    set_mario_animation(m, MARIO_ANIM_GENERAL_LAND);
    stop_and_set_height_to_floor(m);
    if (is_anim_at_end(m)) {
        load_level_init_text(0);
        set_mario_action(m, ACT_IDLE, 0);
    }
    return FALSE;
}

s32 act_bbh_enter_spin(struct MarioState *m) {
    f32 floorDist;
    f32 scale;
    f32 forwardVel;

    f32 cageDX = m->usedObj->oPosX - m->pos[0];
    f32 cageDZ = m->usedObj->oPosZ - m->pos[2];
    f32 cageDist = sqr(cageDX) + sqr(cageDZ);

    if (cageDist > sqr(20.0f)) {
        forwardVel = 10.0f;
    } else {
        forwardVel = sqrtf(cageDist) / 2.0f;
    }
    if (forwardVel < 0.5f) {
        forwardVel = 0.0f;
    }

    switch (m->actionState) {
        case ACT_STATE_BBH_ENTER_SPIN_SET_Y_VEL:
            floorDist = 512.0f - (m->pos[1] - m->floorHeight);
            m->vel[1] = floorDist > 0 ? sqrtf(4.0f * floorDist + 1.0f) - 1.0f : 2.0f;

            m->actionState = ACT_STATE_BBH_ENTER_SPIN_SPINNING;
            m->actionTimer = 100;
            // fallthrough

        case ACT_STATE_BBH_ENTER_SPIN_SPINNING:
            m->faceAngle[1] = atan2s(cageDZ, cageDX);
            mario_set_forward_vel(m, forwardVel);

            if (set_mario_animation(m, MARIO_ANIM_FORWARD_SPINNING) == 0) {
                play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
            }

            m->flags &= ~MARIO_JUMPING;
            perform_air_step(m, 0);
            if (m->vel[1] <= 0) {
                m->actionState = ACT_STATE_BBH_ENTER_SPIN_WAIT_FOR_ANIM;
            }
            break;

        case ACT_STATE_BBH_ENTER_SPIN_WAIT_FOR_ANIM:
            // fallthrough
        case ACT_STATE_BBH_ENTER_SPIN_DIVE:
            m->faceAngle[1] = atan2s(cageDZ, cageDX);
            mario_set_forward_vel(m, forwardVel);
            m->flags &= ~MARIO_JUMPING;
            if (perform_air_step(m, 0) == AIR_STEP_LANDED) {
                level_trigger_warp(m, WARP_OP_SPIN_SHRINK);
#if ENABLE_RUMBLE
                queue_rumble_data(15, 80);
#endif
                m->actionState = ACT_STATE_BBH_ENTER_SPIN_END;
            }
            if (m->actionState == ACT_STATE_BBH_ENTER_SPIN_WAIT_FOR_ANIM) {
                if (m->marioObj->header.gfx.animInfo.animFrame == 0) {
                    m->actionState = ACT_STATE_BBH_ENTER_SPIN_DIVE;
                }
            } else {
                play_sound_if_no_flag(m, SOUND_ACTION_SHRINK_INTO_BBH, MARIO_ACTION_SOUND_PLAYED);
                set_mario_animation(m, MARIO_ANIM_DIVE);
                m->marioObj->header.gfx.angle[0] = atan2s(m->forwardVel, -m->vel[1]);
            }
            m->squishTimer = 0xFF;
            if (m->actionTimer >= 11) {
                m->actionTimer -= 6;
                scale = m->actionTimer / 100.0f;
                vec3_same(m->marioObj->header.gfx.scale, scale);
            }
            break;

        case ACT_STATE_BBH_ENTER_SPIN_END:
            stop_and_set_height_to_floor(m);
            m->marioObj->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
            break;
    }

    return FALSE;
}

s32 act_bbh_enter_jump(struct MarioState *m) {
    play_mario_action_sound(
        m, m->flags & MARIO_METAL_CAP ? SOUND_ACTION_METAL_JUMP : SOUND_ACTION_TERRAIN_JUMP, 1);
    play_mario_jump_sound(m);

    if (m->actionState == ACT_STATE_BBH_ENTER_JUMP_INIT) {
        f32 cageDX = m->usedObj->oPosX - m->pos[0];
        f32 cageDZ = m->usedObj->oPosZ - m->pos[2];
        f32 cageDist = sqrtf(sqr(cageDX) + sqr(cageDZ));

        m->vel[1] = 60.0f;
        m->faceAngle[1] = atan2s(cageDZ, cageDX);
        mario_set_forward_vel(m, cageDist / 20.0f);

        m->flags &= ~MARIO_JUMPING;
        m->actionState = ACT_STATE_BBH_ENTER_JUMP_MOVE;
    }

    set_mario_animation(m, MARIO_ANIM_DOUBLE_JUMP_RISE);
    perform_air_step(m, AIR_STEP_CHECK_NONE);

    if (m->vel[1] <= 0.0f) {
        set_mario_action(m, ACT_BBH_ENTER_SPIN, 0);
    }

    return FALSE;
}

s32 act_teleport_fade_out(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_ACTION_TELEPORT, MARIO_ACTION_SOUND_PLAYED);
    set_mario_animation(m, m->prevAction == ACT_CROUCHING ? MARIO_ANIM_CROUCHING
                                                          : MARIO_ANIM_FIRST_PERSON);

#if ENABLE_RUMBLE
    if (m->actionTimer == 0) {
        queue_rumble_data(30, 70);
        queue_rumble_decay(2);
    }
#endif

    m->flags |= MARIO_TELEPORTING;

    if (m->actionTimer < 32) {
        m->fadeWarpOpacity = (-m->actionTimer << 3) + 0xF8;
    }

    if (m->actionTimer++ == 20) {
        level_trigger_warp(m, WARP_OP_TELEPORT);
    }

    stop_and_set_height_to_floor(m);

    return FALSE;
}

s32 act_teleport_fade_in(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_ACTION_TELEPORT, MARIO_ACTION_SOUND_PLAYED);
    set_mario_animation(m, MARIO_ANIM_FIRST_PERSON);

#if ENABLE_RUMBLE
    if (m->actionTimer == 0) {
        queue_rumble_data(30, 70);
        queue_rumble_decay(2);
    }
#endif

    if (m->actionTimer < 32) {
        m->flags |= MARIO_TELEPORTING;
        m->fadeWarpOpacity = m->actionTimer << 3;
    } else {
        m->flags &= ~MARIO_TELEPORTING;
    }

    if (m->actionTimer++ == 32) {
        if (m->pos[1] < m->waterLevel - 100) {
            // Check if the camera is not underwater.
            if (m->area->camera->mode != WATER_SURFACE_CAMERA_MODE) {
                set_camera_mode(m->area->camera, WATER_SURFACE_CAMERA_MODE, 1);
            }
            set_mario_action(m, ACT_WATER_IDLE, 0);
        } else {
            set_mario_action(m, ACT_IDLE, 0);
        }
    }

    m->pos[1] = m->floorHeight;
    stop_and_set_height_to_floor(m);

    return FALSE;
}

s32 act_shocked(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_MARIO_WAAAOOOW, MARIO_ACTION_SOUND_PLAYED);
    play_sound(SOUND_MOVING_SHOCKED, m->marioObj->header.gfx.cameraToObject);
    set_camera_shake_from_hit(SHAKE_SHOCK);

    if (set_mario_animation(m, MARIO_ANIM_SHOCKED) == 0) {
        m->actionTimer++;
        m->flags |= MARIO_METAL_SHOCK;
    }

    if (m->actionArg == 0) {
        mario_set_forward_vel(m, 0.0f);
        if (perform_air_step(m, AIR_STEP_CHECK_LEDGE_GRAB) == AIR_STEP_LANDED) {
            play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
            m->actionArg = 1;
        }
    } else {
        if (m->actionTimer >= 6) {
            m->invincTimer = 30;
            set_mario_action(m, m->health < 0x0100 ? ACT_ELECTROCUTION : ACT_IDLE, 0);
        }
        stop_and_set_height_to_floor(m);
    }

    return FALSE;
}

s32 act_squished(struct MarioState *m) {
    f32 squishAmount;
    f32 spaceUnderCeil;
    s16 surfAngle;
    s32 underSteepSurf = FALSE; // seems to be responsible for setting velocity?

    if ((spaceUnderCeil = m->ceilHeight - m->floorHeight) < 0) {
        spaceUnderCeil = 0;
    }

    switch (m->actionState) {
        case ACT_STATE_SQUISHED_SCALING:
            if (spaceUnderCeil > 160.0f) {
                m->squishTimer = 0;
                return set_mario_action(m, ACT_IDLE, 0);
            }

            m->squishTimer = 0xFF;

            if (spaceUnderCeil >= 10.1f) {
                // Mario becomes a pancake
                squishAmount = spaceUnderCeil / 160.0f;
                vec3f_set(m->marioObj->header.gfx.scale, 2.0f - squishAmount, squishAmount,
                          2.0f - squishAmount);
            } else {
                if (!(m->flags & MARIO_METAL_CAP) && m->invincTimer == 0) {
                    // cap on: 3 units; cap off: 4.5 units
                    m->hurtCounter += m->flags & MARIO_CAP_ON_HEAD ? 12 : 18;
                    play_sound_if_no_flag(m, SOUND_MARIO_ATTACKED, MARIO_MARIO_SOUND_PLAYED);
                }

                vec3f_set(m->marioObj->header.gfx.scale, 1.8f, 0.05f, 1.8f);
#if ENABLE_RUMBLE
                queue_rumble_data(10, 80);
#endif
                m->actionState = ACT_STATE_SQUISHED_CHECK_HEIGHT;
            }
            break;
        case ACT_STATE_SQUISHED_CHECK_HEIGHT:
            if (spaceUnderCeil >= 30.0f) {
                m->actionState = ACT_STATE_SQUISHED_UNSQUISH;
            }
            break;
        case ACT_STATE_SQUISHED_UNSQUISH:
            m->actionTimer++;
            if (m->actionTimer >= 15) {
                // 1 unit of health
                if (m->health < 0x100) {
                    level_trigger_warp(m, WARP_OP_DEATH);
                    // woosh, he's gone!
                    set_mario_action(m, ACT_DISAPPEARED, 0);
                } else if (m->hurtCounter == 0) {
                    // un-squish animation
                    m->squishTimer = 30;
                    set_mario_action(m, ACT_IDLE, 0);
                }
            }
            break;
    }

    // steep floor
    if (m->floor != NULL && m->floor->normal.y < 0.5f) {
        surfAngle = m->floorYaw;
        underSteepSurf = TRUE;
    }
    // steep ceiling
    if (m->ceil != NULL && -0.5f < m->ceil->normal.y) {
        surfAngle = m->ceilYaw;
        underSteepSurf = TRUE;
    }

    if (underSteepSurf) {
        m->vel[0] = sins(surfAngle) * 10.0f;
        m->vel[2] = coss(surfAngle) * 10.0f;
        m->vel[1] = 0;

        // check if there's no floor 10 units away from the surface
        if (perform_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
            // instant un-squish
            m->squishTimer = 0;
            set_mario_action(m, ACT_IDLE, 0);
            return FALSE;
        }
    }

    // squished for more than 10 seconds, so kill Mario
    if (m->actionArg++ > 300) {
        // 0 units of health
        m->health = 0x00FF;
        m->hurtCounter = 0;
        level_trigger_warp(m, WARP_OP_DEATH);
        // woosh, he's gone!
        set_mario_action(m, ACT_DISAPPEARED, 0);
    }
    stop_and_set_height_to_floor(m);
    set_mario_animation(m, MARIO_ANIM_A_POSE);
    return FALSE;
}

s32 act_putting_on_cap(struct MarioState *m) {
    s32 animFrame = set_mario_animation(m, MARIO_ANIM_PUT_CAP_ON);

    if (animFrame == 0) {
        enable_time_stop();
    }

    if (animFrame == 28) {
        cutscene_put_cap_on(m);
    }

    if (is_anim_at_end(m)) {
        set_mario_action(m, ACT_IDLE, 0);
        disable_time_stop();
    }

    stationary_ground_step(m);
    return FALSE;
}

void stuck_in_ground_handler(struct MarioState *m, s32 animation, s32 unstuckFrame, s32 target2,
                             s32 target3, s32 endAction) {
    s32 animFrame = set_mario_animation(m, animation);

    if (m->input & INPUT_A_PRESSED) {
        m->actionTimer++;
        if (m->actionTimer >= 5 && animFrame < unstuckFrame - 1) {
            animFrame = unstuckFrame - 1;
            set_anim_to_frame(m, animFrame);
        }
    }

    stop_and_set_height_to_floor(m);

    if (animFrame == -1) {
        play_sound_and_spawn_particles(m, SOUND_ACTION_TERRAIN_STUCK_IN_GROUND, 1);
    } else if (animFrame == unstuckFrame) {
#if ENABLE_RUMBLE
        queue_rumble_data(5, 80);
#endif
        play_sound_and_spawn_particles(m, SOUND_ACTION_UNSTUCK_FROM_GROUND, 1);
    } else if (animFrame == target2 || animFrame == target3) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
    }

    if (is_anim_at_end(m)) {
        set_mario_action(m, endAction, 0);
    }
}

s32 act_head_stuck_in_ground(struct MarioState *m) {
    stuck_in_ground_handler(m, MARIO_ANIM_HEAD_STUCK_IN_GROUND, 96, 105, 135, ACT_IDLE);
    return FALSE;
}

s32 act_butt_stuck_in_ground(struct MarioState *m) {
    stuck_in_ground_handler(m, MARIO_ANIM_BOTTOM_STUCK_IN_GROUND, 127, 136, -2, ACT_GROUND_POUND_LAND);
    return FALSE;
}

s32 act_feet_stuck_in_ground(struct MarioState *m) {
    stuck_in_ground_handler(m, MARIO_ANIM_LEGS_STUCK_IN_GROUND, 116, 129, -2, ACT_IDLE);
    return FALSE;
}

/**
 * advance_cutscene_step: Advances the current step in the current cutscene.
 * Resets action state and action timer, adds 1 to the action arg (responsible
 * for keeping track of what step of the cutscene Mario is in.)
 */
static void advance_cutscene_step(struct MarioState *m) {
    m->actionState = 0;
    m->actionTimer = 0;
    m->actionArg++;
}

static void intro_cutscene_hide_hud_and_mario(struct MarioState *m) {
    gHudDisplay.flags = HUD_DISPLAY_NONE;
    m->statusForCamera->cameraEvent = CAM_EVENT_START_INTRO;
    m->marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
    advance_cutscene_step(m);
}

#define TIMER_SPAWN_PIPE 37

static void intro_cutscene_peach_lakitu_scene(struct MarioState *m) {
    if ((s16) m->statusForCamera->cameraEvent != CAM_EVENT_START_INTRO) {
        if (m->actionTimer++ == TIMER_SPAWN_PIPE) {
            sIntroWarpPipeObj =
                spawn_object_abs_with_rot(gCurrentObject, 0, MODEL_CASTLE_GROUNDS_WARP_PIPE,
                                          bhvStaticObject, -1328, 60, 4664, 0, 180, 0);
            advance_cutscene_step(m);
        }
    }
}
#undef TIMER_SPAWN_PIPE

#define TIMER_RAISE_PIPE 38

static void intro_cutscene_raise_pipe(struct MarioState *m) {
    sIntroWarpPipeObj->oPosY = camera_approach_f32_symmetric(sIntroWarpPipeObj->oPosY, 260.0f, 10.0f);

    if (m->actionTimer == 0) {
        play_sound(SOUND_MENU_EXIT_PIPE, sIntroWarpPipeObj->header.gfx.cameraToObject);
    }

    if (m->actionTimer++ == TIMER_RAISE_PIPE) {
        m->vel[1] = 60.0f;
        advance_cutscene_step(m);
    }
}
#undef TIMER_RAISE_PIPE

static void intro_cutscene_jump_out_of_pipe(struct MarioState *m) {
    if (m->actionTimer == 25) {
        gHudDisplay.flags = HUD_DISPLAY_DEFAULT;
    }

    if (m->actionTimer++ >= 118) {
        m->marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;

        play_sound_if_no_flag(m, SOUND_ACTION_HIT_3, MARIO_ACTION_SOUND_PLAYED);
        play_sound_if_no_flag(m, SOUND_MARIO_YAHOO, MARIO_MARIO_SOUND_PLAYED);

        set_mario_animation(m, MARIO_ANIM_SINGLE_JUMP);
        mario_set_forward_vel(m, 10.0f);
        if (perform_air_step(m, AIR_STEP_CHECK_NONE) == AIR_STEP_LANDED) {
            sound_banks_enable(SEQ_PLAYER_SFX, SOUND_BANKS_DISABLED_DURING_INTRO_CUTSCENE);
            play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
            play_sound(SOUND_MARIO_HAHA, m->marioObj->header.gfx.cameraToObject);
            advance_cutscene_step(m);
        }
    }
}

static void intro_cutscene_land_outside_pipe(struct MarioState *m) {
    set_mario_animation(m, MARIO_ANIM_LAND_FROM_SINGLE_JUMP);

    if (is_anim_at_end(m)) {
        advance_cutscene_step(m);
    }

    stop_and_set_height_to_floor(m);
}

static void intro_cutscene_lower_pipe(struct MarioState *m) {
    if (m->actionTimer++ == 0) {
        play_sound(SOUND_MENU_ENTER_PIPE, sIntroWarpPipeObj->header.gfx.cameraToObject);
        set_mario_animation(m, MARIO_ANIM_FIRST_PERSON);
    }

    sIntroWarpPipeObj->oPosY -= 5.0f;
    if (sIntroWarpPipeObj->oPosY <= 50.0f) {
        obj_mark_for_deletion(sIntroWarpPipeObj);
        advance_cutscene_step(m);
    }

    stop_and_set_height_to_floor(m);
}

static void intro_cutscene_set_mario_to_idle(struct MarioState *m) {
    if (gCamera->cutscene == CUTSCENE_NONE) {
        gCameraMovementFlags &= ~CAM_MOVE_C_UP_MODE;
        set_mario_action(m, ACT_IDLE, 0);
    }

    stop_and_set_height_to_floor(m);
}

enum {
    INTRO_CUTSCENE_HIDE_HUD_AND_MARIO,
    INTRO_CUTSCENE_PEACH_LAKITU_SCENE,
    INTRO_CUTSCENE_RAISE_PIPE,
    INTRO_CUTSCENE_JUMP_OUT_OF_PIPE,
    INTRO_CUTSCENE_LAND_OUTSIDE_PIPE,
    INTRO_CUTSCENE_LOWER_PIPE,
    INTRO_CUTSCENE_SET_MARIO_TO_IDLE
};

static s32 act_intro_cutscene(struct MarioState *m) {
    switch (m->actionArg) {
        case INTRO_CUTSCENE_HIDE_HUD_AND_MARIO:
            intro_cutscene_hide_hud_and_mario(m);
            break;
        case INTRO_CUTSCENE_PEACH_LAKITU_SCENE:
            intro_cutscene_peach_lakitu_scene(m);
            break;
        case INTRO_CUTSCENE_RAISE_PIPE:
            intro_cutscene_raise_pipe(m);
            break;
        case INTRO_CUTSCENE_JUMP_OUT_OF_PIPE:
            intro_cutscene_jump_out_of_pipe(m);
            break;
        case INTRO_CUTSCENE_LAND_OUTSIDE_PIPE:
            intro_cutscene_land_outside_pipe(m);
            break;
        case INTRO_CUTSCENE_LOWER_PIPE:
            intro_cutscene_lower_pipe(m);
            break;
        case INTRO_CUTSCENE_SET_MARIO_TO_IDLE:
            intro_cutscene_set_mario_to_idle(m);
            break;
    }
    return FALSE;
}

// jumbo star cutscene: Mario lands after grabbing the jumbo star
static void jumbo_star_cutscene_falling(struct MarioState *m) {
    if (m->actionState == ACT_STATE_JUMBO_STAR_CUTSCENE_FALLING_FALL) {
#ifdef POWER_STARS_HEAL
        m->healCounter = 31;
#ifdef BREATH_METER
        m->breathCounter = 31;
#endif
#endif
        m->input |= INPUT_A_DOWN;
        m->flags |= (MARIO_WING_CAP | MARIO_CAP_ON_HEAD);

        m->faceAngle[1] = -0x8000;
        m->pos[0] = 0.0f;
        m->pos[2] = 0.0f;

        mario_set_forward_vel(m, 0.0f);
        set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);

        if (perform_air_step(m, AIR_STEP_CHECK_LEDGE_GRAB) == AIR_STEP_LANDED) {
            play_cutscene_music(SEQUENCE_ARGS(15, SEQ_EVENT_CUTSCENE_VICTORY));
            play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
            m->actionState = ACT_STATE_JUMBO_STAR_CUTSCENE_FALLING_LAND;
        }
    } else {
        set_mario_animation(m, MARIO_ANIM_GENERAL_LAND);
        if (is_anim_at_end(m)) {
            m->statusForCamera->cameraEvent = CAM_EVENT_START_GRAND_STAR;
            advance_cutscene_step(m);
        }
    }
}

// jumbo star cutscene: Mario takes off
static void jumbo_star_cutscene_taking_off(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;

    if (m->actionState == ACT_STATE_JUMBO_STAR_CUTSCENE_TAKING_OFF_DANCE) {
        set_mario_animation(m, MARIO_ANIM_FINAL_BOWSER_RAISE_HAND_SPIN);
        marioObj->oMarioJumboStarCutscenePosZ = 0.0f;

        if (is_anim_past_end(m)) {
            play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
            m->actionState = ACT_STATE_JUMBO_STAR_CUTSCENE_TAKING_OFF_JUMPING;
        }
    } else {
        s32 animFrame = set_mario_animation(m, MARIO_ANIM_FINAL_BOWSER_WING_CAP_TAKE_OFF);
        if (animFrame == 3 || animFrame == 28 || animFrame == 60) {
            play_sound_and_spawn_particles(m, SOUND_ACTION_TERRAIN_JUMP, 1);
        }
        if (animFrame >= 3) {
            marioObj->oMarioJumboStarCutscenePosZ -= 32.0f;
        }

        switch (animFrame) {
            case 3:
                play_sound(SOUND_MARIO_YAH_WAH_HOO + (gAudioRandom % 3 << 16),
                           marioObj->header.gfx.cameraToObject);
                break;

            case 28:
                play_sound(SOUND_MARIO_HOOHOO, marioObj->header.gfx.cameraToObject);
                break;

            case 60:
                play_sound(SOUND_MARIO_YAHOO, marioObj->header.gfx.cameraToObject);
                break;
        }
        m->particleFlags |= PARTICLE_SPARKLES;

        if (is_anim_past_end(m)) {
            advance_cutscene_step(m);
        }
    }

    vec3f_set(m->pos, 0.0f, 307.0, marioObj->oMarioJumboStarCutscenePosZ);
    update_mario_pos_for_anim(m);
    vec3f_copy(marioObj->header.gfx.pos, m->pos);
    vec3s_set(marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
}

// jumbo star cutscene: Mario flying
static void jumbo_star_cutscene_flying(struct MarioState *m) {
    Vec3f targetPos, targetD;
    f32 targetHyp;
    s16 targetAngle;

    switch (m->actionState) {
        case ACT_STATE_JUMBO_STAR_CUTSCENE_FLYING_INIT:
            set_mario_animation(m, MARIO_ANIM_WING_CAP_FLY);
            anim_spline_init(sJumboStarKeyframes);
            m->actionState = ACT_STATE_JUMBO_STAR_CUTSCENE_FLYING_ANGLES;
            // fallthrough
        case ACT_STATE_JUMBO_STAR_CUTSCENE_FLYING_ANGLES:
            if (anim_spline_poll(targetPos)) {
                // does this twice
                set_mario_action(m, ACT_FREEFALL, 0);
                m->actionState = ACT_STATE_JUMBO_STAR_CUTSCENE_FLYING_FALL;
            } else {
                vec3f_diff(targetD, targetPos, m->pos);
                targetHyp = sqrtf(sqr(targetD[0]) + sqr(targetD[2]));
                targetAngle = atan2s(targetD[2], targetD[0]);

                vec3f_copy(m->pos, targetPos);
                m->marioObj->header.gfx.angle[0] = -atan2s(targetHyp, targetD[1]);
                m->marioObj->header.gfx.angle[1] = targetAngle;
                m->marioObj->header.gfx.angle[2] = ((((m->faceAngle[1] - targetAngle) << 16) >> 16) * 20);
                m->faceAngle[1] = targetAngle;
            }
            break;
        case ACT_STATE_JUMBO_STAR_CUTSCENE_FLYING_FALL:
            set_mario_action(m, ACT_FREEFALL, 0);
            break;
    }

    m->marioBodyState->handState = MARIO_HAND_RIGHT_OPEN;
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    m->particleFlags |= PARTICLE_SPARKLES;

    if (m->actionTimer++ == 500) {
        level_trigger_warp(m, WARP_OP_CREDITS_START);
    }
}

enum { JUMBO_STAR_CUTSCENE_FALLING, JUMBO_STAR_CUTSCENE_TAKING_OFF, JUMBO_STAR_CUTSCENE_FLYING };

static s32 act_jumbo_star_cutscene(struct MarioState *m) {
    switch (m->actionArg) {
        case JUMBO_STAR_CUTSCENE_FALLING:
            jumbo_star_cutscene_falling(m);
            break;
        case JUMBO_STAR_CUTSCENE_TAKING_OFF:
            jumbo_star_cutscene_taking_off(m);
            break;
        case JUMBO_STAR_CUTSCENE_FLYING:
            jumbo_star_cutscene_flying(m);
            break;
    }
    return FALSE;
}

void generate_yellow_sparkles(s16 x, s16 y, s16 z, f32 radius) {
    static s32 sSparkleGenTheta = 0;
    static s32 sSparkleGenPhi = 0;

    s16 offsetX = radius * coss(sSparkleGenTheta) * sins(sSparkleGenPhi);
    s16 offsetY = radius * sins(sSparkleGenTheta);
    s16 offsetZ = radius * coss(sSparkleGenTheta) * coss(sSparkleGenPhi);

    spawn_object_abs_with_rot(gCurrentObject, 0, MODEL_NONE, bhvSparkleSpawn, x + offsetX, y + offsetY,
                              z + offsetZ, 0, 0, 0);

    //! copy paste error
    offsetX = offsetX * 4 / 3;
    offsetX = offsetY * 4 / 3;
    offsetX = offsetZ * 4 / 3;
    // Fix below:
    // offsetY = offsetY * 4 / 3;
    // offsetZ = offsetZ * 4 / 3;

    spawn_object_abs_with_rot(gCurrentObject, 0, MODEL_NONE, bhvSparkleSpawn, x - offsetX, y - offsetY,
                              z - offsetZ, 0, 0, 0);

    sSparkleGenTheta += 0x3800;
    sSparkleGenPhi += 0x6000;
}

// not sure what this does, returns the height of the floor.
// (animation related?)
static f32 end_obj_set_visual_pos(struct Object *obj) {
    struct Surface *surf;
    Vec3s translation;

    find_mario_anim_flags_and_translation(obj, obj->header.gfx.angle[1], translation);

    return find_floor((obj->header.gfx.pos[0] + translation[0]),
                      (obj->header.gfx.pos[1] + 10.0f),
                      (obj->header.gfx.pos[2] + translation[2]),
                      &surf);
}

// make Mario fall and soften wing cap gravity
static void end_peach_cutscene_mario_falling(struct MarioState *m) {
    if (m->actionTimer == 1) {
        m->statusForCamera->cameraEvent = CAM_EVENT_START_ENDING;
    }

    m->input |= INPUT_A_DOWN;
    m->flags |= (MARIO_WING_CAP | MARIO_CAP_ON_HEAD);

    set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
    mario_set_forward_vel(m, 0.0f);

    if (perform_air_step(m, AIR_STEP_CHECK_NONE) == AIR_STEP_LANDED) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
        advance_cutscene_step(m);
    }
}

// set Mario on the ground, wait and spawn the jumbo star outside the castle.
static void end_peach_cutscene_mario_landing(struct MarioState *m) {
    set_mario_animation(m, MARIO_ANIM_GENERAL_LAND);
    stop_and_set_height_to_floor(m);

    if (is_anim_at_end(m)) {
        // make wing cap run out
        m->capTimer = 60;

        sEndJumboStarObj = spawn_object_abs_with_rot(gCurrentObject, 0, MODEL_STAR, bhvStaticObject, 0,
                                                     2528, -1800, 0, 0, 0);
        obj_scale(sEndJumboStarObj, 3.0f);
        advance_cutscene_step(m);
    }
}

// raise hand animation, lower hand animation, do some special effects
static void end_peach_cutscene_summon_jumbo_star(struct MarioState *m) {
    set_mario_animation(m, m->actionState == 0 ? MARIO_ANIM_CREDITS_RAISE_HAND
                                               : MARIO_ANIM_CREDITS_LOWER_HAND);

    if (m->actionState == ACT_STATE_END_PEACH_CUTSCENE_SUMMON_JUMBO_STAR_RAISE_HAND && is_anim_past_end(m)) {
        m->actionState  = ACT_STATE_END_PEACH_CUTSCENE_SUMMON_JUMBO_STAR_LOWER_HAND;
    }
    if (m->actionTimer == 90) {
        play_cutscene_music(SEQUENCE_ARGS(0, SEQ_EVENT_CUTSCENE_ENDING));
    }
    if (m->actionTimer == 255) {
        advance_cutscene_step(m);
    }

    sEndJumboStarObj->oFaceAngleYaw += 0x400;
    generate_yellow_sparkles(0, 2528, -1800, 250.0f);
    play_sound(SOUND_AIR_PEACH_TWINKLE, sEndJumboStarObj->header.gfx.cameraToObject);
}

#if defined(VERSION_SH)
    #define TIMER_FADE_IN_PEACH 276
    #define TIMER_DESCEND_PEACH 400
#else
    #define TIMER_FADE_IN_PEACH 276
    #define TIMER_DESCEND_PEACH 355
#endif

// free peach from the stained glass window
static void end_peach_cutscene_spawn_peach(struct MarioState *m) {
    if (m->actionTimer == 1) {
        play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 14, 255, 255, 255);
    }
    if (m->actionTimer == 2) {
        play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
    }
    if (m->actionTimer == 44) {
        play_transition(WARP_TRANSITION_FADE_FROM_COLOR, 192, 255, 255, 255);
    }
    if (m->actionTimer == 40) {
        obj_mark_for_deletion(sEndJumboStarObj);

        sEndPeachObj = spawn_object_abs_with_rot(gCurrentObject, 0, MODEL_PEACH, bhvEndPeach, 0, 2428,
                                                 -1300, 0, 0, 0);
        gCutsceneFocus = sEndPeachObj;

        sEndRightToadObj = spawn_object_abs_with_rot(gCurrentObject, 0, MODEL_TOAD, bhvEndToad, 200,
                                                     906, -1290, 0, 0, 0);

        sEndLeftToadObj = spawn_object_abs_with_rot(gCurrentObject, 0, MODEL_TOAD, bhvEndToad, -200,
                                                    906, -1290, 0, 0, 0);

        sEndPeachObj->oOpacity = 127;
        sEndRightToadObj->oOpacity = 255;
        sEndLeftToadObj->oOpacity = 255;

        sPeachManualBlinkTime = 4;
        sEndPeachAnimation = PEACH_ANIM_DESCEND_FROM_WINDOW;

        sEndToadAnims[END_TOAD_INDEX_WEST] = TOAD_ANIM_WEST_STANDING;
        sEndToadAnims[END_TOAD_INDEX_EAST] = TOAD_ANIM_EAST_STANDING;
    }

    if (m->actionTimer >= TIMER_FADE_IN_PEACH) {
        sEndPeachObj->oOpacity = camera_approach_f32_symmetric(sEndPeachObj->oOpacity, 255.0f, 2.0f);
    }
    if (m->actionTimer >= 40) {
        generate_yellow_sparkles(0, 2628, -1300, 150.0f);
    }

    if (m->actionTimer == TIMER_DESCEND_PEACH) {
        advance_cutscene_step(m);
    }
    // probably added sounds later and missed the previous >= 40 check
    if (m->actionTimer >= 40) {
        play_sound(SOUND_AIR_PEACH_TWINKLE, sEndPeachObj->header.gfx.cameraToObject);
    }
}

#define TIMER_RUN_TO_PEACH 584

// descend peach
static void end_peach_cutscene_descend_peach(struct MarioState *m) {
    generate_yellow_sparkles(0, sEndPeachObj->oPosY, -1300, 150.0f);

    if (sEndPeachObj->oPosY >= 1300.0f) {
        if (m->actionState < 60) {
            m->actionState += 5;
        }
    } else {
        if (m->actionState >= 27) {
            m->actionState -= 2;
        }
        set_mario_animation(m, MARIO_ANIM_CREDITS_RETURN_FROM_LOOK_UP);
    }

    if ((sEndPeachObj->oPosY -= m->actionState / 10) <= 907.0f) {
        sEndPeachObj->oPosY = 906.0f;
    }

    play_sound(SOUND_AIR_PEACH_TWINKLE, sEndPeachObj->header.gfx.cameraToObject);

    if (m->actionTimer >= TIMER_RUN_TO_PEACH) {
        advance_cutscene_step(m);
    }
}

#undef TIMER_RUN_TO_PEACH

// Mario runs to peach
static void end_peach_cutscene_run_to_peach(struct MarioState *m) {
    struct Surface *surf;

    if (m->actionTimer == 22) {
        sEndPeachAnimation = PEACH_ANIM_LOOK_UP_AND_OPEN_EYES;
    }

    if ((m->pos[2] -= 20.0f) <= -1181.0f) {
        m->pos[2] = -1180.0f;
        advance_cutscene_step(m);
    }

    m->pos[1] = find_floor(m->pos[0], m->pos[1], m->pos[2], &surf);

    set_mario_anim_with_accel(m, MARIO_ANIM_RUNNING, 0x00080000);
    play_step_sound(m, 9, 45);

    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    m->particleFlags |= PARTICLE_DUST;
}

#ifdef VERSION_SH
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_1 110
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_2 111
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_3 175
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_4 258
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_5 260
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_6 305
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_7 320
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_8 510
#else
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_1  80
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_2  81
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_3 145
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_4 228
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_5 230
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_6 275
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_7 290
    #define END_PEACH_CUTSCENE_DIALOG_1_TIME_8 480
#endif

// dialog 1
// "Mario!"
// "The power of the Stars is restored to the castle..."
static void end_peach_cutscene_dialog_1(struct MarioState *m) {
    s32 animFrame = set_mario_animation(m, m->actionState == 0 ? MARIO_ANIM_CREDITS_TAKE_OFF_CAP
                                                               : MARIO_ANIM_CREDITS_LOOK_UP);

    if (m->actionState == ACT_STATE_END_PEACH_CUTSCENE_DIALOG_1_TAKE_OFF_CAP) {
        if (animFrame == 8) {
            cutscene_take_cap_off(m);
        }

        if (is_anim_at_end(m)) {
            m->actionState = ACT_STATE_END_PEACH_CUTSCENE_DIALOG_1_LOOK_UP;
        }
    }

    switch (m->actionTimer) {
        case END_PEACH_CUTSCENE_DIALOG_1_TIME_1:
            sEndPeachAnimation = PEACH_ANIM_DIALOG_1_PART_1;
            break;

        case END_PEACH_CUTSCENE_DIALOG_1_TIME_2:
            sPeachManualBlinkTime = 3;
            break;

        case END_PEACH_CUTSCENE_DIALOG_1_TIME_3:
            sPeachManualBlinkTime = 2;
            break;

        case END_PEACH_CUTSCENE_DIALOG_1_TIME_4:
            sPeachManualBlinkTime = 1;
            sPeachIsBlinking      = 1;
            break;

        case END_PEACH_CUTSCENE_DIALOG_1_TIME_5:
            set_cutscene_message(0, 30);
#ifndef VERSION_JP
            seq_player_lower_volume(SEQ_PLAYER_LEVEL, 60, 40);
            play_sound(SOUND_PEACH_MARIO, sEndPeachObj->header.gfx.cameraToObject);
#endif
            break;

        case END_PEACH_CUTSCENE_DIALOG_1_TIME_6:
            sPeachManualBlinkTime = 0;
            sPeachIsBlinking      = 0;
            break;

        case END_PEACH_CUTSCENE_DIALOG_1_TIME_7:
            set_cutscene_message(1, 60);
#ifndef VERSION_JP
            play_sound(SOUND_PEACH_POWER_OF_THE_STARS, sEndPeachObj->header.gfx.cameraToObject);
#endif
            break;

        case END_PEACH_CUTSCENE_DIALOG_1_TIME_8:
            advance_cutscene_step(m);
            break;
    }
}

#undef END_PEACH_CUTSCENE_DIALOG_1_TIME_1
#undef END_PEACH_CUTSCENE_DIALOG_1_TIME_2
#undef END_PEACH_CUTSCENE_DIALOG_1_TIME_3
#undef END_PEACH_CUTSCENE_DIALOG_1_TIME_4
#undef END_PEACH_CUTSCENE_DIALOG_1_TIME_5
#undef END_PEACH_CUTSCENE_DIALOG_1_TIME_6
#undef END_PEACH_CUTSCENE_DIALOG_1_TIME_7
#undef END_PEACH_CUTSCENE_DIALOG_1_TIME_8

#if defined(VERSION_SH)
    #define END_PEACH_CUTSCENE_DIALOG_2_TIME_1  39
    #define END_PEACH_CUTSCENE_DIALOG_2_TIME_2  65
    #define END_PEACH_CUTSCENE_DIALOG_2_TIME_3 105
    #define TIMER_SOMETHING_SPECIAL            170
    #define TIMER_PEACH_KISS                   250
#else
    #define END_PEACH_CUTSCENE_DIALOG_2_TIME_1  29
    #define END_PEACH_CUTSCENE_DIALOG_2_TIME_2  45
    #define END_PEACH_CUTSCENE_DIALOG_2_TIME_3  75
    #define TIMER_SOMETHING_SPECIAL            130
    #define TIMER_PEACH_KISS                   200
#endif

// dialog 2
// "...and it's all thanks to you!"
// "Thank you Mario!"
// "We have to do something special for you..."
static void end_peach_cutscene_dialog_2(struct MarioState *m) {
    sEndPeachAnimation = PEACH_ANIM_THANKS_TO_YOU;

    switch (m->actionTimer) {
        case END_PEACH_CUTSCENE_DIALOG_2_TIME_1:
            set_cutscene_message(2, 30);
#ifndef VERSION_JP
            play_sound(SOUND_PEACH_THANKS_TO_YOU, sEndPeachObj->header.gfx.cameraToObject);
#endif
            break;

        case END_PEACH_CUTSCENE_DIALOG_2_TIME_2:
            sPeachIsBlinking = TRUE;
            break;

        case END_PEACH_CUTSCENE_DIALOG_2_TIME_3:
            set_cutscene_message(3, 30);
#ifndef VERSION_JP
            play_sound(SOUND_PEACH_THANK_YOU_MARIO, sEndPeachObj->header.gfx.cameraToObject);
#endif
            break;

        case TIMER_SOMETHING_SPECIAL:
            set_cutscene_message(4, 40);
#ifndef VERSION_JP
            play_sound(SOUND_PEACH_SOMETHING_SPECIAL, sEndPeachObj->header.gfx.cameraToObject);
#endif
            break;

        case TIMER_PEACH_KISS:
            advance_cutscene_step(m);
            break;
    }
}

#undef END_PEACH_CUTSCENE_DIALOG_2_TIME_1
#undef END_PEACH_CUTSCENE_DIALOG_2_TIME_2
#undef END_PEACH_CUTSCENE_DIALOG_2_TIME_3
#undef TIMER_SOMETHING_SPECIAL
#undef TIMER_PEACH_KISS

// blink twice then have half-shut eyes (see end_peach_cutscene_kiss_from_peach)
static u8 sMarioBlinkOverride[20] = {
    MARIO_EYES_HALF_CLOSED, MARIO_EYES_HALF_CLOSED, MARIO_EYES_CLOSED, MARIO_EYES_CLOSED,
    MARIO_EYES_HALF_CLOSED, MARIO_EYES_HALF_CLOSED, MARIO_EYES_OPEN,   MARIO_EYES_OPEN,
    MARIO_EYES_HALF_CLOSED, MARIO_EYES_HALF_CLOSED, MARIO_EYES_CLOSED, MARIO_EYES_CLOSED,
    MARIO_EYES_HALF_CLOSED, MARIO_EYES_HALF_CLOSED, MARIO_EYES_OPEN,   MARIO_EYES_OPEN,
    MARIO_EYES_HALF_CLOSED, MARIO_EYES_HALF_CLOSED, MARIO_EYES_CLOSED, MARIO_EYES_CLOSED,
};

static void end_peach_cutscene_kiss_from_peach(struct MarioState *m) {
    sEndPeachAnimation = PEACH_ANIM_KISS;

    if (m->actionTimer >= 90) {
        m->marioBodyState->eyeState =
            m->actionTimer < 110 ? sMarioBlinkOverride[m->actionTimer - 90] : MARIO_EYES_HALF_CLOSED;
    }

    switch (m->actionTimer) {
        case 8:
            sPeachIsBlinking = FALSE;
            break;

        case 10:
            sPeachManualBlinkTime = 3;
            break;

        case 50:
            sPeachManualBlinkTime = 4;
            break;

        case 75:
            m->marioBodyState->eyeState = MARIO_EYES_HALF_CLOSED;
            break;

        case 76:
            m->marioBodyState->eyeState = MARIO_EYES_CLOSED;
            break;

        case 100:
            sPeachManualBlinkTime = 3;
            break;

        case 136:
            sPeachManualBlinkTime = 0;
            break;

        case 140:
            advance_cutscene_step(m);
            break;
    }
}

static void end_peach_cutscene_star_dance(struct MarioState *m) {
    s32 animFrame = set_mario_animation(m, MARIO_ANIM_CREDITS_PEACE_SIGN);

    if (animFrame == 77) {
        cutscene_put_cap_on(m);
    }
    if (animFrame == 88) {
        play_sound(SOUND_MARIO_HERE_WE_GO, m->marioObj->header.gfx.cameraToObject);
    }
    if (animFrame >= 98) {
        m->marioBodyState->handState = MARIO_HAND_PEACE_SIGN;
    }

    if (m->actionTimer < 52) {
        m->marioBodyState->eyeState = MARIO_EYES_HALF_CLOSED;
    }

    switch (m->actionTimer) {
        case  70: sPeachManualBlinkTime = 1; break;
        case  86: sPeachManualBlinkTime = 2; break;
        case  90: sPeachManualBlinkTime = 3; break;
        case 120: sPeachManualBlinkTime = 0; break;

        case 140:
#ifndef VERSION_JP
            seq_player_unlower_volume(SEQ_PLAYER_LEVEL, 60);
#endif
            play_cutscene_music(SEQUENCE_ARGS(15, SEQ_EVENT_CUTSCENE_CREDITS));
            break;

        case 142:
            advance_cutscene_step(m);
            break;
    }
}

// dialog 3
// "Listen everybody"
// "let's bake a delicious cake..."
// "...for Mario..."
static void end_peach_cutscene_dialog_3(struct MarioState *m) {
    set_mario_animation(m, MARIO_ANIM_FIRST_PERSON);

    sEndPeachObj->oPosY = end_obj_set_visual_pos(sEndPeachObj);
    sEndRightToadObj->oPosY = end_obj_set_visual_pos(sEndRightToadObj);
    sEndLeftToadObj->oPosY = end_obj_set_visual_pos(sEndLeftToadObj);

    switch (m->actionTimer) {
        case 1:
            sEndPeachAnimation = PEACH_ANIM_0;
            sEndToadAnims[END_TOAD_INDEX_WEST] = TOAD_ANIM_WEST_WAVE_THEN_TURN;
            sEndToadAnims[END_TOAD_INDEX_EAST] = TOAD_ANIM_EAST_NOD_THEN_TURN;
            sPeachIsBlinking = TRUE;
            set_cutscene_message(5, 30);
#ifndef VERSION_JP
            play_sound(SOUND_PEACH_BAKE_A_CAKE, sEndPeachObj->header.gfx.cameraToObject);
#endif
            break;

        case 55:
            set_cutscene_message(6, 40);
            break;

        case 130:
            set_cutscene_message(7, 50);
#ifndef VERSION_JP
            play_sound(SOUND_PEACH_FOR_MARIO, sEndPeachObj->header.gfx.cameraToObject);
#endif
            break;
    }

    if (m->actionTimer == 350) {
        advance_cutscene_step(m);
    }
}

// "Mario!"
static void end_peach_cutscene_run_to_castle(struct MarioState *m) {
    set_mario_animation(m, m->actionState == ACT_STATE_END_PEACH_CUTSCENE_RUN_TO_CASTLE_WALK_LOOK_UP ? MARIO_ANIM_CREDITS_START_WALK_LOOK_UP
                                                                                                     : MARIO_ANIM_CREDITS_LOOK_BACK_THEN_RUN);

    m->marioObj->header.gfx.pos[1] = end_obj_set_visual_pos(m->marioObj);

    if ((m->actionState == ACT_STATE_END_PEACH_CUTSCENE_RUN_TO_CASTLE_WALK_LOOK_UP) && is_anim_past_end(m)) {
        m->actionState  = ACT_STATE_END_PEACH_CUTSCENE_RUN_TO_CASTLE_LOOK_BACK_RUN;
    }

    if (m->actionTimer == 95) {
        set_cutscene_message(8, 40);
#ifndef VERSION_JP
        play_sound(SOUND_PEACH_MARIO2, sEndPeachObj->header.gfx.cameraToObject);
#endif
    }
    if (m->actionTimer == 389) {
        advance_cutscene_step(m);
    }
}

static void end_peach_cutscene_fade_out(struct MarioState *m) {
    if (m->actionState == ACT_STATE_END_PEACH_CUTSCENE_FADE_OUT_WARP) {
        level_trigger_warp(m, WARP_OP_CREDITS_NEXT);
        gPaintingMarioYEntry = 1500.0f; // ensure medium water level in WDW credits cutscene
        m->actionState = ACT_STATE_END_PEACH_CUTSCENE_FADE_OUT_END;
    }
}

enum {
    END_PEACH_CUTSCENE_MARIO_FALLING,
    END_PEACH_CUTSCENE_MARIO_LANDING,
    END_PEACH_CUTSCENE_SUMMON_JUMBO_STAR,
    END_PEACH_CUTSCENE_SPAWN_PEACH,
    END_PEACH_CUTSCENE_DESCEND_PEACH,
    END_PEACH_CUTSCENE_RUN_TO_PEACH,
    END_PEACH_CUTSCENE_DIALOG_1,
    END_PEACH_CUTSCENE_DIALOG_2,
    END_PEACH_CUTSCENE_KISS_FROM_PEACH,
    END_PEACH_CUTSCENE_STAR_DANCE,
    END_PEACH_CUTSCENE_DIALOG_3,
    END_PEACH_CUTSCENE_RUN_TO_CASTLE,
    END_PEACH_CUTSCENE_FADE_OUT
};

static s32 act_end_peach_cutscene(struct MarioState *m) {
    switch (m->actionArg) {
        case END_PEACH_CUTSCENE_MARIO_FALLING:
            end_peach_cutscene_mario_falling(m);
            break;
        case END_PEACH_CUTSCENE_MARIO_LANDING:
            end_peach_cutscene_mario_landing(m);
            break;
        case END_PEACH_CUTSCENE_SUMMON_JUMBO_STAR:
            end_peach_cutscene_summon_jumbo_star(m);
            break;
        case END_PEACH_CUTSCENE_SPAWN_PEACH:
            end_peach_cutscene_spawn_peach(m);
            break;
        case END_PEACH_CUTSCENE_DESCEND_PEACH:
            end_peach_cutscene_descend_peach(m);
            break;
        case END_PEACH_CUTSCENE_RUN_TO_PEACH:
            end_peach_cutscene_run_to_peach(m);
            break;
        case END_PEACH_CUTSCENE_DIALOG_1:
            end_peach_cutscene_dialog_1(m);
            break;
        case END_PEACH_CUTSCENE_DIALOG_2:
            end_peach_cutscene_dialog_2(m);
            break;
        case END_PEACH_CUTSCENE_KISS_FROM_PEACH:
            end_peach_cutscene_kiss_from_peach(m);
            break;
        case END_PEACH_CUTSCENE_STAR_DANCE:
            end_peach_cutscene_star_dance(m);
            break;
        case END_PEACH_CUTSCENE_DIALOG_3:
            end_peach_cutscene_dialog_3(m);
            break;
        case END_PEACH_CUTSCENE_RUN_TO_CASTLE:
            end_peach_cutscene_run_to_castle(m);
            break;
        case END_PEACH_CUTSCENE_FADE_OUT:
            end_peach_cutscene_fade_out(m);
            break;
    }

    m->actionTimer++;

    sEndCutsceneVp.vp.vscale[0] = SCREEN_WIDTH  * 2;
    sEndCutsceneVp.vp.vscale[1] = SCREEN_HEIGHT * 1.5f;
    sEndCutsceneVp.vp.vtrans[0] = SCREEN_WIDTH  * 2;
    sEndCutsceneVp.vp.vtrans[1] = SCREEN_HEIGHT * 2;
    override_viewport_and_clip(NULL, &sEndCutsceneVp, 0, 0, 0);

    return FALSE;
}

#define TIMER_CREDITS_SHOW      61
#define TIMER_CREDITS_PROGRESS  90
#define TIMER_CREDITS_WARP     204

static s32 act_credits_cutscene(struct MarioState *m) {
    m->statusForCamera->cameraEvent = CAM_EVENT_START_CREDITS;
    // checks if Mario is underwater (JRB, DDD, SA, etc.)
    if (m->pos[1] < m->waterLevel - 100) {
        if (m->area->camera->mode != CAMERA_MODE_BEHIND_MARIO) {
            set_camera_mode(m->area->camera, CAMERA_MODE_BEHIND_MARIO, 1);
        }
        set_mario_animation(m, MARIO_ANIM_WATER_IDLE);
        vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
        // will copy over roll and pitch, if set
        vec3s_copy(m->marioObj->header.gfx.angle, m->faceAngle);
        m->particleFlags |= PARTICLE_BUBBLE;
    } else {
        set_mario_animation(m, MARIO_ANIM_FIRST_PERSON);
        if (m->actionTimer > 0) {
            stop_and_set_height_to_floor(m);
        }
    }

    if (m->actionTimer >= TIMER_CREDITS_SHOW) {
        if (m->actionState < 40) {
            m->actionState += 2;
        }

        s32 width = m->actionState * (SCREEN_WIDTH * 2) / 100;
        s32 height = m->actionState * (SCREEN_HEIGHT * 2) / 100;

        sEndCutsceneVp.vp.vscale[0] = (SCREEN_WIDTH * 2) - width;
        sEndCutsceneVp.vp.vscale[1] = (SCREEN_HEIGHT * 2) - height;
        sEndCutsceneVp.vp.vtrans[0] =
            (gCurrCreditsEntry->actNum & (1 << 4) ? width : -width) * 56 / 100 + (SCREEN_WIDTH  * 2);
        sEndCutsceneVp.vp.vtrans[1] =
            (gCurrCreditsEntry->actNum & (1 << 5) ? height : -height) * 66 / 100 + (SCREEN_HEIGHT * 2);

        override_viewport_and_clip(&sEndCutsceneVp, 0, 0, 0, 0);
    }

    if (m->actionTimer == TIMER_CREDITS_PROGRESS) {
        reset_cutscene_msg_fade();
    }

    if (m->actionTimer >= TIMER_CREDITS_PROGRESS) {
        sDispCreditsEntry = gCurrCreditsEntry;
    }

    if (m->actionTimer++ == TIMER_CREDITS_WARP) {
        level_trigger_warp(m, WARP_OP_CREDITS_NEXT);
    }

    m->marioObj->header.gfx.angle[1] += (gCurrCreditsEntry->actNum & 0xC0) << 8;

    return FALSE;
}

static s32 act_end_waving_cutscene(struct MarioState *m) {
    if (m->actionState == ACT_STATE_END_WAVING_CUTSCENE_INIT) {
        m->statusForCamera->cameraEvent = CAM_EVENT_START_END_WAVING;

        sEndPeachObj = spawn_object_abs_with_rot(gCurrentObject, 0, MODEL_PEACH, bhvEndPeach, 60, 906,
                                                 -1180, 0, 0, 0);

        sEndRightToadObj = spawn_object_abs_with_rot(gCurrentObject, 0, MODEL_TOAD, bhvEndToad, 180,
                                                     906, -1170, 0, 0, 0);

        sEndLeftToadObj = spawn_object_abs_with_rot(gCurrentObject, 0, MODEL_TOAD, bhvEndToad, -180,
                                                    906, -1170, 0, 0, 0);

        sEndPeachObj->oOpacity = 255;
        sEndRightToadObj->oOpacity = 255;
        sEndLeftToadObj->oOpacity = 255;

        sEndPeachAnimation = PEACH_ANIM_WAVING;
        sEndToadAnims[END_TOAD_INDEX_WEST] = TOAD_ANIM_WEST_WAVING_BOTH_ARMS;
        sEndToadAnims[END_TOAD_INDEX_EAST] = TOAD_ANIM_EAST_WAVING_ONE_ARM;

        m->actionState = ACT_STATE_END_WAVING_CUTSCENE_WAVING;
    }

    set_mario_animation(m, MARIO_ANIM_CREDITS_WAVING);
    stop_and_set_height_to_floor(m);

    m->marioObj->header.gfx.angle[1] += 0x8000;
    m->marioObj->header.gfx.pos[0] -= 60.0f;
    m->marioBodyState->handState = MARIO_HAND_RIGHT_OPEN;

    if (m->actionTimer++ == 300) {
        level_trigger_warp(m, WARP_OP_CREDITS_END);
    }

    return FALSE;
}

static s32 check_for_instant_quicksand(struct MarioState *m) {
    if (m->floor->type == SURFACE_INSTANT_QUICKSAND && m->action & ACT_FLAG_INVULNERABLE
        && m->action != ACT_QUICKSAND_DEATH) {
        update_mario_sound_and_camera(m);
        return drop_and_set_mario_action(m, ACT_QUICKSAND_DEATH, 0);
    }
    return FALSE;
}

s32 mario_execute_cutscene_action(struct MarioState *m) {
    s32 cancel;

    if (check_for_instant_quicksand(m)) {
        return TRUE;
    }

    /* clang-format off */
    switch (m->action) {
        case ACT_DISAPPEARED:                cancel = act_disappeared(m);                break;
        case ACT_INTRO_CUTSCENE:             cancel = act_intro_cutscene(m);             break;
        case ACT_STAR_DANCE_EXIT:            cancel = act_star_dance(m);                 break;
        case ACT_STAR_DANCE_NO_EXIT:         cancel = act_star_dance(m);                 break;
        case ACT_STAR_DANCE_WATER:           cancel = act_star_dance_water(m);           break;
        case ACT_FALL_AFTER_STAR_GRAB:       cancel = act_fall_after_star_grab(m);       break;
        case ACT_READING_AUTOMATIC_DIALOG:   cancel = act_reading_automatic_dialog(m);   break;
        case ACT_READING_NPC_DIALOG:         cancel = act_reading_npc_dialog(m);         break;
        case ACT_DEBUG_FREE_MOVE:            cancel = act_debug_free_move(m);            break;
        case ACT_READING_SIGN:               cancel = act_reading_sign(m);               break;
        case ACT_JUMBO_STAR_CUTSCENE:        cancel = act_jumbo_star_cutscene(m);        break;
        case ACT_WAITING_FOR_DIALOG:         cancel = act_waiting_for_dialog(m);         break;
        case ACT_STANDING_DEATH:             cancel = act_standing_death(m);             break;
        case ACT_QUICKSAND_DEATH:            cancel = act_quicksand_death(m);            break;
        case ACT_ELECTROCUTION:              cancel = act_electrocution(m);              break;
        case ACT_SUFFOCATION:                cancel = act_suffocation(m);                break;
        case ACT_DEATH_ON_STOMACH:           cancel = act_death_on_stomach(m);           break;
        case ACT_DEATH_ON_BACK:              cancel = act_death_on_back(m);              break;
        case ACT_EATEN_BY_BUBBA:             cancel = act_eaten_by_bubba(m);             break;
        case ACT_END_PEACH_CUTSCENE:         cancel = act_end_peach_cutscene(m);         break;
        case ACT_CREDITS_CUTSCENE:           cancel = act_credits_cutscene(m);           break;
        case ACT_END_WAVING_CUTSCENE:        cancel = act_end_waving_cutscene(m);        break;
        case ACT_PULLING_DOOR:
        case ACT_PUSHING_DOOR:               cancel = act_going_through_door(m);         break;
        case ACT_WARP_DOOR_SPAWN:            cancel = act_warp_door_spawn(m);            break;
        case ACT_EMERGE_FROM_PIPE:           cancel = act_emerge_from_pipe(m);           break;
        case ACT_SPAWN_SPIN_AIRBORNE:        cancel = act_spawn_spin_airborne(m);        break;
        case ACT_SPAWN_SPIN_LANDING:         cancel = act_spawn_spin_landing(m);         break;
        case ACT_EXIT_AIRBORNE:              cancel = act_exit_airborne(m);              break;
        case ACT_EXIT_LAND_SAVE_DIALOG:      cancel = act_exit_land_save_dialog(m);      break;
        case ACT_DEATH_EXIT:                 cancel = act_death_exit(m);                 break;
        case ACT_UNUSED_DEATH_EXIT:          cancel = act_unused_death_exit(m);          break;
        case ACT_FALLING_DEATH_EXIT:         cancel = act_falling_death_exit(m);         break;
        case ACT_SPECIAL_EXIT_AIRBORNE:      cancel = act_special_exit_airborne(m);      break;
        case ACT_SPECIAL_DEATH_EXIT:         cancel = act_special_death_exit(m);         break;
        case ACT_FALLING_EXIT_AIRBORNE:      cancel = act_falling_exit_airborne(m);      break;
        case ACT_UNLOCKING_KEY_DOOR:         cancel = act_unlocking_key_door(m);         break;
        case ACT_UNLOCKING_STAR_DOOR:        cancel = act_unlocking_star_door(m);        break;
        case ACT_ENTERING_STAR_DOOR:         cancel = act_entering_star_door(m);         break;
        case ACT_SPAWN_NO_SPIN_AIRBORNE:     cancel = act_spawn_no_spin_airborne(m);     break;
        case ACT_SPAWN_NO_SPIN_LANDING:      cancel = act_spawn_no_spin_landing(m);      break;
        case ACT_BBH_ENTER_JUMP:             cancel = act_bbh_enter_jump(m);             break;
        case ACT_BBH_ENTER_SPIN:             cancel = act_bbh_enter_spin(m);             break;
        case ACT_TELEPORT_FADE_OUT:          cancel = act_teleport_fade_out(m);          break;
        case ACT_TELEPORT_FADE_IN:           cancel = act_teleport_fade_in(m);           break;
        case ACT_SHOCKED:                    cancel = act_shocked(m);                    break;
        case ACT_SQUISHED:                   cancel = act_squished(m);                   break;
        case ACT_HEAD_STUCK_IN_GROUND:       cancel = act_head_stuck_in_ground(m);       break;
        case ACT_BUTT_STUCK_IN_GROUND:       cancel = act_butt_stuck_in_ground(m);       break;
        case ACT_FEET_STUCK_IN_GROUND:       cancel = act_feet_stuck_in_ground(m);       break;
        case ACT_PUTTING_ON_CAP:             cancel = act_putting_on_cap(m);             break;
    }
    /* clang-format on */

    if (!cancel && (m->input & INPUT_IN_WATER)) {
        m->particleFlags |= PARTICLE_IDLE_WATER_WAVE;
    }

    return cancel;
}
