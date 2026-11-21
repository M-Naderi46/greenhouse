#include "user_config.h"
#include <util/delay.h>
#include "hal_twi.h"

void twi_init(void)
{
    I2C_TWI.MASTER.BAUD  = (uint8_t)((F_CPU / (2UL * I2C_HZ)) - 5);
    I2C_TWI.MASTER.CTRLA = TWI_MASTER_ENABLE_bm;
    I2C_TWI.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
}

static void twi_abort(void)
{
    I2C_TWI.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
    I2C_TWI.MASTER.STATUS = TWI_MASTER_ARBLOST_bm | TWI_MASTER_BUSERR_bm |
                            TWI_MASTER_WIF_bm | TWI_MASTER_RIF_bm |
                            TWI_MASTER_BUSSTATE_IDLE_gc;
}

/* wait for a transferred byte; false on timeout or bus error */
static bool twi_wait(void)
{
    uint16_t t = 20000;
    while (!(I2C_TWI.MASTER.STATUS & (TWI_MASTER_WIF_bm | TWI_MASTER_RIF_bm))) {
        if (--t == 0) return false;
        _delay_us(2);
    }
    if (I2C_TWI.MASTER.STATUS & (TWI_MASTER_ARBLOST_bm | TWI_MASTER_BUSERR_bm))
        return false;
    return true;
}

bool twi_write(uint8_t addr, const uint8_t *buf, uint8_t len, bool stop)
{
    I2C_TWI.MASTER.ADDR = (uint8_t)(addr << 1);
    if (!twi_wait() || (I2C_TWI.MASTER.STATUS & TWI_MASTER_RXACK_bm)) {
        twi_abort();
        return false;
    }
    for (uint8_t i = 0; i < len; i++) {
        I2C_TWI.MASTER.DATA = buf[i];
        if (!twi_wait() || (I2C_TWI.MASTER.STATUS & TWI_MASTER_RXACK_bm)) {
            twi_abort();
            return false;
        }
    }
    if (stop) I2C_TWI.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
    return true;
}

bool twi_read(uint8_t addr, uint8_t *buf, uint8_t len)
{
    I2C_TWI.MASTER.ADDR = (uint8_t)((addr << 1) | 1);
    for (uint8_t i = 0; i < len; i++) {
        if (!twi_wait() || !(I2C_TWI.MASTER.STATUS & TWI_MASTER_RIF_bm)) {
            twi_abort();
            return false;
        }
        buf[i] = I2C_TWI.MASTER.DATA;
        if (i == len - 1)
            I2C_TWI.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
        else
            I2C_TWI.MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
    }
    return true;
}
