/*
** zed.cpp
**
** zed editor main module.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#include <new>

#include "hl.h"
#include "zededit.h"
#include "readconf.h"
#include "dynarr.h"
#include "cmt.h"
#include "mytimer.h"
#include "myfile.h"
//fix #include "stlstuff.h"
#include "shutdown.h"
#include <cstdio>

//#define STRESS
#define USE_SYSTEM

#ifdef STRESS
#include "stress.h"
#endif

#include <assert.h>
static void dummy() //todo: optimizing strips away copyright info :-(
{
}

//#define LOG_ZED

#ifdef LOG_ZED
#include <fstream.h>
fstream log;

inline ostream& operator << (ostream  &__strm, bool const &b)
{
    return (__strm << ((b == true) ? "true" : "false"));
}
#endif

zedConfig Config;
int NewConfig = 0;
int ruleractive = 0;
int rulerX = -1;

String __tmpdir;

char help[] = "\nusage: " prgname " filetoedit [filetoedit...]\n";

#define SEARCH_STR_LEN      1025

#if defined(LOADTIME) || defined(SAVETIME)
simple_timer Timer;
#endif
char messbuf[512];
char searchstr[SEARCH_STR_LEN] = {0};
char replacestr[SEARCH_STR_LEN] = {0};
int const optlen = 13;
String lastsearchopt, tempsearchopt;
int STARTY = 1;
int STARTX = 1;
int DELTAY = 1;
int DELTAX = 1;

int gocol  = -1;
int goline = -1;

TNSCollection *editors = NULL;
int editorscount = 0;
int currenteditor = 0;

bool show_comments = true;
bool coloring = true;
bool status = true;

char *scrbuf = NULL;
WORD oldcursor;
BYTE oldpage;
BYTE oldblink = 1;
BYTE system_oldblink = 1;

//
// -1 -- unknown
//  0 --    1 --    2 --    3 --    4 --    5 --    6 --    7 --    8 --
//  C/C++   ASM     C--     Pascal  IDC     Haskell ADA
//
char *ftarr[] =
{
    "undef",
    "c",
    "asm",
    "c--",
    "pascal",
    "idc",
    "haskell",
    "ada"
};

WORD keyYN[]        = {kbY, kbN, kbShiftY, kbShiftN, 0};
WORD keyYNEsc[]     = {kbY, kbN, kbShiftY, kbShiftN, kbEscape, 0};
WORD keyYNACEsc[]   = {kbY, kbN, kbA, kbC, kbShiftY, kbShiftN, kbShiftA, kbShiftC, kbEscape, 0};
WORD keyEsc[]       = {kbEscape, 0};
WORD keyEscF1[]     = {kbEscape, kbF1, 0};
WORD keyEscF10[]    = {kbEscape, kbF10, 0};

//enum {
//    black = 0, blue, green, cyan, red, purple, brown, lightgrey = 7,
//    darkgrey, lightblue, lightgreen, lightcyan,
//    lightred, lightpurple, yellow, white = 15
//};

int const Background = blue;
int const Foreground = lightgrey;

BYTE
    atNormalText        = makeattr(Background, Foreground),
    atSelect            = makeattr(Foreground, Background),
    atStatusLine        = makeattr(cyan, black),
    atMainFrame         = makeattr(black, cyan),

    atKeyWord           = makeattr(Background, white),
    atFunction          = makeattr(Background, lightcyan),
    atSpecial           = makeattr(yellow, blue),
    atLabel             = makeattr(lightgrey, lightblue),
    atSyntax            = makeattr(Background, cyan),
    atInvalid           = makeattr(white, red),

    atOctal             = makeattr(Background, lightred),
    atHex               = makeattr(Background, yellow),
    atDecimal           = makeattr(Background, lightblue),
    atFloat             = makeattr(Background, lightpurple),
    atBinary            = makeattr(Background, lightgreen),

    atChar              = makeattr(Background, yellow),
    atString            = makeattr(Background, brown),
    atPreprocess        = makeattr(Background, lightblue),
    atPreprocessUnknown = makeattr(lightpurple, lightblue),
    atStdinclude        = makeattr(Background, yellow),
    atComment           = makeattr(Background, green),

    atTitle             = makeattr(lightgrey, blue),

    atMBNormal          = makeattr(lightgrey, black),
    atMBSave            = makeattr(white, blue),
    atMBHelp            = makeattr(lightgrey, blue),
    atMBError           = makeattr(red, lightgrey),

    atRulerN            = makeattr(white, red),
    atRulerI            = makeattr(red, white)
    ;

#include "input.h"

bool display_busy = false;
void my_keyb_callback()
{
    if (display_busy)
        return;
    if (Config.debug)
    {
        char buf[25];
        sprintf(buf, "%lu", /*__malloc_get_bytes_in_use()*/0);
        writestring(1, SMY - 1, buf, atStatusLine);
    }
}
void (*keyb_callback)() = my_keyb_callback;

inline zed *editor()
{
    return (zed*)(editors->at(currenteditor));
}

void processing()
{
    messagestart(1, 0, atMBNormal, " *** Processing... *** ", NULL);
}

void printstatus(char *t)
{
    char s[VIDEO_BUF_SIZE];
    memset(s, ' ', VIDEO_BUF_SIZE);
    if (t && *t)
        memcpy(s, t, MIN((int)strlen(t), (int)SMX));
    s[SMX] = 0;
    writestring(0, SMY - 1, s, atStatusLine);
}

void printError(FileException &fe)
{
    sprintf(messbuf, "Info: %s", errorstring(fe));
    printstatus(messbuf);
}

void memoryerror()
{
    messagebox(1, 0, atMBError, keyEsc, " Not enough memory to complete operation. Hit Esc ", NULL);
    flushkeyboard();
}

void crash()
{
    static int counter = 0;
    if (++counter > 1)
        return;
    if (system_oldblink)
        setblinkintensity(system_oldblink);
    SetVPage(oldpage);
    blockcursor(0);
    mygotoxy(0, 2);
    if (editors)
    {
        int n = 0, i;
        for (i = 0; i < editorscount; i++)
        {
            if (((zed *) editors->at(i))->modified)
                n++;
        }
        if (!n)
            return;
        fputs("\n" prgname ": sorry, i'm crashed, and now trying to save your work..\n", stderr);
        fputs("btw, save modified files? y/enter/n/esc:", stderr);
        flushkeyboard();
        mydelay(500);//todo: 3 sec.
        flushkeyboard();
        while (1)
        {
            switch (readkey())
            {
                case kbEscape:
                case kbN:
                case kbShiftN:
                    fputc('\n', stderr);
                    return;
                case kbEnter:
                case kbY:
                case kbShiftY:
                    goto s;
            }
        }
s:
        fputc('\n', stderr);
        if (scrbuf)
            delete[] scrbuf;
        zed *t;
        for (i = 0; i < editorscount; i++)
        {
            t = (zed *) editors->at(i);
            if (!t->modified)
                continue;
            try
            {
                t->write_file(t->getfilename());
            }
            catch (FileException fe)
            {
                fprintf(stderr, "\"%s\" -- write error (%s)\n",
                    fe.FileName,
                    errorstring(fe));
                char buf[cMaxFileNameLength + 1];
                do
                {
                    fprintf(stderr, "%s", "Enter new file name (" INPUTSIG1 " to exit save cycle, " INPUTSIG2 " to skip this file)");
                    flushkeyboard();
                    fscanf(stdin, "%s", buf);
                } while (!*buf);
                if (!strcmp(buf, INPUTSIG1))
                    break;
                else if (!strcmp(buf, INPUTSIG2))
                    continue;
                else
                {
                    t->changefilename(buf);
                    i--;
                    continue;
                }
            }
            catch (zed::InternalError) {}
            catch (exceptionMemExhausted) {}
            catch (...) {}
        }
    }
}

void quit(char *msg, int errorcode, int restorescr, int cleanup = 1)
{
    if (restorescr)
        SetVPage(oldpage);
    if (system_oldblink && restorescr)
        setblinkintensity(system_oldblink);
    if (scrbuf)
    {
        if (restorescr)
            restorewindow(scrbuf);
        if (cleanup)
            delete[] scrbuf;
    }
    blockcursor(0);
    if (restorescr)
        setcursorpos(oldcursor);

    if (msg && *msg)
        fprintf(stderr, msg);
    if (cleanup)
        exit(errorcode);
    else _exit(errorcode);
}

void prepareline(char *buf, int resx, String const &message)
{
    memset(buf, ' ', resx);
    buf[resx] = 0;
    if (!message.empty())
        memcpy(buf, message.c_str(), message.getwidth());
}

void printruler(int X, int Y, int Size)
{
    char t[VIDEO_BUF_SIZE];
    int i, j = 1, k;
    for (i = k = 0; i < Size; ++i, k += 2)
    {
        int Q = (i % 10) + 1;
        int D = (Q == 10)? ((j++) % 10) : Q;
        BYTE a = (BYTE) ((Q == 10)? atRulerI : atRulerN);
        t[k    ] = (char) (D + '0');
        t[k + 1] = a;
    }
    writebuf(X, Y, t, Size << 1);
}

void prepare_status_line(char *buf, int ft, zed &e)
{
    int const sl_max = SMX - 2;
    String status_line = "";

    if (status)
    {
        time_t now = time (NULL);
        struct tm *T = localtime (&now);
        char T_buf[100];
        char const *format = SMX >= 132? Config.status_time_format_long : Config.status_time_format_short;
        strftime (T_buf, sizeof(T_buf), format, T);

        char const *matptr = "win32";

        String file_name = getcomponent(e.getfilename(), 1|2|4|8);

        sprintf(messbuf, "L %06d", e.getvy() + 1);
        status_line += messbuf;
        sprintf(messbuf, " [%02d%%]", (int) (((e.getvy() + 1) * 100.) / e.getlinecount()));
        status_line += messbuf;
        sprintf(messbuf, " C %06d", e.getvx() + 1);
        status_line += messbuf;
        sprintf(messbuf, "  %s", T_buf);
        status_line += messbuf;
/*fix
        DWORD __memavail = memavail();

        {
            char measured_in = 'k';
            double os_avail_mem = __memavail;
            if (!Config.debug && __memavail >= 1024)
                os_avail_mem = __memavail / 1024., measured_in = 'M';
            sprintf(messbuf, "  [%.0f%c(%lu),%s,%s]",
                os_avail_mem,
                measured_in,
                Config.debug? __malloc_get_bytes_in_use() : 0LU,
                ((__dpmi_get_flags() & 4) ? "v" : "nv"),
                matptr);
            status_line += messbuf;
        }
*/
        sprintf(messbuf, "  %d(%d)", currenteditor + 1, editorscount);
        status_line += messbuf;
        sprintf(messbuf, "  {%s}", ftarr[ft + 1]);
        status_line += messbuf;
        sprintf(messbuf, "  %c", e.modified? '*' : ' ');
        status_line += messbuf;
        Fold_Filename(file_name, MAX(0, sl_max - status_line.getwidth() - 2));
        sprintf(messbuf, "\"%s\"", file_name.c_str());
        status_line += messbuf;

        status_line.clip(sl_max);
    }
    prepareline(buf, sl_max, status_line);
}

void _display(zed &e, bool outtext, bool preserve_bot)
{
    static char buf[VIDEO_BUF_SIZE],
        statusline[VIDEO_BUF_SIZE];

    int ft = getfiletype(e.getfilename());

    int X = e.pagex() + STARTX;
    int Y = e.pagey() + STARTY;

    int rX = (rulerX == -1)? X : rulerX;
    int rY = (Y - 1)? (Y - 1) : (Y + 1);

    BYTE frameattr = atMainFrame;

    prepare_status_line(statusline, ft, e);

    if (outtext)
    {
        blockcursor(Config.block_insert? e.insert_state : e.insert_state ^ 1);
        mygotoxy((BYTE) X, (BYTE) Y);

        int const
            DIM_Y = SMY - (DELTAY << 1),
            DIM_X = SMX - (DELTAX << 1);

        bool **comment_area;
        if (Config.multilinecomments)
        {
            alloc_ar(comment_area, DIM_Y, DIM_X);
            fill_ar(comment_area, DIM_Y, DIM_X, false);
            if (show_comments && coloring)
                Cmt::create_comment_area(comment_area, DIM_Y, DIM_X, Cmt::cmtdata, e, currenteditor, ft);
        }

        char **screen_cache;
        int *width_pool;
        if (Config.cache_video)
        {
            alloc_ar(screen_cache, DIM_Y, DIM_X << 1);
            width_pool = new int[DIM_Y];
        }

        for (int i = STARTY; i < SMY - DELTAY; i++)
        {
            int selx1, selx2, L, j = i - STARTY;
            int Length;
            int loaf = e.getclippedlinevirt(buf, j, Length);
            int offset = e.getcornerx();
            char *b = makestrbuf(buf, atNormalText, L);
            int I = e.getcornery() + j;
            if (I < e.getlinecount())
            {
                if (coloring)
                    highlight(e.const_at(I), Length, b, L, loaf, offset, ft);
            }

            if (Config.multilinecomments && show_comments && coloring)
            {
                int kn;
                bool *cmt_row = comment_area[j];
                for (int k = 0; k < DIM_X; k = kn)
                {
                    bool *begin = (bool*)memchr(cmt_row+k, (int)true, DIM_X-k);
                    if (!begin)
                        break;
                    int ibegin = begin-cmt_row;
                    bool *end = begin+1;
                    if (ibegin < DIM_X-1)
                        end = (bool*)memchr(begin+1, (int)false, DIM_X-ibegin-1);
                    int iend = end? end-cmt_row-1 : DIM_X-1;
                    changebufattr(b, L, ibegin, iend, atComment);
                    kn = iend+1;
                }
            }

            if (!e.getselectarea(j, selx1, selx2))
                changebufattr(b, L, selx1, selx2, atSelect);

            #define WRITEBUF(X, Y, BUF, LEN) {\
                if (Config.cache_video) \
                { \
                    memcpy(screen_cache[Y - STARTY], BUF, LEN); \
                    width_pool[Y - STARTY] = LEN; \
                } \
                else writebuf(X, Y, BUF, LEN);}

            if (!ruleractive)
                WRITEBUF(STARTX, i, b, L)
            else
            {
                if (i != rY)
                    WRITEBUF(STARTX, i, b, L)
                else
                {
                    if (rX > 1)
                        WRITEBUF(STARTX, i, b, (rX - 1) << 1)
                    else width_pool[i - STARTY] = 0;
                }
            }

            #undef WRITEBUF

        }

        if (Config.cache_video)
        {
            if (Config.vsync)
                /*_vga_vsync()*/;
            for (int i = STARTY; i < SMY - DELTAY; i++)
            {
                int j = i - STARTY;
                if (width_pool[j])
                    writebuf(STARTX, i, screen_cache[j], width_pool[j]);
            }
            delete[] width_pool;
            free_ar(screen_cache);
        }

        if (Config.multilinecomments)
            free_ar(comment_area);

        char leftchar, rightchar;
#ifdef FRAME_GRAPHICAL
        leftchar = rightchar = '³';
#else
        leftchar = rightchar = '|';
#endif
        writevchar(0, 1, leftchar, SMY - 2, frameattr);
        writevchar(SMX - 1, 1, rightchar, SMY - 2, frameattr);
    }

    writestring(1, 0, statusline, atStatusLine);

#ifdef FRAME_GRAPHICAL
    writechar(0, 0, 'Ú', 1, frameattr);
    writechar(SMX - 1, 0, '¿', 1, frameattr);
    if (!preserve_bot)
    {
        writechar(0, SMY - 1, 'À', 1, frameattr);
        writechar(SMX - 1, SMY - 1, 'Ù', 1, frameattr);
        writechar(1, SMY - 1, 'Ä', SMX - 2, frameattr);
    }
#else
    writechar(0, 0, '-', 1, frameattr);
    writechar(SMX - 1, 0, '-', 1, frameattr);
    if (!preserve_bot)
    {
        writechar(0, SMY - 1, '-', 1, frameattr);
        writechar(SMX - 1, SMY - 1, '-', 1, frameattr);
        writechar(1, SMY - 1, '-', SMX - 2, frameattr);
    }
#endif

    if (ruleractive)
        printruler(rX, rY, SMX - 1 - rX);

}

static void timer_cb1(double elapsed)
{
    char buf[132];
    sprintf(buf, "(display() timing) %07.5f", elapsed);
    printstatus(buf);
}

void display(zed &e, bool outtext) //todo: search
{
    SwapScreen();
#if defined(FREE) || defined(RELEASE)
    _display(e, outtext, false);
#else
    if (Config.debug)
    {
        /*Liquid_timer::mode = Liquid_timer::MODE2;*/
        Liquid_timer liq(timer_cb1, !outtext);
        _display(e, outtext, true);
    }
    else _display(e, outtext, false);
#endif
    SwapScreen();
}

/*fix
WORD fileswitchers[] =
{
        kbCtrlGrayOne, kbCtrlGrayTwo, kbCtrlGrayThree, kbCtrlGrayFour, kbCtrlGrayFive,
        kbCtrlGraySix, kbCtrlGraySeven, kbCtrlGrayEight, kbCtrlGrayNine,
        kbCtrlGrayZero
};
*/

int myindex(WORD code)
{
    for (int i = 0; i < 10; i++)
    {
        //fix if (code == fileswitchers[i])
            return i;
    }
    return -1;
}

inline void mberror(char *s, bool __waitkey = false, double scale = 1.0)
{
    messagestart(1, 0, atMBError, s, NULL);
    if (__waitkey)
    {
        waitkey();
        flushkeyboard();
    }
    else mydelay((int) (1000 * scale));
    messageend();
    flushkeyboard();
}

void writefailed(FileException fe)
{
    messageend();
    printError(fe);
    mberror(" *** Write failed *** ", true);
}

void Save(zed& e)
{
    try
    {
        messagestart(1, 0, atMBSave, " Writing file... ", NULL);

#if defined(LOADTIME) || defined(SAVETIME)
        Timer.start();
#endif
        e.write_file(e.getfilename());
#if defined(LOADTIME) || defined(SAVETIME)
        Timer.stop();
#endif

        messageend();

#ifdef SAVETIME
        sprintf(messbuf, "save time = %.2f", Timer.__sec());
        messagebox(1, 0, atMBHelp, keyEscF10, messbuf, NULL);
#endif
    }
    catch(FileException fe)
    {
        writefailed(fe);
    }
    catch (...) {throw;}
    flushkeyboard();
}

void SaveAll(TNSCollection *e, int ecount, bool critical = true, int everyfile = 0)
{
    zed *t;
    for (int i = 0; i < ecount; i++)
    {
        static char *num[] =
        {
            "first", "second", "third", "fourth", "fifth",
            "sixth", "seventh", "eighth", "ninth", "tenth"
        };
        t = (zed *) e->at(i);
        if (!t->modified && !everyfile)
            continue;
        if (i < 10)
            sprintf(messbuf, " Writing %s file... ", num[i]);
        else sprintf(messbuf, " Writing %d-th file... ", i + 1);
        try
        {
            messagestart(1, 0, atMBSave, messbuf, NULL);
            t->write_file(t->getfilename());
            messageend();
        }
        catch (FileException fe)
        {
            messageend();
            if (!critical)
            {
                printError(fe);
                String file_name = getcomponent(fe.FileName, 4 + 8);
                file_name.downcase();
                sprintf(messbuf, " *** \"%s\": write failed *** ", file_name.c_str());
                mberror(messbuf, false, 2.0);
            }
            else
            {
                char buf[cMaxFileNameLength + 1];
                strcpy(buf, fe.FileName);
                flushkeyboard();
                printError(fe);
                while (!inputline(buf, sizeof(buf) - 1,
                    "Can't write this file (" INPUTSIG1 " to exit save cycle, " INPUTSIG2 " to skip)", INPUT_DEF, 1));
                if (!strcmp(buf, INPUTSIG1))
                    break;
                else if (!strcmp(buf, INPUTSIG2))
                    continue;
                else
                {
                    t->changefilename(buf);
                    i--;
                    continue;
                }
            }
        }
        catch (...) {throw;}
    }
    flushkeyboard();
}

void Main_help()
{
    messagebox(0, 0, atMBHelp, keyEscF1,
               MAINHELP
               NULL
               );
}

void Word_help(zed& ed, bool getting_bored = true)
{
    String Word = ed.Get_word_under_cursor();
    if (Word.empty())
        return;

    char *Start = "info libc alphabetical ";

    if (getting_bored)
    {
        TNSCollection menu(5, 5);
        menu.insert("libc");
        menu.insert("gmp");
        menu.insert("allegro");
        menu.insert("libgpp");
        menu.insert("libm");

        switch (VMenu(&menu, 7, 20, 20, 1, 1, true, true, false, 0, atSelect, atNormalText))
        {
            case -1:
                return;
            case 1:
                Start = "info gmp \"function index\" ";
                break;
            case 2:
                Start = "info allegro index ";
                break;
            case 3:
                Start = "info libgpp ";
                break;
            case 4:
                Start = "info libm index ";
                break;
        }
    }

    String Command_line = Start;

    Command_line += Word;
    Command_line += " --output=";
    String Tmpfile = tmpnam(0);
    Command_line += Tmpfile;
//*------------------------------------------------------------------------------------------------*
    processing();

    freopen("nul", "wb", stderr);
#ifdef USE_SYSTEM
    system(Command_line.c_str());
#else
    spawnlp(P_WAIT, Command_line.c_str(), "info.exe", 0);
#endif
    fflush(stderr);
    freopen("con", "wt", stderr);

    if (!fexist(Tmpfile.c_str()))
    {
        messageend();
        flushkeyboard();
        return;
    }

    zed *New_ed = new zed(SMX - DELTAX * 2, SMY - DELTAY * 2, SMY - 2);

    editors->atInsert(editorscount, New_ed);
    if (Cmt::cmtdata)
        Cmt::cmtdata->atInsert(editorscount, NULL);
    currenteditor = editorscount++;

    try
    {
        New_ed->read_file(Tmpfile.c_str());
    }
    catch (...)
    {
    }

    unlink(Tmpfile.c_str());

    messageend();
//*------------------------------------------------------------------------------------------------*
    Word.prepend(".\\nul.");
    New_ed->changefilename(Word.c_str());

    flushkeyboard();
}

void Help_index()
{
}

void low_level_startup()
{
    dummy();
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
#if defined(MSS)
    /*MSS_STARTUP;*/
#if defined(MSS_SILENT)
//  MSS_DISABLE_LOG_OUTPUT;
#endif
#endif
}

void startup1()
{
    screen_init();
}

void sighandler(int sig_number)
{
    crash();
    switch (sig_number)
    {
        case SIGABRT:           // abnormal termination, such as caused by the  abort function
            quit("Signal raised (SIGABRT): Call to abort()", 1, 0, 0);
        case SIGFPE:            // an erroneous floating-point operation occurs (such as division by zero, overflow and underflow)
            quit("Signal raised (SIGFPE): FPU error", 1, 0, 0);
        case SIGILL:            // illegal instruction encountered
            quit("Signal raised (SIGILL): Bad cpu instruction", 1, 0, 0);
        case SIGINT:            // an interactive attention (CTRL/C on keyboard) is signalled
            quit("Signal raised (SIGINT): Ctrl-C pressed", 1, 0, 1);
        case SIGSEGV:           // an illegal memory reference is detected
            quit("Signal raised (SIGSEGV): Bad memory reference detected", 1, 0, 0);
        case SIGTERM:           // a termination request is sent to the program
            quit("Signal raised (SIGTERM): Termination request", 1, 1, 1);
    }
}

void mymemoryexhausted()
{
    throw exceptionMemExhausted();
}

void startup2()
{
//fix    set_new_handler(mymemoryexhausted);

    signal(SIGSEGV, sighandler);
    signal(SIGILL, sighandler);
    signal(SIGFPE, sighandler);
    signal(SIGABRT, sighandler);
    signal(SIGTERM, sighandler);

#if defined(FREE) || defined(RELEASE)
    signal(SIGINT, SIG_IGN);
#else
    signal(SIGINT, SIG_DFL);
#endif

    char *tmp1 = getenv("TMPDIR");
    char *tmp2 = getenv("TEMP");
    char *tmp3 = getenv("TMP");
    if (tmp1)
        __tmpdir = tmp1;
    else if (tmp2)
        __tmpdir = tmp2;
    else if (tmp3)
        __tmpdir = tmp3;
    else __tmpdir = ".";
    __tmpdir += '\\';

    scrbuf = savewindow(SCREEN);

    CToken::CreateSyntaxTables();

    zed::callback_on_insert     = &Cmt::my_cb_insert;
    zed::callback_on_remove     = &Cmt::my_cb_remove;
    zed::callback_analize       = &Cmt::my_cb_analize;


//fix    ext_init();



/*
    oldcursor = getcursorpos();
    oldpage = GetVPage();
    system_oldblink = oldblink = getblinkintensity();
    if (system_oldblink)
        setblinkintensity(0);
    cursoron();
*/
}

int replacebox()
{
    printstatus("Search succeed. Replace? (Yes No All Cancel)");
    flushkeyboard();
    while (1)
    {
        int r = readkey();
        switch (r)
        {
            case kbEscape: case kbC: case kbShiftC:
                return 0;
            case kbY: case kbShiftY:
                return 1;
            case kbN: case kbShiftN:
                return 2;
            case kbA: case kbShiftA:
                return 3;
        }
    }
}

void _dosearch(zed *, bool, char const *);

//#define SEARCH_TIME
//#define SEARCH_SILENT

void dosearch(zed *e, bool replace)
{
    char searchopt[optlen] = {0};

    int ft = getfiletype(e->getfilename());
    if (ft == ftPascal || ft == ftCmm || ft == ftUndef)
        strcpy(searchopt, "i");

    if (!inputline(searchstr, sizeof(searchstr) - 1,
        "Text to find                                              ", INPUT_DEF, 2))
        return;
    if (!*searchstr)
        return;
    if (replace && !inputline(replacestr, sizeof(replacestr) - 1,
        "Replace with                                              ", INPUT_DEF, 2/*3*/))
        return;

    char const *Otext = replace? "Options (bigwaspf (back ignore glob word all silent prompt files))" :
                                 "Options (bigwcskf (back ignore glob word count silent keep files))";

    if (!inputline(searchopt, sizeof(searchopt) - 3, Otext, INPUT_DEF, 4))
        return;

#if defined(LOADTIME) || defined(SAVETIME)
    Timer.start();
#endif
    _dosearch(e, replace, searchopt);
#if defined(LOADTIME) || defined(SAVETIME)
    Timer.stop();
#endif
#ifdef SEARCH_TIME
    sprintf(messbuf, "search time = %.2f", Timer.__sec());
    messagebox(1, 0, atMBHelp, keyEsc, messbuf, NULL);
#endif
}

void _dosearch(zed *e, bool replace, char const *searchopt)
{
    lastsearchopt = searchopt;
    lastsearchopt.downcase();

    int silent  = lastsearchopt.contains('s');
    int all     = lastsearchopt.contains('a');
    int prompt  = lastsearchopt.contains('p');
    int count   = lastsearchopt.contains('c');
    int keep    = lastsearchopt.contains('k');
    //int files   = lastsearchopt.contains('f');

    int oldsilent = silent;
    if (replace)
    {
        count = 0;
        if (prompt)
            silent = 0;
    }
    else all = prompt = 0;

    if (!count)
        keep = 0;
    int const _m = 256;
    String fn(keep? (_m + 1) : 0);

    File f;

    if (keep && inputline(const_cast<char*>(fn.c_str()), _m, "File name to keep strings               ", INPUT_DEF, 5))
    {
        try {f.__open(fn.c_str(), "a+");}
        catch (FileException) {keep = 0;}
        catch (...) {throw;}
        if (f.invalid())
            keep = 0;
    }
    else keep = 0;

    int L1 = strlen(searchstr);
    int L2 = strlen(replacestr);

    int prevy = -1;

    if (count || replace)
    {
        int n = 1;
        processing();
        int r = e->search(searchstr, lastsearchopt, e->getvx());
        if (!silent)
            messageend();
        if (r == 0)
        {
            if (keep && filelength(fileno( f() )) == 0)
                f.__unlink();
#ifndef SEARCH_SILENT
            mberror(" Search string not found ");
#endif
            return;
        }
        if (!silent)
            display(*e, true);
        if (keep)
        {
            String t(e->currentline());
            t += '\n';
            try
            {
                f.fputs(t.c_str());
                prevy = e->getvy();
            }
            catch (FileException) {}
            catch (...) {throw;}
        }
        if (replace)
        {
            n = 0;
            if (prompt)
            {
                int r = replacebox(); //0=cancel 1=yes 2=no 3=all
                if (r == 0)
                    goto Quit;
                if (r == 2)
                {
                    if (all)
                        goto Cont;
                    else goto Quit;
                }
                if (r == 3)
                {
                    prompt = 0;
                    if (silent != oldsilent)
                        processing();
                    silent = oldsilent;
                }
            }
            e->doreplace(L1, L2, replacestr);
            n = 1;
            if (!all)
                goto Quit;
        }
Cont:
        while (!keypressed())
        {
            if (!silent)
                processing();
            if (replace)
                r = e->repeatsearch(searchstr, lastsearchopt, L1, L2);
            else r = e->repeatsearch(searchstr, lastsearchopt);
            if (!silent)
                messageend();
            if (r == 0)
                break;
            if (!silent)
                display(*e, true);
            if (keep)
            {
                int vy = e->getvy();
                if (vy != prevy)
                {
                    String t(e->currentline());
                    t += '\n';
                    try {f.fputs(t.c_str()); prevy = vy;}
                    catch (FileException) {}
                    catch (...) {throw;}
                }
            }
            if (replace)
            {
                if (prompt)
                {
                    int r = replacebox(); //0=cancel 1=yes 2=no 3=all
                    if (r == 0)
                        break;
                    if (r == 2)
                    {
                        if (all)
                            continue;
                        else break;
                    }
                    if (r == 3)
                    {
                        prompt = 0;
                        if (silent != oldsilent)
                            processing();
                        silent = oldsilent;
                    }
                }
                e->doreplace(L1, L2, replacestr);
            }
            n++;
        }
        flushkeyboard();
#ifndef SEARCH_SILENT
        sprintf(messbuf, replace? " *** %d replaces made. %s *** " : " *** %d entries found. %s *** ", n, "Hit Esc");
        messagebox(1, 0, atMBNormal, keyEsc, messbuf, NULL);
#endif
    }
    else
    {
        processing();
        if (e->search(searchstr, lastsearchopt, e->getvx()) == 0)
#ifndef SEARCH_SILENT
            mberror(" Search String not found ")
#endif
            ;
    }
Quit:
    messageend();
    flushkeyboard();
}

void write_cb()
{
    if (!zed::cb)
        return;
    static char filename[cMaxFileNameLength + 1] = {0};
    File f;
    if (inputline(filename, sizeof(filename) - 1, "File name to save clipboard             ", INPUT_DEF, 6))
    {
        processing();
        try {(zed::cb)->write_file(filename);}
        catch (FileException fe)
        {
            writefailed(fe);
        }
        catch (...) {throw;}
        messageend();
    }
}

void Write_block(zed& ed)
{
    if (!ed.blocklinecount())
        return;
    static char filename[cMaxFileNameLength + 1] = {0};
    File f;
    if (inputline(filename, sizeof(filename) - 1, "File name to save block                 ", INPUT_DEF, 11))
    {
        processing();
        try {ed.write_block(filename);}
        catch (FileException fe)
        {
            writefailed(fe);
        }
        catch (...) {throw;}
        messageend();
    }
}

int filelist_vmenu_cb(TNSCollection* list, String const &S, int& next, int selected)
{
    int old_next = next, cur_next = next;
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
        char *t = (char*)list->at(i);
        int ei = *(int*)(t + strlen(t) + 1);

        String filename = getcomponent( ((zed*)(editors->at(ei)))->getfilename(), 4 + 8);
        filename.downcase();

        if (memcmp(s.c_str(), filename.c_str(), MIN(l, filename.getwidth())) == 0)
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

WORD filelist_vmenu_key_cb(WORD code)
{
    switch (code)
    {
        case kbF12:
            return kbEscape;
    }
    return code;
}

void File_List()
{
    TStringCollection menu(256, 256);
    menu.duplicates = True;
    int i;
    int menu_retcode;

    int const Width = SMX - 30;

    for (i = 0; i < editorscount; ++i)
    {
        zed *e = (zed*)(editors->at(i));
        String Filename = e->getfilename();
        if (e->modified)
        {
            Fold_Filename(Filename, Width - 3);
            Filename.prepend("\x10\x10\x10");
        }
        else
        {
            Fold_Filename(Filename, Width);
        }
        _string t = Filename.storecontents();
        t += "1234";
        int l = t.getwidth();
        *(int*)const_cast<char*>(t.c_str() + l-3) = i;
        t[l-4] = '\0';
        menu.insert(t.c_str());
    }

    vmenu_cb_t old = vmenu_cb;
    vmenu_cb = filelist_vmenu_cb;
    vmenu_key_cb = filelist_vmenu_key_cb;
    menu_retcode = VMenu(&menu, SMY - 3, Width, Width, 1, 1, true, false, true, 0, atSelect, atNormalText);
    vmenu_key_cb = NULL;
    vmenu_cb = old;

    if (menu_retcode != -1)
    {
        char *t = (char*)menu.at(menu_retcode);
        currenteditor = *(int*)(t + strlen(t) + 1);
    }

    menu.freeAll();
}

void User_Screen()
{
    WORD oldcursor = getcursorpos();
    mygotoxy(0, (BYTE)SMY);
    if (oldblink)
        setblinkintensity(oldblink);
    restorewindow(scrbuf);
    while (1)
    {
        if (readkey() == kbF8)
        {
#ifdef HAS_CALCULATOR
            Calculator();
#endif
            mygotoxy(0, (BYTE)SMY);
        }
        else break;
    }
    flushkeyboard();
    if (oldblink)
        setblinkintensity(0);
    setcursorpos(oldcursor);
}

bool Any_modified()
{
    int f = 0;
    for (int i = 0; i < editorscount; i++)
        f += ((zed*)editors->at(i))->modified;
    return f != 0;
}

void Dos_Shell(char const *Shell)
{
    if (Any_modified())
    {
        int r = messagebox(1, 0, atMBError, keyYNEsc, "  Some files are modified. Save them ALL? (Y or N)  ", NULL);
        if (r == kbEscape)
            return;
        else if (r == kbY || r == kbShiftY)
            SaveAll(editors, editorscount);
    }
    restorewindow(scrbuf);
    if (oldblink)
        setblinkintensity(oldblink);
    blockcursor(0);
    setcursorpos(oldcursor);
    int OldSMX = SMX;
    int OldSMY = SMY;
    BYTE OldMode = GetVMode();
    BYTE OldPage = GetVPage();
    char WorkingDir[256];

    mygetwd(WorkingDir, sizeof WorkingDir);

/*------------------------------*/
#ifdef USE_SYSTEM
	system(Shell);
#else
	spawnlp(P_WAIT, Shell, Shell, 0);
#endif
/*------------------------------*/
	int r = mychdir(WorkingDir);
	int NewSMX = GetVColumns();
    int NewSMY = GetVRows();
    BYTE NewMode = GetVMode();
    BYTE NewPage = GetVPage();
    if (NewSMX == OldSMX && NewSMY == OldSMY && NewMode == OldMode)
    {
        delete[] scrbuf;
/*fix
        scrbuf = savewindow(SCREEN);
        oldcursor = getcursorpos();
        oldblink = getblinkintensity();
        if (NewPage != OldPage)
            SetVPage(OldPage);
*/
    }
    else
    {
/*fix
        SetVMode(OldMode);
        oldblink = getblinkintensity();
        SetVPage(OldPage);
*/
    }
/*fix
    if (oldblink)
        setblinkintensity(0);
    flushkeyboard();
*/
    if (r)
    {
        messagebox(1, 0, atMBError, keyEsc, " Warning: error changing directory. Hit Esc ", NULL);
    }
}

void Refresh_VM()
{
    SetVMode(GetVMode());
    SetVPage(oldpage);
    setblinkintensity(0);
}

void Find_next_token(int T, zed& e)
{
    if (getfiletype(e.getfilename()) != ftC)
        return;

    int LC = e.getlinecount();
    int vy = e.getvy() + 1;
    int vx = 0;
    for (int i = vy; i < LC; ++i)
    {
        CToken tok(e.myat(i));
        int x1, x2, x3;
        int t;
        while (tok.GetNextPair(x1, x2, x3, t) == 0)
        {
            if ((int)t == T)
            {
                int fx1 = i == vy? x1 + vx : x1;
                int fx2 = i == vy? x2 + vx : x2;
                e.moveexact(fx1, i);
                e.center();
                e.setanchor(fx1, i, fx2 + 1, i);
                return;
            }
        }
    }
    mberror(" No tokens left ");
}

void enh_goto(zed& ed)
{
    static char buf[20] = {0};
    static int column = -1;
    static int row = -1;
    static double percent = -1;

    if (percent >= 0)
        sprintf(buf, "%.3g%%", percent);
    else
    {
        if (row > 0 && column > 0)
            sprintf(buf, "%d,%d", row, column);
        else if (column > 0)
            sprintf(buf, ",%d", column);
        else if (row > 0)
            sprintf(buf, "%d", row);
        else *buf = 0;
    }

    sprintf(messbuf, "Enhanced goto (lines=%d,strlen(vy)=%d)",
        ed.getlinecount(),
        (int)strlen(ed.currentline())
        );

    int shift = zed::shifted;
    if (!inputline(buf, sizeof(buf), messbuf, INPUT_DEF, 12))
        return;
    zed::shifted = shift;

    String t = buf;
    t.removeall(' ');
    if (t.empty())
        return;

    if (t.search('%') != -1)
    {
        t.removeall('%');
        if (t.empty())
            return;
        sscanf(t.c_str(), "%lf", &percent);
        if (percent < 0)
            percent = 0;
        if (percent > 100)
            percent = 100;
        row = column = -1;
    }
    else
    {
        if (strchr(",.:;/\\#-", t[0]))
        {
            t.remove(0, 1);
            if (t.empty())
                return;
            sscanf(t.c_str(), "%d", &column);
            if (column < 1)
                column = 1;
            row = -1, percent = -1;
        }
        else
        {
            int i = 0, j;
            int L = t.getwidth();
            for (; i < L && (t[i] == '-' || ISDIGIT(t[i])); ++i);
            for (j = i; j < L && !(t[j] == '-' || ISDIGIT(t[j])); ++j);
            if (i >= L)
            {
                sscanf(t.c_str(), "%d", &row);
                if (row < 1)
                    row = 1;
                column = -1, percent = -1;
            }
            else
            {
                String first = t.fromto(0, i - 1);
                String second = t.fromto(j, L - 1);
                sscanf(first.c_str(), "%d", &row);
                sscanf(second.c_str(), "%d", &column);
                if (row < 1)
                    row = 1;
                if (column < 1)
                    column = 1;
                percent = -1;
            }
        }
    }

    ed.sel1();
    int Old_vy = ed.getvy();
    int Old_vx = ed.getvx();
    if (row > 0 && column > 0)
        ed.moveexact(column - 1, row - 1);
    else if (column > 0)
        ed.moveexact(column - 1, ed.getvy());
    else if (row > 0)
        ed.moveexact(ed.getvx(), row - 1);
    else if (percent >= 0)
        ed.moveexact(ed.getvx(), (int) (ed.getlinecount() * (percent / 100)));
    if (ed.getvy() != Old_vy || ed.getvx() == Old_vx)
        ed.center();
    ed.sel2();
}

int pre_process_key(zed& ed, unsigned code, int shifted)
{
    return 0;
}

int post_process_key(zed& ed, unsigned code, int shifted)
{
    if (ischaracter(code))
    {
        char t = tochar(code);
        zed::shifted = 0;
        if (Config.autobrackets && strchr("([{<", (unsigned char)t))
        {
            if (t == '(')
                ed.insertchar(')');
            else if (t == '[')
                ed.insertchar(']');
            else if (t == '{')
                ed.insertchar('}');
            else if (t == '<')
                ed.insertchar('>');
            ed.move(-1, 0);
        }
    }
    else switch (code)
    {
/*fix
        case kbGrayEnter:
            if (Config.grayenter_macro)
            {
                if (getfiletype(ed.getfilename()) == ftC)
                {
                    zed::shifted = 0;
                    ed.insert_state = 1;
                    ed.insertchar('{');
                    ed.doenter();
                    ed.move(0, +1); //todo: last line
                    ed.insertemptyline();
                    ed.insertchar('}');
                    ed.move(-1, -1);
                    ed.dotab(+1);
                }
                else ed.doenter();
            }
            else ed.doenter();
            break;
*/
    }
    return 0;
}

int process_key(zed& ed, unsigned code, int shifted)
{
    int RESULT = 0;
    pre_process_key(ed, code, shifted);
    zed::shifted = shifted;
    if (ischaracter(code))
    {
        zed::shifted = 0;
        ed.insertchar(tochar(code));
    }
    else
    {
        switch (code)
        {
            case kbAltHome:
                ed.movepagestart();
                break;
            case kbAltEnd:
                ed.movepageend();
                break;
            case kbUp:
                ed.move(0, -1);
                break;
            case kbDown:
                ed.move(0, +1);
                break;
            case kbLeft:
                ed.move(-1, 0);
                break;
            case kbRight:
                ed.move(+1, 0);
                break;
                        case kbCtrlUp:
                if (ed.cornermove(0, -1))
                    ed.move(0, -1);
                break;
            case kbCtrlDown:
                if (ed.cornermove(0, +1))
                    ed.move(0, +1);
                break;
            case kbAltUp:
                zed::shifted = 0;
                if (ed.cornermove(0, -1) == 0)
                {
                    if (ed.getvy() < ed.getcornery() + ed.maxy - 1)
                        ed.move(0, +1);
                }
                break;
            case kbAltDown:
                zed::shifted = 0;
                if (ed.cornermove(0, +1) == 0)
                {
                    if (ed.getvy() > ed.getcornery())
                        ed.move(0, -1);
                }
                break;
            case kbShiftF4:
                zed::shifted = 0;
                ed.cornermove(-ed.tab_size, 0);
                break;
            case kbShiftF5:
                zed::shifted = 0;
                ed.cornermove(+ed.tab_size, 0);
                break;
            case kbPageUp:
                ed.pagemove(-(SMY - 2)); //todo
                break;
            case kbPageDown:
                ed.pagemove(+(SMY - 2));
                break;
            case kbAltPageUp:
                ed.pagemove(-100);
                break;
            case kbAltPageDown:
                ed.pagemove(+100);
                break;
            case kbCtrlPageUp:
                ed.movetop();
                break;
            case kbCtrlPageDown:
                ed.movebottom();
                break;
            case kbCtrlHome:
                ed.movetop();
                ed.movestart();
                break;
            case kbCtrlEnd:
                ed.movebottom();
                ed.movestart();
                break;
            case kbHome:
                if (Config.brief_home)
                    ed.movestart_brief();
                else ed.movestart();
                break;
            case kbEnd:
                ed.moveend();
                break;
            case kbCtrlY:
                ed.deletecurrentline();
                break;
            case kbF5:
                ed.dupcurrentline();
                break;
            case kbCtrlN: case kbCtrlF5:
/*fix
case kbCtrlEnter:
*/
                ed.insertemptyline();
                break;
            case kbCtrlZ:
                ed.deletetoeol();
                break;
            case kbCtrlLeft:
                ed.goleftword();
                break;
            case kbCtrlRight:
                ed.gorightword();
                break;
            case kbEnter:
                ed.doenter();
                break;
            case kbTab:
                ed.dotab(+1);
                break;
/*fix
            case kbShiftTab:
                zed::shifted = 0;
                ed.dotab(-1);
                break;
*/
            case kbDelete:
                if (zed::shifted) // Shift-Del
                {
                    zed::shifted = 0;
                    processing();
                    if (ed.copyto_cb())
                    {
                        memoryerror();
                        break;
                    }
                    ed.removeblock();
                    messageend();
                    flushkeyboard();
                }
                else ed.dodelete();
                break;
            case kbInsert:
                if (zed::shifted) // Shift-Ins
                {
                    zed::shifted = 0;
                    processing();
                    ed.copyfrom_cb();
                    messageend();
                    flushkeyboard();
                }
                else ed.insert_state ^= 1;
                break;
            case kbCtrlDelete:
                processing();
                if (zed::shifted)
                    ed.clear_cb();
                else ed.removeblock();
                messageend();
                flushkeyboard();
                break;
            case kbCtrlInsert:
                processing();
                if (ed.copyto_cb())
                {
                    memoryerror();
                    break;
                }
                messageend();
                flushkeyboard();
                break;
/*fix
            case kbCtrlI:
                processing();
                ed.indent_block(ed.tab_size);
                messageend();
                break;
*/
            case kbCtrlU:
                processing();
                ed.indent_block(-ed.tab_size);
                messageend();
                break;
/*fix
            case kbAltI:
                processing();
                ed.indent_block(1);
                messageend();
                break;
            case kbAltU:
                processing();
                ed.indent_block(-1);
                messageend();
                break;
*/
            case kbBackSpace:
                ed.dobackspace();
                break;
            case kbCtrlOpenBracket:
                ed.xcaseword(0);
                break;
            case kbCtrlCloseBracket:
                ed.xcaseword(1);
                break;
            case kbCtrlBackSlash:
                ed.capitalizeword();
                break;
/*fix
            case kbAltSlash:
                ed.synccorner();
                break;
            case kbAltPoint:
                ed.center();
                break;
            case kbAltA:
                {
                    sprintf(messbuf, "%d", ed.tab_size);
                    if (!inputline(messbuf, 5, "New tab size (2..32)", INPUT_DEF, 8))
                        break;
                    int t;
                    sscanf(messbuf, "%d", &t);
                    if (t >= 2 && t <= 32)
                        ed.tab_size = t;
                }
                break;
            case kbAltL:
                ed.dropanchor();
                break;
            case kbCtrlS:
                ed.startblock();
                break;
*/
            case kbCtrlF:
                ed.endblock();
                break;
            case kbCtrlG:
                {
                    sprintf(messbuf, "Column number (1..%d)",
                        (int)strlen(ed.currentline()));
                    char local[10] = {0};
                    if (gocol > 0)
                        sprintf(local, "%d", gocol);
                    int shift = zed::shifted;
                    if (!inputline(local, 9, messbuf, INPUT_DEF, 9))
                        break;
                    zed::shifted = shift;
                    sscanf(local, "%d", &gocol);
                    if (gocol < 1)
                        gocol = 1;
                    ed.sel1();
                    ed.moveexact(gocol - 1, ed.getvy());
                    ed.sel2();
                }
                break;
/*fix
            case kbAltG:
                {
                    sprintf(messbuf, "Line number (1..%d)",
                        ed.getlinecount());
                    char local[10] = {0};
                    if (goline > 0)
                        sprintf(local, "%d", goline);
                    int shift = zed::shifted;
                    if (!inputline(local, 9, messbuf, INPUT_DEF, 10))
                        break;
                    zed::shifted = shift;
                    sscanf(local, "%d", &goline);
                    if (goline < 1)
                        goline = 1;
                    ed.sel1();
                    ed.gotoline(goline - 1);
                    ed.sel2();
                }
                break;
*/
            case kbAltF8:
                enh_goto(ed);
                break;
            case kbShiftF7: case kbCtrlL: case kbAltF7:
                {
                    if (!*searchstr)
                        break;
                    zed::shifted = 0;
                    String u = lastsearchopt;
                    if (code == kbAltF7)
                    {
                        if (u.contains('b'))
                            u.removeall('b');
                        else u += 'b';
                    }
                    processing();
                    if (ed.repeatsearch(searchstr, u) == 0)
                        mberror(" Search string not found ");
                    messageend();
                    flushkeyboard();
                }
                break;
            case kbF7: //todo: search all loaded files, search in block
                dosearch(&ed, false);
                break;
            case kbCtrlF7:
                dosearch(&ed, true);
                break;
            case kbF4:
                ruleractive ^= 1;
                break;
            case kbCtrlF4:
                if (rulerX == -1)
                    rulerX = ed.pagex() + STARTX;
                else rulerX = -1;
                break;
            case kbCtrlGraySlash:
                Find_next_token((int)ctString, ed);
                break;
/*fix
            case kbCtrlGrayAsterisk:
                Find_next_token((int)ctFloat, ed);
                break;
            case kbCtrlGrayMinus:
                Find_next_token((int)ctDecimal, ed);
                break;
            case kbCtrlGrayPlus:
                Find_next_token((int)ctHex, ed);
                break;
            case kbCtrlGrayEnter:
                Find_next_token(zed::shifted? (int)ctComment2 : (int)ctComment1, ed);
                break;
            case kbAltT:
*/
            case kbAltInsert:
                ed.select_word();
                break;
            case kbCtrlT:
                ed.delete_right_word();
                break;
            case kbCtrlBackSpace:
                ed.delete_left_word();
                break;
            case kbCtrlTab:
                zed::shifted = 0;
                ed.dosmarttab(shifted? -1: 1);
                break;
            case kbF9:
                show_comments = !show_comments;
                break;
            case kbAltF9:
                coloring = !coloring;
                break;
            case kbCtrlF9:
                status = !status;
                break;
            case kbCtrlQ:
                break;
            case kbF8:
#ifdef HAS_CALCULATOR
                Calculator();
#endif
                break;
            case kbF2:
                Save(ed);
                break;
            case kbShiftF2:
                {
                    char fbuf[256];
                    strcpy(fbuf, ed.getfilename());
                    int oldft = getfiletype(fbuf);

                    if (inputline(fbuf, sizeof(fbuf) - 1, "Enter new file name             ", INPUT_DEF, 7))
                    {
                        ed.changefilename(fbuf);
                        Save(ed);
                        if (Config.multilinecomments)
                        {
                            int ft = getfiletype(fbuf);
                            if (ft != oldft)
                            {
                                if (Config.analize_rename)
                                {
                                    messagestart(1, 0, atMBNormal, " *** Analizing file, stay tuned... *** ", NULL);
                                    Cmt::init_cmt_data(Cmt::cmtdata, &ed, false, false);
                                    messageend();
                                }
                                else
                                {
                                    Cmt::init_cmt_data(Cmt::cmtdata, &ed, false, true);
                                    messagebox(1, 0, atMBError, keyEsc, " Warning: multiline cmt coloring disabled. Hit Esc ", NULL);
                                }
                            }
                        }
                    }
                }
                break;
            case kbCtrlW:
                write_cb();
                break;
/*fix
            case kbAltW:
                Write_block(ed);
                break;
*/
            default:
                RESULT = 1;
                break;
        }
    }
    post_process_key(ed, code, shifted);
    return RESULT;
}

void runEditor(int argc, char **argv)
{
    int const
        ED_INIT = 256,
        ED_DELTA = 256;
    editors = new TNSCollection(ED_INIT, ED_DELTA);
    if (Config.multilinecomments)
        Cmt::cmtdata = new TNSCollection(ED_INIT, ED_DELTA);

#if defined(LOADTIME) || defined(SAVETIME)
    Timer.start();
#endif

    for (int curopt = 1; curopt < argc; curopt++)
    {
        zed *t = new zed(SMX - DELTAX * 2, SMY - DELTAY * 2, SMY - 2);

        if (NewConfig)
        {
            t->autoindent = Config.autoindent;
            t->cursorbeyondend = Config.cursorbeyondend;
            t->autotabsizeonload = Config.autotabsizeonload;
            t->tab_size = Config.tab_size;
        }

        editors->atInsert(currenteditor++, t);

        char *filetoedit = argv[curopt];
        struct stat z;

        if (stat(filetoedit, &z) != -1)
        {
            int Warning = 0;

            switch (z.st_mode & S_IFMT)
            {
                case S_IFDIR: /* directory */
                    if (curopt > 1)
                        fputc('\n', stderr);
                    fprintf(stderr, "Warning: trying to open directory \"%s\"\n", filetoedit);
                    Warning = 1;
                    break;
                case S_IFCHR: /* character special file */
                    if (curopt > 1)
                        fputc('\n', stderr);
                    fprintf(stderr, "Warning: trying to open non-regular file \"%s\"\n", filetoedit);
                    Warning = 1;
                    break;
                case S_IFREG: /* regular */
                    break;
            }
            if (Warning)
                mydelay(1000);
        }

        if (Config.debug)
            putchar('.');
        try
        {
            t->read_file(filetoedit);
        }
        catch (exceptionMemExhausted)
        {
            fputs("\n" prgname ": dynamic memory exhausted\n", stderr);
            quit(NULL, 1, 0, 1);
        }
        catch (FileException fe)
        {
            sprintf(messbuf, "\n[%s] -- load error (%s)\n",
                filetoedit,
                errorstring(fe));
            quit(messbuf, 1, 0, 1);
        }
        catch (...) {throw;}

        if (t->autotabsizeonload && !t->tabsizedetected)
        {
            switch (getfiletype(t->getfilename()))
            {
                case ftASM:
                    t->tab_size = Config.tab_size_asm;
                    break;
            }
        }

        if (Config.multilinecomments)
            Cmt::init_cmt_data(Cmt::cmtdata, t, true);

    }

#if defined(LOADTIME) || defined(SAVETIME)
    Timer.stop();
#endif

#ifdef LOADTIME
    sprintf(messbuf, "load time = %.2f", Timer.__sec());
    messagebox(1, 0, atMBHelp, keyEscF10, messbuf, NULL);
#endif

    editorscount = currenteditor;
    currenteditor = 0;

    unsigned code;

    flushkeyboard();

__Edit_Loop:

    do
    {
        messageend();
        display_busy = true;
        display(*editor(), true);
        while (!keypressed())
        {
            display(*editor(), false);
            releasetimeslice();
        }

        display_busy = false;
        int shifted;
        code = readkey(&shifted);
        zed *ed = editor();

        switch (code)
        {
            case kbF1:
                Main_help();
                break;
            case kbCtrlF1:
                Word_help(*ed, (bool)shifted);
                break;
            case kbShiftF1:
                Help_index();
                break;
            case kbAltF5:
                User_Screen();
                break;
/*fix
            case kbAltJ:
                Dos_Shell(getenv("COMSPEC"));
                break;
*/
            case kbCtrlJ:
                Dos_Shell("bash");
                break;
            case kbShiftF9:
                {
                    char *ac;
                    if (SMX >= 132)
                        ac = (char*)aCompiler132;
                    else if (SMX >= 100)
                        ac = (char*)aCompiler100;
                    else ac = (char*)aCompiler80;
                    messagebox(1, 0, atMBHelp, keyEsc,
#ifdef FREE
                        "",
                        "Some features are clipped-out, this is free restricted version",
#endif
                        "",
                        "zed editor " ZED_VERSION,
                        "",
                        "Copyright(C)2000-2017 Alexander Derbeev <markina-ulyanka@mail.ru>",
                        "",
                        ac,
                        "",
                        "*** TEST COPY PLEASE DO NOT DISTRIBUTE ***",
                        "",
                        NULL);
                }
                break;
            case kbAltF10:
                Refresh_VM();
                break;
            case kbF6: case kbAltRight:
                currenteditor = (currenteditor < editorscount - 1) ? (++currenteditor) : 0;
                break;
            case kbShiftF6: case kbAltLeft:
                currenteditor = (currenteditor > 0) ? (--currenteditor) : (editorscount - 1);
                break;
            case kbCtrlF2:
                SaveAll(editors, editorscount, false);
                break;
            case kbAltF2:
                SaveAll(editors, editorscount, false, 1);
                break;

/*fix
            case kbAltZero:
*/
            case kbF12:
                File_List();
                break;
            case kbF11:     // test key
                /*
                while (!keypressed())
                {
                    ed->movetop();
                    ed->movestart();
                    while (!keypressed())
                    {
                        int y = ed->getvy();
                        ed->pagemove(+(SMY - 2));
                        display(ed, true);
                        if (y == ed->getvy())
                            break;
                    }
                    currenteditor++;
                    if (currenteditor >= editorscount)
                        currenteditor = 0;
                }
                flushkeyboard();
                */
#ifdef STRESS
                stresstest(ed, 0);
#endif
                break;
            case kbAltF11:
                messagebox(1, 0, atMBError, keyEsc, " Comment data dumped to disk. Hit Esc ", NULL);
                break;
            default:
                if (process_key(*ed, code, shifted))
                {
                    int t = myindex(code);
                    if (t != -1 && t < editorscount)
                        currenteditor = t;
                }
        }
    }
    while (!(

/*fix

code == kbAltX
        ||
*/

(code == kbEscape && Config.esc_exits)
        || code == kbF10
        || code == kbShiftF10));

    if (code == kbShiftF10)
        SaveAll(editors, editorscount);
    else
    {
        if (Any_modified())
        {
            int r = messagebox(1, 0, atMBError, keyYNEsc, "  Save modified files? (Y or N)  ", NULL);
            if (r == kbEscape)
                goto __Edit_Loop;
            else if (r == kbY || r == kbShiftY)
                SaveAll(editors, editorscount);
        }
    }

// *** FINAL CLEAN-UP ***

    zed::shoulddelete = Config.exit_cleanup;

    if (Config.exit_cleanup)
    {
#if defined(MSS) && defined(MSS_LEVEL3)
        MSS_CHECK_ALL_BLOCKS;
        MSS_LOG_INFO; //To see number of blocks and memory used.
        MSS_LOG_INTERNAL_INFO;
#endif
        for (currenteditor = 0; currenteditor < editorscount; currenteditor++)
            delete editor();

        zed::clear_cb();
        Cmt::destroy_cmtdata();

        if (editors)
            delete editors, editors = NULL;

        shutdown();

#if defined(MSS) && defined(MSS_LEVEL3)
        MSS_LOG_INFO; //To see if there is still any allocated memory.
        MSS_LOG_BLOCK_LIST; //To get detailed info about it.
#endif

    }
}

void printException(char *t)
{
    mygotoxy(0, 0);
    fprintf(stderr, uprgname": Exception `%s' raised (press any key).\n", t);
    flushkeyboard();
    readkey();
}

void run(int argc, char **argv)
{
#ifdef LOG_ZED
    log.open("log_zed", ios::out | ios::trunc);
#endif

    Config.highlight = true;
    Config.autobrackets = false;
    Config.multilinecomments = true;
    Config.analize_rename = true;
    Config.cache_video = true;
    Config.vsync = true;
    Config.block_insert = false;
    Config.unix_end = false;
    Config.exit_cleanup = false;

    Config.debug = false;

    Config.autoindent = true;
    Config.cursorbeyondend = true;
    Config.tab_size = 4;
    Config.tab_size_asm = 8;
    Config.hardtabsize = 8;
    Config.autotabsizeonload = true;
    Config.persistent = false;
    Config.esc_exits = false;
    Config.calcoptype = 0;
    Config.calcmpdigits = 256;
    Config.calc_callback = false;
    Config.multichar_nasm = true;
    Config.brief_home = true;
    Config.grayenter_macro = true;

    strcpy(Config.status_time_format_long, "%a %b %e %r %Y");
    strcpy(Config.status_time_format_short, "%H:%M");
    strcpy(Config.status_spec, ""); /*todo*/

    Config.atNormalText = atNormalText;
    Config.atSelect = atSelect;

    Config.atKeyWord = atKeyWord;
    Config.atFunction = atFunction;
    Config.atSpecial = atSpecial;
    Config.atLabel = atLabel;

    Config.atSyntax = atSyntax;
    Config.atInvalid = atInvalid;

    Config.atOctal = atOctal;
    Config.atHex = atHex;
    Config.atDecimal = atDecimal;
    Config.atFloat = atFloat;
    Config.atBinary = atBinary;
    Config.atChar = atChar;
    Config.atString = atString;

    Config.atPreprocess = atPreprocess;
    Config.atPreprocessUnknown = atPreprocessUnknown;
    Config.atComment = atComment;

    Config.atTitle = atTitle;

    Config.atStatusLine = atStatusLine;
    Config.atMainFrame = atMainFrame;
    Config.atMBNormal = atMBNormal;
    Config.atMBSave = atMBSave;
    Config.atMBHelp = atMBHelp;
    Config.atMBError = atMBError;

    Config.atRulerN = atRulerN;
    Config.atRulerI = atRulerI;

    NewConfig = readconfig(*argv, Config);

	CToken::Little_endian = Config.multichar_nasm;
    zed::unix_end = Config.unix_end;

    if (NewConfig)
    {
        zed::persistent = Config.persistent;
        zed::hardtabsize = Config.hardtabsize;

        atNormalText = Config.atNormalText;
        atSelect = Config.atSelect;

        atKeyWord = Config.atKeyWord;
        atFunction = Config.atFunction;
        atSpecial = Config.atSpecial;
        atLabel = Config.atLabel;

        atSyntax = Config.atSyntax;
        atInvalid = Config.atInvalid;

        atOctal = Config.atOctal;
        atHex = Config.atHex;
        atDecimal = Config.atDecimal;
        atFloat = Config.atFloat;
        atBinary = Config.atBinary;
        atChar = Config.atChar;
        atString = Config.atString;

        atPreprocess = Config.atPreprocess;
        atPreprocessUnknown = Config.atPreprocessUnknown;
        atComment = Config.atComment;

        atTitle = Config.atTitle;

        atStatusLine = Config.atStatusLine;
        atMainFrame = Config.atMainFrame;
        atMBNormal = Config.atMBNormal;
        atMBSave = Config.atMBSave;
        atMBHelp = Config.atMBHelp;
        atMBError = Config.atMBError;

        atRulerN = Config.atRulerN;
        atRulerI = Config.atRulerI;
    }

    flushkeyboard();
    runEditor(argc, argv);
    flushkeyboard();
}

static void mytest()
{
    while (1)
    {
        unsigned k = readkey();
        printf("%x\n", k);
    }
    quit(NULL, 0, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    //mytest();

    low_level_startup();

    if (argc < 2)
    {
        puts(help);
        return 1;
    }

    startup1();

    if ((SMX < 80 || SMX > 132) || SMY < 25 || graphicmodeactive())
        exiterror("\n" prgname ": this video mode not supported\n\n", 1);

    FileException::mask &= ~(_feBuf);

    try
    {
        startup2();
#if defined(MSS) && defined(MSS_LEVEL2)
        MSS_ENTER_SCOPE;
#endif
        run(argc, argv);
#if defined(MSS) && defined(MSS_LEVEL2)
        MSS_LEAVE_SCOPE;
#endif
    }
    catch (FileException)
    {
        printException("unhandled file i/o error");
        crash();
        exiterror(NULL, 1, 1);
    }
    catch (TCEInvalidIndex t)
    {
        sprintf(messbuf, "bad index (%d) inside tcollection", t.index);
        printException(messbuf);
        crash();
        exiterror(NULL, 1, 1);
    }
    catch (zed::IllegalLineRef t)
    {
        sprintf(messbuf, "illegal editor line reference (line #%d)", t.line);
        printException(messbuf);
        crash();
        exiterror(NULL, 1, 1);
    }
    catch (zed::InternalError t)
    {
        sprintf(messbuf, "internal editor error (source line %d)", t.line);
        printException(messbuf);
        exiterror(NULL, 1, 1);
    }
    catch (exceptionMemExhausted)
    {
        crash();
        exiterror("\n" prgname ": dynamic memory exhausted" \
            "\nplease report to me if it happened while editing and you're sure it shouldnt\n",
            1);
    }
    catch (String::SEAccessToEmpty)
    {
        printException("access to empty string");
        crash();
        exiterror(NULL, 1, 1);
    }
    catch (String::SEInvalidIndex)
    {
        printException("string index out of range");
        crash();
        exiterror(NULL, 1, 1);
    }
    catch (String::SENegativeAlloc)
    {
        printException("negative size of allocation request in class string");
        crash();
        exiterror(NULL, 1, 1);
    }
    catch (String::SEBadParameter)
    {
        printException("bad parameter passed to class string member");
        crash();
        exiterror(NULL, 1, 1);
    }
    catch (exceptionDummy)
    {
        printException("dummy :-)");
        crash();
        exiterror(NULL, 1, 1);
    }
    catch (...)
    {
        printException("internal error -- unhandled exception");
        crash();
        exiterror(NULL, 1, 1);
    }

    quit(NULL, 0, 1);

    return 0;
}
