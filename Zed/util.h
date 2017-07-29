/*
** util.h
**
** Defines some ZED utilities.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __UTIL_H__
#define __UTIL_H__

inline short __dpmi_get_flags()
{
/*    short r;
    asm volatile (
        "movw $0x400,%%ax\n"
        "int $0x31\n"
        "movw %%bx,%0\n"
        : "=g" (r)
        :
        : "cc", "%eax", "%ebx", "%ecx", "%edx"
    );
    return r;
*/
    return 0;
}

// CH = memory allocation type (0=INT15, 1=XMS, 2=VCPI, 3=DPMI)
inline int __dpmi_ident_call(unsigned char &t)
{
/*    int r;
    asm volatile (
        "movl %1,%%esi\n"
        "pushw %%es\n"
        "movw $0xeeff,%%ax\n"
        "int $0x31\n"
        "setc %%al\n"
        "popw %%es\n"
        "movb %%ch,(%%esi)\n"
        "movzbl %%al,%%eax\n"
        "movl %%eax,%0\n"
        : "=g" (r)
        : "g" (&t)
        : "cc", "%eax", "%ebx", "%ecx", "%edx"
    );
    return r;
*/
        return 0;
}

inline void mydelay(DWORD millisec)
{
/*    asm volatile (
        "movl %0,%%edx\n"
        "movl $0x8600,%%eax\n"
        "xorl %%ecx,%%ecx\n"
        "imul $1000,%%edx\n"
        "shld $16,%%edx,%%ecx\n"
        "int $0x15\n"
        :
        : "g" (millisec)
        : "cc", "%eax", "%ecx", "%edx"
    );
*/
}

unsigned long memavail()
{
/*    __dpmi_free_mem_info t;
    __dpmi_get_free_memory_information(&t);
    return t.largest_available_free_block_in_bytes;
*/
        return 0;
}

#endif //__UTIL_H__
