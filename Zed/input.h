/*
** input.h
**
** Defines prototypes for input functions.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __INPUT_H__
#define __INPUT_H__

#include "pmt.h"
#include "zedclass.h"
#include "tcollect.h"

//#define HAS_CALCULATOR

#define INPUT_DEF NULL, 0, false, 0

typedef struct
{
    TStringCollection *data;
    int _id;
} History;

extern "C++"
{
    int inputline(char *s, int maxlen, char const *title,
        int (*Action)(zed *, int, int, int, int, ushort&),
        int YAdd, bool CenterX, int DeltaX,
        int _id);
#ifdef HAS_CALCULATOR
    void Calculator();
#endif
}

#endif //__INPUT_H__
