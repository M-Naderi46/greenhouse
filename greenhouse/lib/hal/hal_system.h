#ifndef HAL_SYSTEM_H
#define HAL_SYSTEM_H

void system_clock_init(void);        /* 32 MHz internal RC                */
void system_enable_interrupts(void); /* PMIC low level + sei()            */
void system_wdt_start(void);         /* ~4 s watchdog (if WDT_ENABLED)    */

#endif
