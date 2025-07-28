#define RCC_APB2ENR   (*(volatile unsigned int *)0x40021018)
#define GPIOC_CRH     (*(volatile unsigned int *)0x40011004)
#define GPIOC_ODR     (*(volatile unsigned int *)0x4001100C)

void gpio_init_led(void) {
	RCC_APB2ENR |= (1<<4);		// GPIOC clock
	
	// Set PC13 as push-pull output(10MHz)
	GPIOC_CRH &= ~(0xF << 20);	// clear PC13 config (CRH bits 20 ~ 23)
	GPIOC_CRH |=  (0x1 << 20);	// MODE = 01 (10MHz), CNF = 00 (push-pull output)
	
	// Default close LED
	GPIOC_ODR |= (1 << 13);		// High level -> LED close
}

void led_on(void) {
	GPIOC_ODR &= ~(1 << 13);		// low level -> LED ON
}

void led_off(void){
	GPIOC_ODR |= (1 << 13);			// high level -> LED OFF
}

int main(void) {
    	gpio_init_led();
	
	while (1) {
		GPIOC_ODR ^= (1 << 13);	// reverse PC13
		for (volatile int i = 0; i < 500000; i++);
	}
}
