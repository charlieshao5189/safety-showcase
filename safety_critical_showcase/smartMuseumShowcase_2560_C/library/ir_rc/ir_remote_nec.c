#include <avr/io.h>
#include <avr/interrupt.h>
#define  F_CPU 16000000UL
#include "ir_remote_nec.h"


void ir_init() {
	TCCR1B |= (1 << WGM12);     //CTC mode
	TCCR1B |= (1 << CS10); 		/* prescaler = 1 */
	TIMSK1 |= 1<<OCIE1A;	// bit 1 OCIE1A		Use 'Output Compare A Match' Interrupt, i.e. generate an interrupt
	OCR1A = ((F_CPU/1000000) * SAMPLE_RATIO)-1;; // Output Compare Registers (16 bit) OCR1BH and OCR1BL
    TCNT1 = 0;// initialize counter

    DDRD &= ~(1 << IR_PIN);      //set IR_PIN to input
    EICRA |= (1 << ISC31); 		/* Falling edge detection */
    EIMSK |= (1 << INT3); 		/* External INT3 enable */
	sei();					// Enable interrupts at global level, set Global Interrupt Enable (I) bit

	ir_reset();
	total_cycles_counter = 0;
}

void ir_reset() {
	state = STATE_WAIT;
	repeat = 0;
	cycles_counter = 0;
}

uint8_t ir_check_command(uint32_t command) {
	uint8_t address_high = (command >> 24);
	uint8_t address_low = (command >> 16);
	return (address_high & address_low) == 0;
}

void ir_check_timeout() {
	if (total_cycles_counter < TIMEOUT)
	return;

	ir_reset();
	total_cycles_counter = 0;
	check_falling_edge();
}

ISR(TIMER1_COMPA_vect) {
	cycles_counter++;
	total_cycles_counter++;
}

ISR(INT3_vect) {
	ir_check_timeout();

	switch (state) {
		case STATE_WAIT:
		if (edge == EDGE_FALLING)
		ir_handle_state_wait_edge_falling();
		else
		ir_handle_state_wait_edge_rising();
		break;

		case STATE_START:
		if (edge == EDGE_RISING)
		ir_handle_state_start_edge_rising();
		else if (edge == EDGE_FALLING)
		ir_handle_state_start_edge_falling();
		break;

		case STATE_BITS:
		if (edge == EDGE_RISING)
		ir_handle_state_bits_edge_rising();
		else if (edge == EDGE_FALLING)
		ir_handle_state_bits_edge_falling();
		break;

		case STATE_STOP:
		ir_handle_state_stop();
		break;

		default:
		ir_reset();
		break;
	}

	toggle_edge();
}

void ir_handle_state_wait_edge_falling() {
	cycles_counter = 0;
	state = STATE_START;
}

void ir_handle_state_wait_edge_rising() {
	ir_reset();
}

void ir_handle_state_start_edge_falling() {
	
	if(cycles_counter >= TIME_START_HALFBIT_HIGH_COMMAND_MIN && cycles_counter <= TIME_START_HALFBIT_HIGH_COMMAND_MAX) {
		state = STATE_BITS;
		command_bits_counter = 0;
		command = 0;
		return;
	}

	if (cycles_counter >= TIME_START_HALFBIT_HIGH_REPEAT_MIN && cycles_counter <= TIME_START_HALFBIT_HIGH_REPEAT_MAX) {
		state = STATE_STOP;
		repeat = 1;
		command = current_command;
		return;
	}

	ir_reset();
}

void ir_handle_state_start_edge_rising() {
	cycles_counter = 0;
	state = STATE_START;
}

void ir_handle_state_bits_edge_falling() {
		if (cycles_counter >= TIME_COMMAND_HALFBIT_HIGH_ZERO_MIN && cycles_counter <= TIME_COMMAND_HALFBIT_HIGH_ZERO_MAX) {
		command = (command << 1);
		command_bits_counter++;

		if (command_bits_counter < 32) {
			state = STATE_BITS;
			} else if (!ir_check_command(command)) {
			ir_reset();
			} else {
			state = STATE_STOP;
			current_command = command;
		}

		return;
	}

	if (cycles_counter >= TIME_COMMAND_HALFBIT_HIGH_ONE_MIN && cycles_counter <= TIME_COMMAND_HALFBIT_HIGH_ONE_MAX) {
		command = ((command << 1) | 1);
		command_bits_counter++;

		if (command_bits_counter < 32) {
			state = STATE_BITS;
			} else if (!ir_check_command(command)) {
			ir_reset();
			} else {
			state = STATE_STOP;
			current_command = command;
			newCommandFlag=1;
		}

		return;
	}


	command_bits_counter = 0;
	command = 0;
	ir_reset();
}

void ir_handle_state_bits_edge_rising() {
	cycles_counter = 0;
	state = STATE_BITS;
}

void ir_handle_state_stop() {
	cycles_counter = 0;
	state = STATE_WAIT;
	current_command = command;
	total_cycles_counter = 0;
}
