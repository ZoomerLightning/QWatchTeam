/*
** myfile.cpp
**
** Defines some externs for "myfile.h".
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#include "myfile.h"

int FileException::mask = 0xffff & ~(_feClose);

String getcomponent(char const *fullpath, int t)
{
    char drive[myMAXDRIVE] = {0};
    char dir[myMAXDIR] = {0};
    char fname[myMAXFILE] = {0};
    char ext[myMAXEXT] = {0};
    mysplitpath(fullpath, drive, dir, fname, ext);
    String component = 0;
    if (t & 1)
        component += drive;
    if (t & 2)
        component += dir;
    if (t & 4)
        component += fname;
    if (t & 8)
        component += ext;
    return component;
}

void Fold_Filename(String &f, int Req_l)
{
    if (f.getwidth() <= Req_l)
        return;

    String drive = getcomponent(f.c_str(), 1);
    String dir = getcomponent(f.c_str(), 2);
    String fname = getcomponent(f.c_str(), 4 + 8);

    dir.replaceall('/', '\\');

    int L1 = drive.getwidth();
    int L3 = fname.getwidth();

    if (dir.freq('\\') > 0)
    {
        int pos1, pos2;
        bool virgin = true;
        while (1)
        {
            if (virgin)
            {
                pos1 = -1;
                pos2 = dir.search('\\', pos1 + 1);
                if (pos2 > 0)
                {
                    dir.remove(pos1 + 1, pos2 - pos1 - 1);
                    dir.insert(pos1 + 1, "...");
                }
                virgin = false;
            }
            else
            {
                pos1 = dir.search('\\', 0);
                if (pos1 == -1)
                    break;
                pos2 = dir.search('\\', pos1 + 1);
                if (pos2 == -1)
                    break;
                if (pos2 - pos1 < 2)
                {
                    dir[pos1] = '\1';
                    continue;
                }
                dir.remove(pos1 + 1, pos2 - pos1 - 1);
                dir.insert(pos1 + 1, "...");
                dir[pos1] = '\1';
            }
            if (L1 + dir.getwidth() + L3 <= Req_l)
                break;
        }
        dir.replaceall('\1', '\\');
    }
    int L2 = dir.getwidth();
    if (L1 + L2 + L3 <= Req_l)
        f = drive + dir + fname;
    else if (L2 + L3 <= Req_l)
        f = dir + fname;
    else if (L1 + L3 <= Req_l)
        f = drive + fname;
    else if (L3 <= Req_l)
        f = fname;
    else
    {
        f = fname;
        f.clip(Req_l);
    }
}
