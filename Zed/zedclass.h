/*
** zedclass.h
**
** Defines header for ZED edit engine.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __ZEDCLASS_H__
#define __ZEDCLASS_H__

#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strdup.h"

#include "tcollect.h"
#include "pmt.h"
#include "minmax.h"
#include "ascii.h"

#include "mystring.h"
#include "myfile.h"

#define BUFFERED

#ifdef BUFFERED
int const
// FLOP: 26k read 31k write with 32k buf, DOS HDD read slower, WIN95 HDD ne znau
    iobufsize = 65536 / 2; //todo: read: 32k if removable else 4k, write: 32k
#endif

int const zed_maxfilenamelength     = 255;
int const zed_limit                 = 32768;
int const zed_delta                 = 32768;
int const __zed_maxlinelength       = 65536;

class exceptionMemExhausted {};
class exceptionDummy {};

template <class T> class sortedcollect : public TNSSortedCollection
{
public:
    sortedcollect(ccIndex aLimit, ccIndex aDelta) :
        TNSSortedCollection(aLimit, aDelta)
    {}
    virtual ~sortedcollect()
    {
        shutDown();
    }
    virtual void freeItem(void *item)
    {
        if (item)
            delete (T*)item;
    }
private:
    int compare(void const *key1, void const *key2)
    {
        if (*(T*)key1 == *(T*)key2)
            return 0;
        else if (*(T*)key1 < *(T*)key2)
            return -1;
        else return 1;
    }
};

class autotabaux
{
public:
    int dif, count;

    bool sortbydif;

    autotabaux(int newdif, int newcount = 1) :
        dif(newdif),
        count(newcount),
        sortbydif(true)
    {}

    int operator < (const autotabaux &t)
    {
        return sortbydif? (this->dif < t.dif) : (this->count < t.count);
    }
    int operator > (const autotabaux &t)
    {
        return sortbydif? (this->dif > t.dif) : (this->count > t.count);
    }
    int operator == (const autotabaux &t)
    {
        return sortbydif? (this->dif == t.dif) : (this->count == t.count);
    }
};

class mycollection : public TNSCollection
{
public:
    mycollection(ccIndex aLimit, ccIndex aDelta) :
        TNSCollection(aLimit, aDelta)
    {}
    mycollection() :
        TNSCollection()
    {}
    virtual ~mycollection() {}
    void freeItem(void *item)
    {
        if (item)
            delete[] (char*)item;
    }
};

class zed
{
private:
    static char zed_buf[__zed_maxlinelength + 256];
#ifdef BUFFERED
    static char myiobuf[iobufsize];
#endif

protected:
    String int__filename;
    int int__linecount;
    int int__biglinecount;
    int int__actualnewlines;
    bool int__fileloaded;
    int vx, vy;
    int cornerx, cornery;
    int anchorx1, anchory1, anx1, any1;
    int anchorx2, anchory2, anx2, any2;
    bool anchoractive;
    bool selection_started;
    int int__pagesize;
    bool changed, zeroloaded, newcreated;

    bool was_end, was_start;
    int astartx, astarty;
    int aendx, aendy;

    mycollection *linebuf;

public:
    zed(int, int, int);
    ~zed() {shutdown();}

    class InternalError
    {
    public:
        int line;
        InternalError(int curline = 0) :
            line(curline)
        {}
    };
    class MemExhaustedInBlockOperation : public exceptionMemExhausted, public InternalError {};
    class InternalErrorOnReadFile : public InternalError {};
    class InternalErrorOnWriteFile : public InternalError {};
    class IllegalLineRef : public InternalError
    {
    public:
        IllegalLineRef(int refline) :
            InternalError(refline)
        {}
    };

#define ie() _ie(__LINE__)
    void _ie(int curline = 0) {throw InternalError(curline);}

    void initempty();
    void putstr(char const *s, int index);

    void read_file(char const *filename);
    void write_file(char const *filename);
    char const *getfilename() const {return int__filename.c_str();}
    void changefilename(char const *fn)
    {
        if (!fn)
            int__filename.copy("", 0);
        else int__filename = fn;
    }

    void atput(int n, char *t);
    void atreplace(int n, char *t);
    void atremove(int n);
    void atremove(int n, int count);
    void atfree(int n);
    void atfree(int n, int count);
    void atinsert(int n, char *t);

    char const *currentline()           {return myat(vy);}
    char const *myat(int lineno, int *l = 0);
    char const *at(int lineno)          {return (char*)linebuf->at(lineno);}
    char const *const_at(int lineno) const
    {
        return (char*)linebuf->at(lineno);
    }
    _string myat_string2(int lineno);
    int getlinecount();

    String Get_word_under_cursor();

    int getclippedlinevirt(char *buf, int virtind, int &Length) const;
    int pagex() const {return vx - cornerx;}
    int pagey() const {return vy - cornery;}
    int getvx() const {return vx;}
    int getvy() const {return vy;}
    int getcornerx() const {return cornerx;}
    int getcornery() const {return cornery;}
    int move(int, int);
    int cornermove(int DX, int DY);
    void pagemove(int DY);
    void movestart()
    {
        sel1();
        vx = cornerx = 0;
        sel2();
    }
    void movestart_brief();
    void moveend();
    void correctcursor(int clip = 1);
    void movepagestart()
    {
        vy = cornery;
        move(0, 0);
    }
    void movepageend()
    {
        vy = cornery + maxy - 1;
        if (vy > int__linecount - 1)
            vy = int__linecount - 1;
        move(0, 0);
    }
    void movetop()
    {
        sel1();
        vy = cornery = 0;
        sel2();
    }
    void movebottom();

    void deletecurrentline();
    void dupcurrentline();
    void insertemptyline();
    void deletetoeol();
    void insertchar(char);
    void doenter();
    void dotab(int);
    void dosmarttab(int);
    void dodelete();
    void dobackspace();
    void delete_right_word();
    void delete_left_word();

    int search(char const *s, String &o, int X);
    int repeatsearch(char const *s, String &o, int minusdelta = 1, int plusdelta = 1);
    void doreplace(int L1, int L2, char const *replacestr);

    void center();
    void centerpage();
    void gotoline(int line);

    void xcaseword(int mycase);
    void capitalizeword();

    int moveexact(int nvx, int nvy);
    void synccorner()
    {
        cornery = vy;
        pagemove(0);
    }

    void goleftword();
    void gorightword();

    void sel1();
    void sel2();
    void dropanchor();
    void startblock();
    void endblock();
    int getselectarea(int virtind, int &sx1, int &sx2);
    int checkanchor(int &x1, int &y1, int &x2, int &y2);
    int blocklinecount();
    int getcheckedanchor(int &x1, int &y1, int &x2, int &y2);
    int getanchor(int &x1, int &y1, int &x2, int &y2);
    void setanchor(int x1, int y1, int x2, int y2);
    void select_word();

    String blockline(int X, int n);
    void insertattop(String const &X);

    void init_cb(int count);
    void _init_cb(int count);
    int copyto_cb();
    int copyfrom_cb();
    void removeblock();
    static void clear_cb();
    void indent_block(int step);
    void write_block(char const *filename);

    int expandtabtospaces(char *s, int sl, int maxlen, int tabsize);
    int lastnonspace(char const *s);
    char *removewhitespaceatend(char *s, int &L);

private:
    void cbi(int start, int n = 1)
    {
        if (callback_on_insert)
            callback_on_insert(this, start, n);
    }
    void cbr(int start, int n = 1)
    {
        if (callback_on_remove)
            callback_on_remove(this, start, n);
    }
    void cba(int start, int end)
    {
        if (callback_analize)
            callback_analize(this, start, end);
    }

    void cbia(int p1, int p2, int p3, int p4);
    void cbra(int p1, int p2, int p3, int p4);

public:
    static void (*callback_on_insert)(zed*, int, int);
    static void (*callback_on_remove)(zed*, int, int);
    static void (*callback_analize)(zed*, int, int);

    int zed_maxlinelength;
    int maxx, maxy;
    bool modified;
    int insert_state;
    int tab_size;
    bool autoindent;
    bool cursorbeyondend;

    bool __nospaces;
    static bool unix_end;

    int filetypesource; //todo

    static int shifted;
    static bool shoulddelete;
    static int hardtabsize;
    static bool persistent;
    static bool buffered;
    bool autotabsizeonload;
    bool tabsizedetected;
    int selinc;

    static bool select_ok;

    static zed *cb;

private:
    void zero();
    void shutdown();
};

inline char *zed::removewhitespaceatend(char *s, int &L)
{
    if (L == -1)
        L = strlen(s);
    if (L == 0)
        return s;
    int i = L - 1;
    for (; i >= 0 && s[i] == ' '; --i);
    if (i == L - 1)
        return s;
    s[L = i+1] = '\0';
    if (s == zed_buf)
        return s;
    char *t = mystrdup(s, L);
    delete[] s;
    return t;
}

inline bool wordchar(char t)
{
    return ISALNUM(t) || t == '_';
}

inline int startspacecount(char const *s)
{
    if (!s || !*s)
        return 0;
    int t = 0;
    for (; *s && *s++ == ' '; ++t);
    return t;
}

inline int killchar(char *s, char c, int L = -1)
{
    if (!s || !*s)
        return 0;
    char *t = s;
    if (L == -1)
        L = strlen(s);
    while (*t && (t = strchr(t, (unsigned char)c)) != NULL)
    {
        memmove(t, t + 1, (L--) - (t - s));
    }
    return L;
}

inline void exiterror(char const *msg, int errorcode, int cleanup = 1)
{
    if (msg && *msg)
        fprintf(stderr, msg);
    if (cleanup)
        exit(errorcode);
    else _exit(errorcode);
}

#endif //__ZEDCLASS_H__
