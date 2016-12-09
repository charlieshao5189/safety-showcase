#ifndef TRICOLORLED_H_
#define TRICOLORLED_H_

#define LED_RED   0
#define LED_GREEN 1
#define LED_BLUE  2

#define LED_ON  1
#define LED_OFF 0

extern void triColorLed_init();
extern void tricolorled_onoff(unsigned char color, unsigned char ledonoff);
void tricolorled_toggle(unsigned char color);
#endif