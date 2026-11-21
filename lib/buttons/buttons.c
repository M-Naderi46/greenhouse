#include "user_config.h"
#include <stdbool.h>
#include "buttons.h"
#include "gpio.h"
#include "hal_timebase.h"

#define LONG_PRESS_MS 1000UL

static uint8_t  last;
static uint8_t  repeats;
static uint32_t t_scan, t_hold, t_mode;
static bool     mode_long;

void buttons_init(void)
{
    BTN_PORT.DIRCLR = BTN_ALL_BM;
#if BTN_ACTIVE_LOW
    gpio_pinctrl(&BTN_PORT, BTN_ALL_BM, PORT_OPC_PULLUP_gc);
#else
    gpio_pinctrl(&BTN_PORT, BTN_ALL_BM, PORT_OPC_PULLDOWN_gc);
#endif
}

static uint8_t read_keys(void)
{
    uint8_t in = BTN_PORT.IN;
#if BTN_ACTIVE_LOW
    in = (uint8_t)~in;
#endif
    uint8_t k = 0;
    if (in & BTN_MODE_BM) k |= KEY_MODE;
    if (in & BTN_UP_BM)   k |= KEY_UP;
    if (in & BTN_DOWN_BM) k |= KEY_DOWN;
    return k;
}

uint8_t buttons_poll(void)
{
    uint32_t now = millis();
    if (now - t_scan < 25) return 0;          /* 25 ms scan = debounce */
    t_scan = now;

    uint8_t cur = read_keys();
    uint8_t press = cur & (uint8_t)~last;
    uint8_t ev = press & (KEY_UP | KEY_DOWN);

    /* MODE: short press on release, long press while held */
    if (press & KEY_MODE) { t_mode = now; mode_long = false; }
    if ((cur & KEY_MODE) && !mode_long && (now - t_mode) >= LONG_PRESS_MS) {
        ev |= KEY_MODE_LONG;
        mode_long = true;
    }
    if ((last & KEY_MODE) && !(cur & KEY_MODE) && !mode_long) ev |= KEY_MODE;

    /* UP / DOWN auto-repeat */
    if (cur != last) {
        t_hold = now;
        repeats = 0;
    } else if ((cur & (KEY_UP | KEY_DOWN)) && (now - t_hold) > 500) {
        ev |= cur & (KEY_UP | KEY_DOWN);
        t_hold = now - 350;                   /* then every 150 ms */
        if (repeats < 255) repeats++;
    }
    last = cur;
    return ev;
}

uint8_t buttons_repeat_count(void)
{
    return repeats;
}
