/**
 * @file joystick.c
 * @brief This file contains the implementation of the joystick functionalities.
 * 
 * This module is responsible for initializing the joystic and processing the
 * readings from it.
 * 
 * @author Camila Boa Morte
 * @date 23 February 2025
 */

#include "joystick.h"


/* Constants declaration*/
const uint GPIO_JOY_VRX = 27;       // GPIO Joystick for x-axis
const uint GPIO_JOY_VRY = 26;       //GPIO Joystick for y-axis
const uint GPIO_JOY_BUTTON = 22;    // GPIO Joyshtick pushbutton



/* */
/**
 * @brief Initialization of the joystick hardware.
 *
 * This function sets up the ADC connected to the joystick's GPIOs (VRX and VRY), and the joystick 
 * pushbutton using the pull up resistor.
 * 
 * It should be called before any other joystick-related functions are used.
 */
void joystick_init() {
    /* ADC setup*/
    adc_init();
    adc_gpio_init(GPIO_JOY_VRX);
    adc_gpio_init(GPIO_JOY_VRY);
    
    /* Pushbutton setup */
    gpio_init(GPIO_JOY_BUTTON);
    gpio_set_dir(GPIO_JOY_BUTTON, GPIO_IN);
    gpio_pull_up(GPIO_JOY_BUTTON);
}


/**
 * @brief Reads the current ADC value for the joystick's X axis.
 *
 * This function reads the ADC valuee for the joystick's horizontal (VRX) axis
 * and stores it in the provided pointer.
 *
 * @param[out] joy_vrx_adc Pointer to a variable where the ADC value for the VRX axis will be stored.
 */

void joystick_read_vrx(uint16_t* joy_vrx_adc) {
    /* Read ADC value for x axis */
    adc_select_input(1);
    *joy_vrx_adc = adc_read();
 

}


/**
 * @brief Reads the current ADC value for the joystick's Y axis.
 *
 * This function reads the ADC valuee for the joystick's vertical (VRY) axis
 * and stores it in the provided pointer.
 *
 * @param[out] joy_vry_adc Pointer to a variable where the ADC value for the VRY axis will be stored.
 */

void joystick_read_vry(uint16_t* joy_vry_adc) {
    /*Read ADC value for y axis */
    adc_select_input(0);
    *joy_vry_adc = adc_read();

}



