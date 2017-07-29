/*
** reserved.cpp
**
** Defines reserved words for several languages.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#include <cstddef>
#include "reserved.h"

char const *cpreprocessor[] =
{
    "#assert",
    "#define",
    "#elif", "#else", "#endif", "#error",
    "#ident", "#identifier", "#if", "#ifdef", "#ifndef", "#import", "#include", "#include_next",
    "#line",
    "#pragma",
    "#sccs",
    "#unassert", "#undef",
    "#warning",
    NULL
};

char const *cdef[] =
{
    "defined",

    "__LINE__", "__FILE__", "__DATE__", "__TIME__",
    "__STDC__",
    "__STDC_VERSION__",
    "__OBJC__",
    "__STRICT_ANSI__", "__BASE_FILE__", "__INCLUDE_LEVEL__", "__VERSION__",
    "__FUNCTION__", "__PRETTY_FUNCTION__",

    "__SIZE_TYPE__", "__PTRDIFF_TYPE__", "__WCHAR_TYPE__",
    "__USER_LABEL_PREFIX__", "__REGISTER_PREFIX__", "__IMMEDIATE_PREFIX__",
    "__HAVE_BUILTIN_SETJMP__",
    "__CHAR_UNSIGNED__",
    "__NeXT__",
    "__VA_ARGS__",

    "__cplusplus",
    "__EXCEPTIONS", "_CPPUNWIND",
    "__tune_i386__",
    "__tune_i486__",
    "__tune_pentium__",
    "__tune_pentiumpro__",
    "__tune_k6__",
    "__tune_athlon__",
    "__386__",
    "i386",
    "_i386_",
    "__i386__", "__i386",
    "__i486__", "__i486",
    "__i586__", "__i586",
    "__i686__", "__i686",
    "__pentium__", "__pentium",
    "__pentiumpro__", "__pentiumpro",
    "__k6__", "__k6",
    "__athlon__", "__athlon",
    "__sparcv9__",
    "__FLAT__", "__SMALL__", "__MEDIUM__", "__COMPACT__", "__LARGE__", "__HUGE__", "__TINY__",
    "_MT", "__MT__",
    "_REENTRANT",
    "GO32", "__GO32__", "__GO32",
    "MSDOS", "__MSDOS__", "__MSDOS",
    "__DOS__",
    "unix", "__unix__", "__unix",
    "LINUX", "_LINUX", "__LINUX", "__LINUX__",
    "linux", "_linux", "__linux", "__linux__",
    "__bsdi__", "__NetBSD__", "__FreeBSD__",
    "__XWIN__",
    "__WIN32__",
    "__DLL__",
    "WIN32_LEAN_AND_MEAN",

    "_Builtin",
    "_M_IX86",
    "_X86_",
    "_WIN32",
    "WIN32",

    "__32BIT__",
    "__EMX__",

    "__PASCAL__", "__FASTCALL__", "__CDECL__",
    "__MULTI_THREAD__", "__SINGLE_THREAD__",
    "__OS2__",
    "_Windows",
    "__DLLSMALL__",
    "__BCOPT__",
    "__TEMPLATES__",
    "__OVERLAY__",

    "__GNUC__", "__GNUC_MINOR__", "__GNUG__",
    "__GLIBC__",
    "DJGPP", "DJGPP_MINOR", "__DJGPP__", "__DJGPP_MINOR__", "__DJGPP", "__DJGPP_MINOR",
    "__RSXNT__", "__RSX32__", "__CRTRSXNT__",
    "__WATCOMC__", "__WATCOM_CPLUSPLUS__",
    "__TURBOC__", "__BORLANDC__", "__TCPLUSPLUS__", "__BCPLUSPLUS__",
    "__sgi" /*SGI IRIX C++*/,
    "__USLC__" /*SCO UDK 7 compiler (UnixWare 7x, OSR 5, UnixWare 2x)*/,
    "__SUNPRO_CC",
    "__COMO__",
    "__ICL" /*Intel compiler, which uses the EDG front end*/,
    "__CYGWIN__" /*Cygwin32, egcs compiler on MS Windows*/,
    "__MINGW32__" /*Mingw32, egcs compiler using the Microsoft C runtime*/,
    "_MSC_VER" /*Microsoft compiler*/,
    "__LCC__",
    "__CCDL__" /*Ladsoft compiler*/,

#if 1 // 2001-OCT-27/28/29

    "ALLEGRO_VERSION",
    "ALLEGRO_SUB_VERSION",
    "ALLEGRO_VERSION_STR",
    "ALLEGRO_DATE_STR",
    "ALLEGRO_DATE",

    "__GNU_MP__",
    "_CRAY",
    "_CRAYMPP",
    "__GNU_MP_VERSION",
    "__GNU_MP_VERSION_MINOR",
    "__GNU_MP_VERSION_PATCHLEVEL",

    "MSS_VERSION",
    "MSS",

    "PCRE_MAJOR",
    "PCRE_MINOR",
    "PCRE_DATE",

    "SLANG_VERSION",
    "SLANG_VERSION_STRING",
    "DOS",
    "IBMPC_SYSTEM",

    "GRX_VERSION",
    "GRX_VERSION_API",

    "__JPTUI__",

    "JPEG_LIB_VERSION",

    "",
    "",
    "",

    "__P",
    "_PTR",
    "_AND",
    "_NOARGS",
    "_CONST",
    "_VOLATILE",
    "_SIGNED",
    "_DOTS",
    "_VOID",
    "_EXFUN",
    "_DEFUN",
    "_DEFUN_VOID",
    "_CAST_VOID",
    "_LONG_DOUBLE",
    "_PARAMS",
    "_ATTRIBUTE",
    "__BEGIN_DECLS",
    "__END_DECLS",

    "_KEYBRD_READ",
    "_KEYBRD_READY",
    "_KEYBRD_SHIFTSTATUS",
    "_NKEYBRD_READ",
    "_NKEYBRD_READY",
    "_NKEYBRD_SHIFTSTATUS",
    "_TIME_GETCLOCK",
    "_TIME_SETCLOCK",

    "__dj_ENFORCE_FUNCTION_CALLS",
    "__dj_ENFORCE_ANSI_FREESTANDING",

    "__GNUC_PATCHLEVEL__",
    "__OPTIMIZE_SIZE__",
    "__OPTIMIZE__",
    "__NO_INLINE__",
    "__FAST_MATH__",
    "__STDC_HOSTED__",
    "__GXX_DEPRECATED",
    "__GXX_ABI_VERSION",
    "__tune_i586__",
    "__tune_i686__",

    "_POSIX_SOURCE",
    "__GNU_LIBRARY__",
    "STDC_HEADERS",
    "_LIBC",

    "STACK_DIRECTION",
    "HAVE_LONG_DOUBLE",

    "BITSPERBYTE",
    "BYTE_ORDER",
    "LITTLE_ENDIAN",
    "BIG_ENDIAN",
    "CHARBITS",
    "DAYSPERLYEAR",
    "DAYSPERNYEAR",
    "DAYSPERWEEK",
    "DOUBLEBITS",
    "FA_ARCH",
    "FA_DIREC",
    "FA_HIDDEN",
    "FA_LABEL",
    "FA_RDONLY",
    "FA_SYSTEM",
    "FLOATBITS",
    "HOURSPERDAY",
    "IC_AFFINE",
    "IC_PROJECTIVE",
    "LONGBITS",

    "MAXDIR",
    "MAXDOUBLE",
    "MAXDRIVE",
    "MAXEXT",
    "MAXFILE",
    "MAXFLOAT",
    "MAXGETHOSTNAME",
    "MAXINT",
    "MAXLONG",
    "MAXPATH",
    "MAXPATHLEN",
    "MAXSHORT",

    "MCW_EM",
    "MCW_IC",
    "MCW_PC",
    "MCW_RC",
    "MINDOUBLE",
    "MINFLOAT",
    "MININT",
    "MINLONG",
    "MINSHORT",
    "NULL_DEVICE",

    "O_ACCMODE",
    "O_APPEND",
    "O_BINARY",
    "O_CREAT",
    "O_EXCL",
    "O_NOCTTY",
    "O_NOFOLLOW",
    "O_NOINHERIT",
    "O_NOLINK",
    "O_NONBLOCK",
    "O_RDONLY",
    "O_RDWR",
    "O_TEMPORARY",
    "O_TEXT",
    "O_TRUNC",
    "O_WRONLY",

    "PATH_MAX",
    "PC_24",
    "PC_53",
    "PC_64",
    "PDP_ENDIAN",
    "P_WAIT",
    "RC_CHOP",
    "RC_DOWN",
    "RC_NEAR",
    "RC_UP",
    "SECSPERDAY",
    "SECSPERHOUR",
    "SECSPERMIN",
    "MINSPERHOUR",

    "SH_COMPAT",
    "SH_DENYNO",
    "SH_DENYRD",
    "SH_DENYRW",
    "SH_DENYWR",

    "SIZEOF_CHAR",
    "SIZEOF_CHAR_P",
    "SIZEOF_DOUBLE",
    "SIZEOF_DOUBLE_P",
    "SIZEOF_FLOAT",
    "SIZEOF_FLOAT_P",
    "SIZEOF_INT",
    "SIZEOF_INT_P",
    "SIZEOF_LONG",
    "SIZEOF_LONG_DOUBLE",
    "SIZEOF_LONG_DOUBLE_P",
    "SIZEOF_LONG_LONG",
    "SIZEOF_LONG_LONG_P",
    "SIZEOF_LONG_P",
    "SIZEOF_SHORT",
    "SIZEOF_SHORT_P",
    "SIZEOF_VOID_P",

    "STDERR_FILENO",
    "STDIN_FILENO",
    "STDOUT_FILENO",

    "SW_INVALID",
    "SW_DENORMAL",
    "SW_ZERODIVIDE",
    "SW_OVERFLOW",
    "SW_UNDERFLOW",
    "SW_INEXACT",
    "SW_STACKFAULT",
    "SW_ERRORSUMMARY",
    "SW_COND",
    "SW_C0",
    "SW_C1",
    "SW_C2",
    "SW_C3",
    "SW_TOP",
    "SW_TOP_SHIFT",
    "SW_BUSY",

    "EM_INVALID",
    "EM_DENORMAL",
    "EM_ZERODIVIDE",
    "EM_OVERFLOW",
    "EM_UNDERFLOW",
    "EM_INEXACT",

    "TZ_MAX_TIMES",
    "TZ_MAX_TYPES",
    "TZ_MAX_CHARS",
    "TZ_MAX_LEAPS",
    "MONSPERYEAR",

    "TM_SUNDAY",
    "TM_MONDAY",
    "TM_TUESDAY",
    "TM_WEDNESDAY",
    "TM_THURSDAY",
    "TM_FRIDAY",
    "TM_SATURDAY",

    "TM_JANUARY",
    "TM_FEBRUARY",
    "TM_MARCH",
    "TM_APRIL",
    "TM_MAY",
    "TM_JUNE",
    "TM_JULY",
    "TM_AUGUST",
    "TM_SEPTEMBER",
    "TM_OCTOBER",
    "TM_NOVEMBER",
    "TM_DECEMBER",

    "TM_YEAR_BASE",
    "EPOCH_YEAR",
    "EPOCH_WDAY",

    "UCLOCKS_PER_SEC",

    "_A_ARCH",
    "_A_HIDDEN",
    "_A_NORMAL",
    "_A_RDONLY",
    "_A_SUBDIR",
    "_A_SYSTEM",
    "_A_VOLID",

    "_FILESYS_CASE_PRESERVED",
    "_FILESYS_CASE_SENSITIVE",
    "_FILESYS_LFN_SUPPORTED",
    "_FILESYS_UNICODE",
    "_FILESYS_UNKNOWN",
    "_FILESYS_VOL_COMPRESSED",

    "_GO32_RUN_MODE_DPMI",
    "_GO32_RUN_MODE_RAW",
    "_GO32_RUN_MODE_UNDEF",
    "_GO32_RUN_MODE_VCPI",
    "_GO32_RUN_MODE_XMS",

    "_O_APPEND",
    "_O_BINARY",
    "_O_CREAT",
    "_O_EXCL",
    "_O_NOINHERIT",
    "_O_RDONLY",
    "_O_RDWR",
    "_O_TEXT",
    "_O_TRUNC",
    "_O_WRONLY",

    "_POSIX_VERSION",

    "_SH_COMPAT",
    "_SH_DENYNO",
    "_SH_DENYRD",
    "_SH_DENYRW",
    "_SH_DENYWR",

    "__system_allow_long_cmds",
    "__system_allow_multiple_cmds",
    "__system_call_cmdproc",
    "__system_emulate_chdir",
    "__system_emulate_command",
    "__system_handle_null_commands",
    "__system_ignore_chdir",
    "__system_redirect",
    "__system_use_shell",

    "_dos_ds",
    "_go32_conventional_mem_selector",
    "_go32_dpmi_get_free_memory_information",
    "_go32_dpmi_registers",
    "_go32_dpmi_simulate_fcall",
    "_go32_dpmi_simulate_fcall_iret",
    "_go32_dpmi_simulate_int",
    "_go32_my_cs",
    "_go32_my_ds",
    "_go32_my_ss",

    "NDEBUG",

    "S_IEXEC",
    "S_IFBLK",
    "S_IFCHR",
    "S_IFDIR",
    "S_IFLNK",
    "S_IFREG",
    "S_IFSOCK",
    "S_IREAD",
    "S_IRGRP",
    "S_IROTH",
    "S_ISFIFO",
    "S_ISGID",
    "S_ISUID",
    "S_ISVTX",
    "S_IWGRP",
    "S_IWOTH",
    "S_IWRITE",
    "S_IXGRP",
    "S_IXOTH",

    "S_IFMT",
    "S_IRUSR",
    "S_IWUSR",
    "S_IXUSR",

    "",
    "",
    "",

    "__IBMCPP__",
    "__DECCXX",
    "__hpux",
    "__APPLE__",
    "__MSL__",
    "__MWERKS__" /*Metrowerks CW (Code Warrior)*/,
    "__CYGWIN32__",
    "macintosh" /*MPW C++ on the Mac*/,
    "applec",
    "__GLIBCPP__",
    "__SGI_STL_PORT",
    "_RTLDLL",
    "__LNX__",
    "BOOST_MSVC",
    "_CPPLIB_VER",
    "__BEOS__",
    "__DECCXX_VER",
    "__HP_aCC",
    "__STD_ITERATOR__",
    "__CONSOLE__",
    "_DLL",
    "STLPORT",
    "__STL_USE_NAMESPACES",
    "__ZTC__",
    "__alpha",
    "__alpha__",
    "__mips",
    "__sun",
    "pdp11",
    "sgi",
    "solarissparc",
    "sun",
    "vax",
    "x86",
    "__MSVCRT__",
    "__KCC",
    "__KCC_VERSION",
    "__SGI_STL",
    "_STLPORT_VERSION",
    "_XOPEN_SOURCE",
    "_XOPEN_SOURCE_EXTENDED",
    "_XOPEN_VERSION",
    "__INTEL__",
    "__MRC__",
    "__SC__",
    "__SunOS_5_6",
    "__GLIBC_MINOR__",
    "__mc68000__",
    "__mc68020__",
    "mc68020",
    "__mc68k__",
    "__MVS__",
    "__OS400__",
    "__QNX__",
    "__GC",
    "WIN16",
    "_WIN16",
    "_STLP_USE_EXCEPTIONS",
    "__STL_USE_EXCEPTIONS",
    "EXTENDED_DOS",
    "_EXTENDED_DOS",
    "_MSDOS",
    "_WINDOWS",
    "_ABIN32",
    "_ABI64",
    "__PACIFIC__",
    "__NT__",
    "_NOTHREADS",
    "__STL_WIN32THREADS",
    "_CXXRT_STD",
    "__STL_SGI_THREADS",
    "__DLL",
    "_WINDLL",
    "__IBMC__",
    "IBMCPP__",
    "__WORD64",
    "__arch64__",
    "_NAMESPACE_STD",
    "__ELF__",
    "_BSD_SOURCE",
    "_MAC",
    "_PA_RISC",
    "_AIX",
    "_CRAYT3E",
    "__DECC",
    "_NAMESPACES",
    "_MIPS_SIM",
    "_ABIO32",
    "_COMPILER_VERSION",
    "_STANDARD_C_PLUS_PLUS",
    "_EXCEPTIONS",
    "__ISCPP__",
    "__COMO_VERSION__",
    "MACOS",
    "_AMIGA",
    "__SASC",
    "_M68881",
    "_MIPSEB",
    "__sparc",
    "__sparc_v9__",
    "__sparcv9",
    "__digital__",
    "__xlC__",
    "__SunOS_5_8",
    "__MSL_CPP__",
    "__MLCCPP__",
    "__QNXNTO__",
    "__EDG_VERSION__",
    "_M_IA64",
    "__macintosh",
    "__sparc__",
    "__powerpc__",
    "__hppa",
    "__hppa__",
    "__ppc__",
    "__mips__",
    "_MIPS_SIM_ABI32",
    "BSD",
    "CRAY",
    "_M_MRX000",
    "_M_ALPHA",
    "_MSL_USING_NAMESPACE",
    "DOS386",
    "_SGIAPI",
    "_M_I86",
    "M_I86",
    "_INTEL_80x86",
    "__MC68K__",
    "__WINDOWS__",
    "_UNICODE",
    "__STDC_32_MODE__",
    "WORD64_AVAILABLE",
    "_MSVCPP_",
    "_MSC_FULL_VER",
    "__GNUC_MAJOR__",
    "_WINDOWS_",
    "__MULTI__",
    "__SUNPRO_CC_COMPAT",
    "_STLP_THREADS",
    "M_I86MM",
    "VMS",
    "_LP64",
    "__SUNPRO_C",
    "_PTHREADS",
    "__STL_PTHREADS",
    "_STLP_PTHREADS",
    "__STL_UITHREADS",
    "_STLP_OS2THREADS",
    "_STLP_BETHREADS",
    "_STLP_WIN32THREADS",
    "_STLP_SPARC_SOLARIS_THREADS",
    "_STLP_UITHREADS",
    "_STLP_SGI_THREADS",
    "_PTHREADS_DRAFT4",
    "_DECTHREADS_",
    "_PTHREAD_USE_D4",
    "_UITHREADS",
    "__STL_SOLARIS_THREADS",
    "__WIN16",
    "_STLP_THREADS_DEFINED",
    "_STLP_NOTHREADS",
    "_STLP_NO_THREADS",
    "NOTHREADS",
    "__vms",
    "__KAI_STRICT",
    "__WIN32",
    "__APOGEE__",
    "__STD_STRICT_ANSI",
    "__STD_STRICT_ANSI_ERRORS",
    "__EDG_SWITCHES",
    "_STLP_USE_NAMESPACES",
    "__STLPORT_VERSION",
    "__STL_THREADS",
    "__WATCOMCPLUSPLUS__",
    "__ICC",
    "__ghs",
    "M_I386",
    "INFINITY",
    "Macintosh",
    "BITS_PER_LONG",
    "_WIN32_WCE",
    "RISCOS",
    "RISC",
    "_KCC",
    "_RWSTD_VER",
    "NeXT",
    "UNICODE",
    "i8086",
    "CLK_TCK",
    "REAL_UNIX_SYSTEM",
    "MSWINDOWS",
    "__VMS_VER",
    "__os2__",
    "__MSDOS_16BIT__",
    "OS2",
    "__os2_16__",
    "CYGWIN32",
    "__VC__",
    "__WIN16__",
    "POSIX",
    "SYSV",
    "WINDOWS",
    "ultrix",
    "THINK_C",
    "V10",
    "VAX",
    "__COMPILER_VER__",
    "__SunOS_5_5_1",
    "__SVR4",
    "__svr4__",
    "__sun__",
    "__osf__",
    "GAS_MAJOR",
    "GAS_MINOR",
    "GAS_MINORMINOR",
    "DEC",
    "IBMPC",
    "M_XENIX",
    "MINIX",
    "__386BSD__",
    "__NT_DLL__",
    "__WINDOWS_386__",
    "__OS2V2__",
    "UNIX",
    "CONVEX",
    "ATARI_ST",
    "__BOUNDS_CHECKING_ON",
    "__CHECKER__",
    "__PURIFY__",
    "_BEOS_",
    "VAXC",
    "__TOS__",
    "__atarist__",
    "__palmos__",
    "OSF",
    "__dest_os",
    "__be_os",
    "__win32_os",
    "__mac_os",
    "STDC",
    "MSVMS",
    "__PUREC__",
    "__PIC__",
    "__pic__",
    "M_I86LM",
    "MTS",
    "M_I86SM",
    "AMIGA",
    "DECPDP",
    "MSC",
    "ATARI",
    "atarist",
    "DOS_OS2",
    "MSWIN",
    "HPUX",
    "SUNOS",
    "SOLARIS",
    "VOXWARE",
    "MACINTOSH",
    "DOS_VMS",
    "_UNIX",
    "M_I86CM",
    "__UNIX__",
    "_POSIX_C_SOURCE",
    "__NO_C9X",
    "__DPMI16__",
    "__DPMI32__",
    "_BIG_ENDIAN",
    "_LITTLE_ENDIAN",
    "WINVER",
    "__GPROF__",
    "__BYTE_ORDER__",
    "X11",
    "__FCC_VERSION",
    "__COMO_VERSION_",
    "",
    "",
    "",

#endif

// C++ standard external defines
    "SEEK_SET", "SEEK_CUR", "SEEK_END",
    "BUFSIZ",
    "EOF",
    "FILENAME_MAX",

    "CLOCKS_PER_SEC",
    "EDOM", "ERANGE",
    "EXIT_FAILURE", "EXIT_SUCCESS",
    "FOPEN_MAX",
    "HUGE_VAL",
    "LC_ALL", "LC_COLLATE", "LC_CTYPE", "LC_MONETARY", "LC_NUMERIC", "LC_TIME",
    "L_tmpnam",
    "MB_CUR_MAX",
    "RAND_MAX",
    "SIGABRT", "SIGFPE", "SIGILL", "SIGINT", "SIGSEGV", "SIGTERM",
#if 1
    "SIGQUIT",
    "SIGTRAP",
    "SIGBUS",
    "SIGBREAK",
    "SIGALRM",
    "SIGHUP",
    "SIGKILL",
    "SIGPIPE",
    "SIGMAX",
    "SIGNOFP",
    "SIGPROF",
    "SIGIOT",
    "SIGPWR",
    "SIGVTALRM",
    "SIGXCPU",
    "SIGXFSZ",
#endif
    "SIG_DFL", "SIG_ERR", "SIG_IGN",
    "TMP_MAX",
    "WCHAR_MAX", "WCHAR_MIN",
    "WEOF",
    "_IOFBF", "_IOLBF", "_IONBF",
    "assert",
    "offsetof",
    "setjmp",
    "va_arg",
    "va_end",
    "va_start",

    "CHAR_BIT", "CHAR_MAX", "CHAR_MIN",
    "DBL_DIG", "DBL_EPSILON", "DBL_MANT_DIG", "DBL_MAX", "DBL_MAX_10_EXP",
    "DBL_MAX_EXP", "DBL_MIN", "DBL_MIN_10_EXP", "DBL_MIN_EXP",
    "FLT_DIG", "FLT_EPSILON", "FLT_MANT_DIG", "FLT_MAX", "FLT_MAX_10_EXP", "FLT_MAX_EXP",
    "FLT_MIN", "FLT_MIN_10_EXP", "FLT_MIN_EXP", "FLT_RADIX", "FLT_ROUNDS",
    "INT_MAX", "INT_MIN",
    "LDBL_DIG", "LDBL_EPSILON", "LDBL_MANT_DIG", "LDBL_MAX", "LDBL_MAX_10_EXP",
    "LDBL_MAX_EXP", "LDBL_MIN", "LDBL_MIN_10_EXP", "LDBL_MIN_EXP",
    "LONG_MAX", "LONG_MIN",
    "MB_LEN_MAX",
    "SCHAR_MAX", "SCHAR_MIN",
    "SHRT_MAX", "SHRT_MIN",
    "UCHAR_MAX",
    "UINT_MAX",
    "ULONG_MAX",
    "USHRT_MAX",

    "NULL",
    "__emit__",

    NULL
};

char const *cop[] =
{
// GNU C option `-foperator-names' -- recognize the operator name keywords
    "and",
    "and_eq",
    "bitand",
    "bitor",
    "compl",
    "not",
    "not_eq",
    "or",
    "or_eq",
    "xor",
    "xor_eq",
    NULL
};

char const *crw[] =
{
    "_Bool",
    "_Complex",
    "__bounded",
    "__bounded__",
    "__builtin_va_arg",
    "__ptrbase",
    "__ptrbase__",
    "__ptrextent",
    "__ptrextent__",
    "__ptrvalue",
    "__ptrvalue__",
    "__unbounded",
    "__unbounded__",
// WATCOM C/C++ v10.0, GNU C/C++ 2.95 (DJGPP v2.03), Borland C/C++ v3.1
    "asm",
    "auto",
    "bool",
    "break",
    "case",
    "catch",
    "cdecl",
    "char",
    "class",
    "const",
    "const_cast",
    "continue",
    "default",
    "delete",
    "do",
    "double",
    "dynamic_cast",
    "else",
    "enum",
    "explicit",
    "export",
    "extern",
    "false",
    "far",
    "float",
    "for",
    "fortran",
    "friend",
    "goto",
    "huge",
    "if",
    "inline",
    "int",
    "interrupt",
    "long",
    "main",
    "mutable",
    "namespace",
    "near",
    "new",
    "operator",
    "overload",                 // obsolete
    "pascal",
    "private",
    "protected",
    "public",
    "register",
    "reinterpret_cast",
    "return",
    "short",
    "signature",
    "signed",
    "sigof",
    "sizeof",
    "static",
    "static_cast",
    "struct",
    "switch",
    "template",
    "this",
    "throw",
    "true",
    "try",
    "typedef",
    "typeid",
    "typename",
    "typeof",
    "union",
    "unsigned",
    "using",
    "virtual",
    "void",
    "volatile",
    "wchar_t",
    "while",

    "restrict",

    "__restrict",
    "__restrict__",
    "classof",
    "headof",
    "signature",
    "sigof",
    "__classof__",
    "__headof__",
    "__signature__",
    "__sigof__",

    "__multiple_inheritance",
    "__single_inheritance",
    "__virtual_inheritance",

// -- LCC --
    "__int64",
    "__int32",

    "__complex",
    "__complex__",
    "__imag",
    "__imag__",
    "__iterator",
    "__iterator__",
    "__null",
    "__real",
    "__real__",

// STD MACRO (compilers see above)
    "_Cdecl",
    "_Far16",
    "_Packed",
    "_Pascal",
    "_Seg16",
    "_Syscall",
    "_System",
    "__alignof",
    "__alignof__",
    "__asm",
    "__asm__",
    "__attribute",
    "__attribute__",
    "__based",
    "__cdecl",
    "__const",
    "__const__",
    "__cs",
    "__ds",
    "__es",
    "__export",
    "__extension__",
    "__far",
    "__far16",
    "__fastcall",
    "__fortran",
    "__huge",
    "__inline",
    "__inline__",
    "__interrupt",
    "__label__",
    "__loadds",
    "__near",
    "__pascal",
    "__saveregs",
    "__seg",
    "__segment",
    "__segname",
    "__self",
    "__signature__",
    "__signed",
    "__signed__",
    "__sigof__",
    "__ss",
    "__stdcall",
    "__syscall",
    "__system",
    "__typeof",
    "__typeof__",
    "__volatile",
    "__volatile__",
    "__wchar_t",
    "_asm",
    "_based",
    "_cdecl",
    "_cs",
    "_ds",
    "_es",
    "_export",
    "_far",
    "_far16",
    "_fastcall",
    "_fortran",
    "_fortran",
    "_huge",
    "_interrupt",
    "_loadds",
    "_main",
    "_near",
    "_pascal",
    "_regparam",
    "_saveregs",
    "_seg",
    "_segment",
    "_segname",
    "_self",
    "_ss",
    "_syscall",

// --CCDL--
    "_trap",
    "_intrinsic",
    "_genbyte",
    "_stdcall",
    "_absolute",

// C++ standard types and variables
    "clock_t",
    "div_t",
    "FILE",
    "fpos_t",
    "jmp_buf",
    "ldiv_t",
    "mbstate_t",
    "ptrdiff_t",
    "sig_atomic_t",
    "size_t",
    "time_t",
    "va_list",
    "wctrans_t",
    "wctype_t",
    "wint_t",
    "lconv",
    "tm",

    "stdin", "stdout", "stderr", "stdprn", "stdaux",
    "errno",

// --fstream.h-- predefined names
    "cin", "cout", "cerr", "clog",
    "endl", "ends",

    //"ios::in", "ios::out", "ios::ate", "ios::app", "ios::trunc",
    //"ios::nocreate", "ios::noreplace", "ios::binary",

    NULL
};

char const *cfun[] =
{
    "abort",
    "abs",
    "acos",
    "asctime",
    "asin",
    "atan",
    "atan2",
    "atexit",
    "atof",
    "atoi",
    "atol",
    "bsearch",
    "btowc",
    "calloc",
    "ceil",
    "clearerr",
    "clock",
    "cos",
    "cosh",
    "ctime",
    "difftime",
    "div",
    "exit",
    "exp",
    "fabs",
    "fclose",
    "feof",
    "ferror",
    "fflush",
    "fgetc",
    "fgetpos",
    "fgets",
    "fgetwc",
    "fgetws",
    "floor",
    "fmod",
    "fopen",
    "fprintf",
    "fputc",
    "fputs",
    "fputwc",
    "fputws",
    "fread",
    "free",
    "freopen",
    "frexp",
    "fscanf",
    "fseek",
    "fsetpos",
    "ftell",
    "fwide",
    "fwprintf",
    "fwrite",
    "fwscanf",
    "getc",
    "getchar",
    "getenv",
    "gets",
    "getwc",
    "getwchar",
    "gmtime",
    "isalnum",
    "isalpha",
    "iscntrl",
    "isdigit",
    "isgraph",
    "islower",
    "isprint",
    "ispunct",
    "isspace",
    "isupper",
    "iswalnum",
    "iswalpha",
    "iswcntrl",
    "iswctype",
    "iswdigit",
    "iswgraph",
    "iswlower",
    "iswprint",
    "iswpunct",
    "iswspace",
    "iswupper",
    "iswxdigit",
    "isxdigit",
    "labs",
    "ldexp",
    "ldiv",
    "localeconv",
    "localtime",
    "log",
    "log10",
    "longjmp",
    "malloc",
    "mblen",
    "mbrlen",
    "mbrtowc",
    "mbsinit",
    "mbsrtowcs",
    "mbstowcs",
    "mbtowc",
    "memchr",
    "memcmp",
    "memcpy",
    "memmove",
    "memset",
    "mktime",
    "modf",
    "perror",
    "pow",
    "printf",
    "putc",
    "putchar",
    "puts",
    "putwc",
    "putwchar",
    "qsort",
    "raise",
    "rand",
    "realloc",
    "remove",
    "rename",
    "rewind",
    "scanf",
    "setbuf",
    "setlocale",
    "setvbuf",
    "signal",
    "sin",
    "sinh",
    "sprintf",
    "sqrt",
    "srand",
    "sscanf",
    "strcat",
    "strchr",
    "strcmp",
    "strcoll",
    "strcpy",
    "strcspn",
    "strerror",
    "strftime",
    "strlen",
    "strncat",
    "strncmp",
    "strncpy",
    "strpbrk",
    "strrchr",
    "strspn",
    "strstr",
    "strtod",
    "strtok",
    "strtol",
    "strtoul",
    "strxfrm",
    "swprintf",
    "swscanf",
    "system",
    "tan",
    "tanh",
    "time",
    "tmpfile",
    "tmpnam",
    "tolower",
    "toupper",
    "towctrans",
    "towlower",
    "towupper",
    "ungetc",
    "ungetwc",
    "vfprintf",
    "vfwprintf",
    "vprintf",
    "vsprintf",
    "vswprintf",
    "vwprintf",
    "wcrtomb",
    "wcscat",
    "wcschr",
    "wcscmp",
    "wcscoll",
    "wcscpy",
    "wcscspn",
    "wcsftime",
    "wcslen",
    "wcsncat",
    "wcsncmp",
    "wcsncpy",
    "wcspbrk",
    "wcsrchr",
    "wcsrtombs",
    "wcsspn",
    "wcsstr",
    "wcstod",
    "wcstok",
    "wcstol",
    "wcstombs",
    "wcstoul",
    "wcsxfrm",
    "wctob",
    "wctomb",
    "wctrans",
    "wctype",
    "wmemchr",
    "wmemcmp",
    "wmemcpy",
    "wmemmove",
    "wmemset",
    "wprintf",
    "wscanf",
    NULL
};

#ifdef HAS_CMINUSMINUS_PARSER
char const *cmmpreprocessor[] =
{
    "?align", "?aligner", "?alignword", "?assumeDSSS",
    "?beep",
    "?codesize", "?ctrl_c",
    "?define", "?DOSrequired",
    "?include",
    "?jumptomain",
    "?maxerrors",
    "?parsecommandline", "?print", "?printhex",
    "?randombyte", "?resize", "?resizemessage",
    "?speed", "?stack", "?startaddress",
    "?use8086", "?use8088", "?use80186", "?use80286", "?use80386", "?use80486", "?use80586",
    NULL
};

char const *cmmdef[] =
{
    "__CODEPTR__",
    "__DATAPTR__",
    "__POSTPTR__",
    "__COMPILER__",
    "__DATESTR__",
    "__YEAR__",
    "__MONTH__",
    "__DAY__",
    "__HOUR__",
    "__MINUTE__",
    "__SECOND__",
    "__WEEKDAY__",
    "__VER1__",
    "__VER2__",
    NULL
};

char const *cmmrw[] =
{ //SPHINX C-- Compiler  BETA V0.189. Copyright Peter Cellik (C) 1993
    "main",
    "byte",
    "word",
    "char",
    "int",
    "dword",
    "long",
    "if",
    "loop",
    "return",
    "do",
    "while",
    "else",
    "interrupt",
    "void",
    "enum",
    "inline",
    "carryflag",
    "else",
    "extract",
    "false",
    "from",
    "if",
    "notcarryflag",
    "notoverflow",
    "overflow",
    "true",
    "zeroflag",
    "notzeroflag",
    "esbyte",
    "esword",
    "eschar",
    "esint",
    "esdword",
    "eslong",
    "csbyte",
    "csword",
    "cschar",
    "csint",
    "csdword",
    "cslong",
    "ssbyte",
    "ssword",
    "sschar",
    "ssint",
    "ssdword",
    "sslong",
    "dsbyte",
    "dsword",
    "dschar",
    "dsint",
    "dsdword",
    "dslong",
    "fsbyte",
    "fsword",
    "fschar",
    "fsint",
    "fsdword",
    "fslong",
    "gsbyte",
    "gsword",
    "gschar",
    "gsint",
    "gsdword",
    "gslong",
    NULL
};
#endif

#ifdef HAS_NASM_PARSER
char const *nasmpreprocessor[] =
{
    "%assign",
    "%clear",
    "%define",
    "%elif", "%elifctx", "%elifdef", "%elifid", "%elifidn", "%elifidni", "%elifnctx", "%elifndef", "%elifnid",
    "%elifnidn", "%elifnidni", "%elifnnum", "%elifnstr", "%elifnum", "%elifstr", "%else",
    "%endif", "%endm", "%endmacro", "%endrep", "%error", "%exitrep",
    "%iassign", "%idefine", "%if", "%ifctx", "%ifdef", "%ifid", "%ifidn", "%ifidni", "%ifnctx",
    "%ifndef", "%ifnid", "%ifnidn", "%ifnidni", "%ifnnum", "%ifnstr", "%ifnum", "%ifstr",
    "%imacro", "%include",
    "%line",
    "%macro",
    "%pop", "%push",
    "%rep", "%repl", "%rotate",
    "%undef",
    NULL
};

char const *nasmdef[] =
{
    "__NASM_CDecl__",
    "__NASMDEFSEG",
    "__NASM_MAJOR__",
    "__NASM_MINOR__",
    "__FILE__",
    "__LINE__",
    "__SECT__",
    NULL
};

char const *nasmspecial[] =
{
    "..got",
    "..gotoff",
    "..gotpc",
    "..plt",
    "..start",
    "..sym",
    ".absolut",
    ".bss",
    ".comment",
    ".data",
    ".nolist",
    ".rel",
    ".shstrtab",
    ".strtab",
    ".symtab",
    ".text",
//    "class=",
//    "overlay=",
//    "align=",
//    "absolute=",                //todo: conflicts with 'absolute'; and "absolute=xxx" (no whitespace)
//    "parm=",
    NULL
};

char const *nasmreg[] =
{
    "al", "bl", "cl", "dl", "ah", "bh", "ch", "dh", "ax", "bx", "cx", "dx", "si", "di", "bp", "sp",
    "es", "ds", "ss", "cs", "fs", "gs",
    "cr0", "cr2", "cr3", "cr4",
    "dr0", "dr1", "dr2", "dr3", "dr6", "dr7",
    "eax", "ebx", "ecx", "edx", "esi", "edi", "esp", "ebp",
    "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7",
    "st0", "st1", "st2", "st3", "st4", "st5", "st6", "st7",
    "tr3", "tr4", "tr5", "tr6", "tr7",
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",
    NULL
};

char const *nasmrw[] =
{
    "a16",
    "a32",
    "absolute",
    "at",
    "byte",
    "common",
    "debug",
    "dword",
    "far",
    "long",
    "near",
    "nosplit",
    "o16",
    "o32",
    "qword",
    "seg",
    "short",
    "times",
    "tword",
    "word",
    "wrt",

    "use16",
    "use32",
    "align",
    "alignb",
    "bits",
    "struc",
    "endstruc",
    "equ",
    "extern",
    "global",
    "iend",
    "incbin",
    "istruc",
    "library",
    "org",
    "section",
    "segment",
    "resb", "resw", "resd", "resq", "rest",
    "db", "dw", "dd", "dq", "dt",
    NULL
};
#endif

#ifdef HAS_PASCAL_PARSER
char const *prw[] =
{
    "absolute",
    "and",
    "array",
    "as",
    "asm",
    "assembler",
    "begin",
    "boolean",
    "break",
    "byte",
    "bytebool",
    "case",
    "cdecl",
    "char",
    "class",
    "comp",
    "const",
    "constructor",
    "continue",
    "dec",
    "default",
    "destructor",
    "dispinterface",
    "dispose",
    "div",
    "do",
    "double",
    "downto",
    "dynamic",
    "else",
    "end",
    "except",
    "exit",
    "export",
    "exports",
    "extended",
    "external",
    "false",
    "far",
    "file",
    "finalization",
    "finally",
    "for",
    "forward",
    "function",
    "goto",
    "if",
    "implementation",
    "in",
    "inc",
    "index",
    "inherited",
    "initialization",
    "inline",
    "integer",
    "interface",
    "interrupt",
    "is",
    "label",
    "library",
    "longbool",
    "longint",
    "mod",
    "name",
    "near",
    "new",
    "nil",
    "not",
    "object",
    "of",
    "on",
    "or",
    "out",
    "overload",
    "override",
    "packed",
    "pointer",
    "private",
    "procedure",
    "program",
    "property",
    "protected",
    "public",
    "published",
    "raise",
    "read",
    "real",
    "record",
    "reintroduce",
    "repeat",
    "resident",
    "resourcestring",
    "set",
    "shl",
    "shortint",
    "shr",
    "single",
    "size",
    "string",
    "then",
    "threadvar",
    "to",
    "true",
    "try",
    "type",
    "unit",
    "until",
    "uses",
    "var",
    "virtual",
    "while",
    "with",
    "word",
    "wordbool",
    "write",
    "xor",

    "ansichar",
    "ansistring",
    "automated",
    "cardinal",
    "currency",
    "dispid",
    "message",
    "nodefault",
    "null",
    "ord",
    "pansichar",
    "pascal",
    "pchar",
    "pwidechar",
    "register",
    "shortstring",
    "smallint",
    "stdcall",
    "stored",
    "unassigned",
    "variant",
    "widechar",

// GNU Pascal extensions
    "abstract",
    "all",
    "and_then",
    "bindable",
    "fail",
    "import",
    "module",
    "only",
    "operator",
    "or_else",
    "otherwise",
    "pow",
    "qualified",
    "restricted",
    "self",
    "value",
    "view",
    NULL
};
#endif

#ifdef HAS_HASKELL_PARSER
char const *haskelldef[] =
{
    "__HBC__" /*Chalmers Haskell, hbc*/,
    "__NHC__" /*Niklas Rojemo's & Malcolm Wallace's nhc98*/,
    /*Glasgow Haskell (GHC)*/

    "null",

    NULL
};

char const *haskellpreprocessor[] =
{
    //"#assert",
    "#define",
    "#elif",
    "#else",
    "#endif",
    "#error",
    //"#ident",
    //"#identifier",
    "#if",
    "#ifdef",
    "#ifndef",
    //"#import",
    "#include",
    //"#include_next",
    //"#line",
    "#pragma",
    //"#sccs",
    //"#unassert",
    "#undef",
    "#warning",
    NULL
};

char const *haskellspecial[] =
{
    "as",
    "qualified",
    "hiding",
    NULL
};

char const *haskellrw[] =
{
    "_",
    "case",
    "class",
    "data",
    "default",
    "deriving",
    "do",
    "else",
    "if",
    "import",
    "in",
    "infix",
    "infixl",
    "infixr",
    "instance",
    "let",
    "module",
    "newtype",
    "of",
    "then",
    "type",
    "where",

    "return",
    "stderr",
    "stdout",
    "main",

    "Eq",
    "Show",
    "Read",
    "Ord",
    "Num",
    "Bounded",
    "Enum",
    "Real",
    "Fractional",
    "Integral",
    "RealFrac",
    "Floating",
    "Monad",
    "RealFloat",
    "MonadPlus",
    "Functor",

    "True",
    "False",
    "Int",
    "Integer",
    "Char",
    "Bool",
    "Float",
    "Double",
    "IO",
    "Void",
    "Addr",
    "Array",
    "String",
    "Maybe",
    "Either",
    "Ratio",
    "Complex",
    "Ordering",
    "IOError",
    "IOResult",
    "ExitCode",
    "ExitSuccess",
    "GT",
    "LT",
    "EQ",
    "undefined",
    "error",
    "trace",
    NULL
};
#endif

int const cdefcount         = -1 + sizeof(cdef) / sizeof(cdef[0]);
int const cppcount          = -1 + sizeof(cpreprocessor) / sizeof(cpreprocessor[0]);
int const crwcount          = -1 + sizeof(crw) / sizeof(crw[0]);
int const copcount          = -1 + sizeof(cop) / sizeof(cop[0]);
int const cfuncount         = -1 + sizeof(cfun) / sizeof(cfun[0]);
#ifdef HAS_HASKELL_PARSER
int const haskelldefcount   = -1 + sizeof(haskelldef) / sizeof(haskelldef[0]);
int const haskellppcount    = -1 + sizeof(haskellpreprocessor) / sizeof(haskellpreprocessor[0]);
int const haskellrwcount    = -1 + sizeof(haskellrw) / sizeof(haskellrw[0]);
int const haskellspcount    = -1 + sizeof(haskellspecial) / sizeof(haskellspecial[0]);
#endif
#ifdef HAS_CMINUSMINUS_PARSER
int const cmmdefcount       = -1 + sizeof(cmmdef) / sizeof(cmmdef[0]);
int const cmmppcount        = -1 + sizeof(cmmpreprocessor) / sizeof(cmmpreprocessor[0]);
int const cmmrwcount        = -1 + sizeof(cmmrw) / sizeof(cmmrw[0]);
#endif
#ifdef HAS_NASM_PARSER
int const nasmdefcount      = -1 + sizeof(nasmdef) / sizeof(nasmdef[0]);
int const nasmppcount       = -1 + sizeof(nasmpreprocessor) / sizeof(nasmpreprocessor[0]);
int const nasmspecialcount  = -1 + sizeof(nasmspecial) / sizeof(nasmspecial[0]);
int const nasmregcount      = -1 + sizeof(nasmreg) / sizeof(nasmreg[0]);
int const nasmrwcount       = -1 + sizeof(nasmrw) / sizeof(nasmrw[0]);
#endif
#ifdef HAS_PASCAL_PARSER
int const prwcount          = -1 + sizeof(prw) / sizeof(prw[0]);
#endif