#include <string.h>
#include <util.h>

/**
 * memset - Fill a region of memory with the given values
 * @dest: Pointer to the start address of memory.
 * @byte: The byte to fill the area with
 * @len: The size of area.
 */
void *memset(void *dest, int byte, size_t len)
{
	char *s = dest;

	while(len--)
		*(s++) = byte;
	return dest;
}

/**
 * memcpy - Copy one area of memory to another
 * @dest: Where to copy to.
 * @src: Where to copy from
 * @len: The size of area memory.
 */
void *memcpy(void *dest, const void *src, size_t len)
{
	char *to = dest;
	const char *from = src;

	if(to == from)
		return dest;

	while(len--)
		*(to++) = *(from++);
	return dest;
}

/**
 * memmove - Copy one area of memory to another
 * @dest: Where to copy to.
 * @src: Where to copy from
 * @len: The size of area memory.
 * Unlike memcpy(), memmove() copies with overlapping area.
 */
#if 0
void *memmove(void *dest, const void *src, size_t len)
{
	char *tmp,*s;

	if(dest == src)
		return dest;

	if(dest <= src) {
		tmp = (char *)dest;
		s   = (char *)src;
		while(len--)
			*(tmp++) = *(s++);
	} else {
		tmp = (char *)dest + len;
		s   = (char *)src + len;
		while(len--)
			*(--tmp) = *(--s);
	}
	return dest;
}
#endif

int memcmp(const void *dest, const void *src, size_t len)
{
	const unsigned char *s1 = dest,*s2 = src;
	int res;

	while(len--) {
		if((res = *(s1++) - *(s2++)) != 0)
			break;
	}
	return res;
}

/**
 * strcpy - Copy a %NUL terminated string
 * @dest: Where to copy string to
 * @src: Where to copy string from
 */
char *strcpy(char *dest, const char *src)
{
	char *d = dest;

	while((*(d++) = *(src++)) != '\0')
		/* nothing */;
	return dest;
}

/**
 * strcmp - Compare two strings, return zero if they are same
 * @s1: One string
 * @s2: Another string
 */
int strcmp(const char *s1, const char *s2)
{
	signed char res;

	while(1) {
		if(((res = *s1 - *(s2++)) != 0) || (!*(s1++)))
			break;
	}
	return res;
}

/**
 * strlen - Find the length of a string
 * @s: The string to be sized
 */
size_t strlen(const char *s)
{
	const char *p = s;

	while (*p != '\0')
		p++;
	return p - s;
}

/**
 * strnlen - Find the length of a string
 * @s: The string to be sized
 */
#if 0
size_t strnlen(const char *s, size_t n)
{
	const char *p = s;

	while (n-- && *p)
		p++;
	return p - s;
}
#endif
#if 0
long atol(const char* str)
{
	long res = 0;
	int sign = 0;

	while (*str == ' ')
		str++;

	if (*str == '-' || *str == '+') {
		sign = *str == '-';
		str++;
	}

	while (*str) {
		res *= 10;
		res += *str++ - '0';
	}

	return sign ? -res : res;
}
#endif
