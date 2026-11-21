#include "user_config.h"
#include "outputs.h"
#include "gpio.h"

typedef struct {
    PORT_t *port;
    uint8_t bm;
    uint8_t active_high;
} out_cfg_t;

static const out_cfg_t cfg[OUT_COUNT] = {
    [OUT_FAN]    = { &FAN_PORT,    BM(FAN_PIN),    FAN_ACTIVE_HIGH },
    [OUT_HEATER] = { &HEATER_PORT, BM(HEATER_PIN), HEATER_ACTIVE_HIGH },
    [OUT_LAMP]   = { &LAMP_PORT,   BM(LAMP_PIN),   LAMP_ACTIVE_HIGH },
    [OUT_PUMP]   = { &PUMP_PORT,   BM(PUMP_PIN),   PUMP_ACTIVE_HIGH },
};

static bool state[OUT_COUNT];

void outputs_set(out_id_t id, bool on)
{
    if (id >= OUT_COUNT) return;
    gpio_write(cfg[id].port, cfg[id].bm, cfg[id].active_high ? on : !on);
    state[id] = on;
}

void outputs_init(void)
{
    for (uint8_t i = 0; i < OUT_COUNT; i++) {
        outputs_set((out_id_t)i, false);          /* level first, then output */
        cfg[i].port->DIRSET = cfg[i].bm;
    }
}

bool outputs_get(out_id_t id)
{
    return id < OUT_COUNT ? state[id] : false;
}
