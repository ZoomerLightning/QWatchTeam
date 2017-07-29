/*
** tok.h
**
** Defines class CToken (and several parsers as members).
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __CTOKEN_H__
#define __CTOKEN_H__

#include <string.h>
#include "mystring.h"
#include "pmt.h"
#include "cfg.h"

typedef enum
{
	ctInvalid = 0,
	ctOctal = 1, ctHex = 2, ctDecimal = 4, ctFloat = 8,
	ctBinary = 0x4000, ctRoman = 0x8000,
	ctChar = 0x10, ctString = 0x20,
	ctSymbol = 0x40, ctPreprocessor = 0x80,
	ctJumpLabel = 0x100 /*todo: hard to implement*/,
	ctComment1 = 0x200 /*single line ("//")*/,
	ctComment2 = 0x400 /*multi-line in one line("/ * * /")*/,
	ctComment3 = 0x800 /*multi-line open only ("/ *")*/,
	ctComment4 = 0x1000 /*multi-line close only ("* /")*/,
	ctSyntax = 0x2000,
	ctStdinclude = 0x10000
} tCToken;

#ifdef HAS_HASKELL_PARSER
typedef enum
{
	hasktInvalid = 0,
	hasktOctal = 1, hasktHex = 2, hasktDecimal = 4,
	hasktFloat = 8,
	hasktChar = 0x10, hasktString = 0x20,
	hasktSymbol = 0x40,
	hasktPreprocessor = 0x80 /* "{-# #-}" */,
	hasktComment1 = 0x200 /*single line ("--")*/,
	hasktComment2 = 0x400 /*multi-line in one line("{- -}")*/,
	hasktComment3 = 0x800 /*multi-line open only ("{-")*/,
	hasktComment4 = 0x1000 /*multi-line close only ("-}")*/,
	hasktSyntax = 0x2000
} tHaskellToken;
#endif

#ifdef HAS_NASM_PARSER
typedef enum
{
	nasmtInvalid = 0,
	nasmtOctal = 1, nasmtHex = 2, nasmtDecimal = 4, nasmtFloat = 8,
	nasmtBinary = 0x4000,
	/*nasmtChar = 0x10,*/
	nasmtString = 0x20,
	nasmtSymbol = 0x40,
	nasmtPreprocessor = 0x80,
	nasmtLabel = 0x100,
	nasmtComment = 0x200 /*single line (";")*/,
	nasmtSyntax = 0x2000
} tNasmToken;
#endif

#ifdef HAS_CMINUSMINUS_PARSER
typedef enum
{
	cmintInvalid = 0,
	cmintOctal = 1, cmintHex = 2, cmintDecimal = 4,
	/*cmintFloat = 8,*/
	cmintBinary = 0x4000,
	/*cmintRoman = 0x8000,*/
	cmintChar = 0x10, cmintString = 0x20,
	cmintSymbol = 0x40, cmintPreprocessor = 0x80,
	/*cmintJumpLabel = 0x100,*/
	cmintComment1 = 0x200 /*single line ("//")*/,
	cmintComment2 = 0x400 /*multi-line in one line("/ * * /")*/,
	cmintComment3 = 0x800 /*multi-line open only ("/ *")*/,
	cmintComment4 = 0x1000 /*multi-line close only ("* /")*/,
	cmintSyntax = 0x2000
} tCMinusToken;
#endif

#ifdef HAS_PASCAL_PARSER
typedef enum
{
	ptInvalid = 0,
	/*ptOctal = 1,*/
	ptHex = 2, ptDecimal = 4, ptFloat = 8,
	/*ptBinary = 0x4000, ptRoman = 0x8000,*/
	ptChar = 0x10, ptString = 0x20,
	ptSymbol = 0x40,
	ptPreprocessor = 0x80 /*{$...} (*$...*)*/,
	ptComment1 = 0x200 /*single line ("//")*/,
	ptComment2 = 0x400 /*multi-line in one line("{} (**)")*/,
	ptComment3 = 0x800 /*multi-line open only ("{ (*")*/,
	ptComment4 = 0x1000 /*multi-line close only ("} *)")*/,
	ptSyntax = 0x2000
} tPascalToken;
#endif

int const ctNumber  = ctOctal + ctHex + ctDecimal + ctFloat
						+ ctBinary + ctRoman
						+ ctChar
						;
int const ctInteger = ctOctal + ctHex + ctDecimal
						+ ctBinary + ctRoman
						+ ctChar
						;

class CToken
{
private:
	_string _s;

	int     pos;
	int     L;

	bool    pp;                         /* preprocessor directive found */
	int     ppx1, ppx2;

public:
	class exc {};

public:
	CToken() :
		pos(0),
		L(0),
		pp(false)
	{}
	CToken(char const *S, int new_len = -1) :
		pos(0),
		L(0),
		pp(false)
	{
		if (new_len == -1)
		{
			_s = const_cast<char*>(S);
			L = _s.getwidth();
		}
		else
		{
			_s.assign(const_cast<char*>(S), new_len);
			L = new_len;
		}
	}
	CToken(String const &S) :
		pos(0),
		L(0),
		pp(false)
	{
		_s.assign(const_cast<char*>(S.c_str()), S.getwidth());
		L = _s.getwidth();
	}
	~CToken()
	{}

	void init(String const &S)
	{
		pos = 0, pp = false;
		_s.assign(const_cast<char*>(S.c_str()), S.getwidth());
		L = _s.getwidth();
	}

#ifdef MSS
#undef free //cpp must be destroyed!(bjarne)
#endif
	void free()
	{
		_s.destroy();
		pos = L = 0, pp = false;
	}

private:
	bool bad(int pos) const
	{
		return ((unsigned)pos >= (unsigned)L)? true : false;
	}

public:

	typedef int (CToken::*CT_GETNEXT)(int&, int&, int&, int&);

	int GetNextPair(int &X1, int &X2, int &X3, int &t); // returns >=1 if error, else 0
#ifdef HAS_PASCAL_PARSER
	int GetNextPair_Pascal(int &X1, int &X2, int &X3, int &t); // returns >=1 if error, else 0
#endif
#ifdef HAS_CMINUSMINUS_PARSER
	int GetNextPair_CMinus(int &X1, int &X2, int &X3, int &t); // returns >=1 if error, else 0
#endif
#ifdef HAS_NASM_PARSER
	int GetNextPair_Nasm(int &X1, int &X2, int &X3, int &t); // returns >=1 if error, else 0
#endif
#ifdef HAS_HASKELL_PARSER
	int GetNextPair_Haskell(int &X1, int &X2, int &X3, int &t); // returns >=1 if error, else 0
#endif

	void reset()
	{
		pos = 0, pp = false;
	}

	static char const *TTName(int t);

    static double ReadNumber(String const &s, int x1, int x2, int t);
	static unsigned read_char(char const *S, bool little_endian = true);
	static bool Little_endian;

	static void CreateSyntaxTables();

private:
	int SkipWS();

	static char SyntaxTable[256];
	static char __SyntaxTable[256];
#ifdef HAS_HASKELL_PARSER
	static char SyntaxTable_Haskell[256];
    static char __SyntaxTable_Haskell[256];
#endif
#ifdef HAS_PASCAL_PARSER
    static char SyntaxTable_Pascal[256];
    static char __SyntaxTable_Pascal[256];
#endif
#ifdef HAS_CMINUSMINUS_PARSER
    static char SyntaxTable_CMinus[256];
    static char __SyntaxTable_CMinus[256];
#endif
#ifdef HAS_NASM_PARSER
    static char SyntaxTable_Nasm[256];
    static char __SyntaxTable_Nasm[256];
#endif

};

class CTokenAtom
{
public:
    int         type;
    int         start, end;
    String      token;

    int         usertype;
	double     	value;

public:
    CTokenAtom() :
        type(ctInvalid),
        start(0),
        end(0),
        token(""),
        usertype(0),
        value(0)
    {}
    CTokenAtom(char const *s, int x1, int x2, int t) :
        type(t),
        start(x1),
        end(x2),
        usertype(0)
    {
        if (x2 < x1)
            x2 = x1;
        _string dummy = const_cast<char*>(s);
        token = dummy.fromto(x1, x2);
        value = CToken::ReadNumber(dummy, x1, x2, t);
    }
    CTokenAtom(String const &s, int x1, int x2, int t) :
        type(t),
        start(x1),
        end(x2),
        usertype(0)
    {
        if (x2 < x1)
            x2 = x1;
        token = s.fromto(x1, x2);
        value = CToken::ReadNumber(s, x1, x2, t);
    }
    CTokenAtom(char const *s, int x1, int x2, int t, int ut) :
        type(t),
        start(x1),
        end(x2),
        usertype(ut)
    {
        if (x2 < x1)
            x2 = x1;
        _string dummy = const_cast<char*>(s);
        token = dummy.fromto(x1, x2);
        value = CToken::ReadNumber(dummy, x1, x2, t);
    }
    CTokenAtom(int ut, const double& V) :
        type(ctInvalid),
        start(0),
        end(0),
        token(""),
        usertype(ut),
        value(V)
    {}
    CTokenAtom(int ut) :
        type(ctInvalid),
        start(0),
        end(0),
        token(""),
        usertype(ut),
        value(0)
    {}
    CTokenAtom(int ut, char const *s) :
        type(ctInvalid),
        start(0),
        end(0),
        token(s),
        usertype(ut),
        value(0)
    {}
    CTokenAtom(int ut, String const &s) :
        type(ctInvalid),
        start(0),
        end(0),
        token(s),
        usertype(ut),
        value(0)
    {}
    CTokenAtom(int x1, int x2, int t, int ut) :
        type(t),
        start(x1),
        end(x2),
        token(""),
        usertype(ut),
        value(0)
    {}

    CTokenAtom(CTokenAtom const &src)
    {
        type    = src.type;
        start   = src.start;
        end     = src.end;
        token   = src.token;
        usertype= src.usertype;
        value   = src.value;
    }
    ~CTokenAtom()               {}

    CTokenAtom &copy(CTokenAtom const &src)
    {
        type    = src.type;
        start   = src.start;
        end     = src.end;
        token   = src.token;
        usertype= src.usertype;
        value   = src.value;
        return *this;
    }
    CTokenAtom &operator=(CTokenAtom const &src)
    {
        return copy(src);
    }
};

#endif //__CTOKEN_H__
