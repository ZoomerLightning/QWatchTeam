#ifndef __CFG_H__
#define __CFG_H__

#define HAS_PASCAL_PARSER
#define HAS_CMINUSMINUS_PARSER
#define HAS_NASM_PARSER
#define HAS_HASKELL_PARSER

#ifdef FREE
#undef HAS_CMINUSMINUS_PARSER
#undef HAS_NASM_PARSER
#undef HAS_HASKELL_PARSER
#endif

#endif //__CFG_H__
