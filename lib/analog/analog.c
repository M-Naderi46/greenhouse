#include "user_config.h"
#include <avr/pgmspace.h>
#include "analog.h"
#include "gpio.h"

static uint8_t read_calib_byte(uint8_t index)
{
    uint8_t r;
    NVM.CMD = NVM_CMD_READ_CALIB_ROW_gc;
    r = pgm_read_byte(index);
    NVM.CMD = NVM_CMD_NO_OPERATION_gc;
    return r;
}

void analog_init(void)
{
    ANALOG_PORT.DIRCLR = ANALOG_PIN_MASK;
    gpio_pinctrl(&ANALOG_PORT, ANALOG_PIN_MASK, PORT_ISC_INPUT_DISABLE_gc);

    ANALOG_ADC.CALL = read_calib_byte(ANALOG_ADC_CAL_OFFSET);
    ANALOG_ADC.CALH = read_calib_byte(ANALOG_ADC_CAL_OFFSET + 1);
    ANALOG_ADC.CTRLB = ADC_RESOLUTION_12BIT_gc;          /* unsigned, single ended */
    ANALOG_ADC.REFCTRL = ANALOG_REFSEL;
    ANALOG_ADC.PRESCALER = ADC_PRESCALER_DIV32_gc;       /* 1 MHz ADC clock */
    ANALOG_ADC.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
    ANALOG_ADC.CTRLA = ADC_ENABLE_bm;
}

static uint16_t adc_once(void)
{
    ANALOG_ADC.CH0.CTRL |= ADC_CH_START_bm;
    while (!(ANALOG_ADC.CH0.INTFLAGS & ADC_CH_CHIF_bm)) { }
    ANALOG_ADC.CH0.INTFLAGS = ADC_CH_CHIF_bm;
    return ANALOG_ADC.CH0.RES;
}

uint16_t analog_read(uint8_t pin)
{
    uint32_t sum = 0;

    ANALOG_ADC.CH0.MUXCTRL = (uint8_t)(pin << ADC_CH_MUXPOS_gp);
    (void)adc_once();                                    /* dummy: let the mux settle */
    for (uint8_t i = 0; i < 16; i++) sum += adc_once();
    return (uint16_t)(sum >> 4);
}

uint8_t analog_to_percent(uint16_t raw, uint16_t raw_at_0, uint16_t raw_at_100)
{
    int32_t a = raw_at_0, b = raw_at_100;
    if (a == b) return 0;
    int32_t p = ((a - (int32_t)raw) * 100) / (a - b);
    if (p < 0) p = 0;
    if (p > 100) p = 100;
    return (uint8_t)p;
}
