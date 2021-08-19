#include "util.h"


void* lan64_memset(void* dest, int ch, lan64_u32 count)
{
    lan64_u32 i = 0;
    for(; i < count; ++i)
    {
        ((char*)dest)[i] = (char)ch;
    }
    return dest;
}

/* Very naive implementation */
void* lan64_memcpy(void* dest, const void* src, lan64_u32 count)
{
    lan64_u32 i = 0;
    for(; i < count; ++i)
    {
        ((lan64_u8*)dest)[i] = ((const lan64_u8*)src)[i];
    }
    return dest;
}

char* lan64_strncpy(char* dest, const char* src, lan64_u32 max_len)
{
    lan64_u32 i = 0;
    do
    {
        dest[i] = src[i];
        ++i;
    }while(src[i - 1] != 0 && i < max_len);

    dest[max_len - 1] = 0;
    return dest;
}

lan64_u32 lan64_strnlen(const char* str, lan64_u32 max_len)
{
    lan64_u32 len = 0;
    for(; str[len] != '\0' && len < max_len; ++len){}
    return len;
}
