#ifndef LAN64_UTIL_H
#define LAN64_UTIL_H

#include "types.h"


/* Sets n bytes at buf to ch */
void* lan64_memset(void* dest, int ch, lan64_u32 count);

/* Copies count bytes from src to dest, returns dest */
void* lan64_memcpy(void* dest, const void* src, lan64_u32 count);

/* Copies c string from source to destination. Stops after max_len bytes.
   Destination is always null terminated. */
char* lan64_strncpy(char* dest, const char* src, lan64_u32 max_len);

/* Returns length of string, excluding terminator.
   Returns max_len if terminator was not found within the first max_len - 1 bytes */
lan64_u32 lan64_strnlen(const char* str, lan64_u32 max_len);

#endif 
