#include "my.h"
#include <stdlib.h> 

/* I pledge my honor that I have abided by
the Stevens Honor System - Steph Oro */
/*
Same as my_strcmp except: Only compares first
n chars of each string with eachother, 
or until the end of either string is reached.
Longer strings are equal to shorter 
strings if the first n chars are equal 
(i.e. abc == abcdefg)
Treat n<0 as 0
*/
int my_strkcmp(const char * a, const char * b, int n){
  char c1, c2;
  if(a == b)
    return 0;
  if(a == NULL)
    return -1;
  if(b == NULL)
    return 1;
  while((n > 0) && (c1 = *a) && (c2 = *b)){
    if(c1 != c2){
      if(c1 < c2){
        return -1;
      }else{
        return 1;
      }
    }
    ++a;
    ++b;
    --n;
  }
  
  return 0;
}
