/*
** pmt.h
**
** Defines some useful typedefs.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __PROTECTEDMODETYPES_H__
#define __PROTECTEDMODETYPES_H__

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

typedef BYTE byte;
typedef WORD word;
typedef DWORD dword;

typedef unsigned char u1;
typedef unsigned short u2;
typedef unsigned long u4;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

typedef unsigned char UBYTE;
typedef unsigned short UWORD;
typedef unsigned long UDWORD;
typedef signed char SBYTE;
typedef signed short SWORD;
typedef signed long SDWORD;

#ifdef __DJGPP__
#ifndef __dj_ENFORCE_ANSI_FREESTANDING
#ifndef __STRICT_ANSI__
#ifndef _POSIX_SOURCE

typedef unsigned long long QWORD;
typedef unsigned long long UQWORD;
typedef unsigned long long u8;
typedef long long SQWORD;

typedef unsigned long long u8;

#endif /* !_POSIX_SOURCE */
#endif /* !__STRICT_ANSI__ */
#endif /* !__dj_ENFORCE_ANSI_FREESTANDING */
#endif /* __DJGPP__ */

#endif //__PROTECTEDMODETYPES_H__
