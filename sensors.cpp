#include <Arduino.h>

#include <TaskAction.h>

#include "sensors.h"

static SENSORS const * sp_sensors = NULL;

uint16_t s_averages[4];

static void averager_task_fn(TaskAction*task)
{
	(void)task;
	s_averages[0] = (sensors_get_reading(TOP_LEFT) + s_averages[0]) / 2;
	s_averages[1] = (sensors_get_reading(TOP_RIGHT) + s_averages[1]) / 2;
	s_averages[2] = (sensors_get_reading(BOT_LEFT) + s_averages[2]) / 2;
	s_averages[3] = (sensors_get_reading(BOT_RIGHT) + s_averages[3]) / 2;
}
static TaskAction s_averager_task(averager_task_fn, 5, INFINITE_TICKS);

static COORD_STATE get_coord_state(uint16_t positive, uint16_t negative)
{
	int16_t diff = positive - negative;

	if (abs(diff) <= THRESHOLD)
	{
		return COORD_STATE_NEUTRAL;
	}

	if (diff > (THRESHOLD))
	{
		return COORD_STATE_POS;
	}

	if (diff < (-THRESHOLD))
	{
		return COORD_STATE_NEG;
	}

	return COORD_STATE_NEUTRAL;
}

void sensors_setup(const SENSORS& sensors)
{
	sp_sensors = &sensors;

	pinMode(sensors.top_left, INPUT);
	pinMode(sensors.bot_left, INPUT);
	pinMode(sensors.top_right, INPUT);
	pinMode(sensors.bot_right, INPUT);
}

COORD_STATE sensors_get_azimuth_state(bool average)
{
	uint16_t left;
	uint16_t right;

	if (average)
	{
		left = s_averages[TOP_LEFT] + s_averages[BOT_LEFT];
		right = s_averages[TOP_RIGHT] + s_averages[BOT_RIGHT];
	}
	else
	{
		left = analogRead(sp_sensors->top_left) + analogRead(sp_sensors->top_right);
		right = analogRead(sp_sensors->bot_left) + analogRead(sp_sensors->bot_right);
	}

	return get_coord_state(left, right);
}

COORD_STATE sensors_get_elevation_state(bool average)
{
	uint16_t top;
	uint16_t bottom;

	if (average)
	{
		top = s_averages[TOP_LEFT] + s_averages[TOP_RIGHT];
		bottom = s_averages[BOT_LEFT] + s_averages[BOT_RIGHT];
	}
	else
	{
		top = analogRead(sp_sensors->top_left) + analogRead(sp_sensors->top_right);
		bottom = analogRead(sp_sensors->bot_left) + analogRead(sp_sensors->bot_right);
	}

	return get_coord_state(top, bottom);
}

uint16_t sensors_get_reading(SENSOR sensor)
{
	switch(sensor)
	{
	case TOP_LEFT:
		return analogRead(sp_sensors->top_left);
	case TOP_RIGHT:
		return analogRead(sp_sensors->top_right);
	case BOT_LEFT:
		return analogRead(sp_sensors->bot_left);
	case BOT_RIGHT:
		return analogRead(sp_sensors->bot_right);
	}

	return 0;
}

uint16_t sensors_get_average(SENSOR sensor)
{
	return s_averages[sensor];
}

uint16_t sensors_get_azimuth_left(bool average)
{
	if(average)
	{
		return s_averages[TOP_LEFT] + s_averages[BOT_LEFT];
	}
	else
	{
		return sensors_get_reading(TOP_LEFT) + sensors_get_reading(BOT_LEFT);
	}
}

uint16_t sensors_get_azimuth_right(bool average)
{
	if(average)
	{
		return s_averages[TOP_RIGHT] + s_averages[BOT_RIGHT];
	}
	else
	{
		return sensors_get_reading(TOP_RIGHT) + sensors_get_reading(BOT_RIGHT);
	}
}

uint16_t sensors_get_elevation_top(bool average)
{
	if(average)
	{
		return s_averages[TOP_LEFT] + s_averages[TOP_RIGHT];
	}
	else
	{
		return sensors_get_reading(TOP_LEFT) + sensors_get_reading(TOP_RIGHT);
	}
}

uint16_t sensors_get_elevation_bottom(bool average)
{
	if(average)
	{
		return s_averages[BOT_LEFT] + s_averages[BOT_RIGHT];
	}
	else
	{
		return sensors_get_reading(BOT_LEFT) + sensors_get_reading(BOT_RIGHT);
	}
}

void sensors_service()
{
	s_averager_task.tick();
}
