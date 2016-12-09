/*
 * CFile1.c
 *
 * Created: 11/9/2016 3:27:16 PM
 *  Author: charlie
 */ 

#include <avr/io.h>
#include "tricolorled.h"

#define F_CPU 16000000UL
#include <util/delay.h>
//setup port
#define TRICOLORLED_DDR DDRL
#define TRICOLORLED_PORT PORTL
#define TRICOLORLED_PIN PINL

#define TRICOLORLED_RED_PIN PL0
#define TRICOLORLED_GREEN_PIN PL1
#define TRICOLORLED_BLUE_PIN PL2

void triColorLed_init(){
	
	TRICOLORLED_DDR |= (1<<TRICOLORLED_RED_PIN)|(1<<TRICOLORLED_GREEN_PIN)|(1<<TRICOLORLED_BLUE_PIN) ; //set three pins to output
	TRICOLORLED_PORT &= ~((1<<TRICOLORLED_RED_PIN)|(1<<TRICOLORLED_GREEN_PIN)|(1<<TRICOLORLED_BLUE_PIN)); //trun off all the leds
	
	//check red led
	tricolorled_onoff(LED_RED,LED_ON);
	_delay_ms(1000);
	tricolorled_onoff(LED_RED,LED_OFF);
	//check green led
	tricolorled_onoff(LED_GREEN,LED_ON);
	_delay_ms(1000);
	tricolorled_onoff(LED_GREEN,LED_OFF);
	//check blue led
	tricolorled_onoff(LED_BLUE,LED_ON);
	_delay_ms(1000);
	tricolorled_onoff(LED_BLUE,LED_OFF);	
	};
	
void tricolorled_onoff(unsigned char color, unsigned char ledonoff){
	switch(color){
		case LED_RED:
				if(ledonoff){
					TRICOLORLED_PORT |= (1<<TRICOLORLED_RED_PIN); //turn on led
				}
				else{
					TRICOLORLED_PORT &= ~(1<<TRICOLORLED_RED_PIN); //turn off led 
				};
				break;
		case LED_GREEN:
				if(ledonoff){
					TRICOLORLED_PORT |= (1<<TRICOLORLED_GREEN_PIN); //turn on led
				}
				else{
					TRICOLORLED_PORT &= ~(1<<TRICOLORLED_GREEN_PIN); //turn off led
				};
				break;
		case LED_BLUE:
				if(ledonoff){
					TRICOLORLED_PORT |= (1<<TRICOLORLED_BLUE_PIN); //turn on led
				}
				else{
					TRICOLORLED_PORT &= ~(1<<TRICOLORLED_BLUE_PIN); //turn off led
				};
				break;
		default: TRICOLORLED_PORT &= ~((1<<TRICOLORLED_RED_PIN)|(1<<TRICOLORLED_GREEN_PIN)|(1<<TRICOLORLED_BLUE_PIN)); //trun off all the leds
		         break;		
	}
	};
	
	void tricolorled_toggle(unsigned char color){
		switch(color){
			case LED_RED:
				TRICOLORLED_PORT ^= (1<<TRICOLORLED_RED_PIN); //toggle led
			    break;
			case LED_GREEN:
				TRICOLORLED_PORT ^= (1<<TRICOLORLED_GREEN_PIN); //toggle led
			    break;
			case LED_BLUE:
				TRICOLORLED_PORT ^= (1<<TRICOLORLED_BLUE_PIN); //toggle led
			    break;
			default: TRICOLORLED_PORT &= ~((1<<TRICOLORLED_RED_PIN)|(1<<TRICOLORLED_GREEN_PIN)|(1<<TRICOLORLED_BLUE_PIN)); //trun off all the leds
			    break;
		}
	};