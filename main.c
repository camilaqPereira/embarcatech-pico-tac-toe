#include "main.h"

volatile absolute_time_t joy_button_last_pressed_time; // Stores the last time the joystick button was pressed
volatile bool position_selected = false;




int main()
{
    /* Initialize board 
    * e = empty space
    * b = board
    * c = cursor
    * x = player x
    * o = player o
    */
    char board[5][5] = {
        {'e', 'b', 'e', 'b', 'e'},
        {'b', 'b', 'b', 'b', 'b'},
        {'e', 'b', 'c', 'b', 'e'},
        {'b', 'b', 'b', 'b', 'b'},
        {'e', 'b', 'e', 'b', 'e'}
    };

    /* Game management variables */

    bool x_turn, game_over;                               //Indicate if it's X's turn and if the game is over
    char winner;                                          // Variable to track the winner
    uint8_t cursor_row, cursor_column;                    // Define the cursor position
    uint block_position[2] = {WIDTH/2 - 4, HEIGHT/2 - 4}; // Block position on the display

    joy_button_last_pressed_time = get_absolute_time();   // Initialize the last pressed time for the joystick button
    
    
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

    /* RGB LED configuration*/
    uint rgb_mask = 0b11100000000000;
    gpio_init_mask(rgb_mask);
    gpio_set_dir_masked(rgb_mask, rgb_mask);

    /* Configure IRQ pushbutton*/
    gpio_init(GPIO_JOY_BUTTON);
    gpio_set_dir(GPIO_JOY_BUTTON, GPIO_IN);
    gpio_pull_up(GPIO_JOY_BUTTON);

    gpio_set_irq_enabled_with_callback(GPIO_JOY_BUTTON, GPIO_IRQ_EDGE_FALL, true, &press_handler);
    
    /* Inicialize the joystick */
    joystick_init();

    /* Buzzer configuration*/
    buzzer_init(GPIO_BUZZER);
    buzzer_setup_pwm(GPIO_BUZZER, BUZZER_FREQ);


    /* SSD1306 display configuration */
    ssd1306_t display_ssd; 

    i2c_init(DISPLAY_I2C_PORT, DISPLAY_I2C_FREQ);

    gpio_set_function(DISPLAY_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(DISPLAY_I2C_SCL, GPIO_FUNC_I2C);

    gpio_pull_up(DISPLAY_I2C_SDA);
    gpio_pull_up(DISPLAY_I2C_SCL);

    ssd1306_init(&display_ssd, WIDTH, HEIGHT, false, DISPLAY_I2C_ADDRESS, DISPLAY_I2C_PORT);
    ssd1306_config(&display_ssd);
    
    ssd1306_send_data(&display_ssd);
    


    /* ------------------- MAIN LOOP -------------------------*/
    while(true) { 

        printf("NEW GAME\n"); // Print new game message
        
        /* Reset game*/
        clear_board(board); // Clear board
        
        //Clear display and draw initial border and block
        ssd1306_fill(&display_ssd, false);    
        ssd1306_rect(&display_ssd, 3, 3, 122, 60, true, false);
        ssd1306_send_data(&display_ssd);
        
        gpio_put(GPIO_RED_LED, 1); // Turn on red LED
        gpio_put(GPIO_GREEN_LED, 1); // Turn off green LED
        
        game_over = false; // Reset game over flag
        x_turn = true; // Inicialize turn with player X
        winner = ' '; // Reset winner
        
        //Reset cursor position
        cursor_row = 2;
        cursor_column = 2; // Variables to track the cursor position
        board[cursor_row][cursor_column] = 'c'; // Mark the initial position with 'c'

        uint8_t old_position[2] = {cursor_row, cursor_column};
        
        display_board(board, pio, state_machine); // Display the cleared board  

        /* -----------------------GAME LOOP ----------------------*/
        while (!game_over){
            
            // Check if the position has been selected
            if(position_selected) { 
                position_selected = false; // Reset the flag
                board[old_position[0]][old_position[1]] = x_turn ? 'x' : 'o'; // Update board with the current player's symbol
                
                x_turn = !x_turn; // Switch turns
                gpio_put(GPIO_RED_LED, x_turn); // Update LED state
                printf("Turn: %c\n", x_turn ? 'X' : 'O'); // Print current turn
                
                bool has_empty_space = get_empty_space(board, &cursor_row, &cursor_column); // Check for empty spaces
                
                board[cursor_row][cursor_column] = 'c'; // Mark the new cursor position 
                old_position[0] = cursor_row; // Update old position
                old_position[1] = cursor_column; // Update old position
                
                display_board(board, pio, state_machine); // Display the updated board


                /* Check draw */
                if(!has_empty_space){
                    game_over = true; // Set game over flag
                    printf("Game over! It's a draw.\n"); // Print draw message
                    break;
                }
    
                /* Check winner*/
                check_winner(board, &game_over, &winner);
                
                if(game_over){
                    printf("Game over! Winner: %c\n", winner); // Print winner message
                    break;
                }
    
            }

            /* Get new cursor position with joystick*/
            get_position(&cursor_row, &cursor_column, block_position, &display_ssd); // Get the cursor position from the user
    
            if(board[cursor_row][cursor_column] == 'e') { // If new position is empty
                board[old_position[0]][old_position[1]] = 'e'; // Erase cursor
                board[cursor_row][cursor_column] = 'c'; // Mark the new position
                old_position[0] = cursor_row; // Update the old position
                old_position[1] = cursor_column; // Update the old position
            }
            
            display_board(board, pio, state_machine);
            
            sleep_ms(300); // Sleep for 300 ms

        }

        /* End game routine*/
        matrix_turn_all(pio, state_machine, 0, 0, 0); // Turn off matrix
        gpio_put(GPIO_RED_LED, 0); // Turn off red LED
        gpio_put(GPIO_GREEN_LED, 0); // Turn off green LED
    
        // Clear the display
        ssd1306_fill(&display_ssd, false); 
        ssd1306_send_data(&display_ssd); 
    
        char winner_message[20];
        
        ssd1306_rect(&display_ssd, 2, 2, 126, 62, true, false); // Draw a rectangle on the display
        if(winner == ' ')
            ssd1306_draw_string(&display_ssd, "   ITS A DRAW  ", 8 , HEIGHT/2); // Display the draw message
        else {
            // Display the winner message
            ssd1306_draw_string(&display_ssd, "  WINNER IS ", 8 , HEIGHT/2); 
            ssd1306_draw_char(&display_ssd, winner, 104 , HEIGHT/2);
        }
    
        ssd1306_send_data(&display_ssd); 
    
        buzzer_play(4, 440, 200); // Play signal on the buzzer
        sleep_ms(1500); // Sleep for 1.5 seconds
    }


    return 0;

    
}


/**
 * @brief Displays the current board on the LED matrix.
 *
 * This function takes a 5x5 game board and displays its contents on the LED matrix using the PIO
 * and state machine provided. Each character on the board is mapped to a specific color on the LED matrix.
 *
 * @param board A 5x5 char array representing the game board.
 * @param pio   The PIO instance used for controlling the LED matrix
 * @param state_machine The state machine within the PIO
 */
void display_board(char board[5][5], PIO pio, uint state_machine) {
    int i, j, step, start, end;
    uint32_t led_value;

    // Display the board on the LED matrix
    for (i = 4; i >= 0; i--) {

        // Even rows are displayed left to right, odd rows right to left
        step = (i & 1) ? 1 : -1; // Determine step direction based on row parity
        start = (i & 1) ? 0 : 4; // Start index for the row
        end = (i & 1) ? 5 : -1; // End index for the row

        for (j = start; j != end; j += step) {
            switch (board[i][j]) {
                case 'x':
                    led_value = matrix_rgb(0, 0.1, 0.1); // Yellow
                    break;
                case 'o':
                    led_value = matrix_rgb(0, 0, 0.1); // Green
                    break;
                case 'b':
                    led_value = matrix_rgb(0.05, 0.05, 0.05); // White
                    break;
                case'c':
                    led_value = matrix_rgb(0.1, 0.1, 0); // Purple
                    break;
                default:
                    led_value = matrix_rgb(0, 0, 0); // Off
                    break;
            }
            pio_sm_put_blocking(pio, state_machine, led_value);
        }
    }
}


/**
 * @brief Retrieves the current cursor position on the board and block position on the OLED display.
 *
 * This function fetches the current cursor row and column positions, as well as
 * the block position on the display, and stores them in the provided pointers
 * and array.
 *
 * @param[out] cursor_row Pointer to store the row position.
 * @param[out] cursor_column Pointer to store the column position.
 * @param[in] display_ssd Pointer to the SSD1306 display structure
 */
void get_position(uint8_t *cursor_row, uint8_t *cursor_column, uint block_position[2], ssd1306_t *display_ssd) {
    uint16_t joy_vrx_adc, joy_vry_adc; // Joystick ADC values
    uint16_t vry_diff, vrx_diff; // Difference from center position
    bool changed = false; // Flag to indicate if the joystick position has changed


    while(!changed && !position_selected) { // Wait for joystick movement or button press

        /* Read joystick ADC value*/
        joystick_read_vrx(&joy_vrx_adc);
        joystick_read_vry(&joy_vry_adc);

        /* Calculate difference between ADC values and center*/

        vrx_diff = abs(joy_vrx_adc - JOY_CENTER);
        vry_diff = abs(joy_vry_adc - JOY_CENTER);

        /* Update cursor position (if necessary)*/
        if (vry_diff > CENTER_RADIUS) {
            
            if (joy_vry_adc < JOY_CENTER && *cursor_row < 4) {
                (*cursor_row)+=2;
            }else if (joy_vry_adc > JOY_CENTER && *cursor_row > 0) {
                (*cursor_row)-=2;
            }
            changed = true;            
        }
        
        if (vrx_diff > CENTER_RADIUS) {
            
            if (joy_vrx_adc > JOY_CENTER && *cursor_column < 4) {
                (*cursor_column)+=2;
            } else if (joy_vrx_adc < JOY_CENTER && *cursor_column > 0) {
                (*cursor_column)-=2;
            }
            changed = true;
        }


        /* Display block in SSD1306 */

        //Erase block
        ssd1306_draw_string(display_ssd, " ", block_position[0], block_position[1]);

        // Calculate new position
        block_position[0] = WIDTH * (joy_vrx_adc/4095.0) - 4;
        block_position[1] = HEIGHT * (1 - (joy_vry_adc/4095.0)) - 4;

        // Borders delimitation
        block_position[0] = fmaxf(6, fminf(block_position[0], 116));
        block_position[1] = fmaxf(6, fminf(block_position[1], 52));

        //Display new cursor
        ssd1306_draw_string(display_ssd, ".", block_position[0], block_position[1]);
        ssd1306_send_data(display_ssd);
    }
}


void press_handler(uint gpio, uint32_t events) {
    absolute_time_t current_time = get_absolute_time(); //get current time for debounce

    /* Routine for joystick pushbutton*/
    if(gpio == GPIO_JOY_BUTTON && absolute_time_diff_us(joy_button_last_pressed_time, current_time) > DEBOUNCE_TIME){
        joy_button_last_pressed_time = current_time;
        position_selected = true; // Set the flag to indicate that the position has been selected
        
    }

}

/**
 * @brief Finds an empty space on a 5x5 game board and updates the cursor position.
 *
 * This function searches through a 5x5 game board to locate the first empty space.
 * If an empty space is found, the function updates the cursor's row and column
 * positions to point to the location of the empty space.
 *
 * @param board A 5x5 char array representing the game board.
 * @param cursor_row A pointer store the row index of the empty space.
 * @param cursor_column A pointer sotre the column index of the empty space.
 * @return true if an empty space is found, false otherwise.
 */
bool get_empty_space(char board[5][5], uint8_t *cursor_row, uint8_t *cursor_column) {
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if(board[i][j] == 'e') {
                board[i][j] = 'c';
                *cursor_row = i; // Update cursor position
                *cursor_column = j; // Update cursor position
                return true; // Found an empty space
            }
        }
    }

    return false;
}




/**
 * @brief Checks the game board to determine if there is a winner.
 *
 * This function evaluates the current state of the game board to check if a player
 * has won the game. If a winner is found, the function sets the `game_over` flag
 * to true and updates the `winner` variable with the winning player's symbol.
 *
 * @param board A 5x5 char array representing the game board.
 * @param game_over A boolean flag to indicate if the game is over.
 * @param winner A char pointer to store the winning player's symbol.If no winner is found, it remains unchanged.
 */
void check_winner(char board[5][5], bool *game_over, char *winner) {

    if(board[0][0] != 'e' && board[0][0] == board[2][2] && board[2][2] == board[4][4]) {
        *winner = board[0][0]; // Diagonal winner
        *game_over = true;
    }else if(board[0][4] != 'e' && board[0][4] == board[2][2] && board[2][2] == board[4][0]) {
        *winner = board[0][2]; // Diagonal winner
        *game_over = true;
    }else {
        for(int i = 0; i < 5; i+=2) {
            if(board[i][0] == board[i][2] && board[i][2] == board[i][4] && board[i][0] != 'e') {
                *winner = board[i][0]; // Row winner
                *game_over = true;
            } else if(board[0][i] == board[2][i] && board[2][i] == board[4][i] && board[0][i] != 'e') {
                *winner = board[0][i]; // Column winner
                *game_over = true;
            }
        }
    }
}


/**
 * @brief Clears the game board by resetting all its cells to an initial state.
 *
 * This function initializes a 5x5 game board by setting all its elements
 * to a default value ('b' for board cells and 'e' for empty cells).
 *
 * @param board A 5x5 char array representing the game board.
 */
void clear_board(char board[5][5]) {
    for(int i = 0; i < 5; i+=2) {
        for(int j = 0; j < 5; j+=2) {
            board[i][j] = 'e'; // Clear the board
        }
    }
}