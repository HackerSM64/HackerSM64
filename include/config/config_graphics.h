#pragma once

/*********************
 * GRAPHICS SETTINGS *
 *********************/

/**
 * The size of the master display list (gDisplayListHead). 6400 is vanilla.
 */
#define GFX_POOL_SIZE 10000

/**
 * Causes the global light direction to be in world space,
 * this allows you to have a singular light source that doesn't change with the camera's rotation.
 * By modifying `globalLightDirection`, you can choose the direction that points TOWARDS the light,
 * but keep in mind that this direction should be normalized to roughly ~127 if changed.
 */
// #define WORLDSPACE_LIGHTING

/**
 * Show a watermark on the title screen that reads "Made with HackerSM64", instead of the copyright message.
 */
#define INTRO_CREDIT

/**
 * Spawns floombas on the title screen.
 */
#define INTRO_FLOOMBAS

/**
 * Enables widescreen (16:9) support.
 */
#define WIDE

/**
 * Skybox size modifier, changing this will add support for larger skybox images. NOTE: Vanilla skyboxes may break if you change this option. Be sure to rescale them accordingly.
 * Whenever you change this, make sure to run "make -C tools clean" to rebuild the skybox tool (alternatively go into skyconv.c and change the file in any way (like adding/deleting a space) to specifically rebuild that tool).
 * When increasing this, you should probably also increase the GFX pool size (the GFX_POOL_SIZE define above).
 */
#define SKYBOX_SIZE 1

/**
 * When this option is enabled, LODs will ONLY work on console.
 * When this option is disabled, LODs will work regardless of whether console or emulator is used.
 * Regardless of whether this setting is enabled or not, you can use gEmulator to wrap your own code in a console check.
 */
#define AUTO_LOD

/**
 * Enables Puppyprint, a display library for text and large images.
 * Automatically enabled when PUPPYPRINT_DEBUG is enabled.
 */
// #define PUPPYPRINT

/**
 * Disables AA (Improves console performance but causes visible seams between unconnected geometry).
 */
// #define DISABLE_AA

/**
 * Makes the coins ia8 64x64 instead of ia16 32x32. Uses new ia8 textures so that vanilla coins look better.
 */
// #define IA8_COINS

/**
 * Similar to the above, but 30 FPS (Textures by InTheBeef, cleaned up by Arceveti).
 */
// #define IA8_30FPS_COINS

/**
 * Use .rej microcode for certain objects (experimental - only should be used when F3DEX_GBI_2 is defined).
 * For advanced users only. Does not work perfectly out the box, best used when exported actor models are
 * using 64 vertex sizes, offered by Fast64 in the microcode menu.
 */
// #define OBJECTS_REJ

/**
 * Mario's silhouette when behind solid objects/surfaces.
 * Also enables new render layers, such as LAYER_ALPHA_DECAL.
 * The number is the intensity of the silhouette, from 0-255.
 * NOTE: The overlap between Mario's model parts is visible on certain HLE plugins.
 * Also, this also disables anti-aliasing on Mario.
 * NOTE: Regarding performance, silhouette seems to lose 1-2 frames on console.
 */
// #define SILHOUETTE 127

/**
 * Use 64x64 quarter shadow textures (Vanilla are 16x16).
 */
#define HD_SHADOWS

/**
 * Makes certain objects (mainly trees) transparent when the camera gets close.
 */
// #define OBJ_OPACITY_BY_CAM_DIST

/**
 * Fixes the game reading the ia8 burn smoke texture as an rgba16.
 */
#define BURN_SMOKE_FIX

/**
 * Disables the fix to Koopa's unshelled model.
 */
#define KOOPA_KEEP_PINK_SHORTS

/**
 * Uses the star object's model in the star dance cutscene.
 * This has a side effect of making the star dance star also transparent when Mario collects a transparent star.
 */
// #define STAR_DANCE_USES_STARS_MODEL

/**
 * Disables all object shadows. You'll probably only want this either as a last resort for performance or if you're making a super stylized hack.
 */
// #define DISABLE_SHADOWS

/**
 * Uses old shadow IDs for Fast64 compatibility. This is a temporary fix until Fast64 is updated to use the enum defines.
 * NOTE: When this is enabled, The 49th hardcoded rectangle shadow will act as a regular circular shadow, due to Mario's shadow ID being 99 in vanilla.
 */
#define LEGACY_SHADOW_IDS


/**
 * May break viewport widescreen hacks.
 * When this is disabled, the culling will only be skipped according to the NO_CULLING_EMULATOR_BLACKLIST.
 */
// #define CULLING_ON_EMULATOR

/**
 * Makes objects below the screen be culled. 
 * NOTE: Vanilla objects do not account for vertical culling.
 */
// #define VERTICAL_CULLING

/**
 * If the first command of an object´s geolayout is not GEO_CULLING_RADIUS, DEFAULT_CULLING_RADIUS
 * will be used instead.
 */
#define DEFAULT_CULLING_RADIUS 300

/**
 * Eases the textured screen transitions to make them look smoother. 
 * Extends the full radius for mario, bowser and the star transitions.
 */
// #define POLISHED_TRANSITIONS

/**
 * Uses frustratio of 2 instead of 1.
 * Can improve performance in some circumstances, though it can also cause large tris to warp if cut off from the camera.
 * Only use this if you can test the difference of your hack with and without this change on console.
 */
// #define USE_FRUSTRATIO2

/**
 * Disables instant input.
 * Mostly used for debugging purposes, or for future-proofing a release if concerned.
 */
//#define DISABLE_INSTANT_INPUT

/**
 * Primary graphics microcode selection
 * Define F3DEX_VERSION as one of the following to select the primary graphics microcode:
 * 1 - F3DEX   - Worse console performance for improved Z-buffer precision.
 * 2 - F3DZEX2 - (Majora's Mask version). HLE emulator compatible, good console performance.
 * 3 - F3DEX3  - Created by Sauraen. Better console performance in most situations. 
 * 4 - F3DEX3 LVP (legacy vertex pipeline) - F3DEX3 with legacy vertex pipeline.
 *                                           Lowers RSP processing time at the cost of new features.
 *                                           Read F3DEX3 README for more information.
 * All microcodes provided by HackerSM64 use the "NoN" (no near-field clipping) variants.
 * A built-in profiler is available for F3DEX3 and can be enabled inside config/config_debug.h
 * under DEBUG_F3DEX3_PROFILER.
 */

#define F3DEX_VERSION 2

/**
 * Enables usage of the line microcode. It can be loaded with gSPLoadUcode within a display list.
 * This feature is not available for use with F3DEX3.
 */

// #define ENABLE_LINE_UCODE

/**
 * Enables RSP MemSet for FB clear.
 * Might have issues.
 */

//#define F3DEX3_FB_MEMCLEAR
