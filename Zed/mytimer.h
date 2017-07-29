#ifndef __MYTIMER_H__
#define __MYTIMER_H__

#include <assert.h>
#include "pmt.h"

class Basic_timer
{
public:
    typedef void (*cb_t)(double);

    Basic_timer(cb_t, bool) {}
    virtual ~Basic_timer() {}

private:
    virtual void set_callback(cb_t) =0;
    virtual void start() =0;
    virtual void stop() =0;
};

class Liquid_timer : public Basic_timer
{
public:
    Liquid_timer(cb_t cb, bool fake) :
        Basic_timer(cb, fake),
        its_fake(fake)
    {
        set_callback(cb);
        start();
    }
    ~Liquid_timer()
    {
        stop();
    }

    enum liq_mode
    {
        MODE1 = 6,
        MODE2 = 3,
        MODE3 = 9
    };

    static liq_mode mode;
    static double elapsed;

private:
    struct uniq_t
    {
        double r1;
        byte r2;
    };

    virtual void set_callback(cb_t cb)
    {
        callback = cb;
    }

    friend void periodic_int_handler();

    virtual void start();
    virtual void stop();

private:
    bool its_fake;
    static volatile dword liq_ticks;
    static uniq_t uniq[16];
//    static _go32_dpmi_seginfo old_vector, new_vector;
    double resolution;
    byte RS;
    cb_t callback;
};

#endif //__MYTIMER_H__
