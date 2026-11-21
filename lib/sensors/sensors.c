#include "user_config.h"
#include "sensors.h"
#include "settings.h"
#include "analog.h"
#include "soil_sensor.h"
#include "light_sensor.h"
#include "dht_sensor.h"
#include "tank.h"

static sensors_t s;
static uint8_t dht_fail;

void sensors_init(void)
{
    dht_init();
    tank_init();
    light_init();            /* starts BH1750 early so the first reading is valid */
}

static void combine_soil(void)
{
    uint8_t n = 0;
    uint16_t sum = 0;
    uint8_t lowest = 100;

    for (uint8_t i = 0; i < s.soil_count; i++) {
        if (!s.soil_valid[i]) continue;
        n++;
        sum += s.soil_p[i];
        if (s.soil_p[i] < lowest) lowest = s.soil_p[i];
    }

    s.soil_ok = (n > 0);
    if (!n) {
        s.soil_pct = 0;
    } else {
#if SOIL_COMBINE_MODE == 1
        s.soil_pct = lowest;                    /* driest probe wins */
#else
        s.soil_pct = (uint8_t)(sum / n);        /* average of working probes */
#endif
    }
}

void sensors_update(void)
{
    s.rtc_ok = rtc_read(&s.time);

    s.soil_count = feature_on(FEAT_SOIL2) ? 2 : 1;
    soil_read_all(s.soil_raw, s.soil_count);
    for (uint8_t i = 0; i < 2; i++) {
        if (i < s.soil_count) {
            s.soil_valid[i] = soil_raw_valid(s.soil_raw[i]);
            s.soil_p[i] = analog_to_percent(s.soil_raw[i],
                                            g_settings.soil_dry_raw[i],
                                            g_settings.soil_wet_raw[i]);
        } else {
            s.soil_valid[i] = false;
            s.soil_p[i] = 0;
        }
    }
    combine_soil();

    if (feature_on(FEAT_LIGHT)) {
        s.light_ok = light_read_lux(&s.light_lux);
    } else {
        s.light_ok = false;
        s.light_lux = 0;
    }

    tank_update();
    s.tank_empty = feature_on(FEAT_TANK) && tank_empty();
}

void sensors_update_air(void)
{
    int8_t t;
    uint8_t h;

    if (dht_read(&t, &h)) {
        s.air_t = t;
        s.air_h = h;
        s.dht_ok = true;
        dht_fail = 0;
    } else if (++dht_fail >= 5) {      /* 5 failures in a row -> report error */
        s.dht_ok = false;
        dht_fail = 5;
    }
}

const sensors_t *sensors_get(void)
{
    return &s;
}
