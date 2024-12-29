#ifndef COMMON1_H
#define COMMON1_H

#include "types.h"

// blue_fish
extern const GeoLayout fish_shadow_geo[];
extern const GeoLayout fish_geo[];
extern const Gfx blue_fish_seg3_dl_0301BEC0[];
extern const Gfx blue_fish_seg3_dl_0301BFB8[];
extern const Gfx blue_fish_seg3_dl_0301C0A8[];
extern const Gfx blue_fish_seg3_dl_0301C150[];
extern const struct Animation *const blue_fish_seg3_anims_0301C2B0[];

// bowser_key
extern const GeoLayout bowser_key_geo[];
extern const GeoLayout bowser_key_cutscene_geo[];
extern const Gfx bowser_key_dl[];
extern const struct Animation *const bowser_key_seg3_anims_list[];

// butterfly
extern const GeoLayout butterfly_geo[];
extern const Gfx butterfly_seg3_dl_03005408[];
extern const Gfx butterfly_seg3_dl_030054A0[];
extern const struct Animation *const butterfly_seg3_anims_030056B0[];

// coin
extern const GeoLayout yellow_coin_geo[];
extern const GeoLayout yellow_coin_no_shadow_geo[];
extern const GeoLayout blue_coin_geo[];
extern const GeoLayout blue_coin_no_shadow_geo[];
extern const GeoLayout red_coin_geo[];
extern const GeoLayout red_coin_no_shadow_geo[];
#ifdef IA8_30FPS_COINS
extern const GeoLayout silver_coin_geo[];
extern const GeoLayout silver_coin_no_shadow_geo[];

extern const Gfx coin_seg3_dl_0[];
extern const Gfx coin_seg3_dl_22_5[];
extern const Gfx coin_seg3_dl_45[];
extern const Gfx coin_seg3_dl_67_5[];
extern const Gfx coin_seg3_dl_90[];

extern const Gfx coin_seg3_dl_secret_draw[];

extern const Gfx coin_seg3_dl_yellow_draw_r[];
extern const Gfx coin_seg3_dl_blue_draw_r[];
extern const Gfx coin_seg3_dl_red_draw_r[];
extern const Gfx coin_seg3_dl_secret_draw_r[];

#else
extern const Gfx coin_seg3_dl_front[];
extern const Gfx coin_seg3_dl_tilt_right[];
extern const Gfx coin_seg3_dl_side[];
extern const Gfx coin_seg3_dl_tilt_left[];
#endif

extern const Gfx coin_seg3_dl_yellow_draw[];
extern const Gfx coin_seg3_dl_blue_draw[];
extern const Gfx coin_seg3_dl_red_draw[];
extern const Gfx coin_seg3_dl_end[];

// dirt
extern const GeoLayout dirt_animation_geo[];
extern const GeoLayout cartoon_star_geo[];
extern const Gfx dirt_seg3_sub_dl_dirt_particle[];
extern const Gfx dirt_seg3_dl_dirt_particle[];
extern const Gfx dirt_seg3_sub_dl_cartoon_star[];
extern const Gfx dirt_seg3_dl_cartoon_star_red[];
extern const Gfx dirt_seg3_dl_cartoon_star_green[];
extern const Gfx dirt_seg3_dl_cartoon_star_blue[];
extern const Gfx dirt_seg3_dl_cartoon_star_yellow[];
extern const Gfx dirt_seg3_dl_cartoon_star_billboard[];
extern const Gfx dirt_seg3_dl_tiny_particle_red[];
extern const Gfx dirt_seg3_dl_tiny_particle_green[];
extern const Gfx dirt_seg3_dl_tiny_particle_blue[];
extern const Gfx dirt_seg3_dl_tiny_particle_yellow[];
extern const Gfx dirt_seg3_dl_tiny_particle_billboard[];

// door
extern const GeoLayout castle_door_geo[];
extern const GeoLayout cabin_door_geo[];
extern const GeoLayout wooden_door_geo[];
extern const GeoLayout wooden_door2_geo[];
extern const GeoLayout metal_door_geo[];
extern const GeoLayout hazy_maze_door_geo[];
extern const GeoLayout haunted_door_geo[];
extern const GeoLayout castle_door_0_star_geo[];
extern const GeoLayout castle_door_1_star_geo[];
extern const GeoLayout castle_door_3_stars_geo[];
extern const GeoLayout key_door_geo[];
extern const Gfx door_seg3_sub_dl_polished_wooden_door_near[];
extern const Gfx door_seg3_sub_dl_polished_wooden_door_handle_1[];
extern const Gfx door_seg3_sub_dl_polished_wooden_door_handle_2[];
extern const Gfx door_seg3_dl_polished_wooden_door_near[];
extern const Gfx door_seg3_dl_cabin_door_near[];
extern const Gfx door_seg3_sub_dl_polished_wooden_door_far[];
extern const Gfx door_seg3_dl_polished_wooden_door_far[];
extern const Gfx door_seg3_dl_cabin_door_far[];
extern const Gfx door_seg3_vertex_star_sign_begin[];
extern const Gfx door_seg3_vertex_star_sign_end[];
extern const Gfx door_seg3_dl_zero_star_near[];
extern const Gfx door_seg3_dl_zero_star_far[];
extern const Gfx door_seg3_dl_one_star_near[];
extern const Gfx door_seg3_dl_one_star_far[];
extern const Gfx door_seg3_dl_three_stars_near[];
extern const Gfx door_seg3_dl_three_stars_far[];
extern const Gfx door_seg3_dl_key_door[];
extern const Gfx door_seg3_sub_dl_themed_handle_near[];
extern const Gfx door_seg3_sub_dl_themed_door_sides[];
extern const Gfx door_seg3_sub_dl_themed_door[];
extern const Gfx door_seg3_sub_dl_themed_door_begin[];
extern const Gfx door_seg3_dl_rough_wooden_door_near[];
extern const Gfx door_seg3_dl_rough_wooden_door2_near[];
extern const Gfx door_seg3_dl_metal_door_near[];
extern const Gfx door_seg3_dl_cave_door_near[];
extern const Gfx door_seg3_dl_haunted_door_near[];
extern const Gfx door_seg3_sub_dl_themed_door_far[];
extern const Gfx door_seg3_sub_dl_themed_handle_far[];
extern const Gfx door_seg3_sub_dl_themed_door_far_begin[];
extern const Gfx door_seg3_dl_rough_wooden_door_far[];
extern const Gfx door_seg3_dl_rough_wooden_door2_far[];
extern const Gfx door_seg3_dl_metal_door_far[];
extern const Gfx door_seg3_dl_cave_door_far[];
extern const Gfx door_seg3_dl_haunted_door_far[];
extern const struct Animation *const door_seg3_anims_030156C0[];
extern const Collision door_seg3_collision_door[];

// explosion
extern const GeoLayout explosion_geo[];
extern const Gfx explosion_seg3_dl_03004208[];
extern const Gfx explosion_seg3_dl_03004298[];
extern const Gfx explosion_seg3_dl_030042B0[];
extern const Gfx explosion_seg3_dl_030042C8[];
extern const Gfx explosion_seg3_dl_030042E0[];
extern const Gfx explosion_seg3_dl_030042F8[];
extern const Gfx explosion_seg3_dl_03004310[];
extern const Gfx explosion_seg3_dl_03004328[];

// flame
extern const GeoLayout red_flame_shadow_geo[];
extern const GeoLayout red_flame_geo[];
extern const GeoLayout blue_flame_geo[];
extern const Gfx flame_seg3_dl_0301B320[];
extern const Gfx flame_seg3_dl_0301B3B0[];
extern const Gfx flame_seg3_dl_0301B3C8[];
extern const Gfx flame_seg3_dl_0301B3E0[];
extern const Gfx flame_seg3_dl_0301B3F8[];
extern const Gfx flame_seg3_dl_0301B410[];
extern const Gfx flame_seg3_dl_0301B428[];
extern const Gfx flame_seg3_dl_0301B440[];
extern const Gfx flame_seg3_dl_0301B458[];
extern const Gfx flame_seg3_dl_0301B470[];
extern const Gfx flame_seg3_dl_0301B500[];
extern const Gfx flame_seg3_dl_0301B518[];
extern const Gfx flame_seg3_dl_0301B530[];
extern const Gfx flame_seg3_dl_0301B548[];
extern const Gfx flame_seg3_dl_0301B560[];
extern const Gfx flame_seg3_dl_0301B578[];
extern const Gfx flame_seg3_dl_0301B590[];
extern const Gfx flame_seg3_dl_0301B5A8[];

// leaves
extern const GeoLayout leaves_geo[];
extern const Gfx leaves_seg3_dl_0301CDE0[];

// mario_cap
extern const GeoLayout marios_cap_geo[];
extern const GeoLayout marios_metal_cap_geo[];
extern const GeoLayout marios_wing_cap_geo[];
extern const GeoLayout marios_winged_metal_cap_geo[];
extern const Gfx mario_cap_seg3_dl_03022B30[];
extern const Gfx mario_cap_seg3_dl_03022B68[];
extern const Gfx mario_cap_seg3_dl_03022CC8[];
extern const Gfx mario_cap_seg3_dl_03022D10[];
extern const Gfx mario_cap_seg3_dl_03022E78[];
extern const Gfx mario_cap_seg3_dl_03022EA8[];
extern const Gfx mario_cap_seg3_dl_03022ED8[];
extern const Gfx mario_cap_seg3_dl_03022F20[];
extern const Gfx mario_cap_seg3_dl_03022F48[];
extern const Gfx mario_cap_seg3_dl_03022FF8[];
extern const Gfx mario_cap_seg3_dl_030230B0[];
extern const Gfx mario_cap_seg3_dl_03023108[];
extern const Gfx mario_cap_seg3_dl_03023160[];
extern const Gfx mario_cap_seg3_dl_03023298[];

// mist
extern const GeoLayout mist_geo[];
extern const GeoLayout white_puff_geo[];
extern const Gfx mist_seg3_dl_03000880[];
extern const Gfx mist_seg3_dl_03000920[];

// mushroom_1up
extern const GeoLayout mushroom_1up_geo[];
extern const Gfx mushroom_1up_seg3_dl_0302A628[];
extern const Gfx mushroom_1up_seg3_dl_0302A660[];

// number
extern const GeoLayout number_geo[];

// pebble
extern const Gfx pebble_seg3_dl_0301CB00[];

// power_meter
extern const Texture *const power_meter_health_segments_lut[];
extern const Gfx dl_power_meter_base[];
extern const Gfx dl_power_meter_health_segments_begin[];
extern const Gfx dl_power_meter_health_segments_end[];

#ifdef BREATH_METER
// breath_meter
extern const Texture *const breath_meter_segments_lut[];
extern const Gfx dl_breath_meter_base[];
extern const Gfx dl_breath_meter_health_segments_begin[];
extern const Gfx dl_breath_meter_health_segments_end[];
#endif

// sand
extern const Gfx sand_seg3_dl_particle[];

// star
extern const GeoLayout star_geo[];
extern const Gfx star_seg3_sub_dl_body[];
extern const Gfx star_seg3_dl_body[];
extern const Gfx star_seg3_sub_dl_eyes[];
extern const Gfx star_seg3_dl_eyes[];

// transparent_star
extern const GeoLayout transparent_star_geo[];
extern const Gfx transparent_star_seg3_sub_dl_body[];
extern const Gfx transparent_star_seg3_dl_body[];

// tree
extern const GeoLayout bubbly_tree_geo[];
extern const GeoLayout spiky_tree_geo[];
extern const GeoLayout snow_tree_geo[];
extern const GeoLayout palm_tree_geo[];
extern const Gfx tree_seg3_dl_bubbly[];
extern const Gfx tree_seg3_dl_bubbly_transparent[];
extern const Gfx tree_seg3_dl_spiky[];
extern const Gfx tree_seg3_dl_spiky_transparent[];
extern const Gfx tree_seg3_dl_snowy_pine[];
extern const Gfx tree_seg3_dl_snowy_pine_transparent[];
extern const Gfx tree_seg3_dl_palm[];
extern const Gfx tree_seg3_dl_palm_transparent[];

// warp_pipe
extern const GeoLayout warp_pipe_geo[];
extern const Gfx warp_pipe_seg3_dl_03008E40[];
extern const Gfx warp_pipe_seg3_dl_03008F98[];
extern const Gfx warp_pipe_seg3_dl_03009968[];
extern const Gfx warp_pipe_seg3_dl_03009A20[];
extern const Gfx warp_pipe_seg3_dl_03009A50[];
extern const Collision warp_pipe_seg3_collision_03009AC8[];

// white_particle
extern const GeoLayout white_particle_geo[];
extern const Gfx white_particle_dl[];

// wooden_signpost
extern const GeoLayout wooden_signpost_geo[];
extern const Gfx wooden_signpost_seg3_dl_0302D9C8[];
extern const Gfx wooden_signpost_seg3_dl_0302DA48[];
extern const Gfx wooden_signpost_seg3_dl_0302DC40[];
extern const Gfx wooden_signpost_seg3_dl_0302DCD0[];
extern const Gfx wooden_signpost_seg3_dl_0302DD08[];
extern const Collision wooden_signpost_seg3_collision_0302DD80[];

#endif
