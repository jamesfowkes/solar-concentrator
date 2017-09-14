#include <avr/pgmspace.h>

#include <TaskAction.h>

#include "motors.h"
#include "sensors.h"

static MOTOR_AXIS s_azimuth_motor = { .name="azimuth", .step=5, .en=12, .dir=4, .home=A4, .position=0 };
static MOTOR_AXIS s_elevation_motor = { .name="elevation", .step=6, .en=8, .dir=7, .home=A5, .position=0 };
static const SENSORS s_sensors = { .top_left=A0, .top_right=A1, .bot_left=A2, .bot_right=A3 };

static void print_sensor_reading(char const*const sensor, uint16_t reading)
{
	Serial.print(sensor);
	Serial.print(" = ");
	Serial.println(reading);
}

static void debug_task_fn(TaskAction * task)
{
	(void)task;
	uint16_t tl, bl, tr, br;
	tl = sensors_get_reading(TOP_LEFT);
	bl = sensors_get_reading(BOT_LEFT);
	tr = sensors_get_reading(TOP_RIGHT);
	br = sensors_get_reading(BOT_RIGHT);

	print_sensor_reading("TL", tl);
	print_sensor_reading("BL", bl);
	print_sensor_reading("TR", tr);
	print_sensor_reading("BR", br);
	
	print_sensor_reading("AZ left", sensors_get_azimuth_left(false));
	print_sensor_reading("AZ right", sensors_get_azimuth_right(false));
	print_sensor_reading("AZ", sensors_get_azimuth_state(false));

	print_sensor_reading("EL top", sensors_get_elevation_top(false));
	print_sensor_reading("EL bottom", sensors_get_elevation_bottom(false));
	print_sensor_reading("EL", sensors_get_elevation_state(false));
}
static TaskAction s_debug_task(debug_task_fn, 500, INFINITE_TICKS);

static void handle_move(COORD coord)
{
	COORD_STATE state = (coord == COORD_AZIMUTH) ? sensors_get_azimuth_state(true) : sensors_get_elevation_state(true);
	MOTOR_AXIS& axis = (coord == COORD_AZIMUTH) ? s_azimuth_motor : s_elevation_motor;
	if (state == COORD_STATE_NEG)
	{
		motor_move(axis, 1, true);
	}
	else if (state == COORD_STATE_POS)
	{
		motor_move(axis, -1, true);	
	}
}

void setup()
{
	
	Serial.begin(115200);

	motor_setup(s_elevation_motor);
	motor_setup(s_azimuth_motor);

	motor_enable(s_elevation_motor);
	motor_enable(s_azimuth_motor);

	motor_home(s_elevation_motor);
	motor_home(s_azimuth_motor);

	sensors_setup(s_sensors);
}

void loop()
{
	s_debug_task.tick();
	sensors_service();

	handle_move(COORD_AZIMUTH);
	handle_move(COORD_ELEVATION);
}

