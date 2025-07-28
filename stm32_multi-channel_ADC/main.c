#include <stdint.h>
// RCC
#define RCC_APB2ENR   (*(volatile unsigned int *)0x40021018)

// GPIOC (LED PC13)
#define GPIOC_CRH     (*(volatile unsigned int *)0x40011004)
#define GPIOC_ODR     (*(volatile unsigned int *)0x4001100C)	

// GPIOA (USART1 TX:PA9, RX: PA10, ADC:PA0)
#define GPIOA_CRH     (*(volatile unsigned int *)0x40010804)

// USART1
#define USART1_SR     (*(volatile unsigned int *)0x40013800)
#define USART1_DR     (*(volatile unsigned int *)0x40013804)
#define USART1_BRR    (*(volatile unsigned int *)0x40013808)
#define USART1_CR1    (*(volatile unsigned int *)0x4001380C)

// ADC1
#define ADC1_SR	      (*(volatile unsigned int *)0x40012400)
#define ADC1_CR1      (*(volatile unsigned int *)0x40012404)
#define ADC1_CR2      (*(volatile unsigned int *)0x40012408)
#define ADC1_SMPR2    (*(volatile unsigned int *)0x40012410)
#define ADC1_SQR1     (*(volatile unsigned int *)0x4001242C)
#define ADC1_SQR3     (*(volatile unsigned int *)0x40012434)
#define ADC1_DR       (*(volatile unsigned int *)0x4001244C)

void delay (volatile int t) {
	while (t--) __asm__("nop");
}

// ====================== UART =========================== //
void uart_init(void) {
    	RCC_APB2ENR |= (1<<2) | (1<<14);	// GPIOA + USART1 clock
	GPIOA_CRH &= ~(0xFF << 4);		// clear PA9
	GPIOA_CRH |= (0xB << 4);		// AF push-pull

    	// 設定 Baudrate = 9600 (假設 HSI 8MHz)
    	USART1_BRR = 833;           // 8MHz/9600 ≈ 833
    	USART1_CR1 |= (1<<13) | (1<<3);  // UE + TE
}

void uart_send(char c) {
    while (!(USART1_SR & (1<<7)));  // 等待 TXE 空閒
    USART1_DR = c;
}

void uart_print(const char *s) {
    while (*s) uart_send(*s++);
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

// ============================ ADC ======================== //
void adc_init(void) {
	// 1. 開啟時鐘
	RCC_APB2ENR |= (1<<9);		// activate ADC1 clock
	RCC_APB2ENR |= (1<<2);		// activate GPIOA clock (PA0 analog)
	RCC_APB2ENR |= (1<<0);		// DMA enable

	// 3. 設定取樣時間 (239.5 cycles 最穩定)
	ADC1_SMPR2 |= (0x7 << 0);	// ch0 取樣時間 239.5 cycles
	ADC1_SMPR2 |= (0x7 << 3);	// ch1

	// 4. 先 ADON 一次喚醒
	ADC1_CR2 |= (1<<0);		// 第一次 ADON=1 只是喚醒
	for (volatile int i = 0; i < 1000; i++); // 等待一點時間
	
	// 5. Reset Calibration
	ADC1_CR2 |= (1<<2);		// RSTCAL=1 Reset calibartion
	while(ADC1_CR2 & (1<<2));	// 等待 Reset Calibration 完成
	
	// 6. Calibration
	ADC1_CR2 |= (1<<3);		// CAL=1 開始校正
	while(ADC1_CR2 & (1<<3));	// 等待 Calibration 完成
}

uint16_t adc_read(uint8_t ch) {
	ADC1_CR2 |= (1<<0);		// 重新喚醒 ADON (Single Conversion 模式需要)
	ADC1_SQR3 = ch;			// 選擇通道ch (ch0, ch1)
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

int main(void) {
   	uart_init();
	adc_init();

   	uart_print("ADC Dual Channel Ready (PA0, PA1)\r\n");
	while (1) {
		uint16_t adc0 = adc_read(0);
		uint16_t adc1 = adc_read(1);

		// 輸出 RAW & Voltage
		uart_print("PA0 = ");
		uart_print_uint(adc0);
		uart_print(" / ");
		uart_print_uint((adc0 * 3300) / 4095);
		uart_print(" mV, ");

		uart_print("PA1 = ");
                uart_print_uint(adc1);
                uart_print(" / ");
                uart_print_uint((adc1 * 3300) / 4095);
                uart_print(" mV, \r\n");

		delay(500000);

	}
}
