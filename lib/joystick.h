#ifndef JOYSTICK_H
#define JOYSTICK_H


#include <hardware/adc.h>
#include "pico/stdlib.h"


/* Constants declaration */
#define JOY_CENTER 1990.0           // Joystick resting position (x and y)

#define CENTER_RADIUS 1500.0         // Radius to reduce LED avtivation by trepidation 
#define JOY_MIN_ADC 20              // MIN experimental ADC value
#define JOY_MAX_ADC 4087            // MAX experimental ADC value

extern const uint GPIO_JOY_VRX;           // GPIO Joystick for x-axis
extern const uint GPIO_JOY_VRY;           // GPIO Joystick for y-axis
extern const uint GPIO_JOY_BUTTON;        // GPIO Joystick pushbutton

void joystick_init();
void joystick_read_vrx(uint16_t* joy_vrx_adc);
void joystick_read_vry(uint16_t* joy_vry_adc);


#endif // JOYSTICK_H
