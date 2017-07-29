/*
** zeddef.h
**
** Defines some stuff for ZED main module.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __ZEDDEF_H__
#define __ZEDDEF_H__

#include "pmt.h"

typedef struct
{
    bool    highlight;
    bool    autobrackets;
    bool    multilinecomments;
    bool    analize_rename;
    bool    cache_video;
    bool    vsync;
    bool    block_insert;
    bool    unix_end;
    bool    exit_cleanup;

    bool    debug;

    bool    autoindent;
    bool    cursorbeyondend;
    int     tab_size;
    int     tab_size_asm;
    int     hardtabsize;
    bool    autotabsizeonload;
    bool    persistent;
    bool    esc_exits;
    bool    grayenter_macro;
    bool    brief_home;

    int     calcoptype;
    int     calcmpdigits;
    bool    calc_callback;
    bool    multichar_nasm;

    char    status_spec[128];
    char    status_time_format_short[64];
    char    status_time_format_long[64];

    BYTE    atNormalText;
    BYTE    atSelect;
    BYTE    atStatusLine;
    BYTE    atMainFrame;

    BYTE    atKeyWord;
    BYTE    atFunction;
    BYTE    atSpecial;
    BYTE    atLabel;
    BYTE    atSyntax;
    BYTE    atInvalid;

    BYTE    atOctal;
    BYTE    atHex;
    BYTE    atDecimal;
    BYTE    atFloat;
    BYTE    atBinary;

    BYTE    atChar;
    BYTE    atString;
    BYTE    atPreprocess;
    BYTE    atPreprocessUnknown;
    BYTE    atStdinclude;
    BYTE    atComment;

    BYTE    atTitle;

    BYTE    atMBNormal;
    BYTE    atMBSave;
    BYTE    atMBHelp;
    BYTE    atMBError;

    BYTE    atRulerN;
    BYTE    atRulerI;

} zedConfig;

#if defined(__WATCOMC__)
typedef struct
{
    unsigned deverr;
    unsigned errcode;
    unsigned far *devhdr;
} __HardError;

extern __HardError HE;
#endif

#define prgname "zed"
#define uprgname "ZED"

#define SCREEN 0, 0, SMX - 1, SMY - 1

#define MAINHELP \
    "<common>                        |<navigate>", \
    "@f5     user screen             |^up, ^down         screen move vert", \
    "esc, @x, f10    quit            |#f4, #f5           screen move horiz", \
    "#f10    save all modif. & quit  |@pgup, @pgdn       up/down 100 lines", \
    "#f9     about                   |^pgup, ^pgdn       start/end of file", \
    "f8      calculator              |@home, @end        start/end of page", \
    "f4, ^f4    ruler                |@slash             sync page with cur. line", \
    "@j/^j   shell                   |@point             center current line", \
    "^f1     context help (gnu info) |@g / ^g / @f8      goto line / column / enh.", \
    "                                |f7                 search", \
    "<files>-------------------------|#f7, ^L / @f7      repeat search / reverse", \
    "f6, @right    next file         |<edit>---------------------------------------", \
    "#f6, @left    prev file         |@L                 drop anchor", \
    "f2            save current      |^s..^f             start/finish selection", \
    "^f2           save all modif.   |^f7                search and replace", \
    "^g0-^g9       switch file       |^y                 delete current line", \
    "#f2           save as..         |f5                 duplicate line", \
    "@f2           save ALL          |^f5, ^n, ^enter    insert empty line", \
    "@0, f12       file list         |^z                 delete to eol", \
    "--------------------------------|#tab               backward tab", \
    "^[]\\    change case of word     |@a                 change tab size", \
    "^t, ^bksp delete word           |@w/^w              save block/clipboard", \
    "@t       select word            |^i/u, @i/u         block indenting",


#define INPUTSIG1 "???"
#define INPUTSIG2 "***"

int const
    ftUndef     = -1,
    ftC         = 0,
    ftASM       = 1,
    ftCmm       = 2,
    ftPascal    = 3,
    ftIDC       = 4,
    ftHaskell   = 5,
    ftADA       = 6;

#endif //__ZEDDEF_H__
