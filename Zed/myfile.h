/*
** myfile.h
**
** Defines class 'File' and some exceptions for it.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __MYFILE_H__
#define __MYFILE_H__

#include <stdio.h>
#include <io.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int const cMaxFileNameLength = 260;

#include <direct.h>
#define myMAXPATH       1024
#define myMAXDRIVE      3
#define myMAXDIR        1024
#define myMAXFILE       1024
#define myMAXEXT        1024
#define mysplitpath     _splitpath
#define mychdir(path)   chdir((path))
#define mygetwd(wd,g)   getcwd((wd), (g))

#include "mystring.h"
#include "strdup.h"

extern "C++"
{
    String getcomponent(char const *fullpath, int t);
    void Fold_Filename(String &f, int Req_l);
}

class FileException;
class File;

class FileOpenException;
class FileReadException;
class FileWriteException;
class FileSeekException;
class FileCloseException;
class FileBufException;

typedef enum feState {
    _feOpen = 1, _feRead = 2, _feWrite = 4, _feSeek = 8, _feBuf = 16, _feClose = 32,
    _feInvalid = 0
} _feState;

class __errorinfo
{
public:
    int     errcode;
        int olderror;
};

class FileException
{
public:
    char FileName[cMaxFileNameLength];

    _feState        __cause;
    __errorinfo     __e;

    static int      mask;

    FileException(char const *__filename) :
        __cause(_feInvalid)
    {
        if (!__filename)
            *FileName = '\0';
        else strcpy(FileName, __filename);
        __e.errcode     = errno;
    }
};

class FileOpenException : public FileException
{
public:
    FileOpenException(char const *__filename) :
        FileException(__filename)
    {__cause = _feOpen;}
};

class FileReadException : public FileException
{
public:
    FileReadException(char const *__filename) :
        FileException(__filename)
    {__cause = _feRead;}
};

class FileWriteException : public FileException
{
public:
    FileWriteException(char const *__filename) :
        FileException(__filename)
    {__cause = _feWrite;}
};

class FileSeekException : public FileException
{
public:
    FileSeekException(char const *__filename) :
        FileException(__filename)
    {__cause = _feSeek;}
};

class FileCloseException : public FileException
{
public:
    FileCloseException(char const *__filename) :
        FileException(__filename)
    {__cause = _feClose;}
};

class FileBufException : public FileException
{
public:
    FileBufException(char const *__filename) :
        FileException(__filename)
    {__cause = _feBuf;}
};

class File
{
    FILE    *f;
    char    *FileName;
public:
    File() :
        f(NULL),
        FileName(NULL)
    {}
    File(char const *__filename, char const *__mode) :
        f(NULL),
        FileName(NULL)
    {
        __open(__filename, __mode);
    }
    File(FILE *__f) :
        f(__f),
        FileName(NULL)
    {}
    /*virtual*/ ~File() {__close();}
    FILE *operator () (void) {return f;}
    FILE *getptr() {return f;}
    char const *getfilename() {return FileName;}
    int valid() {return f != NULL;}
    int invalid() {return f == NULL;}
    void GetExtendedError()
    {
    }
    int __open(char const *__filename, char const *__mode)
    {
        __close();
        f = ::fopen(__filename, __mode);
        if (FileException::mask & _feOpen)
        {
            if (!f)
            {
                GetExtendedError();
                throw FileOpenException(__filename);
            }
        }
        if (f)
        {
            FileName = mystrdup(__filename);
            return 1;
        }
        else return 0;
    }
    void __unlink()
    {
        if (f && FileName)
        {
            clearerr(f);
            ::fclose(f);
            f = NULL;
            ::unlink(FileName);
            delete[] FileName, FileName = NULL;
        }
    }
    void __close()
    {
        if (FileName)
            delete[] FileName, FileName = NULL;
        if (f)
        {
            clearerr(f);
            int t = ::fclose(f);
            f = NULL;
            if (FileException::mask & _feClose)
            {
                if (t == EOF)
                {
                    GetExtendedError();
                    throw FileCloseException("<Name Deleted>");
                }
            }
        }
    }

    int fgetc()
        {return fgetc(f);}
    char *fgets(char *__s, int __n)
        {return fgets(__s, __n, f);}
    int fputc(int __c)
        {return fputc(__c, f);}
    int fputs(char const *__s)
        {return fputs(__s, f);}
    size_t fread(void *__ptr, size_t __size, size_t __n = 1)
        {return fread(__ptr, __size, __n, f);}
    int fseek(long int __offset, int __whence)
        {return fseek(f, __offset, __whence);}
    size_t fwrite(void const *__ptr, size_t __size, size_t __n = 1)
        {return fwrite(__ptr, __size, __n, f);}
    int setvbuf(char *__buf, int __mode, size_t __size)
        {return setvbuf(f, __buf, __mode, __size);}

    int fgetc(FILE *__fp)
    {
        int t = ::fgetc(__fp);
        if (FileException::mask & _feRead)
        {
            if (ferror(__fp))
            {
                GetExtendedError();
                throw FileReadException(FileName);
            }
        }
        return t;
    }
    char *fgets(char *__s, int __n, FILE *__fp)
    {
        char *t = ::fgets(__s, __n, __fp);
        if (FileException::mask & _feRead)
        {
            if (ferror(__fp))
            {
                GetExtendedError();
                throw FileReadException(FileName);
            }
        }
        return t;
    }
    int fputc(int __c, FILE *__fp)
    {
        int t = ::fputc(__c, __fp);
        if (FileException::mask & _feWrite)
        {
            if (t == EOF && ferror(__fp))
            {
                GetExtendedError();
                throw FileWriteException(FileName);
            }
        }
        return t;
    }
    int fputs(char const *__s, FILE *__fp)
    {
        int t = ::fputs(__s, __fp);
        if (FileException::mask & _feWrite)
        {
            if (t == EOF && ferror(__fp))
            {
                GetExtendedError();
                throw FileWriteException(FileName);
            }
        }
        return t;
    }
    size_t fread(void *__ptr, size_t __size, size_t __n, FILE *__fp)
    {
        size_t t = ::fread(__ptr, __size, __n, __fp);
        if (FileException::mask & _feRead)
        {
            if (t != __n && ferror(__fp))
            {
                GetExtendedError();
                throw FileReadException(FileName);
            }
        }
        return t;
    }
    int fseek(FILE *__fp, long int __offset, int __whence)
    {
        int t = ::fseek(__fp, __offset, __whence);
        if (FileException::mask & _feSeek)
        {
            if (t != 0)
            {
                GetExtendedError();
                throw FileSeekException(FileName);
            }
        }
        return t;
    }
    size_t fwrite(void const *__ptr, size_t __size, size_t __n, FILE *__fp)
    {
        size_t t = ::fwrite(__ptr, __size, __n, __fp);
        if (FileException::mask & _feWrite)
        {
            if (t != __n && ferror(__fp))
            {
                GetExtendedError();
                throw FileWriteException(FileName);
            }
        }
        return t;
    }
    int setvbuf(FILE *__fp, char *__buf, int __mode, size_t __size)
    {
        int t = ::setvbuf(__fp, __buf, __mode, __size);
        if (FileException::mask & _feBuf)
        {
            if (t != 0)
            {
                GetExtendedError();
                throw FileBufException(FileName);
            }
        }
        return t;
    }
};


inline char const *__perror(__errorinfo &e)
{
    static char buf[256];
    sprintf(buf, "%s [%s]", "unknown error", strerror(e.olderror));
    return buf;
}

#define F_OK 0

inline bool fexist(char const *filename)
{
    return access(filename, F_OK) == 0;
}

#endif //__MYFILE_H__
