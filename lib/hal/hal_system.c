#include "user_config.h"
#include <avr/interrupt.h>
#include "hal_system.h"

void system_clock_init(void)
{
    OSC.CTRL |= OSC_RC32MEN_bm;
    while (!(OSC.STATUS & OSC_RC32MRDY_bm)) { }
    CCP = CCP_IOREG_gc;
    CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
}

void system_enable_interrupts(void)
{
    PMIC.CTRL = PMIC_LOLVLEN_bm;
    sei();
}

void system_wdt_start(void)
{
#if WDT_ENABLED
    CCP = CCP_IOREG_gc;
    WDT.CTRL = WDT_PER_4KCLK_gc | WDT_ENABLE_bm | WDT_CEN_bm;
    while (WDT.STATUS & WDT_SYNCBUSY_bm) { }
#endif
}
