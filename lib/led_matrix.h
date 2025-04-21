#ifndef LED_MATRIZ_H
#define LED_MATRIX_H


#include "pico/stdlib.h"
#include "hardware/pio.h"

#define MTX_NUM_LEDS 25 //Total de leds na matriz
#define MTX_NUM_ROWS 5 //Numero de linhas na matriz

extern const uint GPIO_LED_MTX; // GPIO da matriz de leds


/*
*   @brief Função para definir a intensidade de cores do LED
*/
uint32_t matrix_rgb(double b, double r, double g);


void matrix_turn_all(PIO pio, uint state_machine, double red, double green, double blue);

#endif