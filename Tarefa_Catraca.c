/*
    Tarefa 04 Sistemas Digitais - Embarcatech
    Aluno: Isac de Lima Feliciano;
    Matrícula: 104.265.784-06 / 20251ZL00130048.
    
    CÓDIGO PRINCIPAL

*/


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "hardware/adc.h"
#include "Display.h"


// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7

#define LED_R 13 // PINO DO LED VERMELHO
#define LED_G 11 // PINO DO LED VERDE

#define VRY 26 //Porta ADC de variação do Y do Joystick
#define VRX 27 //Porta ADC de variação do X do joystick
#define SW 22 //Pino do Botão do Joystick

#define BOTTON_A 5 //Porta ADC de variação do Y do Joystick
#define BOTTON_B 6 //Porta ADC de variação do X do joystick

const int ADC_CHANNEL_0 = 0; // Canal ADC para o eixo X do joystick
const int ADC_CHANNEL_1 = 1; // Canal ADC para o eixo Y do joystick

static uint16_t vrx_value, vry_value; 

// Definição de pixel GRB
struct pixel_t {
  uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

void setup_joystick(); // função para configurar adc
void npInit(uint pin); //Inicializa a máquina PIO para controle da matriz de LEDs.
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b); //Atribui uma cor RGB a um LED.
void npClear();//Limpa o buffer de pixels.
void npWrite();//Escreve os dados do buffer nos LEDs.

int main() {

  // Inicializa entradas e saídas.
  stdio_init_all();
  setup_oled();
  setup_joystick();

  uint countdown = 0; //verificar seleções para baixo do joystick
  uint countup = 3; //verificar seleções para cima do joystick
  uint pos_y=12; //inicialização de variável da posição dor retangulo do menu
  uint posy_ant=12;//posição anterior do reatangulo do menu
  uint adc_y_raw; //Variável par leitura da entrada do adc
  const uint bar_width = 40; // Valor máximo da posição do joystick
  const uint adc_max = (1 << 12) - 1;
  uint bar_y_pos;

  print_menu(pos_y);//impressão inicial do menu

  // Não faz mais nada. Loop infinito.
  while (true) {

        adc_select_input(ADC_CHANNEL_1);
        adc_y_raw = adc_read();//leitura do joystick
        bar_y_pos = adc_y_raw * bar_width / adc_max; //bar_y_pos determinará se o Joystick foi pressionado para cima ou para baixo
       
        //o valor de 19 é o estado de repouso do Joystick da minha placa 
        // Minha placa apresenta um drif do joystick. Ajustei o código para funcionar nela, por isso o valor 19 para o repouso do joystick.
        
        if(bar_y_pos < 19 && countdown <3 ){
            printf("valor de bar %d\n", bar_y_pos);
            pos_y+=12;
            countdown+=1;
            countup-=1;

        }else if(bar_y_pos > 19 && countup <3){
            printf("valor de bar %d\n", bar_y_pos);
            pos_y-=12;
            countup+=1;
            countdown-=1;
        }
            //texto do Menu
            if(pos_y!=posy_ant){//verifica se houve mudança de posição no menu.
                print_menu(pos_y);
            }

    sleep_ms(100);//delay de atualização
    posy_ant=pos_y;//atualização posição anterior.

    }
}
// função para configurar adc
void setup_joystick(){

        //inicialização do botão do joystick
        gpio_init(SW);
        gpio_set_dir(SW, GPIO_IN);
        gpio_pull_up(SW);
    
        //Inicializar ADC do joystick
        adc_init();
        adc_gpio_init(VRY);
        adc_gpio_init(VRX);

}; 


//Inicializa a máquina PIO para controle da matriz de LEDs.

void npInit(uint pin) {

    // Cria programa PIO.
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
  
    // Toma posse de uma máquina PIO.
    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0) {
      np_pio = pio1;
      sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
    }
  
    // Inicia programa na máquina PIO obtida.
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);
  
    // Limpa buffer de pixels.
    for (uint i = 0; i < LED_COUNT; ++i) {
      leds[i].R = 0;
      leds[i].G = 0;
      leds[i].B = 0;
    }
  }


//Atribui uma cor RGB a um LED.

  void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
  }


//Limpa o buffer de pixels.

  void npClear() {
    for (uint i = 0; i < LED_COUNT; ++i)
      npSetLED(i, 0, 0, 0);
  }


//Escreve os dados do buffer nos LEDs.

  void npWrite() {
    // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
    for (uint i = 0; i < LED_COUNT; ++i) {
      pio_sm_put_blocking(np_pio, sm, leds[i].G);
      pio_sm_put_blocking(np_pio, sm, leds[i].R);
      pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
  }
  