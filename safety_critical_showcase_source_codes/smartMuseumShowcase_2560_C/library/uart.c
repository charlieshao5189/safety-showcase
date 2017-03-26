/*
 * uart.c
 *
 * Created: 10/12/2016 5:58:51 PM
 *  Author: charlie
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include "uart.h"

#define F_CPU 16000000UL

#define CR 0x0D
#define LF 0x0A
#define SPACE 0x20

void USART0_SETUP_9600_BAUD_ASSUME_1MHz_CLOCK()
{
	//UCSR0A ?USART Control and Status Register A
	// bit 7 RXC Receive Complete (flag)
	// bit 6 TXC Transmit Complete (flag)
	// bit 5 UDRE Data Register Empty (flag)
	// bit 4 FE Frame Error (flag) - programmatically clear this when writing to UCSRA
	// bit 3 DOR Data OverRun (flag)
	// bit 2 PE Parity Error
	// bit 1 UX2 Double the USART TX speed (but also depends on value loaded into the Baud Rate Registers)
	// bit 0 MPCM Multi-Processor Communication Mode
	UCSR0A = 0b00000010; // Set U2X (Double the USART Tx speed, to reduce clocking error)

	// UCSR0B - USART Control and Status Register B
	// bit 7 RXCIE Receive Complete Interrupt Enable
	// bit 6 TXCIE Transmit Complete Interrupt Enable
	// bit 5 UDRIE Data Register Empty Interrupt Enable
	// bit 4 RXEN Receiver Enable
	// bit 3 TXEN Transmitter Enable
	// bit 2 UCSZ2 Character Size (see also UCSZ1:0 in UCSRC)
	// 0 = 5,6,7 or 8-bit data
	// 1 = 9-bit data
	// bit 1 RXB8 RX Data bit 8 (only for 9-bit data)
	// bit 0 TXB8 TX Data bit 8 (only for 9-bit data)
	UCSR0B = 0b10011000;  // RX Complete Int Enable, RX Enable, TX Enable, 8-bit data

	// UCSR0C - USART Control and Status Register C
	// *** This register shares the same I/O location as UBRRH ***
	// Bits 7:6 ?UMSELn1:0 USART Mode Select (00 = Asynchronous)
	// bit 5:4 UPM1:0 Parity Mode
	// 00 Disabled
	// 10 Even parity
	// 11 Odd parity
	// bit 3 USBS Stop Bit Select
	// 0 = 1 stop bit
	// 1 = 2 stop bits
	// bit 2:1 UCSZ1:0 Character Size (see also UCSZ2 in UCSRB)
	// 00 = 5-bit data (UCSZ2 = 0)
	// 01 = 6-bit data (UCSZ2 = 0)
	// 10 = 7-bit data (UCSZ2 = 0)
	// 11 = 8-bit data (UCSZ2 = 0)
	// 11 = 9-bit data (UCSZ2 = 1)
	// bit 0 UCPOL Clock POLarity
	// 0 Rising XCK edge
	// 1 Falling XCK edge
	UCSR0C = 0b00000111;		// Asynchronous, No Parity, 1 stop, 8-bit data, Falling XCK edge

	// UBRR0 - USART Baud Rate Register (16-bit register, comprising UBRR0H and UBRR0L)
	UBRR0H = 0; // 9600 baud, UBRR = 12, and  U2X must be set to '1' in UCSRA
	UBRR0L = F_CPU/8/9600-1;
	//sei();	   // Enable interrupts at global level, set Global Interrupt Enable (I) bit
}

 void USART0_TX_SingleByte(unsigned char cByte)
{
	while(!(UCSR0A & (1 << UDRE0)));	// Wait for Tx Buffer to become empty (check UDRE flag)
	UDR0 = cByte;	// Writing to the UDR transmit buffer causes the byte to be transmitted
}

void USART0_TX_String(char* sData)
{
	int iCount;
	int iStrlen = strlen(sData);
	if(0 != iStrlen)
	{
		for(iCount = 0; iCount < iStrlen; iCount++)
		{
			USART0_TX_SingleByte(sData[iCount]);
		}
		USART0_TX_SingleByte(CR);
		USART0_TX_SingleByte(LF);
	}
}

// void USART0_DisplayBanner()
// {
// 	USART0_TX_String("\r\n\n*****************************************************");
// 	USART0_TX_String("             Atmel 2560 USART example");
// 	USART0_TX_String("*****************************************************");
// 	USART0_DisplayPrompt();
// }

// void USART0_DisplayPrompt()
// {
// 	USART0_TX_String("Enter command {1,2,3,4} >");
// }

ISR(USART0_RX_vect) // (USART_RX_Complete_Handler) USART Receive-Complete Interrupt Handler
{
//	char cData = UDR0;
// 	switch(cData)
// 	{
// 		case '1':
// 		USART0_TX_String("Command '1' received");
// 		PORTB = 0b000000001;		// Turn on LED (bit 0)
// 		break;
// 		case '2':
// 		USART0_TX_String("Command '2' received");
// 		PORTB = 0b000000010;		// Turn on LED (bit 1)
// 		break;
// 		case '3':
// 		USART0_TX_String("Command '3' received");
// 		PORTB = 0b000000100;		// Turn on LED (bit 2)
// 		break;
// 		case '4':
// 		USART0_TX_String("Command '4' received");
// 		PORTB = 0b000001000;		// Turn on LED (bit 3)
// 		break;
// 	}
// 	USART0_DisplayPrompt();
}

//Portotype functions
int usart_putchar_printf(char var, FILE *stream)
{
	if (var == '\n') USART0_TX_SingleByte('\r');
	USART0_TX_SingleByte(var);
	return 0;
}

// fprintf(USART,"im usart%d yeeeah",d2);// send format string to Uart "im usart2 yeeeah"
void uart_gotoxy(int x, int y)
{
	fprintf(USART,"%c[%d;%df",0x1B,y,x);
}

void uart_clear_screen()
{
	printf("\e[1;1H\e[2J");
}