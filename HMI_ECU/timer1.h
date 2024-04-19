/*
 * timer1.h
 *
 *  Created on: Mar 14, 2024
 *      Author: Zyad Montaser
 */

#ifndef TIMER1_H_
#define TIMER1_H_

#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
	/*WGM12 =0 in normal mode and WGM12=1 in compare mode*/
	NormalMode,CompareMode
}Timer1_Mode;

typedef enum
{
	StopTimer,F_CPU_1,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024,ExternalCLK_Falling,ExternalCLK_RISING
}Timer1_Prescaler;

typedef struct {
 uint16 initial_value;
 uint16 compare_value; /* it will be used in compare mode only.*/
 Timer1_Prescaler prescaler;
 Timer1_Mode mode;
} Timer1_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void Timer1_init(const Timer1_ConfigType * Config_Ptr);
void Timer1_deInit(void);
void Timer1_setCallBack(void(*a_ptr)(void));


#endif /* TIMER1_H_ */
