#include "led.h"
#include <stdint.h>

#define RCC_APB2ENR	(*(volatile unsigned int *)0x40021018)
#define GPIOC_CRH	(*(volatile unsigned int *)0x40011004)
#define GPIOC_ODR	(*(volatile unsigned int *)0x4001100C)

void led_init(void) {
	RCC_APB2ENR |= (1 << 4);
	GPIOC_CRH &= ~(0xF << 20);
	GPIOC_CRH |=  (0x2 << 20);
	GPIOC_ODR |=  (1 << 13);
}

void led_toggle(){
	GPIOC_ODR ^= (1 << 13);
}
