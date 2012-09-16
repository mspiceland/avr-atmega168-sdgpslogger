#include <inttypes.h>
#include <string.h>
#include "gps.h"

#define GPS_BUFFER_SIZE 80

char temp[GPS_BUFFER_SIZE]; // store sentence we are parsing

/**********************************************************
* gps_parse
* newdata - new byte from serial
* **sentence - array of strings to store parsed RMC sentence
* return: 0 - still waiting for a comlete sentence
*         1 - parsed a complete sentence
**********************************************************/
int8_t gps_parse(uint8_t newdata, char **sentence)
{
	static int offset = 0;
	static char parsing = 0;
	int i;

	if ('$' == newdata)
	{
		parsing = 1;
		memset(temp, '\0', GPS_BUFFER_SIZE);
	}
	if (parsing && (offset < (sizeof(temp))))
	{
		temp[offset++] = newdata;
	}
	if ('\n' == newdata)
	{
		if (!(strncmp("$GPRMC",temp, 6)))
		{
			sentence[0] = temp;
			for (i = 1; i < 5; i++)
			{
				sentence[i] = (char *)(strchrnul(sentence[i-1], ',') + 1);
				if (i > 1)
				{
					*(char *)(strchrnul(sentence[i-1], ',')) = '\0';
				}
			}
			return 1; // we just parsed our sentence
		}
		offset = 0;
		parsing = 0;
	}
	return 0;
}
