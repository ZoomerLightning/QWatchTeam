
#include <assert.h>
#include "cmt.h"

TNSCollection *Cmt::cmtdata = NULL;

byte *Cmt::pack_cmtline(int index, Unpacked const &linedata)
{
    int cmtCount = linedata.getcount();
    if (!cmtCount)
        return NULL;
    char const *symptr = linedata.get_sym_ptr();
    int const *posptr = linedata.get_pos_ptr();

    int flag = 0, mask = 0;
    static int stor[2];

    if (cmtCount == 1)
        mask = T_CMT_BIT_COUNT_1;
    else if (cmtCount == 2)
        mask = T_CMT_BIT_COUNT_2;
    else if (cmtCount <= 8)
        mask = T_CMT_BIT_COUNT_BE8;
    else mask = T_CMT_BIT_COUNT_A8;
    flag |= mask;

    int const
        MAX_BYTE = 255,
        MAX_WORD = 65535;

    if (cmtCount <= MAX_BYTE)
        mask = T_CMT_BIT_STOR_COUNT_B, stor[0] = 1;
    else if (cmtCount <= MAX_WORD)
        mask = T_CMT_BIT_STOR_COUNT_W, stor[0] = 2;
    else mask = T_CMT_BIT_STOR_COUNT_D, stor[0] = 4;
    flag |= mask;

    int maxpos = 0;
    for (int i = 0; i < cmtCount; i++)
    {
        if (posptr[i] > maxpos)
            maxpos = posptr[i];
    }
    if (maxpos <= MAX_BYTE)
        mask = T_CMT_BIT_STOR_POS_B, stor[1] = 1;
    else if (maxpos <= MAX_WORD)
        mask = T_CMT_BIT_STOR_POS_W, stor[1] = 2;
    else mask = T_CMT_BIT_STOR_POS_D, stor[1] = 4;
    flag |= mask;

    if (cmtCount < 3)
    {
        if (*symptr == '{')
            flag |= CMT_BIT_I_PAREN;
        if (cmtCount == 2 && symptr[1] == '{')
            flag |= CMT_BIT_II_PAREN;
    }

    int const bit_stor_length = (cmtCount+7) >> 3;
    int blockSize = sizeof(int) + sizeof(byte);

    if (cmtCount > 2)
        blockSize += stor[0] + bit_stor_length;
    blockSize += stor[1] * cmtCount;

    byte *buf = new byte[blockSize];

#define AT(OFS, TYPE) *((TYPE *) (buf + (OFS)))
    int o = 0;
    AT(o, int) = index, o += sizeof(int);
    AT(o, byte) = (byte) flag, o += sizeof(byte);

    if (cmtCount > 2)
    {
        switch (stor[0])
        {
            case 1:
                AT(o, byte) = (byte) cmtCount, o += sizeof(byte);
                break;
            case 2:
                AT(o, word) = (word) cmtCount, o += sizeof(word);
                break;
            case 4:
                AT(o, dword) = (dword) cmtCount, o += sizeof(dword);
                break;
        }

        static byte const M[] = {1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80};
        byte B = '\0';
        int curbit = 0;

        for (int i = 0; i < cmtCount; i++)
        {
            if (symptr[i] == '{')
                B |= M[curbit];
            if (++curbit == 8)
            {
                AT(o, byte) = B, o++;
                B = '\0', curbit = 0;
            }
        }
        if (curbit)
            AT(o, byte) = B, o++;
    }

    switch (stor[1])
    {
        case 1:
            for (int i = 0; i < cmtCount; i++)
                AT(o, byte) = (byte) posptr[i], o += sizeof(byte);
            break;
        case 2:
            for (int i = 0; i < cmtCount; i++)
                AT(o, word) = (word) posptr[i], o += sizeof(word);
            break;
        case 4:
            for (int i = 0; i < cmtCount; i++)
                AT(o, dword) = (dword) posptr[i], o += sizeof(dword);
            break;
    }

#undef AT
    return buf;
}

Unpacked const &Cmt::unpack_cmtline(int &index, byte *linedata)
{
    static Unpacked u(2);

    if (!linedata)
    {
        u.count = 0;
        return u;
    }
#define AT(OFS, TYPE) (*((TYPE *) (linedata + (OFS))))
    int o = 0;
    index = AT(o, int), o += sizeof(int);
    byte flag = AT(o, byte);
    o += sizeof(byte);

    byte bc_alone = flag&CMT_BIT_COUNT;
    int cmtCount = 0;
    if (bc_alone == T_CMT_BIT_COUNT_1)
        cmtCount = 1;
    else if (bc_alone == T_CMT_BIT_COUNT_2)
        cmtCount = 2;
    else
    {
        switch (flag&CMT_BIT_STOR_COUNT)
        {
            case T_CMT_BIT_STOR_COUNT_B:
                cmtCount = AT(o, byte), o += sizeof(byte);
                break;
            case T_CMT_BIT_STOR_COUNT_W:
                cmtCount = AT(o, word), o += sizeof(word);
                break;
            case T_CMT_BIT_STOR_COUNT_D:
                cmtCount = AT(o, dword), o += sizeof(dword);
        }
    }
    assert(cmtCount);

    int next = cmtCount;
    if (next > u.size)
    {
        int size = u.size << 1;
        while (next > size)
            size <<= 1;
        u.update_size(size, Unpacked::UPDATE_FAST);
    }

    char *symptr = u.u_sym;
    int *posptr = u.u_pos;

    if (cmtCount < 3)
    {
        symptr[0] = (flag&CMT_BIT_I_PAREN) ? '{' : '}';
        if (cmtCount == 2)
            symptr[1] = (flag&CMT_BIT_II_PAREN) ? '{' : '}';
    }
    else
    {
        static byte const M[] = {1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80};
        int curbit = 0;
        byte B = AT(o, byte);

        for (int i = 0; i < cmtCount; i++)
        {
            symptr[i] = (B & M[curbit]) ? '{' : '}';
            if (++curbit == 8)
            {
                o++;
                B = AT(o, byte), curbit = 0;
            }
        }
        if (curbit)
            o++;
    }

    switch (flag&CMT_BIT_STOR_POS)
    {
        case T_CMT_BIT_STOR_POS_B:
            for (int i = 0; i < cmtCount; i++)
                posptr[i] = AT(o, byte), o += sizeof(byte);
            break;
        case T_CMT_BIT_STOR_POS_W:
            for (int i = 0; i < cmtCount; i++)
                posptr[i] = AT(o, word), o += sizeof(word);
            break;
        case T_CMT_BIT_STOR_POS_D:
            for (int i = 0; i < cmtCount; i++)
                posptr[i] = (int) AT(o, dword), o += sizeof(dword);
            break;
    }

#undef AT
    u.count = cmtCount;

    return u;
}


void Cmt::destroy_cmtdata()
{
    if (!cmtdata)
        return;
    for (int i = cmtdata->getCount() - 1; i >= 0; --i)
    {
        Cmt_linedata *perline = (Cmt_linedata *) cmtdata->at(i);
        if (perline)
        {
            perline->freeAll();
            delete perline;
        }
    }
    delete cmtdata, cmtdata = NULL;
}

void Cmt::apply_comment(bool **comment_area, int DIM_Y, int DIM_X,
    CMT_DATA const &OPEN, CMT_DATA const &CLOSE,
    int iOpen, int iClose,
    int starty, int endy,
    int startx)
{
    int TEXT_X1 = OPEN.pos;
    int TEXT_Y1 = iOpen;
    int TEXT_X2 = CLOSE.pos;
    int TEXT_Y2 = iClose;

    for (int i = TEXT_Y1; i <= TEXT_Y2; i++)
    {
        int I = i - starty;
        if (I < 0)
            continue;
        if (I >= DIM_Y)
            break;
        int x1 = i == TEXT_Y1? TEXT_X1 : 0;
        int x2 = i == TEXT_Y2? TEXT_X2 : __npos;

        for (int j = 0; j < DIM_X; j++)
        {
            int test = j + startx;
            if (test >= x1 && test <= x2)
                comment_area[I][j] = true;
        }
    }
}

void Cmt::create_comment_area(bool **comment_area, int DIM_Y, int DIM_X, TNSCollection *cmtdata, zed &e, int eindex, int ft)
{
    assert(cmtdata && cmtdata->getCount() == editors->getCount());

    Cmt_linedata *perline = (Cmt_linedata *) cmtdata->at(eindex);
    if (perline == NULL)
        return;
    int purelinecount = perline->getCount();

    int starty = e.getcornery();
    int endy = starty + DIM_Y - 1;
    int startx = e.getcornerx();

    int open = 0;
    CMT_DATA OPEN, CLOSE;
    int iOpen, iClose;

    bool basta = false;

    for (int I = 0, i; I < purelinecount; I++)
    {
        Unpacked const &linedata = unpack_cmtline(i, (byte *) perline->at(I));
        int cmtcount = linedata.getcount();

        bool inside = i >= starty && i <= endy;
        bool below = i < starty;

        char const *symptr = linedata.get_sym_ptr();
        int const *posptr = linedata.get_pos_ptr();

        for (int j = 0; j < cmtcount; j++)
        {
            CMT_DATA CUR(symptr[j] == '{', posptr[j]);
            bool curopen = CUR.open;

            if (below)
            {
                if (curopen)
                {
                    if (open == 0)
                    {
                        open++;
                        OPEN = CUR, iOpen = i;
                    }
                }
                else
                {
                    open = 0;
                }
            }
            else /* !below */
            {
                if (inside)
                {
                    if (curopen)
                    {
                        if (open == 0)
                        {
                            open++;
                            OPEN = CUR, iOpen = i;
                        }
                    }
                    else
                    {
                        if (open)
                        {
                            open = 0;
                            CLOSE = CUR, iClose = i;
                            apply_comment(comment_area, DIM_Y, DIM_X, OPEN, CLOSE, iOpen, iClose, starty, endy, startx);
                        }
                    }
                }
                else /* above */
                {
                    basta = true;
                    break;
                }
            }
        }
        if (basta)
            break;
    }

    if (open)
    {
        CMT_DATA myCLOSE(false, 0);
        iClose = endy + 1;
        apply_comment(comment_area, DIM_Y, DIM_X, OPEN, myCLOSE, iOpen, iClose, starty, endy, startx);
    }
}

void Cmt::my_cb_insert(zed *e, int start, int n)
{
    if (editors == NULL)
        return;
    int eindex = editors->indexOf(e);
    if (eindex == ccNotFound)
        return;

    if (cmtdata)
    {
        assert(cmtdata->getCount() == editors->getCount());

        Cmt_linedata *perline = (Cmt_linedata *) cmtdata->at(eindex);
        if (perline)
        {
            int count = perline->getCount();
            if (!count)
                return;
            int a = start;
            int start = 0, end = count-1;
            while (end >= start)
            {
                int piv = (start+end)>>1;
                if (*((int*)perline->at(piv)) >= a)
                {
                    for (int j = piv; j <= end; j++)
                        *((int*)perline->at(j)) += n;
                    end = piv-1;
                    continue;
                }
                start = piv+1;
            }
        }
    }
}

void Cmt::my_cb_remove(zed *e, int start, int n)
{
    if (editors == NULL)
        return;
    int eindex = editors->indexOf(e);
    if (eindex == ccNotFound)
        return;

    if (cmtdata)
    {
        assert(cmtdata->getCount() == editors->getCount());

        Cmt_linedata *perline = (Cmt_linedata *) cmtdata->at(eindex);
        if (perline)
        {
            int count = perline->getCount();
            if (!count)
                return;
            int a = start, b = start+n-1;

#define X(I) ( *((int*)perline->at( (I) )) )

            {
                int start = 0, end = count-1;
                int alpha, beta, piv;

                if (X(start) > b || X(end) < a)
                    goto quit;

                if (X(start) >= a)
                    alpha = start;
                else
                {
                    while (end >= start)
                    {
                        piv = (start+end)>>1;
                        if (X(piv) < a)
                            start = piv+1;
                        else end = piv-1;
                    }
                    alpha = (start == piv+1)? piv+1 : piv;
                }

                start = 0, end = count-1;

                if (X(end) <= b)
                    beta = end;
                else
                {
                    while (end >= start)
                    {
                        piv = (start+end)>>1;
                        if (X(piv) > b)
                            end = piv-1;
                        else start = piv+1;
                    }
                    beta = (end == piv-1)? piv-1 : piv;
                }

                assert(alpha >= 0 && beta < count);

                if (alpha > beta)
                {
                    assert(X(alpha) > b);
                    assert(X(beta) < a);
                    goto quit;
                }

                assert(X(alpha) >= a && X(beta) <= b);

                perline->atFree(alpha, beta-alpha+1);
                count = perline->getCount();
            }
quit:
            {
                int start = 0, end = count-1;
                while (end >= start)
                {
                    int piv = (start+end)>>1;
                    if (X(piv) > b)
                    {
                        for (int j = piv; j <= end; j++)
                            X(j) -= n;
                        end = piv-1;
                        continue;
                    }
                    start = piv+1;
                }
            }
#undef X
        }
    }
}

Unpacked const &Cmt::create_cmtline(zed *e, int i, int ft)
{
    static Unpacked u(2);

    static CToken::CT_GETNEXT const GETNEXT[] = {
#ifdef FREE
        &CToken::GetNextPair,
        NULL,                           /* Nasm */
        NULL,                           /* c-- */
        &CToken::GetNextPair_Pascal,
        &CToken::GetNextPair,
        NULL,                           /* Haskell */
        NULL
#else
        &CToken::GetNextPair,
        NULL,                           /* Nasm */
        &CToken::GetNextPair_CMinus,
        &CToken::GetNextPair_Pascal,
        &CToken::GetNextPair,
        &CToken::GetNextPair_Haskell,
        NULL
#endif
    };

    int cmtcount = 0;
    _string const s(const_cast<char *>(e->at(i)));
    if (s.getwidth() == 0)
        goto quit;
    {
        bool cmode = ft == ftC || ft == ftIDC || ft == ftCmm;
        if (cmode)
        {
            if (!s.contains('*'))
                goto quit;
            if (!s.contains('/'))
                goto quit;
        }
    }

    {
        CToken::CT_GETNEXT const getnext = GETNEXT[ft];
        assert(getnext);
        int x1, x2, x3, T;
        CToken tok(s);

        while (true)
        {
            if ((tok.*getnext)(x1, x2, x3, T))
                break;

            if (T == 0x400 || T == 0x800 || T == 0x1000)
            {
                int next = cmtcount + 2;
                if (next > u.size)
                {
                    int size = u.size << 1;
                    while (next > size)
                        size <<= 1;
                    u.update_size(size, Unpacked::UPDATE_COPY);
                }
            }
            else continue;

            char *symptr = u.u_sym;
            int *posptr = u.u_pos;

            if (T == 0x400)         /* multi-line in one line */
            {
                symptr[cmtcount+0] = '{';
                symptr[cmtcount+1] = '}';
                posptr[cmtcount+0] = x1;
                posptr[cmtcount+1] = x2;
                cmtcount += 2;
            }
            else if (T == 0x800)    /* multi-line open */
            {
                symptr[cmtcount+0] = '{';
                posptr[cmtcount+0] = x1;
                cmtcount++;
            }
            else /*0x1000*/         /* multi-line close */
            {
                symptr[cmtcount+0] = '}';
                posptr[cmtcount+0] = x2;
                cmtcount++;
            }
        }
    }

quit:
    u.count = cmtcount;

    return u;
}

void Cmt::my_cb_analize(zed *e, int start, int end) // be careful, it may change the caller context (e.g. zed::myat())
{
    if (editors == NULL)
        return;
    int eindex = editors->indexOf(e);
    if (eindex == ccNotFound)
        return;

    if (cmtdata)
    {
        assert(cmtdata->getCount() == editors->getCount());

        Cmt_linedata *perline = (Cmt_linedata *) cmtdata->at(eindex);
        if (perline)
        {
            int ft = getfiletype(e->getfilename());
            for (int i = start; i <= end; i++)
            {
                int index;
                bool found = (bool) perline->search(&i, index);/**/
                if (found)
                    perline->atFree(index);
                Unpacked const &linedata = create_cmtline(e, i, ft);
                if (linedata.getcount())
                {
                    byte *packed = pack_cmtline(i, linedata);
                    if (!found)
                        perline->insert(packed);
                    else perline->atInsert(index, packed);
                    /*todo: atFree/atInsert pair too slow*/
                }
            }
        }
    }
}

void Cmt::init_cmt_data(TNSCollection *cmtdata, zed *e, bool firsttime, bool justdestroy)
{
    assert(cmtdata);

    int eindex = editors->indexOf(e);
    bool found = eindex != ccNotFound && !firsttime;
    if (found)
    {
        Cmt_linedata *perline = (Cmt_linedata *) cmtdata->at(eindex);
        if (perline)
        {
            perline->freeAll();
            delete perline;
            cmtdata->atPut(eindex, NULL);
        }
        if (justdestroy)
            return;
    }

    int const N_LINES = e->getlinecount();
    int const N_EDITORS = cmtdata->getCount();

    int ft = getfiletype(e->getfilename());

    switch (ft)
    {
        case ftC:
        case ftIDC:
        case ftPascal:
            break;
        case ftHaskell:
        case ftCmm:
#ifndef FREE
            break;
#else
            // fall thru
#endif
        default:
            if (!found)
                cmtdata->atInsert(N_EDITORS, NULL);
            return;
    }

    Cmt_linedata *perline = new Cmt_linedata(2, 2);

    for (int i = 0, count = 0; i < N_LINES; i++)
    {
        Unpacked const &linedata = create_cmtline(e, i, ft);
        if (linedata.getcount())
            perline->atInsert(count++, pack_cmtline(i, linedata));
    }

    if (!found)
        cmtdata->atInsert(N_EDITORS, perline);
    else cmtdata->atPut(eindex, perline);
}
