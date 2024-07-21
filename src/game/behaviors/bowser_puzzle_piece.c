#include <ultra64.h>
#include "behavior_data.h"
#include "global_object_fields.h"
#include "engine/math_util.h"
#include "game/object_helpers.h"
#include "game/spawn_sound.h"

/* Bowser Puzzle */
#define /*0x0F4*/ oBowserPuzzleCompletionFlags OBJECT_FIELD_S32(0x1B)

/* Bowser Puzzle Piece */
#define /*0x0FC*/ O_BOWSER_PUZZLE_PIECE_OFFSET_INDEX         0x1D
#define /*0x0FC*/ O_BOWSER_PUZZLE_PIECE_OFFSET_X_INDEX       (O_BOWSER_PUZZLE_PIECE_OFFSET_INDEX + 0) // 0x1D
#define /*0x100*/ O_BOWSER_PUZZLE_PIECE_OFFSET_Y_INDEX       (O_BOWSER_PUZZLE_PIECE_OFFSET_INDEX + 1) // 0x1E
#define /*0x104*/ O_BOWSER_PUZZLE_PIECE_OFFSET_Z_INDEX       (O_BOWSER_PUZZLE_PIECE_OFFSET_INDEX + 2) // 0x1F
#define /*0x0FC*/ oBowserPuzzlePieceOffsetVec                OBJECT_FIELD_F32(O_BOWSER_PUZZLE_PIECE_OFFSET_INDEX)
#define /*0x0FC*/ oBowserPuzzlePieceOffsetX                  OBJECT_FIELD_F32(O_BOWSER_PUZZLE_PIECE_OFFSET_X_INDEX)
#define /*0x100*/ oBowserPuzzlePieceOffsetY                  OBJECT_FIELD_F32(O_BOWSER_PUZZLE_PIECE_OFFSET_Y_INDEX)
#define /*0x104*/ oBowserPuzzlePieceOffsetZ                  OBJECT_FIELD_F32(O_BOWSER_PUZZLE_PIECE_OFFSET_Z_INDEX)
#define /*0x108*/ oBowserPuzzlePieceContinuePerformingAction OBJECT_FIELD_S32(0x20)
#define /*0x10C*/ oBowserPuzzlePieceActionList               OBJECT_FIELD_VPTR(0x21)
#define /*0x110*/ oBowserPuzzlePieceNextAction               OBJECT_FIELD_VPTR(0x22)

/**
 * Behavior for the sliding Bowser puzzle in Lethal Lava Land.
 */

/*
 * The pieces move in this order:
 *
 *   1, 2, 5, 6, 10, 9, 13, 12, 8, 7, 3, 4
 *
 * Once they reach the end of the routine they follow it backwards until the
 *   puzzle is complete again.
 *
 * Note that pieces 11 and 14 do not move.
 */
static ObjAction8 sPieceActions01[] = { BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_LEFT, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE ,
                                        BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_RIGHT, BOWSER_PUZZLE_PIECE_ACT_IDLE, -1 };
static ObjAction8 sPieceActions02[] = { BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_LEFT, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE ,
                                        BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_RIGHT, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, -1 };
static ObjAction8 sPieceActions05[] = { BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_UP  , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE ,
                                        BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_DOWN , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, -1 };
static ObjAction8 sPieceActions06[] = { BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_LEFT, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE ,
                                        BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_RIGHT, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, -1 };
static ObjAction8 sPieceActions10[] = { BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_UP  , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE ,
                                        BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_DOWN , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, -1 };
static ObjAction8 sPieceActions09[] = { BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_RIGHT, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE ,
                                        BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_LEFT, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, -1 };
static ObjAction8 sPieceActions13[] = { BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_UP   , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE ,
                                        BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_DOWN, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, -1 };
static ObjAction8 sPieceActions12[] = { BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_RIGHT, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE ,
                                        BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_LEFT, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, -1 };
static ObjAction8 sPieceActions08[] = { BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_DOWN , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE ,
                                        BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_UP  , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, -1 };
static ObjAction8 sPieceActions07[] = { BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_RIGHT, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE ,
                                        BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_LEFT, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, -1 };
static ObjAction8 sPieceActions03[] = { BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_DOWN, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE ,
                                        BOWSER_PUZZLE_PIECE_ACT_UP  , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, -1 };
static ObjAction8 sPieceActions04[] = { BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_LEFT, BOWSER_PUZZLE_PIECE_ACT_RIGHT,
                                        BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, -1 };
static ObjAction8 sPieceActions11[] = { BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE ,
                                        BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, -1 };
static ObjAction8 sPieceActions14[] = { BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE ,
                                        BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE, BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE , BOWSER_PUZZLE_PIECE_ACT_IDLE, -1 };

struct BowserPuzzlePiece {
    ModelID8 model;
    s8 xOffset;
    s8 zOffset;
    ObjAction8 initialAction;
    ObjAction8 *actionList;
};

/*
 * The puzzle pieces are initially laid out in the following manner:
 *
 *       +---+---+---+
 *       | 1 | 2 | * |
 *   +---+---+---+---+
 *   | 3 | 4 | 5 | 6 |
 *   +---+---+---+---+
 *   | 7 | 8 | 9 |10 |
 *   +---+---+---+---+
 *   |11 |12 |13 |14 |
 *   +---+---+---+---+
 *
 * (* = star platform)
 */
static struct BowserPuzzlePiece sBowserPuzzlePieces[] = {
    { MODEL_LLL_BOWSER_PIECE_1, -5, -15, 1, sPieceActions01 },
    { MODEL_LLL_BOWSER_PIECE_2, 5, -15, 0, sPieceActions02 },
    { MODEL_LLL_BOWSER_PIECE_3, -15, -5, 0, sPieceActions03 },
    { MODEL_LLL_BOWSER_PIECE_4, -5, -5, 0, sPieceActions04 },
    { MODEL_LLL_BOWSER_PIECE_5, 5, -5, 0, sPieceActions05 },
    { MODEL_LLL_BOWSER_PIECE_6, 15, -5, 0, sPieceActions06 },
    { MODEL_LLL_BOWSER_PIECE_7, -15, 5, 0, sPieceActions07 },
    { MODEL_LLL_BOWSER_PIECE_8, -5, 5, 0, sPieceActions08 },
    { MODEL_LLL_BOWSER_PIECE_9, 5, 5, 0, sPieceActions09 },
    { MODEL_LLL_BOWSER_PIECE_10, 15, 5, 0, sPieceActions10 },
    { MODEL_LLL_BOWSER_PIECE_11, -15, 15, 0, sPieceActions11 },
    { MODEL_LLL_BOWSER_PIECE_12, -5, 15, 0, sPieceActions12 },
    { MODEL_LLL_BOWSER_PIECE_13, 5, 15, 0, sPieceActions13 },
    { MODEL_LLL_BOWSER_PIECE_14, 15, 15, 0, sPieceActions14 }
};

/**
 * Spawn a single puzzle piece.
 */
void bhv_lll_bowser_puzzle_spawn_piece(ModelID16 model, const BehaviorScript *behavior,
                                       f32 xOffset, f32 zOffset,
                                       ObjAction8 initialAction, ObjAction8 *actionList) {
    struct Object *puzzlePiece = spawn_object(o, model, behavior);
    puzzlePiece->oPosX += xOffset;
    puzzlePiece->oPosY += 50.0f;
    puzzlePiece->oPosZ += zOffset;
    puzzlePiece->oAction = initialAction; // This action never gets executed.
    puzzlePiece->oBowserPuzzlePieceActionList = actionList;
    puzzlePiece->oBowserPuzzlePieceNextAction = actionList;
}

/**
 * Spawn the 14 puzzle pieces.
 */
void bhv_lll_bowser_puzzle_spawn_pieces(f32 pieceWidth) {
    s32 i;

    // Spawn all 14 puzzle pieces.
    for (i = 0; i < 14; i++) {
        bhv_lll_bowser_puzzle_spawn_piece(sBowserPuzzlePieces[i].model, bhvLllBowserPuzzlePiece,
                                          sBowserPuzzlePieces[i].xOffset * pieceWidth / 10.0f,
                                          sBowserPuzzlePieces[i].zOffset * pieceWidth / 10.0f,
                                          sBowserPuzzlePieces[i].initialAction,
                                          sBowserPuzzlePieces[i].actionList);
    }

    // The pieces should only be spawned once so go to the next action.
    o->oAction = BOWSER_PUZZLE_ACT_WAIT_FOR_COMPLETE;
}

/*
 * Does the initial spawn of the puzzle pieces and then waits to spawn 5 coins.
 */
void bhv_lll_bowser_puzzle_loop(void) {
    s32 i;

    switch (o->oAction) {
        case BOWSER_PUZZLE_ACT_SPAWN_PIECES:
            bhv_lll_bowser_puzzle_spawn_pieces(512.0f);
            break;

        case BOWSER_PUZZLE_ACT_WAIT_FOR_COMPLETE:
            // If both completion flags are set and Mario is within 1000 units...
            if ((o->oBowserPuzzleCompletionFlags == (BOWSER_PUZZLE_COMPLETION_FLAG_MARIO_ON_PLATFORM | BOWSER_PUZZLE_COMPLETION_FLAG_PUZZLE_COMPLETE)) && (o->oDistanceToMario < 1000.0f)) {
                // Spawn 5 coins.
                for (i = 0; i < 5; i++) {
                    spawn_object(o, MODEL_YELLOW_COIN, bhvSingleCoinGetsSpawned);
                }

                // Reset completion flags (even though they never get checked again).
                o->oBowserPuzzleCompletionFlags = BOWSER_PUZZLE_COMPLETION_FLAGS_NONE;

                // Go to next action so we don't spawn 5 coins ever again.
                o->oAction = BOWSER_PUZZLE_ACT_DONE;
            }
            break;

        case BOWSER_PUZZLE_ACT_DONE:
            break;
    }
}

/*
 * Update the puzzle piece.
 */
void bhv_lll_bowser_puzzle_piece_update(void) {
    ObjAction8 *nextAction = o->oBowserPuzzlePieceNextAction;

    // If Mario is standing on this puzzle piece, set a flag in the parent.
    if (gMarioObject->platform == o) {
        o->parentObj->oBowserPuzzleCompletionFlags = BOWSER_PUZZLE_COMPLETION_FLAG_MARIO_ON_PLATFORM;
    }

    // If we should advance to the next action...
    if (!o->oBowserPuzzlePieceContinuePerformingAction) {
        // Start doing the next action.
        cur_obj_change_action(*nextAction);

        // Advance the pointer to the next action.
        nextAction++;
        o->oBowserPuzzlePieceNextAction = nextAction;

        // If we're at the end of the list...
        if (*nextAction == -1) {
            // Set the other completion flag in the parent.
            o->parentObj->oBowserPuzzleCompletionFlags |= BOWSER_PUZZLE_COMPLETION_FLAG_PUZZLE_COMPLETE;

            // The next action is the first action in the list again.
            o->oBowserPuzzlePieceNextAction = o->oBowserPuzzlePieceActionList;
        }

        // Keep doing this action until it's complete.
        o->oBowserPuzzlePieceContinuePerformingAction = TRUE;
    }
}

void bhv_lll_bowser_puzzle_piece_move(f32 xOffset, f32 zOffset, s32 duration, UNUSED s32 a3) {
    // For the first 20 frames, shake the puzzle piece up and down.
    if (o->oTimer < 20) {
        o->oBowserPuzzlePieceOffsetY = o->oTimer & 0x1 ? 0.0f : -6.0f;
    } else {
        // On frame 20, play the shifting sound.
        if (o->oTimer == 20) {
            cur_obj_play_sound_2(SOUND_OBJ2_BOWSER_PUZZLE_PIECE_MOVE);
        }

        // For the number of frames specified by duration, move the piece.
        if (o->oTimer < duration + 20) {
            o->oBowserPuzzlePieceOffsetX += xOffset;
            o->oBowserPuzzlePieceOffsetZ += zOffset;
        } else {
            // This doesn't actually accomplish anything since
            // cur_obj_change_action is going to be called before the
            // next action is performed anyway.
            o->oAction = BOWSER_PUZZLE_PIECE_ACT_IDLE;

            // Advance to the next action.
            o->oBowserPuzzlePieceContinuePerformingAction = 0;
        }
    }
}

void bhv_lll_bowser_puzzle_piece_idle(void) {
    // For the first 24 frames, do nothing.
    // Then advance to the next action.
    if (o->oTimer >= 24) {
        o->oBowserPuzzlePieceContinuePerformingAction = FALSE;
    }
}

void bhv_lll_bowser_puzzle_piece_move_left(void) {
    bhv_lll_bowser_puzzle_piece_move(-128.0f, 0.0f, 4, 4);
}

void bhv_lll_bowser_puzzle_piece_move_right(void) {
    bhv_lll_bowser_puzzle_piece_move(128.0f, 0.0f, 4, 5);
}

void bhv_lll_bowser_puzzle_piece_move_up(void) {
    bhv_lll_bowser_puzzle_piece_move(0.0f, -128.0f, 4, 6);
}

void bhv_lll_bowser_puzzle_piece_move_down(void) {
    bhv_lll_bowser_puzzle_piece_move(0.0f, 128.0f, 4, 3);
}

ObjActionFunc sBowserPuzzlePieceActions[] = {
    bhv_lll_bowser_puzzle_piece_idle,
    bhv_lll_bowser_puzzle_piece_move_left,
    bhv_lll_bowser_puzzle_piece_move_right,
    bhv_lll_bowser_puzzle_piece_move_up,
    bhv_lll_bowser_puzzle_piece_move_down,
};

void bhv_lll_bowser_puzzle_piece_loop(void) {
    bhv_lll_bowser_puzzle_piece_update();

    cur_obj_call_action_function(sBowserPuzzlePieceActions);
    vec3f_sum(&o->oPosVec, &o->oBowserPuzzlePieceOffsetVec, &o->oHomeVec);
}