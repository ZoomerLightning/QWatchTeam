/*
** input.cpp
**
** Defines input function & calculator.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

#include "input.h"
#include "pmt.h"
#include "video.h"
#include "keyboard.h"
#include "tok.h"
#include "zedclass.h"
#include "zeddef.h"
#include "stlstuff.h"

jmp_buf __FPE_jb;

#define CALC_LINE_MAX           8192

extern BYTE
    atNormalText,
    atTitle,
    atSelect,
    atMBHelp;
extern WORD keyEscF1[];
extern zedConfig Config;

static bool CalculatorStarted = false;

int Search_id(TNSCollection &where, int _id)
{
    int Count = where.getCount();
    if (Count == 0)
        return -1;
    for (int i = 0; i < Count; ++i)
        if (((History*)where.at(i))->_id == _id)
            return i;
    return -1;
}

int inputline(char *s, int maxlen, char const *title,
    int (*Action)(zed *, int, int, int, int, ushort&),
    int YAdd, bool CenterX, int DeltaX,
    int _id)
{
    static TNSCollection His_pool(10, 10);

    int id_pos = Search_id(His_pool, _id);
    if (id_pos == -1)
    {
        History *tmp = new History;
        tmp->_id = _id;
        tmp->data = new TStringCollection(16, 16);
        int Count = His_pool.getCount();
        His_pool.atInsert(Count, ptr_add(PTR_History, tmp));
        id_pos = Count;
    }

    int tl = strlen(title);
    int x1, y1, x2, y2;

    if (CenterX)
        x1 = (SMX - (tl + 2)) / 2, x2 = x1 + (tl + 2) - 1;
    else x1 = DeltaX, x2 = (tl + 2) - 1 + DeltaX;

    y1 = SMY / 2 - 4 - YAdd / 2;
    y2 = y1 + 3;

    char *mbuf = new char[tl + 1];
    char *buf = savewindow(x1, y1, x2, y2);
    window(x1, y1, x2, y2, 0, atNormalText);
    writestring(x1 + 1, y1 + 1, title, atTitle);

    bool oldpers = zed::persistent;
    zed::persistent = false;

    zed *e = new zed(tl, 1, 1);
    e->initempty();

    e->zed_maxlinelength = maxlen;
    e->cursorbeyondend = false;
    e->__nospaces = false;
    e->putstr(s, 0);

    e->setanchor(0, 0, __npos, 0);

    ushort code = 0;
    bool virgin = true;

    if (Action)
        Action(e, x1, y1, x2, y2, code);

    do
    {
        blockcursor(e->insert_state ^ 1);
        mygotoxy((BYTE) (e->pagex() + x1 + 1), (BYTE) (e->pagey() + y1 + 2));
        int Length;
        e->getclippedlinevirt(mbuf, 0, Length);
        int t;
        char *b = makestrbuf(mbuf, atNormalText, t);

        int selx1, selx2;
        if (!e->getselectarea(0, selx1, selx2))
            changebufattr(b, t, selx1, selx2, atSelect);

        writebuf(x1 + 1, y1 + 2, b, t);

        char tmp[40];
        sprintf(tmp, "%05d (%05d)", e->getvx() + 1, (int)strlen(e->at(0)));
        writestring(x1 + 2, y2, tmp, atNormalText);

        code = readkey(&zed::shifted);
        if (ischaracter(code))
        {
            if (virgin)
                e->deletecurrentline();
            virgin = false;
            zed::shifted = 0;
            e->insertchar(tochar(code));
        }
        else
        {
            switch (code)
            {
                case kbDown:
                    {
                        int i = VMenu(((History*)His_pool.at(id_pos))->data,
                            7, x2 - x1 - 1, x2 - x1 - 1, x1 + 1, y1 + 3, false, false, true, 0, atSelect, atNormalText);
                        if (i != -1)
                        {
                            char *t = mystrdup((char*)((History*)His_pool.at(id_pos))->data->at(i));
                            e->atreplace(0, t);
                            e->setanchor(0, 0, __npos, 0);
                        }
                    }
                    break;
                case kbLeft:
                    e->move(-1, 0);
                    break;
                case kbRight:
                    e->move(+1, 0);
                    break;
                case kbHome:
                    e->movestart();
                    break;
                case kbEnd:
                    e->moveend();
                    break;
                case kbCtrlLeft:
                    e->goleftword();
                    break;
                case kbCtrlRight:
                    e->gorightword();
                    break;
                case kbDelete:
                    if (zed::shifted) // Shift-Del
                    {
                        zed::shifted = 0;
                        if (e->copyto_cb())
                            break;
                        e->removeblock();
                        flushkeyboard();
                    }
                    else e->dodelete();
                    break;
                case kbInsert:
                    if (zed::shifted) // Shift-Ins
                    {
                        zed::shifted = 0;
                        if (zed::cb && zed::cb->getlinecount() < 2)
                        {
                            if (virgin)
                                e->deletecurrentline();
                            e->copyfrom_cb();
                        }
                        flushkeyboard();
                    }
                    else e->insert_state ^= 1;
                    break;
                case kbCtrlDelete:
                    if (zed::shifted)
                        e->clear_cb();
                    else e->removeblock();
                    flushkeyboard();
                    break;
                case kbCtrlInsert:
                    if (e->copyto_cb())
                        break;
                    flushkeyboard();
                    break;
                case kbBackSpace:
                    e->dobackspace();
                    break;
                case kbCtrlOpenBracket:
                    e->xcaseword(0);
                    break;
                case kbCtrlCloseBracket:
                    e->xcaseword(1);
                    break;
                case kbCtrlBackSlash:
                    e->capitalizeword();
                    break;
                case kbCtrlY:
                    e->moveexact(0, 0);
                    e->deletetoeol();
                    break;
                case kbCtrlZ:
                    e->deletetoeol();
                    break;
                case kbCtrlT:
                    e->delete_right_word();
                    break;
                case kbCtrlBackSpace:
                    e->delete_left_word();
                    break;
/*fix
                case kbAltL:
                    e->dropanchor();
                    break;
                case kbAltT:
                case kbAltInsert:
                    e->select_word();
                    break;
                case kbCtrlS:
                    e->startblock();
                    break;
*/
                case kbCtrlF:
                    e->endblock();
                    break;
            }
            virgin = false;
        }

		if (code == kbEnter || code == kbEscape)
        {
            int dummy;
            char *S = (char*)e->myat(0);
            if (S && *S && ((History*)His_pool.at(id_pos))->data->search(S, dummy) == False)
                ((History*)His_pool.at(id_pos))->data->insert(mystrdup(S));
        }

        if (Action)
        {
            if (Action(e, x1, y1, x2, y2, code))
                virgin = true;
        }

    } while (!(code == kbEnter || code == kbEscape));

    if (code == kbEnter)
    {
        strcpy(s, e->myat(0));
    }

    delete e;
    restorewindow(buf);
    delete[] buf;
    delete[] mbuf;
    zed::persistent = oldpers;
    return (int) (code == kbEnter);
}

#ifdef HAS_CALCULATOR
String PrintResults0(Real const &z, int X, int Y, int loud = 1, unsigned n = 0)
{
    static char buf[100], buf2[100];

    static String num[9];

    #define WRITEBUF(YOFS) if (loud) writestring(X, Y + YOFS, buf, atNormalText)

    sprintf(buf, "Value             = %40.40s", to_dec(z, 10, 14, false).c_str());
    WRITEBUF(1);
    sprintf(buf, "Exponential     F2= %40.40s", (num[0] = to_dec(z, 10, 15, true)).c_str());
    WRITEBUF(2);

    double Z = z;
    unsigned d1 = ((unsigned*)&Z)[0];
    unsigned d2 = ((unsigned*)&Z)[1];
    sprintf(buf2, "0x%08x%08x", d2, d1);
    sprintf(buf, "Dump            F3= %40s", (num[1] = buf2).c_str());
    WRITEBUF(3);

    char const *const error = "Overflow";
    char numbuf[100];

    unsigned u;
    int i;
    int e1 = round_u32(z, u);
    int e2 = round_s32(z, i);

    sprintf(buf, "Signed int      F4= %40s", (num[2] = (e2? error : itoa(i, numbuf, 10))).c_str());
    WRITEBUF(4);
    sprintf(numbuf, "%u", u);
    sprintf(buf, "Unsigned int    F5= %40s", (num[3] = (e1? error : numbuf)).c_str());
    WRITEBUF(5);
    sprintf(buf, "Octal           F6= %40s", (num[4] = (e1? error : itoa(u, numbuf, 8))).c_str());
    WRITEBUF(6);
    sprintf(buf, "Binary          F7= %40s", (num[5] = (e1? error : itoa(u, numbuf, 2))).c_str());
    WRITEBUF(7);
    sprintf(buf, "Hex             F9= %40s", (num[6] = (e1? error : _STRUPR(itoa(u, numbuf, 16)))).c_str());
    WRITEBUF(8);
    sprintf(buf, "Roman          F11= %40s", (num[8] = (e1? error : upcase(to_rom(z)).c_str())).c_str());
    WRITEBUF(9);
    sprintf(buf, "Boolean           = %40s", ((int)z)? "Yes" : "No");
    WRITEBUF(10);
    #undef WRITEBUF

    num[7] = "0";

    return num[n];
}

String PrintResults1(MP_Real const &z, int X, int Y, int loud = 1, unsigned n = 0)
{
    static char buf[100], buf2[100];

    static String num[9];

    #define WRITEBUF(YOFS) if (loud) writestring(X, Y + YOFS, buf, atNormalText)

    sprintf(buf, "Value             = %40.40s",
        to_dec(z, 10, MIN(29, Config.calcmpdigits), false).c_str());
    WRITEBUF(1);
    sprintf(buf, "Exponential     F2= %40.40s",
        (num[0] = to_dec(z, 10, MIN(29, Config.calcmpdigits), true)).c_str());
    WRITEBUF(2);

    double Z = z;
    unsigned d1 = ((unsigned*)&Z)[0];
    unsigned d2 = ((unsigned*)&Z)[1];
    sprintf(buf2, "0x%08x%08x", d2, d1);
    sprintf(buf, "Dump            F3= %40s", (num[1] = buf2).c_str());
    WRITEBUF(3);

    char const *const error = "Overflow";
    char numbuf[100];

    unsigned u;
    int i;
    int e1 = round_u32(z, u);
    int e2 = round_s32(z, i);
    unsigned long long U;

	sprintf(buf, "Signed int      F4= %40s", (num[2] = (e2? error : itoa(i, numbuf, 10))).c_str());
	WRITEBUF(4);
	sprintf(numbuf, "%u", u);
	sprintf(buf, "Unsigned int    F5= %40s", (num[3] = (e1? error : numbuf)).c_str());
	WRITEBUF(5);

	sprintf(buf, "Octal           F6= %40s", (num[4] = (e1? error : itoa(u, numbuf, 8))).c_str());
	WRITEBUF(6);
	sprintf(buf, "Binary          F7= %40s", (num[5] = (e1? error : itoa(u, numbuf, 2))).c_str());
	WRITEBUF(7);

	sprintf(buf, "Hex(32)         F9= %40s", (num[6] = (e1? error : _STRUPR(itoa(u, numbuf, 16)))).c_str());
	WRITEBUF(8);
	sprintf(numbuf, "%llX", U);

	sprintf(buf, "Roman          F11= %40s", (num[8] = (e1? error : upcase(to_rom(z)).c_str())).c_str());
	WRITEBUF(10);

	char cbuf[7] = {'\0'};
	if (!e1)
	{
        cbuf[0] = '\x10';
        cbuf[5] = '\x11';
        cbuf[6] = '\0';
        if (Config.multichar_nasm)
        {
            cbuf[1] = (char) (u & 0xff);
            cbuf[2] = (char) ((u >> 8) & 0xff);
            cbuf[3] = (char) ((u >> 16) & 0xff);
            cbuf[4] = (char) ((u >> 24) & 0xff);
        }
        else
        {
            cbuf[4] = (char) (u & 0xff);
            cbuf[3] = (char) ((u >> 8) & 0xff);
            cbuf[2] = (char) ((u >> 16) & 0xff);
            cbuf[1] = (char) ((u >> 24) & 0xff);
        }
        for (int i = 1; i <= 4; ++i)
            if (cbuf[i] == '\0')
                cbuf[i] = ' ';
    }
    if (e1)
        sprintf(buf, "Wide char         = %40s", error);
    else
    {
        String t = "Wide char         = ";
        t += String(' ', 40 - 6);
        t += cbuf;
        strcpy(buf, t.c_str());
    }
    WRITEBUF(11);

    return num[n];
}

#define CALCHELP \
    "Binary op. ::= (**) (* /) (%) (+ -) (<< >>) (> >= < <=)", \
    "               (== !=) (& ^ |) (&& ||) (()) (=)", \
    "Unary op. ::= (- + \\ ~ !)", \
    "Constants ::= (pi e inf nan)", \
    "Numbers ::= (123 1.2e-3 01232 0b101 0xDEAD 0rDCLXVI '\\66')", \
    "Suffixes ::= (U L LL UL ULL) (F L)", \
    "Functions ::= 1)", \
    "Cos Sin Tan CosDg SinDg TanDg ACos ASin ATan CosH SinH TanH", \
    "ACosH ASinH ATanH Sec Csc SecH CscH ASec ACsc ASecH ACscH", \
    "CoTan CoTanDg ACoTan CoTanH ACoTanH Ln Lg Log2 Exp Pow2", \
    "Pow10 Sqrt Cubr Sqr Cbrt Sign Abs Neg Not Inv Mask Rad Grad", \
    "Round Ceil Floor Frac Int Next Prev Fac IsPrime Permut; 2)", \
    "ATan2 Log FMod Mod FDiv RoundTo R Pow Root LdExp SumiGP", \
    "Min Max RotL RotLB RotLW RotR RotRB RotRW Sar SarB SarW", \
    "Bit Test AndNot BT BC BR BS GCD LCM Comb Accom Comb_ Accom_;", \
    "SumAP GetAP SumGP GetGP",

class CalculatorStopped
{
public:
    CalculatorStopped() {}
    ~CalculatorStopped() {}
};

void CalcBreak(int)
{
    /*throw CalculatorStopped();*/
    longjmp(__FPE_jb, 1);
}

int CB_X, CB_Y;

void callback(UIntMax N, MP_Real::mp_cb_senders sender)
{
    char *sender_name = "";
    int delta = 0;

    switch (sender)
    {
        case MP_Real::cb_Cosh:
            sender_name = "cosh()", delta = -6;
            break;
        case MP_Real::cb_Fac:
            sender_name = "fac()", delta = -5;
            break;
        case MP_Real::cb_Exp:
            sender_name = "exp()", delta = -4;
            break;
        case MP_Real::cb_Log:
            sender_name = "log()", delta = -3;
            break;
        case MP_Real::cb_Sin:
            sender_name = "sin()", delta = -2;
            break;
        case MP_Real::cb_Cos:
            sender_name = "cos()", delta = -1;
            break;
        case MP_Real::cb_ASin:
            sender_name = "asin()", delta = 0;
            break;
    }
    char buf[60];
    sprintf(buf, "%s %020llu", sender_name, N);
    writestring(CB_X, CB_Y + delta, buf, atTitle);
}

static bool calc_change_skin = false;

int CalculatorAction(zed *e, int x1, int, int x2, int y2, ushort &code)
{
    static int prev_rv = 0;
    int rv = 0;

    static Real PrevResult0;
    static MP_Real PrevResult1;
    static char buf[CALC_LINE_MAX],
        buf2[CALC_LINE_MAX];

    int Y = y2 + 1;

    if (code == kbEscape || code == kbF8 || code == kbF12)
    {
        if (code == kbF12)
            calc_change_skin = true;
        code = kbEnter;
        return rv;
    }
    else if (code == kbF1)
    {
        messagebox(0, 0, atMBHelp, keyEscF1,
            CALCHELP
            NULL
            );
        return rv;
    }
    else if (code == kbF2 || code == kbF3 || code == kbF4 || code == kbF5 || code == kbF6 || code == kbF7
        || code == kbF9 || code == kbF10 || code == kbF11)
    {
        String N = "";
        #define GetNumber(KEY, NUM) case KEY: \
            N = Config.calcoptype == 0? \
                PrintResults0(PrevResult0, x1 + 1, Y, 0, NUM) : \
                PrintResults1(PrevResult1, x1 + 1, Y, 0, NUM);
        switch (code)
        {
/*exp*/     GetNumber(kbF2, 0) break;
/*dump*/    GetNumber(kbF3, 1)
                N += "ULL";
                break;
/*signed*/  GetNumber(kbF4, 2) break;
/*unsign*/  GetNumber(kbF5, 3) break;
/*oct*/     GetNumber(kbF6, 4)
                if (N != "0")
                    N.prepend("0");
                break;
/*bin*/     GetNumber(kbF7, 5)
                if (N != "0")
                    N.prepend("0b");
                break;
/*hex32*/   GetNumber(kbF9, 6)
                if (N != "0")
                    N.prepend("0x");
                break;
/*hex64*/   GetNumber(kbF10, 7)
                if (N != "0")
                {
                    N.prepend("0x");
                    N += "ULL";
                }
                break;
/*roman*/   GetNumber(kbF11, 8)
                if (N != "0")
                    N.prepend("0r");
                break;
        }
        #undef GetNumber
        e->clear_cb();
        e->init_cb(1);
        e->cb->atinsert(0, N.storecontents());
    }

    bool enter = code == kbEnter;

    if (CalculatorStarted)
        CalculatorStarted = false;
    else
    {
        if (!e->modified && !enter)
            return rv;
    }

    class dummy
    {
        bool    *flag;
    public:
        dummy(bool *f)  :flag(f) {}
        ~dummy()        {*flag = false;}
    }dummy(&e->modified);

    window(x1, Y, x2, Y + 13, 0, atNormalText);

    String Expr = (char *) e->at(0);

    Real z0;
    MP_Real z1;
    typedef void (*sigh)(int);
    sigh Old_SIGINT = signal(SIGINT, CalcBreak); /*todo: SIGABRT & SIGFPE*/
    int r;

    if (Config.calc_callback)
    {
        MP_Real::callback = callback;
        CB_X = x1 + 1;
        CB_Y = Y + 11;
        MP_Real::cb_senders = 0xffffffff;
    }

    try
    {
        if (Config.calcoptype == 1 && !enter) /* print out gmp stuff */
        {
            writestring(x1 + 1, Y + 1, "GNU MP claims to be fastest in the world..", atNormalText);
            writestring(x1 + 1, Y + 12, "Ctrl-Break to stop ;-)", atNormalText);
        }
        int fun = setjmp(__FPE_jb);
        if (fun == 0)
        {
            if (enter)
            {
                r = prev_rv;
                z0 = PrevResult0, z1 = PrevResult1;
            }
            else
            {
                if (Config.calcoptype == 0)
                    r = Inst_Calculate(Expr.c_str(), z0);
                else
                {
                    r = Inst_Calculate(Expr.c_str(), z1);
                    /*
                    if (r == ce_OK)
                        z1 = roundto(z1, Config.calcmpdigits);
                        */
                }
                prev_rv = r;
            }
        }
        else
        {
            r = ce_FPException;
            z0 = 0, z1 = 0;
            e->moveexact(0, 0);
            e->deletetoeol();
        }
    }
    catch (CalculatorStopped)
    {
        r = ce_OK;
        z0 = 0, z1 = 0;
    }
    catch (...) {throw;}

    signal(SIGINT, Old_SIGINT);
    MP_Real::callback = NULL;

    window(x1, Y, x2, Y + 13, 0, atNormalText);

    if (r != ce_OK && __RD__ == 0 && (r == ce_ExpressionEmpty || r == ce_ExpressionEmpty2 || r == ce_ExpressionEmpty3))
        r = ce_OK, z0 = 0, z1 = 0;

    if (r == ce_OK)
    {
        PrevResult0 = z0, PrevResult1 = z1;
        if (code == kbEnter)
        {
            if (Config.calcoptype == 0)
            {
                Expr = to_dec(z0, 10, 14, false);
                code = 0, rv = 1;
                e->atreplace(0, Expr.storecontents());
                e->moveexact(0, 0);
                e->setanchor(0, 0, Expr.getwidth(), 0);
                /*e->copyto_cb();*/
            }
            else
            {
                Expr = to_dec(z1, 10, Config.calcmpdigits, false);
                Expr.clip(CALC_LINE_MAX);
                code = 0, rv = 1;
                e->atreplace(0, Expr.storecontents());
                e->moveexact(0, 0);
                e->setanchor(0, 0, Expr.getwidth(), 0);
            }
        }
        if (Config.calcoptype == 0)
            PrintResults0(z0, x1 + 1, Y);
        else PrintResults1(z1, x1 + 1, Y);
    }
    else
    {
        int enter = 0;
		if (code == kbEnter)
            code = 0, enter = 1;
        if (Config.calcoptype == 0)
            PrintResults0(PrevResult0, x1 + 1, Y);
        else PrintResults1(PrevResult1, x1 + 1, Y);
        *buf2 = 0;
        if (__ErrorTokenDefined)
        {
            if (enter)
            {
                e->moveexact(__ErrorToken.start, 0);
                e->setanchor(__ErrorToken.start, 0, __ErrorToken.end + 1, 0);
            }
            String tok = __ErrorToken.token;
            if (tok.getwidth() > 13)
                tok = tok.at(0, 13) + "...";
            sprintf(buf2, " (`%s`, (%d, %d), \"%s\")",
                CToken::TTName(__ErrorToken.type),
                __ErrorToken.start + 1,
                __ErrorToken.end + 1,
                tok.c_str()
                );
        }
        if (__RD__ > 0 && (r == ce_ExpressionEmpty || r == ce_ExpressionEmpty2 || r == ce_ExpressionEmpty3))
            sprintf(buf2, " (Offset=%d)", __Offset__ + 1);
        sprintf(buf, "FAIL: \"%s\"%s", CalculatorErrorStr(r), buf2);
        buf[60] = 0;
        writestring(x1 + 1, Y + 12, buf, atTitle);
    }
    return rv;
}

extern void init_calc(int optype, int mpdigits);

void Calculator()
{
    do
    {
        char *screen_buf = savewindow(0, 0, SMX - 1, SMY - 1);

        static char buf[CALC_LINE_MAX] = {'\0'};

        FPUState State;

/*
        __fp_reset();
        _control87(MCW_EM, MCW_EM);
        typedef void (*_eh)(int);
        _eh OldSig = signal(SIGFPE, SIG_IGN);
*/

        CalculatorStarted = true;

        if (calc_change_skin)
        {
            switch (Config.calcoptype)
            {
                case 0:
                    Config.calcoptype = 1;
                    break;
                case 1:
                    Config.calcoptype = 0;
                    break;
            }
        }
        calc_change_skin = false;

        init_calc(Config.calcoptype, Config.calcmpdigits);

        inputline(buf, sizeof buf,
            "Enter expression:                                           ",
            CalculatorAction,
            13,
            true,
            0,
            0
            );

        CalculatorStarted = false;

/*fix        signal(SIGFPE, OldSig);*/

        restorewindow(screen_buf);
        delete[] screen_buf;

    } while (calc_change_skin);
}
#endif //HAS_CALCULATOR
