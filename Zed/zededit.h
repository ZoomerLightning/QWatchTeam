/*
** zededit.h
**
** Main header for ZED main module ;-).
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#include <stdio.h>
#include <io.h>
#ifdef unix
#include <unistd.h>
#endif
#include <time.h>
#include <stdlib.h>
#include <new>
#include <signal.h>
#include <process.h>
#include <dos.h>
#include <sys/stat.h>
#include <conio.h>

#include "zconfig.h"

#include "keyboard.h"
#include "video.h"
#include "tcollect.h"
#include "util.h"
#if defined(LOADTIME) || defined(SAVETIME)
#include "timer.cc"
#endif
#include "tok.h"
#include "myfile.h"
#include "zedclass.h"
#include "zeddef.h"

inline char *errorstring(FileException &fe)
{
    static char E[512] = {"test"};
/*    sprintf(E, "(0x%02X=%d)\"%s\"",
        fe.__e.errcode, fe.__e.errcode,
        __perror(fe.__e)
        );*/
    return E;
}
