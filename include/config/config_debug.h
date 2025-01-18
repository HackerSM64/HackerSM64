#pragma once

/******************
 * DEBUG SETTINGS *
 ******************/

/**
 * Enables most debug options, including PUPPYPRINT_DEBUG.
 */
// #define DEBUG_ALL

/**
 * Disables all debug options (except PUPPYPRINT).
 */
// #define DISABLE_ALL


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * Enables a comprehensive standalone profiler. Automatically enabled by PUPPYPRINT_DEBUG.
 * If not using PUPPYPRINT_DEBUG, press L + D-Pad Up to toggle the profiler.
 */
#define USE_PROFILER

/**
 * -- TEST LEVEL --
 * Uncomment this define and set a test level in order to boot straight into said level.
 * This allows you to quickly test the level you're working on.
 * If you want the game to boot normally, just comment out the define again.
 */
// #define TEST_LEVEL LEVEL_BOB

/**
 * Enables debug level select. Hold L while the game boots to turn it on.
 */
#define DEBUG_LEVEL_SELECT

/**
 * Enables debug free move (D-pad up to enter, A to exit).
 */
#define ENABLE_DEBUG_FREE_MOVE

/**
 * Enables a custom, enhanced performance profiler. (Enables PUPPYPRINT by default in config_safeguards).
 */
// #define PUPPYPRINT_DEBUG

/**
 * Uses cycles instead of microseconds in Puppyprint debug output.
 */
// #define PUPPYPRINT_DEBUG_CYCLES

/**
 * A vanilla style debug mode. It doesn't rely on a text engine, but it's much less powerful that PUPPYPRINT_DEBUG.
 * Press D-pad left to show the debug UI.
 */
// #define VANILLA_STYLE_CUSTOM_DEBUG

/**
 * Visual debug enables some collision visuals. Tapping Right on the D-pad will cycle between visual hitboxes, visual surfaces, both, and neither.
 * If puppyprint is enabled, then this can be cycled only while the screen is active.
 */
// #define VISUAL_DEBUG

/**
 * Opens all courses and doors. Used for debugging purposes to unlock all content.
 */
#define UNLOCK_ALL

/**
 * Same as above, but also reads all save file flags as complete.
 * This will not overwrite existing save file data unless you save over it.
 */
// #define COMPLETE_SAVE_FILE

/**
 * Removes the limit on FPS.
 */
// #define UNLOCK_FPS

/**
 * Includes vanilla debug functionality.
 */
// #define VANILLA_DEBUG

/**
 * Forces a crash when the game starts. Useful for debugging the crash screen.
 */
// #define DEBUG_FORCE_CRASH_ON_BOOT

/**
 * Intentionally crash the game whenever a runtime assertion fails (also invoked by the DEBUG define in the Makefile).
 */
#define DEBUG_ASSERTIONS

/**
 * Enable F3DEX3 profiling tool. Hold L + C Down to display and cycle through profiler views.
 * Using pages 2, 3, and 4 can disable certain F3DEX3 features. Read the F3DEX3 README for more information.
 */

// #define DEBUG_F3DEX3_PROFILER
