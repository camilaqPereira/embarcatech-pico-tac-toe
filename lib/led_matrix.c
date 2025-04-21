
#include "led_matrix.h"

/* Definicao de constantes*/
const uint GPIO_LED_MTX = 7;

uint32_t matrix_rgb(double b, double r, double g) {
    unsigned char R = r * 255;
    unsigned char G = g * 255;
    unsigned char B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}



void matrix_turn_all(PIO pio, uint state_machine, double red, double green, double blue){
    uint32_t led_value = matrix_rgb(blue, red, green);

    for (int i = 0; i < MTX_NUM_LEDS; i++){
        pio_sm_put_blocking(pio, state_machine, led_value);
    }
}



