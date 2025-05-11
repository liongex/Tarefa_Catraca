/*
    Tarefa 04 Sistemas Digitais - Embarcatech
    Aluno: Isac de Lima Feliciano;
    Matrícula: 104.265.784-06 / 20251ZL00130048.
    
    BIBLIOTECA RESPONSÁVEL PELO DISPLAY

*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "hardware/adc.h"
#include "hardware/pio.h"
#include "Display.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15

ssd1306_t disp;//variável display do Oled

void setup(){
    //inicialização do Oled
    stdio_init_all();
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT);
    ssd1306_clear(&disp);
    ssd1306_invert(&disp, true);

}

void print_texto(int x, int y, int tam, char * msg){
    ssd1306_draw_string(&disp, x, y, tam, msg);
    ssd1306_show(&disp);
}
void print_retangulo(int x1, int y1, int x2, int y2){
    ssd1306_draw_empty_square(&disp, x1, y1, x2, y2);
    ssd1306_show(&disp);
}

void print_menu(int pos){
        ssd1306_clear(&disp);//Limpa a tela
        print_texto(4, 2, 1, "Portas-Logicas");
        print_retangulo(2,pos+2,125,12);
        print_texto(6, 18, 1.5,"AND");
        print_texto(6, 30, 1.5, "OR");
        print_texto(6, 42, 1.5, "NOT");
        print_texto(6, 54, 1.5, "NAND");
}

void print_menu2(int pos){
    ssd1306_clear(&disp);//Limpa a tela
    print_texto(4, 2, 1, "Portas-Logicas");
    print_retangulo(2,pos+2,125,12);
    print_texto(6, 18, 1.5,"NOR");
    print_texto(6, 30, 1.5, "XOR");
    print_texto(6, 42, 1.5, "XNOR");
}