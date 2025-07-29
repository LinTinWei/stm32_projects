#include "uart.h"

#define RCC_APB2ENR   (*(volatile unsigned int *)0x40021018)
#define GPIOA_CRH     (*(volatile unsigned int *)0x40010804)
#define USART1_SR     (*(volatile unsigned int *)0x40013800)
#define USART1_DR     (*(volatile unsigned int *)0x40013804)
#define USART1_BRR    (*(volatile unsigned int *)0x40013808)
#define USART1_CR1    (*(volatile unsigned int *)0x4001380C)

void uart_init(void) {
    // 開啟 GPIOA & USART1 時鐘
    RCC_APB2ENR |= (1<<2) | (1<<14);

    // PA9 (TX): Alternate function push-pull, 50MHz
    GPIOA_CRH &= ~(0xF << 4);
    GPIOA_CRH |=  (0xB << 4);  			// MODE=11 (50MHz), CNF=10 (AF PP)
				
    // PA10 (RX): Floating input
    GPIOA_CRH &= ~(0xF << 8);
    GPIOA_CRH |=  (0x4 << 8);			// MODE=00, CNF=01

    // 設定 Baudrate = 9600 (假設 HSI 8MHz)
    USART1_BRR = 833;           		// 8MHz/9600 ≈ 833

    // activate USART, TX, RX
    USART1_CR1 |= (1 << 2) | (1 << 3) | (1<<13);
}

void uart_send_char(char c) {
    while (!(USART1_SR & (1<<7)));  // 等待 TXE 空閒
    USART1_DR = c;
}

char uart_recv_char(void) {
    while (!(USART1_SR & (1 << 5)));  // 等待 RXNE 有資料
    return (char)USART1_DR;
}

void uart_print(const char *s) {
    while (*s) uart_send_char(*s++);
}

void delay(volatile int count) {
    while (count--) __asm__("nop");
}
