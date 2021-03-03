#ifndef __CP1251_CONVERTER_H_
#define __CP1251_CONVERTER_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int utf8_buf_to_cp1251(const char* utf8, char* windows1251, size_t n);
char* utf8_string_to_cp1251(const char* utf8, char* windows1251);
#ifdef __cplusplus
}
#endif
#endif
