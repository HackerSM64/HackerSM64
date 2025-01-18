// 0x0704A1B8 - 0x0704A1D0

// 0x0704A1D0 - 0x0704A290
#include "PR/gbi.h"
static const Vtx inside_castle_seg7_vertex_0704A1D0[] = {
    {{{  4332,   1408,   3415}, 0, {     0,      0}, {0x81, 0x00, 0x00, 0xff}}},
    {{{  4332,   2125,   2647}, 0, {     0,      0}, {0x81, 0x00, 0x00, 0xff}}},
    {{{  4332,   1408,   2647}, 0, {     0,      0}, {0x81, 0x00, 0x00, 0xff}}},
    {{{  4332,   2125,   3415}, 0, {     0,      0}, {0x81, 0x00, 0x00, 0xff}}},
    {{{  4332,   1408,   1008}, 0, {     0,      0}, {0x81, 0x00, 0x00, 0xff}}},
    {{{  4332,   2125,    240}, 0, {     0,      0}, {0x81, 0x00, 0x00, 0xff}}},
    {{{  4332,   1408,    240}, 0, {     0,      0}, {0x81, 0x00, 0x00, 0xff}}},
    {{{  4332,   2125,   1008}, 0, {     0,      0}, {0x81, 0x00, 0x00, 0xff}}},
    {{{  4332,   1408,   2493}, 0, {     0,      0}, {0x81, 0x00, 0x00, 0xff}}},
    {{{  4332,   2125,   1162}, 0, {     0,      0}, {0x81, 0x00, 0x00, 0xff}}},
    {{{  4332,   1408,   1162}, 0, {     0,      0}, {0x81, 0x00, 0x00, 0xff}}},
    {{{  4332,   2125,   2493}, 0, {     0,      0}, {0x81, 0x00, 0x00, 0xff}}},
};

// 0x0704A290 - 0x0704A2E0
static const Gfx inside_castle_seg7_dl_0704A290[] = {
    gsSPLightColor(LIGHT_1, 0xf5f5ffff),
    gsSPLightColor(LIGHT_2, 0x3d3d3fff),
    gsSPVertex(inside_castle_seg7_vertex_0704A1D0, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0704A2E0 - 0x0704A368
const Gfx inside_castle_seg7_dl_0704A2E0[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN),
#ifdef F3DEX_GBI_3
    gsSPSetGeometryMode(G_LIGHTING_SPECULAR),
#endif
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
    gsDPLoadTextureBlock(inside_castle_seg7_texture_light_radial_inverse, G_IM_FMT_IA, G_IM_SIZ_16b, 32, 32, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0x07C0, 0x07C0, 0, G_TX_RENDERTILE, G_ON),
    gsSPDisplayList(inside_castle_seg7_dl_0704A290),
    gsSPTexture(0x07C0, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
#ifdef F3DEX_GBI_3
    gsSPClearGeometryMode(G_LIGHTING_SPECULAR),
#endif
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
