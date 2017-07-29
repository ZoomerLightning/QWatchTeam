#ifndef __DYNAMICARRAY_H__
#define __DYNAMICARRAY_H__

template <class atomic> void alloc_ar(atomic** &pnew, int left, int right)
{
    atomic **p = new atomic *[left],
        *buf = new atomic[left * right];
    for (int i = 0; i < left; ++i)
    {
        p[i] = buf;
        buf += right;
    }
    pnew = p;
}

template <class atomic> void free_ar(atomic** &p)
{
    delete[] p[0];
    delete[] p;
    p = 0;
}

template <class atomic> void fill_ar(atomic** p, int left, int right, atomic stuff)
{
    int i = 0, j;
    for (; i < left; ++i)
    {
        for (j = 0; j < right; ++j)
            p[i][j] = stuff;
    }
}

#endif //__DYNAMICARRAY_H__
