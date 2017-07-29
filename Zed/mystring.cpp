/*
** stringp.cxx
**
** Defines class 'String' externs (Pascal style).
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#include "stringp.h"

String::String(char const *s, int w) :
    _S(NULL),
    W(-1)
{
    if (!s)
        return;
    int L = w==-1? (int)strlen(s) : w;
    _S = Allocate(L + 1);
    if (!L)
        *_S = '\0';
    else memcpy(_S, s, L + 1);
    W = L;
}

String::String(char const *s, int pos, int n, int w) :
    _S(NULL),
    W(-1)
{
    if (!s)
        return;
    if (pos < 0)
        eBadIndex();
    if (n < 0)
        eBadParam();
    int L = w==-1? (int)strlen(s) : w;
    if (pos > L)
        eBadIndex();
    int count = MIN(L - pos, n);
    _S = Allocate(count + 1);
    if (!count)
        *_S = '\0';
    else
    {
        memcpy(_S, s + pos, count);
        _S[count] = '\0';
    }
    W = count;
}

String::String(String const &s) :
    _S(NULL),
    W(-1)
{
    if (!s)
        return;
    char *t = (char *) s.getptr();
    int L = s.getwidth();
    _S = Allocate(L + 1);
    if (!L)
        *_S = '\0';
    else memcpy(_S, t, L + 1);
    W = L;
}

String::String(char C, int __rep) :
    _S(NULL),
    W(-1)
{
    if (__rep < 0)
        return;
    _S = Allocate(__rep + 1);
    if (__rep)
    {
        if (__rep == 1)
            *_S = C;
        else memset(_S, C, __rep);
    }
    _S[__rep] = '\0';
    W = __rep;
}

String& String::copy(char const *s, int w)
{
    destroy();
    if (s)
    {
        int L = w==-1? (int)strlen(s) : w;
        _S = Allocate(L + 1);
        if (!L)
            *_S = '\0';
        else memcpy(_S, s, L + 1);
        W = L;
    }
    return *this;
}

String& String::assign(char C, int __rep)
{
    if (__rep < 0)
        return *this;
    reAllocate(_S, __rep + 1);
    if (__rep == 1)
        *_S = C;
    else memset(_S, C, __rep);
    _S[__rep] = '\0';
    W = __rep;
    return *this;
}

String& String::append(char const *s, int w)
{
    if (!s)
        return *this;
    if (__empty())
        return copy(s);
    if (!*s)
        return *this;
    int L1 = W, L2 = w==-1? (int)strlen(s) : w;
    char *t = Allocate(L1 + L2 + 1);
    memcpy(t, _S, L1);
    memcpy(t + L1, s, L2 + 1);
    return assign(t, L1 + L2);
}

String& String::append(char C)
{
    if (__empty())
        return assign(C);
    else
    {
        int L = W;
        char *t = Allocate(L + 2);
        memcpy(t, _S, L);
        t[L] = C, t[L + 1] = '\0';
        return assign(t, L + 1);
    }
}

String& String::upcase()
{
    checkvalid();
    if (empty())
        return *this;
    if (!_S[1])
        *_S = (char) TOUPPER(*_S);
    else STRUPR(_S);
    return *this;
}

String& String::downcase()
{
    checkvalid();
    if (empty())
        return *this;
    if (!_S[1])
        *_S = (char) TOLOWER(*_S);
    else STRLWR(_S);
    return *this;
}

String& String::capitalize()
{
    checkvalid();
    if (empty())
        return *this;
    char *t = _S;
    if (!t[1])
        *t = (char) TOLOWER(*t);
    else STRLWR(t);
    do
    {
        while (*t && !ISALPHA(*t))
            t++;
        if (*t)
            *t = (char) TOUPPER(*t);
        while (*t && ISALPHA(*t))
            t++;
    } while (*t);
    return *this;
}

String& String::reverse()
{
    checkvalid();
    if (empty())
        return *this;
    int L = W;
    if (L < 2)
        return *this;
    int M = L >> 1;
    for (int i = 0, j = L - 1; i < M; ++i, --j) /*strrev(_S);*/
        SWAP(_S[i], _S[j]);
    return *this;
}

String& String::remove(int pos, int n) // todo: if n < 0 then count rl else lr
{
    checkvalid();
    if (pos < 0)
        eBadIndex();
    /*
    if (n < 0)
        eBadParam();
        */
    if (n < 1 || !*_S)
        return *this;
    int L = W;
    int M = L - 1;
    /*
    if (pos > M)
        eBadIndex();
        */
    if (pos > M)
        return *this;
    int count = MIN(L - pos, n);
    char *t = Allocate(L - count + 1);
    if (pos)
        memcpy(t, _S, pos);
    memcpy(t + pos, _S + pos + count, (L + 1) - (pos + count));
    return assign(t, L - count);
}

String& String::insert(int pos, char const *x, int w)
{
    if (pos > 0)
        checkvalid();
    if (pos < 0)
        eBadIndex();
    if (!x || (!*x && valid()))
        return *this;
    if (invalid())
    {
        copy(x);
        return *this;
    }
    int L1 = W;
    if (pos > L1)
        eBadIndex();
    int L2 = w==-1? (int)strlen(x) : w;
    char *t = Allocate(L1 + L2 + 1);
    if (pos)
        memcpy(t, _S, pos);
    memcpy(t + pos, x, L2);
    memcpy(t + pos + L2, _S + pos, L1 - pos + 1);
    return assign(t, L1 + L2);
}

String& String::insert(int pos, char C)
{
    if (pos > 0)
        checkvalid();
    if (pos < 0)
        eBadIndex();
    if (invalid())
    {
        assign(C);
        return *this;
    }
    int L = W;
    if (pos > L)
        eBadIndex();
    char *t = Allocate(L + 2);
    if (pos)
        memcpy(t, _S, pos);
    t[pos] = C;
    memcpy(t + pos + 1, _S + pos, L - pos + 1);
    return assign(t, L + 1);
}

String String::at(int pos, int n) const // todo: if n < 0 then count rl else lr
{
    checkvalid();
    if (pos < 0)
        eBadIndex();
    if (n < 0)
        eBadParam();
    if (empty())
    {
        if (!pos)
            return String(0);
        else eBadIndex();
    }
    int L = W;
    if (pos > L)
        eBadIndex();
    if (pos == L || n < 1)
        return String(0);
    int count = MIN(L - pos, n);
    char *t = Allocate(count + 1);
    memcpy(t, _S + pos, count);
    t[count] = '\0';
    String r;
    return r.assign(t, count);
}

int String::search(char C, int pos) const
{
    if (__empty())
        return -1;
    int L = W;
    if (pos > L - 1)
        return -1;
    char *delta = _S + pos;
    char *t = strchr(delta, (unsigned char)C);
    return (t == NULL)? (-1) : (t - _S);
}

int String::search(char const *x, int pos) const
{
    if (__empty() || !x || !*x)
        return -1;
    int L = W;
    if (pos > L - 1)
        return -1;
    char *delta = _S + pos;
    char *t = strstr(delta, x);
    return (t == NULL)? (-1) : (t - _S);
}

int String::rsearch(char C, int pos) const
{
    if (__empty())
        return -1;
    int L = W;
    if (pos > L - 1)
        pos = L - 1;
    for (int i = pos; i >= 0; --i)
    {
        if (_S[i] == C)
            return i;
    }
    return -1;
}

int String::rsearch(char const *x, int pos, int w) const
{
    if (__empty() || !x || !*x)
        return -1;
    int L = W;
    int L2 = w==-1? (int)strlen(x) : w;
    if (L2 > L)
        return -1;
    if (pos > L - 1)
        pos = L - 1;
    int start = MIN(pos, L - L2);
    int i = start, j, k;
    for (; i >= 0; --i)
    {
        int ok = 1;
        for (j = i, k = 0; k < L2; ++j, ++k)
        {
            if (_S[j] != x[k])
            {
                ok = 0;
                break;
            }
        }
        if (ok)
            return i;
    }
    return -1;
}

int String::freq(char const *x) const
{
    if (__empty() || !x || !*x)
        return 0;
    int pos = 0, n = 0;
    while ((pos = search(x, pos)) != -1)
        n++, pos++;
    return n;
}

int String::freq(char C) const
{
    if (__empty())
        return 0;
    int pos = 0, n = 0;
    while ((pos = search(C, pos)) != -1)
        n++, pos++;
    return n;
}

String& String::put(int pos, char const *x, int w)
{
    if (pos > 0)
        checkvalid();
    if (pos < 0)
        eBadIndex();
    if (!x || (!*x && valid()))
        return *this;
    if (invalid())
    {
        copy(x);
        return *this;
    }
    int L1 = W;
    if (pos > L1)
        eBadIndex();
    int L2 = x[1]? (w==-1? (int)strlen(x) : w) : 1;
    if (pos == L1)
        return append(x, L2);
    if (pos + L2 <= L1)
    {
        if (L2 == 1)
            _S[pos] = *x;
        else memcpy(_S + pos, x, L2);
        return *this;
    }
    char *t = Allocate(pos + L2 + 1);
    if (pos)
        memcpy(t, _S, pos);
    memcpy(t + pos, x, L2);
    t[pos + L2] = '\0';
    return assign(t, pos + L2);
}

String& String::put(int pos, char C)
{
    if (pos > 0)
        checkvalid();
    if (pos < 0)
        eBadIndex();
    if (invalid())
    {
        copy(C);
        return *this;
    }
    int L = W;
    if (pos > L)
        eBadIndex();
    if (pos == L)
        return append(C);
    _S[pos] = C;
    return *this;
}

String& String::removetrailingspace()//int *L_req
{
    if (__empty())
        return *this;
    int L = W;
    if ((unsigned char)_S[L - 1] > ' ')
        return *this;
    char *end = _S + L;
    int oldL = L;
    while (L > 0 && ISWHITESPACE(*--end))
        L--;
    if (L == oldL)
        return *this;
    char *t = Allocate(L + 1);
    if (L)
        memcpy(t, _S, L);
    t[L] = '\0';
    return assign(t, L);
}

String& String::removeall(char const *x, int w)
{
    if (__empty() || !x || !*x)
        return *this;
    int pos = 0;
    int L = w==-1? (int)strlen(x) : w;
    while ((pos = search(x, pos)) != -1)
        remove(pos, L);
    return *this;
}

String& String::removeall(char C)
{
    if (__empty())
        return *this;
    int pos = 0;
    while ((pos = search(C, pos)) != -1)
        remove(pos, 1);
    return *this;
}

String& String::replaceall(char C_Old, char C_New)
{
    if (__empty())
        return *this;
    int L = W;
    char *found = _S,
        *prev = _S;
    while (L > 0)
    {
        if ((found = (char*)memchr(found, (unsigned char)C_Old, L)) == NULL)
            break;
        *found++ = C_New;
        L -= found - prev;
        prev = found;
    }
    return *this;
}

String& String::replaceall(char const *S_Old, char const *S_New, int w, int w_)
{
    if (__empty() || !S_Old || !*S_Old || !S_New)
        return *this;
    int pos = 0;
    int L1 = w==-1? (int)strlen(S_Old) : w;
    int L2 = w_==-1? (int)strlen(S_New) : w_;
    while ((pos = search(S_Old, pos)) != -1)
    {
        remove(pos, L1);
        if (L2)
            insert(pos, S_New, L2);
        pos += L2;
    }
    return *this;
}
