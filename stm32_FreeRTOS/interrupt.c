#include "FreeRTOS.h"
#include "portmacro.h"
#include "freertos/portable/GCC/ARM_CM3/portmacro.h"

void xPortSysTickHandler();
void xPortPendSVHandler();
void vPortSVCHandler();

void SysTick_Handler(void) {
xPortSysTickHandler();
}

void PendSV_Handler(void) {
    xPortPendSVHandler();
}

void SVC_Handler(void) {
    vPortSVCHandler();
}
