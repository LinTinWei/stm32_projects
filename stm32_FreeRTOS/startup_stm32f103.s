    .syntax unified
    .cpu cortex-m3
    .thumb

/* 向量表放在 .isr_vector 區段 */
.section .isr_vector,"a",%progbits
.word   _estack             /* 初始 MSP 堆疊指針 */
.word   Reset_Handler       /* Reset Handler */
.word   0                   /* NMI_Handler */
.word   0                   /* HardFault_Handler */
.word   0                   /* MemManage_Handler */
.word   0                   /* BusFault_Handler */
.word   0                   /* UsageFault_Handler */
.word   0,0,0,0             /* 保留 */
.word   0                   /* SVC_Handler */
.word   0                   /* DebugMon_Handler */
.word   0                   /* 保留 */
.word   0                   /* PendSV_Handler */
.word   0                   /* SysTick_Handler */

.text
.global Reset_Handler
.type   Reset_Handler, %function

Reset_Handler:
    /* 初始化 .data 段: 從 Flash (_sidata) 搬到 RAM (_sdata ~ _edata) */
    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata
1:
    cmp r1, r2
    ittt lt
    ldrlt r3, [r0], #4
    strlt r3, [r1], #4
    blt 1b

    /* 清空 .bss 段 (_sbss ~ _ebss) */
    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r2, #0
2:
    cmp r0, r1
    it lt
    strlt r2, [r0], #4
    blt 2b

    /* 呼叫 main() */
    bl main

LoopForever:
    b LoopForever

.size Reset_Handler, .-Reset_Handler

/* 堆疊頂端地址 (在 linker.ld 定義) */
.extern _estack
.extern _sidata
.extern _sdata
.extern _edata
.extern _sbss
.extern _ebss

