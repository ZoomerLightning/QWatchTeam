/*
** readconf.h
**
** Defines header for config file processing function.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __READCONF_H__
#define __READCONF_H__

#include "zeddef.h"
#include "mystring.h"

class Define
{
public:
    String V1;
    String V2;
    Define(String const &v1, String const &v2) :
        V1(v1),
        V2(v2)
    {}
    ~Define()
    {}
};

extern "C++"
{
    int readconfig(char *argv, zedConfig &C);
}

#endif //__READCONF_H__
