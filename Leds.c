/*
    Tarefa 04 Sistemas Digitais - Embarcatech
    Aluno: Isac de Lima Feliciano;
    Matrícula: 104.265.784-06 / 20251ZL00130048.
    
    BIBLIOTECA RESPONSÁVEL PELO LEDS

*/

#include <stdio.h>
#include <pico/stdlib.h>
#include "Leds.h"


//Atribui uma cor RGB a um LED.

void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b, npLED_t leds[]) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

  //Limpa o buffer de pixels.

void npClear( npLED_t leds[],int LED_COUNT ) {
    for (uint i = 0; i < LED_COUNT; ++i)
      npSetLED(i, 0, 0, 0,leds);
}

//Escreve os dados do buffer nos LEDs.

void npWrite(int LED_COUNT, npLED_t leds[], PIO np_pio, uint sm) {
    // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
    for (uint i = 0; i < LED_COUNT; ++i) {
      pio_sm_put_blocking(np_pio, sm, leds[i].G);
      pio_sm_put_blocking(np_pio, sm, leds[i].R);
      pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

//Função para configurar o LED RGB D1

void setup_rgb(int LED_R, int LED_G){

    gpio_init(LED_R);
    gpio_init(LED_G);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_put(LED_R, false);
    gpio_put(LED_G, false);

}