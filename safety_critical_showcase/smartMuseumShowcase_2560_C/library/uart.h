/*
 * uart.h
 *
 * Created: 10/12/2016 5:59:06 PM
 *  Author: charlie
 */ 


#ifndef UART_H_
#define UART_H_

#include <stdio.h>
#define USART (&str_uart) 



int usart_putchar_printf(char var, FILE *stream);
void USART0_SETUP_9600_BAUD_ASSUME_1MHz_CLOCK();
void USART0_TX_SingleByte(unsigned char cByte);
void USART0_TX_String(char* sData);
void uart_gotoxy(int x, int y);
void uart_clear_screen();

//Declaration of file for Uart
static FILE str_uart = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);
//fprintf(USART,"Trg: %x\n",Trg);

#endif /* UART_H_ */