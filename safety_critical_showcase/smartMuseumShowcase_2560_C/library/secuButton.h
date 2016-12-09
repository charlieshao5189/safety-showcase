/*
 * secuButton.h
 *
 * Created: 11/10/2016 1:50:56 PM
 *  Author: charlie
 */ 


#ifndef SECUBUTTON_H_
#define SECUBUTTON_H_

volatile unsigned char securityEnableFlag;//default:1,security enable;0, security disable, mute buzzer and red led
volatile unsigned char alarmFlag;
void securityInit();
void alarm_OFF();


#endif /* SECUBUTTON_H_ */