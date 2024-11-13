// This is free and unencumbered software released into the public domain.

// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

// For more information, please refer to <http://unlicense.org/>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

static void search(int a1, int a2, int *a3, int *a4, uint8_t* data_in);
static int mischarsearch(unsigned char *a1, int a2, unsigned char *a3, int a4);
static void initskip(unsigned char *a1, int a2, unsigned short* skip);

static inline uint32_t bswap32(uint32_t x) {
    return __builtin_bswap32(x);
}

#define DIVIDE_ROUND_UP(a, b) (((a) + (b) - 1) / (b))
#define MAX(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})
#define MIN(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})


typedef struct {
    uint32_t magic;
    uint32_t uncompressed_size;
    uint32_t padding[2];
} Yaz0Header;

// dst is caller-freed memory!
uint32_t compress(uint32_t input_size, uint8_t* src, uint8_t** dst)
{
    // Worst-case size for output is zero compression on the input, meaning the input size plus the number of layout bytes.
    // There would be one layout byte for every 8 input bytes, so the worst-case size is:
    //   input_size + ROUND_UP_DIVIDE(input_size, 8)
    uint8_t* output = calloc(input_size + DIVIDE_ROUND_UP(input_size, 8), sizeof(unsigned char));
    uint8_t* cur_layout_byte = &output[0];
    uint8_t* out_ptr = cur_layout_byte + 1;
    unsigned int input_pos = 0;
    unsigned int cur_layout_bit = 0x80;

    while ( input_pos < input_size ) {
        int group_pos;
        int group_size;

        search(input_pos, input_size, &group_pos, &group_size, src);
        
        // If the group isn't larger than 2 bytes, copying the input without compression is smaller
        if ( group_size <= 2 ) {
            // Set the current layout bit to indicate that this is an uncompressed byte
            *cur_layout_byte |= cur_layout_bit;
            *out_ptr++ = src[input_pos++];
        } else {
            int new_size;
            int new_position;
            
            // Search for a new group after one position after the current one
            search(input_pos + 1, input_size, &new_position, &new_size, src);
            
            // If the new group is better than the current group by at least 2 bytes, use it one instead
            if ( new_size >= group_size + 2 ) {
                // Mark the current layout bit to skip compressing this byte, as the next input position yielded better compression
                *cur_layout_byte |= cur_layout_bit;
                // Copy the input byte to the output
                *out_ptr++ = src[input_pos++];

                // Advance to the next layout bit
                cur_layout_bit >>= 1;

                if ( !cur_layout_bit ) {
                    cur_layout_bit = 0x80;
                    cur_layout_byte = out_ptr++;
                    *cur_layout_byte = 0;
                }

                group_size = new_size;
                group_pos = new_position;
            }

            // Calculate the offset for the current group
            int group_offset = input_pos - group_pos - 1;
            
            // Determine which encoding to use for the current group
            if ( group_size >= 0x12 ) {
                // Three bytes, 0RRRNN
                *out_ptr++ = (group_offset >> 8);
                *out_ptr++ = (group_offset & 0xFF);
                *out_ptr++ = group_size - 0x12;
            } else {
                // Two bytes, NRRR
                *out_ptr++ = (group_offset >> 8) | ((group_size - 2) << 4);
                *out_ptr++ = (group_offset & 0xFF);
            }

            // Move forward in the input by the size of the group
            input_pos += group_size;
        }

        // Advance to the next layout bit
        cur_layout_bit >>= 1;

        if ( !cur_layout_bit ) {
            cur_layout_bit = 0x80;
            cur_layout_byte = out_ptr++;
            *cur_layout_byte = 0;
        }
    }

    if ( cur_layout_bit != 0x80 ) {
        out_ptr++;
    }
    
    *dst = output;
    return out_ptr - output;
}

#define MAX_OFFSET 0x1000
#define MAX_SIZE 0x111

static void search(int input_pos, int input_size, int *pos_out, int *size_out, uint8_t* data_in)
{
    // Current group size
    int cur_size = 3;
    // Current position being searched
    int search_pos = MAX(input_pos - MAX_OFFSET, 0);
    // Number of bytes to search for
    int search_size = MIN(input_size - input_pos, MAX_SIZE);
    // Position of the current group
    int found_pos = 0;
    // Offset from the search pos that the group starts at
    int found_offset;

    if (search_size >= 3) {
        while (input_pos > search_pos) {
            found_offset = mischarsearch(&data_in[input_pos], cur_size, &data_in[search_pos], cur_size + input_pos - search_pos);

            if (found_offset >= input_pos - search_pos) {
                break;
            }

            while (search_size > cur_size) {
                if (data_in[cur_size + search_pos + found_offset] != data_in[cur_size + input_pos]) {
                    break;
                }

                cur_size++;
            }

            if (search_size == cur_size) {
                *pos_out = found_offset + search_pos;
                *size_out = cur_size;
                return;
            }

            found_pos = search_pos + found_offset;
            search_pos = found_pos + 1;
            ++cur_size;
        }

        *pos_out = found_pos;

        if (cur_size > 3) {
            *size_out = cur_size - 1;
        } else {
            *size_out = 0;
        }
    } else {
        // Not enough room to find a group
        *size_out = 0;
        *pos_out = 0;
    }
}

static int mischarsearch(unsigned char *pattern, int patternlen, unsigned char *data, int datalen)
{
    static unsigned short skip[256]; // idb
    int result; // eax
    int i; // ebx
    int v6; // eax
    int j; // ecx

    result = datalen;
    if (patternlen <= datalen) {
        initskip(pattern, patternlen, skip);
        i = patternlen - 1;

        while (true) {
            if (pattern[patternlen - 1] == data[i]) {
                --i;
                j = patternlen - 2;
                
                if ( patternlen - 2 < 0 ) {
                    return i + 1;
                }

                while (pattern[j] == data[i]) {
                    j--;
                    i--;
                    if (j < 0) {
                        return i + 1;
                    }
                }
                v6 = patternlen - j;

                if (skip[data[i]] > patternlen - j) {
                    v6 = skip[data[i]];
                }
            } else {
                v6 = skip[data[i]];
            }

            i += v6;
        }
    }
    return result;
}

static void initskip(unsigned char *pattern, int len, unsigned short* skip)
{
    for (int i = 0; i < 256; i++) {
        skip[i] = len;
    }
    
    for (int i = 0; i < len; i++) {
        skip[pattern[i]] = len - i - 1;
    }
}

void naive_copy(void* dst, void* src, int size) {
    uint8_t *cur_out = dst;
    uint8_t *cur_in = src;
    
    while (size--) {
        *cur_out++ = *cur_in++;
    }
}

void decompress(uint32_t uncompressedLength, uint32_t compressedSize, uint8_t* srcPtr, uint8_t* dstPtr) {
    int32_t layoutBitIndex;
    uint8_t *srcEnd;
    uint8_t *dstEnd;
    uint8_t layoutBits;

    srcEnd = srcPtr + compressedSize;
    dstEnd = dstPtr + uncompressedLength;

    while (srcPtr < srcEnd) {
        layoutBitIndex = 0;
        layoutBits = *srcPtr++;

        while (layoutBitIndex < 8 && srcPtr < srcEnd && dstPtr < dstEnd) {
            if (layoutBits & 0x80) {
                *dstPtr++ = *srcPtr++;
            } else {
                int32_t firstByte = *srcPtr++;
                int32_t secondByte = *srcPtr++;
                uint32_t bytes = firstByte << 8 | secondByte;
                uint32_t offset = (bytes & 0x0FFF) + 1;
                uint32_t length;

                // Check how the group length is encoded
                if ((firstByte & 0xF0) == 0) {
                    // 3 byte encoding, 0RRRNN
                    int32_t thirdByte = *srcPtr++;
                    length = thirdByte + 0x12;
                } else {
                    // 2 byte encoding, NRRR
                    length = ((bytes & 0xF000) >> 12) + 2;
                }

                naive_copy(dstPtr, dstPtr - offset, length);
                dstPtr += length;
            }

            layoutBitIndex++;
            layoutBits <<= 1;
        }
    }
}

int do_decompress(const char* input_path, const char* output_path) {
    // Open the input file
    FILE* input_file = fopen(input_path, "rb");
    if (input_file == NULL) {
        fprintf(stderr, "Failed to open input file: %s\n", input_path);
        return EXIT_FAILURE;
    }

    // Read the input file length
    size_t input_file_size;
    fseek(input_file, 0, SEEK_END);
    input_file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    // Read and validate the Yaz0 header
    Yaz0Header header;
    fread(&header, sizeof(Yaz0Header), 1, input_file);

    if (memcmp(&header.magic, "Yaz0", sizeof(header.magic)) != 0) {
        fprintf(stderr, "Invalid Yaz0 file: %s\n", input_path);
        return EXIT_FAILURE;
    }

    header.uncompressed_size = bswap32(header.uncompressed_size);

    // Allocate input and output buffers and read the file into the input
    uint8_t* input_data = calloc(sizeof(uint8_t), input_file_size - sizeof(Yaz0Header));
    uint8_t* output_data = calloc(sizeof(uint8_t), header.uncompressed_size);
    fread(input_data, 1, input_file_size - sizeof(Yaz0Header), input_file);
    fclose(input_file);

    // Decompress
    decompress(header.uncompressed_size, input_file_size - sizeof(Yaz0Header), input_data, output_data);
    free(input_data);

    // Write the output file
    FILE* output_file = fopen(output_path, "wb");
    fwrite(output_data, 1, header.uncompressed_size, output_file);
    fclose(output_file);
    free(output_data);

    return EXIT_SUCCESS;
}

int do_compress(const char* input_path, const char *output_path) {
    // Open the input file
    FILE* input_file = fopen(input_path, "rb");
    if (input_file == NULL) {
        fprintf(stderr, "Failed to open input file: %s\n", input_path);
        return EXIT_FAILURE;
    }

    // Read the input file length
    size_t input_file_size;
    fseek(input_file, 0, SEEK_END);
    input_file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    // Allocate the input buffers and read the file into the input
    uint8_t* input_data = calloc(input_file_size, sizeof(uint8_t));
    fread(input_data, 1, input_file_size, input_file);
    fclose(input_file);

    // Compress
    uint8_t* output_data;
    uint32_t compressed_size = compress(input_file_size, input_data, &output_data);
    free(input_data);

    // Create the Yaz0 header
    Yaz0Header header;
    memcpy(&header.magic, "Yaz0", sizeof(header.magic));
    header.padding[0] = 0;
    header.padding[1] = 0;
    header.uncompressed_size = bswap32(input_file_size);

    // Write the output file
    FILE* output_file = fopen(output_path, "wb");
    fwrite(&header, 1, sizeof(Yaz0Header), output_file);
    fwrite(output_data, 1, compressed_size, output_file);
    fclose(output_file);
    free(output_data);

    return EXIT_SUCCESS;
}

void print_usage(const char* prog_name) {
    printf("Usage: %s [flags (optional)] [input file] [output file]\n", prog_name);
    printf("  Flags: -d (decompress)\n");
}

int main(int argc, char** argv) {
    bool decompression_mode = false;

    if (argc < 3 || argc > 4) {
        print_usage(argv[0]);
        return EXIT_SUCCESS;
    }

    if (argc == 4) {
        if (argv[1][0] == '-') {
            if (strncmp(argv[1], "-d", 3) == 0) {
                decompression_mode = true;
            } else {
                fprintf(stderr, "Unknown flag: %s\n", argv[1]);
                return EXIT_FAILURE;
            }
        } else {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }
    }

    if (decompression_mode) {
        return do_decompress(argv[2], argv[3]);
    } else {
        return do_compress(argv[1], argv[2]);
    }
}
