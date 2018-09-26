#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <math.h>
#include <errno.h>
#include <time.h>

#include "vsh_exports.h"


/*
 * This is necessary for the linker to succeed in building the project.
 */




/*
 * Untested and likely broken port of ungetc
 */
inline int ungetc(int c, FILE *stream) {
	if (c<0 || c>255)
    return EOF;

  if (feof(stream)) {
      printf("EOF flag: %d", stream->_Bflag);
  }

  printf("ungetc %d ... ", c);

  /* GNU configure configure wants us to differentiate between an ungetc
   * of the byte that was actually there ("backup ungetc") and a
   * different byte. Sigh. */
  if (stream->_offset) {
    --stream->_offset;
    stream->_Buf[stream->_offset]=c;
  }

  printf("ungetc done (%d)\n", stream->_Bflag);

  //stream->_Bflag&=~(ERRORINDICATOR|EOFINDICATOR);
  return c;
}

inline char * strpbrk(const char *s1, const char *s2)
{
	const char *scanp;
	int c, sc;
  
	while ((c = *s1++) != 0) {
		for (scanp = s2; (sc = *scanp++) != 0;)
			if (sc == c)
				return ((char *)(s1 - 1));
	}
	return (NULL);
}

size_t strspn(const char *p, const char *s)
{
	int i, j;

	for (i = 0; p[i]; i++) {
		for (j = 0; s[j]; j++) {
			if (s[j] == p[i])
				break;
		}
		if (!s[j])
			break;
	}
	return (i);
}

size_t strcspn(const char *s1, const char *s2)
{
	size_t ret = 0;
	while(*s1)
		if(strchr(s2,*s1))
			return ret;
		else
			s1++, ret++;

	return ret;
}

inline double tanh(double x) {return 0;}
inline const char* tmpnam(const char * filename) {return NULL;}
inline int remove(const char *filename) {return cellFsUnlink(filename) != CELL_FS_SUCCEEDED;}
