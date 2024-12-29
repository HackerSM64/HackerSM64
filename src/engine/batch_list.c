#include "batch_list.h"

#include "actors/common1.h"
#include "game/segment2.h"

static const struct BatchDisplayLists BatchesTransparentDecal[] = {
    [ BATCH_TRANSPARENT_DECAL_SHADOW_CIRCLE ] = { dl_shadow_circle, dl_shadow_end },
    [ BATCH_TRANSPARENT_DECAL_SHADOW_SQUARE ] = { dl_shadow_square, dl_shadow_end },
};
STATIC_ASSERT(BATCH_TRANSPARENT_DECAL_COUNT == sizeof(BatchesTransparentDecal) / sizeof(*BatchesTransparentDecal), "Mismatch in transparent decal batch count");

static const struct BatchDisplayLists BatchesCLD[] = {
    [ BATCH_CLD_SHADOW_CIRCLE ] = { dl_shadow_circle, dl_shadow_end },
    [ BATCH_CLD_SHADOW_SQUARE ] = { dl_shadow_square, dl_shadow_end },
};
STATIC_ASSERT(BATCH_CLD_COUNT == sizeof(BatchesCLD) / sizeof(*BatchesCLD), "Mismatch in CLD batch count");

static const struct BatchDisplayLists BatchesAlpha[] = {
#if SILHOUETTE
};
static const struct BatchDisplayLists BatchesOccludeSilhouetteAlpha[] = {
#endif

#ifdef IA8_30FPS_COINS
    [ BATCH_OCCLUDE_SILHOUETTE_ALPHA_COIN_FIRST + 0 ] = { coin_seg3_dl_0      , coin_seg3_dl_end },
    [ BATCH_OCCLUDE_SILHOUETTE_ALPHA_COIN_FIRST + 1 ] = { coin_seg3_dl_22_5   , coin_seg3_dl_end },
    [ BATCH_OCCLUDE_SILHOUETTE_ALPHA_COIN_FIRST + 2 ] = { coin_seg3_dl_45     , coin_seg3_dl_end },
    [ BATCH_OCCLUDE_SILHOUETTE_ALPHA_COIN_FIRST + 3 ] = { coin_seg3_dl_67_5   , coin_seg3_dl_end },
    [ BATCH_OCCLUDE_SILHOUETTE_ALPHA_COIN_FIRST + 4 ] = { coin_seg3_dl_90     , coin_seg3_dl_end },
#else
    [ BATCH_OCCLUDE_SILHOUETTE_ALPHA_COIN_FIRST + 0 ] = { coin_seg3_dl_front     , coin_seg3_dl_end },
    [ BATCH_OCCLUDE_SILHOUETTE_ALPHA_COIN_FIRST + 1 ] = { coin_seg3_dl_tilt_right, coin_seg3_dl_end },
    [ BATCH_OCCLUDE_SILHOUETTE_ALPHA_COIN_FIRST + 2 ] = { coin_seg3_dl_side      , coin_seg3_dl_end },
    [ BATCH_OCCLUDE_SILHOUETTE_ALPHA_COIN_FIRST + 3 ] = { coin_seg3_dl_tilt_left , coin_seg3_dl_end },
#endif
};

STATIC_ASSERT(BATCH_ALPHA_COUNT == sizeof(BatchesAlpha) / sizeof(*BatchesAlpha), "Mismatch in alpha batch count");
#if SILHOUETTE
STATIC_ASSERT(BATCH_OCCLUDE_SILHOUETTE_ALPHA_COUNT == sizeof(BatchesOccludeSilhouetteAlpha) / sizeof(*BatchesOccludeSilhouetteAlpha), "Mismatch in occlude silhouette alpha batch count");
#endif

static inline struct BatchArray* batch_array_alloc(struct AllocOnlyPool *pool, int count, const struct BatchDisplayLists* dls) {
    struct BatchArray* batches = alloc_only_pool_alloc(pool, sizeof(struct BatchArray) + count * sizeof(struct DisplayListLinks));
    batches->count = count;
    batches->batchDLs = dls;
    return batches;
}

struct BatchArray* batch_list_objects_alloc_xlu_decal(struct AllocOnlyPool *pool) {
    return batch_array_alloc(pool, BATCH_TRANSPARENT_DECAL_COUNT, BatchesTransparentDecal);
}

struct BatchArray* batch_list_objects_alloc_cld(struct AllocOnlyPool *pool) {
    return batch_array_alloc(pool, BATCH_CLD_COUNT, BatchesCLD);
}

struct BatchArray* batch_list_objects_alloc_alpha(struct AllocOnlyPool *pool) {
    return batch_array_alloc(pool, BATCH_ALPHA_COUNT, BatchesAlpha);
}

#if SILHOUETTE
struct BatchArray* batch_list_objects_alloc_occlude_silhouette_alpha(struct AllocOnlyPool *pool) {
    return batch_array_alloc(pool, BATCH_OCCLUDE_SILHOUETTE_ALPHA_COUNT, BatchesOccludeSilhouetteAlpha);
}
#endif
