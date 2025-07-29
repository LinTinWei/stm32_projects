#include <stdio.h>

// RCC
#define RCC_APB2ENR   (*(volatile uint32_t *)0x40021018)
#define RCC_APB1ENR   (*(volatile uint32_t *)0x4002101C)

// GPIOA
#define GPIOA_CRL     (*(volatile uint32_t *)0x40010800)

// TIM2
#define TIM2_CR1      (*(volatile uint32_t *)0x40000000)
#define TIM2_CCMR1    (*(volatile uint32_t *)0x40000000)
#define TIM2_CCER     (*(volatile uint32_t *)0x40000000)
#define TIM2_PSC      (*(volatile uint32_t *)0x40000000)
#define TIM2_ARR      (*(volatile uint32_t *)0x40000000)
#define TIM2_CCR2     (*(volatile uint32_t *)0x40000000)

void delay(volatile uint32_t t) {
	while (t--) __asm__("nop");
}

// PWM initialization
void pwm_init(void) {
	// 啟用 GPIOA 以及 TIM2 時鐘
	RCC_APB2ENR |= (1 << 2); 	// GPIOA
	RCC_APB1ENR |= (1 << 0);	// TIM2

	// 設定 PA1 為 Alternative Function Push-Pull
	GPIOA_CRL &= ~(0xF << 4);	// 清除 PA1 config
	GPIOA_CRL |=  (0xB << 4);	// MODE = 11 (50MHz), CNF = 10 (AF PP)

	// TIM2 設定為 PWM 模式 (CH2)
	TIM2_PSC = 72 - 1;		// 降頻至 1MHz (72MHz / 72)
	TIM2_ARR = 1000;		// 自動重載值 = 1000 -> 1kHz
	TIM2_CCR2 = 0;			// 初始占空比為 0

	TIM2_CCMR1 &= ~(0xFF << 8); // 清除 CH2 設定
    	TIM2_CCMR1 |= (6 << 12);    // OC2M = 110: PWM mode 1
    	TIM2_CCMR1 |= (1 << 11);    // OC2PE = 1
    	TIM2_CCER  |= (1 << 4);     // CC2E = 1 啟用 CH2 輸出

    	TIM2_CR1 |= (1 << 7);       // ARPE = 1
    	TIM2_CR1 |= (1 << 0);       // CEN = 1 啟動計時器
}


int main(void) {
    	pwm_init();

	while (1) {
		// 緩慢調整占空比 (呼吸燈效果)
		for (int i = 0; i <= 1000; i++) {
			TIM2_CCR2 = i;
			delay(5000);
		}
		for (int i = 1000; i >= 0; i--) {
			TIM2_CCR2 = i;
			delay(5000);
		}
	}
}
