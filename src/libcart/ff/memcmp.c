#include <string.h>

int memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char *c1 = s1;
	const unsigned char *c2 = s2;
	while (n-- > 0)
	{
		unsigned char a = *c1++;
		unsigned char b = *c2++;
		if (a < b) return -1;
		if (a > b) return +1;
	}
	return 0;
}
