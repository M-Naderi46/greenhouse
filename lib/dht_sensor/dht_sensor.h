#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <stdbool.h>
#include <stdint.h>

void dht_init(void);
bool dht_read(int8_t *temp_c, uint8_t *hum_pct);   /* false on error/checksum */

#endif
