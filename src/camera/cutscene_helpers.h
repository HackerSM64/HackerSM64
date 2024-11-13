#pragma once

#include "config/config_game.h"
#include "game/camera.h"
#include "game/save_file.h"

/**
 * The same type as a CameraEvent, but because these are generally longer, and happen in sequential
 * order, they're are called "shots," a term taken from cinematography.
 *
 * To further tell the difference: CutsceneShots usually call multiple CameraEvents at once, but only
 * one CutsceneShot is ever called on a given frame.
 */
typedef CameraEvent CutsceneShot;

/**
 * A camera shot that is active for a number of frames.
 * Together, a sequence of shots makes up a cutscene.
 */
struct Cutscene {
    /// The function that gets called.
    CutsceneShot shot;
    /// How long the shot lasts.
    s16 duration;
};

extern u8 sCutsceneDialogResponse;
extern u8 sObjectCutscene;
extern u8 gRecentCutscene;
extern u8 sCutsceneDialogResponse;
extern s16 sCutsceneDialogID;
extern s16 gCutsceneTimer;
extern s16 sCutsceneShot;
extern s16 sCutsceneSplineSegment;
extern s32 gObjCutsceneDone;
extern u32 gCutsceneObjSpawn;
extern f32 sCutsceneSplineSegmentProgress;
extern Vec3f sPlayer2FocusOffset;
extern struct CutsceneVariable sCutsceneVars[10];
extern struct Object *gCutsceneFocus;

// Cutscene Interface
void init_cutscene_vars();
void reset_cutscene_vars();
void start_cutscene(struct Camera *c, u8 cutscene);
void play_cutscene(struct Camera *c);
void cutscene_reset_spline(void);
void init_current_credits_spline();

// Cutscene Helpers
void stop_cutscene_and_retrieve_stored_info(struct Camera *c);
void cutscene_goto_cvar_pos(struct Camera *c, f32 goalDist, s16 goalPitch, s16 rotPitch, s16 rotYaw);
void cutscene_soften_music(UNUSED struct Camera *c);
void cutscene_unsoften_music(UNUSED struct Camera *c);
void cutscene_update_camera_yaw(struct Camera *c);
s16 cutscene_common_set_dialog_state(s32 state);
void cutscene_stop_dialog(UNUSED struct Camera *c);
void cutscene_shake_explosion(UNUSED struct Camera *c);
void focus_in_front_of_mario(struct Camera *c, f32 dist, f32 speed);
void set_focus_rel_mario(struct Camera *c, f32 leftRight, f32 yOff, f32 forwBack, s16 yawOff);
void store_info_star(struct Camera *c);
void retrieve_info_star(struct Camera *c);
void store_info_cannon(struct Camera *c);
void retrieve_info_cannon(struct Camera *c);
void cutscene_mario_dialog_look_up(UNUSED struct Camera *c);
void cutscene_mario_dialog_look_front(UNUSED struct Camera *c);
void cutscene_mario_dialog_look_down(UNUSED struct Camera *c);
void set_flag_post_door(struct Camera *c);
void reset_pan_distance(UNUSED struct Camera *c);
s32 move_point_along_spline(Vec3f p, struct CutsceneSplinePoint spline[], s16 *splineSegment, f32 *progress);
void trigger_cutscene_dialog(s32 trigger);
u8 get_cutscene_from_mario_status(struct Camera *c);
void cutscene_event(CameraEvent event, struct Camera * c, s16 start, s16 end);
void star_dance_bound_yaw(struct Camera *c, s16 absYaw, s16 yawMax);
void cutscene_set_fov_shake_preset(u8 preset);
void player2_rotate_cam(struct Camera *c, s16 minPitch, s16 maxPitch, s16 minYaw, s16 maxYaw);

// Object cutscene functions
s16 cutscene_object(u8 cutscene, struct Object *obj);
s16 cutscene_object_with_dialog(u8 cutscene, struct Object *obj, s16 dialogID);
s16 cutscene_object_without_dialog(u8 cutscene, struct Object *obj);
void start_object_cutscene_without_focus(u8 cutscene);
void cutscene_spawn_obj(u32 obj, s16 frame);

// Shared cutscene functions
void cutscene_dance_move_to_mario(struct Camera *c);
void cutscene_death_stomach_start(struct Camera *c);
void water_death_move_to_mario_side(struct Camera *c);
void cutscene_double_doors_end(struct Camera *c);
void cutscene_quicksand_death(struct Camera *c);
void cutscene_exit_to_castle_grounds_end(struct Camera *c);

extern struct Cutscene sCutsceneCapSwitchPress[];
extern struct Cutscene sCutsceneCredits[];
extern struct Cutscene sCutsceneDanceCloseup[];
extern struct Cutscene sCutsceneDanceDefaultRotate[];
extern struct Cutscene sCutsceneDanceFlyAway[];
extern struct Cutscene sCutsceneDeathOnBack[];
extern struct Cutscene sCutsceneStandingDeath[];
extern struct Cutscene sCutsceneDeathStomach[];
extern struct Cutscene sCutsceneDialog[];
extern struct Cutscene sCutsceneDoorPull[];
extern struct Cutscene sCutsceneDoorPullMode[];
extern struct Cutscene sCutsceneDoorPush[];
extern struct Cutscene sCutsceneDoorPushMode[];
extern struct Cutscene sCutsceneDoorWarp[];
extern struct Cutscene sCutsceneEndWaving[];
extern struct Cutscene sCutsceneEnding[];
extern struct Cutscene sCutsceneEnterBowserArena[];
extern struct Cutscene sCutsceneEnterCannon[];
extern struct Cutscene sCutsceneEnterPainting[];
extern struct Cutscene sCutsceneEnterPool[];
extern struct Cutscene sCutsceneEnterPyramidTop[];
extern struct Cutscene sCutsceneGrandStar[];
extern struct Cutscene sCutsceneIntroPeach[];
extern struct Cutscene sCutsceneKeyDance[];
extern struct Cutscene sCutsceneDeathExit[];
extern struct Cutscene sCutsceneExitBowserDeath[];
extern struct Cutscene sCutsceneExitBowserSuccess[];
extern struct Cutscene sCutsceneExitPaintingSuccess[];
extern struct Cutscene sCutsceneExitSpecialSuccess[];
extern struct Cutscene sCutsceneExitWaterfall[];
extern struct Cutscene sCutsceneFallToCastleGrounds[];
extern struct Cutscene sCutsceneNonPaintingDeath[];
extern struct Cutscene sCutsceneUnusedExit[];
extern struct Cutscene sCutscenePrepareCannon[];
extern struct Cutscene sCutscenePyramidTopExplode[];
extern struct Cutscene sCutsceneQuicksandDeath[];
extern struct Cutscene sCutsceneReadMessage[];
extern struct Cutscene sCutsceneRedCoinStarSpawn[];
extern struct Cutscene sCutsceneSlidingDoorsOpen[];
extern struct Cutscene sCutsceneStarSpawn[];
extern struct Cutscene sCutsceneSuffocation[];
extern struct Cutscene sCutsceneUnlockKeyDoor[];
extern struct Cutscene sCutsceneUnused[];
extern struct Cutscene sCutsceneWaterDeath[];
