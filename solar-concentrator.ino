#include <avr/pgmspace.h>

#include <TaskAction.h>
#include <AccelStepper.h>

#include "motors.h"
#include "sensors.h"

static const SENSORS s_sensors = { .top_left=A3, .top_right=A2, .bot_left=A0, .bot_right=A1 };

/*static int16_t steps_to_degrees(int16_t steps)
{
	return ((steps * 360) + (MOTOR_MICROSTEPS / 2)) / MOTOR_MICROSTEPS;
}*/

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
}

static void debug_elevation_motor()
{
	Serial.print  ("Elevation: Target: ");
	Serial.print  (motor_get_axis(COORD_ELEVATION).motor->targetPosition());
	Serial.print  (", Current: ");
	Serial.println(motor_get_axis(COORD_ELEVATION).motor->currentPosition());
}

static void debug_azimuth_motor()
{
	Serial.print  ("Azimuth: Target: ");
	Serial.print  (motor_get_axis(COORD_AZIMUTH).motor->targetPosition());
	Serial.print  (", Current: ");
	Serial.println(motor_get_axis(COORD_AZIMUTH).motor->currentPosition());
}

static void debug_task_fn(TaskAction * task)
{
	(void)task;
	if (0) { debug_sensors(); }
	if (1) { debug_elevation_motor(); }
	if (0) { debug_azimuth_motor(); }
}
static TaskAction s_debug_task(debug_task_fn, 500, INFINITE_TICKS);

static void handle_move(COORD coord)
{
	COORD_STATE state = (coord == COORD_AZIMUTH) ? sensors_get_azimuth_state(true) : sensors_get_elevation_state(true);
	if (state == COORD_STATE_NEG)
	{
		//motor_move(coord, 1);
	}
	else if (state == COORD_STATE_POS)
	{
		//motor_move(coord, -1);	
	}
}

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

	motor_setup();

	motor_enable_control(COORD_ELEVATION, true);
	motor_enable_control(COORD_AZIMUTH, false);

	motor_set_at_home(COORD_ELEVATION);

	motor_move(COORD_ELEVATION, degrees_to_steps(COORD_ELEVATION, 90));
	while(motor_get_axis(COORD_ELEVATION).motor->distanceToGo()) { motor_run(); s_debug_task.tick(); }
	
	motor_move(COORD_ELEVATION, -degrees_to_steps(COORD_ELEVATION, 180));
	while(motor_get_axis(COORD_ELEVATION).motor->distanceToGo()) { motor_run(); s_debug_task.tick(); }
	
	motor_move(COORD_ELEVATION, degrees_to_steps(COORD_ELEVATION, 90));
	while(motor_get_axis(COORD_ELEVATION).motor->distanceToGo()) { motor_run(); s_debug_task.tick(); }
	
	while(true)
	{
		motor_move(COORD_AZIMUTH, degrees_to_steps(COORD_AZIMUTH, 45));
		while(motor_get_axis(COORD_AZIMUTH).motor->distanceToGo())
		{
			motor_run();
			s_debug_task.tick();
		}
		delay(200);
		motor_move(COORD_AZIMUTH, degrees_to_steps(COORD_AZIMUTH, -45));
		while(motor_get_axis(COORD_AZIMUTH).motor->distanceToGo())
		{
			motor_run();
			s_debug_task.tick();
		}
		delay(200);
	}

	sensors_setup(s_sensors);
}

void loop()
{
	s_debug_task.tick();
	sensors_service();

	motor_run();

	handle_move(COORD_AZIMUTH);
	handle_move(COORD_ELEVATION);
}
