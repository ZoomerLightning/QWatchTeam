#ifndef __AT_H__
#define __AT_H__

#include "pmt.h"

extern BYTE
    atNormalText,
    atSelect,
    atStatusLine,
    atMainFrame,
    atKeyWord,
    atFunction,
    atSpecial,
    atLabel,
    atSyntax,
    atInvalid,

    atOctal,
    atHex,
    atDecimal,
    atFloat,
    atBinary,

    atChar,
    atString,
    atPreprocess,
    atPreprocessUnknown,
    atStdinclude,
    atComment,

    atTitle,

    atMBNormal,
    atMBSave,
    atMBHelp,
    atMBError,

    atRulerN,
    atRulerI;

#endif //__AT_H__
