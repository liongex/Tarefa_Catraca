/*
    Tarefa 04 Sistemas Digitais - Embarcatech
    Aluno: Isac de Lima Feliciano;
    Matrícula: 104.265.784-06 / 20251ZL00130048.
    
    CÓDIGO PRINCIPAL

*/


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "Display.h"
#include "Leds.h"

// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

// Definição do número de LEDs e pino.
#define LED_PIN0  4
#define LED_PIN1  3
#define LED_PIN2  2
#define LED_PIN3  1

#define LED_R 13 // PINO DO LED VERMELHO
#define LED_G 11 // PINO DO LED VERDE

#define VRY 26 //Porta ADC de variação do Y do Joystick
#define VRX 27 //Porta ADC de variação do X do joystick
#define SW 22 //Pino do Botão do Joystick

#define LED_COUNT 25
#define LED_PIN 7

// Define o tempo de debounce em milissegundos
#define DEBOUNCE_TIME_MS 150

const int ADC_CHANNEL_0 = 0; // Canal ADC para o eixo X do joystick
const int ADC_CHANNEL_1 = 1; // Canal ADC para o eixo Y do joystick


// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

// Declaração do buffer de pixels que formam a matriz.
 npLED_t leds[LED_COUNT];

static uint16_t vrx_value, vry_value; 

static volatile uint GR = 0, HO = 0, DI = 0, PT = 1;
volatile uint32_t last_interrupt_time = 0;
static int menu = 0;

void setup_joystick(); // função para configurar adc
void npInit(uint pin); //Inicializa a máquina PIO para controle da matriz de LEDs.
static void gpio_irq_handler(uint gpio, uint32_t events);// função de callback da interrupção
void catraca(); // Função de controle da catraca

int main() {

  // Inicializa entradas e saídas.
  stdio_init_all();
  setup_oled();
  setup_joystick();
  setup_rgb(LED_R,LED_G);

  npInit(LED_PIN); // Inicialização da Maquina de Estado para a Matriz de Leds.
  npClear(leds, LED_COUNT); // Limpeza do buffer de leds.

  uint countdown = 0; //verificar seleções para baixo do joystick
  uint countup = 3; //verificar seleções para cima do joystick
  uint pos_y=12; //inicialização de variável da posição dor retangulo do menu
  uint posy_ant=12;//posição anterior do reatangulo do menu
  uint adc_y_raw; //Variável par leitura da entrada do adc
  const uint bar_width = 40; // Valor máximo da posição do joystick
  const uint adc_max = (1 << 12) - 1;
  uint bar_y_pos;

  print_menu(pos_y); // Impressão inicial do menu

  gpio_set_irq_enabled_with_callback(SW, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Função de interrupção para controle do menu através do botão

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
            if(menu <= 3){
                menu += 1;
            }

        }else if(bar_y_pos > 19 && countup <3){
            printf("valor de bar %d\n", bar_y_pos);
            pos_y-=12;
            countup+=1;
            countdown-=1;
            if(menu >= 0){
                menu -= 1;
            }
        }
            //texto do Menu
            if(pos_y!=posy_ant){//verifica se houve mudança de posição no menu.
                print_menu(pos_y);
            }

        posy_ant=pos_y;//atualização posição anterior.

        if(GR == 1){
            npSetLED(LED_PIN0, 0, 80, 0, leds);    
        }else{
            npSetLED(LED_PIN0, 80, 0, 0, leds);   
        }
        if(HO == 1){
            npSetLED(LED_PIN1, 0, 80, 0, leds);    
        }else{
            npSetLED(LED_PIN1, 80, 0, 0, leds);   
        }
        if(DI == 1){
            npSetLED(LED_PIN2, 0, 80, 0, leds);    
        }else{
            npSetLED(LED_PIN2, 80, 0, 0, leds);   
        }
        if(PT == 0){
            npSetLED(LED_PIN3, 0, 80, 0, leds);    
        }else{
            npSetLED(LED_PIN3, 80, 0, 0, leds);   
        }
        npWrite(LED_COUNT,leds,np_pio,sm);
        catraca();
        sleep_ms(100);//delay de atualização

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

//Função da callback da interrupção

static void gpio_irq_handler(uint gpio, uint32_t events){

    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    switch (menu) {
        case 0:
            if ((current_time - last_interrupt_time) > DEBOUNCE_TIME_MS) {
                GR = !GR;
                last_interrupt_time = current_time;
            }
            break;
        case 1:
            if ((current_time - last_interrupt_time) > DEBOUNCE_TIME_MS) {
                HO = !HO;
                last_interrupt_time = current_time;
            }
            break;
        case 2:
            if ((current_time - last_interrupt_time) > DEBOUNCE_TIME_MS) {
                DI = !DI;
                last_interrupt_time = current_time;
            }
        break;
        case 3:
            if ((current_time - last_interrupt_time) > DEBOUNCE_TIME_MS) {
                PT = !PT;
                last_interrupt_time = current_time;
            }
        break;
        default:
            printf("Número inválido!\n");
    }
};

// Função de controle da catraca
void catraca(){
    if(GR && HO && DI){
        gpio_put(LED_G, true);
        gpio_put(LED_R, false);
    } else if(!PT){
        gpio_put(LED_G, true);
        gpio_put(LED_R, false);
    } else {
        gpio_put(LED_G, false);
        gpio_put(LED_R, true);
    };
};
