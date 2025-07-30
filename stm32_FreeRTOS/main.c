#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "queue.h"

QueueHandle_t xQueue;

void vTaskProducer(void *pvParameters);
void vTaskConsumer(void *pvParameters);
int main(void) {
    	uart_init();
	uart_print("FreeRTOS Queue Demo...\r\n");

	// 建立 Queue: 可存 10 筆 uint16_t
	xQueue = xQueueCreate(10, sizeof(uint16_t));

	if (xQueue == NULL) {
		uart_print("Queue create Failed!");
		while(1);
	}

	xTaskCreate(vTaskProducer, "Producer", 128, NULL, 1, NULL);
	xTaskCreate(vTaskConsumer, "Consumer", 128, NULL, 1, NULL);


	vTaskStartScheduler();
	while(1);

	return 0;
}
