#include <Arduino.h>
#include <AccelStepper.h>

#include <avr/pgmspace.h>

#include "motors.h"

static void elevation_fwd();
static void elevation_bck();
static void azimuth_fwd();
static void azimuth_bck();

static AccelStepper s_elevation_motor(elevation_fwd, elevation_bck);
static AccelStepper s_azimuth_motor(azimuth_fwd, azimuth_bck);

static const int16_t AZIMUTH_MAX_DEGREES=181;
static const int16_t AZIMUTH_MIN_DEGREES=-181;
static const int16_t ELEVATION_MAX_DEGREES=65;
static const int16_t ELEVATION_MIN_DEGREES=-65;

static bool s_oor_error[2];

static MOTOR_AXIS s_elevation_axis = {
	.name="elevation", .step=5,  .en=12, .dir=4, .home=A5, .min_limit=INT32_MIN, .max_limit=INT32_MAX, .motor=&s_elevation_motor
};

static MOTOR_AXIS s_azimuth_axis = {
	.name="azimuth", .step=6, .en=8, .dir=7, .home=A4, .min_limit=INT32_MIN, .max_limit=INT32_MAX, .motor=&s_azimuth_motor
};

static void step(const MOTOR_AXIS& axis)
{
	digitalWrite(axis.step, HIGH);
	digitalWrite(axis.step, LOW);
}

static void set_direction(uint8_t pin, bool dir)
{
	digitalWrite(pin, dir);
}

static void elevation_fwd() { set_direction(s_elevation_axis.dir, true), step(s_elevation_axis); }
static void elevation_bck() { set_direction(s_elevation_axis.dir, false), step(s_elevation_axis); }
static void azimuth_fwd() { set_direction(s_azimuth_axis.dir, true), step(s_azimuth_axis); }
static void azimuth_bck() { set_direction(s_azimuth_axis.dir, false), step(s_azimuth_axis); }

static bool is_home(const MOTOR_AXIS& axis)
{
	return digitalRead(axis.home) == LOW;
}

static bool position_within_limits(const MOTOR_AXIS& axis, int32_t pos)
{
	bool at_past_upper_limit = (pos >= axis.max_limit);
	bool at_past_lower_limit = (pos <= axis.min_limit);

	return !at_past_lower_limit && !at_past_upper_limit; 
}

static void motor_set_at_home(MOTOR_AXIS& axis)
{
	axis.motor->setCurrentPosition(0);
}

/* Public Functions */

MOTOR_AXIS& motor_get_axis(COORD coord)
{
	return (coord == COORD_ELEVATION) ? s_elevation_axis : s_azimuth_axis;
}

void motor_move(COORD coord, int32_t steps)
{
	if (steps == 0) { return; }

	MOTOR_AXIS& axis = motor_get_axis(coord);

	int32_t new_position = axis.motor->currentPosition() + steps;

	s_oor_error[coord] = !position_within_limits(axis, new_position);
	if (!s_oor_error[coord])
	{
		axis.motor->moveTo(new_position);
	}
}

void motor_set_at_home(COORD coord)
{
	MOTOR_AXIS& axis = motor_get_axis(coord);
	axis.motor->setCurrentPosition(0);
}

void motor_home(COORD coord)
{
	MOTOR_AXIS& axis = motor_get_axis(coord);

	while(!is_home(axis))
	{
		motor_move(coord, 1);
	}
	motor_set_at_home(axis);	
}

void motor_enable_control(COORD coord, bool en)
{
	MOTOR_AXIS& axis = motor_get_axis(coord);
	digitalWrite(axis.en, en ? LOW : HIGH);
}

void motor_setup()
{
	pinMode(s_elevation_axis.step, OUTPUT);
	pinMode(s_elevation_axis.en, OUTPUT);
	pinMode(s_elevation_axis.dir, OUTPUT);
	pinMode(s_elevation_axis.home, INPUT_PULLUP);
	s_elevation_axis.motor = &s_elevation_motor;
	s_elevation_motor.setMaxSpeed(50);
	s_elevation_motor.setAcceleration(20);
	s_elevation_axis.max_limit = degrees_to_steps(COORD_ELEVATION, ELEVATION_MAX_DEGREES);
	s_elevation_axis.min_limit = degrees_to_steps(COORD_ELEVATION, ELEVATION_MIN_DEGREES);
	
	pinMode(s_azimuth_axis.step, OUTPUT);
	pinMode(s_azimuth_axis.en, OUTPUT);
	pinMode(s_azimuth_axis.dir, OUTPUT);
	pinMode(s_azimuth_axis.home, INPUT_PULLUP);
	s_azimuth_axis.motor = &s_azimuth_motor;
	s_azimuth_motor.setMaxSpeed(1000);
	s_azimuth_motor.setAcceleration(100);
	s_azimuth_axis.max_limit = degrees_to_steps(COORD_AZIMUTH, AZIMUTH_MAX_DEGREES);
	s_azimuth_axis.min_limit = degrees_to_steps(COORD_AZIMUTH, AZIMUTH_MIN_DEGREES);

	Serial.println("Motor Data:");
	Serial.print("Elevation SPR: "); Serial.println(ELEVATION_MOTOR_MICROSTEPS);
	Serial.print("Azimuth SPR: "); Serial.println(AZIMUTH_MOTOR_MICROSTEPS);

	Serial.println("Motor Limits:");
	Serial.print("Elevation Max: "); Serial.println(motor_get_axis(COORD_ELEVATION).max_limit);
	Serial.print("Elevation Min: "); Serial.println(motor_get_axis(COORD_ELEVATION).min_limit);
	Serial.print("Azimuth Max: "); Serial.println(motor_get_axis(COORD_AZIMUTH).max_limit);
	Serial.print("Azimuth Min: "); Serial.println(motor_get_axis(COORD_AZIMUTH).min_limit);

}

bool motor_out_of_range(COORD coord) { return s_oor_error[coord]; }
void motor_run()
{
	s_elevation_motor.run();
	s_azimuth_motor.run();
}

int16_t degrees_to_steps(COORD coord, int16_t degrees)
{
	int32_t microsteps = coord == COORD_AZIMUTH ? AZIMUTH_MOTOR_MICROSTEPS : ELEVATION_MOTOR_MICROSTEPS;

	return (((int32_t)degrees * microsteps) + 180) / 360;
}
