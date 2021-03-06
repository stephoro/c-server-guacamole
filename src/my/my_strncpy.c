#include "my.h"
#include <stdlib.h> 

/* I pledge my honor that I have abided by
the Stevens Honor System - Steph Oro */
/*
Same as my_strcpy except: Only copies n chars
or until the end of src *Always copies a '\0'.*
Does not allocate memory. Because always copies 
a ‘/0’, you can assume dst will be at least
length n+1. Treat n<0 as 0, and n>|src| as |src|
*/
char * my_strncpy(char * dst, const char * src, int n){
  if(src == NULL || dst == NULL)
    return dst;
  char * p = dst;
  while((n-- > 0) && *src){
    *(p++) = *(src++);
  }
  *p = 0;
  return dst;
}
