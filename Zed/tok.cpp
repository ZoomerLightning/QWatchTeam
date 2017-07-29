/*
** tok.cpp
**
** Defines class CToken members.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#include <stdio.h>
#include <stdlib.h>

#include "tok.h"

char CToken::SyntaxTable[256];
char CToken::__SyntaxTable[256];
#ifdef HAS_HASKELL_PARSER
char CToken::SyntaxTable_Haskell[256];
char CToken::__SyntaxTable_Haskell[256];
#endif
#ifdef HAS_PASCAL_PARSER
char CToken::SyntaxTable_Pascal[256];
char CToken::__SyntaxTable_Pascal[256];
#endif
#ifdef HAS_CMINUSMINUS_PARSER
char CToken::SyntaxTable_CMinus[256];
char CToken::__SyntaxTable_CMinus[256];
#endif
#ifdef HAS_NASM_PARSER
char CToken::SyntaxTable_Nasm[256];
char CToken::__SyntaxTable_Nasm[256];
#endif

void CToken::CreateSyntaxTables()
{
    static int done = 0;
    if (done)
        return;

//!"#$%&'()*+,-./0123456789:;<=>?
//@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_
//`abcdefghijklmnopqrstuvwxyz{|}~
    {
        char *Syntax = "!%&()+,-:;<=>?[\\]^{|}~";
        char *__Syntax = "!%&:<=>^|"; // "-~+" might be unary

        memset(SyntaxTable, 0, sizeof(SyntaxTable));
        memset(__SyntaxTable, 0, sizeof(__SyntaxTable));

        while (*Syntax)
            SyntaxTable[(_BYTE)*Syntax++] = 1;
        while (*__Syntax)
            __SyntaxTable[(_BYTE)*__Syntax++] = 1;
    }

//!"#$%&'()*+,-./0123456789:;<=>?
//@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_
//`abcdefghijklmnopqrstuvwxyz{|}~
#ifdef HAS_HASKELL_PARSER
    {
        char *Syntax_Haskell = "!$%&()*+,./:;<=>?@[\\]^`|}~";
        char *__Syntax_Haskell = "";

        memset(SyntaxTable_Haskell, 0, sizeof(SyntaxTable_Haskell));
        memset(__SyntaxTable_Haskell, 0, sizeof(__SyntaxTable_Haskell));

        while (*Syntax_Haskell)
            SyntaxTable_Haskell[(_BYTE)*Syntax_Haskell++] = 1;
        while (*__Syntax_Haskell)
            __SyntaxTable_Haskell[(_BYTE)*__Syntax_Haskell++] = 1;
    }
#endif

//!"#$%&'()*+,-./0123456789:;<=>?
//@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_
//`abcdefghijklmnopqrstuvwxyz{|}~
#ifdef HAS_CMINUSMINUS_PARSER
    {
        char *Syntax_CMinus = "!#$%&()+,-.:;<=>@[\\]^{|}";
        char *__Syntax_CMinus = "";

        memset(SyntaxTable_CMinus, 0, sizeof(SyntaxTable_CMinus));
        memset(__SyntaxTable_CMinus, 0, sizeof(__SyntaxTable_CMinus));

        while (*Syntax_CMinus)
            SyntaxTable_CMinus[(_BYTE)*Syntax_CMinus++] = 1;
        while (*__Syntax_CMinus)
            __SyntaxTable_CMinus[(_BYTE)*__Syntax_CMinus++] = 1;
    }
#endif

//!"#$%&'()*+,-./0123456789:;<=>?
//@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_
//`abcdefghijklmnopqrstuvwxyz{|}~
#ifdef HAS_PASCAL_PARSER
    {
        char *Syntax_Pascal = ")+,-.:;<=>@[]";
        char *__Syntax_Pascal = "";

        memset(SyntaxTable_Pascal, 0, sizeof(SyntaxTable_Pascal));
        memset(__SyntaxTable_Pascal, 0, sizeof(__SyntaxTable_Pascal));

        while (*Syntax_Pascal)
            SyntaxTable_Pascal[(_BYTE)*Syntax_Pascal++] = 1;
        while (*__Syntax_Pascal)
            __SyntaxTable_Pascal[(_BYTE)*__Syntax_Pascal++] = 1;
    }
#endif

//!"#$%&'()*+,-./0123456789:;<=>?
//@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_
//`abcdefghijklmnopqrstuvwxyz{|}~
#ifdef HAS_NASM_PARSER
    {
        char *Syntax_Nasm = "!#&()*+,-/:<=>@[]^{|}~";
        char *__Syntax_Nasm = "";

        memset(SyntaxTable_Nasm, 0, sizeof(SyntaxTable_Nasm));
        memset(__SyntaxTable_Nasm, 0, sizeof(__SyntaxTable_Nasm));

        while (*Syntax_Nasm)
            SyntaxTable_Nasm[(_BYTE)*Syntax_Nasm++] = 1;
        while (*__Syntax_Nasm)
            __SyntaxTable_Nasm[(_BYTE)*__Syntax_Nasm++] = 1;
    }
#endif

    done = 1;
}

char const *CToken::TTName(int t)
{
    char const *S = "Unknown Token Type";

    switch (t)
    {
        #define r(s) S = s; break;

        case ctInvalid:         r("invalid")

        case ctOctal:           r("octal")
        case ctHex:             r("hex")
        case ctDecimal:         r("decimal")
        case ctBinary:          r("binary")
        case ctRoman:           r("roman")
        case ctFloat:           r("float")

        case ctChar:            r("char")
        case ctString:          r("string")

        case ctSymbol:          r("symbol")
        case ctPreprocessor:    r("preprocessor")
        case ctJumpLabel:       r("jumplabel")

        case ctComment1:        r("comment1")
        case ctComment2:        r("comment2")
        case ctComment3:        r("comment3")
        case ctComment4:        r("comment4")

        case ctSyntax:          r("syntax")

        case ctStdinclude:      r("stdinclude")

        #undef r
    }

    return S;
}

unsigned CToken::read_char(char const *S, bool little_endian)
{
#define WEIRD_CAST(OBJECT)  (unsigned)(unsigned char)OBJECT
    String s(S);
    if (s.__empty() || s.getwidth() <= 2)
        return '\0';
    int L = s.getwidth();
    if (s[1] != '\\')
    {
        switch (L)
        {
            case 3:
                return WEIRD_CAST(s[1]);
            case 4:
                if (little_endian)
                    return (WEIRD_CAST(s[2]) << 8) | WEIRD_CAST(s[1]);
                else return (WEIRD_CAST(s[1]) << 8) | WEIRD_CAST(s[2]);
            case 5:
                if (little_endian)
                    return (WEIRD_CAST(s[3]) << 16) | (WEIRD_CAST(s[2]) << 8) | WEIRD_CAST(s[1]);
                else return (WEIRD_CAST(s[1]) << 16) | (WEIRD_CAST(s[2]) << 8) | WEIRD_CAST(s[3]);
            case 6:
                if (little_endian)
                    return (WEIRD_CAST(s[4]) << 24) | (WEIRD_CAST(s[3]) << 16) | (WEIRD_CAST(s[2]) << 8) | WEIRD_CAST(s[1]);
                else return (WEIRD_CAST(s[1]) << 24) | (WEIRD_CAST(s[2]) << 16) | (WEIRD_CAST(s[3]) << 8) | WEIRD_CAST(s[4]);
            default:
                return '\0';
        }
    }
    else
    {
        if (L == 3)
            return '\0';
        if (strchr("\'\"\?\\", (_BYTE)s[2]))
            return WEIRD_CAST(s[2]);
        if (strchr("abfnrtv", (_BYTE)s[2]))
        {
            switch (s[2])
            {
                case 'a':   return WEIRD_CAST('\a');
                case 'b':   return WEIRD_CAST('\b');
                case 'f':   return WEIRD_CAST('\f');
                case 'n':   return WEIRD_CAST('\n');
                case 'r':   return WEIRD_CAST('\r');
                case 't':   return WEIRD_CAST('\t');
                case 'v':   return WEIRD_CAST('\v');
            }
        }
        else
        {
            if (s[2] == 'x' || s[2] == 'X')
            {
                if (L < 5)
                    return '\0';
                String hex = "";
                for (int i = 3; i < L - 1 && ISXDIGIT(s[i]); ++i)
                    hex += s[i];
                if (hex.empty())
                    return '\0';
				unsigned v;
				sscanf(hex.c_str(), "%x", &v);
				return v;
            }
            else
            {
                if (!ISODIGIT(s[2]))
                    return '\0';
                String octal = s[2];
                if (L > 4 && ISODIGIT(s[3]))
                    octal += s[3];
                if (L > 5 && ISODIGIT(s[4]))
                    octal += s[4];
				unsigned v;
				sscanf(octal.c_str(), "%o", &v);
                return (unsigned)v;
            }
        }
    }
    return '\0';
#undef WEIRD_CAST
}

bool CToken::Little_endian = true;

double CToken::ReadNumber(String const &s, int x1, int x2, int t)
{
	if (((int)t & ctNumber) == 0)
		return 0;

	String W = s.fromto(x1, x2);
    double r = 0;

    if (t == ctChar)
        return read_char(W.c_str(), Little_endian);

    W.downcase();

    if (t != ctRoman)
        W.removeall('l');
    if (t == ctFloat)
        W.removeall('f');
    else W.removeall('u');
    if (t == ctHex || t == ctBinary || t == ctRoman)
        W.remove(0, 2);

    switch (t)
    {
		case ctOctal:
								{
									unsigned r_u;
									sscanf(W.c_str(), "%o", &r_u);
									r=r_u;
								}
		// todo: handle errno and pass error to CTokenAtom
								break;
		case ctHex:
								{
									unsigned r_u;
									sscanf(W.c_str(), "%x", &r_u);
									r=r_u;
								}
								break;
		case ctBinary:          throw 666;
								break;
		case ctRoman:           throw 666;
								break;
		case ctDecimal:
		case ctFloat:           sscanf(W.c_str(), "%le", &r);
                                break;
        default:
            r = 0;
    }

    return r;
}

int CToken::SkipWS()
{
    char const *s = _s.c_str();

    if (!L || bad(pos))
        return 1;

    while (ISSPACE(s[pos]) && !bad(++pos));
    if (bad(pos))
        return 2;

    return 0;
}

int CToken::GetNextPair(int &X1, int &X2, int &X3, int &t)
{
    char const *s = _s.c_str();

    X1 = X2 = X3 = pos;
	t = ctInvalid;

    if (!L || bad(pos))
        return 1;

    int dummy = SkipWS();
    X1 = X2 = X3 = pos;

    if (dummy)
        return 1;

    char CurChar = s[pos];

    if (!ISPRINTABLEASCII(CurChar))
    {
        while (!bad(++pos) && !ISPRINTABLEASCII(s[pos]) && !ISSPACE(s[pos]));
        X2 = pos - 1;
        t = ctInvalid;
		return 0;
    }

    if (_ISALPHA(CurChar) || CurChar == '_')
    {
        while (!bad(++pos) && (_ISALNUM(s[pos]) || s[pos] == '_'));
        X3 = X2 = pos - 1;
        t = ctSymbol;
        /*todo: ctJumpLabel*/
    }
    else if (CurChar == '#')
    {
        if (!bad(pos - 1) && s[pos - 1] == '#')
        {
            X2 = X1;
            pos++;
            t = ctInvalid;
            return 0;
		}

        int i = pos;
        bool error = false;
        while (!bad(--i))
        {
            if (!ISSPACE(s[i]))
            {
                error = true;
                break;
            }
        }

        pos++;
        if (error || SkipWS() || !(_ISALPHA(s[pos]) || s[pos] == '_'))
        {
            if (!bad(pos) && s[pos] == '#' && s[pos - 1] == '#' && !bad(pos + 1) && s[pos + 1] != '#')
            {
				X3 = X2 = X1 + 1;
                pos++;
                t = ctPreprocessor;
                return 0;
            }
            X2 = X1;
            t = (error && !bad(pos) && s[pos] != '#') ? ctPreprocessor : ctInvalid;
        }
        else
        {
            X3 = pos;
            while (!bad(++pos) && (_ISALNUM(s[pos]) || s[pos] == '_'));
            X2 = pos - 1;
            t = ctPreprocessor;
            pp = true;
            ppx1 = X3, ppx2 = X2;
        }
    }
	else if (SyntaxTable[(_BYTE)CurChar])
    {
        if (pp && CurChar == '<' && _s.fromto(ppx1, ppx2) == "include")
        {
            while (!bad(++pos) && (_BYTE)s[pos] != '>');
            if (bad(pos))
            {
                X2 = pos - 1;
                t = ctInvalid;
            }
            else
            {
                X2 = pos++;
                pp = false;
                t = ctStdinclude;
            }
        }
        else
		{
            if (__SyntaxTable[(_BYTE)s[pos]])
            {
                while (!bad(++pos) && __SyntaxTable[(_BYTE)s[pos]]);
                X2 = pos - 1;
            }
            else X2 = pos++;
            t = ctSyntax;
        }
    }
    else if (CurChar == '/')
    {
        if (bad(++pos) || !(s[pos] == '/' || s[pos] == '*'))
        {
            if (!bad(pos) && __SyntaxTable[(_BYTE)s[pos]])
            {
                while (__SyntaxTable[(_BYTE)s[pos]] && !bad(++pos));
                X2 = pos - 1;
			}
            else X2 = X1;
            t = ctSyntax;
        }
        else
        {
            if (s[pos] == '/')
            {
                X2 = L - 1;
                pos = L;
                t = ctComment1;
            }
            else if (s[pos] == '*')
            {
                int i = pos + 1;

                do
                {
					while (!bad(i) && s[i] != '*')
                        i++;
                    if (bad(i))
                        break;
                    if (!bad(++i) && s[i] == '/')
                    {
                        X2 = i;
                        pos = i + 1;
                        t = ctComment2;
                        return 0;
                    }
                } while (!bad(i));

                X2 = L - 1;
                pos = L;
                t = ctComment3;

            }
		}
    }
    else if (CurChar == '*')
    {
        if (bad(++pos) || s[pos] != '/')
        {
            if (!bad(pos) && (__SyntaxTable[(_BYTE)s[pos]] || s[pos] == '*'))
            {
                while ((__SyntaxTable[(_BYTE)s[pos]]
                    || (s[pos] == '*' && !(!bad(pos + 1) && s[pos + 1] == '/'))) && !bad(++pos));
                X2 = pos - 1;
            }
            else X2 = X1;
            t = ctSyntax;
        }
        else
        {
            if (!bad(pos + 1) && (s[pos + 1] == '*' || s[pos + 1] == '/')) //todo:
			{
                X2 = pos - 1;
                t = ctSyntax;
            }
            else
            {
                X1 = 0;
                X2 = pos++;
                t = ctComment4;
            }
        }
    }
    else if (CurChar == '\'')
    {
        int i = pos;

        while (!bad(++i) && s[i] != '\'')
        {
			if (s[i] == '\\' && !bad(i + 1))
            {
                i++;
            }
        }

        if (!bad(i))
        {
            X2 = i;
            pos = i + 1;
            t = ctChar;
        }
        else
        {
            X2 = X1;
            ++pos;
            t = ctInvalid;
        }

    }
    else if (CurChar == '"')
    {
        int i = pos;

        while (!bad(++i) && s[i] != '"')
        {
            if (s[i] == '\\' && !bad(i + 1))
            {
                i++;
            }
        }

        X2 = bad(i) ? (L - 1) : i;
        pos = X2 + 1;
        t = ctString;

	}
    else if (ISDIGIT(CurChar) || CurChar == '.')
    {
        char c = s[pos++];

        if (bad(pos))
        {
            X2 = X1;
            if (c == '.')
                t = ctSyntax;
            else t = ctDecimal;
        }
        else
        {
            if (c == '.' && !ISDIGIT(s[pos]))
            {
                X2 = X1;
                t = ctSyntax;
				return 0;
            }

            static char const *const
                badend = ".";

            if (c == '0' && _TOUPPER(s[pos]) == 'X')
            {
                if (bad(++pos) || !ISXDIGIT(s[pos]))
                {
                    X2 = X1 + 1;
                    t = ctInvalid;
                    return 0;
                }

                bool LZ = s[pos] == '0';
                int count = LZ? 0 : 1;

				int _32BitPos = 0;
                bool _64BitNumber = false;

                while (!bad(++pos) && ISXDIGIT(s[pos]) && count < 16) // 64-bit hex allowed (see below)
                {
                    if (LZ && s[pos] != '0')
                        LZ = false;
                    if (!LZ)
                        count++;
                    if (count == 8)
                        _32BitPos = pos;
                }

                //
                // DJGPP 64-bit hex number checking
                //

                /**/

                if ((!bad(pos + 1) && _TOUPPER(s[pos]) == 'L' && _TOUPPER(s[pos + 1]) == 'L')
                    || (!bad(pos + 2) && _TOUPPER(s[pos]) == 'U' && _TOUPPER(s[pos + 1]) == 'L' && _TOUPPER(s[pos + 2]) == 'L'))
                    _64BitNumber = true;

                if (count > 8 && !_64BitNumber)
                {
                    int modif = 0;
                    while (modif < 3 && !bad(pos) && (_TOUPPER(s[pos]) == 'U' || _TOUPPER(s[pos]) == 'L'))
                        pos++, modif++;
                    X2 = pos - 1;//_32BitPos;
                    //pos = _32BitPos + 1;
                    t = ctInvalid;
                    return 0;
                }

                /**/

				int modif = 0;
                while (modif < 3 && !bad(pos) && (_TOUPPER(s[pos]) == 'U' || _TOUPPER(s[pos]) == 'L'))
                    pos++, modif++;
                X2 = pos - 1;
                if (!bad(pos) && (ISDIGIT(s[pos]) || strchr(badend, (_BYTE)s[pos])))
                    t = ctInvalid;
                else t = ctHex;
                return 0;
            }

            if (c == '0' && _TOUPPER(s[pos]) == 'B')
            {
                if (bad(++pos) || !ISBDIGIT(s[pos]))
                {
                    X2 = X1 + 1;
                    t = ctInvalid;
                    return 0;
                }
				bool LZ = s[pos] == '0';
                int count = LZ? 0 : 1;
                int _32BitPos = 0;
                bool _64BitNumber = false;
                while (!bad(++pos) && ISBDIGIT(s[pos]) && count < 64)
                {
                    if (LZ && s[pos] != '0')
                        LZ = false;
                    if (!LZ)
                        count++;
                    if (count == 32)
                        _32BitPos = pos;
                }
                if ((!bad(pos + 1) && _TOUPPER(s[pos]) == 'L' && _TOUPPER(s[pos + 1]) == 'L')
                    || (!bad(pos + 2) && _TOUPPER(s[pos]) == 'U' && _TOUPPER(s[pos + 1]) == 'L' && _TOUPPER(s[pos + 2]) == 'L'))
                    _64BitNumber = true;
                if (count > 32 && !_64BitNumber)
                {
					int modif = 0;
                    while (modif < 3 && !bad(pos) && (_TOUPPER(s[pos]) == 'U' || _TOUPPER(s[pos]) == 'L'))
                        pos++, modif++;
                    X2 = pos - 1;
                    t = ctInvalid;
                    return 0;
                }
                int modif = 0;
                while (modif < 3 && !bad(pos) && (_TOUPPER(s[pos]) == 'U' || _TOUPPER(s[pos]) == 'L'))
                    pos++, modif++;
                X2 = pos - 1;
                if (!bad(pos) && (ISDIGIT(s[pos]) || strchr(badend, (_BYTE)s[pos])))
                    t = ctInvalid;
                else t = ctBinary;
                return 0;
            }

            if (c == '0' && _TOLOWER(s[pos]) == 'r')
			{
                if (bad(++pos) || !ISRDIGIT(s[pos]))
                {
                    X2 = X1 + 1;
                    t = ctInvalid;
                    return 0;
                }
                while (!bad(++pos) && ISRDIGIT(s[pos]));
                X2 = pos - 1;
                if (!bad(pos) && (ISDIGIT(s[pos]) || strchr(badend, (_BYTE)s[pos])))
                    t = ctInvalid;
                else t = ctRoman;
                return 0;
            }

            if (c == '.' && ISDIGIT(s[pos]))
            {
                while (!bad(++pos) && ISDIGIT(s[pos]));
				bool error = false;
                if (!bad(pos) && _TOUPPER(s[pos]) == 'E')
                {
                    pos++, error = true;
                    if (!bad(pos) && (s[pos] == '-' || s[pos] == '+'))
                        pos++;
                    while (!bad(pos) && ISDIGIT(s[pos]))
                        pos++, error = false;
                }
                if (!bad(pos) && (_TOUPPER(s[pos]) == 'F' || _TOUPPER(s[pos]) == 'L'))
                    pos++;
                X2 = pos - 1;
                t = ctFloat;
                if (!bad(pos) && (ISDIGIT(s[pos]) || strchr(badend, (_BYTE)s[pos])))
                    t = ctInvalid;
                if (error)
                    t = ctInvalid;
                return 0;
			}

            bool octalonly = true;

            while (ISDIGIT(s[pos]))
            {
                if (s[pos] == '8' || s[pos] == '9')
                    octalonly = false;
                if (bad(++pos))
                    break;
            }

            if (bad(pos) || (!(s[pos] == '.' || _TOUPPER(s[pos]) == 'E')))
            {
                int modif = 0;
                while (modif < 3 && !bad(pos) && (_TOUPPER(s[pos]) == 'U' || _TOUPPER(s[pos]) == 'L'))
                    pos++, modif++;
                X2 = pos - 1;
				t = (c == '0' && (X2 - X1 > 0) && (X2 - X1 != modif))? ctOctal : ctDecimal;
                if (!bad(pos) && (ISDIGIT(s[pos]) || strchr(badend, (_BYTE)s[pos])))
                    t = ctInvalid;
                if (t == ctOctal && !octalonly)
                    t = ctInvalid;
                return 0;
            }

            bool error = false;
            if (_TOUPPER(s[pos]) == 'E')
                pos++, error = true;
            else
            {
                while (!bad(++pos) && ISDIGIT(s[pos]));
                if (!bad(pos) && _TOUPPER(s[pos]) == 'E')
                    pos++, error = true;
            }
            if (error)
			{
                if (!bad(pos) && (s[pos] == '-' || s[pos] == '+'))
                    pos++;
                while (!bad(pos) && ISDIGIT(s[pos]))
                    pos++, error = false;
            }

            if (!bad(pos) && (_TOUPPER(s[pos]) == 'F' || _TOUPPER(s[pos]) == 'L'))
                pos++;
            X2 = pos - 1;
            t = ctFloat;
            if (!bad(pos) && (ISDIGIT(s[pos]) || strchr(badend, (_BYTE)s[pos])))
                t = ctInvalid;
            if (error)
                t = ctInvalid;

        }
    }
	else
    {
        pos++;
        X2 = X1;
        t = ctInvalid;
    }
    return 0;
}

#ifdef HAS_PASCAL_PARSER
int CToken::GetNextPair_Pascal(int &X1, int &X2, int &X3, int &t)
{
    char const *s = _s.c_str();

    X1 = X2 = X3 = pos;
    t = ptInvalid;

    if (!L || bad(pos))
		return 1;

    int dummy = SkipWS();
    X1 = X2 = X3 = pos;

    if (dummy)
        return 1;

    char CurChar = s[pos];

    if (!ISPRINTABLEASCII(CurChar))
    {
        while (!bad(++pos) && !ISPRINTABLEASCII(s[pos]) && !ISSPACE(s[pos]));
        X2 = pos - 1;
        t = ptInvalid;
        return 0;
    }

	if (_ISALPHA(CurChar) || CurChar == '_')
    {
        while (!bad(++pos) && (_ISALNUM(s[pos]) || s[pos] == '_'));
        X3 = X2 = pos - 1;
        t = ptSymbol;
    }
    else if (SyntaxTable_Pascal[(_BYTE)CurChar])
    {
        if (__SyntaxTable_Pascal[(_BYTE)s[pos]])
        {
            while (!bad(++pos) && __SyntaxTable_Pascal[(_BYTE)s[pos]]);
            X2 = pos - 1;
        }
        else X2 = pos++;
        t = ptSyntax;
    }
    else if (CurChar == '/')
    {
		if (bad(++pos) || !(s[pos] == '/'))
        {
            if (!bad(pos) && __SyntaxTable_Pascal[(_BYTE)s[pos]])
            {
                while (__SyntaxTable_Pascal[(_BYTE)s[pos]] && !bad(++pos));
                X2 = pos - 1;
            }
            else X2 = X1;
            t = ptSyntax;
        }
        else
        {
            X2 = L - 1;
            pos = L;
            t = ptComment1;
        }
    }
    else if (CurChar == '*')
	{
        if (bad(++pos) || s[pos] != ')')
        {
            if (!bad(pos) && (__SyntaxTable_Pascal[(_BYTE)s[pos]]))
            {
                while (__SyntaxTable_Pascal[(_BYTE)s[pos]] && !bad(++pos));
                X2 = pos - 1;
            }
            else X2 = X1;
            t = ptSyntax;
        }
        else
        {
            X1 = 0;
            X2 = pos++;
            t = ptComment4;
        }
    }
	else if (CurChar == '\'')
    {
        int i = pos;

        while (!bad(++i))
        {
            if (s[i] == '\'')
            {
                if (!bad(i + 1) && s[i + 1] == '\'')
                    i++;
                else break;
            }
        }

        if (!bad(i))
        {
            X2 = i;
            pos = i + 1;
			if (X2 - X1 == 2 || (X2 - X1 == 3 && s[X1 + 1] == '\'' && s[X2 - 1] == '\''))
                t = ptChar;
            else t = ptString;
        }
        else
        {
            X2 = i - 1;
            pos = i;
            t = ptString;
        }

    }
    else if (CurChar == '#')
    {
        if (bad(++pos) || !(ISDIGIT(s[pos]) || s[pos] == '$'))
        {
            X2 = X1;
            t = ptInvalid;
		}
        else
        {
            if (s[pos] == '$')
            {
                bool LZ = true;
                int count = 0;
                while (!bad(++pos) && ISXDIGIT(s[pos]) && count < 2)
                {
                    if (s[pos] != '0')
                        LZ = false;
                    if (!LZ)
                        count++;
                }
            }
            else
            {
                bool LZ = s[pos] == '0';
				int count = LZ? 0 : 1;
                while (!bad(++pos) && ISDIGIT(s[pos]) && count < 3)
                {
                    if (s[pos] != '0')
                        LZ = false;
                    if (!LZ)
                        count++;
                }
            }
            X2 = pos - 1;
            t = ptChar;
        }
    }
    else if (CurChar == '^')
    {
        if (bad(++pos))
        {
            X2 = X1;
			t = ptSyntax;
        }
        else
        {
            if (_ISALPHA(s[pos]) && !(!bad(pos + 1) && (_ISALNUM(s[pos + 1]) || s[pos + 1] == '_')))
            {
                X2 = pos++;
                t = ptChar;
            }
            else
            {
                X2 = X1;
                t = ptSyntax;
            }
        }
    }
    else if (CurChar == '{')
    {
		if (bad(++pos))
        {
            X2 = pos - 1;
            t = ptComment3;
        }
        else
        {
            while (s[pos] != '}' && !bad(++pos));
            if (bad(pos))
            {
                if (s[X1 + 1] == '$')
                    t = ptInvalid;
                else t = ptComment3;
                X2 = pos - 1;
            }
            else
            {
                if (s[X1 + 1] == '$')
				{
                    t = (!bad(X1 + 2) && ISSPACE(s[X1 + 2]))? ptInvalid : ptPreprocessor;
                }
                else t = ptComment2;
                X2 = pos++;
            }
        }
    }
    else if (CurChar == '}')
    {
        t = ptComment4;
        X1 = 0;
        X2 = pos++;
    }
    else if (CurChar == '(')
    {
        if (bad(++pos) || s[pos] != '*')
        {
			X2 = X1;
            t = ptSyntax;
        }
        else
        {
            while (!bad(++pos) && !(s[pos] == '*' && !bad(pos + 1) && s[pos + 1] == ')'));
            if (bad(pos))
            {
                if (!bad(X1 + 2) && s[X1 + 2] == '$')
                    t = ptInvalid;
                else t = ptComment3;
                X2 = pos - 1;
            }
            else
            {
                if (s[X1 + 2] == '$')
                {
                    t = (!bad(X1 + 3) && ISSPACE(s[X1 + 3]))? ptInvalid : ptPreprocessor;
				}
                else t = ptComment2;
                X2 = pos + 1;
                pos += 2;
            }
        }
    }
    else if (ISDIGIT(CurChar) || CurChar == '$')
    {
        char c = s[pos++];

        if (bad(pos))
        {
            X2 = X1;
            if (c == '$')
                t = ptInvalid;
            else t = ptDecimal;
        }
		else
        {
            if (c == '$')
            {
                if (!ISXDIGIT(s[pos]))
                {
                    X2 = X1;
                    t = ptInvalid;
                    return 0;
                }

                bool LZ = s[pos] == '0';
                int count = LZ? 0 : 1;

                while (!bad(++pos) && ISXDIGIT(s[pos]) && count < 16)
                {
                    if (LZ && s[pos] != '0')
                        LZ = false;
					if (!LZ)
                        count++;
                }

                X2 = pos - 1;
                t = ptHex;
                return 0;
            }

            while (ISDIGIT(s[pos]) && !bad(++pos));

            if (bad(pos) || (!((s[pos] == '.' && !(!bad(pos + 1) && s[pos + 1] == '.')) || _TOUPPER(s[pos]) == 'E')))
            {
                X2 = pos - 1;
                t = ptDecimal;
                return 0;
            }

			bool error = false;
            if (_TOUPPER(s[pos]) == 'E')
                pos++, error = true;
            else
            {
                while (!bad(++pos) && ISDIGIT(s[pos]));
                if (!bad(pos) && _TOUPPER(s[pos]) == 'E')
                    pos++, error = true;
            }
            if (error)
            {
                if (!bad(pos) && (s[pos] == '-' || s[pos] == '+'))
                    pos++;
                while (!bad(pos) && ISDIGIT(s[pos]))
                    pos++, error = false;
            }

            X2 = pos - 1;
			t = ptFloat;
            if (error)
                t = ptInvalid;

        }
    }
    else
    {
        pos++;
        X2 = X1;
        t = ptInvalid;
    }
    return 0;
}
#endif

#ifdef HAS_CMINUSMINUS_PARSER
int CToken::GetNextPair_CMinus(int &X1, int &X2, int &X3, int &t)
{
    char const *s = _s.c_str();

    X1 = X2 = X3 = pos;
    t = cmintInvalid;

    if (!L || bad(pos))
        return 1;

    int dummy = SkipWS();
    X1 = X2 = X3 = pos;

    if (dummy)
        return 1;

    char CurChar = s[pos];

    if (!ISPRINTABLEASCII(CurChar))
	{
        while (!bad(++pos) && !ISPRINTABLEASCII(s[pos]) && !ISSPACE(s[pos]));
        X2 = pos - 1;
        t = cmintInvalid;
        return 0;
    }

    if (_ISALPHA(CurChar) || CurChar == '_')
    {
        while (!bad(++pos) && (_ISALNUM(s[pos]) || s[pos] == '_'));
        X3 = X2 = pos - 1;
        t = cmintSymbol;
        /*todo: cmintJumpLabel*/
    }
    else if (CurChar == '?') /*todo: changed only '#', too lazy ;-)*/
    {
        if (!bad(pos - 1) && s[pos - 1] == '?')
        {
			X2 = X1;
            pos++;
            t = cmintInvalid;
            return 0;
        }

        bool error = false;
        /*
        int i = pos;
        while (!bad(--i))
        {
            if (!ISSPACE(s[i]))
            {
                error = true;
                break;
            }
        }
        */

        pos++;
        if (error || SkipWS() || !(_ISALPHA(s[pos]) || s[pos] == '_'))
        {
            if (!bad(pos) && s[pos] == '?' && s[pos - 1] == '?' && !bad(pos + 1) && s[pos + 1] != '?')
            {
                X3 = X2 = X1 + 1;
                pos++;
                t = cmintPreprocessor;
                return 0;
            }
            X2 = X1;
            t = (error && !bad(pos) && s[pos] != '?') ? cmintPreprocessor : cmintInvalid;
        }
        else
        {
            X3 = pos;
            while (!bad(++pos) && (_ISALNUM(s[pos]) || s[pos] == '_'));
			X2 = pos - 1;
            t = cmintPreprocessor;
        }
    }
    else if (SyntaxTable_CMinus[(_BYTE)CurChar])
    {
        if (__SyntaxTable_CMinus[(_BYTE)s[pos]])
        {
            while (!bad(++pos) && __SyntaxTable_CMinus[(_BYTE)s[pos]]);
            X2 = pos - 1;
        }
        else X2 = pos++;
        t = cmintSyntax;
    }
    else if (CurChar == '/')
    {
        if (bad(++pos) || !(s[pos] == '/' || s[pos] == '*'))
        {
			if (!bad(pos) && __SyntaxTable_CMinus[(_BYTE)s[pos]])
            {
                while (__SyntaxTable_CMinus[(_BYTE)s[pos]] && !bad(++pos));
                X2 = pos - 1;
            }
            else X2 = X1;
            t = cmintSyntax;
        }
        else
        {
            if (s[pos] == '/')
            {
                X2 = L - 1;
                pos = L;
                t = cmintComment1;
            }
            else if (s[pos] == '*')
            {
				int i = pos + 1;

                do
                {
                    while (!bad(i) && s[i] != '*')
                        i++;
                    if (bad(i))
                        break;
                    if (!bad(++i) && s[i] == '/')
                    {
                        X2 = i;
                        pos = i + 1;
                        t = cmintComment2;
                        return 0;
                    }
                } while (!bad(i));

                X2 = L - 1;
				pos = L;
                t = cmintComment3;

            }
        }
    }
    else if (CurChar == '*')
    {
        if (bad(++pos) || s[pos] != '/')
        {
            if (!bad(pos) && (__SyntaxTable_CMinus[(_BYTE)s[pos]] || s[pos] == '*'))
            {
                while ((__SyntaxTable_CMinus[(_BYTE)s[pos]]
                    || (s[pos] == '*' && !(!bad(pos + 1) && s[pos + 1] == '/'))) && !bad(++pos));
                X2 = pos - 1;
            }
            else X2 = X1;
            t = cmintSyntax;
		}
        else
        {
            if (!bad(pos + 1) && (s[pos + 1] == '*' || s[pos + 1] == '/')) //todo:
            {
                X2 = pos - 1;
                t = cmintSyntax;
            }
            else
            {
                X1 = 0;
                X2 = pos++;
                t = cmintComment4;
            }
        }
    }
    else if (CurChar == '\'')
    {
		int i = pos;

        while (!bad(++i) && s[i] != '\'')
        {
            if (s[i] == '\\' && !bad(i + 1))
            {
                i++;
            }
        }

        if (!bad(i))
        {
            X2 = i;
            pos = i + 1;
            t = cmintChar;
        }
        else
        {
			X2 = X1;
            pos++;
            t = cmintInvalid;
        }

    }
    else if (CurChar == '"')
    {
        int i = pos;

        while (!bad(++i) && s[i] != '"')
        {
            if (s[i] == '\\' && !bad(i + 1))
            {
                i++;
            }
        }

		X2 = bad(i) ? (L - 1) : i;
        pos = X2 + 1;
        t = cmintString;

    }
    else if (ISDIGIT(CurChar))
    {
        char c = s[pos++];

        if (bad(pos))
        {
            X2 = X1;
            t = cmintDecimal;
        }
        else
        {

            #define PARSE_NUMBER(PREFIX, ISXXXDIGIT, MAX_COUNT, NUM_TYPE, INV_TYPE) \
				if (c == '0' && _TOUPPER(s[pos]) == (PREFIX)) \
                { \
                    if (bad(++pos) || !ISXXXDIGIT(s[pos])) \
                    { \
                        X2 = X1 + 1; \
                        t = INV_TYPE; \
                        return 0; \
                    } \
                    bool LZ = s[pos] == '0'; \
                    int count = LZ? 0 : 1; \
                    while (!bad(++pos) && ISXXXDIGIT(s[pos]) && count < (MAX_COUNT)) \
                    { \
                        if (LZ && s[pos] != '0') \
                            LZ = false; \
                        if (!LZ) \
                            count++; \
                    } \
                    X2 = pos - 1; \
					t = NUM_TYPE; \
                    return 0; \
                }

            PARSE_NUMBER('X', ISXDIGIT, 16, cmintHex, cmintInvalid)
            PARSE_NUMBER('O', ISODIGIT, 22, cmintOctal, cmintInvalid)
            PARSE_NUMBER('B', ISBDIGIT, 64, cmintBinary, cmintInvalid)

            while (ISDIGIT(s[pos]) && !bad(++pos));

            X2 = pos - 1;
            t = cmintDecimal;

        }
    }
    else
    {
        pos++;
		X2 = X1;
        t = cmintInvalid;
    }
    return 0;
}
#endif

#ifdef HAS_NASM_PARSER
int CToken::GetNextPair_Nasm(int &X1, int &X2, int &X3, int &t)
{
    char const *s = _s.c_str();

    X1 = X2 = X3 = pos;
    t = nasmtInvalid;

    if (!L || bad(pos))
        return 1;

	int dummy = SkipWS();
    X1 = X2 = X3 = pos;

    if (dummy)
        return 1;

    char CurChar = s[pos];

    if (!ISPRINTABLEASCII(CurChar))
    {
        while (!bad(++pos) && !ISPRINTABLEASCII(s[pos]) && !ISSPACE(s[pos]));
        X2 = pos - 1;
        t = nasmtInvalid;
        return 0;
    }

    if (_ISALPHA(CurChar) || CurChar == '_' || CurChar == '?') /*"$." see below*/
    {
		while (!bad(++pos) && (_ISALNUM(s[pos]) || strchr("._?$#@~", (_BYTE)s[pos])));
        X2 = pos - 1;
        t = nasmtSymbol;
    }
    else if (CurChar == '%')
    {
        bool local = false;
        if (!bad(pos + 1) && s[pos + 1] == '%')
        {
            local = true;
            pos++;
        }
        else if (!bad(pos + 1) && s[pos + 1] == '$')
        {
            local = true;
            while (!bad(++pos) && s[pos] == '$');
            pos--;
        }
		if (local)
        {
            if (!bad(++pos) && (_ISALPHA(s[pos]) || strchr("._?$", (_BYTE)s[pos])))
            {
                while (!bad(++pos) && (_ISALNUM(s[pos]) || strchr("._?$#@~", (_BYTE)s[pos])));
                X2 = pos - 1;
                t = nasmtLabel;
            }
            else
            {
                X2 = pos - 1;
                t = nasmtInvalid;
            }
        }
        else
        {
            if (!bad(++pos) && (_ISALPHA(s[pos]) || s[pos] == '_'))
            {
				while (!bad(++pos) && (_ISALNUM(s[pos]) || s[pos] == '_'));
                X2 = pos - 1;
                t = X1 == 0 && X2 > X1? nasmtPreprocessor : nasmtInvalid;
            }
            else
            {
                X2 = pos - 1;
                t = nasmtSyntax;
            }
        }
    }
    else if (SyntaxTable_Nasm[(_BYTE)CurChar])
    {
        if (__SyntaxTable_Nasm[(_BYTE)s[pos]])
        {
            while (!bad(++pos) && __SyntaxTable_Nasm[(_BYTE)s[pos]]);
            X2 = pos - 1;
        }
		else X2 = pos++;
        t = nasmtSyntax;
    }
    else if (CurChar == '\'' || CurChar == '"')
    {
        while (!bad(++pos) && s[pos] != CurChar);
        X2 = bad(pos)? pos - 1 : pos++;
        t = nasmtString;
    }
    else if (CurChar == ';')
    {
        X2 = L - 1;
        pos = L;
        t = nasmtComment;
    }
    else if (CurChar == '$')
    {
        if (bad(++pos))
		{
            X2 = X1;
            t = nasmtSyntax;
            return 0;
        }
        if (ISDIGIT(s[pos]))
        {
            while (!bad(++pos) && ISXDIGIT(s[pos]));
            X2 = pos - 1;
            t = nasmtHex;
        }
        else
        {
            while ((_ISALNUM(s[pos]) || strchr("._?$#@~", (_BYTE)s[pos])) && !bad(++pos));
            X2 = pos - 1;
            if (X2 == X1 || (X2 == X1 + 1 && s[X2] == '$'))
                t = nasmtSyntax;
            else t = nasmtSymbol;
		}
    }
    else if (CurChar == '.')
    {
        while (!bad(++pos) && (_ISALNUM(s[pos]) || strchr("._?$#@~", (_BYTE)s[pos])));
        X2 = pos - 1;
        bool label = false;
/*--NOT SURE--------------------------------------------------------------------------------------*/
        if (X2 > X1 + 2)
        {
            if (s[X1 + 1] == '.' && s[X1 + 2] == '@')
                label = true;
        }
/*------------------------------------------------------------------------------------------------*/
        t = label? nasmtLabel : nasmtSymbol;
    }
    else if (ISDIGIT(CurChar))
    {
		char c = s[pos++];

        if (bad(pos))
        {
            X2 = X1;
            t = nasmtDecimal;
        }
        else
        {

            PARSE_NUMBER('X', ISXDIGIT, 16, nasmtHex, nasmtInvalid)

            bool Binary = ISBDIGIT(c)? true : false;
            bool Octal = ISODIGIT(c)? true : false;
            bool Decimal = true;

            bool StartZero = c == '0';
            bool LZ = StartZero;
			int Count = LZ? 0 : 1;

            for (pos--; !bad(++pos) && ISXDIGIT(s[pos]);)
            {
                /*if (!ISBDIGIT(s[pos])) Binary = false;*/
                if (!ISODIGIT(s[pos]))
                    Octal = false;
                if (!ISDIGIT(s[pos]))
                    Decimal = false;
                if (s[pos] != '0')
                    LZ = false;
                if (!LZ)
                    Count++;
            }

            if (bad(pos) || !strchr("HQhq.", (_BYTE)s[pos]))
            {
                X2 = pos - 1;
				if (_TOUPPER(s[X2]) == 'B')
                {
                    Binary = true;
                    for (int i = X1; i < X2; ++i)
                    {
                        if (!ISBDIGIT(s[i]))
                        {
                            Binary = false;
                            break;
                        }
                    }
                    t = Binary? nasmtBinary : nasmtInvalid;
                }
                else
                {
                    t = Decimal? nasmtDecimal : nasmtInvalid;
                }
                return 0;
			}

            char Suffix = _TOUPPER(s[pos]);

            X2 = pos++;

            switch (Suffix)
            {
                case 'H':
                    t = nasmtHex;
                    return 0;
                case 'Q':
                    t = Octal? nasmtOctal : nasmtInvalid;
                    return 0;
            }

            if (bad(pos) || !Decimal)
            {
				t = Decimal? nasmtFloat : nasmtInvalid;
                return 0;
            }

            while (ISDIGIT(s[pos]) && !bad(++pos));

            if (bad(pos) || _TOUPPER(s[pos]) != 'E')
            {
                t = nasmtFloat;
                X2 = pos - 1;
                return 0;
            }

            bool error = true;
            if (!bad(++pos) && (s[pos] == '-' || s[pos] == '+'))
                pos++;
            while (!bad(pos) && ISDIGIT(s[pos]))
                pos++, error = false;

            X2 = pos - 1;
            t = error? nasmtInvalid : nasmtFloat;

        }
    }
    else
    {
        pos++;
        X2 = X1;
        t = nasmtInvalid;
    }
    return 0;
}
#endif

#ifdef HAS_HASKELL_PARSER
int CToken::GetNextPair_Haskell(int &X1, int &X2, int &X3, int &t)
{
    char const *s = _s.c_str();

    X1 = X2 = X3 = pos;
    t = hasktInvalid;

    if (!L || bad(pos))
        return 1;

    int dummy = SkipWS();
    X1 = X2 = X3 = pos;

    if (dummy)
        return 1;

    char CurChar = s[pos];

    if (!ISPRINTABLEASCII(CurChar))
	{
        while (!bad(++pos) && !ISPRINTABLEASCII(s[pos]) && !ISSPACE(s[pos]));
        X2 = pos - 1;
        t = hasktInvalid;
        return 0;
    }

    if (_ISALPHA(CurChar) || CurChar == '_')
    {
        while (!bad(++pos) && (_ISALNUM(s[pos]) || s[pos] == '_'));
        X3 = X2 = pos - 1;
        t = hasktSymbol;
    }
    else if (SyntaxTable_Haskell[(_BYTE)CurChar])
    {
        if (__SyntaxTable_Haskell[(_BYTE)s[pos]])
        {
            while (!bad(++pos) && __SyntaxTable_Haskell[(_BYTE)s[pos]]);
			X2 = pos - 1;
        }
        else X2 = pos++;
        t = hasktSyntax;
    }
    else if (CurChar == '#') /*todo: from c parser, but there is no #ifdef etc. in Haskell98 draft ;-(*/
    {
        if (!bad(pos - 1) && s[pos - 1] == '#')
        {
            X2 = X1;
            pos++;
            t = hasktInvalid;
            return 0;
        }

        int i = pos;
        bool error = false;
        while (!bad(--i))
		{
            if (!ISSPACE(s[i]))
            {
                error = true;
                break;
            }
        }

        pos++;
        if (error || SkipWS() || !(_ISALPHA(s[pos]) || s[pos] == '_'))
        {
            if (!bad(pos) && s[pos] == '#' && s[pos - 1] == '#' && !bad(pos + 1) && s[pos + 1] != '#')
            {
                X3 = X2 = X1 + 1;
                pos++;
                t = hasktPreprocessor;
                return 0;
            }
			X2 = X1;
            t = (error && !bad(pos) && s[pos] != '#') ? hasktPreprocessor : hasktInvalid;
        }
        else
        {
            X3 = pos;
            while (!bad(++pos) && (_ISALNUM(s[pos]) || s[pos] == '_'));
            X2 = pos - 1;
            t = hasktPreprocessor;
        }
    }
    else if (CurChar == '\'')
    {
        int i = pos;

        while (!bad(++i) && s[i] != '\'')
        {
            if (s[i] == '\\' && !bad(i + 1))
			{
                i++;
            }
        }

        if (!bad(i))
        {
            X2 = i;
            pos = i + 1;
            t = hasktChar;
        }
        else
        {
            X2 = X1;
            pos++;
            t = hasktInvalid;
        }

	}
    else if (CurChar == '"')
    {
        int i = pos;

        while (!bad(++i) && s[i] != '"')
        {
            if (s[i] == '\\' && !bad(i + 1))
            {
                i++;
            }
        }

        X2 = bad(i) ? (L - 1) : i;
        pos = X2 + 1;
        t = hasktString;

    }
	else if (CurChar == '{')
    {
        if (bad(++pos) || s[pos] != '-')
        {
            X2 = X1;
            t = hasktSyntax;
        }
        else
        {
            while (!bad(++pos))
            {
                if (s[pos] == '-' && !bad(pos + 1) && s[pos + 1] == '}')
                    break;
            }
            if (!bad(pos++))
            {
                X2 = pos++;
                t = hasktComment2;
				if (X2 >= X1 + 5 && s[X1 + 2] == '#' && s[X2 - 2] == '#')
                    t = hasktPreprocessor;
            }
            else
            {
                X2 = L - 1;
                pos = L;
                t = hasktComment3;
                if (X2 >= X1 + 2 && s[X1 + 2] == '#')
                    t = hasktInvalid;
            }
        }
    }
    else if (CurChar == '-')
    {
        if (bad(++pos) || !(s[pos] == '-' || s[pos] == '}'))
        {
            X2 = X1;
			t = hasktSyntax;
            return 0;
        }
        if (s[pos] == '-')
        {
            X2 = L - 1;
            pos = L;
            t = hasktComment1;
        }
        else
        {
            X2 = pos++;
            X1 = 0;
            t = hasktComment4;
        }
    }
    else if (ISDIGIT(CurChar))
    {
		char c = s[pos++];

        if (bad(pos))
        {
            X2 = X1;
            t = hasktDecimal;
        }
        else
        {
            PARSE_NUMBER('X', ISXDIGIT, 16, hasktHex, hasktInvalid)
            PARSE_NUMBER('O', ISODIGIT, 22, hasktOctal, hasktInvalid)

            while (ISDIGIT(s[pos]) && !bad(++pos));

            if (bad(pos) || !(s[pos] == '.' && !(!bad(pos + 1) && s[pos + 1] == '.')))
            {
                X2 = pos - 1;
                t = hasktDecimal;
				return 0;
            }

            bool error = false,
                error2 = true;
            while (!bad(++pos) && ISDIGIT(s[pos]))
                error2 = false;
            if (!bad(pos) && _TOUPPER(s[pos]) == 'E')
                pos++, error = true;
            if (error)
            {
                if (!bad(pos) && (s[pos] == '-' || s[pos] == '+'))
                    pos++;
                while (!bad(pos) && ISDIGIT(s[pos]))
                    pos++, error = false;
            }

            X2 = pos - 1;
			t = error || error2? hasktInvalid : hasktFloat;

        }
    }
    else
    {
        pos++;
        X2 = X1;
        t = hasktInvalid;
    }
    return 0;
}
#endif
