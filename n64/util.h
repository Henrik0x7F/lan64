#ifndef LAN64_UTIL_H
#define LAN64_UTIL_H

#include "types.h"


/* Sets n bytes at buf to ch */
void* memset(void* dest, int ch, u32 count);

/* Copies count bytes from src to dest, returns dest */
void* memcpy(void* dest, const void* src, u32 count);

/* Copies c string from source to destination. Stops after max_len bytes.
   Destination is always null terminated. */
char* strncpy(char* dest, const char* src, u32 max_len);

/* Returns length of string, excluding terminator.
   Returns max_len if terminator was not found within the first max_len - 1 bytes */
u32 strnlen(const char* str, u32 max_len);

#endif 
