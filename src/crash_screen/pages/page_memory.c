#include <ultra64.h>

#include "types.h"
#include "sm64.h"

#include "crash_screen/util/memory_read.h"
#include "crash_screen/cs_controls.h"
#include "crash_screen/cs_draw.h"
#include "crash_screen/cs_main.h"
#include "crash_screen/cs_pages.h"
#include "crash_screen/cs_print.h"
#include "crash_screen/cs_settings.h"

#include "crash_screen/popups/popup_address.h"

#include "page_memory.h"

#ifdef UNF
#include "usb/usb.h"
#include "usb/debug.h"
#endif // UNF


enum MemoryDisplayModes {
    MEMORY_MODE_HEX,
    MEMORY_MODE_ASCII,
    MEMORY_MODE_BINARY,
    MEMORY_MODE_RGBA16,
    MEMORY_MODE_RGBA32,
};

const char* gValNames_mem_disp_mode[] = {
    [MEMORY_MODE_HEX   ] = "HEX",
    [MEMORY_MODE_ASCII ] = "ASCII",
    [MEMORY_MODE_BINARY] = "BINARY",
    [MEMORY_MODE_RGBA16] = "RGBA16",
    [MEMORY_MODE_RGBA32] = "RGBA32",
};


struct CSSetting cs_settings_group_page_memory[] = {
    [CS_OPT_HEADER_PAGE_MEMORY      ] = { .type = CS_OPT_TYPE_HEADER,  .name = "MEMORY",                         .valNames = &gValNames_bool,          .val = SECTION_EXPANDED_DEFAULT,  .defaultVal = SECTION_EXPANDED_DEFAULT,  .lowerBound = FALSE,                 .upperBound = TRUE,                       },
    [CS_OPT_MEMORY_SHOW_RANGE       ] = { .type = CS_OPT_TYPE_SETTING, .name = "Show current address range",     .valNames = &gValNames_bool,          .val = TRUE,                      .defaultVal = TRUE,                      .lowerBound = FALSE,                 .upperBound = TRUE,                       },
#ifdef INCLUDE_DEBUG_MAP
    [CS_OPT_MEMORY_SYMBOL_DIVIDERS  ] = { .type = CS_OPT_TYPE_SETTING, .name = "Show symbol dividers",           .valNames = &gValNames_bool,          .val = TRUE,                      .defaultVal = TRUE,                      .lowerBound = FALSE,                 .upperBound = TRUE,                       },
    [CS_OPT_MEMORY_SHOW_SYMBOL      ] = { .type = CS_OPT_TYPE_SETTING, .name = "Show current symbol name",       .valNames = &gValNames_bool,          .val = TRUE,                      .defaultVal = TRUE,                      .lowerBound = FALSE,                 .upperBound = TRUE,                       },
#endif // INCLUDE_DEBUG_MAP
    [CS_OPT_MEMORY_DISPLAY_MODE     ] = { .type = CS_OPT_TYPE_SETTING, .name = "Display mode",                   .valNames = &gValNames_mem_disp_mode, .val = MEMORY_MODE_HEX,           .defaultVal = MEMORY_MODE_HEX,           .lowerBound = MEMORY_MODE_HEX,       .upperBound = MEMORY_MODE_RGBA32,         },
    [CS_OPT_END_MEMORY              ] = { .type = CS_OPT_TYPE_END, },
};


const enum ControlTypes cs_cont_list_memory[] = {
    CONT_DESC_SWITCH_PAGE,
    CONT_DESC_PAGE_SELECT,
    CONT_DESC_SHOW_CONTROLS,
    CONT_DESC_HIDE_CRASH_SCREEN,
#ifdef UNF
    CONT_DESC_OS_PRINT,
#endif // UNF
    CONT_DESC_CURSOR,
    CONT_DESC_JUMP_TO_ADDRESS,
    CONT_DESC_TOGGLE_ASCII,
    CONT_DESC_LIST_END,
};


#define MEMORY_NUM_SHOWN_ROWS 20


static Address sRamViewViewportIndex = 0x00000000;
static u32 sRamViewNumShownRows = MEMORY_NUM_SHOWN_ROWS;

static const char gHex[0x10] = "0123456789ABCDEF";
#ifdef UNF
static u32 sMemoryViewData[MEMORY_NUM_SHOWN_ROWS][4];
#endif // UNF


void page_memory_init(void) {
    sRamViewViewportIndex = gSelectedAddress;
}

void cs_memory_draw_byte() {

}

void ram_viewer_print_data(u32 line, Address startAddr) {
    const enum MemoryDisplayModes mode = cs_get_setting_val(CS_OPT_GROUP_PAGE_MEMORY, CS_OPT_MEMORY_DISPLAY_MODE);
    __OSThreadContext* tc = &gInspectThread->context;
    CSScreenCoord_u32 charX = (TEXT_X(SIZEOF_HEX(Address)) + 3);
    CSScreenCoord_u32 charY = TEXT_Y(line);

#ifdef UNF
    bzero(&sMemoryViewData, sizeof(sMemoryViewData));
#endif // UNF

    for (CSTextCoord_u32 y = 0; y < sRamViewNumShownRows; y++) {
        Address rowAddr = (startAddr + (y * PAGE_MEMORY_STEP));

        // Row header:
        // "[XXXXXXXX]"
        cs_print(TEXT_X(0), TEXT_Y(line + y), (STR_COLOR_PREFIX STR_HEX_WORD),
            ((y % 2) ? COLOR_RGBA32_CRASH_MEMORY_ROW1 : COLOR_RGBA32_CRASH_MEMORY_ROW2), rowAddr
        );

        charX = (TEXT_X(SIZEOF_HEX(Word)) + 3);
        charY = TEXT_Y(line + y);

        _Bool isColor = ((mode == MEMORY_MODE_RGBA16) || (mode == MEMORY_MODE_RGBA32));
        // if (!isColor) {
        //     // RGBA32 highlightColor = is_in_code_segment(rowAddr) ? RGBA32_SET_ALPHA(COLOR_RGBA32_CRASH_FUNCTION_NAME, 0x3F) : RGBA32_SET_ALPHA(COLOR_RGBA32_CRASH_VARIABLE, 0x3F);
        //     RGBA32 highlightColor = is_in_code_segment(rowAddr) ? COLOR_RGBA32_CRASH_FUNCTION_NAME : COLOR_RGBA32_CRASH_VARIABLE;
        //     cs_draw_rect(charX, (charY - 2), (TEXT_WIDTH(CRASH_SCREEN_NUM_CHARS_X - 9) + 5), TEXT_HEIGHT(1), highlightColor);
        // }

        for (u32 wordOffset = 0; wordOffset < 4; wordOffset++) {
            Word_4Bytes data = {
                .word = 0x00000000,
            };
            Address currAddrAligned = (rowAddr + (wordOffset * sizeof(Word)));
            _Bool valid = try_read_word_aligned(&data.word, currAddrAligned);

#ifdef UNF
            if (valid) {
                sMemoryViewData[y][wordOffset] = data.word;
            }
#endif // UNF

            charX += 2;

            for (u32 byteOffset = 0; byteOffset < sizeof(Word); byteOffset++) {
                Address currAddr = (currAddrAligned + byteOffset);

                RGBA32 textColor = (((mode == MEMORY_MODE_ASCII) || (byteOffset % 2)) ? COLOR_RGBA32_CRASH_MEMORY_DATA1 : COLOR_RGBA32_CRASH_MEMORY_DATA2);
                RGBA32 selectColor = COLOR_RGBA32_NONE;

                if (currAddr == gSelectedAddress) {
                    selectColor = COLOR_RGBA32_CRASH_MEMORY_SELECT;
                    textColor = RGBA32_INVERT(textColor);
                } else if (currAddr == GET_EPC(tc)) {
                    selectColor = COLOR_RGBA32_CRASH_MEMORY_PC;
                }
                _Bool selected = (selectColor != COLOR_RGBA32_NONE);

#ifdef INCLUDE_DEBUG_MAP
                // Draw symbol separator lines:
                if (currAddr >= sizeof(Byte)) {
                    const MapSymbol *currSymbol = get_map_symbol(currAddr, SYMBOL_SEARCH_BINARY);
                    // const RGBA32 dividerColor = COLOR_RGBA32_GRAY;//  is_in_code_segment(currAddr) ? RGBA32_SET_ALPHA(COLOR_RGBA32_CRASH_FUNCTION_NAME, 0x7F) : RGBA32_SET_ALPHA(COLOR_RGBA32_CRASH_VARIABLE, 0x7F);
                    const RGBA32 dividerColor = is_in_code_segment(currAddr) ? COLOR_RGBA32_CRASH_FUNCTION_NAME : COLOR_RGBA32_CRASH_VARIABLE;
                    _Bool aligned0 = ((currAddr & (sizeof(Word) - 1)) == 0); // (byteOffset == 0);
                    _Bool aligned3 = ((currAddr & (sizeof(Word) - 1)) == (sizeof(Word) - 1)); // (byteOffset == (sizeof(Word) - 1));
                    // Prev byte:
                    const MapSymbol *otherSymbol = get_map_symbol((currAddr - sizeof(Byte)), SYMBOL_SEARCH_BINARY);
                    if (currSymbol != otherSymbol) {
                        cs_draw_rect(((charX - 2) - aligned0), (charY - 2), 2, (TEXT_HEIGHT(1) + 1), dividerColor);
                        if ((y != 0) && (wordOffset == 0) && aligned0) {
                            cs_draw_rect((CRASH_SCREEN_X2 - 2), ((charY - TEXT_HEIGHT(1)) - 2), 1, (TEXT_HEIGHT(1) + 1), dividerColor);
                        }
                    }
                    // Prev row:
                    if (currAddr >= PAGE_MEMORY_STEP) {
                        otherSymbol = get_map_symbol((currAddr - PAGE_MEMORY_STEP), SYMBOL_SEARCH_BINARY);
                        if (currSymbol != otherSymbol) {
                            cs_draw_rect(((charX - 2) - aligned0), (charY - 2), (TEXT_WIDTH(2) + 1 + aligned0 + aligned3), 1, dividerColor);
                        }
                    }
                }
#endif // INCLUDE_DEBUG_MAP

                // If the display mode isn't a color, draw a solid box behind the data.
                if (selected && !isColor) {
                    cs_draw_rect((charX - 1), (charY - 1), (TEXT_WIDTH(2) + 1), (TEXT_HEIGHT(1) - 1), selectColor);
                }

                if (valid) {
                    Byte byte = data.byte[byteOffset];
                    switch (mode) {
                        case MEMORY_MODE_HEX:
                            cs_draw_glyph((charX + TEXT_WIDTH(0)), charY, gHex[byte >> BITS_PER_HEX], textColor);
                            cs_draw_glyph((charX + TEXT_WIDTH(1)), charY, gHex[byte & BITMASK(BITS_PER_HEX)], textColor);
                            break;
                        case MEMORY_MODE_ASCII:
                            cs_draw_glyph((charX + TEXT_WIDTH(1)), charY, byte, textColor);
                            break;
                        case MEMORY_MODE_BINARY:;
                            CSScreenCoord_u32 bitX = 0;
                            for (Byte bit = 0; bit < BITS_PER_BYTE; bit++) {
                                RGBA32 color = (((byte >> ((BITS_PER_BYTE - 1) - bit)) & 0b1) ? COLOR_RGBA32_LIGHT_GRAY : COLOR_RGBA32_DARK_GRAY);
                                cs_draw_rect((charX + bitX), charY, 1, CRASH_SCREEN_FONT_CHAR_HEIGHT, color);
                                bitX += (1 + (bit & 0x1));
                            }
                            break;
                        case MEMORY_MODE_RGBA16:;
                            RGBA16 color = data.halfword[byteOffset > 1];
                            cs_draw_rect((charX - 1), (charY - 1), (TEXT_WIDTH(2) + 1), (TEXT_HEIGHT(1) - 1), RGBA16_TO_RGBA32(color));
                            break;
                        case MEMORY_MODE_RGBA32:;
                            cs_draw_rect((charX - 1), (charY - 1), (TEXT_WIDTH(2) + 1), (TEXT_HEIGHT(1) - 1), data.word);
                            break;

                    }
                } else {
                    cs_draw_glyph((charX + TEXT_WIDTH(1)), charY, '*', COLOR_RGBA32_CRASH_OUT_OF_BOUNDS);
                }

                // If the display mode is a color, draw a translucent box on top of the data.
                if (selected && isColor) {
                    cs_draw_rect((charX - 1), (charY - 1), (TEXT_WIDTH(2) + 1), (TEXT_HEIGHT(1) - 1), RGBA32_SET_ALPHA(selectColor, 0x7F));
                }

                charX += (TEXT_WIDTH(2) + 1);
            }
        }
    }
}

void page_memory_draw(void) {
    __OSThreadContext* tc = &gInspectThread->context;

    sRamViewNumShownRows = MEMORY_NUM_SHOWN_ROWS;
    const _Bool showCurrentRange  = cs_get_setting_val(CS_OPT_GROUP_PAGE_MEMORY, CS_OPT_MEMORY_SHOW_RANGE);
    sRamViewNumShownRows -= showCurrentRange;
    const _Bool showCurrentSymbol = cs_get_setting_val(CS_OPT_GROUP_PAGE_MEMORY, CS_OPT_MEMORY_SHOW_SYMBOL);
    sRamViewNumShownRows -= showCurrentSymbol;

    u32 line = 1;

    Address startAddr = sRamViewViewportIndex;
    Address endAddr = (startAddr + ((sRamViewNumShownRows - 1) * PAGE_MEMORY_STEP));

    if (showCurrentRange) {
        // "[XXXXXXXX] in [XXXXXXXX]-[XXXXXXXX]"
        cs_print(TEXT_X(0), TEXT_Y(line),
            (STR_COLOR_PREFIX STR_HEX_WORD" in "STR_HEX_WORD"-"STR_HEX_WORD),
            COLOR_RGBA32_WHITE, gSelectedAddress, startAddr, endAddr
        );
        line++;
    }

    if (showCurrentSymbol) {
        cs_print_addr_location_info(TEXT_X(0), TEXT_Y(line), CRASH_SCREEN_NUM_CHARS_X, gSelectedAddress, TRUE);
        line++;
    }

    if (showCurrentRange || showCurrentSymbol) {
        cs_draw_divider(DIVIDER_Y(line));
    }

    u32 charX = (TEXT_X(SIZEOF_HEX(Address)) + 3);

    // Print column headers:
    for (u32 i = 0; i < (4 * sizeof(Word)); i++) {
        if ((i % 4) == 0) {
            charX += 2;
        }

        // "[XX]"
        cs_print(charX, TEXT_Y(line), (STR_COLOR_PREFIX STR_HEX_BYTE), ((i % 2) ? COLOR_RGBA32_CRASH_MEMORY_COL1 : COLOR_RGBA32_CRASH_MEMORY_COL2), i);

        charX += (TEXT_WIDTH(2) + 1);
    }

    ram_viewer_print_data((line + 1), startAddr);

    // Veertical divider
    cs_draw_rect((TEXT_X(SIZEOF_HEX(Address)) + 2), DIVIDER_Y(line), 1, TEXT_HEIGHT(sRamViewNumShownRows + 1), COLOR_RGBA32_CRASH_DIVIDER);

    // "MEMORY"
    cs_print(TEXT_X(1), TEXT_Y(line), "MEMORY");

    line++;

    cs_draw_divider(DIVIDER_Y(line));

    u32 line2 = (line + sRamViewNumShownRows);

    cs_draw_divider(DIVIDER_Y(line2));

    u32 scrollTop = (DIVIDER_Y(line) + 1);
    u32 scrollBottom = DIVIDER_Y(line2);

    const size_t shownSection = ((sRamViewNumShownRows - 1) * PAGE_MEMORY_STEP);

    // Scroll bar:
    cs_draw_scroll_bar(
        scrollTop, scrollBottom,
        shownSection, VIRTUAL_RAM_SIZE,
        (sRamViewViewportIndex - VIRTUAL_RAM_START),
        COLOR_RGBA32_CRASH_SCROLL_BAR, TRUE
    );

    // Scroll bar crash position marker:
    cs_draw_scroll_bar(
        scrollTop, scrollBottom,
        shownSection, VIRTUAL_RAM_SIZE,
        (GET_EPC(tc) - VIRTUAL_RAM_START),
        COLOR_RGBA32_CRASH_AT, FALSE
    );

    osWritebackDCacheAll();
}

void page_memory_input(void) {
    if (gCSDirectionFlags.pressed.up) {
        // Scroll up.
        if (gSelectedAddress >= (VIRTUAL_RAM_START + PAGE_MEMORY_STEP)) {
            gSelectedAddress -= PAGE_MEMORY_STEP;
        }
    }

    if (gCSDirectionFlags.pressed.down) {
        // Scroll down.
        if (gSelectedAddress <= (VIRTUAL_RAM_END - PAGE_MEMORY_STEP)) {
            gSelectedAddress += PAGE_MEMORY_STEP;
        }
    }

    if (gCSDirectionFlags.pressed.left) {
        // Prevent wrapping.
        if (((gSelectedAddress - 1) & BITMASK(4)) != 0xF) {
            gSelectedAddress--;
        }
    }

    if (gCSDirectionFlags.pressed.right) {
        // Prevent wrapping.
        if (((gSelectedAddress + 1) & BITMASK(4)) != 0x0) {
            gSelectedAddress++;
        }
    }

    u16 buttonPressed = gCSCompositeController->buttonPressed;

    if (buttonPressed & A_BUTTON) {
        Word dataAtAddr = 0x00000000;
        if (try_read_word_aligned(&dataAtAddr, gSelectedAddress) && is_valid_ram_addr(dataAtAddr)) {
            open_address_select(dataAtAddr);
        } else {
            open_address_select(gSelectedAddress);
        }
    }

    if (buttonPressed & B_BUTTON) {
        cs_inc_setting(CS_OPT_GROUP_PAGE_MEMORY, CS_OPT_MEMORY_DISPLAY_MODE, 1);
    }

    sRamViewViewportIndex = cs_clamp_view_to_selection(sRamViewViewportIndex, gSelectedAddress, sRamViewNumShownRows, PAGE_MEMORY_STEP);
}

void page_memory_print(void) {
#ifdef UNF
    osSyncPrintf("\n");

    Address startAddr = sRamViewViewportIndex;
    Address endAddr = (startAddr + ((sRamViewNumShownRows - 1) * PAGE_MEMORY_STEP));

    osSyncPrintf("- SECTION: ["STR_HEX_WORD"-"STR_HEX_WORD"]\n", startAddr, endAddr);

    for (u32 row = 0; row < sRamViewNumShownRows; row++) {
        osSyncPrintf("- ["STR_HEX_WORD"]:", (startAddr + (row * PAGE_MEMORY_STEP))); // Row address.

        for (u32 wordOffset = 0; wordOffset < 4; wordOffset++) {
            osSyncPrintf(" "STR_HEX_WORD, sMemoryViewData[row][wordOffset]);
        }

        osSyncPrintf("\n");
    }
#endif // UNF
}


struct CSPage gCSPage_memory = {
    .name         = "MEMORY VIEW",
    .initFunc     = page_memory_init,
    .drawFunc     = page_memory_draw,
    .inputFunc    = page_memory_input,
    .printFunc    = page_memory_print,
    .contList     = cs_cont_list_memory,
    .settingsList = cs_settings_group_page_memory,
    .flags = {
        .initialized = FALSE,
        .crashed     = FALSE,
    },
};
