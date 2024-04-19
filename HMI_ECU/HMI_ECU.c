/*
 * HMI_ECU.c
 *
 *  Created on: Mar 15, 2024
 *      Author: Zyad Montaser
 */

#include "timer1.h"
#include "keypad.h"
#include "lcd.h"
#include "uart.h"
#include <avr/io.h> /* to use TCNT1 and SREG register*/
#include <util/delay.h> /*for delay function*/

#define PLUSBUTTON  '+'
#define MINUSBUTTON '-'
#define EQUALBUTTON '='

/* codes of communication between HMI_ECU and CONTROL_ECU */
#define CHANGE_PASS			   0xF4
#define OPEN_DOOR			   0xF5

#define NO_ERROR			   0xF6
#define ERROR_ACTION		   0xF7

#define CONTROL_ECU_IS_READY   0XFB
#define HMI_ECU_IS_READY 	   0XFC

#define UNMATCHED_PASSWORDS    0xFD
#define MATCHED_PASSWORDS      0xFE

/*function prototype*/
void MainOptions(void);

/*Global variables*/
uint8 g_tick=0; 			/*Timer1 tick variable*/
uint8 g_time_1min=0; 		/*Flag of 1 min */
uint8 g_time_15sec=0;		/*Flag of 15 sec */
uint8 g_time_3sec=0;		/*Flag of 3 sec */
uint8 g_wrong_entry_counter=0;

/*function to get the password from the user and store it in an array
 * and display * for each number then go to infinite loop until press = */
void getPassword(uint8 *pass,uint8 size,uint8 firstPlace)
{
	uint8 i,key;
	for(i=0;i<size;i++)
	{
		key=KEYPAD_getPressedKey();
		if(key <10 )/*if the user enters any number from 0 to 9 store it in pass array*/
		{
			pass[i]=key;
			LCD_moveCursor(1,firstPlace+i);
			LCD_displayCharacter('*');
			_delay_ms(20);
		}
		else if(key ==13)/*handling the delete button */
		{
			i--;
			LCD_moveCursor(1,firstPlace+i);
			LCD_displayCharacter(' ');
			i--;
			_delay_ms(20);
		}
		else/*if the user enters any other button*/
		{
			while(key >10){key=KEYPAD_getPressedKey();}
		}
	}
	/*wait for pressing =(enter) */
	while(KEYPAD_getPressedKey() !=EQUALBUTTON){}
}

void timer1_1min_callback(void)
{
	g_tick++;
	if(g_tick==4)
	{
		g_time_1min=1;
		g_tick=0;
	}
}

void timer1_15sec_callback(void)
{
	g_time_15sec=1;
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
	Timer1_ConfigType Timer1_15sec_Configuration={0,11111,F_CPU_1024,CompareMode};
	Timer1_init(&Timer1_15sec_Configuration);
	Timer1_setCallBack(&timer1_15sec_callback);

	while(g_time_15sec != 1){}

	g_time_15sec=0;
}

void wait_and_sendArray(uint8 *arr,uint8 size)
{
	/*Send readiness signal to Control_ECU*/
	UART_sendByte(HMI_ECU_IS_READY);
	/* Wait until the Control_ECU is ready to receive the array */
	while(UART_recieveByte() != CONTROL_ECU_IS_READY){}
	/* Send the array to Control_ECU through UART */
	UART_sendArray(arr,size);
}

void wait_and_sendByte(uint8 DataByte)
{
	/*Send readiness signal to Control_ECU*/
	UART_sendByte(HMI_ECU_IS_READY);
	/* Wait until the Control_ECU is ready to receive the data */
	while(UART_recieveByte() != CONTROL_ECU_IS_READY){}
	/* Send the required byte to the Control_ECU*/
	UART_sendByte(DataByte);
}

uint8 wait_and_receiveByte(void)
{
    /* Wait for readiness signal from Control_ECU*/
    while (UART_recieveByte() != CONTROL_ECU_IS_READY) {}
    /*Send readiness signal to Control_ECU*/
    UART_sendByte(HMI_ECU_IS_READY);
    /* Now receive actual data from Control_ECU*/
    return UART_recieveByte();
}

void operate_motor(void)
{
	LCD_clearScreen();
	LCD_displayString("Door is Unlocked");

	calc_15sec();

	LCD_clearScreen();
	LCD_displayString("Door is locked");

	calc_15sec();
}

void wrong_entry_3times(void)
{
	LCD_clearScreen();
	LCD_displayString("ERROR !!!");

	/*ask the CONTROL_ECU to activate the buzzer*/
	wait_and_sendByte(ERROR_ACTION);

	calc_1min();

	g_wrong_entry_counter=0;

	MainOptions();
}

void OpenDoor(void)
{
	uint8 pass[5],CheckPass;

	LCD_clearScreen();
	LCD_displayString("plz enter pass:");

	/*Get the password from the user and store it in pass array
	 * and display '*' for each number entered and wait for pressing =(enter)*/
	getPassword(pass,5,0);

	/* Send pass array to CONTROL_ECU through UART */
	wait_and_sendArray(pass,5);

	/* Receive password feedback from Control_ECU through UART */
	CheckPass=wait_and_receiveByte();

	/*if the password is matched with the saved one in the EEPROM*/
	if(CheckPass==MATCHED_PASSWORDS)
	{
		g_wrong_entry_counter=0;
		operate_motor();
	}

	/*if the password is unmatched with the saved one in the EEPROM*/
	else if(CheckPass== UNMATCHED_PASSWORDS)
	{
		LCD_clearScreen();
		LCD_displayString("Wrong password");
		LCD_displayStringRowColumn(1,0,"Retry");
		_delay_ms(200);/*delay for vision*/

		g_wrong_entry_counter++;
		if(g_wrong_entry_counter ==3)
		{
			wrong_entry_3times();
		}
		else
		{
			wait_and_sendByte(NO_ERROR);
			OpenDoor();
		}
	}
}

void CreatePassword(void)
{
	uint8 pass1[5]={0},pass2[5]={0},CheckPass;

	LCD_clearScreen();
	LCD_displayString("plz enter pass:");

	/*Get the 1st password from the user and store it in pass1 array
	 * and display '*' for each number entered and wait for pressing =(enter)*/
	getPassword(pass1,5,0);

	/*clear screen and display the new string*/
	LCD_clearScreen();
	LCD_displayString("plz re-enter the");
	LCD_displayStringRowColumn(1,0,"same pass: ");

	/*Get the 2nd password from the user and store it in pass2 array
	 * and display '*' for each number entered and wait for pressing =(enter)*/
	getPassword(pass2,5,11);

	/* Send pass1 array to CONTROL_ECU through UART */
	wait_and_sendArray(pass1,5);

	/* Send pass2 array to CONTROL_ECU through UART */
	wait_and_sendArray(pass2,5);

	/* Receive passwords feedback from Control_ECU through UART */
	CheckPass=wait_and_receiveByte();

	/*if 2 different passwords are entered ,the Control_ECU will send 0xFD(unmatched)
	 * then call again this function */
	if(CheckPass== UNMATCHED_PASSWORDS)
	{
		LCD_clearScreen();
		LCD_displayString("Passwords differ");
		LCD_displayStringRowColumn(1,0,"Retry");
		_delay_ms(200);/*delay for vision*/
		CreatePassword();
	}

	/*if 2 matched passwords are entered ,the Control_ECU will send 0xFE(matched)
	 * then go to the next step*/
	else if(CheckPass==MATCHED_PASSWORDS)
	{
		LCD_clearScreen();
		LCD_displayString("Password created");
		LCD_displayStringRowColumn(1,0,"successfully");
		_delay_ms(200);/*delay for vision*/
		MainOptions();
	}
}

void ChangePassword(void)
{
	uint8 pass[5],CheckPass;
	LCD_clearScreen();
	LCD_displayString("plz enter old");
	LCD_displayStringRowColumn(1,0,"password:");

	/*Get the password from the user and store it in pass array
	 * and display '*' for each number entered and wait for pressing =(enter)*/
	getPassword(pass,5,10);

	/* Send pass array to CONTROL_ECU through UART */
	wait_and_sendArray(pass,5);

	/* Receive passwords feedback from Control_ECU through UART */
	CheckPass=wait_and_receiveByte();

	/*if the password is matched with the saved one in the EEPROM*/
	if(CheckPass== MATCHED_PASSWORDS)
	{
		g_wrong_entry_counter=0;
		CreatePassword();
	}

	/*if the password is unmatched with the saved one on the EEPROM*/
	else if(CheckPass==UNMATCHED_PASSWORDS)
	{
		LCD_clearScreen();
		LCD_displayString("Wrong password");
		LCD_displayStringRowColumn(1,0,"Retry");
		_delay_ms(200);/*delay for vision*/

		g_wrong_entry_counter++;
		if(g_wrong_entry_counter ==3)
		{
			wrong_entry_3times();
		}
		else
		{
			wait_and_sendByte(NO_ERROR);
			ChangePassword();
		}
	}
}

/*function to display main options and get choice from the user then send it to the control_ecu*/
void MainOptions(void)
{
	uint8 key;
	LCD_clearScreen();
	LCD_displayString("+ : Open Door");
	LCD_displayStringRowColumn(1,0,"- : Change Pass");

	key=KEYPAD_getPressedKey();

	while(key != PLUSBUTTON && key != MINUSBUTTON)
	{
		KEYPAD_getPressedKey();
	}
	if(key == PLUSBUTTON)
	{
		wait_and_sendByte(OPEN_DOOR);
		OpenDoor();
	}
	else if(key == MINUSBUTTON)
	{
		wait_and_sendByte(CHANGE_PASS);
		ChangePassword();
	}
}

int main()
{
	SREG |=(1<<7);/*enable I-Bit for timer interrupt*/

	UART_ConfigType UART_Frame={Eight_Bits,Even_Parity,One_StopBit,9600};
	UART_init(&UART_Frame);

	LCD_init();

	LCD_displayString("Door Locker");
	LCD_displayStringRowColumn(1,0,"Security System");
	_delay_ms(200);

	LCD_clearScreen();
	LCD_displayString("Create password");
	_delay_ms(100);

	CreatePassword();
	while(1)
	{

	}
}

