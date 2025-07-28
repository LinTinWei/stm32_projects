#include <stdint.h>
// RCC
#define RCC_APB2ENR   (*(volatile unsigned int *)0x40021018)
#define RCC_AHBENR    (*(volatile unsigned int *)0x40021014)

// GPIO
#define GPIOA_CRL     (*(volatile unsigned int *)0x40010800)

// USART1
#define USART1_SR     (*(volatile unsigned int *)0x40013800)
#define USART1_DR     (*(volatile unsigned int *)0x40013804)
#define USART1_BRR    (*(volatile unsigned int *)0x40013808)
#define USART1_CR1    (*(volatile unsigned int *)0x4001380C)
#define GPIOA_CRH     (*(volatile unsigned int *)0x40010804)

// ADC1
#define ADC1_SR	      (*(volatile unsigned int *)0x40012400)
#define ADC1_CR1      (*(volatile unsigned int *)0x40012404)
#define ADC1_CR2      (*(volatile unsigned int *)0x40012408)
#define ADC1_SMPR2    (*(volatile unsigned int *)0x40012410)
#define ADC1_SQR1     (*(volatile unsigned int *)0x4001242C)
#define ADC1_SQR3     (*(volatile unsigned int *)0x40012434)
#define ADC1_DR       (*(volatile unsigned int *)0x4001244C)

// DMA
#define DMA1_CPAR1    (*(volatile unsigned int *)0x4002000C)
#define DMA1_CMAR1    (*(volatile unsigned int *)0x40020010)
#define DMA1_CNDTR1   (*(volatile unsigned int *)0x40020008)
#define DMA1_CCR1     (*(volatile unsigned int *)0x40020004)

volatile uint16_t adc_buffer[2];

void delay (void) {
	for (volatile int i = 0; i < 1000000; i++);
}

// ====================== UART =========================== //
void uart_init(void) {
    	RCC_APB2ENR |= (1<<2) | (1<<14);	// GPIOA + USART1 clock
	GPIOA_CRH &= ~(0xF << 4);		// clear PA9
	GPIOA_CRH |=  (0xB << 4);		// PA9 = AF_PP 50MHz

    	// 設定 Baudrate = 9600 (假設 HSI 8MHz)
    	USART1_BRR = 833;           // 8MHz/9600 ≈ 833
    	USART1_CR1 |= (1<<2) | (1<<3) | (1<<13);  // RE + TE + UE
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

// ======================== ADC-DMA ======================== //
void adc_dma_init(void) {
	RCC_APB2ENR |= (1<<9) | (1<<2);		// ADC1 + GPIOA
	RCC_AHBENR  |= (1<<0);			// DMA1
	
	GPIOA_CRL &= ~(0xF << 0);		// PA0 analog
	GPIOA_CRL |= ~(0xF << 4);		// PA1 analog

	ADC1_CR1 = 0;
	ADC1_CR2 = (1<<1) | (1<<8) | (1<<20);	// CONT + DMA + SWSTART
	ADC1_SMPR2 |= (0x7 << 0) | (0x7 << 3);	// PA0 & PA1 sample time 239.5 cycles
	ADC1_SQR1 = (1<<20);			// 2 conversions
	ADC1_SQR3 = (0<<0) | (1<<5);		// 1st: ch0, 2nd: ch1

	DMA1_CPAR1 = (uint32_t)&ADC1_DR;
	DMA1_CMAR1 = (uint32_t)adc_buffer;
	DMA1_CNDTR1 = 2;
	DMA1_CCR1 = (1<<0) | (1<<1) | (1<<7) | (1<<5) | (1<<4);	// EN, TCIE, MINC, CIRC, DIR=0

	ADC1_CR2 |= (1<<0);			// ADON
	for (volatile int i = 0; i < 1000; i++);
	ADC1_CR2 |= (1<<3);			// CAL
	for (volatile int i = 0; i < 1000; i++);
	ADC1_CR2 |= (1<<22);			// SWSTART
}

int main(void) {
   	uart_init();
	adc_dma_init();

   	uart_print("DMA ADC PA0+PA1 Start \r\n");
	while (1) {
		uint16_t val0 = adc_buffer[0];
		uint16_t val1 = adc_buffer[1];

		uint32_t mv0 = (val0 * 3300) / 4095;
		uint32_t mv1 = (val1 * 3300) / 4095;

		// 輸出 RAW & Voltage
		uart_print("PA0 = ");
		uart_print_uint(val0);
		uart_print(" / ");
		uart_print_uint(mv0);
		uart_print(" mV, PA1 = ");
                uart_print_uint(val1);
                uart_print(" / ");
                uart_print_uint(mv1);
                uart_print(" mV\r\n");

		delay();
	}
}
