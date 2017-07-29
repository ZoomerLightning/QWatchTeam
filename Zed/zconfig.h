/*
** zconfig.h
**
** Defines some defines for ZED ;-).
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#define COMPILER        "Visual Studio 2015"
#define COMPILER_VER    ""
#define CPU_TUNE        "i386"
#define CPU_ARCH        "i386"

#define ZED_VERSION     "v0.70"

char const *const aCompiler132 = //132 columns
    "Compiled at " __DATE__ "(" __TIME__ ")" \
    " by " COMPILER \
    " targeting " CPU_ARCH " and tuned for " CPU_TUNE " processor family";

char const *const aCompiler100 = //100 columns
    "Compiled at " __DATE__ "(" __TIME__ ")" \
    " by " COMPILER \
    " tuned for " CPU_TUNE " processor family";

char const *const aCompiler80 = //80 columns
    "Compiled at " __DATE__ \
    " by " COMPILER \
    " for " CPU_TUNE " cpu family";
