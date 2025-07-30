CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m3 -mthumb -Os -Wall -ffreestanding 
CFLAGS += -DSTM32F103xB
LDFLAGS = -Tlinker.ld -nostartfiles -lc -lgcc -lnosys

#Source
SRCS = main.c \
       drivers/uart.c \
       drivers/led.c \
       freertos/tasks.c \
       freertos/list.c \
       freertos/queue.c \
       freertos/timers.c \
       freertos/portable/GCC/ARM_CM3/port.c \
       freertos/portable/MemMang/heap_4.c \
       syscalls.c \
       interrupt.c \
       startup_stm32f103.s

#include path
INCLUDES = -I. -Ifreertos/include -Ifreertos/portable/GCC/ARM_CM3 -Idrivers

OBJS = $(SRC:.c=.o)

all: main.elf main.bin

main.elf: $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS) -o $@ $(LDFLAGS)

main.bin: main.elf
	arm-none-eabi-objcopy -O binary $< $@

clean:
	rm -f *.o *.elf *.bin
