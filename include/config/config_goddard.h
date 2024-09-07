#pragma once

/********************
 * GODDARD SETTINGS *
 ********************/

/**
 * Enables Goddard (the Mario head).
 */
#define KEEP_MARIO_HEAD

/**
 * Enables the Goddard easter egg from Shindou (has no effect if KEEP_MARIO_HEAD is disabled).
 */
#define GODDARD_EASTER_EGG

/**
 * Disables the demo that plays when idle on the start screen (has no effect if KEEP_MARIO_HEAD is disabled).
 */
// #define DISABLE_DEMO

/**
 * Boots directly to `START_LEVEL` (see config_game.h) and prints inputs to a debug console.
 * Press Start to end the recording.
 * Copy the console output to a new file in `assets/demos/`. The name of the file should be printed at the top of the output.
 * For Parallel Launcher users, `ISVPRINT` in the Makefile must be set to 1.
 * For Ares users or N64 testing with a USB-enabled flashcart, `UNF` in the Makefile must be set to 1.
 */
#define DEMO_RECORDING_MODE
