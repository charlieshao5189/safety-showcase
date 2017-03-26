/*
* ledlight.c
*
* Created: 11/11/2016 4:07:32 PM
*  Author: charlie
*/
#include <avr/io.h>

void LEDLIGHT_Timer4_PWM_ChannelA_Init()
{
	DDRH |=(1<<PH3);
	// TCCR4A ?Timer/Counter 4 Control Register A
	// Bit 7:6 ?COMnA1:0: Compare Output Mode for Channel A
	// Bit 5:4 ?COMnB1:0: Compare Output Mode for Channel B
	// Bit 3:2 ?COMnC1:0: Compare Output Mode for Channel C
	// Bit 1:0 ?WGMn1:0: Waveform Generation Mode (0101 Fast PWM, 8-bit)
	TCCR4A = 0b10000001;	// No output pins in use, set all to normal mode, waveform  = Fast PWM, 8-bit
	
	// TCCR4B ?Timer/Counter 4 Control Register B
	// Bit 7 ?ICNCn: Input Capture Noise Canceler
	// Bit 6 ?ICESn: Input Capture Edge Select
	// Bit 4 ?Reserved Bit
	// Bit 4:3 ?WGMn3:2: Waveform Generation Mode (0101 Fast PWM, 8-bit)
	// Bit 2:0 ?CSn2:0: Clock Select (010 = 8 prescaler)
	TCCR4B = 0b00001010; // waveform  = Fast PWM, 8-bit, 8 prescaler
	
	// TCCR4C ?Timer/Counter 4 Control Register C
	// Bit 7 ?FOCnA: Force Output Compare for Channel A
	// Bit 6 ?FOCnB: Force Output Compare for Channel B
	// Bit 5 ?FOCnC: Force Output Compare for Channel C
	TCCR4C = 0b00000000;
	
	// TCNT4H and TCNT4L –Timer/Counter 4
	TCNT4 = 0;
	
	// OCR4AH and OCR4AL ?Output Compare Register 4 A
	OCR4AH = 0x00;
	OCR4AL = 0x00;
}

