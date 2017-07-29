/*
** readconf.cpp
**
** Defines config file processing function.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#include <stdio.h>

#include "readconf.h"
#include "keyboard.h"
#include "video.h"
#include "tcollect.h"
#include "tok.h"
#include "zedclass.h"
#include "stlstuff.h"

extern char messbuf[];
extern "C++" void quit(char *, int, int, int = 1);

int getboolean(String const &V, int t, bool &field)
{
    String temp = V;

    if (t == ctSymbol)
    {
        temp.downcase();
        if (temp == "on" || temp == "true" || temp == "yes" || temp == "y")
            field = true;
        else if (temp == "off" || temp == "false" || temp == "no" || temp == "n")
            field = false;
        else return 1;
    }
    else if ((int)t & ctInteger)
    {
        int r = (int) CToken::ReadNumber(temp.c_str(), 0, __npos - 1, t);
        field = r? true : false;
    }
    else return 1;

    return 0;
}

int NextToken(CToken &token, int &x1, int &x2, int &x3, int &t)
{
    int C = 0;
    do
    {
        int r = token.GetNextPair(x1, x2, x3, t);
        if (r)
            break;
        if (t != ctComment2)
            C = 1;
    }
    while (C == 0);
    return C;
}

int search_define(TNSCollection &defs, String const &v1, String &Value)
{
    int Count = defs.getCount();
    if (Count == 0)
        return 0;
    for (int i = 0; i < Count; i++)
    {
        if (((Define*)defs.at(i))->V1 == v1)
        {
            Value = ((Define*)defs.at(i))->V2;
            return 1;
        }
    }
    return 0;
}

void insert_define(TNSCollection &defs, String const &v1, String const &v2)
{
    String V;
    if (search_define(defs, v1, V) == 0)
    {
        Define *D = new Define(v1, v2);
        defs.insert(ptr_add(PTR_Define, D));
    }
}

int GetColor(String s, BYTE &C)
{
    s.downcase();
    if (s == "black") C = black;
    else if (s == "blue") C = blue;
    else if (s == "green") C = green;
    else if (s == "cyan") C = cyan;
    else if (s == "red") C = red;
    else if (s == "purple" || s == "magenta") C = purple;
    else if (s == "brown") C = brown;
    else if (s == "lightgrey" || s == "lightgray") C = lightgrey;
    else if (s == "darkgrey" || s == "grey" || s == "darkgray" || s == "gray") C = darkgrey;
    else if (s == "lightblue") C = lightblue;
    else if (s == "lightgreen") C = lightgreen;
    else if (s == "lightcyan") C = lightcyan;
    else if (s == "lightred") C = lightred;
    else if (s == "lightpurple" || s == "lightmagenta") C = lightpurple;
    else if (s == "yellow") C = yellow;
    else if (s == "white") C = white;
    else return 0;
    return 1;
}

int readconfig(char *argv, zedConfig &C)
{
    String configname = getcomponent(argv, 4) + ".cfg";
    if (!fexist(configname.c_str()))
    {
        configname = getcomponent(argv, 1 + 2 + 4) + ".cfg";
        if (!fexist(configname.c_str()))
            return 0;
    }

    int const __tokenlen = 256;
    char buf[__tokenlen];

    try
    {
        File f(configname.c_str(), "r");

        int error = 0;
        int curline = 0;

        String S;

        CToken token;
        int t1, t2, t3, t4, t5;
        TNSCollection Defs(5, 5);

        int x1, x2, x3;
        int y1, y2, y3;
        int z1, z2, z3;
        int a1, a2, a3;
        int b1, b2, b3;

        String tok1, tok2, tok3, tok4, tok5;
        String DefValue;

        while (error == 0 && f.fgets(buf, __tokenlen))
        {
            curline++;

            S = buf;

            //MSS_SET_BLOCK_LABEL(const_cast<char*>(S.c_str()), "l1");

            token.init(S);

            int count = 0;

            count += NextToken(token, x1, x2, x3, t1);
            count += NextToken(token, y1, y2, y3, t2);
            count += NextToken(token, z1, z2, z3, t3);
            count += NextToken(token, a1, a2, a3, t4);
            count += NextToken(token, b1, b2, b3, t5);
            if (count == 5 && (t5 == ctComment1 || t5 == ctComment2))
                count--;
            if (count == 4 && (t4 == ctComment1 || t4 == ctComment2))
                count--;

            if (count == 3 && t1 == ctPreprocessor)
            {
                tok1 = S.fromto(x1, x2);
                tok1.downcase();
                tok1.removeall(' ');
                if (tok1 != "#define")
                {
                    error = 5;
                    break;
                }
                if (t2 != ctSymbol || t3 != ctSymbol)
                {
                    error = 6;
                    break;
                }
                tok2 = S.fromto(y1, y2);
                tok3 = S.fromto(z1, z2);
                insert_define(Defs, tok2, tok3);
                continue;
            }

            if (count == 5 && t1 == ctSymbol)
            {
                tok1 = S.fromto(x1, x2);
                tok2 = S.fromto(y1, y2);
                tok3 = S.fromto(z1, z2);
                tok4 = S.fromto(a1, a2);
                tok5 = S.fromto(b1, b2);
                if (tok2 != "=")
                {
                    error = 2;
                    break;
                }
                if (t1 != ctSymbol || t3 != ctSymbol || t4 != ctSymbol || t5 != ctSymbol)
                {
                    error = 100;
                    break;
                }
                tok4.downcase();
                if (tok4 != "on")
                {
                    error = 100;
                    break;
                }
                if (search_define(Defs, tok1, DefValue))
                    tok1 = DefValue;
                if (search_define(Defs, tok3, DefValue))
                    tok3 = DefValue;
                if (search_define(Defs, tok5, DefValue))
                    tok5 = DefValue;
                BYTE C1, C2;
                int r1 = GetColor(tok3, C1);
                int r2 = GetColor(tok5, C2);
                if (!r1 || !r2)
                {
                    error = 7;
                    break;
                }
                BYTE A = makeattr(C2, C1);
                tok1.downcase();
                if (tok1 == "atnormaltext"      ) C.atNormalText = A;
                #define SET_AT(Name, Field) \
                    else if (tok1 == Name) C.Field = A;
                SET_AT("atselect"     , atSelect)               SET_AT("atkeyword"    , atKeyWord)
                SET_AT("atfunction"   , atFunction)             SET_AT("atspecial"    , atSpecial)
                SET_AT("atlabel"      , atLabel)                SET_AT("atsyntax"     , atSyntax)
                SET_AT("atinvalid"    , atInvalid)              SET_AT("atoctal"      , atOctal)
                SET_AT("athex"        , atHex)                  SET_AT("atdecimal"    , atDecimal)
                SET_AT("atfloat"      , atFloat)                SET_AT("atbinary"     , atBinary)
                SET_AT("atchar"       , atChar)                 SET_AT("atstring"     , atString)
                SET_AT("atpreprocess" , atPreprocess)           SET_AT("atpreprocessunknown" , atPreprocessUnknown)
                SET_AT("atstdinclude" , atStdinclude)
                SET_AT("atcomment"    , atComment)              SET_AT("attitle"      , atTitle)
                SET_AT("atstatusline" , atStatusLine)           SET_AT("atmainframe"  , atMainFrame)
                SET_AT("atmbnormal"   , atMBNormal)             SET_AT("atmbsave"     , atMBSave)
                SET_AT("atmbhelp"     , atMBHelp)               SET_AT("atmberror"    , atMBError)
                SET_AT("atrulern"     , atRulerN)               SET_AT("atruleri"     , atRulerI)
                else
                {
                    error = 8;
                    break;
                }
                continue;
            }

            if (count > 3)
                count = 3;

            if ((count && t1 != ctSymbol) || !count)
                continue;

            if (count != 3)
            {
                error = 100;
                break;
            }

            if (count == 3)
            {
                tok1 = S.fromto(x1, x2);
                tok2 = S.fromto(y1, y2);
                tok3 = S.fromto(z1, z2);
                if (search_define(Defs, tok1, DefValue))
                    tok1 = DefValue;
                if (t3 == ctSymbol && search_define(Defs, tok3, DefValue))
                    tok3 = DefValue;
                tok1.downcase();

                if (tok2 != "=")
                {
                    error = 2;
                    break;
                }

                if (tok1 == "autoindent")                           error = getboolean(tok3, t3, C.autoindent);

                else if (tok1 == "debug")                           error = getboolean(tok3, t3, C.debug);

                else if (tok1 == "cursorbeyondend")                 error = getboolean(tok3, t3, C.cursorbeyondend);
                else if (tok1 == "autotabsizeonload")               error = getboolean(tok3, t3, C.autotabsizeonload);
                else if (tok1 == "persistentblocks")                error = getboolean(tok3, t3, C.persistent);
                else if (tok1 == "autobrackets")                    error = getboolean(tok3, t3, C.autobrackets);
                else if (tok1 == "highlight")                       error = getboolean(tok3, t3, C.highlight);
                else if (tok1 == "escexits")                        error = getboolean(tok3, t3, C.esc_exits);
                else if (tok1 == "calc_callback")                   error = getboolean(tok3, t3, C.calc_callback);
                else if (tok1 == "multichar_nasm")                  error = getboolean(tok3, t3, C.multichar_nasm);
                else if (tok1 == "grayenter_macro")                 error = getboolean(tok3, t3, C.grayenter_macro);
                else if (tok1 == "brief_home")                      error = getboolean(tok3, t3, C.brief_home);
                else if (tok1 == "multilinecomments")               error = getboolean(tok3, t3, C.multilinecomments);
                else if (tok1 == "analize_rename")                  error = getboolean(tok3, t3, C.analize_rename);
                else if (tok1 == "cache_video")                     error = getboolean(tok3, t3, C.cache_video);
                else if (tok1 == "vsync")                           error = getboolean(tok3, t3, C.vsync);
                else if (tok1 == "block_insert")                    error = getboolean(tok3, t3, C.block_insert);
                else if (tok1 == "unix_end")                        error = getboolean(tok3, t3, C.unix_end);
                else if (tok1 == "exit_cleanup")                    error = getboolean(tok3, t3, C.exit_cleanup);

                #define GET_INT(Name, Min, Max, Field) \
                    else if (tok1 == Name) \
                    { \
                        if ((int)t3 & ctInteger) \
                        { \
                            int v = (int) CToken::ReadNumber(S.c_str(), z1, z2, t3); \
                            if (v >= Min && v <= Max) C.Field = v; \
                            else error = 3; \
                        } else error = 4; \
                    }
                GET_INT("hardtabsize", 2, 32, hardtabsize)
                GET_INT("tabsize", 2, 32, tab_size)
                GET_INT("tabsizeasm", 2, 32, tab_size_asm)
                GET_INT("calcmpdigits", 20, 256, calcmpdigits)
                else if (tok1 == "calcoptype")
                {
                    tok3.downcase();
                    if (tok3 == "real")
                        C.calcoptype = 0;
                    else if (tok3 == "mp_real")
                        C.calcoptype = 1;
                    else error = 90;
                }

                else error = 99; // unknown var. name

#if 0
                printf("%s, %s, %s (t=(%s), v=%s, error=%d)\n",
                    tok1.c_str(),
                    tok2.c_str(),
                    tok3.c_str(),
                    CToken::TTName(t3),
                    to_dec(CToken::ReadNumber(S.c_str(), z1, z2, t3), 10, 20, false).c_str(),
                    error);
                readkey();
#endif

            }

        } // while

        if (error)
        {
            String __cause = "";
            switch (error)
            {
                #define r(s) __cause = s; break;
                case 1:         r("invalid boolean setting")
                case 2:         r("missing =")
                case 3:         r("numeric constant out of range")
                case 4:         r("invalid numeric setting")

                case 5:         r("unknown preprocessor directive")
                case 6:         r("bad preprocessor directive syntax")
                case 7:         r("bad attribute setting")
                case 8:         r("unknown attribute")

                case 90:        r("invalid setting")
                case 99:        r("weird variable name")
                case 100:       r("syntax violation")
                #undef r
            }
            if (error == 1)
                __cause += " (\"" + tok3 + "\")";
            else if (error == 3 || error == 4)
            {
                sprintf(messbuf, " (\"%s\" (%s) = %s)",
                    tok3.c_str(),
                    CToken::TTName(t3),
                    "666" //to_dec(CToken::ReadNumber(S.c_str(), z1, z2, t3), 10, 20, false).c_str()
                    );
                __cause += messbuf;
            }
            else if (error == 99)
                __cause += " (\"" + tok1 + "\")";

            sprintf(messbuf, "\n" prgname ": error \"%s\" in config file \"%s\" line %d\n\n",
                __cause.c_str(),
                configname.c_str(),
                curline);

            quit(messbuf, 1, 0, 1);
        }

    }
    catch (FileException) {return 0;}
    catch (...) {throw;}

    return 1;
}
