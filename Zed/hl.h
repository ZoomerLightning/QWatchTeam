#ifndef __HL_H__
#define __HL_H__

#include "zeddef.h"

extern zedConfig Config;

/*fix extern int const
	ftUndef,
	ftC,
	ftASM,
	ftCmm,
	ftPascal,
	ftIDC,
	ftHaskell,
	ftADA;
*/	

extern "C++"
{
    int getfiletype(char const *filename);
    void highlight(char const *s, int L, char *buf, int buflen, int loaf, int offset, int ft);
#ifndef NDEBUG
    void hl_test();
#endif
}

#endif //__HL_H__
