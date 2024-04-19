/*
 * Control_ECU.c
 *
 *  Created on: Mar 15, 2024
 *      Author: Zyad Montaser
 */

#include <avr/io.h> /* to use TCNT1 and SREG register*/
#include "buzzer.h"
#include "DC_motor.h"
#include "external_eeprom.h"
#include "uart.h"
#include "twi.h"
#include "timer1.h"

/* codes of communication between HMI_ECU and CONTROL_ECU */
#define CHANGE_PASS			   0xF4
#define OPEN_DOOR			   0xF5

#define NO_ERROR			   0xF6
#define ERROR_ACTION		   0xF7

#define CONTROL_ECU_IS_READY   0XFB
#define HMI_ECU_IS_READY 	   0XFC

#define UNMATCHED_PASSWORDS    0xFD
#define MATCHED_PASSWORDS      0xFE

#define PASSWORD_EEPROM_ADRS   0x0300
#define CONTROL_ECU_ADRS	   10

/*global variables*/
uint8 g_tick=0;			/*Timer1 tick variable*/
uint8 g_time_1min=0;	/*Flag of 1 min */
uint8 g_time_15sec=0;	/*Flag of 15 sec */
uint8 g_time_3sec=0;	/*Flag of 3 sec */

void timer1_1min_callback(void)
{
	g_tick++;
	if(g_tick==4)
	{
		g_time_1min=1;
	}
}

void timer1_15sec_callback(void)
{
    g_time_15sec = 1;
}

void timer1_3sec_callback(void)
{
	g_time_3sec=1;
}

void calc_1min(void)
{
	Timer1_ConfigType Timer1_1min_Configuration={0,11111,F_CPU_1024,CompareMode};
	Timer1_init(&Timer1_1min_Configuration);
	Timer1_setCallBack(&timer1_1min_callback);

	while(g_time_1min != 1){}

	g_time_1min=0;
}

void calc_15sec(void)
{
    Timer1_ConfigType Timer1_15sec_Configuration = {0, 11111, F_CPU_1024, CompareMode};
    Timer1_init(&Timer1_15sec_Configuration);
    Timer1_setCallBack(&timer1_15sec_callback);

    while (g_time_15sec != 1) {}

    g_time_15sec = 0;
}

void calc_3sec(void)
{
    Timer1_ConfigType Timer1_3sec_Configuration = {0, 2222, F_CPU_1024, CompareMode};
    Timer1_init(&Timer1_3sec_Configuration);
    Timer1_setCallBack(&timer1_3sec_callback);

    while (g_time_3sec != 1) {}

    g_time_3sec = 0;
}

void wait_and_sendByte(uint8 DataByte)
{
	/*Send readiness signal to HMI_ECU*/
	UART_sendByte(CONTROL_ECU_IS_READY);
	/* Wait until the HMI_ECU is ready to receive the data */
	while(UART_recieveByte() != HMI_ECU_IS_READY){}
	/* Send the required byte to the HMI_ECU*/
	UART_sendByte(DataByte);
}

uint8 wait_and_receiveByte(void)
{
    /* Wait for readiness signal from HMI_ECU*/
    while (UART_recieveByte() != HMI_ECU_IS_READY) {}
    /*Send readiness signal to HMI_ECU*/
    UART_sendByte(CONTROL_ECU_IS_READY);
    /* Now receive actual data from HMI_ECU*/
    return UART_recieveByte();
}

void wait_and_receiveArray(uint8 *arr,uint8 size)
{
    /* Wait for readiness signal from HMI_ECU*/
    while (UART_recieveByte() != HMI_ECU_IS_READY) {}
    /*Send readiness signal to HMI_ECU*/
    UART_sendByte(CONTROL_ECU_IS_READY);
    /* Now receive the array from HMI_ECU*/
    UART_receiveArray(arr,size);
}

uint8 compareArrays(uint8 *arr1,uint8 *arr2,uint8 size)
{
	uint8 i;
	for(i=0;i<size;i++)
	{
		if(arr1[i]!=arr2[i])
		{
			return 0;
		}
	}
	return 1;
}

/*function responsible for receive 2 passwords and check their equality
 * then send the check feedback to HMI_ECU and save the password in EEPROM in case they are equal*/
void Check_Created_Password(void)
{
	uint8 pass1[5],pass2[5];

	/* Receive pass1 array from HMI_ECU through UART */
	wait_and_receiveArray(pass1,5);

	/* Receive pass2 array from HMI_ECU through UART */
	wait_and_receiveArray(pass2,5);

	/*if the 2 passwords are unmatched */
	if(compareArrays(pass1,pass2,5)==0)
	{
		wait_and_sendByte(UNMATCHED_PASSWORDS);
		Check_Created_Password();
	}

	/*if the 2 passwords are matched */
	else if(compareArrays(pass1,pass2,5)==1)
	{
		TWI_ConfigType TWI_configuration={CONTROL_ECU_ADRS,FastMode};
		TWI_init(&TWI_configuration);

		wait_and_sendByte(MATCHED_PASSWORDS);

		/*save the password in the memory*/
		EEPROM_writeArray(PASSWORD_EEPROM_ADRS,pass1,5);
	}
}

/*function to receive a password then compare it with the saved one in the EEPROM
 * then send the check feedback to HMI_ECU */
void Check_opendoor_Password(void)
{
	uint8 entered_pass[5],memory_pass[5],temp;

	/* Receive password array from HMI_ECU through UART */
	wait_and_receiveArray(entered_pass,5);

	EEPROM_readArray(PASSWORD_EEPROM_ADRS,memory_pass,5);

	/*if the 2 passwords are unmatched */
	if(compareArrays(entered_pass,memory_pass,5)==0)
	{
		wait_and_sendByte(UNMATCHED_PASSWORDS);

		temp=wait_and_receiveByte();
		if( temp==ERROR_ACTION)
		{
			Buzzer_init();
			Buzzer_on();

			calc_1min();

			Buzzer_off();
		}
		else if (temp ==NO_ERROR)
		{
			Check_opendoor_Password();
		}
	}

	/*if the 2 passwords are matched*/
	else if(compareArrays(entered_pass,memory_pass,5)==1)
	{
		wait_and_sendByte(MATCHED_PASSWORDS);

	/*open the door(operate motor for 15sec CW then hold it 3sec then operate it for 15sec ACW)*/
		DcMotor_Init();
		DcMotor_Rotate(DcMotor_CW,100);

		calc_15sec();

		DcMotor_Rotate(DcMotor_OFF,0);

		calc_3sec();

		DcMotor_Rotate(DcMotor_A_CW,100);

		calc_15sec();
		DcMotor_Rotate(DcMotor_OFF,0);
	}
}

/*function to receive a password then compare it with the saved one in the EEPROM
 * then send the check feedback to HMI_ECU */
void check_changepassword(void)
{
	uint8 entered_pass[5],memory_pass[5],temp;

	/* Receive pass array from HMI_ECU through UART */
	wait_and_receiveArray(entered_pass,5);

	EEPROM_readArray(PASSWORD_EEPROM_ADRS,memory_pass,5);

	/*if the 2 passwords are unmatched */
	if(compareArrays(entered_pass,memory_pass,5)==0)
	{
		wait_and_sendByte(UNMATCHED_PASSWORDS);

		temp=wait_and_receiveByte();
		if( temp==ERROR_ACTION)
		{
			Buzzer_init();
			Buzzer_on();

			calc_1min();

			Buzzer_off();
		}
		else if (temp ==NO_ERROR)
		{
			check_changepassword();
		}
	}

	/*if the 2 passwords are matched */
	else if(compareArrays(entered_pass,memory_pass,5)==1)
	{
		wait_and_sendByte(MATCHED_PASSWORDS);
	}
}

int main()
{
	uint8 choice;

	SREG |=(1<<7);/*enable I-Bit for timer interrupt*/

	UART_ConfigType UART_Frame={Eight_Bits,Even_Parity,One_StopBit,9600};
	UART_init(&UART_Frame);

	while(1)
	{
		Check_Created_Password();

		choice=wait_and_receiveByte();

		if(choice ==OPEN_DOOR)
			Check_opendoor_Password();
		else if(choice==CHANGE_PASS)
			check_changepassword();
	}
}
