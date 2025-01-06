#if !defined(_WIN32) && !defined(__APPLE__)
 #include <byteswap.h>
#else
#define __bswap_constant_32(x) \
     ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >>  8) |	      \
      (((x) & 0x0000ff00u) <<  8) | (((x) & 0x000000ffu) << 24))

static __inline unsigned int
__bswap_32 (unsigned int __bsx)
{
  return __bswap_constant_32 (__bsx);
}
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>

FILE *fin, *fout;
size_t insize, outsize;
void* data;

FILE* try_open(char* file, char* mode)
{
	FILE* f = fopen(file, mode);
	
	if (!f)
		perror("fopen() failed");

	// remember, if it failed it's returning null anyway
	return f;
}

size_t get_file_size(FILE* f)
{
	size_t cur_pos, file_size;

	cur_pos = ftell(f);
	fseek(f, 0, SEEK_END);
	file_size = ftell(f);
	fseek(f, cur_pos, SEEK_SET);
	return file_size;
}

int main(int argc, char *argv[argc + 1])
{
	uint8_t bytes_to_align;
	uint32_t* size_stored;

	if(argc < 4)
	{
		fputs(
			"Filesizer\n"
			"Usage: [infile] [outfile] [uncompressed size]\n",
			stderr);
		return 1;
	}

	fin = try_open(argv[1], "rb");
	if (!fin)
		return EXIT_FAILURE;

	fout = try_open(argv[2], "wb");
	if (!fout)
		return EXIT_FAILURE;
	
	insize = get_file_size(fin);

	// align to 16 bytes
	bytes_to_align = 0x10 - (insize % 0x10);
	outsize = insize + bytes_to_align + 0x10;

	data = calloc(1, outsize);
	if (!data)
	{
		fputs("Failed to allocate data buffer\n", stderr);
		return EXIT_FAILURE;
	}

	size_t fread_result = fread(data, insize, 1, fin);

	size_stored = data + outsize - 4;
	*size_stored = __bswap_32(atoi(argv[3]));
	
	fwrite(data, outsize, 1, fout);

	free(data);
	fclose(fin);
	fclose(fout);

	return EXIT_SUCCESS;
}