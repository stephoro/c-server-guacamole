#include "my.h"
#include <stdlib.h> 

/* I pledge my honor that I have abided
by the Stevens Honor System - Steph Oro */
/*
Returns the last index at which the character
is found in the string or -1 if the character
is not found/input string is null.
*/
int my_strrindex(const char * str, char c){
  char tmp = 0;
  if(str == NULL) 
    return -1;
  int len = 0;
  int index = -1;
  while((tmp = *(str++))){
    if(tmp == c)
      index = len;
    len++;
  }
  
  if(c == '\0')
    index = len;
        
  return index;
}
