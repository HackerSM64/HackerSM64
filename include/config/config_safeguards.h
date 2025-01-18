#pragma once

/*****************************
 * COMPATIBILITY SAFE GUARDS *
 *****************************/

/**
 * NOTE: Only mess with these if you know what you're doing!
 * These are put in place to insure that connected functionality works as intended.
 */


/*****************
 * config_audio.h
 */

#ifndef MAX_SIMULTANEOUS_NOTES_EMULATOR
    #ifdef EXPAND_AUDIO_HEAP
        #define MAX_SIMULTANEOUS_NOTES_EMULATOR 40
    #else
        #define MAX_SIMULTANEOUS_NOTES_EMULATOR 20
    #endif
#endif // MAX_SIMULTANEOUS_NOTES_EMULATOR

#ifndef MAX_SIMULTANEOUS_NOTES_CONSOLE
    #ifdef EXPAND_AUDIO_HEAP
        #define MAX_SIMULTANEOUS_NOTES_CONSOLE 24
    #else
        #define MAX_SIMULTANEOUS_NOTES_CONSOLE 16
    #endif
#endif // MAX_SIMULTANEOUS_NOTES_CONSOLE

#if (MAX_SIMULTANEOUS_NOTES_EMULATOR >= MAX_SIMULTANEOUS_NOTES_CONSOLE)
    #define MAX_SIMULTANEOUS_NOTES MAX_SIMULTANEOUS_NOTES_EMULATOR
#else
    #define MAX_SIMULTANEOUS_NOTES MAX_SIMULTANEOUS_NOTES_CONSOLE
#endif

// Anything higher than 64 will most likely crash on boot. Even if it doesn't, it's still dangerous.
#if (MAX_SIMULTANEOUS_NOTES > 64)
    #undef MAX_SIMULTANEOUS_NOTES
    #define MAX_SIMULTANEOUS_NOTES 64
#elif (MAX_SIMULTANEOUS_NOTES < 0)
    #undef MAX_SIMULTANEOUS_NOTES
    #define MAX_SIMULTANEOUS_NOTES 0
#endif

#if defined(BETTER_REVERB) && !(defined(VERSION_US) || defined(VERSION_JP))
    #undef BETTER_REVERB
#endif

/*****************
 * config_graphics.h
 */

#ifdef F3DEX_VERSION
    #if F3DEX_VERSION == 1
        #define F3DEX_GBI
    #elif F3DEX_VERSION == 2
        #define F3DEX_GBI_2
    #elif F3DEX_VERSION == 3 || F3DEX_VERSION == 4
        #define F3DEX_GBI_3
    #else
        #error "Invalid microcode selection. Valid options are 1-4 inclusive."
    #endif
#else
    #error "F3DEX_VERSION is not defined in config/config_graphics.h"
#endif

#ifdef ENABLE_LINE_UCODE
    #if F3DEX_VERSION > 2
        #undef ENABLE_LINE_UCODE
        #warning "F3DEX3 does not support ENABLE_LINE_UCODE, so it will be disabled."
    #endif
#endif

/*****************
 * config_debug.h
 */

#ifdef DISABLE_ALL
    #undef DEBUG_ALL
    #undef USE_PROFILER
    #undef TEST_LEVEL
    #undef DEBUG_LEVEL_SELECT
    #undef ENABLE_DEBUG_FREE_MOVE
    #undef PUPPYPRINT_DEBUG
    #undef PUPPYPRINT_DEBUG_CYCLES
    #undef VANILLA_STYLE_CUSTOM_DEBUG
    #undef VISUAL_DEBUG
    #undef UNLOCK_ALL
    #undef COMPLETE_SAVE_FILE
    #undef UNLOCK_FPS
    #undef VANILLA_DEBUG
    #undef DEBUG_FORCE_CRASH_ON_BOOT
    #undef DEBUG_ASSERTIONS
#endif // DISABLE_ALL

#ifdef DEBUG_ALL
    #undef USE_PROFILER
    #define USE_PROFILER

    #undef DEBUG_LEVEL_SELECT
    #define DEBUG_LEVEL_SELECT

    #undef ENABLE_DEBUG_FREE_MOVE
    #define ENABLE_DEBUG_FREE_MOVE

    #undef PUPPYPRINT
    #define PUPPYPRINT

    #undef PUPPYPRINT_DEBUG
    #define PUPPYPRINT_DEBUG

    #undef VISUAL_DEBUG
    #define VISUAL_DEBUG

    #undef UNLOCK_ALL
    #define UNLOCK_ALL

    #undef COMPLETE_SAVE_FILE
    #define COMPLETE_SAVE_FILE

    #undef DEBUG_ASSERTIONS
    #define DEBUG_ASSERTIONS
#endif // DEBUG_ALL

#ifdef PUPPYPRINT_DEBUG
    #undef PUPPYPRINT
    #define PUPPYPRINT
    #undef USE_PROFILER
    #define USE_PROFILER
#endif // PUPPYPRINT_DEBUG

#ifdef COMPLETE_SAVE_FILE
    #undef UNLOCK_ALL
    #define UNLOCK_ALL
#endif // COMPLETE_SAVE_FILE

#ifdef DEBUG_F3DEX3_PROFILER
    #ifdef F3DEX_GBI_3
        #undef PUPPYPRINT
        #define PUPPYPRINT
    #else
        #undef DEBUG_F3DEX3_PROFILER
    #endif
#endif // DEBUG_F3DEX3_PROFILER

#ifdef DEBUG
    #undef DEBUG_ASSERTIONS
    #define DEBUG_ASSERTIONS
#endif // DEBUG

/*****************
 * config_camera.h
 */

#ifdef FORCED_CAMERA_MODE
    #undef USE_COURSE_DEFAULT_MODE
    #define USE_COURSE_DEFAULT_MODE // Forced camera mode overwrites the default mode.
#endif // FORCED_CAMERA_MODE

#ifndef WATER_SURFACE_CAMERA_MODE
    #define WATER_SURFACE_CAMERA_MODE CAMERA_MODE_WATER_SURFACE
#endif // !WATER_SURFACE_CAMERA_MODE

#ifndef DEEP_WATER_CAMERA_MODE
    #define DEEP_WATER_CAMERA_MODE CAMERA_MODE_BEHIND_MARIO
#endif // !DEEP_WATER_CAMERA_MODE

#ifndef FLYING_CAMERA_MODE
    #define FLYING_CAMERA_MODE CAMERA_MODE_BEHIND_MARIO
#endif // !FLYING_CAMERA_MODE


/*****************
 * config_game.h
 */

#ifndef START_LEVEL
    #define START_LEVEL LEVEL_CASTLE_GROUNDS
#endif // !START_LEVEL

#ifndef MULTILANG
    #undef ENABLE_FRENCH
    #undef ENABLE_GERMAN
    #undef ENABLE_JAPANESE
    #undef ENABLE_SPANISH
#endif // !MULTILANG

#ifdef ENABLE_JAPANESE
    #define JAPANESE_CHARACTERS
#endif // ENABLE_JAPANESE


/*****************
 * config_goddard.h
 */

#ifndef KEEP_MARIO_HEAD
    #undef GODDARD_EASTER_EGG

    #undef DISABLE_DEMO
    #define DISABLE_DEMO
#endif // !KEEP_MARIO_HEAD


/*****************
 * config_menu.h
 */

#ifdef DISABLE_EXIT_COURSE
    #undef EXIT_COURSE_WHILE_MOVING
    #undef EXIT_COURSE_LEVEL
    #undef EXIT_COURSE_AREA
    #undef EXIT_COURSE_NODE
#endif // DISABLE_EXIT_COURSE


/*****************
 * config_objects.h
 */

// Enable floombas if the intro floombas are enabled.
#ifdef INTRO_FLOOMBAS
    #undef FLOOMBAS
    #define FLOOMBAS
#endif


/*****************
 * config_rom.h
 */

#ifndef TARGET_N64
    #undef BORDER_HEIGHT_CONSOLE
    #define BORDER_HEIGHT_CONSOLE  0

    #undef BORDER_HEIGHT_EMULATOR
    #define BORDER_HEIGHT_EMULATOR 0
#endif // !TARGET_N64

#ifdef DISABLE_INSTANT_INPUT
    #undef DISABLE_INSTANT_INPUT
    #define DISABLE_INSTANT_INPUT 0xFFFFFFFF
#else
    #define DISABLE_INSTANT_INPUT 0
#endif
