#include <PR/ultratypes.h>
#include <PR/gbi.h>

#include "config.h"
#include "macros.h"
#include "types.h"
#include "game/ingame_menu.h"
#include "game/puppyprint.h"

#include "make_const_nonconst.h"

// SM64 (US/JP/EU/SH) Segment 02
#ifdef PUPPYPRINT
ALIGNED8 static const Texture small_font_default[] = {
#include "textures/segment2/custom_text.i4.inc.c"
};
ALIGNED8 static const Texture small_font_outline[] = {
#include "textures/segment2/custom_text2.ia4.inc.c"
};
ALIGNED8 static const Texture small_font_plain[] = {
#include "textures/segment2/custom_text3.i4.inc.c"
};
ALIGNED8 static const Texture small_font_vanilla[] = {
#include "textures/segment2/custom_text4.i4.inc.c"
};

const u8 small_font_kerning_default[] = {
    /*!*/ 4, /*"*/ 5, /*#*/ 0, /*$*/ 0, /*%*/ 8, /*&*/ 8, /*'*/ 2, /*(*/ 5, /*)*/ 5, /***/ 0, /*+*/ 8, /*,*/ 3, /*-*/ 8, /*.*/ 3, /*/*/ 8, /*0*/ 6,
    /*1*/ 5, /*2*/ 7, /*3*/ 7, /*4*/ 7, /*5*/ 7, /*6*/ 8, /*7*/ 7, /*8*/ 7, /*9*/ 6, /*:*/ 3, /*;*/ 3, /*<*/ 0, /*=*/ 0, /*>*/ 0, /*?*/ 6, /*@*/ 0,
    /*A*/ 7, /*B*/ 7, /*C*/ 7, /*D*/ 7, /*E*/ 6, /*F*/ 5, /*G*/ 8, /*H*/ 6, /*I*/ 6, /*J*/ 5, /*K*/ 7, /*L*/ 6, /*M*/ 7, /*N*/ 7, /*O*/ 7, /*P*/ 6, 
    /*Q*/ 8, /*R*/ 6, /*S*/ 7, /*T*/ 7, /*U*/ 7, /*V*/ 7, /*W*/ 8, /*X*/ 7, /*Y*/ 7, /*Z*/ 7, /*[*/ 0, /*\\*/ 0, /*]*/ 0, /*^*/ 8, /*_*/ 0, /*`*/ 0, 
    /*a*/ 7, /*b*/ 7, /*c*/ 6, /*d*/ 7, /*e*/ 7, /*f*/ 7, /*g*/ 7, /*h*/ 7, /*i*/ 3, /*j*/ 5, /*k*/ 8, /*l*/ 4, /*m*/ 7, /*n*/ 7, /*o*/ 7, /*p*/ 7, 
    /*q*/ 7, /*r*/ 6, /*s*/ 6, /*t*/ 6, /*u*/ 6, /*v*/ 7, /*w*/ 8, /*x*/ 6, /*y*/ 8, /*z*/ 7, /*{*/ 0, /*|*/ 0, /*}*/ 0, /*~*/ 8, 
};

const u16 small_font_offsets_default[] = {
    /*!*/ 0, /*"*/ 8, /*#*/ 16, /*$*/ 24, /*%*/ 32, /*&*/ 40, /*'*/ 48, /*(*/ 56, /*)*/ 64, /***/ 72, /*+*/ 80, /*,*/ 88, /*-*/ 96, /*.*/ 104, /*/*/ 112,
    /*0*/ 120, /*1*/ 128, /*2*/ 136, /*3*/ 144, /*4*/ 152, /*5*/ 160, /*6*/ 168, /*7*/ 176, /*8*/ 184, /*9*/ 192, /*:*/ 200, /*;*/ 208, /*<*/ 216, /*=*/ 216,
    /*>*/ 216, /*?*/ 216, /*@*/ 224, /*A*/ 224, /*B*/ 232, /*C*/ 240, /*D*/ 248, /*E*/ 256, /*F*/ 264, /*G*/ 272, /*H*/ 280, /*I*/ 288, /*J*/ 296, /*K*/ 304,
    /*L*/ 312, /*M*/ 320, /*N*/ 328, /*O*/ 336, /*P*/ 344, /*Q*/ 352, /*R*/ 360, /*S*/ 368, /*T*/ 376, /*U*/ 384, /*V*/ 392, /*W*/ 400, /*X*/ 408, /*Y*/ 416,
    /*Z*/ 424, /*[*/ 432, /*\*/ 432, /*]*/ 432, /*^*/ 432, /*_*/ 440, /*`*/ 440, /*a*/ 440, /*b*/ 448, /*c*/ 456, /*d*/ 464, /*e*/ 472, /*f*/ 480, /*g*/ 488, 
    /*h*/ 496, /*i*/ 504, /*j*/ 512, /*k*/ 520, /*l*/ 528, /*m*/ 536, /*n*/ 544, /*o*/ 552, /*p*/ 560, /*q*/ 568, /*r*/ 576, /*s*/ 584, /*t*/ 592, /*u*/ 600, 
    /*v*/ 608, /*w*/ 616, /*x*/ 624, /*y*/ 632, /*z*/ 640, /*{*/ 648, /*|*/ 648, /*}*/ 648, /*~*/ 648, /*:)*/ 656
};

static const u8 small_font_kerning_outline[] = {
    /*!*/ 3, /*"*/ 4, /*#*/ 0, /*$*/ 0, /*%*/ 6, /*&*/ 6, /*'*/ 2, /*(*/ 4, /*)*/ 4, /***/ 0, /*+*/ 6, /*,*/ 2, /*-*/ 6, /*.*/ 2, /*/*/ 6, /*0*/ 6,
    /*1*/ 6, /*2*/ 6, /*3*/ 6, /*4*/ 6, /*5*/ 6, /*6*/ 6, /*7*/ 6, /*8*/ 6, /*9*/ 6, /*:*/ 2, /*;*/ 2, /*<*/ 0, /*=*/ 0, /*>*/ 0, /*?*/ 6, /*@*/ 0, 
    /*A*/ 6, /*B*/ 6, /*C*/ 6, /*D*/ 6, /*E*/ 6, /*F*/ 6, /*G*/ 6, /*H*/ 6, /*I*/ 5, /*J*/ 7, /*K*/ 6, /*L*/ 6, /*M*/ 6, /*N*/ 6, /*O*/ 6, /*P*/ 6, 
    /*Q*/ 6, /*R*/ 6, /*S*/ 6, /*T*/ 6, /*U*/ 6, /*V*/ 6, /*W*/ 6, /*X*/ 6, /*Y*/ 6, /*Z*/ 6, /*[*/ 0, /*\\*/ 0, /*]*/ 0, /*^*/ 7, /*_*/ 0, /*`*/ 0,  
    /*a*/ 5, /*b*/ 5, /*c*/ 5, /*d*/ 5, /*e*/ 5, /*f*/ 5, /*g*/ 5, /*h*/ 5, /*i*/ 2, /*j*/ 6, /*k*/ 4, /*l*/ 2, /*m*/ 5, /*n*/ 5, /*o*/ 5, /*p*/ 5, 
    /*q*/ 5, /*r*/ 5, /*s*/ 5, /*t*/ 5, /*u*/ 5, /*v*/ 5, /*w*/ 5, /*x*/ 5, /*y*/ 5, /*z*/ 5, /*{*/ 0, /*|*/ 0, /*}*/ 0, /*~*/ 6,   
};

const u16 small_font_offsets_outline[] = {
    /*!*/ 0, /*"*/ 8, /*#*/ 16, /*$*/ 24, /*%*/ 32, /*&*/ 40, /*'*/ 48, /*(*/ 56, /*)*/ 64, /***/ 72, /*+*/ 80, /*,*/ 88, /*-*/ 96, /*.*/ 104, /*/*/ 112,
    /*0*/ 120, /*1*/ 128, /*2*/ 136, /*3*/ 144, /*4*/ 152, /*5*/ 160, /*6*/ 168, /*7*/ 176, /*8*/ 184, /*9*/ 192, /*:*/ 200, /*;*/ 208, /*<*/ 216, /*=*/ 216,
    /*>*/ 216, /*?*/ 216, /*@*/ 224, /*A*/ 224, /*B*/ 232, /*C*/ 240, /*D*/ 248, /*E*/ 256, /*F*/ 264, /*G*/ 272, /*H*/ 280, /*I*/ 288, /*J*/ 296, /*K*/ 304,
    /*L*/ 312, /*M*/ 320, /*N*/ 328, /*O*/ 336, /*P*/ 344, /*Q*/ 352, /*R*/ 360, /*S*/ 368, /*T*/ 376, /*U*/ 384, /*V*/ 392, /*W*/ 400, /*X*/ 408, /*Y*/ 416,
    /*Z*/ 424, /*[*/ 432, /*\*/ 432, /*]*/ 432, /*^*/ 432, /*_*/ 440, /*`*/ 440, /*a*/ 440, /*b*/ 448, /*c*/ 456, /*d*/ 464, /*e*/ 472, /*f*/ 480, /*g*/ 488, 
    /*h*/ 496, /*i*/ 504, /*j*/ 512, /*k*/ 520, /*l*/ 528, /*m*/ 536, /*n*/ 544, /*o*/ 552, /*p*/ 560, /*q*/ 568, /*r*/ 576, /*s*/ 584, /*t*/ 592, /*u*/ 600, 
    /*v*/ 608, /*w*/ 616, /*x*/ 624, /*y*/ 632, /*z*/ 640, /*{*/ 648, /*|*/ 648, /*}*/ 648, /*~*/ 648, /*:)*/ 656
};

static const u8 small_font_kerning_plain[] = {
    /*!*/ 5, /*"*/ 4, /*#*/ 0, /*$*/ 0, /*%*/ 6, /*&*/ 7, /*'*/ 2, /*(*/ 4, /*)*/ 4, /***/ 0, /*+*/ 6, /*,*/ 2, /*-*/ 6, /*.*/ 2, /*/*/ 6, /*0*/ 6,
    /*1*/ 5, /*2*/ 5, /*3*/ 5, /*4*/ 5, /*5*/ 5, /*6*/ 5, /*7*/ 5, /*8*/ 5, /*9*/ 5, /*:*/ 3, /*;*/ 3, /*<*/ 0, /*=*/ 0, /*>*/ 0, /*?*/ 6, /*@*/ 0, 
    /*A*/ 6, /*B*/ 6, /*C*/ 6, /*D*/ 6, /*E*/ 6, /*F*/ 6, /*G*/ 6, /*H*/ 6, /*I*/ 4, /*J*/ 6, /*K*/ 6, /*L*/ 6, /*M*/ 7, /*N*/ 7, /*O*/ 6, /*P*/ 6, 
    /*Q*/ 6, /*R*/ 6, /*S*/ 6, /*T*/ 6, /*U*/ 6, /*V*/ 6, /*W*/ 7, /*X*/ 6, /*Y*/ 6, /*Z*/ 6, /*[*/ 0, /*\\*/ 0, /*]*/ 0, /*^*/ 7, /*_*/ 0, /*`*/ 0,
    /*a*/ 6, /*b*/ 6, /*c*/ 6, /*d*/ 6, /*e*/ 6, /*f*/ 6, /*g*/ 6, /*h*/ 6, /*i*/ 3, /*j*/ 4, /*k*/ 6, /*l*/ 5, /*m*/ 7, /*n*/ 6, /*o*/ 6, /*p*/ 6, 
    /*q*/ 6, /*r*/ 6, /*s*/ 6, /*t*/ 6, /*u*/ 6, /*v*/ 6, /*w*/ 7, /*x*/ 6, /*y*/ 6, /*z*/ 6, /*{*/ 0, /*|*/ 0, /*}*/ 0, /*~*/ 7,   
};

const u16 small_font_offsets_plain[] = {
    /*!*/ 0, /*"*/ 8, /*#*/ 16, /*$*/ 24, /*%*/ 32, /*&*/ 40, /*'*/ 48, /*(*/ 56, /*)*/ 64, /***/ 72, /*+*/ 80, /*,*/ 88, /*-*/ 96, /*.*/ 104, /*/*/ 112,
    /*0*/ 120, /*1*/ 128, /*2*/ 136, /*3*/ 144, /*4*/ 152, /*5*/ 160, /*6*/ 168, /*7*/ 176, /*8*/ 184, /*9*/ 192, /*:*/ 200, /*;*/ 208, /*<*/ 216, /*=*/ 216,
    /*>*/ 216, /*?*/ 216, /*@*/ 224, /*A*/ 224, /*B*/ 232, /*C*/ 240, /*D*/ 248, /*E*/ 256, /*F*/ 264, /*G*/ 272, /*H*/ 280, /*I*/ 288, /*J*/ 296, /*K*/ 304,
    /*L*/ 312, /*M*/ 320, /*N*/ 328, /*O*/ 336, /*P*/ 344, /*Q*/ 352, /*R*/ 360, /*S*/ 368, /*T*/ 376, /*U*/ 384, /*V*/ 392, /*W*/ 400, /*X*/ 408, /*Y*/ 416,
    /*Z*/ 424, /*[*/ 432, /*\*/ 432, /*]*/ 432, /*^*/ 432, /*_*/ 440, /*`*/ 440, /*a*/ 440, /*b*/ 448, /*c*/ 456, /*d*/ 464, /*e*/ 472, /*f*/ 480, /*g*/ 488, 
    /*h*/ 496, /*i*/ 504, /*j*/ 512, /*k*/ 520, /*l*/ 528, /*m*/ 536, /*n*/ 544, /*o*/ 552, /*p*/ 560, /*q*/ 568, /*r*/ 576, /*s*/ 584, /*t*/ 592, /*u*/ 600, 
    /*v*/ 608, /*w*/ 616, /*x*/ 624, /*y*/ 632, /*z*/ 640, /*{*/ 648, /*|*/ 648, /*}*/ 648, /*~*/ 648, /*:)*/ 656
};

static const u8 small_font_kerning_vanilla[] = {
    /*!*/ 4, /*"*/ 4, /*#*/ 0, /*$*/ 0, /*%*/ 6, /*&*/ 7, /*'*/ 3, /*(*/ 4, /*)*/ 4, /***/ 0, /*+*/ 6, /*,*/ 2, /*-*/ 4, /*.*/ 3, /*/*/ 4, /*0*/ 5,
    /*1*/ 5, /*2*/ 6, /*3*/ 6, /*4*/ 6, /*5*/ 6, /*6*/ 6, /*7*/ 6, /*8*/ 6, /*9*/ 6, /*:*/ 4, /*;*/ 4, /*<*/ 0, /*=*/ 0, /*>*/ 0, /*?*/ 5, /*@*/ 0,  
    /*A*/ 5, /*B*/ 5, /*C*/ 5, /*D*/ 5, /*E*/ 5, /*F*/ 5, /*G*/ 5, /*H*/ 5, /*I*/ 3, /*J*/ 5, /*K*/ 5, /*L*/ 5, /*M*/ 7, /*N*/ 7, /*O*/ 6, /*P*/ 5, 
    /*Q*/ 6, /*R*/ 5, /*S*/ 5, /*T*/ 5, /*U*/ 5, /*V*/ 5, /*W*/ 7, /*X*/ 6, /*Y*/ 5, /*Z*/ 5, /*[*/ 0, /*\\*/ 0, /*]*/ 0, /*^*/ 7, /*_*/ 0, /*`*/ 0, 
    /*a*/ 5, /*b*/ 4, /*c*/ 4, /*d*/ 4, /*e*/ 4, /*f*/ 5, /*g*/ 5, /*h*/ 4, /*i*/ 3, /*j*/ 4, /*k*/ 3, /*l*/ 2, /*m*/ 6, /*n*/ 4, /*o*/ 4, /*p*/ 4, 
    /*q*/ 5, /*r*/ 4, /*s*/ 4, /*t*/ 4, /*u*/ 4, /*v*/ 4, /*w*/ 7, /*x*/ 5, /*y*/ 4, /*z*/ 5, /*{*/ 0, /*|*/ 0, /*}*/ 0, /*~*/ 6,   
};

const u16 small_font_offsets_vanilla[] = {
    /*!*/ 0, /*"*/ 8, /*#*/ 16, /*$*/ 24, /*%*/ 32, /*&*/ 40, /*'*/ 48, /*(*/ 56, /*)*/ 64, /***/ 72, /*+*/ 80, /*,*/ 88, /*-*/ 96, /*.*/ 104, /*/*/ 112,
    /*0*/ 120, /*1*/ 128, /*2*/ 136, /*3*/ 144, /*4*/ 152, /*5*/ 160, /*6*/ 168, /*7*/ 176, /*8*/ 184, /*9*/ 192, /*:*/ 200, /*;*/ 208, /*<*/ 216, /*=*/ 216,
    /*>*/ 216, /*?*/ 216, /*@*/ 224, /*A*/ 224, /*B*/ 232, /*C*/ 240, /*D*/ 248, /*E*/ 256, /*F*/ 264, /*G*/ 272, /*H*/ 280, /*I*/ 288, /*J*/ 296, /*K*/ 304,
    /*L*/ 312, /*M*/ 320, /*N*/ 328, /*O*/ 336, /*P*/ 344, /*Q*/ 352, /*R*/ 360, /*S*/ 368, /*T*/ 376, /*U*/ 384, /*V*/ 392, /*W*/ 400, /*X*/ 408, /*Y*/ 416,
    /*Z*/ 424, /*[*/ 432, /*\*/ 432, /*]*/ 432, /*^*/ 432, /*_*/ 440, /*`*/ 440, /*a*/ 440, /*b*/ 448, /*c*/ 456, /*d*/ 464, /*e*/ 472, /*f*/ 480, /*g*/ 488, 
    /*h*/ 496, /*i*/ 504, /*j*/ 512, /*k*/ 520, /*l*/ 528, /*m*/ 536, /*n*/ 544, /*o*/ 552, /*p*/ 560, /*q*/ 568, /*r*/ 576, /*s*/ 584, /*t*/ 592, /*u*/ 600, 
    /*v*/ 608, /*w*/ 616, /*x*/ 624, /*y*/ 632, /*z*/ 640, /*{*/ 648, /*|*/ 648, /*}*/ 648, /*~*/ 648, /*:)*/ 656
};

const struct PPTextFont sPPFont_default = {
    small_font_default,
    small_font_kerning_default,
    small_font_offsets_default,
    NULL,
    G_IM_FMT_I, G_IM_SIZ_4b,
    672, 12,
    8, 12
};

const struct PPTextFont sPPFont_outline = {
    small_font_outline,
    small_font_kerning_outline,
    small_font_offsets_outline,
    NULL,
    G_IM_FMT_IA, G_IM_SIZ_4b,
    672, 12,
    8, 12
};

const struct PPTextFont sPPFont_plain = {
    small_font_plain,
    small_font_kerning_plain,
    small_font_offsets_plain,
    NULL,
    G_IM_FMT_I, G_IM_SIZ_4b,
    672, 12,
    8, 12
};

const struct PPTextFont sPPFont_vanilla = {
    small_font_vanilla,
    small_font_kerning_vanilla,
    small_font_offsets_vanilla,
    NULL,
    G_IM_FMT_I, G_IM_SIZ_4b,
    672, 12,
    8, 12
};

const struct PPTextFont *const gPuppyPrintFontTable[] = {
    &sPPFont_default, &sPPFont_outline, &sPPFont_plain, &sPPFont_vanilla
};


const Texture *const puppyprint_font_lut[] = {
    small_font_default, small_font_outline, small_font_plain, small_font_vanilla
};

const u8 *const puppyprint_kerning_lut[][95] = {
    small_font_kerning_default, small_font_kerning_outline, small_font_kerning_plain, small_font_kerning_vanilla
};

#endif

ALIGNED8 static const Texture texture_hud_char_0[] = {
#include "textures/segment2/segment2.00000.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_1[] = {
#include "textures/segment2/segment2.00200.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_2[] = {
#include "textures/segment2/segment2.00400.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_3[] = {
#include "textures/segment2/segment2.00600.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_4[] = {
#include "textures/segment2/segment2.00800.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_5[] = {
#include "textures/segment2/segment2.00A00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_6[] = {
#include "textures/segment2/segment2.00C00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_7[] = {
#include "textures/segment2/segment2.00E00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_8[] = {
#include "textures/segment2/segment2.01000.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_9[] = {
#include "textures/segment2/segment2.01200.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_A[] = {
#include "textures/segment2/segment2.01400.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_B[] = {
#include "textures/segment2/segment2.01600.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_C[] = {
#include "textures/segment2/segment2.01800.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_D[] = {
#include "textures/segment2/segment2.01A00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_E[] = {
#include "textures/segment2/segment2.01C00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_F[] = {
#include "textures/segment2/segment2.01E00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_G[] = {
#include "textures/segment2/segment2.02000.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_H[] = {
#include "textures/segment2/segment2.02200.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_I[] = {
#include "textures/segment2/segment2.02400.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_J[] = {
#include "textures/segment2/segment2.hud_char_j.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_K[] = {
#include "textures/segment2/segment2.02800.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_L[] = {
#include "textures/segment2/segment2.02A00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_M[] = {
#include "textures/segment2/segment2.02C00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_N[] = {
#include "textures/segment2/segment2.02E00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_O[] = {
#include "textures/segment2/segment2.03000.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_P[] = {
#include "textures/segment2/segment2.03200.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_Q[] = {
#include "textures/segment2/segment2.hud_char_q.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_R[] = {
#include "textures/segment2/segment2.03600.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_S[] = {
#include "textures/segment2/segment2.03800.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_T[] = {
#include "textures/segment2/segment2.03A00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_U[] = {
#include "textures/segment2/segment2.03C00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_V[] = {
#include "textures/segment2/segment2.hud_char_v.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_W[] = {
#include "textures/segment2/segment2.04000.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_X[] = {
#include "textures/segment2/segment2.hud_char_x.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_Y[] = {
#include "textures/segment2/segment2.04400.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_Z[] = {
#include "textures/segment2/segment2.hud_char_z.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_apostrophe[] = {
#include "textures/segment2/segment2.04800.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_double_quote[] = {
#include "textures/segment2/segment2.04A00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_exclamation[] = {
#include "textures/segment2/segment2.exclamation.rgba16.inc.c"// JP !
};

ALIGNED8 static const Texture texture_hud_char_double_exclamation[] = {
#include "textures/segment2/segment2.double_exclamation.rgba16.inc.c"// JP !!
};

ALIGNED8 static const Texture texture_hud_char_question[] = {
#include "textures/segment2/segment2.question.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_ampersand[] = {
#include "textures/segment2/segment2.ampersand.rgba16.inc.c"// JP &
};

ALIGNED8 static const Texture texture_hud_char_percent[] = {
#include "textures/segment2/segment2.percent.rgba16.inc.c"// JP %
};

ALIGNED8 static const Texture texture_hud_char_minus[] = {
#include "textures/segment2/segment2.minus.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_multiply[] = {
#include "textures/segment2/segment2.05600.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_coin[] = {
#include "textures/segment2/segment2.05800.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_mario_head[] = {
#include "textures/segment2/segment2.05A00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_star[] = {
#include "textures/segment2/segment2.05C00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_decimal_point[] = {
#include "textures/segment2/segment2.decimal_point.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_beta_key[] = {
#include "textures/segment2/segment2.beta_key.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_floomba[] = {
#include "textures/segment2/segment2.floomba.rgba16.inc.c"
};

ALIGNED8 const Texture texture_hud_char_umlaut[] = {
#include "textures/segment2/segment2.umlaut_us.rgba16.inc.c"// EU ¨
};

ALIGNED8 static const Texture texture_credits_char_3[] = {
#include "textures/segment2/segment2.06200.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_4[] = {
#include "textures/segment2/segment2.06280.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_6[] = {
#include "textures/segment2/segment2.06300.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_A[] = {
#include "textures/segment2/segment2.06380.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_B[] = {
#include "textures/segment2/segment2.06400.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_C[] = {
#include "textures/segment2/segment2.06480.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_D[] = {
#include "textures/segment2/segment2.06500.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_E[] = {
#include "textures/segment2/segment2.06580.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_F[] = {
#include "textures/segment2/segment2.06600.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_G[] = {
#include "textures/segment2/segment2.06680.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_H[] = {
#include "textures/segment2/segment2.06700.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_I[] = {
#include "textures/segment2/segment2.06780.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_J[] = {
#include "textures/segment2/segment2.06800.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_K[] = {
#include "textures/segment2/segment2.06880.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_L[] = {
#include "textures/segment2/segment2.06900.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_M[] = {
#include "textures/segment2/segment2.06980.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_N[] = {
#include "textures/segment2/segment2.06A00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_O[] = {
#include "textures/segment2/segment2.06A80.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_P[] = {
#include "textures/segment2/segment2.06B00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_Q[] = {
#include "textures/segment2/segment2.06B80.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_R[] = {
#include "textures/segment2/segment2.06C00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_S[] = {
#include "textures/segment2/segment2.06C80.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_T[] = {
#include "textures/segment2/segment2.06D00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_U[] = {
#include "textures/segment2/segment2.06D80.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_V[] = {
#include "textures/segment2/segment2.06E00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_W[] = {
#include "textures/segment2/segment2.06E80.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_X[] = {
#include "textures/segment2/segment2.06F00.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_Y[] = {
#include "textures/segment2/segment2.06F80.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_Z[] = {
#include "textures/segment2/segment2.07000.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_credits_char_period[] = {
#include "textures/segment2/segment2.07080.rgba16.inc.c"
};

#ifdef JAPANESE_CHARACTERS
// JP Small Font
ALIGNED8 static const Texture texture_font_char_jp_0[] = {
#include "textures/segment2/segment2.07100.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_1[] = {
#include "textures/segment2/segment2.07110.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_2[] = {
#include "textures/segment2/segment2.07120.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_3[] = {
#include "textures/segment2/segment2.07130.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_4[] = {
#include "textures/segment2/segment2.07140.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_5[] = {
#include "textures/segment2/segment2.07150.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_6[] = {
#include "textures/segment2/segment2.07160.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_7[] = {
#include "textures/segment2/segment2.07170.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_8[] = {
#include "textures/segment2/segment2.07180.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_9[] = {
#include "textures/segment2/segment2.07190.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_A[] = {
#include "textures/segment2/segment2.071A0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_B[] = {
#include "textures/segment2/segment2.071B0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_C[] = {
#include "textures/segment2/segment2.071C0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_D[] = {
#include "textures/segment2/segment2.071D0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_E[] = {
#include "textures/segment2/segment2.071E0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_F[] = {
#include "textures/segment2/segment2.071F0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_G[] = {
#include "textures/segment2/segment2.07200.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_H[] = {
#include "textures/segment2/segment2.07210.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_I[] = {
#include "textures/segment2/segment2.07220.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_J[] = {
#include "textures/segment2/segment2.07230.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_K[] = {
#include "textures/segment2/segment2.07240.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_L[] = {
#include "textures/segment2/segment2.07250.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_M[] = {
#include "textures/segment2/segment2.07260.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_N[] = {
#include "textures/segment2/segment2.07270.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_O[] = {
#include "textures/segment2/segment2.07280.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_P[] = {
#include "textures/segment2/segment2.07290.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_Q[] = {
#include "textures/segment2/segment2.072A0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_R[] = {
#include "textures/segment2/segment2.072B0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_S[] = {
#include "textures/segment2/segment2.072C0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_T[] = {
#include "textures/segment2/segment2.072D0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_U[] = {
#include "textures/segment2/segment2.072E0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_V[] = {
#include "textures/segment2/segment2.072F0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_W[] = {
#include "textures/segment2/segment2.07300.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_X[] = {
#include "textures/segment2/segment2.07310.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_Y[] = {
#include "textures/segment2/segment2.07320.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_Z[] = {
#include "textures/segment2/segment2.07330.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_long_vowel[] = {
#include "textures/segment2/segment2.07340.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_exclamation[] = {
#include "textures/segment2/segment2.07350.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_dakuten[] = {
#include "textures/segment2/segment2.07360.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_period[] = {
#include "textures/segment2/segment2.jp_period.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_percent[] = {
#include "textures/segment2/segment2.07380.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_question[] = {
#include "textures/segment2/segment2.07390.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_open_parentheses[] = {
#include "textures/segment2/segment2.073B0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_close_parentheses[] = {
#include "textures/segment2/segment2.073D0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_multiply[] = {
#include "textures/segment2/segment2.073F0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_a[] = {
#include "textures/segment2/segment2.07420.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_i[] = {
#include "textures/segment2/segment2.07430.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_u[] = {
#include "textures/segment2/segment2.07440.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_e[] = {
#include "textures/segment2/segment2.07450.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_o[] = {
#include "textures/segment2/segment2.07460.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ka[] = {
#include "textures/segment2/segment2.07470.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ki[] = {
#include "textures/segment2/segment2.07480.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ku[] = {
#include "textures/segment2/segment2.07490.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ke[] = {
#include "textures/segment2/segment2.074A0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ko[] = {
#include "textures/segment2/segment2.074B0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_sa[] = {
#include "textures/segment2/segment2.074C0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_shi[] = {
#include "textures/segment2/segment2.074D0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_su[] = {
#include "textures/segment2/segment2.074E0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_se[] = {
#include "textures/segment2/segment2.074F0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_so[] = {
#include "textures/segment2/segment2.07500.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ta[] = {
#include "textures/segment2/segment2.07510.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_chi[] = {
#include "textures/segment2/segment2.07520.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_tsu[] = {
#include "textures/segment2/segment2.07530.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_te[] = {
#include "textures/segment2/segment2.07540.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_to[] = {
#include "textures/segment2/segment2.07550.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_na[] = {
#include "textures/segment2/segment2.07560.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ni[] = {
#include "textures/segment2/segment2.07570.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_nu[] = {
#include "textures/segment2/segment2.07580.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ne[] = {
#include "textures/segment2/segment2.07590.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_no[] = {
#include "textures/segment2/segment2.075A0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ha[] = {
#include "textures/segment2/segment2.075B0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_hi[] = {
#include "textures/segment2/segment2.075C0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_fu[] = {
#include "textures/segment2/segment2.075D0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_he[] = {
#include "textures/segment2/segment2.075E0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ho[] = {
#include "textures/segment2/segment2.075F0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ma[] = {
#include "textures/segment2/segment2.07600.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_mi[] = {
#include "textures/segment2/segment2.07610.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_mu[] = {
#include "textures/segment2/segment2.07620.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_me[] = {
#include "textures/segment2/segment2.07630.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_mo[] = {
#include "textures/segment2/segment2.07640.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ya[] = {
#include "textures/segment2/segment2.07650.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_yu[] = {
#include "textures/segment2/segment2.07660.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_yo[] = {
#include "textures/segment2/segment2.07670.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ra[] = {
#include "textures/segment2/segment2.07680.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ri[] = {
#include "textures/segment2/segment2.07690.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ru[] = {
#include "textures/segment2/segment2.076A0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_re[] = {
#include "textures/segment2/segment2.076B0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_ro[] = {
#include "textures/segment2/segment2.076C0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_wa[] = {
#include "textures/segment2/segment2.076D0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_wo[] = {
#include "textures/segment2/segment2.076E0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_n[] = {
#include "textures/segment2/segment2.076F0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_small_e[] = {
#include "textures/segment2/segment2.07700.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_small_tsu[] = {
#include "textures/segment2/segment2.07710.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_small_ya[] = {
#include "textures/segment2/segment2.07720.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_small_yu[] = {
#include "textures/segment2/segment2.07730.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_small_yo[] = {
#include "textures/segment2/segment2.07740.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_small_a[] = {
#include "textures/segment2/segment2.07750.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_small_i[] = {
#include "textures/segment2/segment2.07760.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_small_o[] = {
#include "textures/segment2/segment2.07770.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_hiragana_small_u[] = {
#include "textures/segment2/segment2.07780.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_interpunct[] = {
#include "textures/segment2/segment2.07790.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_comma[] = {
#include "textures/segment2/segment2.077A0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_a[] = {
#include "textures/segment2/segment2.077B0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_i[] = {
#include "textures/segment2/segment2.077C0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_u[] = {
#include "textures/segment2/segment2.077D0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_e[] = {
#include "textures/segment2/segment2.077E0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_o[] = {
#include "textures/segment2/segment2.077F0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ka[] = {
#include "textures/segment2/segment2.07800.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ki[] = {
#include "textures/segment2/segment2.07810.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ku[] = {
#include "textures/segment2/segment2.07820.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ke[] = {
#include "textures/segment2/segment2.07830.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ko[] = {
#include "textures/segment2/segment2.07840.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_sa[] = {
#include "textures/segment2/segment2.07850.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_shi[] = {
#include "textures/segment2/segment2.07860.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_su[] = {
#include "textures/segment2/segment2.07870.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_se[] = {
#include "textures/segment2/segment2.07880.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_so[] = {
#include "textures/segment2/segment2.07890.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ta[] = {
#include "textures/segment2/segment2.078A0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_chi[] = {
#include "textures/segment2/segment2.078B0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_tsu[] = {
#include "textures/segment2/segment2.078C0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_te[] = {
#include "textures/segment2/segment2.078D0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_to[] = {
#include "textures/segment2/segment2.078E0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_na[] = {
#include "textures/segment2/segment2.078F0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ni[] = {
#include "textures/segment2/segment2.07900.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_nu[] = {
#include "textures/segment2/segment2.07910.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ne[] = {
#include "textures/segment2/segment2.07920.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_no[] = {
#include "textures/segment2/segment2.07930.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ha[] = {
#include "textures/segment2/segment2.07940.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_hi[] = {
#include "textures/segment2/segment2.07950.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_fu[] = {
#include "textures/segment2/segment2.07960.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_he[] = {
#include "textures/segment2/segment2.07970.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ho[] = {
#include "textures/segment2/segment2.07980.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ma[] = {
#include "textures/segment2/segment2.07990.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_mi[] = {
#include "textures/segment2/segment2.079A0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_mu[] = {
#include "textures/segment2/segment2.079B0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_me[] = {
#include "textures/segment2/segment2.079C0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_mo[] = {
#include "textures/segment2/segment2.079D0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ya[] = {
#include "textures/segment2/segment2.079E0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_yu[] = {
#include "textures/segment2/segment2.079F0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_yo[] = {
#include "textures/segment2/segment2.07A00.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ra[] = {
#include "textures/segment2/segment2.07A10.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ri[] = {
#include "textures/segment2/segment2.07A20.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ru[] = {
#include "textures/segment2/segment2.07A30.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_re[] = {
#include "textures/segment2/segment2.07A40.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_ro[] = {
#include "textures/segment2/segment2.07A50.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_wa[] = {
#include "textures/segment2/segment2.07A60.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_n[] = {
#include "textures/segment2/segment2.07A70.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_small_e[] = {
#include "textures/segment2/segment2.07A80.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_small_tsu[] = {
#include "textures/segment2/segment2.07A90.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_small_ya[] = {
#include "textures/segment2/segment2.07AA0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_small_yu[] = {
#include "textures/segment2/segment2.07AB0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_small_yo[] = {
#include "textures/segment2/segment2.07AC0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_small_a[] = {
#include "textures/segment2/segment2.07AD0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_small_i[] = {
#include "textures/segment2/segment2.07AE0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_small_u[] = {
#include "textures/segment2/segment2.07AF0.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_katakana_small_o[] = {
#include "textures/segment2/segment2.07B00.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_double_quotation_open[] = {
#include "textures/segment2/segment2.07B10.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_double_quotation_close[] = {
#include "textures/segment2/segment2.07B20.ia1.inc.c"
};

ALIGNED8 static const Texture texture_font_char_jp_tilde[] = {
#include "textures/segment2/segment2.07B30.ia1.inc.c"
};
#endif

// US Small Font

ALIGNED8 static const Texture texture_font_char_us_0[] = {
#include "textures/segment2/font_graphics.05900.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_1[] = {
#include "textures/segment2/font_graphics.05940.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_2[] = {
#include "textures/segment2/font_graphics.05980.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_3[] = {
#include "textures/segment2/font_graphics.059C0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_4[] = {
#include "textures/segment2/font_graphics.05A00.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_5[] = {
#include "textures/segment2/font_graphics.05A40.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_6[] = {
#include "textures/segment2/font_graphics.05A80.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_7[] = {
#include "textures/segment2/font_graphics.05AC0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_8[] = {
#include "textures/segment2/font_graphics.05B00.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_9[] = {
#include "textures/segment2/font_graphics.05B40.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_A[] = {
#include "textures/segment2/font_graphics.05B80.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_B[] = {
#include "textures/segment2/font_graphics.05BC0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_C[] = {
#include "textures/segment2/font_graphics.05C00.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_D[] = {
#include "textures/segment2/font_graphics.05C40.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_E[] = {
#include "textures/segment2/font_graphics.05C80.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_F[] = {
#include "textures/segment2/font_graphics.05CC0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_G[] = {
#include "textures/segment2/font_graphics.05D00.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_H[] = {
#include "textures/segment2/font_graphics.05D40.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_I[] = {
#include "textures/segment2/font_graphics.05D80.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_J[] = {
#include "textures/segment2/font_graphics.05DC0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_K[] = {
#include "textures/segment2/font_graphics.05E00.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_L[] = {
#include "textures/segment2/font_graphics.05E40.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_M[] = {
#include "textures/segment2/font_graphics.05E80.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_N[] = {
#include "textures/segment2/font_graphics.05EC0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_O[] = {
#include "textures/segment2/font_graphics.05F00.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_P[] = {
#include "textures/segment2/font_graphics.05F40.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_Q[] = {
#include "textures/segment2/font_graphics.05F80.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_R[] = {
#include "textures/segment2/font_graphics.05FC0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_S[] = {
#include "textures/segment2/font_graphics.06000.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_T[] = {
#include "textures/segment2/font_graphics.06040.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_U[] = {
#include "textures/segment2/font_graphics.06080.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_V[] = {
#include "textures/segment2/font_graphics.060C0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_W[] = {
#include "textures/segment2/font_graphics.06100.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_X[] = {
#include "textures/segment2/font_graphics.06140.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_Y[] = {
#include "textures/segment2/font_graphics.06180.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_Z[] = {
#include "textures/segment2/font_graphics.061C0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_a[] = {
#include "textures/segment2/font_graphics.06200.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_b[] = {
#include "textures/segment2/font_graphics.06240.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_c[] = {
#include "textures/segment2/font_graphics.06280.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_d[] = {
#include "textures/segment2/font_graphics.062C0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_e[] = {
#include "textures/segment2/font_graphics.06300.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_f[] = {
#include "textures/segment2/font_graphics.06340.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_g[] = {
#include "textures/segment2/font_graphics.06380.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_h[] = {
#include "textures/segment2/font_graphics.063C0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_i[] = {
#include "textures/segment2/font_graphics.06400.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_j[] = {
#include "textures/segment2/font_graphics.06440.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_k[] = {
#include "textures/segment2/font_graphics.06480.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_l[] = {
#include "textures/segment2/font_graphics.064C0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_m[] = {
#include "textures/segment2/font_graphics.06500.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_n[] = {
#include "textures/segment2/font_graphics.06540.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_o[] = {
#include "textures/segment2/font_graphics.06580.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_p[] = {
#include "textures/segment2/font_graphics.065C0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_q[] = {
#include "textures/segment2/font_graphics.06600.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_r[] = {
#include "textures/segment2/font_graphics.06640.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_s[] = {
#include "textures/segment2/font_graphics.06680.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_t[] = {
#include "textures/segment2/font_graphics.066C0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_u[] = {
#include "textures/segment2/font_graphics.06700.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_v[] = {
#include "textures/segment2/font_graphics.06740.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_w[] = {
#include "textures/segment2/font_graphics.06780.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_x[] = {
#include "textures/segment2/font_graphics.067C0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_y[] = {
#include "textures/segment2/font_graphics.06800.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_z[] = {
#include "textures/segment2/font_graphics.06840.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_left_right_arrow[] = {
#include "textures/segment2/font_graphics.06880.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_exclamation[] = {
#include "textures/segment2/font_graphics.068C0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_coin[] = {
#include "textures/segment2/font_graphics.06900.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_multiply[] = {
#include "textures/segment2/font_graphics.06940.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_open_parentheses[] = {
#include "textures/segment2/font_graphics.06980.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_close_parentheses[] = {
#include "textures/segment2/font_graphics.06A00.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_tilde[] = {
#include "textures/segment2/font_graphics.06A40.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_period[] = {
#include "textures/segment2/font_graphics.06A80.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_percent[] = {
#include "textures/segment2/font_graphics.06AC0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_interpunct[] = {
#include "textures/segment2/font_graphics.06B00.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_comma[] = {
#include "textures/segment2/font_graphics.06B40.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_apostrophe[] = {
#include "textures/segment2/font_graphics.06B80.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_question[] = {
#include "textures/segment2/font_graphics.06BC0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_star_filled[] = {
#include "textures/segment2/font_graphics.06C00.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_star_hollow[] = {
#include "textures/segment2/font_graphics.06C40.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_double_quote_open[] = {
#include "textures/segment2/font_graphics.06C80.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_double_quote_close[] = {
#include "textures/segment2/font_graphics.06CC0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_double_low_quote[] = {
#include "textures/segment2/font_graphics.double_low_quote.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_colon[] = {
#include "textures/segment2/font_graphics.06D00.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_hyphen[] = {
#include "textures/segment2/font_graphics.06D40.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_ampersand[] = {
#include "textures/segment2/font_graphics.06D80.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_button_A[] = {
#include "textures/segment2/font_graphics.06DC0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_button_B[] = {
#include "textures/segment2/font_graphics.06E00.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_button_C[] = {
#include "textures/segment2/font_graphics.06E40.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_button_Z[] = {
#include "textures/segment2/font_graphics.06E80.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_button_R[] = {
#include "textures/segment2/font_graphics.06EC0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_button_C_up[] = {
#include "textures/segment2/font_graphics.06F00.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_button_C_down[] = {
#include "textures/segment2/font_graphics.06F40.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_button_C_left[] = {
#include "textures/segment2/font_graphics.06F80.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_button_C_right[] = {
#include "textures/segment2/font_graphics.06FC0.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_plus[] = {
#include "textures/segment2/font_graphics.plus.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_slash[] = {
#include "textures/segment2/font_graphics.slash.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_backslash[] = {
#include "textures/segment2/font_graphics.backslash.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_inverted_exclamation_mark[] = {
#include "textures/segment2/font_graphics.inverted_exclamation_mark.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_inverted_question_mark[] = {
#include "textures/segment2/font_graphics.inverted_question_mark.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_eszett[] = {
#include "textures/segment2/font_graphics.eszett.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_diacritic_grave[] = {
#include "textures/segment2/font_graphics.grave.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_diacritic_acute[] = {
#include "textures/segment2/font_graphics.acute.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_diacritic_circumflex[] = {
#include "textures/segment2/font_graphics.circumflex.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_diacritic_tilde[] = {
#include "textures/segment2/font_graphics.tilde_diacritic.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_diacritic_umlaut[] = {
#include "textures/segment2/font_graphics.umlaut.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_diacritic_cedilla[] = {
#include "textures/segment2/font_graphics.cedilla.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_us_i_no_dot[] = {
#include "textures/segment2/font_graphics.i_no_dot.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_char_amogus[] = {
#include "textures/segment2/font_graphics.amogus.ia4.inc.c"
};

ALIGNED8 static const Texture texture_font_missing_character[] = {
#include "textures/segment2/font_graphics.missing_character.ia4.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_camera[] = {
#include "textures/segment2/segment2.07B50.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_lakitu[] = {
#include "textures/segment2/segment2.07D50.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_no_camera[] = {
#include "textures/segment2/segment2.07F50.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_arrow_up[] = {
#include "textures/segment2/segment2.08150.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_arrow_down[] = {
#include "textures/segment2/segment2.081D0.rgba16.inc.c"
};

// Moved from menu segment 7 to segment 2 to be unified with rest of HUD font
#ifdef JAPANESE_CHARACTERS
ALIGNED8 static const Texture texture_hud_char_katakana_fu[] = {
#include "levels/menu/main_menu_seg7.073D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_small_a[] = {
#include "levels/menu/main_menu_seg7.075D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_i[] = {
#include "levels/menu/main_menu_seg7.077D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_ru[] = {
#include "levels/menu/main_menu_seg7.079D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_se[] = {
#include "levels/menu/main_menu_seg7.07BD0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_re[] = {
#include "levels/menu/main_menu_seg7.07DD0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_ku[] = {
#include "levels/menu/main_menu_seg7.07FD0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_to[] = {
#include "levels/menu/main_menu_seg7.081D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_hiragana_wo[] = {
#include "levels/menu/main_menu_seg7.083D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_ko[] = {
#include "levels/menu/main_menu_seg7.085D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_kana_handakuten_pi[] = {
#include "levels/menu/main_menu_seg7.087D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_long_vowel[] = {
#include "levels/menu/main_menu_seg7.089D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_hiragana_su[] = {
#include "levels/menu/main_menu_seg7.08BD0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_hiragana_ru[] = {
#include "levels/menu/main_menu_seg7.08DD0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_hiragana_ke[] = {
#include "levels/menu/main_menu_seg7.08FD0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_ma[] = {
#include "levels/menu/main_menu_seg7.091D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_ri[] = {
#include "levels/menu/main_menu_seg7.093D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_o[] = {
#include "levels/menu/main_menu_seg7.095D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_su[] = {
#include "levels/menu/main_menu_seg7.097D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_a[] = {
#include "levels/menu/main_menu_seg7.099D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_hiragana_mi[] = {
#include "levels/menu/main_menu_seg7.09BD0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_hira_dakuten_do[] = {
#include "levels/menu/main_menu_seg7.09DD0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_hiragana_no[] = {
#include "levels/menu/main_menu_seg7.09FD0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_sa[] = {
#include "levels/menu/main_menu_seg7.0A3D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_u[] = {
#include "levels/menu/main_menu_seg7.0A5D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_n[] = {
#include "levels/menu/main_menu_seg7.0A7D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_kana_dakuten_do[] = {
#include "levels/menu/main_menu_seg7.0A9D0.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_katakana_ra[] = {
#include "levels/menu/main_menu_seg7.katakana_ra.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_kana_dakuten_ge[] = {
#include "levels/menu/main_menu_seg7.kana_dakuten_ge.rgba16.inc.c"
};

ALIGNED8 static const Texture texture_hud_char_kana_dakuten_ji[] = {
#include "levels/menu/main_menu_seg7.kana_dakuten_ji.rgba16.inc.c"
};
#endif

// ASCII lookup table for the colorful HUD font
const struct AsciiCharLUTEntry main_hud_lut[] = {
    {NULL, 8}, // 32 " "
    {texture_hud_char_exclamation, 12}, // 33 "!"
    {texture_hud_char_double_quote, 10}, // 34 "\""
    {NULL, 0}, // 35 "#" (Unimplemented)
    {NULL, 0}, // 36 "$" (Unimplemented)
    {texture_hud_char_percent, 12}, // 37 "%"
    {texture_hud_char_ampersand, 12}, // 38 "&"
    {texture_hud_char_apostrophe, 8}, // 39 "'"
    {NULL, 0}, // 40 "(" (Unimplemented)
    {NULL, 0}, // 41 ")" (Unimplemented)
    {NULL, 0}, // 42 "*" (Unimplemented)
    {NULL, 0}, // 43 "+" (Unimplemented)
    {texture_hud_char_apostrophe, 8}, // 44 ","
    {texture_hud_char_minus, 16}, // 45 "-"
    {texture_hud_char_decimal_point, 8}, // 46 "."
    {NULL, 0}, // 47 "/" (Unimplemented)
    {texture_hud_char_0, 12}, // 48 "0"
    {texture_hud_char_1, 12}, // 49 "1"
    {texture_hud_char_2, 12}, // 50 "2"
    {texture_hud_char_3, 12}, // 51 "3"
    {texture_hud_char_4, 12}, // 52 "4"
    {texture_hud_char_5, 12}, // 53 "5"
    {texture_hud_char_6, 12}, // 54 "6"
    {texture_hud_char_7, 12}, // 55 "7"
    {texture_hud_char_8, 12}, // 56 "8"
    {texture_hud_char_9, 12}, // 57 "9"
    {NULL, 0}, // 58 ":" (Unimplemented)
    {NULL, 0}, // 59 ";" (Unimplemented)
    {NULL, 0}, // 60 "<" (Unimplemented)
    {NULL, 0}, // 61 "=" (Unimplemented)
    {NULL, 0}, // 62 ">" (Unimplemented)
    {texture_hud_char_question, 12}, // 63 "?"
    {NULL, 0}, // 64 "@" (Unimplemented)
    {texture_hud_char_A, 12}, // 65 "A"
    {texture_hud_char_B, 12}, // 66 "B"
    {texture_hud_char_C, 12}, // 67 "C"
    {texture_hud_char_D, 12}, // 68 "D"
    {texture_hud_char_E, 12}, // 69 "E"
    {texture_hud_char_F, 12}, // 70 "F"
    {texture_hud_char_G, 12}, // 71 "G"
    {texture_hud_char_H, 12}, // 72 "H"
    {texture_hud_char_I, 12}, // 73 "I"
    {texture_hud_char_J, 12}, // 74 "J"
    {texture_hud_char_K, 12}, // 75 "K"
    {texture_hud_char_L, 12}, // 76 "L"
    {texture_hud_char_M, 12}, // 77 "M"
    {texture_hud_char_N, 12}, // 78 "N"
    {texture_hud_char_O, 12}, // 79 "O"
    {texture_hud_char_P, 12}, // 80 "P"
    {texture_hud_char_Q, 12}, // 81 "Q"
    {texture_hud_char_R, 12}, // 82 "R"
    {texture_hud_char_S, 12}, // 83 "S"
    {texture_hud_char_T, 12}, // 84 "T"
    {texture_hud_char_U, 12}, // 85 "U"
    {texture_hud_char_V, 12}, // 86 "V"
    {texture_hud_char_W, 12}, // 87 "W"
    {texture_hud_char_X, 12}, // 88 "X"
    {texture_hud_char_Y, 12}, // 89 "Y"
    {texture_hud_char_Z, 12}, // 90 "Z"
    {NULL, 0}, // 91 "[" (Unimplemented)
    {NULL, 0}, // 92 "\\" (Unimplemented)
    {NULL, 0}, // 93 "]" (Unimplemented)
    {NULL, 0}, // 94 "^" (Unimplemented)
    {NULL, 0}, // 95 "_" (Unimplemented)
    {NULL, 0}, // 96 "`" (Unimplemented)
    {texture_hud_char_A, 12}, // 97 "a"
    {texture_hud_char_B, 12}, // 98 "b"
    {texture_hud_char_C, 12}, // 99 "c"
    {texture_hud_char_D, 12}, // 100 "d"
    {texture_hud_char_E, 12}, // 101 "e"
    {texture_hud_char_F, 12}, // 102 "f"
    {texture_hud_char_G, 12}, // 103 "g"
    {texture_hud_char_H, 12}, // 104 "h"
    {texture_hud_char_I, 12}, // 105 "i"
    {texture_hud_char_J, 12}, // 106 "j"
    {texture_hud_char_K, 12}, // 107 "k"
    {texture_hud_char_L, 12}, // 108 "l"
    {texture_hud_char_M, 12}, // 109 "m"
    {texture_hud_char_N, 12}, // 110 "n"
    {texture_hud_char_O, 12}, // 111 "o"
    {texture_hud_char_P, 12}, // 112 "p"
    {texture_hud_char_Q, 12}, // 113 "q"
    {texture_hud_char_R, 12}, // 114 "r"
    {texture_hud_char_S, 12}, // 115 "s"
    {texture_hud_char_T, 12}, // 116 "t"
    {texture_hud_char_U, 12}, // 117 "u"
    {texture_hud_char_V, 12}, // 118 "v"
    {texture_hud_char_W, 12}, // 119 "w"
    {texture_hud_char_X, 12}, // 120 "x"
    {texture_hud_char_Y, 12}, // 121 "y"
    {texture_hud_char_Z, 12}, // 122 "z"
    {NULL, 0}, // 123 "{" (Unimplemented)
    {NULL, 0}, // 124 "|" (Unimplemented)
    {NULL, 0}, // 125 "}" (Unimplemented)
    {NULL, 0}, // 126 "~" (Unimplemented)
};

// UTF-8 lookup tables for the colorful HUD font
const struct Utf8CharLUTEntry main_hud_utf8_2byte_lut[] = {
    {0x00C4, 12, TEXT_DIACRITIC_UMLAUT_UPPERCASE, texture_hud_char_A}, // Ä
    {0x00CB, 12, TEXT_DIACRITIC_UMLAUT_UPPERCASE, texture_hud_char_E}, // Ë
    {0x00CF, 12, TEXT_DIACRITIC_UMLAUT_UPPERCASE, texture_hud_char_I}, // Ï
    {0x00D6, 12, TEXT_DIACRITIC_UMLAUT_UPPERCASE, texture_hud_char_O}, // Ö
    {0x00D7, 15, 0, texture_hud_char_multiply}, // ×
    {0x00DC, 12, TEXT_DIACRITIC_UMLAUT_UPPERCASE, texture_hud_char_U}, // Ü
};

const struct Utf8CharLUTEntry main_hud_utf8_3byte_lut[] = {
    {0x203C, 12, 0, texture_hud_char_double_exclamation}, // ‼
    {0x2605, 16, 0, texture_hud_char_star}, // ★
    {0x263A, 16, 0, texture_hud_char_mario_head}, // ☺
    {0x26BF, 16, 0, texture_hud_char_beta_key}, // ⚿
    {0x272A, 16, 0, texture_hud_char_coin}, // ✪

#ifdef JAPANESE_CHARACTERS
    {0x3000, 16, 0, NULL}, // "　" (ideographic space)

    {0x3051, 16, 0, texture_hud_char_hiragana_ke}, // け
    {0x3059, 16, 0, texture_hud_char_hiragana_su}, // す
    {0x3069, 16, 0, texture_hud_char_hira_dakuten_do}, // ど
    {0x306E, 16, 0, texture_hud_char_hiragana_no}, // の
    {0x307F, 16, 0, texture_hud_char_hiragana_mi}, // み
    {0x308B, 16, 0, texture_hud_char_hiragana_ru}, // る
    {0x3092, 16, 0, texture_hud_char_hiragana_wo}, // を

    {0x30A1, 16, 0, texture_hud_char_katakana_small_a}, // ァ
    {0x30A2, 16, 0, texture_hud_char_katakana_a}, // ア
    {0x30A4, 16, 0, texture_hud_char_katakana_i}, // イ
    {0x30A6, 16, 0, texture_hud_char_katakana_u}, // ウ
    {0x30AA, 16, 0, texture_hud_char_katakana_o}, // オ
    {0x30AF, 16, 0, texture_hud_char_katakana_ku}, // ク
    {0x30B2, 16, 0, texture_hud_char_kana_dakuten_ge}, // ゲ
    {0x30B3, 16, 0, texture_hud_char_katakana_ko}, // コ
    {0x30B5, 16, 0, texture_hud_char_katakana_sa}, // サ
    {0x30B8, 16, 0, texture_hud_char_kana_dakuten_ji}, // ジ
    {0x30B9, 16, 0, texture_hud_char_katakana_su}, // ス
    {0x30BB, 16, 0, texture_hud_char_katakana_se}, // セ
    {0x30C8, 16, 0, texture_hud_char_katakana_to}, // ト
    {0x30C9, 16, 0, texture_hud_char_kana_dakuten_do}, // ド
    {0x30D4, 16, 0, texture_hud_char_kana_handakuten_pi}, // ピ
    {0x30D5, 16, 0, texture_hud_char_katakana_fu}, // フ
    {0x30DE, 16, 0, texture_hud_char_katakana_ma}, // マ
    {0x30E9, 16, 0, texture_hud_char_katakana_ra}, // ラ
    {0x30EA, 16, 0, texture_hud_char_katakana_ri}, // リ
    {0x30EB, 16, 0, texture_hud_char_katakana_ru}, // ル
    {0x30EC, 16, 0, texture_hud_char_katakana_re}, // レ
    {0x30F3, 16, 0, texture_hud_char_katakana_n}, // ン

    {0x30FC, 16, 0, texture_hud_char_long_vowel}, // ー
    {0xFF1F, 16, 0, texture_hud_char_question}, // ？
#endif
};

const struct Utf8CharLUTEntry main_hud_utf8_4byte_lut[] = {
    {0x1F633, 16, 0, texture_hud_char_floomba}, // 😳
};

const struct Utf8CharLUTEntry main_hud_utf8_missing_char = {0, 16, 0, texture_hud_char_question};

const struct Utf8LUT main_hud_utf8_lut = {
    main_hud_utf8_2byte_lut,
    main_hud_utf8_3byte_lut,
    main_hud_utf8_4byte_lut,
    ARRAY_COUNT(main_hud_utf8_2byte_lut),
    ARRAY_COUNT(main_hud_utf8_3byte_lut),
    ARRAY_COUNT(main_hud_utf8_4byte_lut),
    &main_hud_utf8_missing_char,
};

// Diacritics for the generic white font
const struct DiacriticLUTEntry main_font_diacritic_lut[] = {
    [TEXT_DIACRITIC_CIRCUMFLEX]           = { 0,  0, "ˆ"},
    [TEXT_DIACRITIC_CIRCUMFLEX_UPPERCASE] = { 1,  4, "ˆ"},
    [TEXT_DIACRITIC_ACUTE]                = {-1,  0, "ˊ"},
    [TEXT_DIACRITIC_ACUTE_UPPERCASE]      = { 0,  4, "ˊ"},
    [TEXT_DIACRITIC_GRAVE]                = {-1,  0, "ˋ"},
    [TEXT_DIACRITIC_GRAVE_UPPERCASE]      = { 0,  4, "ˋ"},
    [TEXT_DIACRITIC_TILDE]                = {-1,  0, "˜"},
    [TEXT_DIACRITIC_TILDE_UPPERCASE]      = { 1,  4, "˜"},
    [TEXT_DIACRITIC_UMLAUT]               = { 0,  0, "¨"},
    [TEXT_DIACRITIC_UMLAUT_UPPERCASE]     = { 1,  4, "¨"},
    [TEXT_DIACRITIC_CEDILLA]              = { 0,  0, "¸"},
#ifdef JAPANESE_CHARACTERS
    [TEXT_DIACRITIC_DAKUTEN]              = { 4,  6, "゛"},
    [TEXT_DIACRITIC_HANDAKUTEN]           = { 7, 10, "゜"},
#endif
};

// ASCII lookup table for the generic white font
const struct AsciiCharLUTEntry main_font_lut[] = {
    {NULL, 5}, // 32 " "
    {texture_font_char_us_exclamation, 5}, // 33 "!"
    {texture_font_char_us_double_quote_open, 6}, // 34 "\""
    {NULL, 0}, // 35 "#" (Unimplemented)
    {NULL, 0}, // 36 "$" (Unimplemented)
    {texture_font_char_us_percent, 7}, // 37 "%"
    {texture_font_char_us_ampersand, 8}, // 38 "&"
    {texture_font_char_us_apostrophe, 4}, // 39 "'"
    {texture_font_char_us_open_parentheses, 5}, // 40 "("
    {texture_font_char_us_close_parentheses, 5}, // 41 ")"
    {NULL, 0}, // 42 "*" (Unimplemented)
    {texture_font_char_plus, 7}, // 43 "+"
    {texture_font_char_us_comma, 4}, // 44 ","
    {texture_font_char_us_hyphen, 6}, // 45 "-"
    {texture_font_char_us_period, 4}, // 46 "."
    {texture_font_char_slash, 5}, // 47 "/"
    {texture_font_char_us_0, 7}, // 48 "0"
    {texture_font_char_us_1, 7}, // 49 "1"
    {texture_font_char_us_2, 7}, // 50 "2"
    {texture_font_char_us_3, 7}, // 51 "3"
    {texture_font_char_us_4, 7}, // 52 "4"
    {texture_font_char_us_5, 7}, // 53 "5"
    {texture_font_char_us_6, 7}, // 54 "6"
    {texture_font_char_us_7, 7}, // 55 "7"
    {texture_font_char_us_8, 7}, // 56 "8"
    {texture_font_char_us_9, 7}, // 57 "9"
    {texture_font_char_us_colon, 4}, // 58 ":"
    {NULL, 0}, // 59 ";" (Unimplemented)
    {NULL, 0}, // 60 "<" (Unimplemented)
    {NULL, 0}, // 61 "=" (Unimplemented)
    {NULL, 0}, // 62 ">" (Unimplemented)
    {texture_font_char_us_question, 7}, // 63 "?"
    {NULL, 0}, // 64 "@" (Unimplemented, used for color codes by default)
    {texture_font_char_us_A, 6}, // 65 "A"
    {texture_font_char_us_B, 6}, // 66 "B"
    {texture_font_char_us_C, 6}, // 67 "C"
    {texture_font_char_us_D, 6}, // 68 "D"
    {texture_font_char_us_E, 6}, // 69 "E"
    {texture_font_char_us_F, 6}, // 70 "F"
    {texture_font_char_us_G, 6}, // 71 "G"
    {texture_font_char_us_H, 6}, // 72 "H"
    {texture_font_char_us_I, 5}, // 73 "I"
    {texture_font_char_us_J, 6}, // 74 "J"
    {texture_font_char_us_K, 6}, // 75 "K"
    {texture_font_char_us_L, 5}, // 76 "L"
    {texture_font_char_us_M, 8}, // 77 "M"
    {texture_font_char_us_N, 8}, // 78 "N"
    {texture_font_char_us_O, 6}, // 79 "O"
    {texture_font_char_us_P, 6}, // 80 "P"
    {texture_font_char_us_Q, 6}, // 81 "Q"
    {texture_font_char_us_R, 6}, // 82 "R"
    {texture_font_char_us_S, 6}, // 83 "S"
    {texture_font_char_us_T, 5}, // 84 "T"
    {texture_font_char_us_U, 6}, // 85 "U"
    {texture_font_char_us_V, 6}, // 86 "V"
    {texture_font_char_us_W, 8}, // 87 "W"
    {texture_font_char_us_X, 7}, // 88 "X"
    {texture_font_char_us_Y, 6}, // 89 "Y"
    {texture_font_char_us_Z, 6}, // 90 "Z"
    {NULL, 0}, // 91 "[" (Unimplemented)
    {texture_font_char_backslash, 6}, // 92 "\\"
    {NULL, 0}, // 93 "]" (Unimplemented)
    {NULL, 0}, // 94 "^" (Unimplemented)
    {NULL, 0}, // 95 "_" (Unimplemented)
    {NULL, 0}, // 96 "`" (Unimplemented)
    {texture_font_char_us_a, 6}, // 97 "a"
    {texture_font_char_us_b, 5}, // 98 "b"
    {texture_font_char_us_c, 5}, // 99 "c"
    {texture_font_char_us_d, 6}, // 100 "d"
    {texture_font_char_us_e, 5}, // 101 "e"
    {texture_font_char_us_f, 5}, // 102 "f"
    {texture_font_char_us_g, 6}, // 103 "g"
    {texture_font_char_us_h, 5}, // 104 "h"
    {texture_font_char_us_i, 4}, // 105 "i"
    {texture_font_char_us_j, 5}, // 106 "j"
    {texture_font_char_us_k, 5}, // 107 "k"
    {texture_font_char_us_l, 3}, // 108 "l"
    {texture_font_char_us_m, 7}, // 109 "m"
    {texture_font_char_us_n, 5}, // 110 "n"
    {texture_font_char_us_o, 5}, // 111 "o"
    {texture_font_char_us_p, 5}, // 112 "p"
    {texture_font_char_us_q, 6}, // 113 "q"
    {texture_font_char_us_r, 5}, // 114 "r"
    {texture_font_char_us_s, 5}, // 115 "s"
    {texture_font_char_us_t, 5}, // 116 "t"
    {texture_font_char_us_u, 5}, // 117 "u"
    {texture_font_char_us_v, 5}, // 118 "v"
    {texture_font_char_us_w, 7}, // 119 "w"
    {texture_font_char_us_x, 7}, // 120 "x"
    {texture_font_char_us_y, 5}, // 121 "y"
    {texture_font_char_us_z, 5}, // 122 "z"
    {NULL, 0}, // 123 "{" (Unimplemented)
    {NULL, 0}, // 124 "|" (Unimplemented)
    {NULL, 0}, // 125 "}" (Unimplemented)
    {texture_font_char_us_tilde, 8}, // 126 "~"
};

/* 
 * This struct defines the UTF-8 characters supported by the main white font!
 * There are also similar tables for colorful HUD font and the smaller white font.
 * Adding new characters to the font is very easy!
 *
 * Look up the UTF-8 codepoint for your character. If the character is U+0D9E, the codepoint is 0x0D9E.
 * Determine which table the character belongs to, depending on if it takes up 2, 3 or 4 bytes.
 * Between U+0080 and U+07FF: 2 bytes
 * Between U+0800 and U+FFFF: 3 bytes
 * Between U+10000 and U+10FFFF: 4 bytes
 *
 * Add the texture with the other textures above, and add an entry for your new character in the corresponding table.
 * The format of the entry is: {<codepoint>, <character width>, <flags>, <texture name>}.
 * (flags will likely be 0).
 * 
 * MUST NOTE: You must place your entry in EXACTLY the right spot! The table is sorted by codepoint.
 * The tables will not work if they are not sorted properly.
 */

// UTF-8 lookup table for the generic white font
const struct Utf8CharLUTEntry main_font_utf8_2byte_lut[] = {
    {0x00A1, 5, 0, texture_font_char_inverted_exclamation_mark}, // ¡
    {0x00A8, 0, 0, texture_font_char_diacritic_umlaut}, // ¨
    {0x00B7, 4, 0, texture_font_char_us_interpunct}, // ·
    {0x00B8, 0, 0, texture_font_char_diacritic_cedilla}, // ¸
    {0x00BF, 7, 0, texture_font_char_inverted_question_mark}, // ¿

    {0x00C0, 6, TEXT_DIACRITIC_GRAVE_UPPERCASE, texture_font_char_us_A}, // À
    {0x00C1, 6, TEXT_DIACRITIC_ACUTE_UPPERCASE, texture_font_char_us_A}, // Á
    {0x00C2, 6, TEXT_DIACRITIC_CIRCUMFLEX_UPPERCASE, texture_font_char_us_A}, // Â
    {0x00C3, 6, TEXT_DIACRITIC_TILDE_UPPERCASE, texture_font_char_us_A}, // Ã
    {0x00C4, 6, TEXT_DIACRITIC_UMLAUT_UPPERCASE, texture_font_char_us_A}, // Ä

    {0x00C7, 6, TEXT_DIACRITIC_CEDILLA, texture_font_char_us_C}, // Ç
    {0x00C8, 6, TEXT_DIACRITIC_GRAVE_UPPERCASE, texture_font_char_us_E}, // È
    {0x00C9, 6, TEXT_DIACRITIC_ACUTE_UPPERCASE, texture_font_char_us_E}, // É
    {0x00CA, 6, TEXT_DIACRITIC_CIRCUMFLEX_UPPERCASE, texture_font_char_us_E}, // Ê
    {0x00CB, 6, TEXT_DIACRITIC_UMLAUT_UPPERCASE, texture_font_char_us_E}, // Ë

    {0x00CC, 5, TEXT_DIACRITIC_GRAVE_UPPERCASE, texture_font_char_us_I}, // Ì
    {0x00CD, 5, TEXT_DIACRITIC_ACUTE_UPPERCASE, texture_font_char_us_I}, // Í
    {0x00CE, 5, TEXT_DIACRITIC_CIRCUMFLEX_UPPERCASE, texture_font_char_us_I}, // Î
    {0x00CF, 5, TEXT_DIACRITIC_UMLAUT_UPPERCASE, texture_font_char_us_I}, // Ï

    {0x00D1, 8, TEXT_DIACRITIC_TILDE_UPPERCASE, texture_font_char_us_N}, // Ñ
    {0x00D2, 6, TEXT_DIACRITIC_GRAVE_UPPERCASE, texture_font_char_us_O}, // Ò
    {0x00D3, 6, TEXT_DIACRITIC_ACUTE_UPPERCASE, texture_font_char_us_O}, // Ó
    {0x00D4, 6, TEXT_DIACRITIC_CIRCUMFLEX_UPPERCASE, texture_font_char_us_O}, // Ô
    {0x00D5, 6, TEXT_DIACRITIC_TILDE_UPPERCASE, texture_font_char_us_O}, // Õ
    {0x00D6, 6, TEXT_DIACRITIC_UMLAUT_UPPERCASE, texture_font_char_us_O}, // Ö

    {0x00D7, 6, 0, texture_font_char_us_multiply}, // ×

    {0x00D9, 6, TEXT_DIACRITIC_GRAVE_UPPERCASE, texture_font_char_us_U}, // Ù
    {0x00DA, 6, TEXT_DIACRITIC_ACUTE_UPPERCASE, texture_font_char_us_U}, // Ú
    {0x00DB, 6, TEXT_DIACRITIC_CIRCUMFLEX_UPPERCASE, texture_font_char_us_U}, // Û
    {0x00DC, 6, TEXT_DIACRITIC_UMLAUT_UPPERCASE, texture_font_char_us_U}, // Ü

    {0x00DF, 6, 0, texture_font_char_eszett}, // ß

    {0x00E0, 6, TEXT_DIACRITIC_GRAVE, texture_font_char_us_a}, // à
    {0x00E1, 6, TEXT_DIACRITIC_ACUTE, texture_font_char_us_a}, // á
    {0x00E2, 6, TEXT_DIACRITIC_CIRCUMFLEX, texture_font_char_us_a}, // â
    {0x00E3, 6, TEXT_DIACRITIC_TILDE, texture_font_char_us_a}, // ã
    {0x00E4, 6, TEXT_DIACRITIC_UMLAUT, texture_font_char_us_a}, // ä

    {0x00E7, 5, TEXT_DIACRITIC_CEDILLA, texture_font_char_us_c}, // ç
    {0x00E8, 5, TEXT_DIACRITIC_GRAVE, texture_font_char_us_e}, // è
    {0x00E9, 5, TEXT_DIACRITIC_ACUTE, texture_font_char_us_e}, // é
    {0x00EA, 5, TEXT_DIACRITIC_CIRCUMFLEX, texture_font_char_us_e}, // ê
    {0x00EB, 5, TEXT_DIACRITIC_UMLAUT, texture_font_char_us_e}, // ë

    {0x00EC, 4, TEXT_DIACRITIC_GRAVE, texture_font_char_us_i_no_dot}, // ì
    {0x00ED, 4, TEXT_DIACRITIC_ACUTE, texture_font_char_us_i_no_dot}, // í
    {0x00EE, 4, TEXT_DIACRITIC_CIRCUMFLEX, texture_font_char_us_i_no_dot}, // î
    {0x00EF, 4, TEXT_DIACRITIC_UMLAUT, texture_font_char_us_i_no_dot}, // ï

    {0x00F1, 5, TEXT_DIACRITIC_TILDE, texture_font_char_us_n}, // ñ
    {0x00F2, 5, TEXT_DIACRITIC_GRAVE, texture_font_char_us_o}, // ò
    {0x00F3, 5, TEXT_DIACRITIC_ACUTE, texture_font_char_us_o}, // ó
    {0x00F4, 5, TEXT_DIACRITIC_CIRCUMFLEX, texture_font_char_us_o}, // ô
    {0x00F5, 5, TEXT_DIACRITIC_TILDE, texture_font_char_us_o}, // õ
    {0x00F6, 5, TEXT_DIACRITIC_UMLAUT, texture_font_char_us_o}, // ö

    {0x00F9, 5, TEXT_DIACRITIC_GRAVE, texture_font_char_us_u}, // ù
    {0x00FA, 5, TEXT_DIACRITIC_ACUTE, texture_font_char_us_u}, // ú
    {0x00FB, 5, TEXT_DIACRITIC_CIRCUMFLEX, texture_font_char_us_u}, // û
    {0x00FC, 5, TEXT_DIACRITIC_UMLAUT, texture_font_char_us_u}, // ü

    {0x02C6, 0, 0, texture_font_char_diacritic_circumflex}, // ˆ
    {0x02CA, 0, 0, texture_font_char_diacritic_acute}, // ˊ
    {0x02CB, 0, 0, texture_font_char_diacritic_grave}, // ˋ
    {0x02DC, 0, 0, texture_font_char_diacritic_tilde}, // ˜
};

const struct Utf8CharLUTEntry main_font_utf8_3byte_lut[] = {
    {0x0D9E, 8, 0, texture_font_char_amogus}, // ඞ
    {0x201C, 6, 0, texture_font_char_us_double_quote_open}, // “
    {0x201D, 6, 0, texture_font_char_us_double_quote_close}, // ”
    {0x201E, 6, 0, texture_font_char_us_double_low_quote}, // „
    {0x2194, 9, 0, texture_font_char_us_left_right_arrow}, // ↔
    
    {0x24B6, 7, 0, texture_font_char_us_button_A}, // Ⓐ
    {0x24B7, 7, 0, texture_font_char_us_button_B}, // Ⓑ
    {0x24B8, 6, 0, texture_font_char_us_button_C}, // Ⓒ
    {0x24C7, 7, 0, texture_font_char_us_button_R}, // Ⓡ
    {0x24CF, 7, 0, texture_font_char_us_button_Z}, // Ⓩ

    {0x25B2, 8, 0, texture_font_char_us_button_C_up}, // ▲
    {0x25B6, 8, 0, texture_font_char_us_button_C_right}, // ▶
    {0x25BC, 8, 0, texture_font_char_us_button_C_down}, // ▼
    {0x25C0, 8, 0, texture_font_char_us_button_C_left}, // ◀

    {0x2605, 10, 0, texture_font_char_us_star_filled}, // ★
    {0x2606, 10, 0, texture_font_char_us_star_hollow}, // ☆
    {0x272A, 8, 0, texture_font_char_us_coin}, // ✪

#ifdef JAPANESE_CHARACTERS
    {0x3000, 10, 0, NULL}, // "　" (ideographic space)
    {0x3001, 10, TEXT_FLAG_PACKED, texture_font_char_jp_comma}, // 、
    {0x3002, 10, TEXT_FLAG_PACKED, texture_font_char_jp_period}, // 。
    {0x300E, 10, TEXT_FLAG_PACKED, texture_font_char_jp_double_quotation_open}, // 『
    {0x300F, 10, TEXT_FLAG_PACKED, texture_font_char_jp_double_quotation_close}, // 』

    {0x3041, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_small_a}, // ぁ
    {0x3042, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_a}, // あ
    {0x3043, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_small_i}, // ぃ
    {0x3044, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_i}, // い
    {0x3045, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_small_u}, // ぅ
    {0x3046, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_u}, // う
    {0x3047, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_small_e}, // ぇ
    {0x3048, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_e}, // え
    {0x3049, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_small_o}, // ぉ
    {0x304A, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_o}, // お
    {0x304B, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ka}, // か
    {0x304C, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_ka}, // が
    {0x304D, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ki}, // き
    {0x304E, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_ki}, // ぎ
    {0x304F, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ku}, // く
    {0x3050, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_ku}, // ぐ
    {0x3051, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ke}, // け
    {0x3052, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_ke}, // げ
    {0x3053, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ko}, // こ
    {0x3054, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_ko}, // ご
    {0x3055, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_sa}, // さ
    {0x3056, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_sa}, // ざ
    {0x3057, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_shi}, // し
    {0x3058, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_shi}, // じ
    {0x3059, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_su}, // す
    {0x305A, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_su}, // ず
    {0x305B, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_se}, // せ
    {0x305C, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_se}, // ぜ
    {0x305D, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_so}, // そ
    {0x305E, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_so}, // ぞ
    {0x305F, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ta}, // た
    {0x3060, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_ta}, // だ
    {0x3061, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_chi}, // ち
    {0x3062, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_chi}, // ぢ
    {0x3063, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_small_tsu}, // っ
    {0x3064, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_tsu}, // つ
    {0x3065, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_tsu}, // づ
    {0x3066, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_te}, // て
    {0x3067, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_te}, // で
    {0x3068, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_to}, // と
    {0x3069, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_to}, // ど
    {0x306A, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_na}, // な
    {0x306B, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ni}, // に
    {0x306C, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_nu}, // ぬ
    {0x306D, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ne}, // ね
    {0x306E, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_no}, // の
    {0x306F, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ha}, // は
    {0x3070, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_ha}, // ば
    {0x3071, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_HANDAKUTEN, texture_font_char_jp_hiragana_ha}, // ぱ
    {0x3072, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_hi}, // ひ
    {0x3073, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_hi}, // び
    {0x3074, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_HANDAKUTEN, texture_font_char_jp_hiragana_hi}, // ぴ
    {0x3075, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_fu}, // ふ
    {0x3076, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_fu}, // ぶ
    {0x3077, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_HANDAKUTEN, texture_font_char_jp_hiragana_fu}, // ぷ
    {0x3078, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_he}, // へ
    {0x3079, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_he}, // べ
    {0x307A, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_HANDAKUTEN, texture_font_char_jp_hiragana_he}, // ぺ
    {0x307B, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ho}, // ほ
    {0x307C, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_hiragana_ho}, // ぼ
    {0x307D, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_HANDAKUTEN, texture_font_char_jp_hiragana_ho}, // ぽ
    {0x307E, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ma}, // ま
    {0x307F, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_mi}, // み
    {0x3080, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_mu}, // む
    {0x3081, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_me}, // め
    {0x3082, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_mo}, // も
    {0x3083, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_small_ya}, // ゃ
    {0x3084, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ya}, // や
    {0x3085, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_small_yu}, // ゅ
    {0x3086, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_yu}, // ゆ
    {0x3087, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_small_yo}, // ょ
    {0x3088, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_yo}, // よ
    {0x3089, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ra}, // ら
    {0x308A, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ri}, // り
    {0x308B, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ru}, // る
    {0x308C, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_re}, // れ
    {0x308D, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_ro}, // ろ
    {0x308F, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_wa}, // わ
    {0x3092, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_wo}, // を
    {0x3093, 10, TEXT_FLAG_PACKED, texture_font_char_jp_hiragana_n}, // ん

    {0x309B, 0, TEXT_FLAG_PACKED, texture_font_char_jp_dakuten}, // ゛
    {0x309C, 0, TEXT_FLAG_PACKED, texture_font_char_jp_period}, // ゜

    {0x30A1, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_small_a}, // ァ
    {0x30A2, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_a}, // ア
    {0x30A3, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_small_i}, // ィ
    {0x30A4, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_i}, // イ
    {0x30A5, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_small_u}, // ゥ
    {0x30A6, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_u}, // ウ
    {0x30A7, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_small_e}, // ェ
    {0x30A8, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_e}, // エ
    {0x30A9, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_small_o}, // ォ
    {0x30AA, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_o}, // オ
    {0x30AB, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ka}, // カ
    {0x30AC, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_ka}, // ガ
    {0x30AD, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ki}, // キ
    {0x30AE, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_ki}, // ギ
    {0x30AF, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ku}, // ク
    {0x30B0, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_ku}, // グ
    {0x30B1, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ke}, // ケ
    {0x30B2, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_ke}, // ゲ
    {0x30B3, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ko}, // コ
    {0x30B4, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_ko}, // ゴ
    {0x30B5, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_sa}, // サ
    {0x30B6, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_sa}, // ザ
    {0x30B7, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_shi}, // シ
    {0x30B8, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_shi}, // ジ
    {0x30B9, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_su}, // ス
    {0x30BA, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_su}, // ズ
    {0x30BB, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_se}, // セ
    {0x30BC, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_se}, // ゼ
    {0x30BD, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_so}, // ソ
    {0x30BE, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_so}, // ゾ
    {0x30BF, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ta}, // タ
    {0x30C0, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_ta}, // ダ
    {0x30C1, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_chi}, // チ
    {0x30C2, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_chi}, // ヂ
    {0x30C3, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_small_tsu}, // ッ
    {0x30C4, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_tsu}, // ツ
    {0x30C5, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_tsu}, // ヅ
    {0x30C6, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_te}, // テ
    {0x30C7, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_te}, // デ
    {0x30C8, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_to}, // ト
    {0x30C9, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_to}, // ド
    {0x30CA, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_na}, // ナ
    {0x30CB, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ni}, // ニ
    {0x30CC, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_nu}, // ヌ
    {0x30CD, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ne}, // ネ
    {0x30CE, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_no}, // ノ
    {0x30CF, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ha}, // ハ
    {0x30D0, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_ha}, // バ
    {0x30D1, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_HANDAKUTEN, texture_font_char_jp_katakana_ha}, // パ
    {0x30D2, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_hi}, // ヒ
    {0x30D3, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_hi}, // ビ
    {0x30D4, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_HANDAKUTEN, texture_font_char_jp_katakana_hi}, // ピ
    {0x30D5, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_fu}, // フ
    {0x30D6, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_fu}, // ブ
    {0x30D7, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_HANDAKUTEN, texture_font_char_jp_katakana_fu}, // プ
    {0x30D8, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_he}, // ヘ
    {0x30D9, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_he}, // ベ
    {0x30DA, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_HANDAKUTEN, texture_font_char_jp_katakana_he}, // ペ
    {0x30DB, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ho}, // ホ
    {0x30DC, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_DAKUTEN, texture_font_char_jp_katakana_ho}, // ボ
    {0x30DD, 10, TEXT_FLAG_PACKED | TEXT_DIACRITIC_HANDAKUTEN, texture_font_char_jp_katakana_ho}, // ポ
    {0x30DE, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ma}, // マ
    {0x30DF, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_mi}, // ミ
    {0x30E0, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_mu}, // ム
    {0x30E1, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_me}, // メ
    {0x30E2, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_mo}, // モ
    {0x30E3, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_small_ya}, // ャ
    {0x30E4, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ya}, // ヤ
    {0x30E5, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_small_yu}, // ュ
    {0x30E6, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_yu}, // ユ
    {0x30E7, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_small_yo}, // ョ
    {0x30E8, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_yo}, // ヨ
    {0x30E9, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ra}, // ラ
    {0x30EA, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ri}, // リ
    {0x30EB, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ru}, // ル
    {0x30EC, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_re}, // レ
    {0x30ED, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_ro}, // ロ
    {0x30EF, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_wa}, // ワ
    {0x30F3, 10, TEXT_FLAG_PACKED, texture_font_char_jp_katakana_n}, // ン
    {0x30FB, 10, TEXT_FLAG_PACKED, texture_font_char_jp_interpunct}, // ・
    {0x30FC, 10, TEXT_FLAG_PACKED, texture_font_char_jp_long_vowel}, // ー

    {0xFF01, 10, TEXT_FLAG_PACKED, texture_font_char_jp_exclamation}, // ！
    {0xFF05, 10, TEXT_FLAG_PACKED, texture_font_char_jp_percent}, // ％
    {0xFF08, 10, TEXT_FLAG_PACKED, texture_font_char_jp_open_parentheses}, // （
    {0xFF09, 10, TEXT_FLAG_PACKED, texture_font_char_jp_close_parentheses}, // ）
    {0xFF10, 10, TEXT_FLAG_PACKED, texture_font_char_jp_0}, // ０
    {0xFF11, 10, TEXT_FLAG_PACKED, texture_font_char_jp_1}, // １
    {0xFF12, 10, TEXT_FLAG_PACKED, texture_font_char_jp_2}, // ２
    {0xFF13, 10, TEXT_FLAG_PACKED, texture_font_char_jp_3}, // ３
    {0xFF14, 10, TEXT_FLAG_PACKED, texture_font_char_jp_4}, // ４
    {0xFF15, 10, TEXT_FLAG_PACKED, texture_font_char_jp_5}, // ５
    {0xFF16, 10, TEXT_FLAG_PACKED, texture_font_char_jp_6}, // ６
    {0xFF17, 10, TEXT_FLAG_PACKED, texture_font_char_jp_7}, // ７
    {0xFF18, 10, TEXT_FLAG_PACKED, texture_font_char_jp_8}, // ８
    {0xFF19, 10, TEXT_FLAG_PACKED, texture_font_char_jp_9}, // ９
    {0xFF1F, 10, TEXT_FLAG_PACKED, texture_font_char_jp_question}, // ？
    {0xFF21, 10, TEXT_FLAG_PACKED, texture_font_char_jp_A}, // Ａ
    {0xFF22, 10, TEXT_FLAG_PACKED, texture_font_char_jp_B}, // Ｂ
    {0xFF23, 10, TEXT_FLAG_PACKED, texture_font_char_jp_C}, // Ｃ
    {0xFF24, 10, TEXT_FLAG_PACKED, texture_font_char_jp_D}, // Ｄ
    {0xFF25, 10, TEXT_FLAG_PACKED, texture_font_char_jp_E}, // Ｅ
    {0xFF26, 10, TEXT_FLAG_PACKED, texture_font_char_jp_F}, // Ｆ
    {0xFF27, 10, TEXT_FLAG_PACKED, texture_font_char_jp_G}, // Ｇ
    {0xFF28, 10, TEXT_FLAG_PACKED, texture_font_char_jp_H}, // Ｈ
    {0xFF29, 10, TEXT_FLAG_PACKED, texture_font_char_jp_I}, // Ｉ
    {0xFF2A, 10, TEXT_FLAG_PACKED, texture_font_char_jp_J}, // Ｊ
    {0xFF2B, 10, TEXT_FLAG_PACKED, texture_font_char_jp_K}, // Ｋ
    {0xFF2C, 10, TEXT_FLAG_PACKED, texture_font_char_jp_L}, // Ｌ
    {0xFF2D, 10, TEXT_FLAG_PACKED, texture_font_char_jp_M}, // Ｍ
    {0xFF2E, 10, TEXT_FLAG_PACKED, texture_font_char_jp_N}, // Ｎ
    {0xFF2F, 10, TEXT_FLAG_PACKED, texture_font_char_jp_O}, // Ｏ
    {0xFF30, 10, TEXT_FLAG_PACKED, texture_font_char_jp_P}, // Ｐ
    {0xFF31, 10, TEXT_FLAG_PACKED, texture_font_char_jp_Q}, // Ｑ
    {0xFF32, 10, TEXT_FLAG_PACKED, texture_font_char_jp_R}, // Ｒ
    {0xFF33, 10, TEXT_FLAG_PACKED, texture_font_char_jp_S}, // Ｓ
    {0xFF34, 10, TEXT_FLAG_PACKED, texture_font_char_jp_T}, // Ｔ
    {0xFF35, 10, TEXT_FLAG_PACKED, texture_font_char_jp_U}, // Ｕ
    {0xFF36, 10, TEXT_FLAG_PACKED, texture_font_char_jp_V}, // Ｖ
    {0xFF37, 10, TEXT_FLAG_PACKED, texture_font_char_jp_W}, // Ｗ
    {0xFF38, 10, TEXT_FLAG_PACKED, texture_font_char_jp_X}, // Ｘ
    {0xFF39, 10, TEXT_FLAG_PACKED, texture_font_char_jp_Y}, // Ｙ
    {0xFF3A, 10, TEXT_FLAG_PACKED, texture_font_char_jp_Z}, // Ｚ

    {0xFF58, 10, TEXT_FLAG_PACKED, texture_font_char_jp_multiply}, // ｘ
    {0xFF5E, 10, TEXT_FLAG_PACKED, texture_font_char_jp_tilde}, // ～
#endif
};

const struct Utf8CharLUTEntry main_font_utf8_4byte_lut[] = {

};

const struct Utf8CharLUTEntry main_font_utf8_missing_char = {0, 9, 0, texture_font_missing_character};

const struct Utf8LUT main_font_utf8_lut = {
    main_font_utf8_2byte_lut,
    main_font_utf8_3byte_lut,
    main_font_utf8_4byte_lut,
    ARRAY_COUNT(main_font_utf8_2byte_lut),
    ARRAY_COUNT(main_font_utf8_3byte_lut),
    ARRAY_COUNT(main_font_utf8_4byte_lut),
    &main_font_utf8_missing_char,
};

// ASCII lookup table for the green credits font
const struct AsciiCharLUTEntry main_credits_font_lut[] = {
    {NULL, 4}, // 32 " "
    {NULL, 0}, // 33 "!" (Unimplemented)
    {NULL, 0}, // 34 "\"" (Unimplemented)
    {NULL, 0}, // 35 "#" (Unimplemented)
    {NULL, 0}, // 36 "$" (Unimplemented)
    {NULL, 0}, // 37 "%" (Unimplemented)
    {NULL, 0}, // 38 "&" (Unimplemented)
    {NULL, 0}, // 39 "'" (Unimplemented)
    {NULL, 0}, // 40 "(" (Unimplemented)
    {NULL, 0}, // 41 ")" (Unimplemented)
    {NULL, 0}, // 42 "*" (Unimplemented)
    {NULL, 0}, // 43 "+" (Unimplemented)
    {NULL, 0}, // 44 "," (Unimplemented)
    {NULL, 0}, // 45 "-" (Unimplemented)
    {texture_credits_char_period, 7}, // 46 "."
    {NULL, 0}, // 47 "/" (Unimplemented)
    {NULL, 0}, // 48 "0" (Unimplemented)
    {NULL, 0}, // 49 "1" (Unimplemented)
    {NULL, 0}, // 50 "2" (Unimplemented)
    {texture_credits_char_3, 7}, // 51 "3"
    {texture_credits_char_4, 7}, // 52 "4"
    {NULL, 0}, // 53 "5" (Unimplemented)
    {texture_credits_char_6, 7}, // 54 "6"
    {NULL, 0}, // 55 "7" (Unimplemented)
    {NULL, 0}, // 56 "8" (Unimplemented)
    {NULL, 0}, // 57 "9" (Unimplemented)
    {NULL, 0}, // 58 ":" (Unimplemented)
    {NULL, 0}, // 59 ";" (Unimplemented)
    {NULL, 0}, // 60 "<" (Unimplemented)
    {NULL, 0}, // 61 "=" (Unimplemented)
    {NULL, 0}, // 62 ">" (Unimplemented)
    {NULL, 0}, // 63 "?" (Unimplemented)
    {NULL, 0}, // 64 "@" (Unimplemented)
    {texture_credits_char_A, 7}, // 65 "A"
    {texture_credits_char_B, 7}, // 66 "B"
    {texture_credits_char_C, 7}, // 67 "C"
    {texture_credits_char_D, 7}, // 68 "D"
    {texture_credits_char_E, 7}, // 69 "E"
    {texture_credits_char_F, 7}, // 70 "F"
    {texture_credits_char_G, 7}, // 71 "G"
    {texture_credits_char_H, 7}, // 72 "H"
    {texture_credits_char_I, 7}, // 73 "I"
    {texture_credits_char_J, 7}, // 74 "J"
    {texture_credits_char_K, 7}, // 75 "K"
    {texture_credits_char_L, 7}, // 76 "L"
    {texture_credits_char_M, 7}, // 77 "M"
    {texture_credits_char_N, 7}, // 78 "N"
    {texture_credits_char_O, 7}, // 79 "O"
    {texture_credits_char_P, 7}, // 80 "P"
    {texture_credits_char_Q, 7}, // 81 "Q"
    {texture_credits_char_R, 7}, // 82 "R"
    {texture_credits_char_S, 7}, // 83 "S"
    {texture_credits_char_T, 7}, // 84 "T"
    {texture_credits_char_U, 7}, // 85 "U"
    {texture_credits_char_V, 7}, // 86 "V"
    {texture_credits_char_W, 7}, // 87 "W"
    {texture_credits_char_X, 7}, // 88 "X"
    {texture_credits_char_Y, 7}, // 89 "Y"
    {texture_credits_char_Z, 7}, // 90 "Z"
    {NULL, 0}, // 91 "[" (Unimplemented)
    {NULL, 0}, // 92 "\" (Unimplemented)
    {NULL, 0}, // 93 "]" (Unimplemented)
    {NULL, 0}, // 94 "^" (Unimplemented)
    {NULL, 0}, // 95 "_" (Unimplemented)
    {NULL, 0}, // 96 "`" (Unimplemented)
    {texture_credits_char_A, 7}, // 97 "a"
    {texture_credits_char_B, 7}, // 98 "b"
    {texture_credits_char_C, 7}, // 99 "c"
    {texture_credits_char_D, 7}, // 100 "d"
    {texture_credits_char_E, 7}, // 101 "e"
    {texture_credits_char_F, 7}, // 102 "f"
    {texture_credits_char_G, 7}, // 103 "g"
    {texture_credits_char_H, 7}, // 104 "h"
    {texture_credits_char_I, 7}, // 105 "i"
    {texture_credits_char_J, 7}, // 106 "j"
    {texture_credits_char_K, 7}, // 107 "k"
    {texture_credits_char_L, 7}, // 108 "l"
    {texture_credits_char_M, 7}, // 109 "m"
    {texture_credits_char_N, 7}, // 110 "n"
    {texture_credits_char_O, 7}, // 111 "o"
    {texture_credits_char_P, 7}, // 112 "p"
    {texture_credits_char_Q, 7}, // 113 "q"
    {texture_credits_char_R, 7}, // 114 "r"
    {texture_credits_char_S, 7}, // 115 "s"
    {texture_credits_char_T, 7}, // 116 "t"
    {texture_credits_char_U, 7}, // 117 "u"
    {texture_credits_char_V, 7}, // 118 "v"
    {texture_credits_char_W, 7}, // 119 "w"
    {texture_credits_char_X, 7}, // 120 "x"
    {texture_credits_char_Y, 7}, // 121 "y"
    {texture_credits_char_Z, 7}, // 122 "z"
    {NULL, 0}, // 123 "{" (Unimplemented)
    {NULL, 0}, // 124 "|" (Unimplemented)
    {NULL, 0}, // 125 "}" (Unimplemented)
    {NULL, 0}, // 126 "~" (Unimplemented)
};

// HUD camera table 0x020087CC-0x020087E3
const Texture *const main_hud_camera_lut[] = {
    texture_hud_char_camera, texture_hud_char_mario_head, texture_hud_char_lakitu, texture_hud_char_no_camera,
    texture_hud_char_arrow_up, texture_hud_char_arrow_down,
};

// If not using multilang, include the text data here in segment 0x02.
// Otherwise, it will be placed into segment 0x19.
#ifndef MULTILANG
#include "sounds.h"
#include "seq_ids.h"

#define COURSE_NAME_TABLE seg2_course_name_table
#define ACT_NAME_TABLE seg2_act_name_table
#define DIALOG_TABLE seg2_dialog_table

#define DIALOG_FILE "us/dialogs.h"
#define COURSE_FILE "us/courses.h"
#include "text/define_text.inc.c"
#undef DIALOG_FILE
#undef COURSE_FILE
#endif

// 0x0200EC60 - 0x0200EC98
const Gfx dl_hud_img_begin[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_COPY),
    gsDPSetTexturePersp(G_TP_NONE),
    gsDPSetAlphaCompare(G_AC_THRESHOLD),
    gsDPSetBlendColor(255, 255, 255, 255),
    gsDPSetRenderMode(G_RM_NOOP, G_RM_NOOP2),
    gsDPSetTextureFilter(G_TF_POINT),
    gsSPEndDisplayList(),
};

// 0x0200EC98 - 0x0200ECC8
const Gfx dl_hud_img_load_tex_block[] = {
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, (G_TX_WRAP | G_TX_NOMIRROR), 4, G_TX_NOLOD, (G_TX_WRAP | G_TX_NOMIRROR), 4, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, (G_TX_WRAP | G_TX_NOMIRROR), 4, G_TX_NOLOD, (G_TX_WRAP | G_TX_NOMIRROR), 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, ((16 - 1) << G_TEXTURE_IMAGE_FRAC), ((16 - 1) << G_TEXTURE_IMAGE_FRAC)),
    gsSPEndDisplayList(),
};

// 0x0200ECC8 - 0x0200ED00
const Gfx dl_hud_img_end[] = {
    gsDPPipeSync(),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsSPEndDisplayList(),
};

// 0x0200ED00 - 0x0200ED38
const Gfx dl_rgba16_text_begin[] = {
    gsDPPipeSync(),
    gsDPSetTexturePersp(G_TP_NONE),
    gsDPSetCombineMode(G_CC_FADEA, G_CC_FADEA),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetRenderMode(G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2),
    gsDPSetTextureFilter(G_TF_POINT),
    gsSPEndDisplayList(),
};

// 0x0200ED38 - 0x0200ED68
const Gfx dl_rgba16_load_tex_block[] = {
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, (G_TX_WRAP | G_TX_NOMIRROR), 4, G_TX_NOLOD, (G_TX_WRAP | G_TX_NOMIRROR), 4, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, (G_TX_WRAP | G_TX_NOMIRROR), 4, G_TX_NOLOD, (G_TX_WRAP | G_TX_NOMIRROR), 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, ((16 - 1) << G_TEXTURE_IMAGE_FRAC), ((16 - 1) << G_TEXTURE_IMAGE_FRAC)),
    gsSPEndDisplayList(),
};

// 0x0200ED68 - 0x0200EDA8
const Gfx dl_rgba16_text_end[] = {
    gsDPPipeSync(),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsSPEndDisplayList(),
};

const Gfx dl_shade_screen_begin[] = {
    gsDPPipeSync(),
    gsDPSetRenderMode(G_RM_CLD_SURF, G_RM_CLD_SURF2),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetPrimColor(0, 0, 0, 0, 0, 127),
    gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
    gsSPEndDisplayList(),
};

const Gfx dl_shade_screen_end[] = {
    gsDPPipeSync(),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0200EDA8 - 0x0200EDE8
static const Vtx vertex_text_bg_box[] = {
    {{{     0,    -80,      0}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   130,    -80,      0}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   130,      0,      0}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,      0,      0}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
};

// 0x0200EDE8 - 0x0200EE28
const Gfx dl_draw_text_bg_box[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_FADE, G_CC_FADE),
    gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_XLU_SURF2),
    gsSPVertex(vertex_text_bg_box, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0200EE28 - 0x0200EE68
static const Vtx vertex_ia8_char[] = {
    {{{     0,      0,      0}, 0, {     0,    256}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     8,      0,      0}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     8,     16,      0}, 0, {   512,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,     16,      0}, 0, {   512,    256}, {0xff, 0xff, 0xff, 0xff}}},
};

const Gfx dl_ia_text_begin[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_FADEA, G_CC_FADEA),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_XLU_SURF2),
    gsDPSetTextureFilter(G_TF_POINT),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

// 0x020073E8 - 0x02007418
const Gfx dl_ia_text_tex_settings[] = {
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 3, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 4, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 8 + G_IM_SIZ_4b_INCR) >> G_IM_SIZ_4b_SHIFT) - 1, CALC_DXT(16, G_IM_SIZ_4b_BYTES)),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_4b, 1, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 3, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (8 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPVertex(vertex_ia8_char, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0, 0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

static const Vtx vertex_ia8_char_packed[] = {
    {{{     0,      0,      0}, 0, {     0,   1024}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     8,      0,      0}, 0, {   512,   1024}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     8,     16,      0}, 0, {   512,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,     16,      0}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
};

// 0x0200EEA8 - 0x0200EEF0
const Gfx dl_ia_text_tex_settings_packed[] = {
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON), // gross
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 0, 0, G_TX_LOADTILE, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_CLAMP, 3, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((8 * 16) - 1), CALC_DXT(8, G_IM_SIZ_8b_BYTES)),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 1, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_CLAMP, 3, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, ((8 - 1) << G_TEXTURE_IMAGE_FRAC), ((16 - 1) << G_TEXTURE_IMAGE_FRAC)),
    gsSPVertex(vertex_ia8_char_packed, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0, 0,  2,  3, 0x0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON), // gross
    gsSPEndDisplayList(),
};

// 0x0200EEF0 - 0x0200EF30
const Gfx dl_ia_text_end[] = {
    gsDPPipeSync(),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPSetGeometryMode(G_LIGHTING | G_SHADING_SMOOTH),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsSPEndDisplayList(),
};

// 0x0200EF30 - 0x0200EF60
static const Vtx vertex_triangle[] = {
    {{{     0,      0,      0}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     8,      8,      0}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,     16,      0}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
};

// 0x0200EF60 - 0x0200EFB0
const Gfx dl_draw_triangle[] = {
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_FADE, G_CC_FADE),
    gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_XLU_SURF2),
    gsDPSetTextureFilter(G_TF_POINT),
    gsSPVertex(vertex_triangle, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPSetGeometryMode(G_LIGHTING),
    gsDPPipeSync(),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0200EFB0 - 0x0200EFF0
static const Vtx vertex_billboard_num[] = {
    {{{   -32,    -32,      0}, 0, {     0,  32<<5}, {0xff, 0xff, 0xff, 0xff}}},
    {{{    32,    -32,      0}, 0, { 32<<5,  32<<5}, {0xff, 0xff, 0xff, 0xff}}},
    {{{    32,     32,      0}, 0, { 32<<5,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   -32,     32,      0}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
};

// 0x0200EFF0 - 0x0200F038
const Gfx dl_billboard_num_begin[] = {
    gsDPPipeSync(),
    gsDPSetTextureFilter(G_TF_POINT),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, (G_TX_WRAP | G_TX_NOMIRROR), G_TX_NOMASK, G_TX_NOLOD, (G_TX_WRAP | G_TX_NOMIRROR), G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_CLAMP, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, ((16 - 1) << G_TEXTURE_IMAGE_FRAC), ((16 - 1) << G_TEXTURE_IMAGE_FRAC)),
    gsSPEndDisplayList(),
};

// 0x0200F038 - 0x0200F078
const Gfx dl_billboard_num_end[] = {
    gsSPVertex(vertex_billboard_num, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0200F078 - 0x0200F0A8
const Gfx dl_billboard_num_0[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

// 0x0200F0A8 - 0x0200F0D8
const Gfx dl_billboard_num_1[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_1),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

// 0x0200F0D8 - 0x0200F108
const Gfx dl_billboard_num_2[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_2),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

// 0x0200F108 - 0x0200F138
const Gfx dl_billboard_num_3[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_3),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

// 0x0200F138 - 0x0200F168
const Gfx dl_billboard_num_4[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_4),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

// 0x0200F168 - 0x0200F198
const Gfx dl_billboard_num_5[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_5),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

// 0x0200F198 - 0x0200F1C8
const Gfx dl_billboard_num_6[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_6),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

// 0x0200F1C8 - 0x0200F1F8
const Gfx dl_billboard_num_7[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_7),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

// 0x0200F1F8 - 0x0200F228
const Gfx dl_billboard_num_8[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

// 0x0200F228 - 0x0200F258
const Gfx dl_billboard_num_9[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_9),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

#ifdef DIALOG_INDICATOR
const Gfx dl_billboard_num_A[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_A),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

const Gfx dl_billboard_num_B[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_B),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

const Gfx dl_billboard_num_C[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_C),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

const Gfx dl_billboard_num_D[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_D),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

const Gfx dl_billboard_num_E[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_E),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};

const Gfx dl_billboard_num_F[] = {
    gsSPDisplayList(dl_billboard_num_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_hud_char_F),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((16 * 16) - 1), CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(dl_billboard_num_end),
    gsSPEndDisplayList(),
};
#endif

#ifdef HD_SHADOWS
ALIGNED8 static const Texture texture_shadow_quarter_circle_64[] = {
#include "textures/segment2/shadow_quarter_circle_64.ia8.inc.c"
};

ALIGNED8 static const Texture texture_shadow_quarter_square_64[] = {
#include "textures/segment2/shadow_quarter_square_64.ia8.inc.c"
};
#else
ALIGNED8 static const Texture texture_shadow_quarter_circle[] = {
#include "textures/segment2/shadow_quarter_circle.ia8.inc.c"
};

ALIGNED8 static const Texture texture_shadow_quarter_square[] = {
#include "textures/segment2/shadow_quarter_square.ia8.inc.c"
};
#endif

UNUSED ALIGNED8 static const Texture texture_radial_light[] = {
#include "textures/segment2/light_quarter_circle.ia16.inc.c"
};

const Texture texture_transition_star_half[] = {
#include "textures/segment2/segment2.0F458.ia8.inc.c"
};

const Texture texture_transition_circle_half[] = {
#include "textures/segment2/segment2.0FC58.ia8.inc.c"
};

const Texture texture_transition_mario[] = {
#include "textures/segment2/segment2.10458.ia8.inc.c"
};

const Texture texture_transition_bowser_half[] = {
#include "textures/segment2/segment2.11458.ia8.inc.c"
};

const Texture texture_waterbox_water[] = {
#include "textures/segment2/segment2.11C58.rgba16.inc.c"
};

const Texture texture_waterbox_jrb_water[] = {
#include "textures/segment2/segment2.12458.rgba16.inc.c"
};

const Texture texture_waterbox_unknown_water[] = {
#include "textures/segment2/segment2.12C58.rgba16.inc.c"
};

const Texture texture_waterbox_mist[] = {
#include "textures/segment2/segment2.13458.ia16.inc.c"
};

const Texture texture_waterbox_lava[] = {
#include "textures/segment2/segment2.13C58.rgba16.inc.c"
};

// Unreferenced light group

// 0x02014470 - 0x020144B0
static const Mtx matrix_identity = {
#ifndef GBI_FLOATS
    {{ 0x00010000, 0x00000000, 0x00000001, 0x00000000 },
     { 0x00000000, 0x00010000, 0x00000000, 0x00000001 },
     { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
     { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }}
#else
    {{ 1.0f, 0.0f, 0.0f, 0.0f },
     { 0.0f, 1.0f, 0.0f, 0.0f },
     { 0.0f, 0.0f, 1.0f, 0.0f },
     { 0.0f, 0.0f, 0.0f, 1.0f }}
#endif
};


// 0x020144B0 - 0x020144F0
static const Mtx matrix_fullscreen = {
#ifndef GBI_FLOATS
    // {{                               0x00000000, 0x00000000,                              0x00000000, 0x00000000 },
    //  {                               0x00000000, 0xffff0000,                              0xffffffff, 0xffff0001 },
    //  { (((65536 * 2 / SCREEN_WIDTH) << 16) | 0), 0x00000000, (0 << 16) | (65536 * 2 / SCREEN_HEIGHT), 0x00000000 },
    //  {                               0x00000000, 0x00000000,                              0x00000000, 0x00000000 }}
    {{                      0x00000000, 0x00000000,               0x00000000, 0x00000000 },
     {                      0x00000000, 0xffff0000,               0xffffffff, 0xffff0001 },
     { ((131072 / SCREEN_WIDTH) << 16), 0x00000000, (131072 / SCREEN_HEIGHT), 0x00000000 },
     {                      0x00000000, 0x00000000,               0x00000000, 0x00000000 }}
#else
    {{ (2.0f / SCREEN_WIDTH),                   0.0f,  0.0f, 0.0f },
     {                  0.0f, (2.0f / SCREEN_HEIGHT),  0.0f, 0.0f },
     {                  0.0f,                   0.0f, -1.0f, 0.0f },
     {                 -1.0f,                  -1.0f, -1.0f, 1.0f }}
#endif
};


// 0x020144F0 - 0x02014508
const Gfx dl_draw_quad_verts_0123[] = {
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x02014508 - 0x02014520
const Gfx dl_draw_quad_verts_4567[] = {
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

const Gfx dl_shadow_begin[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetCombineMode(G_CC_MODULATEIFADEA, G_CC_MODULATEIFADEA),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

#ifdef HD_SHADOWS
const Gfx dl_shadow_circle[] = {
    gsSPDisplayList(dl_shadow_begin),
    gsDPLoadTextureBlock(texture_shadow_quarter_circle_64, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0, (G_TX_WRAP | G_TX_MIRROR), (G_TX_WRAP | G_TX_MIRROR), 6, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPEndDisplayList(),
};

const Gfx dl_shadow_square[] = {
    gsSPDisplayList(dl_shadow_begin),
    gsDPLoadTextureBlock(texture_shadow_quarter_square_64, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0, (G_TX_WRAP | G_TX_MIRROR), (G_TX_WRAP | G_TX_MIRROR), 6, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPEndDisplayList(),
};
#else
const Gfx dl_shadow_circle[] = {
    gsSPDisplayList(dl_shadow_begin),
    gsDPLoadTextureBlock(texture_shadow_quarter_circle, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0, (G_TX_WRAP | G_TX_MIRROR), (G_TX_WRAP | G_TX_MIRROR), 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPEndDisplayList(),
};

const Gfx dl_shadow_square[] = {
    gsSPDisplayList(dl_shadow_begin),
    gsDPLoadTextureBlock(texture_shadow_quarter_square, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0, (G_TX_WRAP | G_TX_MIRROR), (G_TX_WRAP | G_TX_MIRROR), 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    gsSPEndDisplayList(),
};
#endif

static const Vtx vertex_shadow[] = {
#ifdef HD_SHADOWS
    {{{    -1,      0,     -1}, 0, { -2048,  -2048}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     1,      0,     -1}, 0, {  2048,  -2048}, {0xff, 0xff, 0xff, 0xff}}},
    {{{    -1,      0,      1}, 0, { -2048,   2048}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     1,      0,      1}, 0, {  2048,   2048}, {0xff, 0xff, 0xff, 0xff}}},
#else
    {{{    -1,      0,     -1}, 0, {  -512,   -512}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     1,      0,     -1}, 0, {   512,   -512}, {0xff, 0xff, 0xff, 0xff}}},
    {{{    -1,      0,      1}, 0, {  -512,    512}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     1,      0,      1}, 0, {   512,    512}, {0xff, 0xff, 0xff, 0xff}}},
#endif
};

// 0x02014638 - 0x02014660
const Gfx dl_shadow_end[] = {
    gsSPVertex(vertex_shadow, 4, 0),
    gsSP2Triangles( 0,  2,  1, 0x0,  1,  2,  3, 0x0),
    gsDPPipeSync(),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x02014660 - 0x02014698
const Gfx dl_proj_mtx_fullscreen[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsSPMatrix(&matrix_identity,   (G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH)),
    gsSPMatrix(&matrix_fullscreen, (G_MTX_PROJECTION | G_MTX_MUL  | G_MTX_NOPUSH)),
    gsSPMatrix(&matrix_identity,   (G_MTX_MODELVIEW  | G_MTX_LOAD | G_MTX_NOPUSH)),
    gsSPPerspNormalize(0xFFFF),
    gsSPEndDisplayList(),
};

// 0x02014698 - 0x020146C0
const Gfx dl_screen_transition_end[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
    gsSPEndDisplayList(),
};

// 0x020146C0 - 0x02014708
const Gfx dl_transition_draw_filled_region[] = {
    gsSP2Triangles( 0,  4,  1, 0x0,  1,  4,  5, 0x0),
    gsSP2Triangles( 1,  5,  2, 0x0,  2,  5,  6, 0x0),
    gsSP2Triangles( 2,  6,  7, 0x0,  2,  7,  3, 0x0),
    gsSP2Triangles( 3,  4,  0, 0x0,  3,  7,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x02014708 - 0x02014738
const Gfx dl_skybox_begin[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPPerspNormalize(0xFFFF),
    gsSPMatrix(&matrix_identity, (G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH)),
    gsSPEndDisplayList(),
};

// 0x02014738 - 0x02014768
const Gfx dl_skybox_tile_tex_settings[] = {
    gsSPMatrix(&matrix_identity, (G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH)),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, ((32 - 1) << G_TEXTURE_IMAGE_FRAC), ((32 - 1) << G_TEXTURE_IMAGE_FRAC)),
    gsSPEndDisplayList(),
};

// 0x02014768 - 0x02014790
const Gfx dl_skybox_end[] = {
    gsDPPipeSync(),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsSPSetGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x02014790 - 0x020147D0
const Gfx dl_waterbox_rgba16_begin[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
#ifdef USE_FRUSTRATIO2
    gsSPClipRatio(FRUSTRATIO_1),
#endif
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, (G_TX_WRAP | G_TX_NOMIRROR), 5, G_TX_NOLOD, (G_TX_WRAP | G_TX_NOMIRROR), 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, ((32 - 1) << G_TEXTURE_IMAGE_FRAC), ((32 - 1) << G_TEXTURE_IMAGE_FRAC)),
    gsSPEndDisplayList(),
};

// 0x020147D0 - 0x02014810
const Gfx dl_waterbox_ia16_begin[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
#ifdef USE_FRUSTRATIO2
    gsSPClipRatio(FRUSTRATIO_1),
#endif
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, (G_TX_WRAP | G_TX_NOMIRROR), 5, G_TX_NOLOD, (G_TX_WRAP | G_TX_NOMIRROR), 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, ((32 - 1) << G_TEXTURE_IMAGE_FRAC), ((32 - 1) << G_TEXTURE_IMAGE_FRAC)),
    gsSPEndDisplayList(),
};

// 0x02014810 - 0x02014838
const Gfx dl_waterbox_end[] = {
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
#ifdef USE_FRUSTRATIO2
    gsSPClipRatio(FRUSTRATIO_2),
#endif
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x02014838 - 0x02014878
ALIGNED8 static const Texture texture_ia8_up_arrow[] = {
#include "textures/segment2/segment2.14838.ia8.inc.c"
};

// 0x02014878 - 0x020148B0
const Gfx dl_ia8_up_arrow_begin[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
    gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_NOOP2),
    gsSPPerspNormalize(0xFFFF),
    gsSPMatrix(&matrix_identity, (G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH)),
    gsSPEndDisplayList(),
};

// 0x020148B0 - 0x020148E0
// Unused, seems to be an early DL for the power meter, seeing that is loading a 64x32 texture
const Gfx dl_rgba16_unused[] = {
    gsSPMatrix(&matrix_identity, (G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH)),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, ((64 - 1) << G_TEXTURE_IMAGE_FRAC), ((32 - 1) << G_TEXTURE_IMAGE_FRAC)),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

// 0x020148E0 - 0x02014938
const Gfx dl_ia8_up_arrow_load_texture_block[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 1, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 3, G_TX_NOLOD, G_TX_CLAMP, 3, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, ((8 - 1) << G_TEXTURE_IMAGE_FRAC), ((8 - 1) << G_TEXTURE_IMAGE_FRAC)),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 1, texture_ia8_up_arrow),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 0, 0, G_TX_LOADTILE, 0, (G_TX_WRAP | G_TX_NOMIRROR), G_TX_NOMASK, G_TX_NOLOD, (G_TX_WRAP | G_TX_NOMIRROR), G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, ((8 * 8) - 1), CALC_DXT(8, G_IM_SIZ_8b_BYTES)),
    gsSPEndDisplayList(),
};

// 0x02014938 - 0x02014958
const Gfx dl_ia8_up_arrow_end[] = {
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x02014958 - 0x02014960

// 0x02014970 - 0x020149A8
const Gfx dl_paintings_rippling_begin[] = {
    gsDPPipeSync(),
#ifdef F3DEX_GBI_3
    gsSPSetGeometryMode(G_LIGHTING | G_LIGHTING_SPECULAR | G_SHADING_SMOOTH),
#else
    gsSPSetGeometryMode(G_LIGHTING | G_SHADING_SMOOTH),
#endif
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x505050ff),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

// 0x020149A8 - 0x020149C8
const Gfx dl_paintings_rippling_end[] = {
#ifdef F3DEX_GBI_3
    gsSPClearGeometryMode(G_LIGHTING_SPECULAR),
#endif
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x020149C8 - 0x02014A00
const Gfx dl_paintings_env_mapped_begin[] = {
    gsDPPipeSync(),
#ifdef F3DEX_GBI_3
    gsSPSetGeometryMode(G_LIGHTING | G_LIGHTING_SPECULAR | G_TEXTURE_GEN),
#else
    gsSPSetGeometryMode(G_LIGHTING | G_TEXTURE_GEN),
#endif
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_DECALRGB),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x505050ff),
    gsSPTexture(0x4000, 0x4000, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

// 0x02014A00 - 0x02014A30
const Gfx dl_paintings_env_mapped_end[] = {
    gsSPTexture(0x4000, 0x4000, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
#ifdef F3DEX_GBI_3
    gsSPGeometryModeSetFirst(G_TEXTURE_GEN | G_LIGHTING_SPECULAR, G_LIGHTING),
#else
    gsSPGeometryModeSetFirst(G_TEXTURE_GEN, G_LIGHTING),
#endif
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x02014A30 - 0x02014A60
const Gfx dl_paintings_draw_ripples[] = {
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle( 12, 13, 14, 0x0),
    gsSPEndDisplayList(),
};

// 14A60: triangle mesh
// 0x02014A60
const s16 seg2_painting_triangle_mesh[] = {
    157, // numVtx
    // format:
    // 2D point (x, y), ripple (0 or 1)
    614, 583,   0, // 0
    614, 614,   0, // 1
    562, 614,   0, // 2
    562, 553,   1, // 3
    614, 522,   0, // 4
    511, 583,   1, // 5
    511, 614,   0, // 6
    307, 614,   0, // 7
    307, 583,   1, // 8
    358, 614,   0, // 9
    256, 614,   0, // 10
    256, 553,   1, // 11
    307, 522,   1, // 12
    358, 553,   1, // 13
    409, 583,   1, // 14
    460, 614,   0, // 15
    511, 522,   1, // 16
    460, 553,   1, // 17
    409, 522,   1, // 18
    562, 307,   1, // 19
    614, 338,   0, // 20
    562, 430,   1, // 21
    614, 399,   0, // 22
    562, 368,   1, // 23
    511, 338,   1, // 24
    460, 307,   1, // 25
    460, 430,   1, // 26
    511, 399,   1, // 27
    511, 460,   1, // 28
    409, 338,   1, // 29
    460, 368,   1, // 30
    358, 307,   1, // 31
    409, 460,   1, // 32
    358, 430,   1, // 33
    409, 399,   1, // 34
    358, 368,   1, // 35
    307, 338,   1, // 36
    256, 307,   1, // 37
    307, 399,   1, // 38
    256, 430,   1, // 39
    307, 460,   1, // 40
    614, 460,   0, // 41
    562, 491,   1, // 42
    460, 491,   1, // 43
    358, 491,   1, // 44
    256, 491,   1, // 45
    409, 276,   1, // 46
    511, 276,   1, // 47
    307, 276,   1, // 48
    614,  31,   0, // 49
    614,   0,   0, // 50
    562,   0,   0, // 51
    562, 123,   1, // 52
    614,  92,   0, // 53
    511,  31,   1, // 54
    562,  61,   1, // 55
    460,   0,   0, // 56
    511,   0,   0, // 57
    460, 123,   1, // 58
    511,  92,   1, // 59
    511, 153,   1, // 60
    409,  31,   1, // 61
    460,  61,   1, // 62
    358,   0,   0, // 63
    409,   0,   0, // 64
    409,  92,   1, // 65
    358, 123,   1, // 66
    409, 153,   1, // 67
    307,  31,   1, // 68
    358,  61,   1, // 69
    256,   0,   0, // 70
    307,   0,   0, // 71
    256, 123,   1, // 72
    307,  92,   1, // 73
    307, 153,   1, // 74
    614, 153,   0, // 75
    562, 246,   1, // 76
    614, 215,   0, // 77
    562, 184,   1, // 78
    460, 246,   1, // 79
    511, 215,   1, // 80
    460, 184,   1, // 81
    358, 246,   1, // 82
    409, 215,   1, // 83
    358, 184,   1, // 84
    256, 246,   1, // 85
    307, 215,   1, // 86
    205, 583,   1, // 87
      0, 614,   0, // 88
      0, 583,   0, // 89
     51, 614,   0, // 90
     51, 553,   1, // 91
    102, 583,   1, // 92
    205, 522,   1, // 93
    153, 553,   1, // 94
    153, 614,   0, // 95
    102, 522,   1, // 96
    256, 368,   1, // 97
    205, 338,   1, // 98
    153, 307,   1, // 99
    153, 430,   1, // 100
    205, 399,   1, // 101
    205, 460,   1, // 102
    153, 368,   1, // 103
    102, 338,   1, // 104
     51, 307,   1, // 105
     51, 430,   1, // 106
    102, 399,   1, // 107
    102, 460,   1, // 108
     51, 368,   1, // 109
      0, 338,   0, // 110
      0, 460,   0, // 111
    153, 491,   1, // 112
     51, 491,   1, // 113
    153, 246,   1, // 114
    102, 276,   1, // 115
    205, 276,   1, // 116
      0, 276,   0, // 117
     51, 246,   1, // 118
    205,  31,   1, // 119
    256,  61,   1, // 120
    205,   0,   0, // 121
    153,   0,   0, // 122
    205, 153,   1, // 123
    205,  92,   1, // 124
    153, 123,   1, // 125
    102,  31,   1, // 126
    153,  61,   1, // 127
    102,   0,   0, // 128
     51,   0,   0, // 129
     51, 123,   1, // 130
    102,  92,   1, // 131
    102, 153,   1, // 132
      0,  31,   0, // 133
     51,  61,   1, // 134
      0, 153,   0, // 135
    256, 184,   1, // 136
    205, 215,   1, // 137
    153, 184,   1, // 138
    102, 215,   1, // 139
     51, 184,   1, // 140
    409, 614,   0, // 141
    614, 307,   0, // 142
    614, 276,   0, // 143
    511, 307,   1, // 144
    409, 307,   1, // 145
    307, 307,   1, // 146
    205, 614,   0, // 147
      0, 522,   0, // 148
    102, 614,   0, // 149
    205, 307,   1, // 150
    102, 307,   1, // 151
      0, 399,   0, // 152
      0, 307,   0, // 153
      0, 215,   0, // 154
      0,  92,   0, // 155
      0,   0,   0, // 156
    // triangles
    264,
      8,  12,  13, // 0
      0,   1,   2, // 1
      3,   0,   2, // 2
      4,   0,   3, // 3
      5,   2,   6, // 4
      2,   5,   3, // 5
      7,   8,   9, // 6
      8,   7,  10, // 7
     11,   8,  10, // 8
     12,   8,  11, // 9
      9,   8,  13, // 10
     13,  14,   9, // 11
     14, 141,   9, // 12
      5,   6,  15, // 13
      5,  16,   3, // 14
     16,   5,  17, // 15
     17,   5,  15, // 16
     14,  15, 141, // 17
     15,  14,  17, // 18
     18,  14,  13, // 19
     14,  18,  17, // 20
     19, 142,  20, // 21
     19,  20,  23, // 22
     28,  27,  21, // 23
     21,  23,  22, // 24
     22,  41,  21, // 25
     20,  22,  23, // 26
     23,  24,  19, // 27
     21,  27,  23, // 28
     24,  23,  27, // 29
     25, 144,  24, // 30
     19,  24, 144, // 31
     24,  27,  30, // 32
     25,  24,  30, // 33
     26,  30,  27, // 34
     27,  28,  26, // 35
     36,  38,  97, // 36
     26,  34,  30, // 37
     29,  30,  34, // 38
     30,  29,  25, // 39
     25,  29, 145, // 40
     31, 145,  29, // 41
     31,  29,  35, // 42
     29,  34,  35, // 43
     32,  34,  26, // 44
     33,  35,  34, // 45
     34,  32,  33, // 46
     33,  38,  35, // 47
     35,  36,  31, // 48
     36,  35,  38, // 49
     37,  36,  97, // 50
     37, 146,  36, // 51
     31,  36, 146, // 52
     28,  16,  43, // 53
     38,  40,  39, // 54
     39,  97,  38, // 55
     40,  38,  33, // 56
     21,  41,  42, // 57
     41,   4,  42, // 58
      3,  42,   4, // 59
     42,  28,  21, // 60
     28,  42,  16, // 61
      3,  16,  42, // 62
     26,  28,  43, // 63
     17,  43,  16, // 64
     43,  32,  26, // 65
     32,  43,  18, // 66
     17,  18,  43, // 67
     33,  32,  44, // 68
     32,  18,  44, // 69
     13,  44,  18, // 70
     44,  40,  33, // 71
     13,  12,  44, // 72
     40,  44,  12, // 73
     39,  40,  45, // 74
     40,  12,  45, // 75
     48,  31, 146, // 76
     11,  45,  12, // 77
     25,  47, 144, // 78
     46,  25, 145, // 79
     47,  19, 144, // 80
     19, 143, 142, // 81
     31,  46, 145, // 82
     60,  59,  52, // 83
     49,  53,  55, // 84
     50,  49,  51, // 85
     51,  49,  55, // 86
     52,  55,  53, // 87
     53,  75,  52, // 88
     54,  55,  59, // 89
     52,  59,  55, // 90
     55,  54,  51, // 91
     54,  59,  62, // 92
     56,  54,  62, // 93
     57,  54,  56, // 94
     54,  57,  51, // 95
     58,  62,  59, // 96
     59,  60,  58, // 97
     68,  71,  63, // 98
     61,  62,  65, // 99
     58,  65,  62, // 100
     62,  61,  56, // 101
     61,  65,  69, // 102
     63,  61,  69, // 103
     64,  61,  63, // 104
     61,  64,  56, // 105
     65,  67,  66, // 106
     66,  69,  65, // 107
     67,  65,  58, // 108
     68,  69,  73, // 109
     69,  68,  63, // 110
     66,  73,  69, // 111
     68,  73, 120, // 112
     70,  68, 120, // 113
     71,  68,  70, // 114
     72, 120,  73, // 115
     73,  74,  72, // 116
     74,  73,  66, // 117
     75,  77,  78, // 118
     52,  75,  78, // 119
     76,  78,  77, // 120
     77, 143,  76, // 121
     76,  80,  78, // 122
     60,  78,  80, // 123
     78,  60,  52, // 124
     46,  83,  79, // 125
     58,  60,  81, // 126
     60,  80,  81, // 127
     79,  81,  80, // 128
     80,  47,  79, // 129
     47,  80,  76, // 130
     81,  67,  58, // 131
     67,  81,  83, // 132
     79,  83,  81, // 133
     66,  67,  84, // 134
     67,  83,  84, // 135
     82,  84,  83, // 136
     83,  46,  82, // 137
     84,  74,  66, // 138
     82,  86,  84, // 139
     74,  84,  86, // 140
     74,  86, 136, // 141
     72,  74, 136, // 142
     85, 136,  86, // 143
     86,  48,  85, // 144
     48,  86,  82, // 145
     25,  46,  79, // 146
     79,  47,  25, // 147
     82,  46,  31, // 148
     19,  47,  76, // 149
     76, 143,  19, // 150
     31,  48,  82, // 151
     37,  48, 146, // 152
     85,  48,  37, // 153
     10,  87,  11, // 154
     87,  10, 147, // 155
     92,  95, 149, // 156
     88,  89,  90, // 157
     89, 148,  91, // 158
     90,  89,  91, // 159
     91,  92,  90, // 160
     92, 149,  90, // 161
     93,  87,  94, // 162
     87,  93,  11, // 163
     94,  87,  95, // 164
     87, 147,  95, // 165
     95,  92,  94, // 166
     96,  92,  91, // 167
     92,  96,  94, // 168
     39, 101,  97, // 169
     97,  98,  37, // 170
     98,  97, 101, // 171
     99,  98, 103, // 172
     99, 150,  98, // 173
     37,  98, 150, // 174
     98, 101, 103, // 175
    100, 103, 101, // 176
    101, 102, 100, // 177
    102, 101,  39, // 178
    100, 107, 103, // 179
    103, 104,  99, // 180
    104, 103, 107, // 181
    105, 104, 109, // 182
    105, 151, 104, // 183
     99, 104, 151, // 184
    104, 107, 109, // 185
    106, 109, 107, // 186
    107, 108, 106, // 187
    108, 107, 100, // 188
    109, 110, 105, // 189
    106, 152, 109, // 190
    110, 109, 152, // 191
    105, 110, 153, // 192
    111, 152, 106, // 193
     11,  93,  45, // 194
    102,  45,  93, // 195
     45, 102,  39, // 196
    102,  93, 112, // 197
    100, 102, 112, // 198
     94, 112,  93, // 199
    112, 108, 100, // 200
    108, 112,  96, // 201
     94,  96, 112, // 202
    106, 108, 113, // 203
    108,  96, 113, // 204
     91, 113,  96, // 205
     91, 148, 113, // 206
    113, 111, 106, // 207
    111, 113, 148, // 208
    114, 116,  99, // 209
     99, 115, 114, // 210
    115,  99, 151, // 211
     99, 116, 150, // 212
     72, 124, 120, // 213
    116,  37, 150, // 214
     37, 116,  85, // 215
    117, 105, 153, // 216
    105, 115, 151, // 217
    105, 117, 118, // 218
    118, 115, 105, // 219
    119, 120, 124, // 220
    120, 119,  70, // 221
    119, 124, 127, // 222
    119, 121,  70, // 223
    121, 119, 122, // 224
    122, 119, 127, // 225
    123, 124,  72, // 226
    124, 123, 125, // 227
    125, 127, 124, // 228
    126, 127, 131, // 229
    127, 126, 122, // 230
    125, 131, 127, // 231
    126, 131, 134, // 232
    128, 126, 129, // 233
    129, 126, 134, // 234
    126, 128, 122, // 235
    136, 123,  72, // 236
    130, 134, 131, // 237
    131, 132, 130, // 238
    132, 131, 125, // 239
    133, 134, 155, // 240
    134, 133, 129, // 241
    130, 155, 134, // 242
    133, 156, 129, // 243
    135, 155, 130, // 244
    123, 136, 137, // 245
     85, 137, 136, // 246
    139, 115, 118, // 247
    123, 137, 138, // 248
    125, 123, 138, // 249
    114, 138, 137, // 250
    137, 116, 114, // 251
    116, 137,  85, // 252
    114, 139, 138, // 253
    132, 138, 139, // 254
    138, 132, 125, // 255
    132, 139, 140, // 256
    130, 132, 140, // 257
    115, 139, 114, // 258
    118, 140, 139, // 259
    135, 140, 154, // 260
    118, 154, 140, // 261
    140, 135, 130, // 262
    117, 154, 118, // 263
};

/* 0x02015444: seg2_painting_mesh_neighbor_tris
 * Lists the neighboring triangles for each vertex in the mesh.
 * Used when applying gouraud shading to the generated ripple mesh
 *
 * Format:
 *      num neighbors, neighbor0, neighbor1, ...
 * The nth entry corresponds to the nth vertex in seg2_painting_triangle_mesh
 */
const s16 seg2_painting_mesh_neighbor_tris[] = {
      3,   1,   2,   3,
      1,   1,
      4,   1,   2,   4,   5,
      6,   2,   3,   5,  14,  59,  62,
      3,   3,  58,  59,
      6,   4,   5,  13,  14,  15,  16,
      2,   4,  13,
      2,   6,   7,
      6,   0,   6,   7,   8,   9,  10,
      4,   6,  10,  11,  12,
      4,   7,   8, 154, 155,
      6,   8,   9,  77, 154, 163, 194,
      6,   0,   9,  72,  73,  75,  77,
      6,   0,  10,  11,  19,  70,  72,
      6,  11,  12,  17,  18,  19,  20,
      4,  13,  16,  17,  18,
      6,  14,  15,  53,  61,  62,  64,
      6,  15,  16,  18,  20,  64,  67,
      6,  19,  20,  66,  67,  69,  70,
      8,  21,  22,  27,  31,  80,  81, 149, 150,
      3,  21,  22,  26,
      6,  23,  24,  25,  28,  57,  60,
      3,  24,  25,  26,
      6,  22,  24,  26,  27,  28,  29,
      6,  27,  29,  30,  31,  32,  33,
      8,  30,  33,  39,  40,  78,  79, 146, 147,
      6,  34,  35,  37,  44,  63,  65,
      6,  23,  28,  29,  32,  34,  35,
      6,  23,  35,  53,  60,  61,  63,
      6,  38,  39,  40,  41,  42,  43,
      6,  32,  33,  34,  37,  38,  39,
      8,  41,  42,  48,  52,  76,  82, 148, 151,
      6,  44,  46,  65,  66,  68,  69,
      6,  45,  46,  47,  56,  68,  71,
      6,  37,  38,  43,  44,  45,  46,
      6,  42,  43,  45,  47,  48,  49,
      6,  36,  48,  49,  50,  51,  52,
      8,  50,  51, 152, 153, 170, 174, 214, 215,
      6,  36,  47,  49,  54,  55,  56,
      6,  54,  55,  74, 169, 178, 196,
      6,  54,  56,  71,  73,  74,  75,
      3,  25,  57,  58,
      6,  57,  58,  59,  60,  61,  62,
      6,  53,  63,  64,  65,  66,  67,
      6,  68,  69,  70,  71,  72,  73,
      6,  74,  75,  77, 194, 195, 196,
      6,  79,  82, 125, 137, 146, 148,
      6,  78,  80, 129, 130, 147, 149,
      6,  76, 144, 145, 151, 152, 153,
      3,  84,  85,  86,
      1,  85,
      4,  85,  86,  91,  95,
      6,  83,  87,  88,  90, 119, 124,
      3,  84,  87,  88,
      6,  89,  91,  92,  93,  94,  95,
      6,  84,  86,  87,  89,  90,  91,
      4,  93,  94, 101, 105,
      2,  94,  95,
      6,  96,  97, 100, 108, 126, 131,
      6,  83,  89,  90,  92,  96,  97,
      6,  83,  97, 123, 124, 126, 127,
      6,  99, 101, 102, 103, 104, 105,
      6,  92,  93,  96,  99, 100, 101,
      4,  98, 103, 104, 110,
      2, 104, 105,
      6,  99, 100, 102, 106, 107, 108,
      6, 106, 107, 111, 117, 134, 138,
      6, 106, 108, 131, 132, 134, 135,
      6,  98, 109, 110, 112, 113, 114,
      6, 102, 103, 107, 109, 110, 111,
      4, 113, 114, 221, 223,
      2,  98, 114,
      6, 115, 116, 142, 213, 226, 236,
      6, 109, 111, 112, 115, 116, 117,
      6, 116, 117, 138, 140, 141, 142,
      3,  88, 118, 119,
      6, 120, 121, 122, 130, 149, 150,
      3, 118, 120, 121,
      6, 118, 119, 120, 122, 123, 124,
      6, 125, 128, 129, 133, 146, 147,
      6, 122, 123, 127, 128, 129, 130,
      6, 126, 127, 128, 131, 132, 133,
      6, 136, 137, 139, 145, 148, 151,
      6, 125, 132, 133, 135, 136, 137,
      6, 134, 135, 136, 138, 139, 140,
      6, 143, 144, 153, 215, 246, 252,
      6, 139, 140, 141, 143, 144, 145,
      6, 154, 155, 162, 163, 164, 165,
      1, 157,
      3, 157, 158, 159,
      4, 157, 159, 160, 161,
      6, 158, 159, 160, 167, 205, 206,
      6, 156, 160, 161, 166, 167, 168,
      6, 162, 163, 194, 195, 197, 199,
      6, 162, 164, 166, 168, 199, 202,
      4, 156, 164, 165, 166,
      6, 167, 168, 201, 202, 204, 205,
      6,  36,  50,  55, 169, 170, 171,
      6, 170, 171, 172, 173, 174, 175,
      8, 172, 173, 180, 184, 209, 210, 211, 212,
      6, 176, 177, 179, 188, 198, 200,
      6, 169, 171, 175, 176, 177, 178,
      6, 177, 178, 195, 196, 197, 198,
      6, 172, 175, 176, 179, 180, 181,
      6, 180, 181, 182, 183, 184, 185,
      8, 182, 183, 189, 192, 216, 217, 218, 219,
      6, 186, 187, 190, 193, 203, 207,
      6, 179, 181, 185, 186, 187, 188,
      6, 187, 188, 200, 201, 203, 204,
      6, 182, 185, 186, 189, 190, 191,
      3, 189, 191, 192,
      3, 193, 207, 208,
      6, 197, 198, 199, 200, 201, 202,
      6, 203, 204, 205, 206, 207, 208,
      6, 209, 210, 250, 251, 253, 258,
      6, 210, 211, 217, 219, 247, 258,
      6, 209, 212, 214, 215, 251, 252,
      3, 216, 218, 263,
      6, 218, 219, 247, 259, 261, 263,
      6, 220, 221, 222, 223, 224, 225,
      6, 112, 113, 115, 213, 220, 221,
      2, 223, 224,
      4, 224, 225, 230, 235,
      6, 226, 227, 236, 245, 248, 249,
      6, 213, 220, 222, 226, 227, 228,
      6, 227, 228, 231, 239, 249, 255,
      6, 229, 230, 232, 233, 234, 235,
      6, 222, 225, 228, 229, 230, 231,
      2, 233, 235,
      4, 233, 234, 241, 243,
      6, 237, 238, 242, 244, 257, 262,
      6, 229, 231, 232, 237, 238, 239,
      6, 238, 239, 254, 255, 256, 257,
      3, 240, 241, 243,
      6, 232, 234, 237, 240, 241, 242,
      3, 244, 260, 262,
      6, 141, 142, 143, 236, 245, 246,
      6, 245, 246, 248, 250, 251, 252,
      6, 248, 249, 250, 253, 254, 255,
      6, 247, 253, 254, 256, 258, 259,
      6, 256, 257, 259, 260, 261, 262,
      2,  12,  17,
      2,  21,  81,
      3,  81, 121, 150,
      4,  30,  31,  78,  80,
      4,  40,  41,  79,  82,
      4,  51,  52,  76, 152,
      2, 155, 165,
      3, 158, 206, 208,
      2, 156, 161,
      4, 173, 174, 212, 214,
      4, 183, 184, 211, 217,
      3, 190, 191, 193,
      2, 192, 216,
      3, 260, 261, 263,
      3, 240, 242, 244,
      1, 243,
};
