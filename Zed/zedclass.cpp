/*
** zedclass.cpp
**
** ZED Programmers Editor edit engine.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#include <io.h>
#ifdef unix
#include <unistd.h>
#endif

#include "zedclass.h"

//#define DEBUG_ZEDCLASS

#ifdef DEBUG_ZEDCLASS
#include <conio.h>
#include <fstream.h>
#endif

bool    zed::shoulddelete   = true;
bool    zed::persistent     = false;
int     zed::shifted        = 0;
int     zed::hardtabsize    = 8;
char    zed::zed_buf[__zed_maxlinelength + 256];
#ifdef BUFFERED
char    zed::myiobuf[iobufsize];
bool    zed::buffered       = true;
#else
bool    zed::buffered       = false;
#endif
bool    zed::select_ok      = true;
bool    zed::unix_end       = false;

zed     *zed::cb            = NULL;

void (*zed::callback_on_insert)(zed*, int, int)         = NULL;
void (*zed::callback_on_remove)(zed*, int, int)         = NULL;
void (*zed::callback_analize)(zed*, int, int)           = NULL;
//*-----------------------------------------------------------------------------

void zed::cbia(int p1, int p2, int p3, int p4)
{
    cbi(p1, p2);
    cba(p3, p4);
}

void zed::cbra(int p1, int p2, int p3, int p4)
{
    cbr(p1, p2);
    cba(p3, p4);
}

void zed::atput(int n, char *t)
{
    linebuf->atPut(n, t);
    cba(n, n);
    modified = true;
}

void zed::atreplace(int n, char *t)
{
    delete[] (char*)linebuf->at(n);
    linebuf->atPut(n, t);
    cba(n, n);
    modified = true;
}

void zed::atremove(int n)
{
    linebuf->atRemove(n);
    int__linecount--;
    cbr(n);
    modified = true;
}

void zed::atremove(int n, int count)
{
    if (count <= 0)
        return;
    linebuf->atRemove(n, count);
    int__linecount -= count;
    cbr(n, count);
    modified = true;
}

void zed::atfree(int n)
{
    linebuf->atFree(n);
    int__linecount--;
    cbr(n);
    modified = true;
}

void zed::atfree(int n, int count)
{
    if (count <= 0)
        return;
    linebuf->atFree(n, count);
    int__linecount -= count;
    cbr(n, count);
    modified = true;
}

void zed::atinsert(int n, char *t)
{
    linebuf->atInsert(n, t);
    int__linecount++;
    cbia(n, 1, n, n);
    modified = true;
}

char const *zed::myat(int lineno, int *l)
{
    _string t = (char*)linebuf->at(lineno);
    if (__nospaces)
    {
        t.removetrailingspace();
        linebuf->atPut(lineno, const_cast<char*>(t.c_str()));
    }
    if (l)
        *l = t.getwidth();
    return t.c_str();
}

_string zed::myat_string2(int lineno)
{
    int l;
    char const *s = myat(lineno, &l);
    return _string(const_cast<char*>(s), l);
}

void zed::movestart_brief()
{
    sel1();
    if (vx == 0)
    {
#if 0
        if (vy == cornery)
            movetop();
        else movepagestart();
#endif
    }
    else
    {
        int L;
        char const *s = myat(vy, &L);
        int end = MAX(0, MIN(L - 1, vx));
        if (!end)
            movestart();
        else
        {
            int i = 0;
            for (; i <= end && ISSPACE(s[i]); ++i);
            if (i > end || i >= vx)
                movestart();
            else vx = i;
        }
    }
    if (vx < cornerx || vx > cornerx + maxx - 1)
        cornerx = vx;
    sel2();
}

String zed::Get_word_under_cursor()
{
    int L;
    char const *t = myat(vy, &L);
    String r = "";
    if (L > 0)
    {
        int i, j;
        if (vx < L)
        {
            if (wordchar(t[vx]))
            {
                for (i = vx; i >= 0 && wordchar(t[i]); --i);
                for (j = vx; j < L && wordchar(t[j]); ++j);
                r = _string(const_cast<char*>(t)).fromto(i + 1, j - 1);
            }
            else
            {
                for (i = vx; i >= 0 && !wordchar(t[i]); --i);
                for (j = vx; j < L && !wordchar(t[j]); ++j);
                bool ok1 = ( i >= 0 && wordchar(t[i]) );
                bool ok2 = ( j < L && wordchar(t[j]) );
                if (ok1 && ok2)
                {
                }
                else if (ok1)
                {
                }
                else
                {
                }
            }
        }
        else
        {
        }
    }
    return r;
}

int zed::checkanchor(int &x1, int &y1, int &x2, int &y2)
{
    if (!anchoractive)
        return 0;
    if (y1 > y2)
        SWAP(y1, y2), SWAP(x1, x2);
    int m = int__linecount - 1;
    if (y2 > m)
        y2 = m;
    if (y1 > y2)
        y1 = y2;
    int L1, L2;
    myat(y1, &L1);
    myat(y2, &L2);
    if (x1 > L1)
        x1 = L1;
    if (x2 > L2)
        x2 = L2;
    if (x1 > x2 && y1 == y2)
        SWAP(x1, x2);
    int t = !(x1 == x2 && y1 == y2);
    if (!t)
        anchoractive = 0;
    return t;
}

int zed::getcheckedanchor(int &x1, int &y1, int &x2, int &y2)
{
    x1 = anchorx1, x2 = anchorx2;
    y1 = anchory1, y2 = anchory2;
    if (!checkanchor(x1, y1, x2, y2))
        return 0;
    return 1;
}

int zed::getanchor(int &x1, int &y1, int &x2, int &y2)
{
    x1 = anchorx1, x2 = anchorx2;
    y1 = anchory1, y2 = anchory2;
    if (!anchoractive)
        return 0;
    return 1;
}

int zed::blocklinecount()
{
    if (anchoractive == 0)
        return 0;
    anx1 = anchorx1, anx2 = anchorx2;
    any1 = anchory1, any2 = anchory2;
    if (!checkanchor(anx1, any1, anx2, any2))
        return 0;
    if (any1 == any2)
        return 1;
    int n = any2 - any1 + 1;
    if (anx2 == 0)
        n--;
    if (n < 0)
        ie();
    return n;
}

void zed::_init_cb(int count)
{
    linebuf = new mycollection(count, 1);
    zed_maxlinelength = __zed_maxlinelength;
    autotabsizeonload = false;
    tabsizedetected = true;
}

void zed::init_cb(int count)
{
    if (cb)
        ie();
    cb = new zed(0, 0, 0);
    cb->_init_cb(count);
}

void zed::clear_cb()
{
    if (!cb)
        return;
    bool t = shoulddelete;
    shoulddelete = true;
    delete cb;
    cb = NULL;
    shoulddelete = t;
}

String zed::blockline(int X, int n)
{
    if (!n || X < 0 || X >= n)
        ie();
    if (n == 1)
    {
        String t(at(any1));
        if (__nospaces)
            t.removetrailingspace();
        if (any2 != any1)
            return t.from(anx1) + '\n';
        else return t.at(anx1, anx2 - anx1);
    }
    else
    {
        int ind = any1 + X;
        String t(at(ind));
        if (__nospaces)
            t.removetrailingspace();
        if (ind == any1)
            return t.at(anx1, __npos) + '\n';
        else if (ind == any2)
            return t.at(0, anx2);
        else return t + '\n';
    }
}

void zed::insertattop(String const &X)
{
    linebuf->atInsert(int__linecount, X.storecontents());
    cbia(int__linecount, 1, int__linecount, int__linecount);
    int__linecount++;
}

int zed::copyto_cb()
{
    int n = blocklinecount();
    // \противоречия с write_file() (пустая строка в конце);
    // \blocklinecount() проблем;
    // \blockline() проблем;
    // фигня во write_file() для записи cb временная;
    // myat() и '\n' (если будет removetrailing..);

    if (!n)
        return 0;
    clear_cb();
    try
    {
        init_cb(n);
        for (int i = 0; i < n; ++i)
        {
            cb->insertattop(blockline(i, n));
        }
        /**/
        if (any2 != any1 && anx2 == 0)
            cb->insertattop(String(0));
        /**/
    }
    catch (exceptionMemExhausted)
    {
        clear_cb(); //todo
        return 1;
    }
    catch (...) {throw;}
    return 0;
}

void zed::setanchor(int x1, int y1, int x2, int y2)
{
    anchorx1 = x1, anchory1 = y1;
    anchorx2 = x2, anchory2 = y2;
    anchoractive = selection_started = true;
}

int zed::getlinecount()
{
    int t = linebuf->getCount();
    if (t != int__linecount)
        ie();
    return t;
}

int zed::copyfrom_cb()
{
    if (!cb)
        return 0;

    int n = cb->getlinecount();
    if (n < 1)
        ie();
    if (!*cb->at(n - 1))
        n--;
    if (n < 1)
        ie();

    if (n == 1)
    {
        String x = cb->at(0);
        int l = x.getwidth();
        int lf = x[l - 1] == '\n';
        if (lf)
            x.clip(--l);
        _string t = const_cast<char*>(at(vy));
        int tl = t.getwidth();
        String rest = 0;

        if (vx > tl)
            t += String(' ', vx - tl) + x;
        else
        {
            if (lf && insert_state)
            {
                rest = t.from(vx);
                if (__nospaces)
                    rest.removetrailingspace();
                t.removefrom(vx);
            }
            if (insert_state)
                t.insert(vx, x);
            else t.put(vx, x);
        }
        if (lf)
            setanchor(vx, vy, 0, vy + 1);
        else setanchor(vx, vy, vx + l, vy);

        if (lf)
            atinsert(vy + 1, rest.storecontents());

        t.clip(zed_maxlinelength);
        if (__nospaces)
            t.removetrailingspace();
        atput(vy, const_cast<char*>(t.c_str()));
    }
    else // n >= 2
    {
        String x = cb->at(0);
        int l = x.getwidth();
        x.clip(--l);
        _string t = const_cast<char*>(at(vy));
        int tl = t.getwidth();
        String rest = 0;
        bool rest_zero = true;

        if (vx > tl)
            t += String(' ', vx - tl) + x;
        else
        {
            {
                rest = t.from(vx);
                rest_zero = false;
                if (__nospaces)
                    rest.removetrailingspace();
                t.removefrom(vx);
            }
            t.insert(vx, x);
        }

        atinsert(vy + 1, rest.storecontents());

        t.clip(zed_maxlinelength);
        if (__nospaces)
            t.removetrailingspace();
        atput(vy, const_cast<char*>(t.c_str()));

        int _vy = vy + 1;

        int pure = n - 2;
        if (pure > 0)
        {
            linebuf->atInsertDummy(_vy, pure);
            cbi(_vy, pure);
            int__linecount += pure;
            for (int i = 0; i < pure; ++i)
            {
                String tmp = cb->at(i + 1);
                tmp.clip(tmp.getwidth() - 1);
                atput(_vy + i, tmp.storecontents());
            }
        }

        _vy += pure;

        x = cb->at(n - 1);
        l = x.getwidth();
        int lf = x[l - 1] == '\n';
        if (lf)
            x.clip(--l);

        if (lf)
        {
            if (rest_zero)
                atreplace(_vy, x.storecontents());
            else atinsert(_vy, x.storecontents());
        }
        else
        {
            _string tmp = const_cast<char*>(at(_vy));
            tmp.insert(0, x);
            tmp.clip(zed_maxlinelength);
            if (__nospaces)
                tmp.removetrailingspace();
            atput(_vy, const_cast<char*>(tmp.c_str()));
        }
        if (lf)
            setanchor(vx, vy, 0, _vy + 1);
        else setanchor(vx, vy, l, _vy);
    }

    checkanchor(anchorx1, anchory1, anchorx2, anchory2);

    return 0;
}

void zed::removeblock()
{
    int n = blocklinecount();
    if (!n)
        return;
    dropanchor();
    if (n == 1 && any2 == any1)
    {
        _string t(const_cast<char*>(at(any1)));
        atput(any1, const_cast<char*>(t.remove(anx1, anx2 - anx1).c_str()));
    }
    else
    {
        int pure = n - 2;
        if (anx2 == 0)
            pure++;
        int j = any1 + 1;
        if (pure > 0)
            atfree(j, pure);
        if (anx1 == 0 && anx2 == 0)
            atfree(any1);
        else
        {
            _string F = const_cast<char*>(at(any1));
            String L(at(j));
            F.remove(anx1, __npos);
            F += L.remove(0, anx2);
            atfree(j);
            atput(any1, const_cast<char*>(F.c_str()));
        }
    }
    moveexact(anx1, any1);
    cornermove(0, 0);
    pagemove(0);
}

void zed::write_block(char const *filename)
{
    int n = blocklinecount();
    if (!n)
        return;
    int i;
    File f(filename, "w+");
#ifdef BUFFERED
    if (buffered)
        f.setvbuf(myiobuf, _IOFBF, iobufsize);
#endif
    for (i = 0; i < n; ++i)
        f.fputs(blockline(i, n).c_str());
}

void zed::indent_block(int step)
{
    if (!step)
        return;
    int n = blocklinecount();
    if (!n)
        return;
    int i;
    if (step > 0)
    {
        if (step == tab_size)
        {
            int min_spaces = __npos;
            for (i = 0; i < n; ++i)
            {
                char const *t = myat(any1 + i);
                if (!*t)
                    continue;
                int sc = startspacecount(t);
                if (sc < min_spaces)
                    min_spaces = sc;
            }
            int r = min_spaces % tab_size;
            if (r)
                step -= r;
        }
        for (i = 0; i < n; ++i)
        {
            _string t = const_cast<char*>(at(any1 + i));
            t.insert(0, String(' ', step));
            t.clip(zed_maxlinelength);
            if (__nospaces)
                t.removetrailingspace();
            atput(any1 + i, const_cast<char*>(t.c_str()) );
        }
    }
    else if (step < 0)
    {
        int min_spaces = __npos;
        for (i = 0; i < n; ++i)
        {
            char const *t = myat(any1 + i);
            if (!*t)
                continue;
            int sc = startspacecount(t);
            if (sc < min_spaces)
                min_spaces = sc;
        }
        if (!min_spaces)
            return;
        step = ABS(step);
        if (step == tab_size)
        {
            int r = min_spaces % tab_size;
            if (r)
                step = r;
        }
        if (min_spaces >= step)
        {
            for (i = 0; i < n; ++i)
            {
                _string t = const_cast<char*>(at(any1 + i));
                if (t.__empty())
                    continue;
                t.remove(0, step);
                atput(any1 + i, const_cast<char*>(t.c_str()) );
            }
        }
    }
}

int zed::getselectarea(int virtind, int &sx1, int &sx2)
{
    anx1 = anchorx1, anx2 = anchorx2;
    any1 = anchory1, any2 = anchory2;
    sx1 = sx2 = -1;
    if (!checkanchor(anx1, any1, anx2, any2))
        return 1;

    int i = virtind + cornery;
    if (i >= int__linecount)
        return 1;

    if (anchoractive == 0 || i < any1 || i > any2 || (anx1 == anx2 && any1 == any2))
        return 1;

    int L;
    myat(i, &L);

    if (i > any1 && i < any2)
    {//1
        sx1 = 0, sx2 = maxx - 1;
        return 0;
    }
    if (anx1 == 0 && any1 < any2 && i == any1)
    {//2
        sx1 = 0, sx2 = maxx - 1;
        return 0;
    }
    if (i == any2 && any1 != any2)
    {//3
        sx1 = 0, sx2 = MIN(anx2, L) - 1 - cornerx;
        if (sx2 < 0)
        {
            sx1 = -1, sx2 = -1;
            return 1;
        }
        return 0;
    }
    if (i == any1 && any1 != any2)
    {//4
        sx1 = MAX(anx1 - cornerx, 0), sx2 = maxx - 1;
        return 0;
    }
    if (i == any1 && i == any2 && anx1 != anx2)
    {//5
        if (anx1 > anx2)
            SWAP(anx1, anx2);
        sx1 = MAX(anx1 - cornerx, 0), sx2 = MIN(L, anx2) - 1 - cornerx;
        if (sx2 < 0)
        {
            sx1 = -1, sx2 = -1;
            return 1;
        }
        return 0;
    }
    return 1;
} /*YES!!!*/

void zed::select_word()
{
    int L;
    char const *s = myat(vy, &L);
    if (!*s)
        return;
    if (vx >= L || !wordchar(s[vx]))
        return;
    int i = vx, j = vx;
    for (; i >= 0 && wordchar(s[i]); --i);
    for (; j < L && wordchar(s[j]); ++j);
    i++, j--;
    if (anchoractive && anchorx1 == i && anchorx2 == j + 1 && anchory1 == vy && anchory2 == vy)
        dropanchor();
    else setanchor(i, vy, j + 1, vy);
}

void zed::startblock()
{
    astartx = vx;
    astarty = vy;

    anchorx1 = vx, anchory1 = vy;

    if (was_end)
    {
        anchorx2 = aendx, anchory2 = aendy;
        selection_started = true;
        anchoractive = true;
        //was_end = false;
    }

    was_start = true;
}

void zed::endblock()
{
    if (was_start)
    {
        anchorx1 = astartx, anchory1 = astarty;

        aendx = vx;
        aendy = vy;

        anchorx2 = vx, anchory2 = vy;
        selection_started = true;
        anchoractive = true;

        was_end = true;
    }
}

void zed::dropanchor()
{
    selection_started = false;
    anchoractive = false;
}

void zed::sel1()
{
    if (!select_ok)
        return;
    if (++selinc > 1)
        return;
    if (!shifted && selection_started && !persistent)
        dropanchor();
    if (shifted && (!selection_started || persistent))
    {
        if (persistent)
        {
            if (!selection_started)
                goto L;
            if (vx == anchorx2 && vy == anchory2)
                return;
            dropanchor();
        }
    L:
        anchorx1 = vx, anchory1 = vy;
        selection_started = true;
    }
}

void zed::sel2()
{
    if (!select_ok)
        return;
    selinc--;
    if (selinc < 0)
        ie();
    if (selinc > 0)
        return;
    if (persistent && !shifted)
        return;
    if (selection_started)
    {
        anchorx2 = vx, anchory2 = vy;
        anchoractive = true;
    }
}

void zed::moveend()
{
    sel1();
    vx = __nospaces? (lastnonspace(at(vy)) + 1) : strlen(at(vy));
    if (vx > zed_maxlinelength - 1)
        vx = zed_maxlinelength - 1;
    cornerx = vx - maxx + 1;
    if (cornerx < 0)
        cornerx = 0;
    sel2();
}

void zed::movebottom()
{
    sel1();
    vy = int__linecount - 1;
    cornery = int__linecount - maxy;
    if (cornery < 0)
        cornery = 0;
    sel2();
}

int zed::getclippedlinevirt(char *buf, int virtind, int &Length)
    const
{
    memset(buf, ' ', maxx);
    buf[maxx] = '\0';
    int i = virtind + cornery;
    if (i < int__linecount)
    {
        char const *line = const_at(i);
        int L = Length = strlen(line);
        if (L > cornerx)
        {
            int t = L - cornerx;
            if (t > maxx)
                t = maxx;
            memcpy(buf, line + cornerx, t);
            return t;
        }
    }
    return 0;
}

void zed::pagemove(int DY)
{
    sel1();
    int t = vy - cornery;
    move(0, DY);
    if (vy == int__linecount - 1)
        cornery = vy - maxy + 1;
    else cornery = vy - t;
    if (cornery + maxy > int__linecount)
        cornery = int__linecount - maxy;
    if (cornery < 0) cornery = 0;
    move(0, 0);
    sel2();
}

void zed::correctcursor(int clip)
{
    if (!cursorbeyondend)
    {
        int L = clip? 0 : strlen(at(vy));
        if (clip)
            myat(vy, &L);
        if (vx > L)
        {
            vx = L;
            move(0, 0);
        }
        if (vx == L && cornerx + maxx - 1 > vx && cornerx > 0)
            cornerx--;
    }
}

int zed::cornermove(int DX, int DY)
{
    sel1();

    int oldvx = vx, oldvy = vy;
    int oldcy = cornery, oldcx = cornerx;

    cornery += DY, vy += DY;

    if (zed_maxlinelength > maxx)
    {
        cornerx += DX, vx += DX;

        if (cornerx + maxx > zed_maxlinelength)
            cornerx = zed_maxlinelength - maxx;
        if (vx >= zed_maxlinelength)
            vx = zed_maxlinelength - 1;
        if (cornerx < 0)
            cornerx = 0;
        if (vx < 0)
            vx = 0;
    }

    if (cornery > int__linecount - maxy)
        cornery = int__linecount - maxy;
    if (cornery < 0)
        cornery = 0;
    if (vy >= int__linecount)
        vy = int__linecount - 1;
    if (vy < 0)
        vy = 0;

    if (cornerx == oldcx && cornery == oldcy)
        vx = oldvx, vy = oldvy;
    correctcursor(__nospaces);
    sel2();

    return oldcx == cornerx && oldcy == cornery;

}

int zed::move(int DX, int DY)
{
    sel1();
    int oldvx = vx, oldvy = vy;
    vx += DX, vy += DY;

    if (vx >= zed_maxlinelength)
        vx = zed_maxlinelength - 1;
    if (vx > cornerx + maxx - 1)
        cornerx = vx - maxx + 1;
    if (vx < 0)
        vx = 0;
    if (vx < cornerx)
        cornerx -= cornerx - vx;
    if (vy >= int__linecount)
        vy = int__linecount - 1;
    if (vy > cornery + maxy - 1)
        cornery = vy - maxy + 1;
    if (vy < 0)
        vy = 0;
    if (vy < cornery)
        cornery -= cornery - vy;
    correctcursor(__nospaces);

    sel2();
    return (oldvx == vx && oldvy == vy) ? 0 : 1;
}

void zed::gotoline(int line)
{
    sel1();
    moveexact(0, line);
    center();
    sel2();
}

int zed::moveexact(int nvx, int nvy)
{
    sel1();
    int oldvx = vx, oldvy = vy;
    vx = nvx, vy = nvy;

    if (vx >= zed_maxlinelength)
        vx = zed_maxlinelength - 1;
    if (vx > cornerx + maxx - 1)
        cornerx = vx - maxx + 1;
    if (vx < 0)
        vx = 0;
    if (vx < cornerx)
        cornerx -= cornerx - vx;
    if (vy >= int__linecount)
        vy = int__linecount - 1;
    if (vy > cornery + maxy - 1)
        cornery = vy - maxy + 1;
    if (vy < 0)
        vy = 0;
    if (vy < cornery)
        cornery -= cornery - vy;
    move(0, 0);

    sel2();
    return (oldvx == vx && oldvy == vy) ? 0 : 1;
}

void zed::putstr(char const *s, int index)
{
    delete[] const_cast<char*>(at(index));
    linebuf->atPut(index, mystrdup(s));
    cba(index, index);
}

void zed::zero()
{
    int__linecount = 0;
    int__biglinecount = 0;
    int__actualnewlines = 0;
    int__fileloaded = false;
    vx = vy = 0;
    cornerx = cornery = 0;
    anchorx1 = anchory1 = 0;
    anchorx2 = anchory2 = 0;

    was_start = was_end = false;
    astartx = astarty = aendx = aendy = 0;

    anchoractive = false;
    selection_started = false;
    newcreated = changed = zeroloaded = false;
    modified = false;
    insert_state = 1;
    selinc = 0;

    __nospaces = true;

    zed_maxlinelength = __zed_maxlinelength;
    tabsizedetected = true;

    linebuf = NULL;
}

void zed::shutdown()
{
    if (newcreated && !changed)
    { //todo
        unlink(int__filename.c_str());
    }

    /*if (cb && this != cb && shoulddelete) clear_cb();*/

    if (linebuf && shoulddelete)
    {
        int count = linebuf->getCount();
        if (this != cb)
        {
            if (count != int__linecount)
                ie();
        }
        for (int i = 0; i < count; ++i)
        {
            char *t = (char*)linebuf->at(i);
            if (t)
                delete[] t;
        }
        delete linebuf;
        linebuf = NULL;
    }
    zero();
}

void zed::initempty()
{
    linebuf = new mycollection(2, 1);
    linebuf->insert(mystrdup("", 0));
    int__linecount = 1;
    zed_maxlinelength = __zed_maxlinelength;
    tab_size = 4;
    autotabsizeonload = false;
    tabsizedetected = true;
}

zed::zed(int nmx, int nmy, int pagesize)
{
    zero();
    maxx = nmx;
    maxy = nmy;
    int__pagesize = pagesize;

    tab_size = 4;
    autoindent = true;
    cursorbeyondend = true;
    autotabsizeonload = true;
}

void zed::read_file(char const *filename)
{
    shutdown();

    if (!fexist(filename))
    {
        File t(filename, "w+b");
        newcreated = true;
    }

    File f(filename, "r");

    tabsizedetected = false;

#ifdef BUFFERED
    if (buffered)
    {
        try
        {
            f.setvbuf(myiobuf, _IOFBF, iobufsize);
        }
        catch (FileBufException) {}
        catch (...) {throw;}
    }
#endif

    int filelen = filelength(fileno( f() ));

    int guess_line_count = (int) (filelen * 0.04);
    int const low_bound = 1000;
    if (guess_line_count < low_bound)
        guess_line_count = low_bound;
    int mylimit = (int) (guess_line_count * (guess_line_count < 10000? 1.5 : 1.1));
    int mydelta = (int) (guess_line_count / 2.0);
    if (mydelta > zed_delta)
        mydelta = zed_delta;
    linebuf = new mycollection(mylimit, mydelta);

    zed_maxlinelength = __zed_maxlinelength;

    bool last = false;
    bool prevbig = false;

    sortedcollect<autotabaux> *c = new sortedcollect<autotabaux>(30, 10);
    sortedcollect<autotabaux> *cii = new sortedcollect<autotabaux>(30, 10);

    int prevwhitespace = 0;

#ifdef DEBUG_ZEDCLASS
    fstream log("_zclog", ios::out | ios::trunc);
#endif

    while (f.fgets(zed_buf, zed_maxlinelength + 1))
    {
        int L = strlen(zed_buf);

        if (zed_buf[L - 1] == '\n')
        {
            if (prevbig && L == 1)
            { //todo: not sure ;-(
                prevbig = false;
                continue; // skip newline probably from previous "big" line (true for from-ZED-saved files)
            }
            zed_buf[--L] = 0;
            int__actualnewlines++;
            last = true, prevbig = false;
        }
        else int__biglinecount++, last = false, prevbig = true;

        int new_len = expandtabtospaces(zed_buf, killchar(zed_buf, '\r', L), zed_maxlinelength, hardtabsize);
        removewhitespaceatend(zed_buf, new_len);

        if (autotabsizeonload)
        {
            int space = startspacecount(zed_buf);
            int dif = ABS(space - prevwhitespace); //todo: skip negative? [bug01.cc] [switch-case-break-}]
            prevwhitespace = space;
            if (dif)
            {
#ifdef DEBUG_ZEDCLASS
                log << dif << endl;
#endif
                autotabaux t(dif);
                int index;
                if (c->search(&t, index))
                {
                    ((autotabaux*)c->at(index))->count++;
                }
                else
                {
                    c->insert(new autotabaux(dif));
                }
            }
        }

        linebuf->atInsert(int__linecount++, mystrdup(zed_buf, new_len));
    }

    if (int__linecount || filelen == 0)
    {
        int__fileloaded = true;
        if (filelen == 0)
            zeroloaded = true;
        if (last || filelen == 0)
        {
            linebuf->atInsert(int__linecount++, mystrdup("", 0));
            if (last)
                int__actualnewlines++;
        }
        int__filename = filename;
    }

    int difcount = c->getCount();

#ifdef DEBUG_ZEDCLASS
    log << "---------------------------------------" << endl;
#endif
    if (difcount > 0 && autotabsizeonload)
    {
        int tab = -1, i;
        cii->duplicates = True;

        for (i = 0; i < difcount; ++i)
        {
            autotabaux *t = (autotabaux*)c->at(i);
            t->sortbydif = false;
            cii->insert(t);
        }

#ifdef DEBUG_ZEDCLASS
        for (i = difcount - 1; i >= 0; --i)
        {
            log << ((autotabaux*)cii->at(i))->dif << ' ' << ((autotabaux*)cii->at(i))->count << endl;
        }
#endif
        for (i = difcount - 1; i >= 0; --i)
        {
            tab = ((autotabaux*)cii->at(i))->dif;
            if (tab >= 2 && tab <= 32)
            {
                tab_size = tab;
                tabsizedetected = true;
                break;
            }
        }
        cii->removeAll();
    }

    delete cii;
    delete c;

    if (!int__fileloaded)
        throw InternalErrorOnReadFile();
}

void zed::write_file(char const *filename)
{
    if (!linebuf || !int__linecount)
        throw InternalErrorOnWriteFile();
    File f(filename, "wb");
#ifdef BUFFERED
    if (buffered)
    {
        try
        {
            f.setvbuf(myiobuf, _IOFBF, iobufsize);
        }
        catch (FileBufException) {}
        catch (...) {throw;}
    }
#endif
    int last = int__linecount - 1;
    for (int i = 0; i < int__linecount; i++)
    {
        int L;
        char const *s = myat(i, &L);
        memcpy(zed_buf, s, L+1);
#if 1
        if (L && zed_buf[L - 1] == '\n')
            zed_buf[--L] = '\0';
#endif
        if (i < last)
        {
            if (unix_end)
                zed_buf[L++] = '\n';
            else
            {
                zed_buf[L++] = '\r';
                zed_buf[L++] = '\n';
            }
        }
        else if (!*zed_buf)
            break;
        f.fwrite(zed_buf, L);
    }
    modified = false;
    changed = true;
}

int zed::lastnonspace(char const *s)
{
    int l = strlen(s);
    if (l == 0)
        return -1;
    int i = l - 1;
    for (; i >= 0 && s[i] == ' '; --i);
    return i;
}

int zed::expandtabtospaces(char *s, int L, int maxlen, int tabsize)
{
    if (!s || L == 0)
        return 0;
    if (!memchr(s, '\t', L))
        return L;
    int nl = 0;
#ifdef NONSTATICTAB
    char *temp = new char[maxlen + 1];
#else
    static char temp[__zed_maxlinelength + 256];
#endif
    for (int i=0, o=0; i < L && nl < maxlen; i++) //todo: use memchr()
    {
        if (s[i] == '\t')
        {
            int t = tabsize - (i + o) % tabsize;
            for(int j = 0; j < t && nl < maxlen; j++, o++)
                temp[nl++] = ' ';
            o--;
        }
        else temp[nl++] = s[i];
    }
    if (nl > maxlen)
        nl = maxlen;
    memcpy(s, temp, nl);
    s[nl] = '\0';
#ifdef NONSTATICTAB
    delete[] temp;
#endif
    return nl;
}

void zed::deletecurrentline()
{
    if (int__linecount == 1)
        atreplace(0, mystrdup("", 0));
    else
    {
        atfree(vy);
        pagemove(0);
    }
    modified = true;
}

void zed::dupcurrentline()
{
    int l;
    char const *s = myat(vy, &l);
    atinsert(vy + 1, mystrdup(s, l));
}

void zed::insertemptyline()
{
    atinsert(vy, mystrdup("", 0));
}

void zed::deletetoeol()
{
    _string t = const_cast<char*>(at(vy));
    t.clip(vx);
    if (__nospaces)
        t.removetrailingspace();
    atput(vy, const_cast<char*>(t.c_str()));
}

void zed::goleftword()
{
    sel1();
    int L;
    char const *t = myat(vy, &L);
    if (vx > 0 && L > 0)
    {
        int k = vx - 1;
        if (k > L - 1)
            k = L - 1;
        while (k >= 0 && !wordchar(t[k]))
            k--;
        if (k >= 0)
        {
            while (k >= 0 && wordchar(t[k]))
                k--;
        }
        vx = ++k;
        if (cornerx > vx)
            cornerx = vx;
    }
    else if (vx > 0 && L == 0)
    {
        vx = cornerx = 0;
    }
    sel2();
}

void zed::gorightword()
{
    sel1();
    int L;
    char const *t = myat(vy, &L);
    if (vx < L && L > 0)
    {
        int k = vx;
        while (k < L && wordchar(t[k]))
            k++;
        if (k < L)
        {
            while (k < L && !wordchar(t[k]))
                k++;
        }
        vx = k;
        if (vx > zed_maxlinelength - 1)
            vx = zed_maxlinelength - 1;
        if (vx >= cornerx + maxx)
            cornerx = vx - maxx + 1;
    }
    sel2();
}

void zed::insertchar(char C)
{
    _string t = myat_string2(vy);
    int L = t.getwidth();
    if (!insert_state)
    {   // OVERWRITE
        if (vx == L && L < zed_maxlinelength)
            atput(vy, const_cast<char*>((t += C).c_str()) );
        else if (vx < L)
        {
            t[vx] = C;
            cba(vy, vy);
        }
        else if (vx > L)
        {
            t += String(' ', vx - L) + C;
            atput(vy, const_cast<char*>(t.c_str()));
        }
    }
    else
    {   // INSERT
        if (vx == L && L < zed_maxlinelength)
            atput(vy, const_cast<char*>((t += C).c_str()) );
        else if (vx < L)
        {
            //todo
            if (vx == L - 1 && L == zed_maxlinelength)
            {
                t[vx] = C;
                cba(vy, vy);
            }
            else
            {
                t.insert(vx, C);
                t.clip(zed_maxlinelength);
                atput(vy, const_cast<char*>(t.c_str()));
            }
        }
        else if (vx > L)
        {
            t += String(' ', vx - L) + C;
            atput(vy, const_cast<char*>(t.c_str()));
        }
    }
    bool ns = __nospaces;
    __nospaces = false;
    move(+1, 0);
    __nospaces = ns;
    modified = true;
}

void zed::doenter()
{
    int L;
    char const *t = myat(vy, &L);
    int spaces = 0, i;
    if (autoindent)
    {
        for (i = 0; i < L && i < vx && t[i] == ' '; i++)
            spaces++;
    }
    if (insert_state)
    {
        String N(' ', spaces);
        if (vx < L)
        {
            N += t + vx;
            _string O = const_cast<char*>(t);
            O.clip(vx);
            atput(vy, const_cast<char*>(O.c_str()) );
        }
        atinsert(vy + 1, N.storecontents());
    }
    bool cbe = cursorbeyondend;
    cursorbeyondend = true;
    moveexact(spaces, vy + 1);
    cursorbeyondend = cbe;
}

// -1 -- backward tab,
// +1 -- forward tab
void zed::dotab(int D)
{
    _string t = myat_string2(vy);
    int L = t.getwidth();
    int next;
    if (D == 1)
        next = (vx / tab_size) * tab_size + tab_size;
    else
    {
        if (vx % tab_size)
            next = (vx / tab_size) * tab_size;
        else next = (vx / tab_size) * tab_size - tab_size;
    }
    if (next < 0)
        next = 0;
    if (next >= zed_maxlinelength)
        next = zed_maxlinelength - 1;
    if (insert_state && vx < L)
    {
        if (next > vx)
        {
            String tmp = t.at(0, vx);
            tmp += String(' ', next - vx);
            if (L - vx > 0)
                tmp += t.at(vx, L - vx);
            tmp.clip(zed_maxlinelength);
            atreplace(vy, tmp.storecontents());
        }
        else if (next < vx)
        {
            memcpy(zed_buf, t.c_str(), t.getwidth());
            memmove(zed_buf+next, zed_buf+vx, L-vx);
            atreplace(vy, mystrdup(zed_buf, (L-vx)+next));
        }
    }
    bool cbe = cursorbeyondend;
    if (insert_state)
        cursorbeyondend = true;
    moveexact(next, vy);
    cursorbeyondend = cbe;
}

void zed::dosmarttab(int D)
{
}

void zed::dodelete()
{
    _string t = myat_string2(vy);
    int L = t.getwidth();
    if (vx < L)
    {
        t.remove(vx, 1);
        atput(vy, const_cast<char*>(t.c_str()));
    }
    else
    {
        if (vy >= int__linecount - 1)
            moveend();
        else
        {
            if (vx - L > 0)
                t += String(' ', vx - L);
            t += myat_string2(vy + 1);
            t.clip(zed_maxlinelength);
            atput(vy, const_cast<char*>(t.c_str()));
            atfree(vy + 1);
            pagemove(0);
        }
    }
    if (!persistent)
        dropanchor();
}

void zed::delete_right_word()
{
    _string s = myat_string2(vy);
    int L = s.getwidth();
    if (vx >= L)
        return;
    if (!wordchar(s[vx]) && !ISSPACE(s[vx]))
        dodelete();
    else
    {
        int i = vx;
        while (i < L && ISSPACE(s[i]))
            i++;
        while (i < L && wordchar(s[i]))
            i++;
        i--;
        s.remove(vx, i - vx + 1);
        atput(vy, const_cast<char*>(s.c_str()) );
        /*correctcursor(__nospaces);*/
    }
}

void zed::delete_left_word()
{
    _string s = myat_string2(vy);
    int L = s.getwidth();
    if (vx > L)
        vx = L;
    if (vx > 0)
    {
        if (!wordchar(s[vx - 1]) && !ISSPACE(s[vx - 1]))
            dobackspace();
        else
        {
            int i = vx - 1;
            while (i >= 0 && ISSPACE(s[i]))
                i--;
            while (i >= 0 && wordchar(s[i]))
                i--;
            i++;
            if (insert_state)
                s.remove(i, vx - i);
            else s.put(i, String(' ', vx - i).c_str());
            atput(vy, const_cast<char*>(s.c_str()) );
            moveexact(i, vy);
        }
    }
}

void zed::dobackspace()
{
    _string t = myat_string2(vy);
    int L = t.getwidth();
    if (insert_state)
    {
        if (L > 0 && vx <= L)
        {
            if (vx > 0)
            {
                t.remove(vx - 1, 1);
                atput(vy, const_cast<char*>(t.c_str()));
                move(-1, 0);
                if (vx == myat_string2(vy).getwidth())
                    moveend();
            }
            else if (vy > 0)
            {
                _string prev = myat_string2(vy - 1);
                int pL = prev.getwidth();
                prev += t;
                prev.clip(zed_maxlinelength);
                atput(vy - 1, const_cast<char*>(prev.c_str()));
                atfree(vy);
                moveexact(pL, vy - 1);
                pagemove(0);
            }
        }
        else move(-1, 0);
    }
    else
    {
        int tvx = vx, tvy = vy;
        move(-1, 0);
        if (vy == tvy)
        {
            if (L == 0)
                moveexact(0, vy);
            else if (vx != tvx && L > 0 && vx < L)
            {
                t[vx] = ' ';
                cba(vy, vy);
                if (vx == L - 1 && __nospaces)
                    moveend();
                modified = true;
            }
        }
    }
}

void zed::xcaseword(int mycase)
{
    int L;
    char *t = const_cast<char*>(myat(vy, &L));
    int i = vx;
    if (i >= L)
        return;
    if (wordchar(t[vx]))
    {
        for (; i >= 0 && wordchar(t[i]); --i)
            t[i] = (char) (mycase ? TOUPPER(t[i]) : TOLOWER(t[i]));
        for (i = vx + 1; i < L && wordchar(t[i]); ++i)
            t[i] = (char) (mycase ? TOUPPER(t[i]) : TOLOWER(t[i]));
        cba(vy, vy);
        modified = true;
    }
}

void zed::capitalizeword()
{
    int L;
    char *t = const_cast<char*>(myat(vy, &L));
    int i = vx;
    if (i >= L)
        return;
    if (wordchar(t[vx]))
    {
        xcaseword(0);
        for (i--; i >= 0 && wordchar(t[i]); --i);
        t[i + 1] = (char) TOUPPER(t[i + 1]);
        cba(vy, vy);
        modified = true;
    }
}

void zed::center()
{
    cornery = vy - (maxy / 2 - 1);
    pagemove(0);
}

void zed::centerpage()
{
    center();
    int c = maxx / 2;
    cornerx = vx - c;
    cornermove(0, 0);
}

int zed::search(char const *s, String &o, int X)
{
    String S(s);
    if (S.__empty())
        return -1;
    int c = int__linecount, i;
    o.downcase();
    int backward = o.contains('b');
    int global = o.contains('g');
    int ignore = o.contains('i');
    int onlywords = o.contains('w');

    if (ignore)
        S.downcase();

    int _i = global? (backward? (c - 1) : 0) : vy;
    if (global)
        X = backward? __npos : 0;

    int L = S.getwidth();

    for (i = _i; backward? (i >= 0) : (i < c); i += (backward? -1 : 1))
    {
        //char *ptr = (char*) linebuf->at(i);
        int l;
        char const *ptr = myat(i, &l);
        _string t(ignore? mystrdup(ptr, l) : const_cast<char*>(ptr), l);
        if (ignore)
            t.downcase();
        int pos, whence = ((i == _i)? X : (backward? __npos : 0));
Loop:
        if (L > 1)
            pos = backward? t.rsearch(S, whence) : t.search(S, whence);
        else pos = backward? t.rsearch(S[0], whence) : t.search(S[0], whence);
        if (pos != -1)
        {
            if (onlywords)
            {
                if ((pos > 0 && wordchar(t[pos - 1])) || (pos + L < t.getwidth() && wordchar(t[pos + L])))
                {
                    whence = backward? (pos - 1) : (pos + 1);
                    goto Loop;
                }
            }
            int len = t.getwidth();
            moveexact(pos, i);
            center();
            if (len <= maxx)
                cornerx = 0;
            else centerpage();
            setanchor(vx, vy, vx + L, vy);
            if (ignore)
                t.destroy();
            return 1;
        }
        if (ignore)
            t.destroy();
    }
    return 0;
}

int zed::repeatsearch(char const *s, String &o, int minusdelta, int plusdelta)
{
    o.downcase();
    int backward = o.contains('b');
    o.removeall('g');
    return search(s, o, backward? (vx - minusdelta) : (vx + plusdelta));
}

void zed::doreplace(int L1, int L2, char const *replacestr)
{
    _string t((char*)linebuf->at(vy));
    if (L1 != L2)
    {
//todo: L1 & L2 pass to remove/insert
        t.remove(vx, L1);
        if (L2)
            t.insert(vx, replacestr);
        t.clip(zed_maxlinelength);
        if (__nospaces)
            t.removetrailingspace();
    }
    else t.put(vx, replacestr);
    atput(vy, const_cast<char*>(t.c_str()));
}
