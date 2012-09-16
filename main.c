/* ***********************************************************************
**  Collect GPS data and save it to an SD/MMC card
**  Copyright (C) 2009 Michael Spiceland
*************************************************************************
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software Foundation, 
**  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
*************************************************************************/
#define F_CPU 8000000
#define BAUD_RATE     4800 // desired baud rate
#define UBRR_DATA     (F_CPU/(BAUD_RATE)-1)/16 // sets baud rate

#define ALL_INPUT  0x00 // 0000 0000
#define ALL_OUTPUT 0xFF // 1111 1111

#define LED_ON(x) (PORTC &= ~(1 << x))
#define LED_OFF(x) (PORTC |= (1 << x))

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <util/delay.h>

#include "mmc_if.h"
#include "tff.h"
#include "gps.h"

FATFS fatfs;
FIL logfile;
#define BUFFER_SIZE 32
volatile uint8_t readptr = 0;
volatile uint8_t writeptr = 0;
volatile uint8_t uart_buffer[BUFFER_SIZE];

/**************************************************************************
* Function: send_serial()
* Purpose: send one byte of data out our serial port
* note: this funcion blocks until we are ready to send our next byte
**************************************************************************/
void send_serial(unsigned char byte)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = byte;
}

/**************************************************************************
* Function: init_serial()
* Purpose: initialilze serial port by enabling it and setting baudrate
**************************************************************************/
void init_serial(void)
{
	UCSR0B = _BV(RXEN0) | _BV(TXEN0) | (1 << RXCIE0);
	UBRR0L = UBRR_DATA;
}

SIGNAL(SIG_USART_RECV)
{
	uart_buffer[writeptr] = UDR0;
	writeptr = (writeptr + 1) % BUFFER_SIZE;
}

int main(void)
{
	int i;
	uint8_t c;
	FRESULT res;
	uint16_t bytes_written;
	char *sentence[15];
	char filename[] = "gps001.txt";
	uint8_t fc = 0; // used for file name inc

	DDRC = ALL_OUTPUT;

	/* 8-bit timer for about 8khz timer */

	LED_OFF(1); // power
	LED_ON(2); // sd status
	LED_ON(3); // gps lock
	LED_OFF(4); // unused

	LED_ON(1);

	/* init the uart */
	init_serial();

	/* setup callbacks for stdio use */
	fdevopen(send_serial,NULL);

	/* say hello */
	printf("\n\r\n\r**  mmc_demo is alive. **\n\r");

	/* init mmc card and report status */
	i = mmc_init();
	if (i)
	{
		LED_OFF(2);
	}
	else
	{
		LED_ON(2);
	}

	if (f_mount(0, &fatfs))
	{
		LED_ON(2);
		printf ("mount failed\n\r");
	}
	else
	{
		LED_OFF(2);
		printf ("mount successfull\n\r");
	}

	res = FR_NO_FILE;
	while ((FR_OK != res) && (fc < 250)) // we only try 250 times
	{
		snprintf(filename, sizeof(filename), "gps%03d.txt", fc++);
		printf("res = %d trying %s\n\r", res, filename);
		res = f_open(&logfile, filename, FA_CREATE_NEW | FA_WRITE);
	}
	if (res)
	{
		printf("file open failed\n\r");
		LED_ON(2);
		cli();
	}
	else
	{
		printf("file opened succesfully\n\r");
		LED_OFF(2);
		sei();
	}

	/* main loop */
	while (1)
	{
		while(writeptr == readptr); // block waiting
		c = uart_buffer[readptr];
		readptr = (readptr + 1) % BUFFER_SIZE;
		send_serial(c);

		if (gps_parse(c, sentence)) // done with sentence 
		{
			if ('A' == *sentence[STATUS])
			{
				LED_OFF(3);
			}
			else
			{
				LED_ON(3);
			}
		}

		if ((f_write(&logfile, &c, 1, &bytes_written) == FR_OK)
				&& (1 == bytes_written))
		{
			LED_OFF(2);
		}
		else
		{
			LED_ON(2);
			cli(); // disable interrupts
			printf("Oops, only wrote %d bytes\n\r", bytes_written);
		}

		/* sync the data in case we loose power */
		if ('\n' == c)
		{
			if (f_sync(&logfile) == FR_OK )
			{
				LED_OFF(2);
			}
			else
			{
				LED_ON(2);
				cli(); // disable interrupts
				printf("error syncing to disk.\n\r");
			}
		}
	}

	f_mount(0, NULL);
}
