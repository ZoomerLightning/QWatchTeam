#ifndef __CMT_H__
#define __CMT_H__

#include <string.h>

#include "tcollect.h"
#include "mystring.h"
#include "tok.h"
#include "zedclass.h"
#include "zeddef.h"
#include "pmt.h"

extern int getfiletype(char const *filename);
extern TNSCollection *editors;

class Cmt_linedata : public TNSSortedCollection
{
public:
    Cmt_linedata(ccIndex aLimit, ccIndex aDelta) :
        TNSSortedCollection(aLimit, aDelta)
    {}

private:
    virtual int calculate_limit(int aLimit, int aDelta) const
    {
        return aDelta==2? aLimit << 1 : aLimit * aDelta;
    }
    virtual int compare(void const *key1, void const *key2)
    {
        int x = *((int *) key1);
        int y = *((int *) key2);
        return x < y ? -1 : (x==y? 0 : 1);/**/
    }
    virtual void freeItem(void *item)
    {
        if (item)
            delete[] (byte *) item;
    }
};

class Cmt;

class Unpacked
{
    friend class Cmt;
public:
    Unpacked(int init_size, bool ext = false) :
        use_external_tables(ext),
        u_sym(NULL),
        u_pos(NULL),
        size(0),
        count(0),
        ext_sym(NULL),
        ext_pos(NULL)
    {
        update_size(init_size, UPDATE_FAST);
    }
    ~Unpacked()
    {
        destroy();
    }

    char const *const get_sym_ptr() const
    {
        return use_external_tables? ext_sym : u_sym;
    }

    int const *const get_pos_ptr() const
    {
        return use_external_tables? ext_pos : u_pos;
    }

    int getcount() const
    {
        return count;
    }

private:
    void destroy()
    {
        if (u_sym)
            delete[] u_sym, u_sym = 0;
        if (u_pos)
            delete[] u_pos, u_pos = 0;
    }

    static bool const
        UPDATE_FAST = false,
        UPDATE_COPY = true;

    void update_size(int newsize, bool copy)
    {
        if (newsize > size)
        {
            if (copy)
            {
                char *t_sym = new char[newsize];
                int *t_pos = new int[newsize];
                if (size)
                {
                    memcpy(t_sym, u_sym, size * sizeof(char));
                    memcpy(t_pos, u_pos, size * sizeof(int));
                    destroy();
                }
                u_sym = t_sym;
                u_pos = t_pos;
            }
            else
            {
                destroy();
                u_sym = new char[newsize];
                u_pos = new int[newsize];
            }
            size = newsize;
        }
    }

private:
    bool use_external_tables;

    char *u_sym;
    int *u_pos;
    int size;
    int count;

    char *ext_sym;
    int *ext_pos;
};

class Cmt
{
    friend class Unpacked;
public:
    static TNSCollection *cmtdata;

    Cmt()           {}
    ~Cmt()          {}

    static void init_cmt_data(TNSCollection *cmtdata, zed *e, bool firsttime, bool justdestroy = false);
    static void my_cb_insert(zed *e, int start, int n);
    static void my_cb_remove(zed *e, int start, int n);
    static void my_cb_analize(zed *e, int start, int end);
    static void create_comment_area(bool **comment_area,
        int DIM_Y, int DIM_X,
        TNSCollection *cmtdata,
        zed &e, int eindex,
        int ft);

#ifndef NDEBUG
    static void dump_cmt_data(int eindex);
#endif
    static void destroy_cmtdata();

private:
    struct CMT_DATA
    {
#define __PACKED__ /*__attribute__(__packed__)*/
        bool    open    __PACKED__;
        int     pos     __PACKED__;
#undef __PACKED__
    public:
        CMT_DATA() : open(false), pos(0)
        {}
        CMT_DATA(bool O, int P) : open(O), pos(P)
        {}
    };

    static byte const
        CMT_BIT_COUNT = 1|2,
        CMT_BIT_STOR_COUNT = 4|8,
        CMT_BIT_STOR_POS = 0x10|0x20,
        CMT_BIT_I_PAREN = 0x40,
        CMT_BIT_II_PAREN = 0x80;

    static int const
        S_CMT_BIT_COUNT = 0,
        S_CMT_BIT_STOR_COUNT = 2,
        S_CMT_BIT_STOR_POS = 4,
        S_CMT_BIT_I_PAREN = 6,
        S_CMT_BIT_II_PAREN = 7;

    static byte const
        T_CMT_BIT_COUNT_1 = (byte) (0 << S_CMT_BIT_COUNT) & CMT_BIT_COUNT,
        T_CMT_BIT_COUNT_2 = (byte) (1 << S_CMT_BIT_COUNT) & CMT_BIT_COUNT,
        T_CMT_BIT_COUNT_BE8 = (byte) (2 << S_CMT_BIT_COUNT) & CMT_BIT_COUNT,
        T_CMT_BIT_COUNT_A8 = (byte) (3 << S_CMT_BIT_COUNT) & CMT_BIT_COUNT,

        T_CMT_BIT_STOR_COUNT_B = (byte) (0 << S_CMT_BIT_STOR_COUNT) & CMT_BIT_STOR_COUNT,
        T_CMT_BIT_STOR_COUNT_W = (byte) (1 << S_CMT_BIT_STOR_COUNT) & CMT_BIT_STOR_COUNT,
        T_CMT_BIT_STOR_COUNT_D = (byte) (2 << S_CMT_BIT_STOR_COUNT) & CMT_BIT_STOR_COUNT,

        T_CMT_BIT_STOR_POS_B = (byte) (0 << S_CMT_BIT_STOR_POS) & CMT_BIT_STOR_POS,
        T_CMT_BIT_STOR_POS_W = (byte) (1 << S_CMT_BIT_STOR_POS) & CMT_BIT_STOR_POS,
        T_CMT_BIT_STOR_POS_D = (byte) (2 << S_CMT_BIT_STOR_POS) & CMT_BIT_STOR_POS;

    static void apply_comment(bool **comment_area, int DIM_Y, int DIM_X,
        CMT_DATA const &OPEN, CMT_DATA const &CLOSE,
        int iOpen, int iClose,
        int starty, int endy,
        int startx);

    static Unpacked const   &create_cmtline(zed *e, int i, int ft);
    static byte             *pack_cmtline(int index, Unpacked const &linedata);
    static Unpacked const   &unpack_cmtline(int &index, byte *linedata);
};

#endif //__CMT_H__
