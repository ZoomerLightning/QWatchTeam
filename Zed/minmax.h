/*
** minmax.h
**
** Defines some useful templates.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __MINMAX_H__
#define __MINMAX_H__

#undef MIN
#undef MAX
#undef ABS
#undef SIGN
#undef SWAP

template <class atomic>
inline atomic ABS(atomic X)
{
    return ((X >= atomic(0)) ? X : -X);
}

template <class atomic>
inline int SIGN(atomic X)
{
    return ((X > atomic(0)) ? 1 : ((X == atomic(0)) ? 0 : -1));
}

template <class atomic>
inline atomic MAX(atomic X, atomic Y)
{
    return ((X > Y) ? X : Y);
}

template <class atomic>
inline atomic MIN(atomic X, atomic Y)
{
    return ((X < Y) ? X : Y);
}

template <class atomic>
inline void SWAP(atomic &X, atomic &Y)
{
    atomic T = X;
    X = Y, Y = T;
}

#endif //__MINMAX_H__
