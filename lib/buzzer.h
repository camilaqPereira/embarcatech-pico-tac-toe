#ifndef BUZZER_H
#define BUZZER_h

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

/* Definicao de constantes*/
extern const uint GPIO_BUZZER;

// Escala musical simples: Do, Re, Mi, Fa, Sol, La, Si, Do

extern const uint melody[];
extern const uint melody_duration[];
extern const uint melody_lenght;

#define BUZZER_FREQ 4000 // Frequência do buzzer em Hz


/*
*   @brief Funcao para inicializar o pino do buzzer A
*/
void buzzer_init();

/*
*   @brief Funcao de configuracao do PWM no pino do buzzer com a frequência especificada
*   @param pin - pino do componente a ser modulado
*   @param freq_hz - frequencia de modulacao
*/
void buzzer_setup_pwm(uint pin, uint freq_hz);


/*
*   @brief Função para tocar o buzzer por um tempo especificado (em milissegundos)
*   @param times - quantidade de vezes que o buzzer sera acionado
*   @param freq_hz - frequencia do buzzer
*   @param duration_ms - tempo em ms que o buzzer deve ficar ativo
*/
void buzzer_play(uint times, uint freq_hz, uint duration_ms);


/*
    @brief Funcao para tocar a escala musical simples
*/

void buzzer_play_melody();

#endif