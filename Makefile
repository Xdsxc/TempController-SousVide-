# AVR Makefile
# Author: Dustin Guerrero

OBJDIR := build
SRCDIR := src
INCDIR := include

# Set to your specific AVR chip
DEVICE := atmega1284p
# Set clock speed
CLOCK := 8000000
# Set programming device
PROGRAMMER := avrisp2
# ** Enter file that contains main, minus the suffix
MAIN := main
# ** Enter any additional files that should be linked with main, in filename.o format
OBJECTS := $(MAIN).o pwm.o nokia5110driver.o pid.o ow.o ow_ds18b20.o bitmap2d.o keypad.o timer.o state_machines.o
VPATH := ./$(SRCDIR):./$(INCDIR):./$(OBJDIR):./$(DEPDIR)
# *IMPORTANT*: These fuse settings are for the ATmega1284P. Although the 1280 is practically the same
# microcontroller, I'm not 100% sure if the fuse settings are the same. If you are using
# any other AVR besides the ATmega1284P, double check the fuse settings before making 
# the "fuse" target. By default, disables JTAGEN and CKDIV8. The uncommented portion sets the 16MHz clock,
# the commented portion sets an 8MHz clock.

LFUSE := 0xC2
HFUSE := 0xD1
EFUSE := 0xFD
#LFUSE := 0xFF
#HFUSE := 0xD9
#EFUSE := 0xFC

AVRDUDE := avrdude -c $(PROGRAMMER) -p $(DEVICE)
CC := avr-gcc
CFLAGS := -g -Os -I./$(INCDIR) -Wall -ffunction-sections -fshort-enums -mmcu=$(DEVICE) -DF_CPU=$(CLOCK) -std=gnu99 
LDFLAGS := -lm
default: $(MAIN).hex

flash: $(MAIN).hex
	$(AVRDUDE) -U flash:w:$^:i

eeprom: $(MAIN).eep
	$(AVRDUDE) -U eeprom:w:$^

$(MAIN).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(addprefix ./$(OBJDIR)/, $(OBJECTS))  $(LDFLAGS) -o ./$(OBJDIR)/$@

.elf.S:
	avr-objdump -g -S -j .text -d -m avr5 $< > $@

.elf.hex:
	@rm -rf $@
	avr-objcopy -j .text -j .data -O ihex ./$(OBJDIR)/$< $@
	avr-size --format=avr --mcu=$(DEVICE) ./$(OBJDIR)/$<
.elf.eep:
	@rm -rf $@
	avr-objcopy -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 -O ihex $< $@

.c.o:
	$(CC) -c $(CFLAGS) -mmcu=$(DEVICE) $< -o $(OBJDIR)/$@

# Shows what the file would look like with all macro expansions.
# Useful for testing if macros are being substituted properly
expanded: $(MAIN).c
	$(CC) -mmcu=$(DEVICE) -E -I. $(MAIN).c -o $(MAIN)-pp.c

# Writes the assembly output produced by the compiler, mixed with the source
disasm: $(MAIN).S

clean:
	@rm -f ./$(OBJDIR)/*

cleanall:
	@rm -f *.hex *.elf *.S *.o
fuses:
	$(AVRDUDE) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

.PHONY: cpp clean fuses cleanall
.SUFFIXES: .c .o .hex .elf .asm .eep
