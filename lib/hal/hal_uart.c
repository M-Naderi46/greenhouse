#include "user_config.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "hal_uart.h"

/* 256-byte ring buffer: uint8_t indices wrap by themselves.
 * One slot stays free to tell "full" from "empty" (255 bytes usable). */
static uint8_t buf[256];
static volatile uint8_t head, tail;

void uart_init(void)
{
    UART_PORT.OUTSET = BM(UART_TX_PIN);          /* idle high */
    UART_PORT.DIRSET = BM(UART_TX_PIN);
    UART_PORT.DIRCLR = BM(UART_RX_PIN);

    uint16_t bsel = (uint16_t)((F_CPU + 8UL * UART_BAUD) / (16UL * UART_BAUD) - 1UL);
    UART_USART.CTRLC = USART_CHSIZE_8BIT_gc;     /* async, 8N1 */
    UART_USART.BAUDCTRLA = (uint8_t)(bsel & 0xFF);
    UART_USART.BAUDCTRLB = (uint8_t)((bsel >> 8) & 0x0F);
    UART_USART.CTRLB = USART_TXEN_bm;
}

ISR(UART_DRE_VECT)
{
    if (head == tail) {
        UART_USART.CTRLA = 0;                    /* nothing left: stop DRE irq */
        return;
    }
    UART_USART.DATA = buf[tail];
    tail++;
}

bool uart_write(const char *data, uint8_t len)
{
    bool ok = false;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        uint8_t used = (uint8_t)(head - tail);
        uint8_t room = (uint8_t)(255 - used);
        if (len <= room) {
            for (uint8_t i = 0; i < len; i++) buf[head++] = (uint8_t)data[i];
            UART_USART.CTRLA = USART_DREINTLVL_LO_gc;
            ok = true;
        }
    }
    return ok;
}
