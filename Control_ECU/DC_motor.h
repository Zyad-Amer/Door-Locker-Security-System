/******************************************************************************
 *
 * Module: DC_Motor
 *
 * File Name: DC_motor.h
 *
 * Description: Header file for the DC_Motor driver
 *
 * Author: Zyad Montaser
 *
 *******************************************************************************/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define DC_motor_IN_PORT        PORTD    /*Register name*/
#define DC_motor_IN_PORT_ID     PORTD_ID /* id of the port */
#define DC_motor_IN1_PIN_ID     PIN3_ID  /* id of the pin*/
#define DC_motor_IN2_PIN_ID     PIN4_ID  /* id of the pin*/

#define DC_motor_EN_PORT_ID     PORTB_ID /* id of the port*/
#define DC_motor_EN1_PIN_ID     PIN3_ID  /* id of the pin*/

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

/*enumeration defines 3 states of the DC_motor
 * IN1 --> 0  & IN2 --> 0 :stop the motor
 * IN1 --> 1  & IN2 --> 0 :rotate the motor clockwise
 * IN1 --> 0  & IN2 --> 0 :rotate the motor anticlockwise
 * */
typedef enum
{
	DcMotor_OFF,DcMotor_CW,DcMotor_A_CW
}DcMotor_State;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*Function responsible for setup direction of the two pins of motor and initially stop it */
void DcMotor_Init(void);

/*Function responsible for rotate the motor CW or ACW and speed*/
void DcMotor_Rotate(DcMotor_State state,uint8 speed_percent);

#endif /* DC_MOTOR_H_ */
