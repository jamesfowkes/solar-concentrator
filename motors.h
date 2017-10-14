#ifndef _MOTORS_H_
#define _MOTORS_H_

static const uint16_t ELEVATION_MOTOR_SPR = 200;
static const uint16_t ELEVATION_MOTOR_MICROSTEPS = (ELEVATION_MOTOR_SPR * 2);

static const uint16_t AZIMUTH_MOTOR_SPR = 200;
static const uint16_t AZIMUTH_MOTOR_MICROSTEPS = (AZIMUTH_MOTOR_SPR * 2 * 2685UL) / 100UL;

struct motor_axis
{
	PGM_P name;
	uint8_t step;
	uint8_t en;
	uint8_t dir;
	uint8_t home;
	int32_t min_limit;
	int32_t max_limit;
	AccelStepper * motor;
};
typedef struct motor_axis MOTOR_AXIS;

enum coord
{
	COORD_AZIMUTH,
	COORD_ELEVATION
};
typedef enum coord COORD;

void motor_setup();

void motor_set_homing_params(const COORD coord);
void motor_set_normal_params(const COORD coord);

void motor_enable_control(COORD coord, bool en);

bool motor_is_home(COORD coord);
void motor_home(COORD coord);

void motor_move(COORD coord, int32_t steps);
void motor_set_at_home(COORD coord);
MOTOR_AXIS& motor_get_axis(COORD coord);

int16_t degrees_to_steps(COORD coord, int16_t degrees);

bool motor_out_of_range(COORD coord);

void motor_run();

#endif
