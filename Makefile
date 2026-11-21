# ---------------------------------------------------------------------------
#  Greenhouse controller - ATxmega128  (GNU make + avr-gcc)
#
#    make            build  -> build/greenhouse.hex
#    make flash      program via avrdude (PDI)
#    make clean
#
#  Override on the command line, for example:
#    make MCU=atxmega128a3 AVRDUDE_MCU=x128a3 PROGRAMMER=avrispmkII
# ---------------------------------------------------------------------------
TARGET        := greenhouse
MCU           ?= atxmega128a1
AVRDUDE_MCU   ?= x128a1
PROGRAMMER    ?= atmelice_pdi
AVRDUDE_EXTRA ?=

BUILD := build
SRCS  := $(wildcard src/*.c) $(wildcard lib/*/*.c)
OBJS  := $(SRCS:%.c=$(BUILD)/%.o)
DEPS  := $(OBJS:.o=.d)
INCS  := -Iinclude $(addprefix -I,$(wildcard lib/*))

CC      := avr-gcc
OBJCOPY := avr-objcopy
SIZE    := avr-size

CFLAGS  := -mmcu=$(MCU) -std=gnu99 -Os -Wall -Wextra \
           -ffunction-sections -fdata-sections $(INCS) -MMD -MP
LDFLAGS := -mmcu=$(MCU) -Wl,--gc-sections

ELF := $(BUILD)/$(TARGET).elf
HEX := $(BUILD)/$(TARGET).hex

.PHONY: all size flash clean rebuild

all: $(HEX) size

$(ELF): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

size: $(ELF)
	$(SIZE) --format=avr --mcu=$(MCU) $(ELF)

flash: $(HEX)
	avrdude -p $(AVRDUDE_MCU) -c $(PROGRAMMER) $(AVRDUDE_EXTRA) -U flash:w:$(HEX):i

clean:
	rm -rf $(BUILD)

rebuild: clean all

-include $(DEPS)
