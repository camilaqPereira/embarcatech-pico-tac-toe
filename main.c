#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#include "lib/led_matrix.h"

/* Function prototypes*/
void display_board(float board[5][5]);


int main()
{
    // Initialize board
    char board[5][5] = {
        {'e', 'b', 'e', 'b', 'e'},
        {'b', 'b', 'b', 'b', 'b'},
        {'e', 'b', 'e', 'b', 'e'},
        {'b', 'b', 'b', 'b', 'b'},
        {'e', 'b', 'e', 'b', 'e'}
    };


    stdio_init_all();


    while(true) {
        
        display_board(board);

        //TO DO: GAME2
        sleep_ms(1000); // Sleep for 1 second
    }
    return 0;

    
}




void display_board(float board[5][5]) {
    int i, j;
    // Display the board on the LED matrix
    for (i = 4; i >=0; i--) {

        if (i & 1) { // Odd row
            for (j = 0; j < 5; j++) {

                switch (board[i][j]) {
                    case 'x':
                        matrix_rgb(0.1,0,0); // Blue
                        break;
                    case 'o':
                        matrix_rgb(0,0.1,0); // Red
                        break;
                    case 'b':
                        matrix_rgb(0.1, 0.1, 0.1); // White
                        break;
                    default:
                        matrix_rgb(0,0,0); // Off
                        break;
                }
                pio_sm_put_blocking(pio, state_machine, led_value);
            
            }
        }else {
            for (j = 4; j >= 0; j--) {
            
            }
        }
        printf("\n");

            
    }
}
