/*
 * timer1.c
 *
 *  Created on: Mar 14, 2024
 *      Author: Zyad Montaser
 */
#include "timer1.h"
#include <avr/io.h> /* to use timer1 register*/
#include "common_macros.h"/*to use macros like SET_BIT()*/
#include <avr/interrupt.h> /* For Timer1 ISR */


/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;


/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();/* Call the Call Back function*/
	}
}
ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();/* Call the Call Back function*/
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{
	/* set this bit for non-PWM mode and clear all the other bits*/
	TCCR1A = (1 << FOC1A);

	/*insert the pre-scaler bits in CS12 CS11 CS10
	 *and insert bit mode in WGM12(0-->normal mode & 1-->compare mode with clearing WGM13 11 10)
	 *and clear all the other bits*/
	TCCR1B=(TCCR1B &0)|(Config_Ptr->prescaler |Config_Ptr->mode <<3);

	/*set the initial value*/
	TCNT1=Config_Ptr->initial_value;

	if(Config_Ptr->mode==CompareMode)
	{
		/*set the compare value*/
		OCR1A=Config_Ptr->compare_value;

		/*enable compare match interrupt*/
		SET_BIT(TIMSK,OCIE1A);
	}
	else if(Config_Ptr->mode==Normalmode)
	{
		/*enable overflow interrupt*/
		SET_BIT(TIMSK,TOIE1);
	}

}

void Timer1_deInit(void)
{
	/* Clear All Timer1 Registers */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	ICR1 = 0;
	TIMSK =0;

	/* Reset the global pointer value */
	g_callBackPtr = NULL_PTR;
}

void Timer1_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}

