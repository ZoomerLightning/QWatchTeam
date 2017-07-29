/*
** video.h
**
** Defines video i/o inline functions.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __VIDEO_H__
#define __VIDEO_H__

#define FRAME_GRAPHICAL

#define VIDEO_BUF_SIZE  512

#include <stdarg.h>
#include <string.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "pmt.h"
#include "minmax.h"
#include "keyboard.h"
#include "mystring.h"
#include "tcollect.h"

enum
{
    black = 0, blue, green, cyan, red, purple, brown, lightgrey = 7,
    darkgrey, lightblue, lightgreen, lightcyan,
    lightred, lightpurple, yellow, white = 15
};

#define makeattr(backcolor, forecolor) ((BYTE) (((backcolor) << 4) | (forecolor)))
#define invattr(attr) ((BYTE) ((((attr) & 0xF) << 4) | ((attr) >> 4)))

class xx
{
public:
    int x1, x2;
    xx() :
        x1(0),
        x2(0)
    {}
    xx(int nx1, int nx2) :
        x1(nx1),
        x2(nx2)
    {}
};

extern int SMX, SMY;
extern int mono;
extern HANDLE myconsole;

inline BYTE GetVPage()
{
    return 0;
}

inline WORD GetVColumns()
{
    return 0;
}

inline BYTE GetVRows()
{
    return 0;
}

inline BYTE GetVMode()
{
    return 0;
}

typedef int (*vmenu_cb_t)(TNSCollection*, String const&, int&, int);
typedef WORD (*vmenu_key_cb_t)(WORD);
extern vmenu_cb_t vmenu_cb;
extern vmenu_key_cb_t vmenu_key_cb;
extern "C++" int std_vmenu_cb(TNSCollection*, String const&, int&, int);

extern "C++"
{
    void screen_init();
    char *makestrbuf(char *s, BYTE attr, int &outL);
    void writestring(int x, int y, char const *s, BYTE attr);
    void clearscreen(BYTE attr);
    int getsavebufsize(int x1, int y1, int x2, int y2);
    char *savewindow(int x1, int y1, int x2, int y2);
    void restorewindow(char *buf);

    void SwapScreen();

    void plainwindow(int x1, int y1, int x2, int y2, BYTE attr);
    void frame(int x1, int y1, int x2, int y2, int frametype, BYTE attr);
    void window(int x1, int y1, int x2, int y2, int frametype, BYTE attr);
    void messagestart(int center, int frametype, BYTE attr, ...);
    void messageend();
    WORD messagebox(int center, int frametype, BYTE attr, WORD *keys, ...);
    int VMenu(TNSCollection *Items,
        int H, int MinLength, int MaxLength,
        int X, int Y,
        bool centerX, bool centerY,
        bool ScrollBar,
        int FT, BYTE A, BYTE IA);
};

inline BYTE graphicmodeactive()
{
    return 0;
}

inline void mygotoxy(BYTE x, BYTE y)
{
    COORD CursorPosition;
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(myconsole, CursorPosition);
}

inline WORD getcursorpos(void)
{
    return 0;
}

inline void setcursorpos(WORD t)
{
}

inline void cursoroff()
{
    CONSOLE_CURSOR_INFO CursorInfo;
    GetConsoleCursorInfo(myconsole, &CursorInfo);
    CursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(myconsole, &CursorInfo);
}

inline void cursoron()
{
    CONSOLE_CURSOR_INFO CursorInfo;
    GetConsoleCursorInfo(myconsole, &CursorInfo);
    CursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(myconsole, &CursorInfo);
}

inline void blockcursor(int oui)
{
    CONSOLE_CURSOR_INFO CursorInfo;
    GetConsoleCursorInfo(myconsole, &CursorInfo);
    CursorInfo.dwSize = oui? 100 : 20;
    SetConsoleCursorInfo(myconsole, &CursorInfo);
}

inline void SetVMode(BYTE mode)
{
}

inline void SetVPage(BYTE page)
{
}

inline void setblinkintensity(BYTE blink)
{
}

inline BYTE _getblinkintensity(WORD trigger/*(word 0x463)*/)
{
    return 0;
}

inline void changebufattr(char *buf, int buflen, int x1, int x2, BYTE attr)
{
    if (x1 < 0 || x2 < 0)
        return;
    int mylen = buflen >> 1;
    if (x1 >= mylen)
        return;
    int end = MIN(x2, mylen - 1);
    for (int i = x1; i <= end; i++)
        buf[(i << 1) + 1] = attr;
}

inline void writebuf(int x, int y, char *buf, int buflen)
{
    COORD C;
    C.X = x;
    C.Y = y;
    DWORD Written;
    int L = buflen >> 1;
	WORD A[512];
	char s[512];
    for (int i=0; i < L; i++)
    {
        A[i] = buf[(i << 1) + 1];
        s[i] = buf[(i << 1)];
    }
    WriteConsoleOutputCharacterA(myconsole, LPCSTR(reinterpret_cast<char*>(s)), L, C, &Written);
    WriteConsoleOutputAttribute(myconsole, A, L, C, &Written);
}

inline void writechar(int x, int y, BYTE c, int count, BYTE attr)
{
    COORD C;
    C.X = x, C.Y = y;
    DWORD Written;
    FillConsoleOutputCharacter(myconsole, c, count, C, &Written);
    WORD A[1];
    A[0] = attr;
    FillConsoleOutputAttribute(myconsole, A[0], count, C, &Written);
}

inline void writevchar(int x, int y, BYTE c, int count, BYTE attr)
{
	for (int i = 0; i < count; i++)
        writechar(x, y+i, c, 1, attr);
}

#endif //__VIDEO_H__
