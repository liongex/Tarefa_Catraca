/*
    Tarefa 04 Sistemas Digitais - Embarcatech
    Aluno: Isac de Lima Feliciano;
    Matrícula: 104.265.784-06 / 20251ZL00130048.
    
    BIBLIOTECA RESPONSÁVEL PELO DISPLAY

*/
#ifndef _inc_Display
#define _inc_Display

#include <pico/stdlib.h>
#include <hardware/i2c.h>

//Protótipos das funções
void setup_oled(); // função para configurar display oled e adc

void print_texto(int x, int y, int tam, char * msg); // função para escrever textos
void print_retangulo(int x1, int y1, int x2, int y2); // função para desenhar retangulo do menu
void print_menu(int pos); // função para desenhar o menu

#endif
