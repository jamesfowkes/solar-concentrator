#ifndef _MOTORS_H_
#define _MOTORS_H_

struct motor_axis
{
	PGM_P name;
	uint8_t step;
	uint8_t en;
	uint8_t dir;
	uint8_t home;
	uint32_t position;
};
typedef struct motor_axis MOTOR_AXIS;

enum coord
{
	COORD_AZIMUTH,
	COORD_ELEVATION
};
typedef enum coord COORD;

void motor_setup(const MOTOR_AXIS& motor);
void motor_enable(const MOTOR_AXIS& axis);
void motor_home(MOTOR_AXIS& axis);
void motor_move(MOTOR_AXIS& axis, uint32_t steps, bool direction);

#endif
