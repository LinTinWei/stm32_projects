#ifndef UART_H
#define UART_H

#include <stdint.h>

// Initialize USART1 (PA9: TX, PA10: RX)
void uart_init();

// send one character
void uart_send_char(char c);

// send serial character (end with '\0')
void uart_print(const char *s);

// receive one character (polling)
char uart_recv_char(void);

// delay
void delay(volatile int count);

#endif
