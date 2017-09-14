#include <Arduino.h>

#include <avr/pgmspace.h>

#include "motors.h"

static bool is_home(const MOTOR_AXIS& axis)
{
	return digitalRead(axis.home) == LOW;
}

static void set_direction(uint8_t pin, bool dir)
{
	digitalWrite(pin, dir);
}

static void step(const MOTOR_AXIS& axis)
{
	digitalWrite(axis.step, HIGH);
	digitalWrite(axis.step, LOW);
}

void motor_move(MOTOR_AXIS& axis, uint32_t steps, bool direction)
{
	if (steps == 0) { return; }
	set_direction(axis.dir, direction);
	while(steps && axis.position)
	{
		step(axis);
		steps--;
		axis.position += direction ? -1 : +1;
	}
	Serial.print("Motor ");
	Serial.print(axis.name);
	Serial.print(" at position ");
	Serial.println(axis.position);
}

void motor_home(MOTOR_AXIS& axis)
{
	axis.position = UINT32_MAX;
	while(!is_home(axis))
	{
		motor_move(axis, 1, true);
		delayMicroseconds(1000);
	}
	axis.position = 0;
}

void motor_enable(const MOTOR_AXIS& axis)
{
	digitalWrite(axis.en, LOW);
}

void motor_setup(const MOTOR_AXIS& motor)
{
	pinMode(motor.step, OUTPUT);
	pinMode(motor.en, OUTPUT);
	pinMode(motor.dir, OUTPUT);
	pinMode(motor.home, INPUT_PULLUP);
}
