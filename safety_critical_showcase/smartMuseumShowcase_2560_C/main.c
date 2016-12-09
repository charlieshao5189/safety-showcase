//*****************************************
// Project 		ADC Single Channel Demo Using Variable Resistor, and using Interrupts (Embedded C)
// Target 		Arduino mega2560 board
// Program		smartMuseumShowcase_2560_C.c
// Author		Charlie & Badis
// Created      11/9/2016 2:21:16 PM

// Functions	Monitor inner temperature and humidity of showcase	
//              Access control to showcase lock
//              Adjust inner light brightness
//              Security alarm
//*****************************************
#define  F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "library/uart.h"
#include "library/lcd1602//lcdpcf8574.h"
#include "library/tricolorled.h"
#include "library/dht.h"
#include "library/RFID/spi.h"
#include "library/RFID/mfrc522.h"
#include "library/servo.h"
#include "library/secuButton.h"
#include "library/ir_rc/ir_remote_nec.h"
#include "library/ir_rc/ir_nec_commands.h"
#include "library/ledlight.h"

#define LOCK    1 //locker state
#define UNLOCK  0 //locker state


volatile unsigned char sampleFlag = 10; //sample flag for DHT11, sample rate 1Hz
volatile unsigned char rfidFlag = 0; //sample flag for rfid, sample rate 1Hz
volatile unsigned char irFlag = 0; //sample flag for ir remote controller, sample rate 1Hz
int8_t humidity;
int8_t temperature;

unsigned char lockerState = 1;
unsigned char RFIDstr[MAX_LEN];
const unsigned char KEYstr[MAX_LEN] = {0xe5, 0xdc, 0x07, 0x88, 0xb6, 0, 0, 0};

void lcdFormate();//formate lcd display
void tiemr5_10ms_tick_configure();//10ms tick timer, create period(1s and 300ms) of time for sampling,
void HandleTempHum();//get temperature and humidity value from DHT11 every 1 second.
unsigned char HandleRFID(); // control locker servo according to RFID, active every 300 ms
void HandleIR();// change led light according to IR remote controller commands, active every 300 ms




int main(void)
{
    USART0_SETUP_9600_BAUD_ASSUME_1MHz_CLOCK();//for program debug, show debug information on serial monitor

    lcd_init(LCD_DISP_ON);//initialize lcd,display on, cursor off
    lcd_led(LCD_Backlight_ON); //light lcd backlight
    lcd_home();//lcd cursor go home
    /*
    ---0 1 2 3 4 5 6 7 8 9 A B C D E F---
    0--  T г║8 8 бу C     H : 8 8 %    --0
    1--L : 1 0   D : L O C   A : E N  --1
    ---0 1 2 3 4 5 6 7 8 9 A B C D E F---
    */
    lcdFormate();// formate lcd display according to the upside framework

    triColorLed_init();//three color led initialize

    tiemr5_10ms_tick_configure();// tick timer configure, create interrupt very 10ms, used to fresh flag

    spi_init();//initialize SPI port for mfrc522, AVR as master
    mfrc522_init();//initialize RFID

    Servo_Timer3_FastPWM();//initialize timer3 to fastPWM mode, create 18ms period PWM signal for servo
    SetServoPosition(LOCK);//set servo to lock position originally

    securityInit();//security button initialization,release this button will cause Alarm active

    ir_init();//initialize IR remote controller

    LEDLIGHT_Timer4_PWM_ChannelA_Init();//initialize pwm single for led light dimmer

    //USART0_TX_String("initialization finish!!!\n");
    while(1)
    {
        HandleTempHum();//every 10s, check DHT11 and display
        HandleRFID();   //every 1s, check RFID input, unlock/lock locker
        HandleIR();     //every 300 ms, check IR remoter command, new command input will be Handleed to change led light

    }
}
//set 10ms tick to measure time
void tiemr5_10ms_tick_configure()
{
    TCCR5A = 0b00000000;	// Normal port operation (OC5A, OC5B, OC5C), Clear Timer on 'Compare Match' (CTC) waveform mode)
    TCCR5B = 0b00001010;	// CTC waveform mode, use prescaler 8

    // For F_CPU Mhz cup clock to achieve a 10 millisecond(100MHz) interval:
    // Need to count F_CPU/100 clock cycles (but already divided by 8)
    // So actually need to count to (F_CPU/100 / 8-1) = 16000000/100/8 -1 = 19999 decimal, = 4E1F Hex
    OCR5AH = 0x4E; // Output Compare Registers (16 bit) OCR5BH and OCR5BL
    OCR5AL = 0x1F;

    TCNT5H = 0b00000000;	// Timer/Counter count/value registers (16 bit) TCNT5H and TCNT5L
    TCNT5L = 0b00000000;
    TIMSK5 = 0b00000010;	// bit 1 OCIE5A		Use 'Output Compare A Match' Interrupt, i.e. generate an interrupt
    // when the timer reaches the set value (in the OCR5A register)
    sei();
}


ISR(TIMER5_COMPA_vect) // TIMER5_CompareA_Handler (Interrupt Handler for Timer 5)
{
    static int s_1_count;//1 second counter
    static unsigned char ms_300_count;//300 millisecond counter

    s_1_count++;
    ms_300_count++;

    if(s_1_count >= 100)
    {
        sampleFlag++;// set sampleFlag, trigger HandleTempHum(sampleFlag) function
        rfidFlag = 1; //set rfidFlag, trigger HandleRFID(rfidFlag)  function
        if(0 == alarmFlag)
        {
            tricolorled_toggle(LED_GREEN);//green state indicator toggle
        }
        s_1_count = 0;
        //fprintf(USART,"TimerINT sampleFlag:%d,s_1_count:%d \n\t",sampleFlag,s_1_count);
    }
    if(ms_300_count >= 30)
    {
        if(1 == alarmFlag)
        {
            tricolorled_toggle(LED_RED);
            PORTA ^= ((1<<DDA3)|(1<<DDA2)|(1<<DDA1)|(1<<DDA0)); //active alarm buzzers and lights
        }
        irFlag = 1; //set irFlag, check IR remoter controller command
        ms_300_count = 0;
    }

}

void HandleTempHum()//get temperature and humidity value from DHT11 every 1 second.
{
    char buf[3];
    //fprintf(USART,"s_1_count_begin:%d,sampleFlag:%d \n\t",s_1_count,sampleFlag);
    if(10 <= sampleFlag)
    {
        //fprintf(USART,"temperature:%d,humidity:%d\n\t",temperature,humidity);
        cli();
        dht_gettemperaturehumidity(&temperature, &humidity);//get temperature and humidity from DHT11, must inside interrupt, put it outside will be interrupt by other interruption
        sei();
        lcd_gotoxy(3, 0);//set cursor to (3,0)
        itoa(temperature, buf, 10);
        lcd_puts(buf);
        lcd_gotoxy(11, 0);
        itoa(humidity, buf, 10);
        lcd_puts(buf);
        sampleFlag = 0;
        //fprintf(USART,"s_1_count_end:%d,sampleFlag:%d \n\t",s_1_count,sampleFlag);
    }
}

unsigned char HandleRFID()
{
    unsigned char RFIDbyte;
    if(1 == rfidFlag)
    {
        RFIDbyte = mfrc522_request(PICC_REQALL, RFIDstr); //read mfrc522
        if(RFIDbyte == CARD_FOUND)
        {
            RFIDbyte = mfrc522_get_card_serial(RFIDstr);
            for(RFIDbyte = 0; RFIDbyte < 8; RFIDbyte++)			 								
            {   
				//fprintf(USART,"serialnumber[%d]: %x\n",RFIDbyte,RFIDstr[RFIDbyte]);
				if(KEYstr[RFIDbyte]!=RFIDstr[RFIDbyte])
                {
                    return 0;
                }
            }
            tricolorled_onoff(LED_BLUE, LED_ON);
            if(LOCK == lockerState)
            {
                SetServoPosition(UNLOCK);
                lockerState = UNLOCK;
                lcd_gotoxy(7, 1);
                lcd_puts("OPE");
                //fprintf(USART,"show case door unlocked!");
            }
            else
            {
                SetServoPosition(LOCK);
                lockerState = LOCK;
                lcd_gotoxy(7, 1);
                lcd_puts("LOC");
                //fprintf(USART,"show case door locked!");
            }
            tricolorled_onoff(LED_BLUE, LED_OFF);
        }
        rfidFlag = 0;
        return 1;
    }
    return 0;

}

void HandleIR()
{
    uint32_t current_command = 0;
    static unsigned char SwitchesValue;
    char buf[3];
    if(1 == irFlag)
    {
        current_command = get_current_command();
        if (current_command != 0)
        {
            //fprintf(USART,"current_command:%x\n",current_command);
            switch (current_command)
            {
            case COMMAND_VOL_MINUS:
                if( SwitchesValue == 0)
                {
                    SwitchesValue = 11;
                }
                SwitchesValue--;
                break;
            case COMMAND_VOL_PLUS:
                SwitchesValue++;
                if(SwitchesValue >= 11)
                {
                    SwitchesValue = 0;
                };
                break;
            case COMMAND_PLAY_PAUSE:
                securityEnableFlag ^= (securityEnableFlag | 0x01); //toggle securityEnableFlag flag between 0 and 1
                lcd_gotoxy(13, 1);
                if(0 == securityEnableFlag)
                {
                    alarm_DIS();
                    lcd_puts("DIS");
                }
                else
                {
                    lcd_puts("EN ");
                }
                break;
            case COMMAND_0:
                SwitchesValue = 0;
                break;
            case COMMAND_1:
                SwitchesValue = 1;
                break;
            case COMMAND_2:
                SwitchesValue = 2;
                break;
            case COMMAND_3:
                SwitchesValue = 3;
                break;
            case COMMAND_4:
                SwitchesValue = 4;
                break;
            case COMMAND_5:
                SwitchesValue = 5;
                break;
            case COMMAND_6:
                SwitchesValue = 6;
                break;
            case COMMAND_7:
                SwitchesValue = 7;
                break;
            case COMMAND_8:
                SwitchesValue = 8;
                break;
            case COMMAND_9:
                SwitchesValue = 9;
                break;
            case COMMAND_100_PLUS:
                SwitchesValue = 10;
                break;
            default:
                break;
            }
            //change light value on lcd display
            lcd_gotoxy(2, 1);
            lcd_puts("  ");
            lcd_gotoxy(2, 1);
            itoa(SwitchesValue, buf, 10);
            lcd_puts(buf);
            switch(SwitchesValue)
            {
            case 0:
                OCR4AL =  0;			// led light off
                break;
            case 1:
                OCR4AL = 26;			// 10% duty cycle
                break;
            case 2:
                OCR4AL = 51;			// 20% duty cycle
                break;
            case 3:
                OCR4AL = 77;		// 30% duty cycle
                break;
            case 4:
                OCR4AL = 102;		// 40% duty cycle
                break;
            case 5:
                OCR4AL = 128;		// 50% duty cycle
                break;
            case 6:
                OCR4AL = 153;		// 60% duty cycle
                break;
            case 7:
                OCR4AL = 179;		// 70% duty cycle
                break;
            case 8:
                OCR4AL = 204;		// 80% duty cycle (LEDS appear near-full brightness)
                break;
            case 9:
                OCR4AL = 230;		 // 90% duty cycle (LEDS appear near-full brightness)
                break;
            case 10:
                OCR4AL = 255;		 // 100% duty cycle (LEDS appear near-full brightness)
                break;
            }
        }
        irFlag = 0;
    }
}

void lcdFormate()//formate lcd display
{
    lcd_gotoxy(0, 0);
    lcd_puts(" T:88 C  H:88%");
    lcd_gotoxy(5, 0);
    lcd_putc(0xdf);
    lcd_gotoxy(0, 1);
    lcd_puts("L:0  D:LOC A:EN ");
}