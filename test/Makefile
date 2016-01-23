PROJECT=project


BIN_NAME=$(PROJECT).bin
ELF_NAME=$(PROJECT).elf
MAP_NAME=$(PROJECT).map


CC=arm-none-eabi-g++
LD=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
INC_DIRS=-I./ -I../etk/inc -I../../ -I../
CFLAGS+=-Wall -Wextra -g -fno-common -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -MD -DSTM32F1 $(INC_DIRS) -fno-common  -DGCC_ARMCM3
CPPFLAGS = $(CFLAGS) -fno-rtti -fno-exceptions -std=c++14
LDFLAGS+=--static -lc -lnosys -T ../STM32F103xB.ld -nostartfiles -Wl,--gc-sections -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -lm -Wl,-Map=$(MAP_NAME) -L../libopencm3 -lopencm3_stm32f1
C_SOURCES=
SOURCES=$(wildcard *.cpp) $(wildcard ../*.cpp) $(wildcard ../etk/src/*.cpp)
OBJS=$(SOURCES:.cpp=.o) $(C_SOURCES:.c=.o)
D_FILES=$(SOURCES:.cpp=.d) $(C_SOURCES:.c=.o)

SIZE 	= arm-none-eabi-size


all: $(BIN_NAME)

$(BIN_NAME): $(ELF_NAME)
	$( echo -e "\t" )
	@$(OBJCOPY) -Obinary $(ELF_NAME) $(BIN_NAME)

$(ELF_NAME): $(OBJS)
	@$(LD) -o $(ELF_NAME) $(OBJS) $(LDFLAGS)
	$(SIZE) $@

.cpp.o:
	@$(CC) $(CPPFLAGS) -c $< -o $@

.c.o:
	@$(LD) $(CFLAGS) -c $< -o $@

clean:
	$( echo -e "\t")
	rm -f *.elf *.bin *.list *.map
	rm -f $(OBJS) $(D_FILES)

upload:
	stm32flash -w $(BIN_NAME) -v -g 0x0 /dev/ttyUSB0

monitor:
	python terminal /dev/ttyUSB0

