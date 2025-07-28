#define RCC_APB2ENR   (*(volatile unsigned int *)0x40021018)
#define GPIOA_CRH     (*(volatile unsigned int *)0x40010804)
#define USART1_SR     (*(volatile unsigned int *)0x40013800)
#define USART1_DR     (*(volatile unsigned int *)0x40013804)
#define USART1_BRR    (*(volatile unsigned int *)0x40013808)
#define USART1_CR1    (*(volatile unsigned int *)0x4001380C)

void uart_init(void) {
    // 開啟 GPIOA & USART1 時鐘
    RCC_APB2ENR |= (1<<2) | (1<<14);

    // 設 PA9 為 AF 推挽輸出 (TX)
    GPIOA_CRH &= ~(0xF << 4);
    GPIOA_CRH |=  (0xB << 4);   // TX: AF output, 50MHz

    // 設定 Baudrate = 115200 (假設 HSI 8MHz)
    USART1_BRR = 69;           // 8MHz/115200 ≈ 69
    USART1_CR1 |= (1<<3) | (1<<13);  // 啟用 TX + USART
}

void uart_send(char c) {
    while (!(USART1_SR & (1<<7)));  // 等待 TXE 空閒
    USART1_DR = c;
}

void uart_print(const char *s) {
    while (*s) uart_send(*s++);
}

void delay(volatile int count) {
    while (count--) __asm__("nop");
}

int main(void) {
    uart_init();
    while (1) {
        uart_print("Hello STM32!\r\n");
        delay(800000);
    }
}
