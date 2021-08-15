#include "util.h"


void* memset(void* dest, int ch, u32 count)
{
    u32 i = 0;
    for(; i < count; ++i)
    {
        ((char*)dest)[i] = (char)ch;
    }
    return dest;
}

/* Very naive implementation */
void* memcpy(void* dest, const void* src, u32 count)
{
    u32 i = 0;
    for(; i < count; ++i)
    {
        ((u8*)dest)[i] = ((const u8*)src)[i];
    }
    return dest;
}

char* strncpy(char* dest, const char* src, u32 max_len)
{
    u32 i = 0;
    do
    {
        dest[i] = src[i];
        ++i;
    }while(src[i - 1] != 0 && i < max_len);

    dest[max_len - 1] = 0;
    return dest;
}

u32 strnlen(const char* str, u32 max_len)
{
    u32 len = 0;
    for(; str[len] != '\0' && len < max_len; ++len){}
    return len;
}
