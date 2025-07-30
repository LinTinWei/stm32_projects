#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "led.h"


// task.c 中宣告的任務函數
void vTaskLED(void *pvParameters);
void vTaskUART(void *pvParameters);

int main(void) {
    	uart_init();
	led_init();	
	uart_print("FreeRTOS STM32 Starting...\r\n");

	if (xTaskCreate(vTaskLED, "LED", 128, NULL, 1, NULL) != pdPASS)
    		uart_print("Task LED creation failed!\r\n");
	if (xTaskCreate(vTaskUART, "UART", 128, NULL, 2, NULL) != pdPASS)
    		uart_print("Task UART creation failed!\r\n");

	uart_print("Starting scheduler...\r\n");
	vTaskStartScheduler();
	uart_print("Scheduler failed to start!\r\n");  // 若看到這句就代表 FreeRTOS 出錯

	return 0;
}
