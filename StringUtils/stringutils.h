#ifndef __STRINGUTILS_H_
#define __STRINGUTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
 
char* str_split(char** stringp, const char* delim);
  
#ifdef __cplusplus
}
#endif


#endif
