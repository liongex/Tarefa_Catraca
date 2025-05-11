/*
    Tarefa 04 Sistemas Digitais - Embarcatech
    Aluno: Isac de Lima Feliciano;
    Matrícula: 104.265.784-06 / 20251ZL00130048.
    
    BIBLIOTECA RESPONSÁVEL PELOs LEDS

*/
#ifndef _inc_Leds
#define _inc_Leds

#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/pio.h"



// Definição de pixel GRB
typedef struct{
    uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
  }npLED_t;

//Protótipos das funções
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b, npLED_t leds[]); //Atribui uma cor RGB a um LED.
void npClear( npLED_t leds[],int LED_COUNT );//Limpa o buffer de pixels.
void npWrite(int LED_COUNT, npLED_t leds[], PIO np_pio, uint sm);//Escreve os dados do buffer nos LEDs.
void setup_rgb(int LED_R, int LED_G); // Inicializa os LEDS RGB do D1

#endif
