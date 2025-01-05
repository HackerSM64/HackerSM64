#include <string.h>

void *memset(void *s, int c, size_t n)
{
	char *dst = s;
	while (n-- > 0) *dst++ = c;
	return s;
}
