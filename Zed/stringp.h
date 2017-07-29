/*
** stringp.h
**
** Defines class 'String' (Pascal style).
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __STRINGP_H__
#define __STRINGP_H__

#include <string.h>
#include <stdlib.h>
#include "strdup.h"

#if defined(__TURBOC__) || defined(STRP_NO_EXCEPTIONS)
#include <process.h>
#include <stdio.h>
#endif

#include "minmax.h"
#include "ascii.h"

class String;

static int const __npos = 0x7fffffff;

class String
{
protected:
    char    *_S;
    int     W;                          // (DJGPP) 0..2G and sizeof(string)==8

public:

    class StringException           {};

    class SEInvalidIndex            : public StringException {};
    class SEAccessToEmpty           : public StringException {};
    class SENegativeAlloc           : public StringException {};
    class SEBadParameter            : public StringException {};

    String() :
        _S(NULL),
        W(-1)
    {
    }
    String(char const *s, int w = -1);
    String(char const *s, int pos, int n, int w = -1);
    String(String const &s);
    String(char C, int __rep = 1);
    String(int __Size) :
        _S(NULL),
        W(-1)
    {
        if (__Size >= 0)
        {
            _S = Allocate(__Size + 1);
            *_S = '\0', W = __Size;
        }
    }

    /*virtual*/ ~String()
    {
        destroy();
    }
    void destroy()
    {
        deAllocate(_S);
        W = -1;
    }

    String& copy(char const *s, int w = -1);
    inline String& copy(String const &s);

    void store(char *dest) const
    {
        checkvalid();
        memcpy(dest, _S, W + 1);
    }
    char *storecontents() const
    {
        checkvalid();
        return mystrdup(_S, W);
    }
    void storecontents(char *&dest)
    {
        checkvalid();
        dest = mystrdup(_S, W);
    }

//  Use following only if you know what you're doing
    void justcopy(char const *s, int w = -1)
    {
        if (s && _S)
        {
            W = w==-1? (int)strlen(s) : w;
            memcpy(_S, s, W + 1);
        }
        else _S = NULL, W = -1;
    }

    String& assign(char C, int __rep = 1);

//  Use following only if you know what you're doing
    String& assign(char *s, int w = -1) //pass only ptrs allocated via new!!!
    {
        destroy();
        _S = s;
        W = s? (w==-1? (int)strlen(s) : w) : -1;
        return *this;
    }
    void assignOnly(char *s, int w = -1)
    {
        _S = s;
        W = s? (w==-1? (int)strlen(s) : w) : -1;
    }

    char *Allocate(int __Size) const
    {
        /*
        if (__Size < 0)
            eNAlloc();
            */
        char *t = __Size > 0? (new char[__Size]) : NULL;
        return t;
    }

    void deAllocate(char *&ptr) const
    {
        if (ptr)
        {
            delete[] ptr;
            ptr = NULL;
        }
    }

    char *reAllocate(char *&ptr, int __NewSize) const
    {
        deAllocate(ptr);
        return ptr = Allocate(__NewSize);
    }

#if defined(__TURBOC__) || defined(STRP_NO_EXCEPTIONS)
    static void error(int e)
    {
        fprintf(stderr, "class String error %d\n", e);
        exit(200 + e);
    }
    void eAccess2Empty() const  {error(1);}
    void eBadIndex() const      {error(2);}
    void eNAlloc() const        {error(3);}
    void eBadParam() const      {error(4);}
#else
    void eAccess2Empty() const  {throw SEAccessToEmpty();}
    void eBadIndex() const      {throw SEInvalidIndex();}
    void eNAlloc() const        {throw SENegativeAlloc();}
    void eBadParam() const      {throw SEBadParameter();}
#endif

    void checkvalid() const
    {
/*#if !defined(STRP_LITE)*/
        if (invalid())
            eAccess2Empty();
/*#endif*/
    }

    String const& operator= (char const *s)
    {
        return copy(s);
    }
    String const& operator= (String const &s)
    {
        return copy(s);
    }
    String const& operator= (char C)
    {
        return assign(C);
    }

    int operator! () const      {return invalid();}

    char const *c_str(int o=0) const
    {
        checkvalid();
        if (o && (o < 0 || o > W))
            eBadIndex();
        return getptr()+o;
    }
    char get(int index) const
    {
        checkvalid();
        if (index < 0 || index > W)
            eBadIndex();
        return _S[index];
    }
    char const& operator[] (int index) const
    {
        return _S[index];
    }
    char& operator[] (int index)
    {
        checkvalid();
        if (index < 0 || index > W)
            eBadIndex();
        return _S[index];
    }

/*    operator double() const
    {
        checkvalid();
        return strtod(_S, 0);
    }*/

    String& operator+= (char const *s);
    inline String& operator+= (String const &s);
    String& operator+= (char C);

    friend String operator+ (String const &s1, char const *s2);
    friend String operator+ (char const *s1, String const &s2);
    friend String operator+ (String const &s1, String const &s2);
    friend String operator+ (String const &s, char C);

    friend int operator== (char const *s1, String const &s2);
    friend int operator!= (char const *s1, String const &s2);
    friend int operator< (char const *s1, String const &s2);
    friend int operator<= (char const *s1, String const &s2);
    friend int operator> (char const *s1, String const &s2);
    friend int operator>= (char const *s1, String const &s2);

    int operator== (char const *s) const    {return strcmp(getptr(), s) == 0;}
    int operator!= (char const *s) const    {return strcmp(getptr(), s) != 0;}
    int operator< (char const *s) const     {return strcmp(getptr(), s) < 0;}
    int operator<= (char const *s) const    {return strcmp(getptr(), s) <= 0;}
    int operator> (char const *s) const     {return strcmp(getptr(), s) > 0;}
    int operator>= (char const *s) const    {return strcmp(getptr(), s) >= 0;}

    int operator== (String const &s) const
    {
        if (W != s.W)
            return 0;
        return memcmp(getptr(), s.getptr(), W) == 0;
    }
    int operator!= (String const &s) const
    {
        return memcmp(getptr(), s.getptr(), MIN(W, s.getwidth()) + 1) != 0;
    }
    int operator< (String const &s) const   {return memcmp(getptr(), s.getptr(), MIN(W, s.getwidth()) + 1) < 0;}
    int operator<= (String const &s) const  {return memcmp(getptr(), s.getptr(), MIN(W, s.getwidth()) + 1) <= 0;}
    int operator> (String const &s) const   {return memcmp(getptr(), s.getptr(), MIN(W, s.getwidth()) + 1) > 0;}
    int operator>= (String const &s) const  {return memcmp(getptr(), s.getptr(), MIN(W, s.getwidth()) + 1) >= 0;}

    String& append(char const *s, int w = -1);
    inline String& append(String const &s);
    String& append(char C);

    friend String& cat(String const& x, String const& y, String& z)//fast {z = x + y;}
    {
        z.copy(x);
        return z += y;
    }
    friend String& cat(String const& z, String const& y, String& x, String& X)//fast {x = z + y + x;}
    {
        x += z;
        return X += y;
    }
    String& prepend(char const *x, int w = -1)//fast {y = x + y;}
    {
        return insert(0, x, w);
    }
    String& prepend(String const &x)
    {
        return insert(0, x);
    }
    String& prepend(char C)
    {
        return insert(0, C);
    }

    String& remove(int pos, int n);
    String& removefromto(int start, int end)
    {
        return remove(start, end - start + 1);
    }
    friend String& remove(String &x, int pos, int n)
    {
        return x.remove(pos, n);
    }

    String& removeall(char const *x, int w = -1);
    String& removeall(String const &x);
    String& removeall(char C);

    String& replaceall(char const *S_Old, char const *S_New, int w = -1, int w_ = -1);
    String& replaceall(String const &S_Old, String const &S_New);
    String& replaceall(char C_Old, char C_New);

    String& removebefore(int pos);
    String& removeafter(int pos);
    inline String& removefrom(int pos);

    void clip(int L)
    {
        removefrom(L);
    }
    String& removetrailingspace();

    String& insert(int pos, char const *x, int w = -1);
    inline String& insert(int pos, String const &x);
    String& insert(int pos, char C);

    friend String& insert(String &y, int pos, char const *x, int w=-1)
    {
        return y.insert(pos, x, w);
    }
    friend String& insert(String &y, int pos, String const &x)
    {
        return y.insert(pos, x);
    }
    friend String& insert(String &y, int pos, char C)
    {
        return y.insert(pos, C);
    }

    String& put(int pos, char const *x, int w = -1);
    inline String& put(int pos, String const &x);
    String& put(int pos, char C);

    friend String& put(String &y, int pos, char const *x, int w = -1)   {return y.put(pos, x, w);}
    friend String& put(String &y, int pos, String const &x)             {return y.put(pos, x);}
    friend String& put(String &y, int pos, char C)                      {return y.put(pos, C);}

    String at(int pos, int n) const;
    String before(int pos) const;
    String after(int pos) const;
    String from(int pos) const;
    String fromto(int x1, int x2) const
    {
        if (x2 < x1)
            x2 = x1;
        return at(x1, x2 - x1 + 1);
    }

    String& reverse();
    String& upcase();
    String& downcase();
    String& capitalize();

    friend String reverse(String t)     {return String(t.reverse());}
    friend String upcase(String t)      {return String(t.upcase());}
    friend String downcase(String t)    {return String(t.downcase());}
    friend String capitalize(String t)  {return String(t.capitalize());}

//return 1 if target appears anywhere in String; else 0
    int contains(char C) const
    {
        checkvalid();
        return memchr(_S, (unsigned char)C, W) != NULL;
    }
    int contains(char const *t) const
    {
        checkvalid();
        return t? (strstr(_S, t) != NULL) : 0;
    }
    int contains(String const &y) const
    {
        return contains(y.__getptr());
    }

//  return position of target in String; -1 if target was not found
    int search(char const *x, int pos = 0) const;
    inline int search(String const &x, int pos = 0) const;
    int search(char C, int pos = 0) const;

    friend int search(String const &y, char const *x, int pos = 0)          {return y.search(x, pos);}
    friend int search(String const &y, String const &x, int pos = 0)        {return y.search(x, pos);}
    friend int search(String const &y, char C, int pos = 0)                 {return y.search(C, pos);}

//  reverse search
    int rsearch(char const *x, int pos = __npos, int w = -1) const;
    inline int rsearch(String const &x, int pos = __npos) const;
    int rsearch(char C, int pos = __npos) const;

    friend int rsearch(String const &y, char const *x, int pos = __npos, int w = -1)
    {
        return y.rsearch(x, pos, w);
    }
    friend int rsearch(String const &y, String const &x, int pos = __npos)
    {
        return y.rsearch(x, pos);
    }
    friend int rsearch(String const &y, char C, int pos = __npos)
    {
        return y.rsearch(C, pos);
    }

//  return number of occurences of target in String
    int freq(char const *x) const;
    inline int freq(String const &x) const;
    int freq(char C) const;

    friend int freq(String const &y, char const *x)                         {return y.freq(x);}
    friend int freq(String const &y, String const &x)                       {return y.freq(x);}
    friend int freq(String const &y, char C)                                {return y.freq(C);}

//Stream IO
#ifdef STRP_IOSTREAM
    friend ostream& operator<< (ostream &s, String const &x) const
    {
        return x.__empty() ? s : (s << x.getptr());
    }
    friend istream& operator>> (istream &s, String &x)
    {
        if (x.invalid())
            x.eAccess2Empty();
        return (s >> x._S);
    }
#endif

    char const *getptr() const
    {
        checkvalid();
        return _S;
    }
    char const *__getptr() const
    {
        return _S;
    }
    int getwidth() const
    {
        checkvalid();
        return W;
    }
    int __getwidth() const
    {
        return W;
    }

    int get_asciiz_length() const
    {
        checkvalid();
        return strlen(_S);
    }
    bool empty() const
    {
        checkvalid();
        return W==0;
    }
    bool asciiz_empty() const
    {
        checkvalid();
        return !*_S;
    }
    bool notempty() const                   {return !empty();}
    bool __empty() const                    {return invalid() || W==0;}
    friend bool valid(String const &s)      {return s.valid();}
    friend bool invalid(String const &s)    {return s.invalid();}
    bool valid() const                      {return _S != NULL;}
    bool invalid() const                    {return !valid();}
};

class _string : public String
{
public:

    void destroy()
    {
        _S = NULL, W = -1;
    }
    _string& assign(char *s, int w = -1)
    {
        _S = s;
        W = s? (w==-1? (int)strlen(s) : w) : -1;
        return *this;
    }

    _string() :
        String()
    {
        _S = NULL, W = -1;
    }
    _string(char *t, int w = -1) :
        String()
    {
        _S = t, W = w==-1? (t? (int)strlen(t) : -1) : w;
    }
    _string(_string const &t) :
        String()
    {
        _S = t._S, W = t.W;
    }
    /*virtual*/ ~_string()
    {
        /*destroy();*/
        _S = 0;
        // here goes previous dtor (~string)
    }
    String& operator= (char *s)
    {
        _S = s, W = s? (int)strlen(s) : -1;
        return *this;
    }
    String& operator= (_string const &s)
    {
        _S = s._S, W = s.W;
        return *this;
    }
};

inline int operator== (char const *s1, String const &s2)        {return strcmp(s1, s2.getptr()) == 0;}
inline int operator!= (char const *s1, String const &s2)        {return strcmp(s1, s2.getptr()) != 0;}
inline int operator< (char const *s1, String const &s2)         {return strcmp(s1, s2.getptr()) < 0;}
inline int operator<= (char const *s1, String const &s2)        {return strcmp(s1, s2.getptr()) <= 0;}
inline int operator> (char const *s1, String const &s2)         {return strcmp(s1, s2.getptr()) > 0;}
inline int operator>= (char const *s1, String const &s2)        {return strcmp(s1, s2.getptr()) >= 0;}

inline String& String::operator+= (char const *s)
{
    return append(s);
}
inline String& String::operator+= (String const &s)
{
    return append(s);
}
inline String& String::operator+= (char C)
{
    return append(C);
}

inline String operator+ (String const &s1, char const *s2)
{
    String t(s1);
    return t += s2;
}
inline String operator+ (char const *s1, String const &s2)
{
    String t(s1);
    return t += s2;
}
inline String operator+ (String const &s1, String const &s2)
{
    String t(s1);
    return t += s2;
}
inline String operator+ (String const &s, char C)
{
    String t(s);
    return t += C;
}

inline String& String::copy(String const &s)
{
    if (&s != this) // compare ptrs but not contents
        copy(s._S, s.W);
    return *this;
}

inline String& String::append(String const &s)
{
    return append(s._S, s.W);
}

inline String& String::insert(int pos, String const &x)
{
    return insert(pos, x._S, x.W);
}

inline String& String::put(int pos, String const &x)
{
    return put(pos, x._S, x.W);
}

inline String& String::removebefore(int pos)
{
    return remove(0, pos);
}

inline String& String::removeafter(int pos)
{
    int L = W;
    if (pos > L - 2)
        return *this;
    else
    {
        int n = L - pos - 1;
        return (n > 0)? remove(pos + 1, n) : (*this);
    }
}

inline String& String::removefrom(int pos)
{
    int L = W;
    if (pos > L - 1)
        return *this;
    else
    {
        int n = L - pos;
        return (n > 0)? remove(pos, n) : (*this);
    }
}

inline String String::before(int pos) const
{
    return at(0, pos);
}

inline String String::after(int pos) const
{
    int L = W;
    if (pos > L - 1)
        eBadIndex();
    else if (pos == L - 1)
        return String(0);
    int n = L - pos - 1;
    return (n > 0)? at(pos + 1, n) : String(0);
}

inline String String::from(int pos) const
{
    int L = W;
    if (pos > L)
        eBadIndex();
    else if (pos == L)
        return String(0);
    int n = L - pos;
    return (n > 0)? at(pos, n) : String(0);
}

inline int String::search(String const &x, int pos) const
{
    return search(x._S, pos);
}

inline int String::rsearch(String const &x, int pos) const
{
    return rsearch(x._S, pos, x.W);
}

inline int String::freq(String const &x) const
{
    return freq(x._S);
}

inline String& String::removeall(String const &x)
{
    return removeall(x._S, x.W);
}

inline String& String::replaceall(String const &S_Old, String const &S_New)
{
    return replaceall(S_Old._S, S_New._S, S_Old.W, S_New.W);
}

#endif //__STRINGP_H__
