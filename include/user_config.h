/*
 * ============================================================================
 *  user_config.h  -  pins, hardware options and factory defaults.
 * ============================================================================
 *  1. Default feature switches  (can be changed later from the on-device menu)
 *  2. Pin assignment            (PORT + pin number 0..7)
 *  3. Timing
 *  4. Hardware options          (I2C bus, LCD, RTC, DHT, BH1750, ADC, UART)
 *  5. Factory default settings  (stored in EEPROM, editable from the menu)
 *  6. Safety limits and control behaviour
 *
 *  After changing a pin, make sure it does not collide with another function
 *  (TWID uses PD0/PD1, so do not put the valve/fan there if you move the I2C
 *  bus to TWID; USARTC0 uses PC2/PC3, TWIC uses PC0/PC1).
 *  Soil probes must be on ANALOG_PORT.
 * ============================================================================
 */
#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#include <avr/io.h>
#include <stdint.h>

#ifndef F_CPU
#define F_CPU 32000000UL          /* internal 32 MHz RC oscillator - keep */
#endif

#define BM(pin)  ((uint8_t)(1u << (pin)))

/* ========================================================================== */
/* 1. DEFAULT FEATURE SWITCHES (1 = on)                                       */
/*    Only the FACTORY value: afterwards use  menu > FEATURES  to change them */
/*    (stored in EEPROM, applied after "Save and exit").                      */
/* ========================================================================== */
#define DEFAULT_IRRIG_ENABLED   1   /* automatic watering                      */
#define DEFAULT_SOIL2_ENABLED   0   /* second soil moisture probe              */
#define DEFAULT_LIGHT_ENABLED   1   /* BH1750 light sensor                     */
#define DEFAULT_TANK_ENABLED    0   /* float switch in the water tank          */
#define DEFAULT_PUMP_ENABLED    0   /* water pump (tank mode); runs with the valve */
#define DEFAULT_FAN_ENABLED     1   /* ventilation fan relay                   */
#define DEFAULT_HEATER_ENABLED  1   /* heater relay                            */
#define DEFAULT_LAMP_ENABLED    1   /* grow-light relay (needs the light sensor) */
#define DEFAULT_LOG_ENABLED     1   /* CSV data on UART (for ESP / PC)         */

/* ========================================================================== */
/* 2. PIN ASSIGNMENT                                                          */
/* ========================================================================== */

/* --- Solenoid valve driver (MOSFET / relay module) ----------------------- */
#define VALVE_PORT          PORTD
#define VALVE_PIN           0
#define VALVE_ACTIVE_HIGH   1             /* 1: pin HIGH = valve open        */

/* --- Climate relays ------------------------------------------------------- */
#define FAN_PORT            PORTD
#define FAN_PIN             1
#define FAN_ACTIVE_HIGH     1

#define HEATER_PORT         PORTD
#define HEATER_PIN          4
#define HEATER_ACTIVE_HIGH  1

#define LAMP_PORT           PORTD
#define LAMP_PIN            5
#define LAMP_ACTIVE_HIGH    1

/* --- Water pump relay (tank mode) ------------------------------------------
 *  The pump runs only while the valve is open, and only if the tank is not
 *  empty (needs the tank float switch to protect the pump from dry running).
 */
#define PUMP_PORT           PORTD
#define PUMP_PIN            7
#define PUMP_ACTIVE_HIGH    1

/* --- Buzzer or alarm LED -------------------------------------------------- */
#define ALARM_PORT          PORTD
#define ALARM_PIN           3
#define ALARM_ACTIVE_HIGH   1

/* --- Flow meter pulse input (max 3.3 V!) ---------------------------------- */
#define FLOW_PORT           PORTD
#define FLOW_PIN            2
#define FLOW_INT_VECT       PORTD_INT0_vect   /* MUST match FLOW_PORT:
                                                 PORTC_INT0_vect, PORTE_INT0_vect ... */

/* --- Tank float switch (to GND, internal pull-up) ------------------------- */
#define TANK_PORT           PORTD
#define TANK_PIN            6
#define TANK_EMPTY_WHEN_OPEN 1            /* 1: contact open = tank empty
                                             (also a cut wire = "empty", fail-safe) */

/* --- DHT11 data pin (only used when DHT_USE_I2C = 0) ---------------------- */
#define DHT_PORT            PORTE
#define DHT_PIN             0

/* --- Push buttons (to GND, internal pull-up) ------------------------------ */
#define BTN_PORT            PORTF
#define BTN_MODE_PIN        0
#define BTN_UP_PIN          1
#define BTN_DOWN_PIN        2
#define BTN_ACTIVE_LOW      1             /* 1: pressed = GND                */

/* --- Soil probes (analog, ADC) ---------------------------------------------
 *  ADCA works with PORTA, ADCB with PORTB.
 */
#define ANALOG_ADC          ADCA
#define ANALOG_PORT         PORTA
#define ANALOG_ADC_CAL_OFFSET 0x20        /* 0x20 for ADCA, 0x24 for ADCB    */

#define SOIL1_PIN           0             /* soil moisture probe 1           */
#define SOIL2_PIN           2             /* soil moisture probe 2 (optional)*/

/* optional: power the soil probes only while measuring (saves the probes) -- */
#define SOIL_POWER_SWITCHED 0
#define SOIL_POWER_PORT     PORTA
#define SOIL_POWER_PIN      1

/* --- I2C bus (LCD + RTC + BH1750 + optional DHT12) -------------------------
 *  TWIC = PC0 (SDA), PC1 (SCL)      TWID = PD0, PD1
 *  TWIE = PE0, PE1                  TWIF = PF0, PF1
 */
#define I2C_TWI             TWIC
#define I2C_HZ              100000UL

/* --- UART for data logging (TX only is used) ------------------------------ */
#define UART_USART          USARTC0
#define UART_PORT           PORTC
#define UART_RX_PIN         2
#define UART_TX_PIN         3
#define UART_DRE_VECT       USARTC0_DRE_vect   /* MUST match UART_USART        */
#define UART_BAUD           9600UL

/* ========================================================================== */
/* 3. TIMING                                                                  */
/* ========================================================================== */
#define SENSOR_PERIOD_MS    1000UL        /* RTC + soil + light + logic      */
#define DHT_PERIOD_MS       3000UL        /* DHT11 needs >= 1 s              */
#define LCD_REFRESH_MS      500UL
#define LCD_REINIT_MS       600000UL      /* re-init LCD every 10 min        */
#define MENU_TIMEOUT_MS     60000UL       /* leave menu after 60 s idle      */
#define PAGE_RETURN_MS      30000UL       /* main screen returns to page 1   */
#define LOG_PERIOD_S        10            /* one CSV line every N seconds    */

/* ========================================================================== */
/* 4. HARDWARE OPTIONS                                                        */
/* ========================================================================== */

/* --- LCD 20x4 with PCF8574 backpack --------------------------------------- */
#define LCD_I2C_ADDR        0x27          /* PCF8574 = 0x27, PCF8574A = 0x3F */
#define LCD_COLS            20
#define LCD_ROWS            4
/* PCF8574 bit mapping (most backpacks: RS=P0 RW=P1 EN=P2 BL=P3, D4..D7=P4..P7) */
#define LCD_RS_BIT          0x01
#define LCD_EN_BIT          0x04
#define LCD_BL_BIT          0x08

/* --- RTC ------------------------------------------------------------------ */
#define RTC_I2C_ADDR        0x68          /* DS3231 / DS1307                 */

/* --- Light sensor BH1750 (GY-302 module) -----------------------------------
 *  ADDR pin low/open = 0x23, ADDR pin high = 0x5C (clashes with DHT12!)
 */
#define BH1750_I2C_ADDR     0x23

/* --- Air temperature / humidity sensor ------------------------------------ */
#define DHT_USE_I2C         0             /* 0 = DHT11 one-wire, 1 = DHT12 I2C */
#define DHT12_I2C_ADDR      0x5C

/* --- ADC ------------------------------------------------------------------
 *  REFSEL 0x10 = VCC/1.6 (about 2.06 V at 3.3 V). Probe outputs must stay
 *  BELOW this voltage (use a divider for a capacitive soil probe).
 *  0x00 = internal 1.00 V, 0x20 = AREFA pin, 0x30 = AREFB pin.
 */
#define ANALOG_REFSEL       0x10
#define SOIL_RAW_MIN_VALID  50            /* below  -> probe shorted/open    */
#define SOIL_RAW_MAX_VALID  4090          /* above  -> probe missing         */

/* How two soil probes are combined:  0 = average,  1 = driest probe wins     */
#define SOIL_COMBINE_MODE   0

/* ========================================================================== */
/* 5. FACTORY DEFAULT SETTINGS  (editable later from the on-device menu)      */
/* ========================================================================== */
/* irrigation */
#define DEFAULT_SOIL_LOW          35      /* % : start watering below        */
#define DEFAULT_SOIL_HIGH         60      /* % : stop watering above         */
#define DEFAULT_WIN_START         6       /* allowed hours: start            */
#define DEFAULT_WIN_END           20      /*                end (== start -> 24 h) */
#define DEFAULT_MAX_LITERS        10      /* litres per watering cycle       */
#define DEFAULT_MAX_RUN_MIN       10      /* minutes per watering cycle      */
#define DEFAULT_COOLDOWN_MIN      10      /* pause between cycles            */
#define DEFAULT_MIN_TEMP_C        3       /* no watering at/below this temp  */
#define DEFAULT_PULSES_PER_LITER  450     /* YF-S201 ~450 - calibrate!       */
#define DEFAULT_SOIL_DRY_RAW      2800    /* ADC value in air   - calibrate! */
#define DEFAULT_SOIL_WET_RAW      1300    /* ADC value in water - calibrate! */
/* climate */
#define DEFAULT_FAN_ON_C          30      /* fan on at/above this temp       */
#define DEFAULT_HEAT_ON_C         12      /* heater on at/below this temp    */
#define DEFAULT_HUM_MAX           85      /* fan on above this RH % (100 = off) */
/* lamp (BH1750 gives lux; overcast day ~ 1000-10000 lx, full sun ~ 100000 lx) */
#define DEFAULT_LAMP_START        6       /* lamp allowed from this hour     */
#define DEFAULT_LAMP_END          20      /* ... until this hour (== start -> 24 h) */
#define DEFAULT_LIGHT_LOW_LUX     5000    /* lamp ON  below this many lux    */
#define DEFAULT_LIGHT_HIGH_LUX    15000   /* lamp OFF above this many lux    */

/* ========================================================================== */
/* 6. SAFETY LIMITS AND CONTROL BEHAVIOUR                                     */
/* ========================================================================== */
#define FLOW_CHECK_S        10            /* after this many s flow must exist */
#define MIN_FLOW_PULSES     20            /* ... at least this many pulses   */
#define LEAK_GRACE_S        8             /* ignore pulses after closing     */
#define LEAK_PULSES         100           /* pulses with valve closed = leak */
#define WDT_ENABLED         1             /* ~4 s watchdog                   */

#define CLIMATE_HYST_C      2             /* fan/heater hysteresis (deg C)   */
#define CLIMATE_HUM_HYST    5             /* humidity hysteresis (% RH)      */
#define CLIMATE_MIN_STATE_S 30            /* min time between fan/heater changes */
#define LAMP_MIN_STATE_S    300           /* min time between lamp changes   */
#define FAN_ON_SENSOR_FAIL  1             /* DHT failed: 1 = run fan, heater is always off */

#define PUMP_START_DELAY_S  2             /* pump starts this long AFTER the valve opens */

#define TANK_DEBOUNCE_S     5             /* switch must be stable this long */
#define TANK_ALARM          1             /* buzzer while tank is empty      */

/* ========================================================================== */
/*  DERIVED MACROS  -  do not edit                                            */
/* ========================================================================== */
#define VALVE_BM        BM(VALVE_PIN)
#define ALARM_BM        BM(ALARM_PIN)
#define FLOW_BM         BM(FLOW_PIN)
#define TANK_BM         BM(TANK_PIN)
#define DHT_BM          BM(DHT_PIN)
#define SOILPWR_BM      BM(SOIL_POWER_PIN)
#define BTN_MODE_BM     BM(BTN_MODE_PIN)
#define BTN_UP_BM       BM(BTN_UP_PIN)
#define BTN_DOWN_BM     BM(BTN_DOWN_PIN)
#define BTN_ALL_BM      ((uint8_t)(BTN_MODE_BM | BTN_UP_BM | BTN_DOWN_BM))
#define ANALOG_PIN_MASK ((uint8_t)(BM(SOIL1_PIN) | BM(SOIL2_PIN)))

#endif /* USER_CONFIG_H */
