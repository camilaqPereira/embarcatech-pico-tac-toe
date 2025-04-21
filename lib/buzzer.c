#include "buzzer.h"

/* Definicao de constantes*/

const uint GPIO_BUZZER = 21;

const uint melody[] = {262, 294, 330, 349, 392, 440, 494, 523}; // Notas C4 a C5
const uint melody_duration[] = {300, 300, 300, 300, 300, 300, 300, 600}; // Durações em ms
const uint melody_length = 8;

void buzzer_init(){
    gpio_init(GPIO_BUZZER);
    gpio_set_dir(GPIO_BUZZER, GPIO_OUT);
    gpio_put(GPIO_BUZZER, 0);
}

void buzzer_setup_pwm(uint pin, uint freq_hz){
    gpio_set_function(pin, GPIO_FUNC_PWM); // Configura o pino como saída PWM
    uint slice_num = pwm_gpio_to_slice_num(pin); // Obtém o número do slice
    uint channel = pwm_gpio_to_channel(pin); // Obtém o canal PWM

    // Configura o clock do PWM
    uint32_t clock_div = clock_get_hz(clk_sys) / (freq_hz * 4096);
    pwm_set_clkdiv(slice_num, clock_div);

    // Ativa o PWM e define duty cycle como 0% inicialmente
    pwm_set_chan_level(slice_num, channel, 0);
    pwm_set_enabled(slice_num, true);
}

// Função para tocar o buzzer com PWM
void buzzer_play(uint times, uint freq_hz, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(GPIO_BUZZER); // Obtém o slice
    uint channel = pwm_gpio_to_channel(GPIO_BUZZER); // Obtém o canal PWM

    for (int i = 0; i < times; i++) {
        pwm_set_chan_level(slice_num, channel, 2048); // Duty cycle 50% (som ligado)
        sleep_ms(duration_ms);
        pwm_set_chan_level(slice_num, channel, 0); // Duty cycle 0% (som desligado)
        sleep_ms(200); // Pausa entre os toques
    }
}


// Função para tocar uma melodia no buzzer
void buzzer_play_melody() {
    uint slice_num = pwm_gpio_to_slice_num(GPIO_BUZZER);
    uint channel = pwm_gpio_to_channel(GPIO_BUZZER);

    for (int i = 0; i < melody_length; i++) {
        uint32_t clock_div = clock_get_hz(clk_sys) / (melody[i] * 4096);
        pwm_set_clkdiv(slice_num, clock_div);
        pwm_set_chan_level(slice_num, channel, 2867); // Duty cycle 70%
        sleep_ms(melody_duration[i]);
        pwm_set_chan_level(slice_num, channel, 0); // Desliga o som
        sleep_ms(150); // Pausa entre as notas
    }
}
