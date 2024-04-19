/******************************************************************************
 *
 * Module: DC_Motor
 *
 * File Name: DC_motor.c
 *
 * Description: Source file for the DC_Motor driver
 *
 * Author: Zyad Montaser
 *
 *******************************************************************************/

#include "DC_motor.h"
#include "gpio.h" /*to use GPIO_setupPinDirection and GPIO_writePin functions*/
#include <avr/io.h> /* to use PORT registers*/
#include "pwm.h" /* to use PWM_Timer0_Generate function to control speed of the motor*/

void DcMotor_Init(void)
{
	/*set 2 pins of the motor as output pins*/
	GPIO_setupPinDirection(DC_motor_IN_PORT_ID,DC_motor_IN1_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(DC_motor_IN_PORT_ID,DC_motor_IN2_PIN_ID,PIN_OUTPUT);

	/*initially stop the motor */
	GPIO_writePin(DC_motor_IN_PORT_ID,DC_motor_IN1_PIN_ID,LOGIC_LOW);
	GPIO_writePin(DC_motor_IN_PORT_ID,DC_motor_IN2_PIN_ID,LOGIC_LOW);
}
void DcMotor_Rotate(DcMotor_State state,uint8 speed_percent)
{
	/*set enable pin of the motor as output pin*/
	GPIO_setupPinDirection(DC_motor_EN_PORT_ID,DC_motor_EN1_PIN_ID,PIN_OUTPUT);

	/*write the state of the motor in IN1 pin */
	GPIO_writePin(DC_motor_IN_PORT_ID,DC_motor_IN1_PIN_ID,state>>1);

	/*write the state of the motor in IN2 pin */
	GPIO_writePin(DC_motor_IN_PORT_ID,DC_motor_IN2_PIN_ID,state &1);

	/*generate pwm signal with duty cycle base on the passed value (speed_percent)*/
	PWM_Timer0_Generate(speed_percent);
}
