/**
 * @file
 * A collection of stdlib "clones", required by FreeRTOS.
 */

#include <stddef.h>

/* A convenience macro that defines the upper limit of 'size_t' */
#define SIZE_T_MAX     ( (size_t) (-1) )

static inline size_t minval(size_t x, size_t y)
{
    return ( x<=y ? x : y );
}


void* memset(void* ptr, int value, size_t num )
{
    unsigned char* p = (unsigned char*) ptr;
    size_t n = num;

    /* sanity check */
    if ( NULL==ptr )
    {
        goto endf;
    }

    /*
     * If destination block exceeds the range of 'size_t',
     * decrease 'num' accordingly.
     */
    if ( num > (size_t) ((unsigned char*) SIZE_T_MAX - p) )
    {
        n = (unsigned char*) SIZE_T_MAX - p;
        /* TODO or maybe just goto endf???? */
    }

    /* Set 'value' to each byte of the block: */
    while (n--)
    {
        *(p++) = (unsigned char) value;
    }

endf:
    return ptr;
}

void* memcpy(void* destination, const void* source, size_t num )
{
    unsigned char* srcptr = (unsigned char*) source;
    unsigned char* destptr = (unsigned char*) destination;
    size_t n = num;

    /* sanity check */
    if ( NULL==source || NULL==destination )
    {
        return NULL;
    }

    /* Nothing to do if attempting to copy to itself: */
    if ( srcptr == destptr )
    {
        return destination;
    }

    /*
     * If any block exceeds the range of 'size_t',
     * decrease 'num' accordingly.
     */
    if ( num > (size_t) ((unsigned char*) SIZE_T_MAX-destptr) ||
         num > (size_t) ((unsigned char*) SIZE_T_MAX-srcptr) )
    {
        n = minval((unsigned char*) SIZE_T_MAX-destptr,
                   (unsigned char*) SIZE_T_MAX-srcptr);
    }

    if ( destptr<srcptr || destptr>=(srcptr+n) )
    {
        /*
         * If blocks do not overlap or or backwards copy is requested,
         * it is safe to copy the source block from begin to end.
         */
        while (n--)
        {
            *destptr++ = *srcptr++;
        }
    }
    else
    {
        /*
         * If forward copy is requested and blocks overlap, forward copy
         * (from block's begin to end) would cause a corruption.
         * Hence backward copy (from end to begin) is performed.
         */
        srcptr += n - 1;
        destptr += n - 1;

        while (n--)
        {
            *destptr-- = *srcptr--;
        }
    }

    return destination;
}

char* strcpy (char* destination, const char* source)
{
    const char* srcptr = source;
    char* destptr = destination;

    /* sanity check */
    if ( NULL==destination || NULL==source )
    {
        return NULL;
    }

    while ( '\0' != *srcptr )
    {
        *destptr++ = *srcptr++;
    }

    /* Do not forget to append a '\0' at the end of destination! */
    *destptr = '\0';

    return destination;
}

char *
strncpy(char *s1, const char *s2, size_t n)
{
    char *s = s1;
    while (n > 0 && *s2 != '\0') {
    *s++ = *s2++;
    --n;
    }
    while (n > 0) {
    *s++ = '\0';
    --n;
    }
    return s1;
}

int
strncmp(const char *s1, const char *s2, size_t n)
{
    for ( ; n > 0; s1++, s2++, --n)
    if (*s1 != *s2)
        return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
    else if (*s1 == '\0')
        return 0;
    return 0;
}
