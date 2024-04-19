 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

/*
 * enumeration defines 2 parity modes ordered with the same sequence of the ATmega 32 manual
 * UPM1 --> 0 & UPM0 --> 0 : disable parity
 * UPM1 --> 1 & UPM0 --> 0 : use even parity
 * UPM1 --> 1 & UPM0 --> 1 : use odd parity
 * */
typedef enum
{
	Disable_Parity,Even_Parity=2,Odd_Parity
}UART_Parity;

typedef enum
{
	One_StopBit,Two_StopBits
}UART_StopBit;

typedef enum
{
	Five_Bits,Six_Bits,Seven_Bits,Eight_Bits,Nine_Bits=7
}UART_BitData;

typedef struct{
 UART_BitData bit_data;
 UART_Parity parity;
 UART_StopBit stop_bit;
 uint32 UART_BaudRate;
}UART_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

void UART_sendArray(const uint8*arr,uint8 size);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

void UART_receiveArray(uint8*arr,uint8 size);

#endif /* UART_H_ */
