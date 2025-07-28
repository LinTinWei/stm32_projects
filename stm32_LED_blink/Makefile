TARGET = blink
MCU = cortex-m3

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=$(MCU) -mthumb -O0 -Wall -nostdlib

all: $(TARGET).bin

$(TARGET).elf: startup_stm32f103.s main.c linker.ld
	$(CC) $(CFLAGS) -T linker.ld -o $(TARGET).elf startup_stm32f103.s main.c

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

clean:
	rm -f $(TARGET).elf $(TARGET).bin
