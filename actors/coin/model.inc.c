#include "config.h"

// Coin

#ifdef IA8_30FPS_COINS

// YELLOW
// 0x030056C0
static const Vtx coin_seg3_vertex_yellow[] = {
    {{{   -32,      0,      0}, 0, {   -16,   4080}, {0xff, 0xff, 0x00, 0xff}}},
    {{{    32,      0,      0}, 0, {  4080,   4080}, {0xff, 0xff, 0x00, 0xff}}},
    {{{    32,     64,      0}, 0, {  4080,    -16}, {0xff, 0xff, 0x00, 0xff}}},
    {{{   -32,     64,      0}, 0, {   -16,    -16}, {0xff, 0xff, 0x00, 0xff}}},
};

// BLUE
// 0x03005700
static const Vtx coin_seg3_vertex_blue[] = {
    {{{   -48,      0,      0}, 0, {   -16,   4080}, {0x80, 0x80, 0xff, 0xff}}},
    {{{    48,      0,      0}, 0, {  4080,   4080}, {0x80, 0x80, 0xff, 0xff}}},
    {{{    48,     96,      0}, 0, {  4080,    -16}, {0x80, 0x80, 0xff, 0xff}}},
    {{{   -48,     96,      0}, 0, {   -16,    -16}, {0x80, 0x80, 0xff, 0xff}}},
};

// RED
// 0x03005740
static const Vtx coin_seg3_vertex_red[] = {
    {{{   -36,      0,      0}, 0, {   -16,   4080}, {0xff, 0x00, 0x00, 0xff}}},
    {{{    36,      0,      0}, 0, {  4080,   4080}, {0xff, 0x00, 0x00, 0xff}}},
    {{{    36,     72,      0}, 0, {  4080,    -16}, {0xff, 0x00, 0x00, 0xff}}},
    {{{   -36,     72,      0}, 0, {   -16,    -16}, {0xff, 0x00, 0x00, 0xff}}},
};

// SECRET
// 0x03005780
static const Vtx coin_seg3_vertex_secret[] = {
    {{{   -36,      0,      0}, 0, {   -16,   4080}, {0xff, 0xff, 0xff, 0xff}}},
    {{{    36,      0,      0}, 0, {  4080,   4080}, {0xff, 0xff, 0xff, 0xff}}},
    {{{    36,     72,      0}, 0, {  4080,    -16}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   -36,     72,      0}, 0, {   -16,    -16}, {0xff, 0xff, 0xff, 0xff}}},
};

// YELLOW REV
// 0x030056C0
static const Vtx coin_seg3_vertex_yellow_r[] = {
    {{{   -32,      0,      0}, 0, {  4080,   4080}, {0xff, 0xff, 0x00, 0xff}}},
    {{{    32,      0,      0}, 0, {   -16,   4080}, {0xff, 0xff, 0x00, 0xff}}},
    {{{    32,     64,      0}, 0, {   -16,    -16}, {0xff, 0xff, 0x00, 0xff}}},
    {{{   -32,     64,      0}, 0, {  4080,    -16}, {0xff, 0xff, 0x00, 0xff}}},
};

// BLUE REV
// 0x03005700
static const Vtx coin_seg3_vertex_blue_r[] = {
    {{{   -48,      0,      0}, 0, {  4080,   4080}, {0x80, 0x80, 0xff, 0xff}}},
    {{{    48,      0,      0}, 0, {   -16,   4080}, {0x80, 0x80, 0xff, 0xff}}},
    {{{    48,     96,      0}, 0, {   -16,    -16}, {0x80, 0x80, 0xff, 0xff}}},
    {{{   -48,     96,      0}, 0, {  4080,    -16}, {0x80, 0x80, 0xff, 0xff}}},
};

// RED REV
// 0x03005740
static const Vtx coin_seg3_vertex_red_r[] = {
    {{{   -36,      0,      0}, 0, {  4080,   4080}, {0xff, 0x00, 0x00, 0xff}}},
    {{{    36,      0,      0}, 0, {   -16,   4080}, {0xff, 0x00, 0x00, 0xff}}},
    {{{    36,     72,      0}, 0, {   -16,    -16}, {0xff, 0x00, 0x00, 0xff}}},
    {{{   -36,     72,      0}, 0, {  4080,    -16}, {0xff, 0x00, 0x00, 0xff}}},
};

// SECRET REV
// 0x03005780
static const Vtx coin_seg3_vertex_secret_r[] = {
    {{{   -36,      0,      0}, 0, {  4080,   4080}, {0xff, 0xff, 0xff, 0xff}}},
    {{{    36,      0,      0}, 0, {   -16,   4080}, {0xff, 0xff, 0xff, 0xff}}},
    {{{    36,     72,      0}, 0, {   -16,    -16}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   -36,     72,      0}, 0, {  4080,    -16}, {0xff, 0xff, 0xff, 0xff}}},
};

// 0x03005780
ALIGNED8 static const Texture coin_seg3_texture_0_ia8[] = {
#include "actors/coin/coin_0.ia8.inc.c"
};

// 0x03005F80
ALIGNED8 static const Texture coin_seg3_texture_22_5_ia8[] = {
#include "actors/coin/coin_22_5.ia8.inc.c"
};

// 0x03006780
ALIGNED8 static const Texture coin_seg3_texture_45_ia8[] = {
#include "actors/coin/coin_45.ia8.inc.c"
};

// 0x03006F80
ALIGNED8 static const Texture coin_seg3_texture_67_5_ia8[] = {
#include "actors/coin/coin_67_5.ia8.inc.c"
};

// 0x03007
ALIGNED8 static const Texture coin_seg3_texture_90_ia8[] = {
#include "actors/coin/coin_90.ia8.inc.c"
};

// set geo
// 0x03007780 - 0x030077D0
static const Gfx coin_seg3_dl_start[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
    gsSPTexture(32767, 32767, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0, G_TX_LOADTILE, 0, (G_TX_CLAMP | G_TX_NOMIRROR), 6, G_TX_NOLOD, (G_TX_CLAMP | G_TX_NOMIRROR), 6, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0, 0, 252, 252),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0, G_TX_RENDERTILE, 0, (G_TX_CLAMP | G_TX_NOMIRROR), 6, G_TX_NOLOD, (G_TX_CLAMP | G_TX_NOMIRROR), 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, 252, 252),
    gsSPEndDisplayList(),
};

// clear geo
// 0x030077D0 - 0x03007800
const Gfx coin_seg3_dl_end[] = {
    gsDPPipeSync(),
    gsSPTexture(0x0001, 0x0001, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// YELLOW
const Gfx coin_seg3_dl_0[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_0_ia8),
    gsSPBranchList(coin_seg3_dl_start),
};

const Gfx coin_seg3_dl_22_5[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPBranchList(coin_seg3_dl_start),
};

const Gfx coin_seg3_dl_45[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPBranchList(coin_seg3_dl_start),
};

const Gfx coin_seg3_dl_67_5[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPBranchList(coin_seg3_dl_start),
};

const Gfx coin_seg3_dl_90[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_90_ia8),
    gsSPBranchList(coin_seg3_dl_start),
};

const Gfx coin_seg3_dl_yellow_draw[] = {
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

const Gfx coin_seg3_dl_blue_draw[] = {
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

const Gfx coin_seg3_dl_red_draw[] = {
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

const Gfx coin_seg3_dl_secret_draw[] = {
    gsSPVertex(coin_seg3_vertex_secret, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

const Gfx coin_seg3_dl_yellow_draw_r[] = {
    gsSPVertex(coin_seg3_vertex_yellow_r, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

const Gfx coin_seg3_dl_blue_draw_r[] = {
    gsSPVertex(coin_seg3_vertex_blue_r, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

const Gfx coin_seg3_dl_red_draw_r[] = {
    gsSPVertex(coin_seg3_vertex_red_r, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

const Gfx coin_seg3_dl_secret_draw_r[] = {
    gsSPVertex(coin_seg3_vertex_secret_r, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

#else
#ifdef IA8_COINS
// 0x030056C0
static const Vtx coin_seg3_vertex_yellow[] = {
    {{{   -32,      0,      0}, 0, {     0,   4032}, {0xff, 0xff, 0x00, 0xff}}},
    {{{    32,      0,      0}, 0, {  4032,   4032}, {0xff, 0xff, 0x00, 0xff}}},
    {{{    32,     64,      0}, 0, {  4032,      0}, {0xff, 0xff, 0x00, 0xff}}},
    {{{   -32,     64,      0}, 0, {     0,      0}, {0xff, 0xff, 0x00, 0xff}}},
};

// 0x03005700
static const Vtx coin_seg3_vertex_blue[] = {
    {{{   -50,      0,      0}, 0, {     0,   4032}, {0x78, 0x78, 0xff, 0xff}}},
    {{{    50,      0,      0}, 0, {  4032,   4032}, {0x78, 0x78, 0xff, 0xff}}},
    {{{    50,    100,      0}, 0, {  4032,      0}, {0x78, 0x78, 0xff, 0xff}}},
    {{{   -50,    100,      0}, 0, {     0,      0}, {0x78, 0x78, 0xff, 0xff}}},
};

// 0x03005740
static const Vtx coin_seg3_vertex_red[] = {
    {{{   -35,      0,      0}, 0, {     0,   4032}, {0xff, 0x00, 0x00, 0xff}}},
    {{{    35,      0,      0}, 0, {  4032,   4032}, {0xff, 0x00, 0x00, 0xff}}},
    {{{    35,     70,      0}, 0, {  4032,      0}, {0xff, 0x00, 0x00, 0xff}}},
    {{{   -35,     70,      0}, 0, {     0,      0}, {0xff, 0x00, 0x00, 0xff}}},
};
#else
static const Vtx coin_seg3_vertex_yellow[] = {
    {{{   -32,      0,      0}, 0, {     0,   1984}, {0xff, 0xff, 0x00, 0xff}}},
    {{{    32,      0,      0}, 0, {  1984,   1984}, {0xff, 0xff, 0x00, 0xff}}},
    {{{    32,     64,      0}, 0, {  1984,      0}, {0xff, 0xff, 0x00, 0xff}}},
    {{{   -32,     64,      0}, 0, {     0,      0}, {0xff, 0xff, 0x00, 0xff}}},
};

// 0x03005700
static const Vtx coin_seg3_vertex_blue[] = {
    {{{   -50,      0,      0}, 0, {     0,   1984}, {0x78, 0x78, 0xff, 0xff}}},
    {{{    50,      0,      0}, 0, {  1984,   1984}, {0x78, 0x78, 0xff, 0xff}}},
    {{{    50,    100,      0}, 0, {  1984,      0}, {0x78, 0x78, 0xff, 0xff}}},
    {{{   -50,    100,      0}, 0, {     0,      0}, {0x78, 0x78, 0xff, 0xff}}},
};

// 0x03005740
static const Vtx coin_seg3_vertex_red[] = {
    {{{   -35,      0,      0}, 0, {     0,   1984}, {0xff, 0x00, 0x00, 0xff}}},
    {{{    35,      0,      0}, 0, {  1984,   1984}, {0xff, 0x00, 0x00, 0xff}}},
    {{{    35,     70,      0}, 0, {  1984,      0}, {0xff, 0x00, 0x00, 0xff}}},
    {{{   -35,     70,      0}, 0, {     0,      0}, {0xff, 0x00, 0x00, 0xff}}},
};
#endif

// 0x03005780
#ifdef IA8_COINS
ALIGNED8 static const Texture coin_seg3_texture_front[] = {
#include "actors/coin/coin_front.ia8.inc.c"
};

// 0x03005F80
ALIGNED8 static const Texture coin_seg3_texture_tilt_right[] = {
#include "actors/coin/coin_tilt_right.ia8.inc.c"
};

// 0x03006780
ALIGNED8 static const Texture coin_seg3_texture_side[] = {
#include "actors/coin/coin_side.ia8.inc.c"
};

// 0x03006F80
ALIGNED8 static const Texture coin_seg3_texture_tilt_left[] = {
#include "actors/coin/coin_tilt_left.ia8.inc.c"
};
#else
ALIGNED8 static const Texture coin_seg3_texture_front[] = {
#include "actors/coin/coin_front.ia16.inc.c"
};

ALIGNED8 static const Texture coin_seg3_texture_tilt_right[] = {
#include "actors/coin/coin_tilt_right.ia16.inc.c"
};

ALIGNED8 static const Texture coin_seg3_texture_side[] = {
#include "actors/coin/coin_side.ia16.inc.c"
};

ALIGNED8 static const Texture coin_seg3_texture_tilt_left[] = {
#include "actors/coin/coin_tilt_left.ia16.inc.c"
};
#endif

// 0x03007780 - 0x030077D0
static const Gfx coin_seg3_sub_dl_begin[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
#ifdef IA8_COINS
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64/2 * 64 - 1, CALC_DXT(64/2, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
#else
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
#endif
    gsSPEndDisplayList(),
};

// 0x030077D0 - 0x03007800
const Gfx coin_seg3_dl_end[] = {
    gsDPPipeSync(),
    gsSPTexture(0x0001, 0x0001, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x03007800 - 0x03007828
const Gfx coin_seg3_dl_front[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_front),
    gsSPBranchList(coin_seg3_sub_dl_begin),
};

// 0x03007828 - 0x03007850
const Gfx coin_seg3_dl_tilt_right[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_tilt_right),
    gsSPBranchList(coin_seg3_sub_dl_begin),
};

// 0x03007850 - 0x03007878
const Gfx coin_seg3_dl_side[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_side),
    gsSPBranchList(coin_seg3_sub_dl_begin),
};

// 0x03007878 - 0x030078A0
const Gfx coin_seg3_dl_tilt_left[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_tilt_left),
    gsSPBranchList(coin_seg3_sub_dl_begin),
};

const Gfx coin_seg3_dl_yellow_draw[] = {
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

const Gfx coin_seg3_dl_blue_draw[] = {
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

const Gfx coin_seg3_dl_red_draw[] = {
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

#endif
