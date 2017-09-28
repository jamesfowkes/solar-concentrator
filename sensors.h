#ifndef _SENSORS_H_
#define _SENSORS_H_

static const int16_t THRESHOLD = 40;

struct sensors
{
	uint8_t top_left;
	uint8_t top_right;
	uint8_t bot_left;
	uint8_t bot_right;
};
typedef struct sensors SENSORS;

enum coord_state
{
	COORD_STATE_NEG,
	COORD_STATE_NEUTRAL,
	COORD_STATE_POS
};
typedef enum coord_state COORD_STATE;

enum sensor
{
	TOP_LEFT,
	TOP_RIGHT,
	BOT_LEFT,
	BOT_RIGHT
};
typedef enum sensor SENSOR;

void sensors_setup(const SENSORS& sensors);

COORD_STATE sensors_get_azimuth_state(bool average);
COORD_STATE sensors_get_elevation_state(bool average);

uint16_t sensors_get_azimuth_left(bool average);
uint16_t sensors_get_azimuth_right(bool average);
uint16_t sensors_get_elevation_top(bool average);
uint16_t sensors_get_elevation_bottom(bool average);

uint16_t sensors_get_reading(SENSOR sensor);
uint16_t sensors_get_average(SENSOR sensor);

void sensors_service();

#endif
