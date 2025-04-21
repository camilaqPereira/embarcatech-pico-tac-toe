#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#include "lib/led_matrix.h"
#include "lib/joystick.h"
#include "lib/ssd1306.h"
#include "lib/buzzer.h"

#include "main.pio.h"

#define DEBOUNCE_TIME 150000        // Debounce time for pushbuttons
#define GPIO_RED_LED 13       // GPIO Red RGB led
#define GPIO_GREEN_LED 11     // GPIO Green RGB led
#define GPIO_BLUE_LED 12      // GPIO Blue RGB led


/* Function prototypes*/
void display_board(char board[5][5], PIO pio, uint state_machine);
void clear_board(char board[5][5]);
void get_position(uint8_t *cursor_row, uint8_t *cursor_column, uint block_position[2], ssd1306_t *display_ssd);
bool get_empty_space(char board[5][5], uint8_t *cursor_row, uint8_t *cursor_column);
void check_winner(char board[5][5], bool *game_over, char *winner);
void press_handler(uint gpio, uint32_t events);