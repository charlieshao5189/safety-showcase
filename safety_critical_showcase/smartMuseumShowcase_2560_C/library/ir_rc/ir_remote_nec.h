#ifndef __IR_REMOTE_NEC_H_
#define __IR_REMOTE_NEC_H_

#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define IR_PIN PD3


/* Last edge direction */
#define EDGE_FALLING 0
#define EDGE_RISING 1


/* Receiving signal states */
#define STATE_WAIT			0	/* When the receiver is waiting for the signal from remote controller */
#define STATE_START 		1	/* When start bit is received */
#define STATE_BITS			2	/* When command bits are received (32 bits: 8 - address, 8 - ~address, 8 - command, 8 - ~command) */
#define STATE_STOP 			3	/* When the stop bit is received */


/* Event duration times in microseconds */
#define SAMPLE_RATIO									  10 /* Sampling rate */
#define TIME_TOLLERANCE									 250 /* Tolerance */

#define TIME_COMMAND_HALFBIT_HIGH_ZERO					 560 /* High halfbit of the "0" */
#define TIME_COMMAND_HALFBIT_HIGH_ZERO_MIN 				((TIME_COMMAND_HALFBIT_HIGH_ZERO - TIME_TOLLERANCE) / SAMPLE_RATIO)
#define TIME_COMMAND_HALFBIT_HIGH_ZERO_MAX 				((TIME_COMMAND_HALFBIT_HIGH_ZERO + TIME_TOLLERANCE) / SAMPLE_RATIO)

#define TIME_COMMAND_HALFBIT_HIGH_ONE					1690 /* High halfbit of the "1" */
#define TIME_COMMAND_HALFBIT_HIGH_ONE_MIN 				((TIME_COMMAND_HALFBIT_HIGH_ONE - TIME_TOLLERANCE) / SAMPLE_RATIO)
#define TIME_COMMAND_HALFBIT_HIGH_ONE_MAX 				((TIME_COMMAND_HALFBIT_HIGH_ONE + TIME_TOLLERANCE) / SAMPLE_RATIO)

#define TIME_START_HALFBIT_HIGH_COMMAND					4500 /* High halfbit of the start bit, when the frame contains a command (not repeated) */
#define TIME_START_HALFBIT_HIGH_COMMAND_MIN				((TIME_START_HALFBIT_HIGH_COMMAND - TIME_TOLLERANCE) / SAMPLE_RATIO)
#define TIME_START_HALFBIT_HIGH_COMMAND_MAX				((TIME_START_HALFBIT_HIGH_COMMAND + TIME_TOLLERANCE) / SAMPLE_RATIO)

#define TIME_START_HALFBIT_HIGH_REPEAT					2250 /* High halfbit of the start bit, when the frame contains repeat of a command */
#define TIME_START_HALFBIT_HIGH_REPEAT_MIN				((TIME_START_HALFBIT_HIGH_REPEAT - TIME_TOLLERANCE) / SAMPLE_RATIO)
#define TIME_START_HALFBIT_HIGH_REPEAT_MAX				((TIME_START_HALFBIT_HIGH_REPEAT + TIME_TOLLERANCE) / SAMPLE_RATIO)

#define TIMEOUT											(110000 / SAMPLE_RATIO) /* Timeout */



volatile uint32_t command;
volatile uint32_t current_command;
volatile uint8_t edge;
volatile uint8_t state;
volatile uint32_t cycles_counter;
volatile uint32_t total_cycles_counter;
volatile uint8_t repeat;
volatile uint8_t command_bits_counter;
volatile unsigned char newCommandFlag;

/* Rising edge detection */
void inline check_rising_edge() {
	EICRA |= (1 << ISC31);
	EICRA |= (1 << ISC30);
	edge = EDGE_RISING;
}

/* Falling edge detection */
void inline check_falling_edge() {
	EICRA |= (1 << ISC31);
	EICRA &= ~(1 << ISC30);
	edge = EDGE_FALLING;
}

void inline toggle_edge() {
	if (edge == EDGE_FALLING) {
		check_rising_edge();
		} else if (edge == EDGE_RISING) {
		check_falling_edge();
	}
}

uint32_t inline get_current_command() {
	if(1==newCommandFlag){
		newCommandFlag=0;
		return current_command;
	}
	else{
		return 0;
	}
	
}


void ir_init();
uint8_t ir_check_command(uint32_t command);
void ir_reset();
void ir_check_timeout();

void ir_handle_state_wait_edge_falling();
void ir_handle_state_wait_edge_rising();
void ir_handle_state_start_edge_falling();
void ir_handle_state_start_edge_rising();
void ir_handle_state_bits_edge_falling();
void ir_handle_state_bits_edge_rising();
void ir_handle_state_stop();

#endif /* NEC_H_ */
