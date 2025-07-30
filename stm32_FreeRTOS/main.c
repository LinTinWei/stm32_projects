#include "FreeRTOS.h"
#include "uart.h"

int main(void) {
    	uart_init();
	uart_print("FreeRTOS STM32 Starting...\r\n");

	return 0;
}
