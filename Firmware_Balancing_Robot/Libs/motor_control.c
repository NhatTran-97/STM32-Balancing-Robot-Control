
#include "motor_control.h"

#define KE 0.12f
#define DIAMETER 0.06f
#define RESISTANCE 4.0f
#define VOLTAGE_LEVEL 7.4f


void enable_motor(motor_inst * motor)
{
	HAL_GPIO_WritePin(motor->rst_pin_port, motor->rst_pin_number, GPIO_PIN_SET);
	
}

void disable_motor(motor_inst *motor)
{
	HAL_GPIO_WritePin(motor->rst_pin_port, motor->rst_pin_number, GPIO_PIN_RESET);
	
}
void motor_init(motor_inst *motor)
{
	
}

/*
@brief Set the duty cycle of the pwm in percentage and the 
@param Motor is the motor struct.
@param Duty_cycle_percent in percentage ([+100; -100])
*/
void set_speed_open(motor_inst *motor, float duty_cycle_percent)
{
	if(duty_cycle_percent > 100.0f)
	{
		duty_cycle_percent = 100.0f;
	}
	if(duty_cycle_percent < -100.0f)
	{
		duty_cycle_percent = -100.0f;
	}
	if(duty_cycle_percent > 0.0f)
	{
		// duty_cycle = 100 * (CCR / (arr + 1))
		// CCR = (duty_cycle) * (arr + 1) / 100
		__HAL_TIM_SET_COMPARE(motor -> htim_motor, 
		motor->htim_motor_ch, duty_cycle_percent * (motor->htim_motor->Instance->ARR + 1) / 100);
		
		HAL_GPIO_WritePin(motor -> mdir_pin_port, motor -> mdir_pin_number, GPIO_PIN_SET);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(motor ->htim_motor,
		motor -> htim_motor_ch, -duty_cycle_percent * (motor -> htim_motor->Instance ->ARR + 1) / 100);
		HAL_GPIO_WritePin(motor -> mdir_pin_port, motor->mdir_pin_number, GPIO_PIN_RESET);
	}
	
}
void set_speed_zero(motor_inst *motor)
{
	__HAL_TIM_SET_COMPARE(motor ->htim_motor, motor->htim_motor_ch,0);
	
}


void apply_force(motor_inst *motor, float force, float velocity)
{
	float voltage = force * DIAMETER * RESISTANCE / KE + KE * velocity;
	set_speed_open(motor, voltage * 100 / VOLTAGE_LEVEL);
}
