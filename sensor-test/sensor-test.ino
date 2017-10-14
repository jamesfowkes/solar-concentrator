#include <avr/pgmspace.h>

#include <TaskAction.h>
#include <AccelStepper.h>

#include "sensors.h"
#include "motors.h"

static const SENSORS s_sensors = { .top_left=A3, .top_right=A2, .bot_left=A0, .bot_right=A1 };

static void print_sensor_reading(char const*const sensor, uint16_t reading)
{
	Serial.print(sensor);
	Serial.print(" = ");
	Serial.println(reading);
}

static void debug_sensors()
{
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

	if (motor_is_home(COORD_ELEVATION))
	{
		Serial.println("Elevation home");
	}

	if (motor_is_home(COORD_AZIMUTH))
	{
		Serial.println("Azimuth home");
	}
}


static void debug_task_fn(TaskAction * task)
{
	(void)task;
	debug_sensors();
}
static TaskAction s_debug_task(debug_task_fn, 500, INFINITE_TICKS);

void setup()
{
	Serial.begin(115200);

	Serial.print("Solar Concentrator");

	for (int i=0; i<20; i++)
	{
		Serial.print(".");
		delay(100);
	}
	Serial.println();

	sensors_setup(s_sensors);
	motor_setup();
}

void loop()
{
	s_debug_task.tick();
	sensors_service();
}
