/*
** reserved.h
**
** Defines header for reserved.cc.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __RESERVED_H__
#define __RESERVED_H__

#include "cfg.h"

extern char const *cdef[];
extern char const *cpreprocessor[];
extern char const *crw[];
extern char const *cop[];
extern char const *cfun[];
extern int const cdefcount;
extern int const cppcount;
extern int const crwcount;
extern int const copcount;
extern int const cfuncount;

#ifdef HAS_CMINUSMINUS_PARSER
extern char const *cmmdef[];
extern char const *cmmpreprocessor[];
extern char const *cmmrw[];
extern int const cmmdefcount;
extern int const cmmppcount;
extern int const cmmrwcount;
#endif

#ifdef HAS_NASM_PARSER
extern char const *nasmdef[];
extern char const *nasmpreprocessor[];
extern char const *nasmspecial[];
extern char const *nasmreg[];
extern char const *nasmrw[];
extern int const nasmdefcount;
extern int const nasmppcount;
extern int const nasmspecialcount;
extern int const nasmregcount;
extern int const nasmrwcount;
#endif

#ifdef HAS_PASCAL_PARSER
extern char const *prw[];
extern int const prwcount;
#endif

#ifdef HAS_HASKELL_PARSER
extern char const *haskelldef[];
extern char const *haskellpreprocessor[];
extern char const *haskellrw[];
extern char const *haskellspecial[];
extern int const haskelldefcount;
extern int const haskellppcount;
extern int const haskellrwcount;
extern int const haskellspcount;
#endif

#endif //__RESERVED_H__
