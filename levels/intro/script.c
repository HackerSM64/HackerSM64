#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/area.h"
#include "game/level_update.h"
#include "menu/title_screen.h"

#include "levels/scripts.h"
#include "levels/menu/header.h"

#include "actors/common0.h"
#include "actors/common1.h"

#include "make_const_nonconst.h"
#include "levels/intro/header.h"
#include "farcall.h"

#include "config.h"
#include "game/print.h"

#include "game/object_list_processor.h"

const LevelScript level_intro_splash_screen[] = {
#ifdef SKIP_TITLE_SCREEN
    EXIT_AND_EXECUTE_WITH_CODE(/*seg*/ SEGMENT_MENU_INTRO, _introSegmentRomStart, _introSegmentRomEnd, level_intro_mario_head_regular, _introSegmentBssStart, _introSegmentBssEnd),
#endif
    INIT_LEVEL(),
    LOAD_GODDARD(),
    LOAD_BEHAVIOR_DATA(),
    LOAD_LEVEL_DATA(intro),
#if defined(FLOOMBAS) && defined(INTRO_FLOOMBAS)
    LOAD_COMMON0(),

    // Load "Super Mario 64" logo
    ALLOC_LEVEL_POOL(),
    LOAD_MODEL_FROM_GEO(MODEL_GOOMBA, goomba_geo),
    AREA(/*index*/ 1, intro_geo_splash_screen),
        OBJECT(/*model*/ MODEL_GOOMBA, /*pos*/ -570, -480, 1500, /*angle*/ 0,  50, 0, /*behParam*/ BP(0x18, 0x00, 0x10, 0x0B), /*beh*/ bhvFloombaStartup),
        OBJECT(/*model*/ MODEL_GOOMBA, /*pos*/  570, -480, 1500, /*angle*/ 0, -50, 0, /*behParam*/ BP(0x18, 0x00, 0x90, 0x0B), /*beh*/ bhvFloombaStartup),
    END_AREA(),
    FREE_LEVEL_POOL(),

    // Start animation
    LOAD_AREA(/*area*/ 1),

    SET_MENU_MUSIC(/*seq*/ SEQ_SOUND_PLAYER),
    CALL(/*arg*/ LVL_INTRO_PLAY_ITS_A_ME_MARIO, /*func*/ lvl_intro_update),
    CALL(/*arg*/ 0, /*func*/ load_mario_area),
    
    JUMP_LINK_PUSH_ARG(75),
        UPDATE_OBJECTS(),
        SLEEP(/*frames*/ 1),
    JUMP_N_TIMES(),
    TRANSITION(/*transType*/ WARP_TRANSITION_FADE_INTO_COLOR, /*time*/ 16, /*color*/ 0x00, 0x00, 0x00),
    JUMP_LINK_PUSH_ARG(16),
        UPDATE_OBJECTS(),
        SLEEP(/*frames*/ 1),
    JUMP_N_TIMES(),
#else
    // Load "Super Mario 64" logo
    ALLOC_LEVEL_POOL(),
    AREA(/*index*/ 1, intro_geo_splash_screen),
    END_AREA(),
    FREE_LEVEL_POOL(),

    // Start animation
    LOAD_AREA(/*area*/ 1),

    SET_MENU_MUSIC(/*seq*/ SEQ_SOUND_PLAYER),
    CALL(/*arg*/ LVL_INTRO_PLAY_ITS_A_ME_MARIO, /*func*/ lvl_intro_update),
    SLEEP(/*frames*/ 75),
    TRANSITION(/*transType*/ WARP_TRANSITION_FADE_INTO_COLOR, /*time*/ 16, /*color*/ 0x00, 0x00, 0x00),
    SLEEP(/*frames*/ 16),
#endif
    UNLOAD_AREA(/*area*/ 1),
    CLEAR_LEVEL(),
    BLACKOUT(/*active*/ TRUE),
    SLEEP(/*frames*/ 2),
    EXIT_AND_EXECUTE_WITH_CODE(/*seg*/ SEGMENT_MENU_INTRO, _introSegmentRomStart, _introSegmentRomEnd, level_intro_mario_head_regular, _introSegmentBssStart, _introSegmentBssEnd),
};

const LevelScript level_intro_mario_head_regular[] = {
    INIT_LEVEL(),
    BLACKOUT(/*active*/ TRUE),
    LOAD_GODDARD(),
#ifdef KEEP_MARIO_HEAD
    LOAD_MARIO_HEAD(/*loadHeadID*/ REGULAR_FACE),
    LOAD_BEHAVIOR_DATA(),
    LOAD_TITLE_SCREEN_BG(),

    ALLOC_LEVEL_POOL(),
    AREA(/*index*/ 1, intro_geo_mario_head_regular),
    END_AREA(),
    FREE_LEVEL_POOL(),

    SLEEP(/*frames*/ 2),
    SET_MENU_MUSIC(/*seq*/ SEQ_MENU_TITLE_SCREEN),
    LOAD_AREA(/*area*/ 1),
    TRANSITION(/*transType*/ WARP_TRANSITION_FADE_FROM_STAR, /*time*/ 20, /*color*/ 0x00, 0x00, 0x00),
    SLEEP(/*frames*/ 2),
    BLACKOUT(/*active*/ FALSE),

#else
    PUSH_POOL(),
    BLACKOUT(/*active*/ FALSE),
#endif
    CALL_LOOP(/*arg*/ LVL_INTRO_REGULAR, /*func*/ lvl_intro_update),
    JUMP_IF(/*op*/ OP_EQ, /*arg*/ LEVEL_FILE_SELECT,  script_intro_file_select),
    JUMP_IF(/*op*/ OP_EQ, /*arg*/ LEVEL_LEVEL_SELECT, script_intro_level_select),
    JUMP(script_intro_main_level_entry),
};

const LevelScript level_intro_mario_head_dizzy[] = {
    INIT_LEVEL(),
    BLACKOUT(/*active*/ TRUE),
    LOAD_GODDARD(),
#ifdef KEEP_MARIO_HEAD
    LOAD_MARIO_HEAD(/*loadHeadID*/ DIZZY_FACE),
#endif
    LOAD_BEHAVIOR_DATA(),
    LOAD_TITLE_SCREEN_BG(),
    ALLOC_LEVEL_POOL(),

    AREA(/*index*/ 1, intro_geo_mario_head_dizzy),
    END_AREA(),

    FREE_LEVEL_POOL(),
    SLEEP(/*frames*/ 2),
    BLACKOUT(/*active*/ FALSE),
    LOAD_AREA(/*area*/ 1),
    SET_MENU_MUSIC(/*seq*/ SEQ_MENU_GAME_OVER),
    TRANSITION(/*transType*/ WARP_TRANSITION_FADE_FROM_STAR, /*time*/ 20, /*color*/ 0x00, 0x00, 0x00),
    SLEEP(/*frames*/ 20),
    CALL_LOOP(/*arg*/ LVL_INTRO_GAME_OVER, /*func*/ lvl_intro_update),
    JUMP_IF(/*op*/ OP_EQ, /*arg*/ LEVEL_FILE_SELECT,  script_intro_file_select),
    JUMP_IF(/*op*/ OP_EQ, /*arg*/ LEVEL_LEVEL_SELECT, script_intro_level_select),
    JUMP(script_intro_main_level_entry),
};

const LevelScript level_intro_entry_level_select[] = {
    INIT_LEVEL(),
    LOAD_BEHAVIOR_DATA(),
    LOAD_TITLE_SCREEN_BG(),
    LOAD_YAY0(/*seg*/ SEGMENT_LEVEL_DATA, _debug_level_select_yay0SegmentRomStart, _debug_level_select_yay0SegmentRomEnd),
    FIXED_LOAD(/*loadAddr*/ _goddardSegmentStart, /*romStart*/ _goddardSegmentRomStart, /*romEnd*/ _goddardSegmentRomEnd),
    ALLOC_LEVEL_POOL(),

    AREA(/*index*/ 1, intro_geo_debug_level_select),
    END_AREA(),

    FREE_LEVEL_POOL(),
    LOAD_AREA(/*area*/ 1),
    SET_MENU_MUSIC(/*seq*/ SEQ_MENU_TITLE_SCREEN),
    TRANSITION(/*transType*/ WARP_TRANSITION_FADE_FROM_COLOR, /*time*/ 16, /*color*/ 0xFF, 0xFF, 0xFF),
    SLEEP(/*frames*/ 16),
    CALL_LOOP(/*arg*/ LVL_INTRO_LEVEL_SELECT, /*func*/ lvl_intro_update),
    JUMP_IF(/*op*/ OP_EQ, /*arg*/ LEVEL_RESTART_GAME, script_intro_splash_screen),
    JUMP(script_intro_main_level_entry_stop_music),
};

// These should be static, but C doesn't allow non-sized forward declarations of static arrays

const LevelScript script_intro_file_select[] = {
    STOP_MUSIC(/*fadeOutTime*/ 0x00BE),
    TRANSITION(/*transType*/ WARP_TRANSITION_FADE_INTO_COLOR, /*time*/ 16, /*color*/ 0xFF, 0xFF, 0xFF),
    SLEEP(/*frames*/ 16),
    CLEAR_LEVEL(),
    SLEEP(/*frames*/ 2),
    SET_REG(/*value*/ START_LEVEL),
    EXIT_AND_EXECUTE(/*seg*/ SEGMENT_MENU_INTRO, _menuSegmentRomStart, _menuSegmentRomEnd, level_main_menu_entry_file_select),
};

const LevelScript script_intro_level_select[] = {
    TRANSITION(/*transType*/ WARP_TRANSITION_FADE_INTO_COLOR, /*time*/ 16, /*color*/ 0xFF, 0xFF, 0xFF),
    SLEEP(/*frames*/ 16),
    CLEAR_LEVEL(),
    SLEEP(/*frames*/ 2),
    EXIT_AND_EXECUTE_WITH_CODE(/*seg*/ SEGMENT_MENU_INTRO, _introSegmentRomStart, _introSegmentRomEnd, level_intro_entry_level_select, _introSegmentBssStart, _introSegmentBssEnd),
};

const LevelScript script_intro_main_level_entry_stop_music[] = {
    STOP_MUSIC(/*fadeOutTime*/ 0x00BE),
    JUMP(script_intro_main_level_entry),
};

const LevelScript script_intro_main_level_entry[] = {
    TRANSITION(/*transType*/ WARP_TRANSITION_FADE_INTO_COLOR, /*time*/ 16, /*color*/ 0xFF, 0xFF, 0xFF),
    SLEEP(/*frames*/ 16),
    CLEAR_LEVEL(),
    SLEEP(/*frames*/ 2),
    EXIT_AND_EXECUTE(/*seg*/ SEGMENT_GLOBAL_LEVEL_SCRIPT, _scriptsSegmentRomStart, _scriptsSegmentRomEnd, level_main_scripts_entry),
};

const LevelScript script_intro_splash_screen[] = {
    STOP_MUSIC(/*fadeOutTime*/ 0x00BE),
    TRANSITION(/*transType*/ WARP_TRANSITION_FADE_INTO_COLOR, /*time*/ 16, /*color*/ 0x00, 0x00, 0x00),
    SLEEP(/*frames*/ 16),
    CLEAR_LEVEL(),
    SLEEP(/*frames*/ 2),
    EXIT_AND_EXECUTE_WITH_CODE(/*seg*/ SEGMENT_MENU_INTRO, _introSegmentRomStart, _introSegmentRomEnd, level_intro_splash_screen, _introSegmentBssStart, _introSegmentBssEnd),
};
