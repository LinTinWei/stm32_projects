# stm32_projects

This repository includes a collection of STM32F103 projects developed on Ubuntu 22.04, using cross-compiler to generate bin file and using STM32CubeProgrammer to burn on the board.

## Projects

| Folder                  | Description                          |
|-------------------------|--------------------------------------|
| `stm32_blink`           | Basic GPIO blinking example          |
| `stm32_LED_blink`       | LED blinking with delay loop         |
| `stm32_UART`            | Basic UART transmission              |
| `stm32_UART_echo`       | UART echo test                       |
| `stm32_UART_ADC`        | Read ADC and send via UART           |
| `stm32_UART_LED`        | UART receive to control LED state    |
| `stm32_DMA_ADC`         | Continuous ADC sampling with DMA     |
| `stm32_multi-channel_ADC` | ADC on PA0/PA1 and UART streaming  |
| `stm32_FreeRTOS`        | Burn Real-time OS bin file on board  |

Each folder contains its own README and source code.

## Environment
- Ubuntu 22.04 LTS
- `arm-none-eabi-gcc`
- `make`

---

###  Getting Started
```bash
git clone https://github.com/LinTinWei/stm32_projects.git
cd stm32_UART_LED
make clean & make


