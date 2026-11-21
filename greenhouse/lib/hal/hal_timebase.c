#include "user_config.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "hal_timebase.h"

static volatile uint32_t ms_ticks;

ISR(TCC0_OVF_vect)
{
    ms_ticks++;
}

void timebase_init(void)
{
    TCC0.PER = (uint16_t)(F_CPU / 64UL / 1000UL - 1UL);
    TCC0.CTRLA = TC_CLKSEL_DIV64_gc;
    TCC0.INTCTRLA = TC_OVFINTLVL_LO_gc;
}

uint32_t millis(void)
{
    uint32_t v;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { v = ms_ticks; }
    return v;
}
