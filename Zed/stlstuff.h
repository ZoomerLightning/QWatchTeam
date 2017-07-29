#ifndef __STLSTUFF_H__
#define __STLSTUFF_H__

enum
{
    PTR_Define = 0,
    PTR_History
};

extern "C++"
{
    void ext_init();
    int getfiletype(char const *filename);
    void *ptr_add(int type, void *ptr);
    int ptr_count(int type);
    void *ptr_get(int type, int n);
};

#endif //__STLSTUFF_H__
