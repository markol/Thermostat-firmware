#avr gcc
CC = avr-gcc 
#avr objeccopy
OBJCOPY = avr-objcopy

#The CFLAGS variable sets compile flags for gcc
#-g                 compile with debug information
#-mmcu=atmega8   selects the target device
#-std=gnu99         use the C99 standard language definition
CFLAGS = -g -mmcu=atmega8 -std=gnu99 -Wall -Wextra

#OBJFLAG sets the flag for making a elf to hex
OBJFLAG = -j .text -j .data -O ihex

FLASHFLAGS = -c avrisp -p m8 -P /dev/ttyACM0 -b 19200 

EXECUTABLE = thermostat
SOURCES = ${wildcard *.c}
HEADERS = ${wildcard *.h}
OBJECTS = ${SOURCES:.c=.o}

.PHONY: all
all: ${EXECUTABLE}

$(EXECUTABLE): $(OBJECTS) buildnumber.num
	$(CC) $(CFLAGS) -o $(EXECUTABLE).elf $(OBJECTS)
	$(OBJCOPY) $(OBJFLAG) $(EXECUTABLE).elf $(EXECUTABLE).hex
	@echo "-- Build: " $$(cat buildnumber.num)

# Creeer dependency file met optie -MM van de compiler
depend: $(SOURCES)
	@echo "calling depend"
	$(CC) $(CFLAGS) -Os -c -MM $^ > $@

-include depend

# Buildnumber administratie
buildnumber.num: $(OBJECTS)
	@if ! test -f buildnumber.num; then echo 0 > buildnumber.num; fi
	@echo $$(($$(cat buildnumber.num)+1)) > buildnumber.num
	
# Create a clean environment
.PHONY: clean
clean:
	$(RM) $(EXECUTABLE) $(OBJECTS)

# Clean up dependency file  
.PHONY: clean-depend
clean-depend: clean
	$(RM) depend  
	                       

check: 
	avrdude $(FLASHFLAGS)
	
flash: all
	avrdude $(FLASHFLAGS) -v -e -U flash:w:$(EXECUTABLE).hex
	
fuse:
	avrdude $(FLASHFLAGS) -U lfuse:w:0xef:m -U hfuse:w:0xd9:m