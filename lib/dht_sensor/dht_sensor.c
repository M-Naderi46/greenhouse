#include "user_config.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include "dht_sensor.h"
#include "gpio.h"
#include "hal_twi.h"

#if DHT_USE_I2C
/* ---------------------------- DHT12 (I2C) ------------------------------- */
void dht_init(void) { }

bool dht_read(int8_t *t, uint8_t *h)
{
    uint8_t reg = 0, d[5];
    if (!twi_write(DHT12_I2C_ADDR, &reg, 1, false)) return false;
    if (!twi_read(DHT12_I2C_ADDR, d, 5)) return false;
    if ((uint8_t)(d[0] + d[1] + d[2] + d[3]) != d[4]) return false;

    int8_t tv = (int8_t)d[2];
    if (d[3] & 0x80) tv = (int8_t)-tv;
    *t = tv;
    *h = d[0];
    return true;
}

#else
/* ------------------------ DHT11 (one-wire) ------------------------------ */
void dht_init(void)
{
    DHT_PORT.DIRCLR = DHT_BM;
    gpio_pinctrl(&DHT_PORT, DHT_BM, PORT_OPC_PULLUP_gc);
}

static bool wait_level(bool level, uint8_t us)
{
    while (us--) {
        if (gpio_read(&DHT_PORT, DHT_BM) == level) return true;
        _delay_us(1);
    }
    return false;
}

bool dht_read(int8_t *t, uint8_t *h)
{
    uint8_t d[5] = { 0, 0, 0, 0, 0 };
    bool ok = false;

    DHT_PORT.OUTCLR = DHT_BM;            /* start signal: low >= 18 ms */
    DHT_PORT.DIRSET = DHT_BM;
    _delay_ms(20);

    uint8_t sreg = SREG;
    cli();                               /* timing critical part (~4 ms) */
    DHT_PORT.OUTSET = DHT_BM;
    _delay_us(30);
    DHT_PORT.DIRCLR = DHT_BM;            /* release the line */

    do {
        if (!wait_level(false, 100)) break;
        if (!wait_level(true, 100))  break;
        if (!wait_level(false, 100)) break;

        uint8_t i;
        for (i = 0; i < 40; i++) {
            if (!wait_level(true, 100)) break;
            _delay_us(40);               /* '0' ~27 us, '1' ~70 us */
            d[i >> 3] <<= 1;
            if (gpio_read(&DHT_PORT, DHT_BM)) d[i >> 3] |= 1;
            if (!wait_level(false, 100)) break;
        }
        if (i < 40) break;
        ok = true;
    } while (0);

    SREG = sreg;
    DHT_PORT.DIRCLR = DHT_BM;

    if (!ok) return false;
    if ((uint8_t)(d[0] + d[1] + d[2] + d[3]) != d[4]) return false;
    *h = d[0];
    *t = (int8_t)d[2];
    return true;
}
#endif
