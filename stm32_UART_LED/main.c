#define RCC_APB2ENR   (*(volatile unsigned int *)0x40021018)
#define GPIOC_CRH     (*(volatile unsigned int *)0x40011004)
#define GPIOC_ODR     (*(volatile unsigned int *)0x4001100C)	

// USART1 (TX:PA9, RX: PA10)
#define GPIOA_CRH     (*(volatile unsigned int *)0x40010804)
#define USART1_SR     (*(volatile unsigned int *)0x40013800)
#define USART1_DR     (*(volatile unsigned int *)0x40013804)
#define USART1_BRR    (*(volatile unsigned int *)0x40013808)
#define USART1_CR1    (*(volatile unsigned int *)0x4001380C)

void uart_init(void) {
    	// 開啟 GPIOA & USART1 時鐘
    	RCC_APB2ENR |= (1<<2) | (1<<14);

    	// PA9 TX: AF push-pull
    	GPIOA_CRH &= ~(0xFF << 4);	// clear PA9
    	GPIOA_CRH |=  (0xB << 4);   	// MODE = 11 (50MHz), CNF = 10 (AF PP)
	// PA10 RX: floating input
	GPIOA_CRH &= ~(0xF << 8);	// clear PA10
	GPIOA_CRH |= (0x4 << 8);	// MODE = 00, CNF = 01 floating input


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

// LED Initialization
void gpio_init_led(void) {
	RCC_APB2ENR |= (1<<4);		// GPIOC clock

	GPIOC_CRH &= ~(0xF << 20);	// clear PC13 config (CRH bits 20 ~ 23)
	GPIOC_CRH |= (0x2 << 20);	// MODE = 10(2MHz), CNF = 00 push-pull

	GPIOC_ODR |= (1 << 13);		// High level -> LED close
}

void led_on(void) {
	GPIOC_ODR &= ~(1 << 13);	// low level -> LED ON
}

void led_off(void){
	GPIOC_ODR |= (1 << 13);		// high level -> LED OFF
}

int strcmp(const char *a, const char *b) {
	while (*a && *b) {
		if (*a != *b) return 1;		// not equal
		a++;
		b++;
	}
	return (*a == *b) ? 0: 1;		// 都結束才算完全相等
}

int main(void) {
    	uart_init();
	gpio_init_led();
    	uart_print("UART LED Control Ready! Type ON/OFF\r\n");

	char cmd[4] = {0};
	int idx = 0;

	while (1) {
		char c = uart_recv();
		uart_send(c);	// 回顧

		if (c == '\r' || c == '\n'){
			cmd[idx] = 0;	// 字串結束
			if (idx > 0) {
				if (!strcmp(cmd, "ON")){
					led_on();
					uart_print("\r\nLED ON\r\n");
				} else if (!strcmp(cmd, "OFF")) {
					led_off();
					uart_print("\r\nLED OFF\r\n");
				} else {
					uart_print("\r\nUnknown CMD\r\n");
				}
			}

			idx = 0;	// reset instruction buffer
		} else {
			if (idx < 3) cmd[idx++] = c;
		}
	}
}
