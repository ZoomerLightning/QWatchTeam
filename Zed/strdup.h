/*
** strdup.h
**
** Defines replacement for non-ansi function strdup().
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __MYSTRDUP_INCLUDED__
#define __MYSTRDUP_INCLUDED__

#include <stdlib.h>

inline char *mystrdup(char const *_s)
{
    if (_s == NULL)
        return 0;
    int L = strlen(_s);
    char *t = new char[L + 1];
    /*char *t = (char*)malloc(L + 1);*/
    memcpy(t, _s, L + 1);
    return t;
}

inline char *mystrdup(char const *_s, int w)
{
    if (_s == NULL)
        return 0;
    char *t = new char[w + 1];
    /*char *t = (char*)malloc(w + 1);*/
    memcpy(t, _s, w);
    t[w] = '\0';
    return t;
}

#endif //__MYSTRDUP_INCLUDED__
