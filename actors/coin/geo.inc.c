#ifdef IA8_30FPS_COINS

const GeoLayout yellow_coin_normal_switch_geo[] = {
   GEO_SWITCH_CASE(8, geo_switch_anim_state),
   GEO_OPEN_NODE(),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_0),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_22_5),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_45),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_67_5),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_90),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_67_5_r),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_45_r),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_22_5_r),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

const GeoLayout blue_coin_normal_switch_geo[] = {
   GEO_SWITCH_CASE(8, geo_switch_anim_state),
   GEO_OPEN_NODE(),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_0),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_22_5),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_45),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_67_5),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_90),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_67_5_r),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_45_r),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_22_5_r),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

#ifdef BLUE_COIN_SWITCH_PREVIEW
const GeoLayout blue_coin_transparent_switch_geo[] = {
   GEO_NODE_START(),
   GEO_OPEN_NODE(),
      GEO_ASM(0, geo_update_blue_coin_transparency),
      GEO_SWITCH_CASE(8, geo_switch_anim_state),
      GEO_OPEN_NODE(),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_0),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_22_5),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_45),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_67_5),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_90),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_67_5_r),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_45_r),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_22_5_r),
      GEO_CLOSE_NODE(),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};
#endif // BLUE_COIN_SWITCH_PREVIEW

const GeoLayout red_coin_normal_switch_geo[] = {
   GEO_SWITCH_CASE(8, geo_switch_anim_state),
   GEO_OPEN_NODE(),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_0),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_22_5),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_45),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_67_5),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_90),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_67_5_r),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_45_r),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_22_5_r),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

const GeoLayout silver_coin_normal_switch_geo[] = {
   GEO_SWITCH_CASE(8, geo_switch_anim_state),
   GEO_OPEN_NODE(),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_secret_0),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_secret_22_5),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_secret_45),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_secret_67_5),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_secret_90),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_secret_67_5_r),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_secret_45_r),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_secret_22_5_r),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

#else // IA8_30FPS_COINS

const GeoLayout yellow_coin_normal_switch_geo[] = {
   GEO_SWITCH_CASE(8, geo_switch_anim_state),
   GEO_OPEN_NODE(),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_front),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_front),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_tilt_right),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_tilt_right),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_side),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_side),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_tilt_left),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_yellow_tilt_left),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

const GeoLayout blue_coin_normal_switch_geo[] = {
   GEO_SWITCH_CASE(8, geo_switch_anim_state),
   GEO_OPEN_NODE(),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_front),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_front),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_tilt_right),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_tilt_right),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_side),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_side),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_tilt_left),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_blue_tilt_left),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

#ifdef BLUE_COIN_SWITCH_PREVIEW
const GeoLayout blue_coin_transparent_switch_geo[] = {
   GEO_NODE_START(),
   GEO_OPEN_NODE(),
      GEO_ASM(0, geo_update_blue_coin_transparency),
      GEO_SWITCH_CASE(8, geo_switch_anim_state),
      GEO_OPEN_NODE(),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_front),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_front),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_tilt_right),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_tilt_right),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_side),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_side),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_tilt_left),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, coin_seg3_dl_blue_tilt_left),
      GEO_CLOSE_NODE(),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};
#endif // BLUE_COIN_SWITCH_PREVIEW

const GeoLayout red_coin_normal_switch_geo[] = {
   GEO_SWITCH_CASE(8, geo_switch_anim_state),
   GEO_OPEN_NODE(),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_front),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_front),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_tilt_right),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_tilt_right),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_side),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_side),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_tilt_left),
      GEO_DISPLAY_LIST(LAYER_OCCLUDE_SILHOUETTE_ALPHA, coin_seg3_dl_red_tilt_left),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

#endif // IA8_30FPS_COINS

// 0x1600013C
const GeoLayout yellow_coin_geo[] = {
   GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0xB4, 50),
   GEO_OPEN_NODE(),
      GEO_BRANCH(1, yellow_coin_normal_switch_geo),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

// 0x160001A0
const GeoLayout yellow_coin_no_shadow_geo[] = {
   GEO_BRANCH(1, yellow_coin_normal_switch_geo),
   GEO_END(),
};

#ifdef BLUE_COIN_SWITCH_PREVIEW
// 0x16000200
const GeoLayout blue_coin_geo[] = {
   GEO_SWITCH_CASE(2, geo_switch_blue_coin_transparency),
   GEO_OPEN_NODE(),
      GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0xB4, 80),
      GEO_OPEN_NODE(),
         GEO_BRANCH(1, blue_coin_normal_switch_geo),
      GEO_CLOSE_NODE(),
      GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0x70, 80),
      GEO_OPEN_NODE(),
         GEO_BRANCH(1, blue_coin_transparent_switch_geo),
      GEO_CLOSE_NODE(),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

// 0x16000264
const GeoLayout blue_coin_no_shadow_geo[] = {
   GEO_SWITCH_CASE(2, geo_switch_blue_coin_transparency),
   GEO_OPEN_NODE(),
      GEO_BRANCH(1, blue_coin_normal_switch_geo),
      GEO_BRANCH(1, blue_coin_transparent_switch_geo),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

#else // BLUE_COIN_SWITCH_PREVIEW

// 0x16000200
const GeoLayout blue_coin_geo[] = {
   GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0xB4, 80),
   GEO_OPEN_NODE(),
      GEO_BRANCH(1, blue_coin_normal_switch_geo),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

// 0x16000264
const GeoLayout blue_coin_no_shadow_geo[] = {
   GEO_BRANCH(1, blue_coin_normal_switch_geo),
   GEO_END(),
};
#endif // BLUE_COIN_SWITCH_PREVIEW

// 0x160002C4
const GeoLayout red_coin_geo[] = {
   GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0xB4, 80),
   GEO_OPEN_NODE(),
      GEO_BRANCH(1, red_coin_normal_switch_geo),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

// 0x16000328
const GeoLayout red_coin_no_shadow_geo[] = {
   GEO_BRANCH(1, red_coin_normal_switch_geo),
   GEO_END(),
};

#ifdef IA8_30FPS_COINS
// 0x160002C4
const GeoLayout silver_coin_geo[] = {
   GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0xB4, 80),
   GEO_OPEN_NODE(),
      GEO_BRANCH(1, silver_coin_normal_switch_geo),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

// 0x16000328
const GeoLayout silver_coin_no_shadow_geo[] = {
   GEO_BRANCH(1, silver_coin_normal_switch_geo),
   GEO_END(),
};
#endif // IA8_30FPS_COINS
