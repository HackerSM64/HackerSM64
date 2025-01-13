// Written by aglab2

#define LZ4_HC_STATIC_LINKING_ONLY
#include "lz4hc.c"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define VERIFIER
#define MAX_COMP_SIZE (8*1024*1024)
// #define DEBUG
// #define STATS
// #define FAVOR_DECOMPRESSION_SPEED
#define COMPRESSION_LEVEL LZ4HC_CLEVEL_MAX

// MARK: LZ4HC ops

#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

#ifdef STATS
#define STAT(...) printf(__VA_ARGS__)
#else
#define STAT(...)
#endif

int LZ4T_distanceMax = 65535;
uint32_t LZ4T_hashMask = 0xffffffffU;
int LZ4T_minMatch = 4;

#define LZ4T_IS_SHORT_OFFSET (4096 == LZ4T_distanceMax)

static uint32_t* sCurrentNibblePendingOutputPtr = NULL;
static uint32_t sCurrentNibble = 0;
static uint32_t sCurrentNibbleShift = 8;
static uint32_t sNibblesPushed = 0;

static uint16_t* sDeferredNibblePtr = NULL;

static void LZ4T_resetGlobals(void)
{
    sCurrentNibblePendingOutputPtr = NULL;
    sCurrentNibble = 0;
    sCurrentNibbleShift = 8;
    sNibblesPushed = 0;
    sDeferredNibblePtr = NULL;
}

#define TINY_LITERAL_LIMIT 21
#define TINY_MINMATCH (MINMATCH - 1)
// 0b0000 and 0b0111 are reserved
#define TINY_MATCH_LIMIT (6 + TINY_MINMATCH)
// 0b0000 and 0b1111 are reserved
#define TINY_MATCH_LIMIT_EX (14 + TINY_MINMATCH)

LZ4_FORCE_INLINE void LZ4T_pushNibble(BYTE** _op, uint32_t newNibble)
{
    sNibblesPushed++;
    LOG("%d: Pushing nibble: %x\n", sNibblesPushed, newNibble);

    if (sDeferredNibblePtr)
    {
        LOG("Deferred nibble: %x\n", newNibble);
        uint16_t packedOffset = __builtin_bswap16(*sDeferredNibblePtr);
        packedOffset |= (newNibble << 12);
        *sDeferredNibblePtr = __builtin_bswap16(packedOffset);
        sDeferredNibblePtr = NULL;
        return;
    }

    if (0 == sCurrentNibbleShift)
    {
        LOG("Requesting fresh nibbles\n");
        uint32_t nibbleBE = __builtin_bswap32(sCurrentNibble);
        memcpy(sCurrentNibblePendingOutputPtr, &nibbleBE, 4);
        sCurrentNibblePendingOutputPtr = (uint32_t*)(*_op);
        memset(sCurrentNibblePendingOutputPtr, 0, 4);
        sCurrentNibble = 0;
        sCurrentNibbleShift = 8;
        (*_op) += 4;
    }

    sCurrentNibbleShift--;
    newNibble <<= sCurrentNibbleShift * 4;
    sCurrentNibble |= newNibble;
    LOG("New nibble batch: 0x%x\n", sCurrentNibble);
}

LZ4_FORCE_INLINE void LZ4T_encodeBitLen(BYTE** _op, int len)
{
#define op      (*_op)
    if (0 == len)
    {
        *op++ = 0;
        return;
    }

    while (len) {
        if (len <= 127) {
            LOG("Pushing final len: %d\n", len);
            *op++ = len;
        } else {
            LOG("Pushing partial len: %d\n", len & 127);
            *op++ = 128 | (len & 127);
        }
        len >>= 7;
    }
}

int LZ4_compressBound(int isize)
{ MAX_COMP_SIZE; }

LZ4_FORCE_INLINE int LZ4HC_encodeSequence (
    const BYTE** _ip,
    BYTE** _op,
    const BYTE** _anchor,
    int matchLength,
    int offset,
    limitedOutput_directive,
    BYTE*)
{
#define ip      (*_ip)
#define op      (*_op)
#define anchor  (*_anchor)

    /* Encode Literal length */
    size_t length = (size_t)(ip - anchor);
    LOG("Encoding literals: %d\n", length);
    int tinyMatchLimit = TINY_MATCH_LIMIT;
    if (length > TINY_LITERAL_LIMIT) {
        // means that encoding is extended
        LZ4T_pushNibble(&op, 8);
        LZ4T_encodeBitLen(&op, length - TINY_LITERAL_LIMIT - 1);
        LOG("Copying literals wildly: %p %p %u\n", op, anchor, length);
        LZ4_wildCopy8(op, anchor, op + length);
        op += length;
        tinyMatchLimit = TINY_MATCH_LIMIT_EX;
    } else {
        size_t len = length;
        const void* src = anchor;
        // put raw nibbles
        while (len) {
            LOG("Copying literals sizely: %p %p %u\n", op, anchor, length);
            uint32_t cut = len > 7 ? 7 : len;
            LZ4T_pushNibble(&op, 8 | cut);
            *(uint64_t*) op = *(uint64_t*)src;
            op += cut;
            src += cut;
            len -= cut;
        }

        if ((length % 7) == 0)
            tinyMatchLimit = TINY_MATCH_LIMIT;
        else
            tinyMatchLimit = TINY_MATCH_LIMIT_EX;
    }

    /* Encode Offset */
    LOG("Encoding match with tiny limit %d at offset %d for length %d\n", tinyMatchLimit, offset, matchLength);
    if (matchLength > tinyMatchLimit) {
        // 7 or 15 means that matchLength is extended
        LZ4T_pushNibble(&op, tinyMatchLimit - TINY_MINMATCH + 1);
    } else {
        LZ4T_pushNibble(&op, matchLength - TINY_MINMATCH);
    }
    assert(offset <= LZ4_DISTANCE_MAX );
    assert(offset > 0);
    LZ4_write16(op, __builtin_bswap16((U16)(offset - 1)));
    if (LZ4T_IS_SHORT_OFFSET)
    {
        sDeferredNibblePtr = (uint16_t*)op;
    }
    op += 2;
    LOG("Pushing offset: %d\n", offset);

    /* Encode MatchLength */
    assert(matchLength >= MINMATCH);
    if (matchLength > tinyMatchLimit) {
        LZ4T_encodeBitLen(&op, matchLength - tinyMatchLimit - 1);
    }

    /* Prepare next loop */
    ip += matchLength;
    anchor = ip;

    return 0;

#undef ip
#undef op
#undef anchor
}

int LZ4T_lastLiterals (
    const BYTE** _ip,
    BYTE** _op,
    const BYTE** _anchor,
    limitedOutput_directive limit,
    BYTE* oend,
    size_t length)
{
#define ip      (*_ip)
#define op      (*_op)
#define anchor  (*_anchor)

    if (length)
    {
        LOG("Encoding last literals: %d\n", length);
        if (length > TINY_LITERAL_LIMIT) {
            // means that encoding is extended
            LZ4T_pushNibble(&op, 8);
            LZ4T_encodeBitLen(&op, length - TINY_LITERAL_LIMIT - 1);
            LZ4_wildCopy8(op, anchor, op + length);
            op += length;
        } else {
            size_t len = length;
            const void* src = anchor;
            // put raw nibbles
            while (len) {
                uint32_t cut = len > 7 ? 7 : len;
                LZ4T_pushNibble(&op, 8 | cut);
                *(uint64_t*) op = *(uint64_t*)src;
                op += cut;
                src += cut;
                len -= cut;
            }
        }
    }

    // push ending marker
    uint32_t nibbleBE = __builtin_bswap32(sCurrentNibble);
    memcpy(sCurrentNibblePendingOutputPtr, &nibbleBE, 4);
    sCurrentNibblePendingOutputPtr = (uint32_t*)(*_op);
    memset(sCurrentNibblePendingOutputPtr, 0, 4);
    (*_op) += 4;
    sCurrentNibblePendingOutputPtr = NULL;

    ip = anchor + length;
    return 0;

#undef ip
#undef op
#undef anchor
}

LZ4_FORCE_INLINE int LZ4T_bitCodePrice(int len)
{
    if (0 == len)
        return 2;
    
    int price = 0;
    while (len)
    {
        // each ex size byte is 2 nibbles
        price += 2;
        len >>= 7;
    }

    return price;
}

// Counts are in nibbles so 1 byte is 2 nibbles
LZ4_FORCE_INLINE int LZ4HC_literalsPrice(int const litlen)
{
    int price = litlen * 2;
    assert(litlen >= 0);
    if (litlen > TINY_LITERAL_LIMIT) {
        int len = litlen - TINY_LITERAL_LIMIT - 1;
        // encode 1 nibble for ex size
        price += 1 + LZ4T_bitCodePrice(len);
    } else {
        int len = litlen;
        while (len) {
            // single nibble for short sizes
            int cut = len > 7 ? 7 : len;
            price += 1;
            len -= cut;
        }
    }
    return price;
}

LZ4_FORCE_INLINE int LZ4HC_sequencePrice(int litlen, int mlen)
{
    int price = 4 ; // 16-bit offset is 4 nibbles
    assert(litlen >= 0);
    assert(mlen >= MINMATCH);

    price += LZ4HC_literalsPrice(litlen);
    // nibble for encoding match is saved because it is rolled from offset in the stream
    if (!LZ4T_IS_SHORT_OFFSET)
    {
        price += 1;
    }
    int tinyMatchLimit = TINY_MATCH_LIMIT_EX;
    if (litlen <= TINY_LITERAL_LIMIT) {
        if ((litlen % 7) == 0)
            tinyMatchLimit = TINY_MATCH_LIMIT;
    }

    if (mlen > tinyMatchLimit) {
        // ex size encoding
        int len = mlen - tinyMatchLimit - 1;
        price += LZ4T_bitCodePrice(len);
    }
    return price;
}

// MARK: LZ4T Verifier

struct SizedBuffer
{
    void* buffer;
    size_t size;
};

static void verifySizedBuffer(struct SizedBuffer* data, int amount)
{
    if (data->size < amount)
    {
        printf("Verifier fail: not enough data to verify %d bytes\n", amount);
        abort();
    }
}

static void moveSizedBuffer(struct SizedBuffer* data, int amount)
{
    data->buffer = (uint8_t*)data->buffer + amount;
    data->size -= amount;
}

static uint8_t loadVerifyU8(struct SizedBuffer* data)
{
    uint8_t value;
    size_t size = sizeof(value);

    verifySizedBuffer(data, size);
    value = *(uint8_t*)data->buffer;
    moveSizedBuffer(data, size);

    return value;
}

static uint16_t loadVerifyU16(struct SizedBuffer* data)
{
    uint16_t value;
    size_t size = sizeof(value);

    verifySizedBuffer(data, size);
    value = LZ4_read16(data->buffer);
    moveSizedBuffer(data, size);

    return value;
}

static uint32_t loadVerifyU32(struct SizedBuffer* data)
{
    uint32_t value;
    size_t size = sizeof(value);

    verifySizedBuffer(data, size);
    value = LZ4_read32(data->buffer);
    moveSizedBuffer(data, size);

    return value;
}

static void wildCopyVerify(struct SizedBuffer* in, struct SizedBuffer* out, int amount)
{
    verifySizedBuffer(in, amount);
    verifySizedBuffer(out, amount);

    LZ4_wildCopy8(out->buffer, in->buffer, out->buffer + amount);

    moveSizedBuffer(in, amount);
    moveSizedBuffer(out, amount);
}

static uint64_t wildLoadU64Verify(struct SizedBuffer* data, int size)
{
    uint64_t value;
    verifySizedBuffer(data, size);
    LZ4_memcpy(&value, data->buffer, sizeof(uint64_t));
    moveSizedBuffer(data, size);

    return value;
}

static void wildStoreU64Verify(struct SizedBuffer* data, uint64_t value, int size)
{
    verifySizedBuffer(data, size);
    LZ4_memcpy(data->buffer, &value, sizeof(uint64_t));
    moveSizedBuffer(data, size);
}

static int LZ4T_unpack_size(struct SizedBuffer* in, int* _giganticCounts)
{
#define giganticCounts (*_giganticCounts)
    int amount = 0;
    int shift = 0;
    int iteration = -1;
    while (1)
    {
        if (iteration > 4)
        {
            printf("Verifier fail: too many iterations in unpack_size\n");
            abort();
        }

        iteration++;
        int8_t next = loadVerifyU8(in);
        LOG("Next: %d\n", next);
        if (next < 0)
        {
            amount |= (next & 0x7f) << shift;
            shift += 7;
            LOG("New amount: %d\n", amount);
        }
        else
        {
            amount |= (next << shift);
            LOG("Amount is %d\n", amount);
            break;
        }
    }

    giganticCounts += iteration;

    return amount;
#undef giganticCounts
}

// TODO: Take in compressedSize
// !!! Make sure to avoid all LZ4 constants, it should be all packed in the header
static char* LZ4T_unpack(struct SizedBuffer in)
{
    uint32_t magicHeader = __builtin_bswap32(loadVerifyU32(&in));
    uint32_t srcSize = __builtin_bswap32(loadVerifyU32(&in));
    uint16_t shortOffsetMask = loadVerifyU8(&in);
    shortOffsetMask <<= 12;
    int minMatch = loadVerifyU8(&in);
    (void) loadVerifyU16(&in);

    int32_t nibbles = 0;
    int nibblesHandled = 0;
    int largeLiteralsCounts = 0;
    int giganticLiteralsCounts = 0;
    int largeMatchesCounts = 0;
    int giganticMatchesCounts = 0;
    int literalsCounts = 0;
    int matchesCounts = 0;
    int exMatchesCounts = 0;
    int exMatchesAfterLimLiterals = 0;

    char* dst = malloc(srcSize + 16);
    struct SizedBuffer out = { dst, srcSize };
    while (1)
    {
        if (0 == nibbles)
        {
            nibbles = __builtin_bswap32(loadVerifyU32(&in));
            LOG("Loaded new pack of nibbles: %x\n", nibbles);
            if (!nibbles)
            {
                break;
            }
        }

        int tinyMatchLimit = minMatch + 7;
        nibblesHandled++;

        // literals
        if (nibbles < 0)
        {
            LOG("%d: Handle literal nibble 0x%x\n", nibblesHandled, ((uint32_t) nibbles) >> 28);
            // literal
            literalsCounts++;
            int amount = 7 & (nibbles >> 28);
            LOG("Amount: %d\n", amount);
            if (amount == 0)
            {
                largeLiteralsCounts++;
                LOG("Amount is 0, unpacking extras\n");
                amount = LZ4T_unpack_size(&in, &giganticLiteralsCounts) + TINY_LITERAL_LIMIT + 1;
                LOG("Copying amount %d via memcpy: %p %p\n", amount, out.buffer, in.buffer);
                wildCopyVerify(&in, &out, amount);
            }
            else
            {
                uint64_t value = wildLoadU64Verify(&in, amount);
                wildStoreU64Verify(&out, value, amount);
                LOG("Copying amount %d wildly: %p %p\n", amount, out.buffer, in.buffer);
                if (7 == amount)
                {
                    nibbles <<= 4;
                    continue;
                }
                else
                {
                    exMatchesAfterLimLiterals++;
                }
            }

            nibbles <<= 4;
            if (0 == nibbles)
            {
                nibbles = __builtin_bswap32(loadVerifyU32(&in));
                LOG("Loaded new pack of nibbles: %x\n", nibbles);
                if (!nibbles)
                {
                    break;
                }
            }
            nibblesHandled++;
            exMatchesCounts++;
            tinyMatchLimit = minMatch + 15;
        }

        // matches
        {
            LOG("%d: Handle match nibble 0x%x | 0x%x\n", nibblesHandled, ((uint32_t) nibbles) >> 28, tinyMatchLimit);
            matchesCounts++;
            uint16_t offset = __builtin_bswap16(loadVerifyU16(&in));

            uint32_t nibble = 0;
            if (shortOffsetMask)
            {
                nibble = ((uint32_t) (offset & 0xf000)) << 16;
                offset &= 0xfff;
            }
            offset++;

            LOG("Offset: %d, Nibble: 0x%x\n", offset, nibble);
            int amount = minMatch + (((uint32_t) nibbles) >> 28);
            LOG("Amount: %d\n", amount);
            if (amount == tinyMatchLimit)
            {
                LOG("Amount is %d, unpacking extras\n", tinyMatchLimit);
                largeMatchesCounts++;
                amount = LZ4T_unpack_size(&in, &giganticMatchesCounts) + tinyMatchLimit;
            }

            LOG("Copying amount %d: %p %p\n", amount, out.buffer, out.buffer - offset);
            verifySizedBuffer(&out, amount);
            uint8_t* cpy = (uint8_t*) out.buffer + amount;
            LZ4_memcpy_using_offset(out.buffer, (uint8_t*) out.buffer - offset, cpy, offset);
            moveSizedBuffer(&out, amount);

            if (shortOffsetMask)
            {
                LOG("Push in nibble: 0x%x\n", nibble);
                nibbles &= ~0xf0000000;
                nibbles |= nibble;
            }
            else
            {
                nibbles <<= 4;
            }
        }
    }

    STAT("Literals: %d (%d large, %d gigantic)\n", literalsCounts, largeLiteralsCounts, giganticLiteralsCounts);
    STAT("Matches: %d (%d large, %d gigantic)\n", matchesCounts, largeMatchesCounts, giganticMatchesCounts);
    STAT("Ex matches: %d (%d after !7)\n", exMatchesCounts, exMatchesAfterLimLiterals);

    return dst;
}

static void saveBufferToFile(const char* buffer, size_t size, const char* filename);
static void LZ4T_verify(const struct SizedBuffer* orig, const struct SizedBuffer* compressed)
{
#ifdef VERIFIER

    char* dec = LZ4T_unpack(*compressed);

    if (0 != memcmp(dec, orig->buffer, orig->size))
    {
        printf("Compression failed!\n");
        for (int i = 0; i < orig->size; i++)
        {
            if (dec[i] != ((uint8_t*) orig->buffer)[i])
            {
                printf("Mismatch at %d: %x != %x\n", i, dec[i], ((uint8_t*) orig->buffer)[i]);
                saveBufferToFile(dec, orig->size, "dec.bin");
                break;
            }
        }

        abort();
    }

    free(dec);
#else
    (void) orig;
    (void) compressed;
#endif
}

// MARK: LZ4T Block

static void freeSizedBuffer(struct SizedBuffer* data)
{
    free(data->buffer);
}

struct LZ4T_BlockHeader
{
    uint8_t magicHeader[4];
    uint32_t srcSize;
    uint8_t shortOffsetMode;
    uint8_t minMatch;
    uint16_t stub;
    uint32_t firstNibble;
};

static void LZ4T_fillHeader(struct LZ4T_BlockHeader* header, int srcSize, uint32_t firstNibble, bool isShortOffset, uint8_t minMatch)
{
    header->magicHeader[0] = 'L';
    header->magicHeader[1] = 'Z';
    header->magicHeader[2] = '0' + minMatch;
    header->magicHeader[3] = 'T' + isShortOffset;
    header->srcSize = __builtin_bswap32(srcSize);
    header->shortOffsetMode = isShortOffset ? 0xf : 0;
    header->minMatch = minMatch - 1;
    header->stub = 0;
    header->firstNibble = firstNibble;
}

static struct SizedBuffer LZ4T_doCompress(const char* src, int srcSize)
{
    LZ4T_resetGlobals();
    uint32_t firstNibble = 0;
    char* dst = malloc(MAX_COMP_SIZE);
    LZ4_streamHC_t* state = LZ4_createStreamHC();
#ifdef FAVOR_DECOMPRESSION_SPEED
    LZ4_favorDecompressionSpeed(state, 1);
#endif
    LZ4_setCompressionLevel(state, COMPRESSION_LEVEL);  
    sCurrentNibblePendingOutputPtr = &firstNibble;
    LOG("src=%p dst=%p srcSize=%u", src, dst, srcSize);
    int compSize = LZ4_compress_HC_continue(state, (char*)src, dst + sizeof(struct LZ4T_BlockHeader), srcSize, MAX_COMP_SIZE - sizeof(struct LZ4T_BlockHeader));
    LZ4_freeStreamHC(state);
    if (sCurrentNibblePendingOutputPtr)
    {
        printf("Nibble output pointer is not NULL after compression\n");
        abort();
    }
    if (0 == compSize)
    {
        printf("Compression failed!\n");
        abort();
    }

    LZ4T_fillHeader((struct LZ4T_BlockHeader*)dst, srcSize, firstNibble, LZ4T_IS_SHORT_OFFSET, MINMATCH);

#if 0
    char* buf = malloc(8 * 1024 * 1024);
    patch_lz4_block(dst, compSize, buf, 8 * 1024 * 1024);
    free(buf);
#endif

    return (struct SizedBuffer) { dst, compSize + sizeof(struct LZ4T_BlockHeader) };
}

static void LZ4T_setOffsetMode(bool isShort)
{
    LZ4T_distanceMax = isShort ? 4096 : 65535;
}

static void LZ4T_setMinMatchMode(bool isShort)
{
    if (isShort)
    {
        LZ4T_hashMask = 0x00ffffffU;
        LZ4T_minMatch = 3;
    }
    else
    {
        LZ4T_hashMask = 0xffffffffU;
        LZ4T_minMatch = 4;
    }
}

static struct SizedBuffer LZ4T_compress(const struct SizedBuffer* data)
{
    struct SizedBuffer comp = (struct SizedBuffer) { NULL, MAX_COMP_SIZE };

    for (int i = 0; i < 4; i++)
    {
        LZ4T_setMinMatchMode(!!(i & 1));
        LZ4T_setOffsetMode(!!(i & 2));
        struct SizedBuffer newCandidate = LZ4T_doCompress(data->buffer, data->size);
        STAT("Compressed to %d bytes with minMatch=%d, isShortOffset=%d\n", newCandidate.size, MINMATCH, LZ4T_IS_SHORT_OFFSET);

        LZ4T_verify(data, &newCandidate);

        if (newCandidate.size <= comp.size)
        {
            freeSizedBuffer(&comp);
            comp = newCandidate;
        }
        else
        {
            freeSizedBuffer(&newCandidate);
        }
    }

    return comp;
}

// MARK: Tools to work with files and output blocks

static void saveBufferToFile(const char* buffer, size_t size, const char* filename)
{
    FILE* out = fopen(filename, "wb");
    if (out == NULL)
    {
        printf("Cannot create output file '%s'!\n", filename);
        abort();
    }

    fwrite(buffer, size, 1, out);
    fclose(out);
}

static struct SizedBuffer readFile(const char* filename, int margin)
{
    FILE* in = fopen(filename, "rb");
    if (in == NULL)
    {
        printf("Cannot open input file '%s'\n", filename);
        abort();
    }

    fseek(in, 0, SEEK_END);
    int size = ftell(in);
    fseek(in, 0, SEEK_SET);
    char* buffer = malloc(size + margin);
    size_t fread_result = fread(buffer, size, 1, in);
    fclose(in);
    if (1 != fread_result)
    {
        printf("Failed to read input file '%s': %d != %d\n", filename, (int) fread_result, size);
        abort();
    }

    return (struct SizedBuffer) { buffer, size };
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s [SRC_PATH] [DST_PATH]\n", argv[0]);
        return -1;
    }

    struct SizedBuffer src = readFile(argv[1], 0);

    // Compression
    {
        struct SizedBuffer data = LZ4T_compress(&src);
        saveBufferToFile(data.buffer, data.size, argv[2]);
        freeSizedBuffer(&data);
    }

    // Verifier
    {
        struct SizedBuffer compTest = readFile(argv[2], 8 /*need a slight margin for unpacker*/);
        LZ4T_verify(&src, &compTest);
        freeSizedBuffer(&compTest);
    }

    freeSizedBuffer(&src);
    return 0;
}
