/*
** video.cpp
**
** Defines video i/o non-inline functions.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#include <string.h>
#include <conio.h>
#include "video.h"

int SMX = 80, SMY = 25;
int mono = 0;
HANDLE myconsole;

void screen_init()
{
    CONSOLE_SCREEN_BUFFER_INFO Info;
    SECURITY_ATTRIBUTES security_attrib;

//    FreeConsole();
//    AllocConsole();
/*
    myconsole = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE, 0, &security_attrib,
        CONSOLE_TEXTMODE_BUFFER, NULL);
*/

    myconsole = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(myconsole, &Info);

    SMX = Info.dwSize.X;
    SMY = Info.dwSize.Y;
}

int std_vmenu_cb(TNSCollection* list, String const& S, int& next, int selected)
{
    int old_next = next,
        cur_next = next;
    next = 0;
    int Count = list->getCount();
    if (S.empty() || !Count)
        return selected;
    int first = -1;
    String s = S;
    s.downcase();
    int l = s.getwidth();
    for (int i = 0; i < Count; ++i)
    {
        String elem = (char*)list->at(i);
        elem.downcase();
        char const *t = elem.c_str();
        while (ISSPACE(*t))
            t++;
        if (strncmp(s.c_str(), t, l) == 0)
        {
            if (first == -1)
                first = i;
            if (cur_next == 0)
            {
                next = old_next;
                return i;
            }
            else cur_next--;
        }
    }
    return first != -1? first : selected;
}

vmenu_cb_t vmenu_cb = std_vmenu_cb;
vmenu_key_cb_t vmenu_key_cb = NULL;

char *makestrbuf(char *s, BYTE attr, int &outL)
{
    static char buffer[(VIDEO_BUF_SIZE) * 2];
    int L = 0;
    char *t = buffer;
    while (*s)
    {
        *t++ = *s++;
        *t++ = attr;
        L++;
    }
    outL = L << 1;
    return buffer;
}

void SwapScreen() {}

void writestring(int x, int y, char const *s, BYTE attr)
{
    COORD C;
    C.X = x;
    C.Y = y;
    DWORD Written;
    int L = strlen(s);
    char *T = new char[L+1];
	strcpy(T, s);
	LPCSTR S(T);
	WriteConsoleOutputCharacterA(myconsole, S, L, C, &Written);
    WORD A[512];
    for (int i=0; i< L; i++)
        A[i] = attr;
    WriteConsoleOutputAttribute(myconsole, A, L, C, &Written);
	delete[] T;
}

void clearscreen(BYTE attr)
{
    COORD C;
    C.X = 0, C.Y = 0;
    DWORD Written;
    FillConsoleOutputCharacter(myconsole, ' ', SMX * SMY, C, &Written);
    WORD A[1];
    A[0] = attr;
    FillConsoleOutputAttribute(myconsole, A[0], SMX * SMY, C, &Written);
}

int getsavebufsize(int x1, int y1, int x2, int y2)
{
    return (x2 - x1 + 1) * (y2 - y1 + 1) * 2 + (sizeof(int) * 4);
}

char *savewindow(int x1, int y1, int x2, int y2)
{
    return NULL;
/*fix
    long o = GetVPage() * ((SMY * SMX) << 1) + ((y1 * SMX + x1) << 1);

    char *buf = new char[getsavebufsize(x1, y1, x2, y2)], *oldbuf = buf;

    int const soi = sizeof(int);

    *(int *) (buf + soi * 0) = x1;
    *(int *) (buf + soi * 1) = y1;
    *(int *) (buf + soi * 2) = x2;
    *(int *) (buf + soi * 3) = y2;

    buf += soi * 4;

    int BPWL = (x2 - x1 + 1) << 1;
    int BPSL = SMX << 1;

    for (int i = y1; i <= y2; ++i)
    {
#if defined(__WATCOMC__)
        memcpy((void*)buf, (void*)(screen + o), BPWL);
#elif defined(__DJGPP__)
        movedata(VRAM_Selector, o, _my_ds(), (unsigned)buf, BPWL);
#endif
        buf += BPWL;
        o += BPSL;
    }

    return oldbuf;
*/
}

void restorewindow(char *buf)
{
/*fix
    int const soi = sizeof(int);

    int x1 = *(int *) (buf + soi * 0);
    int y1 = *(int *) (buf + soi * 1);
    int x2 = *(int *) (buf + soi * 2);
    int y2 = *(int *) (buf + soi * 3);

    buf += soi * 4;

    long o = GetVPage() * ((SMY * SMX) << 1) + ((y1 * SMX + x1) << 1);

    int BPWL = (x2 - x1 + 1) << 1;
    int BPSL = SMX << 1;

    for (int i = y1; i <= y2; ++i)
    {
#if defined(__WATCOMC__)
        memcpy((void*)(screen + o), (void*)buf, BPWL);
#elif defined(__DJGPP__)
        movedata(_my_ds(), (unsigned)buf, VRAM_Selector, o, BPWL);
#endif
        buf += BPWL;
        o += BPSL;
    }
*/
}

void plainwindow(int x1, int y1, int x2, int y2, BYTE attr)
{
    int c = x2 - x1 + 1;
    for (int i = y1; i <= y2; ++i)
        writechar(x1, i, ' ', c, attr);
}

void frame(int x1, int y1, int x2, int y2, int frametype, BYTE attr)
{
#ifdef FRAME_GRAPHICAL
    char const *framechars[] = {"Ä³Ú¿ÙÀ", "ÍºÉ»¼È"};
#else
    char const *framechars[] = {"-|----", "=\x13===="};
#endif
    writechar(x1, y1, framechars[frametype][0], x2 - x1 + 1, attr);
    writechar(x1, y2, framechars[frametype][0], x2 - x1 + 1, attr);
    writevchar(x1, y1, framechars[frametype][1], y2 - y1 + 1, attr);
    writevchar(x2, y1, framechars[frametype][1], y2 - y1 + 1, attr);
    writechar(x1, y1, framechars[frametype][2], 1, attr);
    writechar(x2, y1, framechars[frametype][3], 1, attr);
    writechar(x2, y2, framechars[frametype][4], 1, attr);
    writechar(x1, y2, framechars[frametype][5], 1, attr);
}

void window(int x1, int y1, int x2, int y2, int frametype, BYTE attr)
{
    plainwindow(x1, y1, x2, y2, attr);
    frame(x1, y1, x2, y2, frametype, attr);
}

static char *mb_savebuf = NULL;
static WORD mb_oldcursor;
static int mbx1, mbx2, mby1, mby2;
#define MY_WINDOW mbx1, mby1, mbx2, mby2

void messagestart(int center, int frametype, BYTE attr ... )
{
    char *m[70];
    int mL = 0, scount = 0;

    va_list message;
    va_start(message, attr);
    while (1)
    {
        char *tt = va_arg(message, char*);
        if (!tt)
            break;
        if ((int)strlen(tt) > mL)
            mL = strlen(tt);
        m[scount++] = tt;
    };
    va_end(message);

    mbx1 = (SMX - (mL + 2)) / 2;
    mbx2 = mbx1 + (mL + 2) - 1;
    mby1 = (SMY - (scount + 2)) / 2;
    mby2 = mby1 + (scount + 2) - 1;

    messageend();

    mb_savebuf = savewindow(MY_WINDOW);

    window(mbx1, mby1, mbx2, mby2, frametype, attr);

    for (int i = 0; i < scount; ++i)
    {
        int L = strlen(m[i]);
        int x = center ? ((SMX - L) / 2) : (mbx1 + 1);
        writestring(x, mby1 + i + 1, m[i], attr);
    }
    mb_oldcursor = getcursorpos();
    mygotoxy(0, (BYTE)SMY);
}

void messageend()
{
    if (mb_savebuf != NULL)
    {
        restorewindow(mb_savebuf);
        delete[] mb_savebuf;
        mb_savebuf = NULL;
        setcursorpos(mb_oldcursor);
    }
}

WORD messagebox(int center, int frametype, BYTE attr, WORD *keys...)
{
    //va_list message;
    //va_start(message, keys);
    //messagestart(center, frametype, attr, va_arg(message, char*), NULL); //todo

    char *m[70];
    int mL = 0, scount = 0;

    va_list message;
    va_start(message, keys);
    while (1)
    {
        char *tt = va_arg(message, char*);
        if (!tt)
            break;
        if ((int)strlen(tt) > mL)
            mL = strlen(tt);
        m[scount++] = tt;
    };
    va_end(message);

    mbx1 = (SMX - (mL + 2)) / 2;
    mbx2 = mbx1 + (mL + 2) - 1;
    mby1 = (SMY - (scount + 2)) / 2;
    mby2 = mby1 + (scount + 2) - 1;

    messageend();

    WORD oldcursor = getcursorpos();
    mygotoxy(0, (BYTE)SMY);

    mb_savebuf = savewindow(MY_WINDOW);

    window(mbx1, mby1, mbx2, mby2, frametype, attr);

    for (int i = 0; i < scount; ++i)
    {
        int L = strlen(m[i]);
        int x = center ? ((SMX - L) / 2) : (mbx1 + 1);
        writestring(x, mby1 + i + 1, m[i], attr);
    }

    int keyfound = 0;
    WORD tt = 0;
    do
    {
        WORD *t = keys;
        tt = readkey();
        while (*t)
        {
            if (*t++ == tt)
            {
                keyfound = 1;
                break;
            }
        }
    } while (!keyfound);

    mb_oldcursor = oldcursor;
    messageend();

    return tt;
}

int VMenu(TNSCollection *Items,
    int H, int MinLength, int MaxLength,
    int X, int Y,
    bool centerX, bool centerY,
    bool ScrollBar,
    int FT, BYTE A, BYTE IA)
{
    int Count = Items->getCount();
    int i;
    int max_length = 0;
    int Width = MinLength;
    int r = -1;
    int Selected = 0;
    int Top = 0;
    int CurrentChar = 0;
    int Next = 0;
    String SearchStr = "";

    #define item(i)     ((char*)Items->at((i)))
    #define Coordinates mx1, my1, mx2, my2

    for (i = 0; i < Count; ++i)
        max_length = MAX(max_length, (int)strlen(item(i)));

    Width = MIN(MAX(Width, max_length), MaxLength);

    int mx1 = (centerX? (SMX - (Width + 2)) / 2 : X);
    int mx2 = mx1 + (Width + 2) - 1;
    int my1 = (centerY? (SMY - (H + 2)) / 2 : Y);
    int my2 = my1 + (H + 2) - 1;

    WORD OldCursor = getcursorpos();

    char *SB = savewindow(Coordinates);
    window(Coordinates, FT, A);

    WORD Key = 0;

    do
    {

        if (Selected < Top || Selected >= Top + H)
            Top = Selected;

        frame(Coordinates, FT, A);

        for (i = 0; i < H; ++i)
        {
            BYTE CA = (Top + i == Selected? IA : A);
            if (Count && Top + i < Count)
            {
                String Line(item(Top + i));
                int LL = Line.getwidth();
                if (LL < Width)
                    Line += String(' ', Width - LL);
                else if (LL > Width)
                    Line.clip(Width);
                writestring(mx1 + 1, my1 + 1 + i, Line.c_str(), CA);
            }
            else
            {
                writechar(mx1 + 1, my1 + 1 + i, ' ', Width, CA);
            }
        }

        if (Count && ScrollBar && H >= 3)
        {
            writechar(mx2, my1 + 1, 0x1E, 1, A);
            writevchar(mx2, my1 + 2,
#ifdef FRAME_GRAPHICAL
                0xB1,
#else
                0x08,
#endif
                H - 2, A);
            writechar(mx2, my2 - 1, 0x1F, 1, A);
            int o = (int) ( ((Selected + 1) / (double)Count) * (H - 2) );
            if (o == 0 || Selected == 0)
                o = 1;
            writechar(mx2, my1 + 1 + o,
#ifdef FRAME_GRAPHICAL
                0xFE,
#else
                0x01,
#endif
                1, IA);
        }

        if (CurrentChar)
        {
            writestring(mx1 + 1, my2, SearchStr.c_str(), A);
            blockcursor(1);
            mygotoxy((BYTE) (mx1 + CurrentChar + 1), (BYTE) (my2));
        }
        else
        {
            blockcursor(0);
            mygotoxy((BYTE) (mx1 + 1), (BYTE) (my1 + 1 + (Selected - Top)));
        }

        Key = readkey();

        if (vmenu_key_cb)
            Key = (*vmenu_key_cb)(Key);

        if (ischaracter(Key))
        {
            char C = tochar(Key);
            if (Count && SearchStr.getwidth() < MIN(Width, max_length))
            {
                SearchStr += C;
                CurrentChar++;
                Next = 0;
                if (vmenu_cb)
                    Selected = (*vmenu_cb)(Items, SearchStr, Next, Selected);
            }
        }
        else
        {
            switch (Key)
            {
                case kbDown:
                    if (Count && Selected < Count - 1)
                    {
                        Selected++;
                        if (Selected > Top + H - 1)
                            Top++;
                    }
                    break;
                case kbUp:
                    if (Count && Selected > 0)
                    {
                        Selected--;
                        if (Selected < Top)
                            Top--;
                    }
                    break;
                case kbPageDown:
                    if (!Count)
                        break;
                    if (Top + H < Count)
                        Top += H;
                    if (Selected + H < Count)
                        Selected += H;
                    else Selected = Count - 1;
                    if (Top > Selected)
                        Top = Selected;
                    break;
                case kbPageUp:
                    if (!Count)
                        break;
                    if (Top - H >= 0)
                        Top -= H;
                    else Top = 0;
                    if (Selected - H >= 0)
                        Selected -= H;
                    else Selected = 0;
                    if (Selected < Top)
                        Top = 0;
                    break;
                case kbCtrlEnd: case kbCtrlPageDown:
                    if (!Count)
                        break;
                    Selected = Count - 1;
                    Top = MAX(0, Count - H);
                    Next = CurrentChar = 0;
                    SearchStr = "";
                    break;
                case kbCtrlHome: case kbCtrlPageUp:
                    if (!Count)
                        break;
                    Next = Top = Selected = CurrentChar = 0;
                    SearchStr = "";
                    break;
                case kbHome:
                    if (!Count)
                        break;
                    Next = CurrentChar = 0;
                    SearchStr = "";
                    break;
                case kbBackSpace:
                    if (Count && CurrentChar > 0)
                    {
                        CurrentChar--;
                        SearchStr.clip(CurrentChar);
                        Next = 0;
                        if (!CurrentChar)
                            Selected = 0;
                        else if (vmenu_cb)
                            Selected = (*vmenu_cb)(Items, SearchStr, Next, Selected);
                    }
                    break;
/*fix
                case kbCtrlEnter:
                    if (CurrentChar)
                    {
                        if (vmenu_cb)
                        {
                            Next++;
                            Selected = (*vmenu_cb)(Items, SearchStr, Next, Selected);
                        }
                    }
                    break;
*/
            }
        }
    }
    while (!(Key == kbEscape || Key == kbEnter));

    if (Count && Key == kbEnter)
        r = Selected;

    restorewindow(SB);
    delete[] SB;

    setcursorpos(OldCursor);

    return r;
}
