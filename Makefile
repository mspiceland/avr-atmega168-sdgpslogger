# AVR Makefile

PROG=avr-gpslogger
CPU=atmega168

CFLAGS= -g -O3 -Wall -Wstrict-prototypes -Wa,-ahlms=$(PROG).lst -mmcu=$(CPU) -DF_CPU=8000000

LFLAGS= -Wl,-Map=$(PROG).map,--cref -mmcu=$(CPU) -lm
#LFLAGS= -Wl,-u,vfprintf,-Map=$(PROG).map,--cref -mmcu=$(CPU) -lprintf_min -lm
INCL = mmc_if.h tff.h diskio.h gps.h
SRC = mmc_if.c main.c tff.c diskio.c gps.c
OBJ = $(SRC:.c=.o)

# default target when "make" is run w/o arguments
all: $(PROG).rom

# compile serialecho.c into serialecho.o
%.o: %.c
	avr-gcc -c $(CFLAGS) -I. $*.c

	
# link up sample.o and timer.o into sample.elf
$(PROG).elf: $(OBJ)
	avr-gcc $(OBJ) $(LFLAGS) -o $(PROG).elf

$(OBJ): $(INCL)

# copy ROM (FLASH) object out of sample.elf into sample.rom
$(PROG).rom: $(PROG).elf
	avr-objcopy -O srec $(PROG).elf $(PROG).rom

# command to program chip (optional) (invoked by running "make install")
install:
#	avr-prog -p 4 serialecho.rom
	#uisp -dprog=stk200 -dlpt=/dev/parport0 --erase --upload --verify if=$(PROG).rom
	avrdude -p atmega168 -c stk200 -U flash:w:$(PROG).rom

fuse:
	avrdude -p atmega168 -c stk200 -U lfuse:w:0xe2:m

# command to clean up junk (no source files) (invoked by "make clean")
clean:
	rm -f *.o *.rom *.elf *.map *~ *.lst

