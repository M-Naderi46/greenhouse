#ifndef SENSORS_H
#define SENSORS_H

#include <stdbool.h>
#include <stdint.h>
#include "rtc_ds3231.h"

typedef struct {
    rtc_t    time;          bool rtc_ok;

    uint8_t  soil_count;    /* 1 or 2 probes in use (FEAT_SOIL2)         */
    uint16_t soil_raw[2];   /* per probe                                 */
    uint8_t  soil_p[2];     /* per probe, %                              */
    bool     soil_valid[2];
    uint8_t  soil_pct;      /* combined value used for irrigation        */
    bool     soil_ok;       /* at least one probe works                  */

    uint16_t light_lux;     bool light_ok;   /* false: disabled or no answer */

    int8_t   air_t;         uint8_t air_h;     bool dht_ok;

    bool     tank_empty;    /* always false when FEAT_TANK is off        */
} sensors_t;

void sensors_init(void);
void sensors_update(void);        /* RTC + soil + light + tank (every 1 s) */
void sensors_update_air(void);    /* DHT                       (every 3 s) */
const sensors_t *sensors_get(void);

#endif
