#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#include "lib/led_matrix.h"
#include "main.pio.h"

/* Function prototypes*/
void display_board(char board[5][5], PIO pio, uint state_machine);


int main()
{
    // Initialize board
    char board[5][5] = {
        {'e', 'b', 'e', 'b', 'e'},
        {'b', 'b', 'b', 'b', 'b'},
        {'e', 'b', 'e', 'b', 'e'},
        {'b', 'b', 'b', 'b', 'b'},
        {'e', 'b', 'x', 'b', 'e'}
    };

    bool x_turn = true; // Variable to track whose turn it is (X or O)
    bool game_over = false; // Variable to track if the game is over
    uint8_t cursor_position[2] = {2, 2}; // Variable to store the cursor position

    

    /* Clock configuration*/
    if(set_sys_clock_khz(128000, false)){
        printf("Configuration of system clock completed!\n");
    }else{
        printf("Configuration of system clock failed\n");
        return -1;
    }

    /* PIO configuration*/
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &main_program);
    uint state_machine = pio_claim_unused_sm(pio, true);
    main_program_init(pio, state_machine, offset, GPIO_LED_MTX);


    stdio_init_all();


    while(true) {      
        display_board(board, pio, state_machine); // Display the board on the LED matrix

        sleep_ms(1000); // Sleep for 1 second
    }
    return 0;

    
}




void display_board(char board[5][5], PIO pio, uint state_machine) {
    int i, j, step, start, end;
    uint32_t led_value;

    // Display the board on the LED matrix
    for (i = 4; i >= 0; i--) {
        step = (i & 1) ? 1 : -1; // Determine step direction based on row parity
        start = (i & 1) ? 0 : 4; // Start index for the row
        end = (i & 1) ? 5 : -1; // End index for the row

        for (j = start; j != end; j += step) {
            switch (board[i][j]) {
                case 'x':
                    led_value = matrix_rgb(0.1, 0, 0); // Blue
                    break;
                case 'o':
                    led_value = matrix_rgb(0, 0.1, 0); // Red
                    break;
                case 'b':
                    led_value = matrix_rgb(0.1, 0.1, 0.1); // White
                    break;
                default:
                    led_value = matrix_rgb(0, 0, 0); // Off
                    break;
            }
            pio_sm_put_blocking(pio, state_machine, led_value);
        }
    }
}
