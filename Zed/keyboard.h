/*
** keyboard.h
**
** Defines key-codes & some high-level keyboard related functions.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#define KEYBOARD_UTIL

int const
    kbCTRL_BREAK        = 0xFFFF,
    kbNothing           = 0,

    kbEscape            = 0x1b,
/*    kbAltEscape         = 0x0100,*/ //fix

/*
 *  Win95 reserved: Alt-Tab,
 *                  Ctrl-Esc,
 *                  Alt-PrtSc,
 *                  Alt-Space,
 *                  Alt-Esc,
 *                  PrtSc,
 *                  Alt-Enter
 *
 */

//fix
/*    kbCtrlSysRq         = 0x7200,
    kbShiftTab          = 0x9,*/

    kbF1                = 0x3b00,
    kbF2                = 0x3c00,
    kbF3                = 0x3d00,
    kbF4                = 0x3e00,
    kbF5                = 0x3f00,
    kbF6                = 0x4000,
    kbF7                = 0x4100,
    kbF8                = 0x4200,
    kbF9                = 0x4300,
    kbF10               = 0x4400,
    kbF11               = 0x8500,
    kbF12               = 0x8600,
    kbBackSpace         = 0x8,
    kbInsert            = 0x52e0,
    kbHome              = 0x47e0,
    kbPageUp            = 0x49e0,
    kbTab               = 0x9,
    kbEnter             = 0xd,
    kbDelete            = 0x53e0,
    kbEnd               = 0x4fe0,
    kbPageDown          = 0x51e0,
    kbUp                = 0x48e0,
    kbLeft              = 0x4be0,
    kbDown              = 0x50e0,
    kbRight             = 0x4de0,
    kbGraySlash         = 0x2f,
    kbGrayAsterisk      = 0x2a,
    kbGrayMinus         = 0x2d,
    kbGrayPlus          = 0x2b,
    kbGrayEnter         = 0xd,
    kbGrayHome          = 0x4700,
    kbGrayUp            = 0x4800,
    kbGrayPageUp        = 0x4900,
    kbGrayLeft          = 0x4b00,
/*    kbGrayCenter        = 0x4c00,*/ //fix
    kbGrayRight         = 0x4d00,
    kbGrayEnd           = 0x4f00,
    kbGrayDown          = 0x5000,
    kbGrayPageDown      = 0x5100,
    kbGrayIns           = 0x5200,
    kbGrayDel           = 0x5300,

//fix
/*    kbGraySeven         = 0x4737,
    kbGrayEight         = 0x4838,
    kbGrayNine          = 0x4939,
    kbGrayFour          = 0x4b34,
    kbGrayFive          = 0x4c35,
    kbGraySix           = 0x4d36,
    kbGrayOne           = 0x4f31,
    kbGrayTwo           = 0x5032,
    kbGrayThree         = 0x5133,
    kbGrayZero          = 0x5230,
    kbGrayPoint         = 0x532e,*/

    kbAltF1             = 0x6800,
    kbAltF2             = 0x6900,
    kbAltF3             = 0x6a00,
    kbAltF4             = 0x6b00,
    kbAltF5             = 0x6c00,
    kbAltF6             = 0x6d00,
    kbAltF7             = 0x6e00,
    kbAltF8             = 0x6f00,
    kbAltF9             = 0x7000,
    kbAltF10            = 0x7100,
    kbAltF11            = 0x8be0,
    kbAltF12            = 0x8ce0,

/*    kbAltBackSpace      = 0x0e00,*/ //fix

    kbAltInsert         = 0xa200,//ok
    kbAltHome           = 0x9700,
    kbAltPageUp         = 0x9900,

//fix
/*    kbAltTab            = 0xa500,
    kbAltEnter          = 0x1c00,*/ 


    kbAltDelete         = 0xa300,
    kbAltEnd            = 0x9f00,
    kbAltPageDown       = 0xa100,
    kbAltUp             = 0x9800,
    kbAltLeft           = 0x9b00,
    kbAltDown           = 0xa000,
    kbAltRight          = 0x9d00,

//fix
/*    kbAltGraySlash      = 0xa400,
    kbAltGrayAsterisk   = 0x3700,
    kbAltGrayMinus      = 0x4a00,
    kbAltGrayPlus       = 0x4e00,
    kbAltGrayEnter      = 0xa600,*/

    kbCtrlF1            = 0x5e00,
    kbCtrlF2            = 0x5f00,
    kbCtrlF3            = 0x6000,
    kbCtrlF4            = 0x6100,
    kbCtrlF5            = 0x6200,
    kbCtrlF6            = 0x6300,
    kbCtrlF7            = 0x6400,
    kbCtrlF8            = 0x6500,
    kbCtrlF9            = 0x6600,
    kbCtrlF10           = 0x6700,
    kbCtrlF11           = 0x89e0,
    kbCtrlF12           = 0x8ae0,

    kbCtrlBackSpace     = 0x7f,
//ok
    kbCtrlInsert        = 0x92e0,
    kbCtrlHome          = 0x77e0,
    kbCtrlPageUp        = 0x86e0,
    kbCtrlTab           = 0x9400,

//fix
/*    kbCtrlEnter         = 0x1c0a,*/


    kbCtrlDelete        = 0x93e0,
    kbCtrlEnd           = 0x75e0,
    kbCtrlPageDown      = 0x76e0,
    kbCtrlUp            = 0x8de0,
    kbCtrlLeft          = 0x73e0,
    kbCtrlDown          = 0x91e0,
    kbCtrlRight         = 0x74e0,

    kbCtrlGraySlash     = 0x9500,

//fix
/*    kbCtrlGrayAsterisk  = 0x9600,
    kbCtrlGrayMinus     = 0x8e00,
    kbCtrlGrayPlus      = 0x9000,
    kbCtrlGrayEnter     = 0xe00a,
    kbCtrlGraySeven     = 0x7700,
    kbCtrlGrayEight     = 0x8d00,
    kbCtrlGrayNine      = 0x8400,
    kbCtrlGrayFour      = 0x7300,
    kbCtrlGrayFive      = 0x8f00,
    kbCtrlGraySix       = 0x7400,
    kbCtrlGrayOne       = 0x7500,
    kbCtrlGrayTwo       = 0x9100,
    kbCtrlGrayThree     = 0x7600,
    kbCtrlGrayZero      = 0x9200,
    kbCtrlGrayPoint     = 0x9300,*/

    kbShiftF1           = 0x5400,
    kbShiftF2           = 0x5500,
    kbShiftF3           = 0x5600,
    kbShiftF4           = 0x5700,
    kbShiftF5           = 0x5800,
    kbShiftF6           = 0x5900,
    kbShiftF7           = 0x5a00,
    kbShiftF8           = 0x5b00,
    kbShiftF9           = 0x5c00,
    kbShiftF10          = 0x5d00,
    kbShiftF11          = 0x87e0,
    kbShiftF12          = 0x88e0,//ok




////////////////////////////////////////////////////////////////////////////////////////////////////
    kbCtrlTwo           = 0x300,
    kbCtrlBackSlash     = 0x1c,

//ok


////////////////////////////////////////////////////////////////////////////////////////////////////



    kbCtrlQ             = 0x11,
    kbCtrlW             = 0x17,
    kbCtrlE             = 0x5,
    kbCtrlR             = 0x12,


    kbCtrlT             = 0x14,
    kbCtrlY             = 0x19,
    kbCtrlU             = 0x15,
    kbCtrlI             = 0x9,
    kbCtrlO             = 0xf,
    kbCtrlP             = 0x10,


    kbCtrlOpenBracket   = 0x1b,
    kbCtrlCloseBracket  = 0x1d,//ok


    kbCtrlA             = 0x1,
    kbCtrlF             = 0x6,//ok


    kbCtrlG             = 0x7,


    kbCtrlH             = 0x8,
    kbCtrlJ             = 0xa,


    kbCtrlK             = 0xb,
    kbCtrlL             = 0xc,


    kbCtrlZ             = 0x1a,
    kbCtrlX             = 0x18,


    kbCtrlV             = 0x16,


    kbCtrlB             = 0x2,
    kbCtrlN             = 0xe,
    kbCtrlM             = 0xd, //ok


//fix
/*
    kbAltQ              = 0x1000,
    kbAltW              = 0x1100,
    kbAltE              = 0x1200,
    kbAltR              = 0x1300,
    kbAltT              = 0x1400,
    kbAltY              = 0x1500,
    kbAltU              = 0x1600,
    kbAltI              = 0x1700,
    kbAltO              = 0x1800,
    kbAltP              = 0x1900,
    kbAltA              = 0x1e00,
    kbAltS              = 0x1f00,
    kbAltD              = 0x2000,
    kbAltF              = 0x2100,
    kbAltG              = 0x2200,
    kbAltH              = 0x2300,
    kbAltJ              = 0x2400,
    kbAltK              = 0x2500,
    kbAltL              = 0x2600,
    kbAltZ              = 0x2c00,
    kbAltX              = 0x2d00,
    kbAltC              = 0x2e00,
    kbAltV              = 0x2f00,
    kbAltB              = 0x3000,
    kbAltN              = 0x3100,
    kbAltM              = 0x3200,*/


////////////////////////////////////////////////////////////////////////////////////////////////////


    kbWhiteSpace        = 0x20,

    kbBackQuot          = 0x60, // `
    kbOne               = 0x31,
    kbTwo               = 0x32,
    kbThree             = 0x33,
    kbFour              = 0x34,
    kbFive              = 0x35,
    kbSix               = 0x36,
    kbSeven             = 0x37,
    kbEight             = 0x38,
    kbNine              = 0x39,
    kbZero              = 0x30,
    kbMinus             = 0x2d,
    kbEqual             = 0x3d,
    kbPlus              = kbEqual,
    kbBackSlash         = 0x5c,
    kbQ                 = 0x71,
    kbW                 = 0x77,
    kbE                 = 0x65,
    kbR                 = 0x72,
    kbT                 = 0x74,
    kbY                 = 0x79,
    kbU                 = 0x75,
    kbI                 = 0x69,
    kbO                 = 0x6f,
    kbP                 = 0x70,
    kbOpenBracket       = 0x5b,
    kbCloseBracket      = 0x5d,
    kbA                 = 0x61,
    kbS                 = 0x73,
    kbD                 = 0x64,
    kbF                 = 0x66,
    kbG                 = 0x67,
    kbH                 = 0x68,
    kbJ                 = 0x6a,
    kbK                 = 0x6b,
    kbL                 = 0x6c,
    kbPointComma        = 0x3b,
    kbQuot              = 0x27, // '
    kbZ                 = 0x7a,
    kbX                 = 0x78,
    kbC                 = 0x63,
    kbV                 = 0x76,
    kbB                 = 0x62,
    kbN                 = 0x6e,
    kbM                 = 0x6d,
    kbComma             = 0x2c,
    kbPoint             = 0x2e,
    kbSlash             = 0x2f,

    kbShiftBackQuot     = 0x7e, // `
    kbShiftOne          = 0x21,
    kbShiftTwo          = 0x40,
    kbShiftThree        = 0x23,
    kbShiftFour         = 0x24,
    kbShiftFive         = 0x25,
    kbShiftSix          = 0x5e,
    kbShiftSeven        = 0x26,
    kbShiftEight        = 0x2a,
    kbShiftNine         = 0x28,
    kbShiftZero         = 0x29,
    kbShiftMinus        = 0x5f,
    kbShiftEqual        = 0x2b,
    kbShiftBackSlash    = 0x7c,
    kbShiftQ            = 0x51,
    kbShiftW            = 0x57,
    kbShiftE            = 0x45,
    kbShiftR            = 0x52,
    kbShiftT            = 0x54,
    kbShiftY            = 0x59,
    kbShiftU            = 0x55,
    kbShiftI            = 0x49,
    kbShiftO            = 0x4f,
    kbShiftP            = 0x50,
    kbShiftOpenBracket  = 0x7b,
    kbShiftCloseBracket = 0x7d,
    kbShiftA            = 0x41,
    kbShiftS            = 0x53,
    kbShiftD            = 0x44,
    kbShiftF            = 0x46,
    kbShiftG            = 0x47,
    kbShiftH            = 0x48,
    kbShiftJ            = 0x4a,
    kbShiftK            = 0x4b,
    kbShiftL            = 0x4c,
    kbShiftPointComma   = 0x3a,
    kbShiftQuot         = 0x22, // '
    kbShiftZ            = 0x5a,
    kbShiftX            = 0x58,
    kbShiftC            = 0x43,
    kbShiftV            = 0x56,
    kbShiftB            = 0x42,
    kbShiftN            = 0x4e,
    kbShiftM            = 0x4d,
    kbShiftComma        = 0x3c,
    kbShiftPoint        = 0x3e,
    kbShiftSlash        = 0x3f
////////////////////////////////////////////////////////////////////////////////////////////////////
    ;

unsigned short const typable[] =
{
    kbWhiteSpace       ,
    kbBackQuot         , // `
    kbOne              ,
    kbTwo              ,
    kbThree            ,
    kbFour             ,
    kbFive             ,
    kbSix              ,
    kbSeven            ,
    kbEight            ,
    kbNine             ,
    kbZero             ,
    kbMinus            ,
    kbEqual            ,
    kbBackSlash        ,
    kbQ                ,
    kbW                ,
    kbE                ,
    kbR                ,
    kbT                ,
    kbY                ,
    kbU                ,
    kbI                ,
    kbO                ,
    kbP                ,
    kbOpenBracket      ,
    kbCloseBracket     ,
    kbA                ,
    kbS                ,
    kbD                ,
    kbF                ,
    kbG                ,
    kbH                ,
    kbJ                ,
    kbK                ,
    kbL                ,
    kbPointComma       ,
    kbQuot             , // '
    kbZ                ,
    kbX                ,
    kbC                ,
    kbV                ,
    kbB                ,
    kbN                ,
    kbM                ,
    kbComma            ,
    kbPoint            ,
    kbSlash            ,

    kbShiftBackQuot    , // `
    kbShiftOne         ,
    kbShiftTwo         ,
    kbShiftThree       ,
    kbShiftFour        ,
    kbShiftFive        ,
    kbShiftSix         ,
    kbShiftSeven       ,
    kbShiftEight       ,
    kbShiftNine        ,
    kbShiftZero        ,
    kbShiftMinus       ,
    kbShiftEqual       ,
    kbShiftBackSlash   ,
    kbShiftQ           ,
    kbShiftW           ,
    kbShiftE           ,
    kbShiftR           ,
    kbShiftT           ,
    kbShiftY           ,
    kbShiftU           ,
    kbShiftI           ,
    kbShiftO           ,
    kbShiftP           ,
    kbShiftOpenBracket ,
    kbShiftCloseBracket,
    kbShiftA           ,
    kbShiftS           ,
    kbShiftD           ,
    kbShiftF           ,
    kbShiftG           ,
    kbShiftH           ,
    kbShiftJ           ,
    kbShiftK           ,
    kbShiftL           ,
    kbShiftPointComma  ,
    kbShiftQuot        , // '
    kbShiftZ           ,
    kbShiftX           ,
    kbShiftC           ,
    kbShiftV           ,
    kbShiftB           ,
    kbShiftN           ,
    kbShiftM           ,
    kbShiftComma       ,
    kbShiftPoint       ,
    kbShiftSlash       /*,

    kbGraySlash        ,
    kbGrayAsterisk     ,
    kbGrayMinus        ,
    kbGrayPlus         ,

    kbGraySeven        ,
    kbGrayEight        ,
    kbGrayNine         ,
    kbGrayFour         ,
    kbGrayFive         ,
    kbGraySix          ,
    kbGrayOne          ,
    kbGrayTwo          ,
    kbGrayThree        ,
    kbGrayZero         ,
    kbGrayPoint*/
};

int const typcount = sizeof(typable) / sizeof(typable[0]);

#ifdef KEYBOARD_UTIL

#include <conio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define releasetimeslice()

/*
 *  0 = No SHIFT's pressed
 *  1 = Right SHIFT is pressed
 *  2 = Left SHIFT is pressed
 *  3 = Both SHIFT're pressed
 */
inline int shifted()
{
        return 0;
}

inline unsigned long shiftstatus()
{
        return 0;
}

extern void (*keyb_callback)();
#define KEYB_CALLBACK_DEF   keyb_callback

inline int keypressed(void (*callback)() = KEYB_CALLBACK_DEF)
{
    if (callback)
        callback();
    return _kbhit();
}

inline void waitkey(void (*callback)() = KEYB_CALLBACK_DEF)
{
    while (!keypressed(callback))
        releasetimeslice();
}

inline unsigned readkey(int *shift = NULL, void (*callback)() = KEYB_CALLBACK_DEF)
{
    waitkey(callback);
    if (shift)
        *shift = shifted();

    UINT real_key;
    SHORT status;

    for (int j=0; j < 256; j++)
    {
        status = GetAsyncKeyState(j);
        switch (sizeof(SHORT))
        {
            case 2: if (status & 0x8000) 
                        goto LLL;
                    goto KKK;
            default: throw 1;
        }
    KKK:
        continue;
    LLL:
        real_key = MapVirtualKey(j, 0);
        unsigned k = _getch(), kk=0, rk;
        if (k==0xe0 || k==0)
            kk=_getch();
        rk=(kk<<8)|k;
        return rk;
    }
}

inline void flushkeyboard()
{
    while (keypressed())
        readkey();
}

inline int ischaracter(unsigned short code)
{
    for (int i = 0; i < typcount; ++i)
        if (code == typable[i])
            return 1;
    return 0;
}

inline char tochar(unsigned short code)
{
    return (char) (code & 0xFF);
}
#endif //KEYBOARD_UTIL

#endif //__KEYBOARD_H__
