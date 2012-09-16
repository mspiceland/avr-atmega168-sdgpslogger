#ifndef _GPS_H
#define _GPS_H

#define TORADIANS(a) a *= 0.017453293
#define TODEGREES(a) a *= 57.295780

/* GPS stuff */
enum rmc_field {
	TYPE,
	TIME,
	STATUS,
	LATITUDE,
	LAT_DIR,
	LONGITUDE,
	LONG_DIR,
	GROUND_SPEED,
	TRACK_ANGLE,
	DATE,
	MAGNETIC_VARIATION
};

int8_t gps_parse(uint8_t newdata, char **sentence);

#endif
