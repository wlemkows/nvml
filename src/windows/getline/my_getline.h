#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define MAX_STDIN 100

	size_t my_getline(char **lineptr, size_t *n, FILE *stream);
