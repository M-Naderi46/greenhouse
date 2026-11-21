#ifndef GPIO_H
#define GPIO_H

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

/* Write the same PINnCTRL value to every pin in 'mask' of a port. */
static inline void gpio_pinctrl(PORT_t *port, uint8_t mask, uint8_t ctrl)
{
    PORTCFG.MPCMASK = mask;
    port->PIN0CTRL = ctrl;
}

static inline void gpio_write(PORT_t *port, uint8_t bm, bool level)
{
    if (level) port->OUTSET = bm; else port->OUTCLR = bm;
}

static inline bool gpio_read(PORT_t *port, uint8_t bm)
{
    return (port->IN & bm) != 0;
}

#endif
