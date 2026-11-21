#include "user_config.h"
#include "buzzer.h"
#include "gpio.h"
#include "hal_timebase.h"

void buzzer_init(void)
{
    gpio_write(&ALARM_PORT, ALARM_BM, !ALARM_ACTIVE_HIGH);
    ALARM_PORT.DIRSET = ALARM_BM;
}

void buzzer_task(bool alarm_on)
{
    static uint32_t t;
    uint32_t now = millis();

    if (alarm_on) {
        if (now - t >= 500) {
            t = now;
            ALARM_PORT.OUTTGL = ALARM_BM;
        }
    } else {
        gpio_write(&ALARM_PORT, ALARM_BM, !ALARM_ACTIVE_HIGH);
    }
}
