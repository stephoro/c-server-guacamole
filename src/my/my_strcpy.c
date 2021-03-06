#include "my.h"
#include <stdlib.h> 

/* I pledge my honor that I have abided
by the Stevens Honor System - Steph Oro */
/*
Same as my_strcmp except: Only compares
n chars or until the end of a string
Copies the characters from src into dst
overwriting what's already in there.
Always copies a '\0'
Assumes dst has enough memory allocated
for the new string if it is longer.
Does not allocate memory.

Return dst, regardless of input (i.e. if
src is null, return dst, if dst is null, 
return dst, if neither src nor dst are null,
copy src over dst (with the \0), and return dst)
*/
char * my_strcpy(char * dst, const char * src){
  if(src == NULL || dst == NULL)
    return dst;
  char * p = dst;
  while(*src){
    *(p++) = *(src++);
  }
  *p = 0;
  return dst;
}
