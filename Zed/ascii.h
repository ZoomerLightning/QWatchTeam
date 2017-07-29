/*
** ascii.h
**
** Defines some inline functions to work with ASCII and russian characters.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __ASCII_H__
#define __ASCII_H__

#include <stddef.h>
#include <string.h>
#include "pmt.h"

#define __ASCII_RUSSIAN__
#define _BYTE   unsigned char

#ifdef FREE
#undef __ASCII_RUSSIAN__
#endif

#ifndef __ASCII_RUSSIAN__
#define ISALPHA         _ISALPHA
#define ISALNUM         _ISALNUM
#define ISLOWER         _ISLOWER
#define ISUPPER         _ISUPPER
#define TOLOWER         _TOLOWER
#define TOUPPER         _TOUPPER
#define STRLWR          _STRLWR
#define STRUPR          _STRUPR
#endif

inline bool ISASCII(char c)
{
    return (((_BYTE)c & 0x80) == 0);
}

inline bool ISPRINTABLEASCII(char c)
{
    return (ISASCII(c) && (_BYTE)c >= ' ');
}

inline bool ISWHITESPACE(char c)
{
    return ((_BYTE)c == ' ' || (_BYTE)c == '\t');
}

inline bool ISSPACE(char c)
{
    return (strchr("\r\n\f\t\v ", (_BYTE)c) != NULL);
}

inline bool ISDIGIT(char c)
{
    return ((_BYTE)c >= '0' && (_BYTE)c <= '9');
}

inline bool ISBDIGIT(char c)
{
    return ((_BYTE)c == '0' || (_BYTE)c == '1');
}

inline bool ISODIGIT(char c)
{
    return ((_BYTE)c >= '0' && (_BYTE)c <= '7');
}

inline bool ISRDIGIT(char c)
{
    return (strchr("MDCLXVImdclxvi", (_BYTE)c) != NULL);
}

inline bool ISXDIGIT(char c)
{
    return (ISDIGIT(c) || strchr("ABCDEFabcdef", (_BYTE)c) != NULL);
}

#ifdef __ASCII_RUSSIAN__

inline bool ISALPHA(char c)
{
    return ((_BYTE)c >= 'A' && (_BYTE)c <= 'Z')
        || ((_BYTE)c >= 'a' && (_BYTE)c <= 'z')
        || ((_BYTE)c >= 0x80 && (_BYTE)c <= 0xaf)
        || ((_BYTE)c >= 0xe0 && (_BYTE)c <= 0xef);
}

inline bool ISALNUM(char c)
{
    return ISALPHA(c) || ISDIGIT(c);
}

inline bool ISLOWER(char c)
{
    return ((_BYTE)c >= 'a' && (_BYTE)c <= 'z')
        || ((_BYTE)c >= 0xa0 && (_BYTE)c <= 0xaf)
        || ((_BYTE)c >= 0xe0 && (_BYTE)c <= 0xef);
}

inline bool ISUPPER(char c)
{
    return ((_BYTE)c >= 'A' && (_BYTE)c <= 'Z')
        || ((_BYTE)c >= 0x80 && (_BYTE)c <= 0x9f);
}

inline char TOLOWER(char c)
{
    if ((_BYTE)c >= 'A' && (_BYTE)c <= 'Z')
        return (char) ((_BYTE)c + ('a' - 'A'));
    if ((_BYTE)c >= 0x80 && (_BYTE)c <= 0x8f)
        return (char) ((_BYTE)c + 0x20);
    if ((_BYTE)c >= 0x90 && (_BYTE)c <= 0x9f)
        return (char) ((_BYTE)c + 0x50);
    return c;
}

inline char TOUPPER(char c)
{
    if ((_BYTE)c >= 'a' && (_BYTE)c <= 'z')
        return (char) ((_BYTE)c - ('a' - 'A'));
    if ((_BYTE)c >= 0xa0 && (_BYTE)c <= 0xaf)
        return (char) ((_BYTE)c - 0x20);
    if ((_BYTE)c >= 0xe0 && (_BYTE)c <= 0xef)
        return (char) ((_BYTE)c - 0x50);
    return c;
}

inline char *STRLWR(char *string)
{
    if (string == NULL)
        return NULL;
    for (char *ptr = string; *ptr; ++ptr)
        *ptr = TOLOWER(*ptr);
    return string;
}

inline char *STRUPR(char *string)
{
    if (string == NULL)
        return NULL;
    for (char *ptr = string; *ptr; ++ptr)
        *ptr = TOUPPER(*ptr);
    return string;
}

#endif /*__ASCII_RUSSIAN__*/

/*------------------------------------------------------------------------------------------------*/

inline bool _ISALPHA(char c)
{
    return ((_BYTE)c >= 'A' && (_BYTE)c <= 'Z')
        || ((_BYTE)c >= 'a' && (_BYTE)c <= 'z');
}

inline bool _ISALNUM(char c)
{
    return _ISALPHA(c) || ISDIGIT(c);
}

inline bool _ISLOWER(char c)
{
    return ((_BYTE)c >= 'a' && (_BYTE)c <= 'z');
}

inline bool _ISUPPER(char c)
{
    return ((_BYTE)c >= 'A' && (_BYTE)c <= 'Z');
}

inline char _TOLOWER(char c)
{
    if (_ISUPPER(c))
        return (char) ((_BYTE)c + ('a' - 'A'));
    return c;
}

inline char _TOUPPER(char c)
{
    if (_ISLOWER(c))
        return (char) ((_BYTE)c - ('a' - 'A'));
    return c;
}

inline char *_STRLWR(char *string)
{
    if (string == NULL)
        return NULL;
    for (char *ptr = string; *ptr; ++ptr)
        *ptr = _TOLOWER(*ptr);
    return string;
}

inline char *_STRUPR(char *string)
{
    if (string == NULL)
        return NULL;
    for (char *ptr = string; *ptr; ++ptr)
        *ptr = _TOUPPER(*ptr);

    return string;
}

#endif //__ASCII_H__
