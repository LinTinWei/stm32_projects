#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"

void task1(void *param) {
	while (1) {
		uart_print("Hello from Task1 \r\n");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}


int main(void) {
    	uart_init();
	uart_print("FreeRTOS STM32 Starting...\r\n");

	// xTaskCreate(task1, "Task1", 128, NULL, 1, NULL);

	// vTaskStartScheduler();

	// while(1);	// 不會進來
}
