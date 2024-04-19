/******************************************************************************
 *
 * Module: PWM
 *
 * File Name: pwm.h
 *
 * Description: Header file for the ATmega32 PWM driver
 *
 * Author: Zyad Montaser
 *
 *******************************************************************************/
#ifndef PWM_H_
#define PWM_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define pwm_PORT_ID PORTB_ID

#define pwm_PIN_ID  PIN3_ID

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
/* function responsible for generate pwm signal using timer0 with different duty cycles*/
void PWM_Timer0_Generate(uint8 duty_cycle);

#endif /* PWM_H_ */
