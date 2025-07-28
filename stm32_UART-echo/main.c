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
    	GPIOA_CRH &= ~(0xFF << 4);	// clear PA9
    	GPIOA_CRH |=  (0xB << 4);   	// 1011 = Alternate function push-pull
	// PA10 預設為浮空輸入 (0x4)
	GPIOA_CRH &= ~(0xF << 8);	// clear PA10
	GPIOA_CRH |= (0x4 << 8);	// 0100 = Floating input


    	// 設定 Baudrate = 9600 (假設 HSI 8MHz)
    	USART1_BRR = 833;           // 8MHz/9600 ≈ 833
    	USART1_CR1 |= (1<<2) | (1<<3) | (1<<13);  // 啟用 TX + RX + USART
}

void uart_send(char c) {
    while (!(USART1_SR & (1<<7)));  // 等待 TXE 空閒
    USART1_DR = c;
}

char uart_recv(void) {
	while (!(USART1_SR & (1<<5)));	// 等待 RXNE 收到資料
	return (char)USART1_DR;
}

void uart_print(const char *s) {
    while (*s) uart_send(*s++);
}

void delay(volatile int count) {
    while (count--) __asm__("nop");
}

int main(void) {
    	uart_init();
    	uart_print("STM32 UART Echo Ready!\r\n");

    	while (1) {
		char c = USART1_DR;
        	uart_send(c);
		delay(2000000);
    	}
}
