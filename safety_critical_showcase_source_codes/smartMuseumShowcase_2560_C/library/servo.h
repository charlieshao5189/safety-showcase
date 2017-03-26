/*
 * servo.h
 *
 * Created: 11/4/2016 1:29:53 PM
 *  Author: charlie
 */ 


#ifndef SERVO_H_
#define SERVO_H_

#define LockerOFF 0
#define LockerOn 1

void Servo_Timer3_FastPWM();
void SetServoPosition(unsigned char sw);

#endif /* SERVO_H_ */