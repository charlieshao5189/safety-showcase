/*
* servo.c
*
* Created: 11/4/2016 1:29:42 PM
*  Author: charlie
*/
#include <avr/io.h>
#include <avr/interrupt.h>

#define DDRS DDRE
#define DDSPin   DDE3
#define PORTS PORTE
#define SPin  PE3

void Servo_Timer3_FastPWM()
{
	DDRS |= (1<<DDSPin); //set servo PWM pin as OUTPUT
	PORTS &= ~(1<<SPin); //set servo pin to low
	
	// TCCR3A ?Timer/Counter 3 Control Register A
	// Bit 7:6 ?COMnA1:0: Compare Output Mode for Channel A (For FAST PWM 10 = Clear OC3A on Compare match (Non-Inverting))
	// Bit 5:4 ?COMnB1:0: Compare Output Mode for Channel B (For FAST PWM 10 = Clear OC3B on Compare match (Non-Inverting))
	// Bit 3:2 ?COMnC1:0: Compare Output Mode for Channel C (For FAST PWM 10 = Clear OC3C on Compare match (Non-Inverting))
	// Bit 1:0 ?WGMn1:0: Waveform Generation Mode (Waveform bits WGM3(3..0) 1110 Fast PWM ICR3 is TOP)
	TCCR3A = 0b10000010;	// Fast PWM non inverting, ICR3 used as TOP
	
	// TCCR3B ?Timer/Counter 3 Control Register B
	// Bit 7 ?ICNCn: Input Capture Noise Canceler
	// Bit 6 ?ICESn: Input Capture Edge Select
	// Bit 5 ?Reserved Bit
	// Bit 4:3 ?WGMn3:2: Waveform Generation Mode
	// Bit 2:0 ?CSn2:0: Clock Select
	TCCR3B = 0b00011010;	// Fast PWM, Use Prescaler 8

	// TCCR3C ?Timer/Counter 3 Control Register C
	// Bit 7 ?FOCnA: Force Output Compare for Channel A
	// Bit 6 ?FOCnB: Force Output Compare for Channel B
	// Bit 5 ?FOCnC: Force Output Compare for Channel C
	TCCR3C = 0b00000000;

	// Set Timer/Counter3 Input Capture Register (16 bit) ICR3
	// Can only be written to when using a waveform generation mode that uses ICR3 to define the TOP value
	// For the SERVO, the pulses should occur every 18ms, i.e. 18000uS
	// With a 2MHz clock speed, each clock pulse takes 0.5us, therefore need to count 36000 clock pulses
	// Decimal 36000 = 0x8CA0
	// This count value defines where a single cycle ends.
	// The actual pulse width is much shorter than the whole cycle.
	ICR3H = 0x8C; // 16-bit access (write high byte first, read low byte first)
	ICR3L = 0xA0;

	// Set Timer/Counter count/value registers (16 bit) TCNT1H and TCNT1L
	TCNT3H = 0; // 16-bit access (write high byte first, read low byte first)
	TCNT3L = 0;

	// Initialise Channel A servo to mid-range position
	// Set Timer/Counter Output Compare Registers (16 bit) OCR3AH and OCR3AL
	// Pulse width ranges from 750uS to 2250uS
	// 'Neutral' (Mid range) pulse width 1.5mS = 1500uS pulse width
	OCR3A = 3000;
	
	// TIMSK3 ?Timer/Counter 3 Interrupt Mask Register
	// Bit 5 ?ICIEn: Timer/Countern, Input Capture Interrupt Enable
	// Bit 3 ?OCIEnC: Timer/Countern, Output Compare C Match Interrupt Enable
	// Bit 2 ?OCIEnB: Timer/Countern, Output Compare B Match Interrupt Enable
	// Bit 1 ?OCIEnA: Timer/Countern, Output Compare A Match Interrupt Enable
	// Bit 0 ?TOIEn: Timer/Countern, Overflow Interrupt Enable
	TIMSK3 = 0b00000000;	// No interrupts needed, PWM pulses appear directly on OC3A (Port E Bit3)
	
	// TIFR3 ?Timer/Counter3 Interrupt Flag Register
	TIFR3 = 0b00101111;		// Clear all interrupt flags
	
	sei();// Enable interrupts at global level Set Global Interrupt Enable bit
}

// set Servo position to 0-180(+/-10)degree, Pulse width ranges from 500us to 2500us
void SetServoPosition(unsigned char sw){
	if(0==sw){
		OCR3A=2500;
	}
	else{
		OCR3A=4300;
	}
	
	/*	OCR3A = position*200/9+1000;//Minimum value*/
}