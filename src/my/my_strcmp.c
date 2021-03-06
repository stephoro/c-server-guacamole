#include "my.h"
#include <stdlib.h> 

/* I pledge my honor that I have abided
by the Stevens Honor System - Steph Oro */
/*
 Compares strings by ascii value
 If a and b are identical, return 0. 
 if a < b, return negative number 
 if a > b, return positive number
 Two NULL pointers are equal.
 NULL pointer is always less than a normal string
*/
int my_strcmp(const char * a, const char * b){
  char c1, c2;
  if(a == b)
    return 0;
  if(a == NULL)
    return -1;
  if(b == NULL)
    return 1;
  while((c1 = *a) && (c2 = *b)){
    if(c1 != c2){
      if(c1 < c2){
        return -1;
      }else{
        return 1;
      }
    }
    ++a;
    ++b;
  }
  
  if(*a == *b)
    return 0;
  
  if(*b)
    return -1;
  return 1;
}
