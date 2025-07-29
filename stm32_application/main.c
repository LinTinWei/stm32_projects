#include <stdint.h>
// RCC
#define RCC_APB2ENR   (*(volatile unsigned int *)0x40021018)

// GPIOC (LED PC13)
#define GPIOC_CRH     (*(volatile unsigned int *)0x40011004)
#define GPIOC_ODR     (*(volatile unsigned int *)0x4001100C)	

// GPIOA (USART1 TX:PA9, RX: PA10, ADC:PA0)
#define GPIOA_CRH     (*(volatile unsigned int *)0x40010804)
#define USART1_SR     (*(volatile unsigned int *)0x40013800)
#define USART1_DR     (*(volatile unsigned int *)0x40013804)
#define USART1_BRR    (*(volatile unsigned int *)0x40013808)
#define USART1_CR1    (*(volatile unsigned int *)0x4001380C)

// ADC1
#define ADC1_SR	      (*(volatile unsigned int *)0x40012400)
#define ADC1_CR2      (*(volatile unsigned int *)0x40012408)
#define ADC1_SMPR2    (*(volatile unsigned int *)0x40012410)
#define ADC1_SQR3     (*(volatile unsigned int *)0x40012434)
#define ADC1_DR       (*(volatile unsigned int *)0x4001244C)

// ====================== UART =========================== //
void uart_init(void) {
    	RCC_APB2ENR |= (1<<2) | (1<<14);	// GPIOA + USART1 clock

    	// PA9 TX: AF push-pull
    	GPIOA_CRH &= ~(0xF << 4);		// clear PA9
    	GPIOA_CRH |=  (0xB << 4);   		// MODE = 11 (50MHz), CNF = 10 (AF PP)
	
	// PA10 RX: floating input
	GPIOA_CRH &= ~(0xF << 8);		// clear PA10
	GPIOA_CRH |= (0x4 << 8);		// MODE = 00, CNF = 01 floating input


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

// =========================== LED PC13 ========================= //
void gpio_init_led(void) {
	RCC_APB2ENR |= (1<<4);		// GPIOC clock enable

	GPIOC_CRH &= ~(0xF << 20);	// clear PC13 config (CRH bits 20 ~ 23)
	GPIOC_CRH |=  (0x2 << 20);	// MODE = 10 (2MHz), CNF = 00 push-pull

	GPIOC_ODR |= (1 << 13);		// High level -> LED off
}

void led_on(void)  {GPIOC_ODR &= ~(1 << 13);}	// low level -> LED ON

void led_off(void) {GPIOC_ODR |= (1 << 13);}	// high level -> LED OFF

// ========================= ADC PA0 ===================== //
void adc_init(void) {
	// 1. 開啟時鐘
	RCC_APB2ENR |= (1<<9);		// activate ADC1 clock
	RCC_APB2ENR |= (1<<2);		// activate GPIOA clock (PA0 analog)
	
	// 2. PAO analog mode
	// #define GPIOA_CRL     (*(volatile unsigned int *)0x40010800)
	// GPIOA_CRL &= ~(0xF << 0);       // PA0 analog mode (reset state)

	// 3. 設定取樣時間 (239.5 cycles 最穩定)
	ADC1_SMPR2 |= (0x7 << 0);	// 通道0 取樣時間 239.5 cycles

	// 4. 先 ADON 一次喚醒
	ADC1_CR2 |= (1<<0);		// 第一次 ADON=1 只是喚醒
	for (volatile int i = 0; i < 1000; i++); // 等待一點時間
	
	// 5. Reset Calibration
	ADC1_CR2 |= (1<<2);		// RSTCAL=1 Reset calibartion
	while(ADC1_CR2 & (1<<2));	// 等待 Reset Calibration 完成
	
	// 6. Calibration
	ADC1_CR2 |= (1<<3);		// CAL=1 開始校正
	while(ADC1_CR2 & (1<<3));	// 等待 Calibration 完成

	// 7. 再 ADON 一次 -> 進入真正工作狀態
	// ADC1_CR2 |= (1<<0);             // 第二次 ADON=1 真正進入開啟狀態

}

uint16_t adc_read(void) {
	ADC1_CR2 |= (1<<0);		// 重新喚醒 ADON (Single Conversion 模式需要)
	ADC1_SQR3 = 0;			// 選擇通道0 (PA0)
	ADC1_CR2 |= (1<<22);		// SWSTART 軟體觸發轉換
	while (!(ADC1_SR & (1<<1)));	// 等待 EOC(End Of Conversion) 轉換完成
	return (uint16_t)ADC1_DR;	// 回傳 ADC 12bit 結果
}

// ========================== String Tools ================== //
int strcmp(const char *a, const char *b) {
        while (*a && *b) {
                if (*a != *b) return 1;         // not equal
                a++;
                b++;
        }
        return (*a == *b) ? 0: 1;               // 都結束才算完全相等
}

void uart_print_uint(uint32_t val) {
        char buf[12];
        int i = 10;
        buf[11] = '\0'; 

        do {
                buf[i--] = '0' + (val % 10);
                val /= 10;
        } while (val && i >= 0);
        uart_print(&buf[i+1]);
}


int main(void) {
    	uart_init();
	gpio_init_led();
	adc_init();

   	uart_print("UART LED+ADC Ready! Type ON/OFF/ADC\r\n");
	
	char cmd[5] = {0};
	int idx = 0;
	
	while (1) {
		char c = uart_recv();	//
		uart_send(c);		// 回顯
		if (c == '\r' || c == '\n'){
			cmd[idx] = 0;	// 字串結束
			if (idx > 0) {
				if (!strcmp(cmd, "ON")){
					led_on();
					uart_print("\r\nLED ON\r\n");
				} else if (!strcmp(cmd, "OFF")) {
					led_off();
					uart_print("\r\nLED OFF\r\n");
				} else if (!strcmp(cmd, "ADC")) {
                                        // Read pin poltage (ex: PA0)
                                        uint16_t raw = adc_read();
                                        uart_print("\r\nADC RAW=");
                                        uart_print_uint(raw);
                                        
                                        // translate into voltage (mV)
                                        uint32_t mv = (raw * 3300) / 4095;
                                        uint32_t int_part = mv / 1000;   // V 整數部分
                                        uint32_t frac_part = mv % 1000;  // 小數部分 (mV)
                
                                        uart_print("\r\nVoltage=");
                                        uart_print_uint(int_part);
                                        uart_send('.');
                                        if (frac_part < 100) uart_send('0');
                                        if (frac_part < 10) uart_send('0');
                                        uart_print_uint(frac_part);
                                        uart_print("V\r\n");
                                } else {
					uart_print("\r\nUnknown CMD\r\n");
				}
			}

			idx = 0;	// reset instruction buffer
		} else {
			if (idx < 4) cmd[idx++] = c;
		}
	}
}
