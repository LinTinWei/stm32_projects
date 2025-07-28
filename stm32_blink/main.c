#define PERIPH_BASE        0x40000000
#define APB2PERIPH_BASE    (PERIPH_BASE + 0x10000)
#define GPIOA_BASE         (APB2PERIPH_BASE + 0x0800)
#define GPIOB_BASE         (APB2PERIPH_BASE + 0x0C00)
#define GPIOC_BASE         (APB2PERIPH_BASE + 0x1000)
#define RCC_BASE           (APB2PERIPH_BASE + 0x9000)

#define RCC_APB2ENR        (*(volatile unsigned int *)(RCC_BASE + 0x18))

#define GPIOA_CRL          (*(volatile unsigned int *)(GPIOA_BASE + 0x00))
#define GPIOA_CRH          (*(volatile unsigned int *)(GPIOA_BASE + 0x04))
#define GPIOA_ODR          (*(volatile unsigned int *)(GPIOA_BASE + 0x0C))

#define GPIOB_CRL          (*(volatile unsigned int *)(GPIOB_BASE + 0x00))
#define GPIOB_CRH          (*(volatile unsigned int *)(GPIOB_BASE + 0x04))
#define GPIOB_ODR          (*(volatile unsigned int *)(GPIOB_BASE + 0x0C))

#define GPIOC_CRL          (*(volatile unsigned int *)(GPIOC_BASE + 0x00))
#define GPIOC_CRH          (*(volatile unsigned int *)(GPIOC_BASE + 0x04))
#define GPIOC_ODR          (*(volatile unsigned int *)(GPIOC_BASE + 0x0C))

void delay(volatile unsigned int count) {
	while (count--) __asm__("nop");
}

int main(void) {
    // 開啟 GPIOA/B/C 時鐘
    RCC_APB2ENR |= (1 << 2) | (1 << 3) | (1 << 4);

    // 把 A0~A15、B0~B15、C0~C15 都設成「一般推挽輸出 2MHz」
    GPIOA_CRL = 0x22222222;   // A0~A7
    GPIOA_CRH = 0x22222222;   // A8~A15

    GPIOB_CRL = 0x22222222;   // B0~B7
    GPIOB_CRH = 0x22222222;   // B8~B15

    GPIOC_CRL = 0x22222222;   // C0~C7
    GPIOC_CRH = 0x22222222;   // C8~C15

    // 直接把所有腳位拉低
    GPIOA_ODR = 0x0000;
    GPIOB_ODR = 0x0000;
    GPIOC_ODR = 0x0000;

    // MCU 卡在這裡 → LED 接在哪個腳位就會立刻亮
    while (1) {
        // 什麼都不做
    }
}

