#include "my.h"
#include <stdlib.h> 

/* I pledge my honor that I have abided
by the Stevens Honor System - Steph Oro */
/*
Copies src onto the end of dst and returns dst
 Does not allocate memory
 Assumes there is enough memory allocated
 in dst to hold both strings
 Overwrites src's original '\0' and
 places a new '\0' onto the end.

 Always returns dst. NULL src returns
 original dst, NULL dst will always return NULL.
*/
char * my_strcat(char * dst, const char * src){
  if(src == NULL || dst == NULL)
    return dst;
  char * p = dst;
  while(*p)
    ++p;
  while(*src){
    *(p++) = *(src++);
  }
  *p = 0;
  return dst;
}
