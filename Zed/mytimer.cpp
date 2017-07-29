
#include "mytimer.h"

#define CMOS
//#define CHAINED

Liquid_timer::liq_mode  Liquid_timer::mode = Liquid_timer::MODE1;
volatile dword          Liquid_timer::liq_ticks = 0;
double                  Liquid_timer::elapsed = 0;
Liquid_timer::uniq_t    Liquid_timer::uniq[16] =
{
    {0, 0}, {0, 0}, {0, 0},
    {1.220865605642927e-04, 3}, // 8192 hz (max; non-stable under multitaskers)
    {0, 0}, {0, 0},
#ifdef CMOS
    {9.766925838825481e-04, 6}, // 1024 hz (BIOS default)
#else
    {1080./19663, 0},
#endif
    {0, 0}, {0, 0},
    {7.813532191648454e-03, 9}, // 128 hz
    {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}
};
//_go32_dpmi_seginfo Liquid_timer::old_vector, Liquid_timer::new_vector;

#define START_CRITICAL()

#define END_CRITICAL()

#define inbyte(port)
#define outbyte(port, b)

#define END_OF_FUNCTION(x)    
#define LOCK_VARIABLE(x)      
#define LOCK_FUNCTION(x)      
#define UNLOCK_VARIABLE(x)    
#define UNLOCK_FUNCTION(x)    
#define LOCK_DATA(d,s)        
#define LOCK_CODE(c,s)        
#define UNLOCK_DATA(d,s)      
#define UNLOCK_CODE(c,s)      

#define NONSPECIFIC_EOI         (0x20)
#define PIC                     (0x20)
#define PIC_B                   (0xa0)

static void unlock_dpmi_data(void *addr, int size)
{
/*   unsigned long baseaddr;
   __dpmi_meminfo mem;
   assert(__dpmi_get_segment_base_address(_go32_my_ds(), &baseaddr) != -1);
   mem.address = baseaddr + (unsigned long)addr;
   mem.size = size;
   assert(__dpmi_unlock_linear_region(&mem ) != -1);
*/
}

static void unlock_dpmi_code(void *addr, int size)
{
/*   unsigned long baseaddr;
   __dpmi_meminfo mem;
   assert(__dpmi_get_segment_base_address(_go32_my_cs(), &baseaddr) != -1);
   mem.address = baseaddr + (unsigned long)addr;
   mem.size = size;
   assert(__dpmi_unlock_linear_region(&mem ) != -1);*/
}

void periodic_int_handler() /* must be chained !!! */
{
/*    asm volatile ("pusha");
#ifdef CMOS
    outbyte(0x70, 0xb);
    byte b = inbyte(0x71);
    outbyte(0x70, 0xc);
    if (((inbyte(0x71) & b) & 0x40) != 0)
        Liquid_timer::liq_ticks++;
#ifndef CHAINED
    outbyte(PIC_B, NONSPECIFIC_EOI);
    outbyte(PIC, NONSPECIFIC_EOI);
#endif
#else
    Liquid_timer::liq_ticks++;
#ifndef CHAINED
    outbyte(PIC, NONSPECIFIC_EOI);
#endif
#endif
    asm volatile ("popa");*/
}

void Liquid_timer::start()
{
/*    if (its_fake)
        return;

    LOCK_VARIABLE(old_vector);
    LOCK_VARIABLE(liq_ticks);
    LOCK_VARIABLE(new_vector);
    LOCK_FUNCTION(periodic_int_handler);

    START_CRITICAL();
    {
        _go32_dpmi_get_protected_mode_interrupt_vector(LIQ_IRQ, &old_vector);
        new_vector.pm_offset = reinterpret_cast<unsigned long>(periodic_int_handler);
        new_vector.pm_selector = _go32_my_cs();
        assert(_go32_dpmi_allocate_iret_wrapper(&new_vector) == 0);
#ifdef CHAINED
        assert(_go32_dpmi_chain_protected_mode_interrupt_vector(LIQ_IRQ, &new_vector) == 0);
#else
        assert(_go32_dpmi_set_protected_mode_interrupt_vector(LIQ_IRQ, &new_vector) == 0);
#endif
    }

    disable();

    resolution = uniq[mode].r1;
    RS = uniq[mode].r2;
    liq_ticks = 0;

#ifdef CMOS
    outbyte(0x70, 0xc);
    inbyte(0x71);
    outbyte(0x70, 0xa);
    outbyte(0x71, (inbyte(0x71) & 0xf0) | RS);
    outbyte(0x70, 0xb);
    outbyte(0x71, (inbyte(0x71) & 0x7f) | 0x40);
    outbyte(0xa1, inbyte(0xa1) & 0xfe);
#endif

    enable();
	*/
}

void Liquid_timer::stop()
{
/*    if (its_fake)
        return;

    disable();

    double elapsed = liq_ticks * resolution;

#ifdef CMOS
    outbyte(0xa1, inbyte(0xa1) | 1);
    outbyte(0x70, 0xb);
    outbyte(0x71, inbyte(0x71) & 0xbf);
    outbyte(0x70, 0xa);
    outbyte(0x71, (inbyte(0x71) & 0xf0) | 6);
    outbyte(0x70, 0xc);
    inbyte(0x71);
#endif

    enable();

    {
        assert(_go32_dpmi_set_protected_mode_interrupt_vector(LIQ_IRQ, &old_vector) == 0);
        assert(_go32_dpmi_free_iret_wrapper(&new_vector) == 0);
    }
    END_CRITICAL();

    UNLOCK_FUNCTION(periodic_int_handler);
    UNLOCK_VARIABLE(new_vector);
    UNLOCK_VARIABLE(liq_ticks);
    UNLOCK_VARIABLE(old_vector);

    if (callback)
        callback(elapsed);
    Liquid_timer::elapsed = elapsed;
	*/
}
