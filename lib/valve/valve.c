#include "user_config.h"
#include "valve.h"
#include "gpio.h"

static bool is_open;

void valve_set(bool open)
{
    gpio_write(&VALVE_PORT, VALVE_BM, VALVE_ACTIVE_HIGH ? open : !open);
    is_open = open;
}

void valve_init(void)
{
    valve_set(false);                 /* set level first, then make it an output */
    VALVE_PORT.DIRSET = VALVE_BM;
}

bool valve_is_open(void)
{
    return is_open;
}
