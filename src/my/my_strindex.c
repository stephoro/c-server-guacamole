#include "my.h"
#include <stdlib.h> 

/* I pledge my honor that I have abided
by the Stevens Honor System - Steph Oro */
/*
Returns the first index at which the character
is found in the string, or -1 if the character
is not found/input string is null.
*/
int my_strindex(const char * str, char c){
  char tmp = 0;
  if(str == NULL) 
    return -1;
  int len = 0;
  while((tmp = *(str++))){
    if(tmp == c)
      return len;
    len++;
  }
  
  if(c == '\0')
    return len;
        
  return -1;
}
