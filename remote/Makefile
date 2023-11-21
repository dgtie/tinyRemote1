BIN = $(HOME)/.platformio/packages/toolchain-atmelavr/bin
GCC = $(BIN)/avr-g++
OBJCOPY = $(BIN)/avr-objcopy
OBJDUMP = $(BIN)/avr-objdump
SIZE = $(BIN)/avr-size
PYMCUPROG = pymcuprog

SRC = $(shell ls -r *.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = out
MCU = attiny414

CFLAGS = -mmcu=$(MCU) -Os
LDFLAGS = -mmcu=$(MCU) -Wl,-Map=$(TARGET).map

all: $(TARGET).hex

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

%.elf: $(OBJ)
	$(GCC) $(LDFLAGS) -o $@ $^
	$(SIZE) $@

%.o: %.cpp
	$(GCC) $(CFLAGS) -o $@ -c $<

dasm: $(TARGET).elf
	$(OBJDUMP) -S $<

clean:
	rm -rf *.o $(TARGET).*

test:
	minicom -D /dev/ttyACM0 -w

load: $(TARGET).hex
	$(PYMCUPROG) erase -d $(MCU) -t uart -u /dev/ttyACM0 
	$(PYMCUPROG) write -f $(TARGET).hex -d $(MCU) -t uart -u /dev/ttyACM0

# write --erase
# $pymcuprog -t uart -u /dev/ttyACM0 -d attiny414 write -m fuses -o 8 -l 2
