#pragma once

#include <ultra64.h>

#include "types.h"


enum CrashScreenPages {
    FIRST_PAGE,
    PAGE_CONTEXT = FIRST_PAGE,
    PAGE_LOG,
    PAGE_STACK_TRACE,
#ifdef INCLUDE_DEBUG_MAP
    PAGE_MAP_VIEWER,
#endif
    PAGE_RAM_VIEWER,
    PAGE_DISASM,
    PAGE_SETTINGS,
    NUM_PAGES,
    MAX_PAGES = 255U,
};

typedef struct CSPage {
    /*0x00*/ const char* name;
    /*0x04*/ void (*initFunc)(void);
    /*0x08*/ void (*drawFunc)(void);
    /*0x0C*/ void (*inputFunc)(void);
    /*0x10*/ const enum ControlTypes* contList;
    /*0x14*/ struct PACKED {
                /*0x00*/ u32             : 29;
                /*0x03*/ u32 printName   :  1;
                /*0x03*/ u32 crashed     :  1;
                /*0x03*/ u32 initialized :  1;
            } flags; /*0x04*/
} CSPage; /*0x18*/


extern CSPage gCSPages[NUM_PAGES];
extern enum CrashScreenPages gCSPageID;


void crash_screen_set_page(enum CrashScreenPages page);
