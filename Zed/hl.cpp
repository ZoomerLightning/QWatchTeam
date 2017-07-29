#include <string.h>
#include <limits.h>

#include "hl.h"
#include "mystring.h"
#include "tok.h"
#include "video.h"
#include "reserved.h"
#include "zedclass.h"
#include "at.h"

//fix
#define SLOW

#ifndef SLOW
#include "myhash.h"
#endif

#define STATIC_INIT     50
#define STATIC_DELTA    10

#define DEFINE_COLLECT(NAME)    TStringCollection NAME(STATIC_INIT, STATIC_DELTA)

#ifdef SLOW
DEFINE_COLLECT(CRW);
DEFINE_COLLECT(COp);
DEFINE_COLLECT(CFun);
DEFINE_COLLECT(CDef);
DEFINE_COLLECT(CPreprocessor);
#else
HASH::myhashset CRW(crw, crw+crwcount, crwcount);
HASH::myhashset COp(cop, cop+copcount, copcount);
HASH::myhashset CFun(cfun, cfun+cfuncount, cfuncount);
HASH::myhashset CDef(cdef, cdef+cdefcount, cdefcount);
HASH::myhashset CPreprocessor(cpreprocessor, cpreprocessor+cppcount, cppcount);
#endif

#ifdef HAS_HASKELL_PARSER
DEFINE_COLLECT(HaskellDef);
DEFINE_COLLECT(HaskellPreprocessor);
DEFINE_COLLECT(HaskellRW);
DEFINE_COLLECT(HaskellSpecial);
#endif
#ifdef HAS_PASCAL_PARSER
DEFINE_COLLECT(PascalRW);
#endif
#ifdef HAS_CMINUSMINUS_PARSER
DEFINE_COLLECT(CMinusRW);
DEFINE_COLLECT(CMinusDef);
DEFINE_COLLECT(CMinusPreprocessor);
#endif
#ifdef HAS_NASM_PARSER
DEFINE_COLLECT(NasmRW);
DEFINE_COLLECT(NasmDef);
DEFINE_COLLECT(NasmPreprocessor);
DEFINE_COLLECT(NasmSpecial);
/*DEFINE_COLLECT(NasmReg);
DEFINE_COLLECT(NasmCmd);*/
#endif

#undef DEFINE_COLLECT

#if !(defined(NDEBUG) || defined(SLOW) || 1)
#include <cstdio>
#include <cstdlib>
#include <vector>
void hl_test_sub(HASH::myhashset const& H)
{
    char const *const delim = "---";

    printf("H.bucket_count()=%d\n", (int) H.bucket_count());
    printf("H.size()=%d\n", (int) H.size());
    printf("H.maxsize()=%d\n", (int) H.max_size());

    int bcount = H.bucket_count();

    puts(delim);
    for (int i = 0; i < bcount; i++)
        printf("bucket(%03d)-> %d\n", i, (int) H.elems_in_bucket(i));

    HASH::myhashset::const_iterator I = H.begin();
    int bmax = 0, non_zero_bcount = 0;
    stlport::vector<int> stat(8);
    stlport::fill(stat.begin(), stat.end(), 0);

    puts(delim);
    for (int b = 0; b < bcount && I != H.end(); b++)
    {
        int eb = (int) H.elems_in_bucket(b);
        if (eb == 0)
            continue;
        non_zero_bcount++;
        if (eb > bmax)
            bmax = eb;
        stat[eb]++;
        printf("bucket(%03d, %d) ", b, eb);
        for (int elem = 0; elem < eb; elem++)
        {
            printf("%s ", (*I++).c_str());
        }
        putchar('\n');
    }
    printf("bmax=%d\n", bmax);

    stlport::vector<double> p(5);
    for (int i = 1; i <= 5; i++)
        p[i-1] = (double) stat[i] / non_zero_bcount;
    printf("percents are  ");
    for (int i = 0; i < 5; i++)
        printf("(%d) %02.1f%%  ", i+1, p[i]*100);
    putchar('\n');
}

void hl_test()
{
#define PRINT(ARRAY) \
    printf("\nARRAY=%s\n\n", #ARRAY); \
    hl_test_sub(ARRAY);

    PRINT(CRW);
    PRINT(CFun);
    PRINT(CDef);
    PRINT(CPreprocessor);
#undef PRINT

    exit(0);
}
#endif

inline int tolocal(int loaf, int offset, xx &t)
{
    int start = offset;
    int end = offset + loaf - 1;
    if (loaf < 1 || t.x2 < start || t.x1 > end)
        return 0;
    if (t.x1 < start)
        t.x1 = start;
    if (t.x2 > end)
        t.x2 = end;
    t.x1 -= start;
    t.x2 -= start;
    return 1;
}

void highlight(char const *s, int L, char *buf, int buflen, int loaf, int offset, int ft)
{
    if (!Config.highlight || !s || !loaf || !*s)
        return;
    static char temp[__zed_maxlinelength + 256];
#ifndef SLOW
    int const MAX_TOKEN_LEN = 64;
    static std::string tempstr('*', MAX_TOKEN_LEN);
    static std::string sharp1("#", 1);
    static std::string sharp2("##", 2);
#endif

    static bool inited = false;

    if (!inited)
    {
        int i;

#ifdef SLOW
        for (i = 0; i < crwcount; ++i) CRW.insert(crw[i]);
        for (i = 0; i < copcount; ++i) COp.insert(cop[i]);
        for (i = 0; i < cfuncount; ++i) CFun.insert(cfun[i]);
        for (i = 0; i < cdefcount; ++i) CDef.insert(cdef[i]);
        for (i = 0; i < cppcount; ++i) CPreprocessor.insert(cpreprocessor[i]);
#endif

    #ifdef HAS_HASKELL_PARSER
        for (i = 0; i < haskelldefcount; ++i) HaskellDef.insert(haskelldef[i]);
        for (i = 0; i < haskellppcount; ++i) HaskellPreprocessor.insert(haskellpreprocessor[i]);
        for (i = 0; i < haskellrwcount; ++i) HaskellRW.insert(haskellrw[i]);
        for (i = 0; i < haskellspcount; ++i) HaskellSpecial.insert(haskellspecial[i]);
    #endif
    #ifdef HAS_PASCAL_PARSER
        for (i = 0; i < prwcount; ++i) PascalRW.insert(prw[i]);
    #endif
    #ifdef HAS_CMINUSMINUS_PARSER
        for (i = 0; i < cmmrwcount; ++i) CMinusRW.insert(cmmrw[i]);
        for (i = 0; i < cmmdefcount; ++i) CMinusDef.insert(cmmdef[i]);
        for (i = 0; i < cmmppcount; ++i) CMinusPreprocessor.insert(cmmpreprocessor[i]);
    #endif
    #ifdef HAS_NASM_PARSER
        for (i = 0; i < nasmrwcount; ++i) NasmRW.insert(nasmrw[i]);
        for (i = 0; i < nasmdefcount; ++i) NasmDef.insert(nasmdef[i]);
        for (i = 0; i < nasmppcount; ++i) NasmPreprocessor.insert(nasmpreprocessor[i]);
        for (i = 0; i < nasmspecialcount; ++i) NasmSpecial.insert(nasmspecial[i]);
    #endif
        inited = true;
    }

    if (ft == ftC
        || ft == ftIDC /*todo: ;-)*/
        )
    {
        CToken token(s, L);
        int tt;

        int x1, x2, x3;

        while (token.GetNextPair(x1, x2, x3, tt) == 0)
        {
            BYTE A = atNormalText;

            switch (tt)
            {
                case ctInvalid:
                    A = atInvalid;
                    break;
                case ctOctal:
                    A = atOctal;
                    break;
                case ctHex:
                    A = atHex;
                    break;
                case ctDecimal:
                    A = atDecimal;
                    break;
                case ctFloat:
                    A = atFloat;
                    break;
                case ctBinary: case ctRoman: /*todo: there is no binary or roman in c++ :-)*/
                    A = atBinary;
                    break;
                case ctChar:
                    A = atChar;
                    break;
                case ctString:
                    A = atString;
                    break;
                case ctSymbol:
                    {
#ifdef SLOW
                        int toklen = x2 - x1 + 1;
                        memcpy(temp, s + x1, toklen);
                        temp[toklen] = 0;
                        ccIndex index;
                        if (CRW.search(temp, index) == True)
                            A = atKeyWord;
                        else if (CFun.search(temp, index) == True)
                            A = atFunction;
                        else if (CDef.search(temp, index) == True)
                            A = atPreprocess;
                        else if (COp.search(temp, index) == True)
                            A = atSyntax;
#else
                        int l = x2-x1+1;
                        if (l > 1 && l <= MAX_TOKEN_LEN)
                        {
                            tempstr.assign(s+x1, l);
                            if (CRW.find(tempstr) != CRW.end())
                                A = atKeyWord;
                            else if (CFun.find(tempstr) != CFun.end())
                                A = atFunction;
                            else if (CDef.find(tempstr) != CDef.end())
                                A = atPreprocess;
                            else if (COp.find(tempstr) != COp.end())
                                A = atSyntax;
                        }
#endif
                    }
                    break;
                case ctPreprocessor:
                    {
#ifdef SLOW
                        String temp(s, x1, x2 - x1 + 1);
                        temp.removeall(' ');
                        temp.removeall('\t');
                        ccIndex index;
                        A = atPreprocessUnknown;
                        if (temp == "##"
                            || temp == "#"
                            || CPreprocessor.search(temp.getptr(), index) == True)
                            A = atPreprocess;
#else
                        int l = x2-x1+1;
                        char const *ptr = s+x1;
                        if (memchr(ptr, ' ', l) || memchr(ptr, '\t', l))
                        {
                            char *buf = new char[l];
                            int j = 0;
                            for (int i = 0; i < l; i++)
                            {
                                char c = ptr[i];
                                if (c != ' ' && c != '\t')
                                    buf[j++] = c;
                            }
                            tempstr.assign(buf, j);
                            delete[] buf;
                        }
                        else tempstr.assign(ptr, l);
                        A = atPreprocessUnknown;
                        if (CPreprocessor.find(tempstr) != CPreprocessor.end()
                            || tempstr == sharp2 || tempstr == sharp1)
                            A = atPreprocess;
#endif
                    }
                    break;
                case ctJumpLabel:/*todo:*/
                    break;
                case ctComment1: case ctComment2:
                    A = atComment;
                    break;
                case ctComment3: case ctComment4:
                    break;
                case ctSyntax:
                    A = atSyntax;
                    break;
                case ctStdinclude:
                    A = atStdinclude;
                    break;
            }

            xx t(x1, x2);
            if (tolocal(loaf, offset, t))
                changebufattr(buf, buflen, t.x1, t.x2, A);

        }
    }
    else if (ft == ftPascal)
    {
#ifdef HAS_PASCAL_PARSER
        CToken token(s, L);
        int tt;

        int x1, x2, x3;

        while (token.GetNextPair_Pascal(x1, x2, x3, tt) == 0)
        {
            BYTE A = atNormalText;

            switch (tt)
            {
                case ptInvalid:
                    A = atInvalid;
                    break;
                case ptHex:
                    A = atHex;
                    break;
                case ptDecimal:
                    A = atDecimal;
                    break;
                case ptFloat:
                    A = atFloat;
                    break;
                case ptChar:
                    A = atChar;
                    break;
                case ptString:
                    A = atString;
                    break;
                case ptSymbol:
                    {
                        int toklen = x2 - x1 + 1;
                        memcpy(temp, s + x1, toklen);
                        temp[toklen] = 0;
                        _STRLWR(temp);
                        ccIndex index;
                        if (PascalRW.search(temp, index) == True)
                            A = atKeyWord;
                    }
                    break;
                case ptPreprocessor:
                    A = atPreprocess;
                    break;
                case ptComment1: case ptComment2:
                    A = atComment;
                    break;
                case ptComment3: case ptComment4:
                    break;
                case ptSyntax:
                    A = atSyntax;
                    break;
            }

            xx t(x1, x2);
            if (tolocal(loaf, offset, t))
                changebufattr(buf, buflen, t.x1, t.x2, A);

        }
#endif
    }
    else if (ft == ftCmm)
    {
#ifdef HAS_CMINUSMINUS_PARSER
        CToken token(s, L);
        int tt;

        int x1, x2, x3;

        while (token.GetNextPair_CMinus(x1, x2, x3, tt) == 0)
        {
            BYTE A = atNormalText;

            switch (tt)
            {
                case cmintInvalid:
                    A = atInvalid;
                    break;
                case cmintOctal:
                    A = atOctal;
                    break;
                case cmintHex:
                    A = atHex;
                    break;
                case cmintDecimal:
                    A = atDecimal;
                    break;
                case cmintBinary:
                    A = atBinary;
                    break;
                case cmintChar:
                    A = atChar;
                    break;
                case cmintString:
                    A = atString;
                    break;
                case cmintSymbol:
                    {
                        int toklen = x2 - x1 + 1;
                        memcpy(temp, s + x1, toklen);
                        temp[toklen] = 0;
                        /*_STRLWR(temp);*/
                        ccIndex index;
                        if (CMinusRW.search(temp, index) == True)
                            A = atKeyWord;
                        else if (CMinusDef.search(temp, index) == True)
                            A = atPreprocess;
                    }
                    break;
                case cmintPreprocessor:
                    {
                        String temp(s, x1, x2 - x1 + 1);
                        temp.removeall(' ');
                        temp.removeall('\t');
                        ccIndex index;
                        A = atPreprocessUnknown;
                        if (CMinusPreprocessor.search(temp.c_str(), index) == True)
                            A = atPreprocess;
                    }
                    break;
                case cmintComment1: case cmintComment2:
                    A = atComment;
                    break;
                case cmintComment3: case cmintComment4:
                    break;
                case cmintSyntax:
                    A = atSyntax;
                    break;
            }

            xx t(x1, x2);
            if (tolocal(loaf, offset, t))
                changebufattr(buf, buflen, t.x1, t.x2, A);

        }
#endif
    }
    else if (ft == ftASM)
    {
#ifdef HAS_NASM_PARSER
        CToken token(s, L);
        int tt;

        int x1, x2, x3;

        while (token.GetNextPair_Nasm(x1, x2, x3, tt) == 0)
        {
            BYTE A = atNormalText;

            switch (tt)
            {
                case nasmtInvalid:
                    A = atInvalid;
                    break;
                case nasmtOctal:
                    A = atOctal;
                    break;
                case nasmtHex:
                    A = atHex;
                    break;
                case nasmtDecimal:
                    A = atDecimal;
                    break;
                case nasmtFloat:
                    A = atFloat;
                    break;
                case nasmtBinary:
                    A = atBinary;
                    break;
                case nasmtString:
                    A = atString;
                    break;
                case nasmtSymbol:
                    {
                        int toklen = x2 - x1 + 1;
                        memcpy(temp, s + x1, toklen);
                        temp[toklen] = 0;
                        ccIndex index;
                        if (NasmDef.search(temp, index) == True)
                            A = atPreprocess;
                        else if (NasmSpecial.search(temp, index) == True)
                            A = atSpecial;
                        else
                        {
                            _STRLWR(temp);
                            if (NasmRW.search(temp, index) == True)
                                A = atKeyWord;
                        }
                    }
                    break;
                case nasmtLabel:
                    A = atLabel;
                    break;
                case nasmtPreprocessor:
                    {
                        String temp(s, x1, x2 - x1 + 1);
                        /*temp.removeall(' ');
                        temp.removeall('\t');*/
                        temp.downcase();
                        ccIndex index;
                        A = atPreprocessUnknown;
                        if (NasmPreprocessor.search(temp.c_str(), index) == True)
                            A = atPreprocess;
                    }
                    break;
                case nasmtComment:
                    A = atComment;
                    break;
                case nasmtSyntax:
                    A = atSyntax;
                    break;
            }

            xx t(x1, x2);
            if (tolocal(loaf, offset, t))
                changebufattr(buf, buflen, t.x1, t.x2, A);

        }
#endif
    }
    else if (ft == ftHaskell)
    {
#ifdef HAS_HASKELL_PARSER
        CToken token(s, L);
        int tt;

        int x1, x2, x3;

        while (token.GetNextPair_Haskell(x1, x2, x3, tt) == 0)
        {
            BYTE A = atNormalText;

            switch (tt)
            {
                case hasktInvalid:
                    A = atInvalid;
                    break;
                case hasktOctal:
                    A = atOctal;
                    break;
                case hasktHex:
                    A = atHex;
                    break;
                case hasktDecimal:
                    A = atDecimal;
                    break;
                case hasktFloat:
                    A = atFloat;
                    break;
                case hasktChar:
                    A = atChar;
                    break;
                case hasktString:
                    A = atString;
                    break;
                case hasktSymbol:
                    {
                        int toklen = x2 - x1 + 1;
                        memcpy(temp, s + x1, toklen);
                        temp[toklen] = 0;
                        ccIndex index;
                        if (HaskellRW.search(temp, index) == True)
                            A = atKeyWord;
                        else if (HaskellSpecial.search(temp, index) == True)
                            A = atSpecial;
                        else if (HaskellDef.search(temp, index) == True)
                            A = atPreprocess;
                    }
                    break;
                case hasktPreprocessor:
                    {
                    #if 1
                        String temp(s, x1, x2 - x1 + 1);
                        temp.removeall(' ');
                        temp.removeall('\t');
                        ccIndex index;
                        A = atPreprocessUnknown;
                        if (temp[0] != '#'
                            || temp == "#"
                            || temp == "##"
                            || HaskellPreprocessor.search(temp.c_str(), index) == True)
                            A = atPreprocess;
                    #else
                        A = atPreprocess;
                    #endif
                    }
                    break;
                case hasktComment1: case hasktComment2:
                    A = atComment;
                    break;
                case hasktComment3: case hasktComment4:
                    break;
                case hasktSyntax:
                    A = atSyntax;
                    break;
            }

            xx t(x1, x2);
            if (tolocal(loaf, offset, t))
                changebufattr(buf, buflen, t.x1, t.x2, A);

        }
#endif
    }
    else if (ft == ftADA)
    {
    }
}
