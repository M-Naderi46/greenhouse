#include "user_config.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "flow_meter.h"
#include "gpio.h"

static volatile uint32_t pulses;

ISR(FLOW_INT_VECT)
{
    pulses++;
}

void flow_init(void)
{
    FLOW_PORT.DIRCLR = FLOW_BM;
    gpio_pinctrl(&FLOW_PORT, FLOW_BM, PORT_ISC_RISING_gc | PORT_OPC_PULLUP_gc);
    FLOW_PORT.INT0MASK = FLOW_BM;
    FLOW_PORT.INTCTRL = PORT_INT0LVL_LO_gc;
}

uint32_t flow_pulses(void)
{
    uint32_t v;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { v = pulses; }
    return v;
}
