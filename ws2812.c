#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7
#define tempo 100
#define botaoA 5
#define botaoB 6
#define ledgreen 11
#define ledred 13 
#define ledblue 12

uint8_t led_r = 0; // Intensidade do vermelho
uint8_t led_g = 0; // Intensidade do verde
uint8_t led_b = 200; // Intensidade do azul
uint8_t var=0;
bool zero = false;

// Prototipo da função de interrupção
static void gpio_irq_handler(uint gpio, uint32_t events);


bool led_buffer0[NUM_PIXELS] = { // numero 0
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0,                      // [0,1,2,3,4,
    0, 1, 0, 1, 0,                      //  5,6,7,8,9
    0, 1, 0, 1, 0,                      //  10,11,12,13,14   ---> como realmente é a ordem da matriz de leds
    0, 1, 1, 1, 0                       //  15,16,17,18,19
};                                      //  20,21,22,23,24]

bool led_buffer1[NUM_PIXELS] = { // numero 1
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0,                      
    0, 0, 1, 0, 0,                      
    0, 1, 1, 0, 0,                      
    0, 0, 1, 0, 0                       
}; 

bool led_buffer2[NUM_PIXELS] = { // numero 2
    0, 1, 1, 1, 0, 
    0, 1, 0, 0, 0,                      
    0, 1, 1, 1, 0,                      
    0, 0, 0, 1, 0,                      
    0, 1, 1, 1, 0                       
}; 

bool led_buffer3[NUM_PIXELS] = {// numero 3
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0,                      
    0, 1, 1, 1, 0,                      
    0, 0, 0, 1, 0,                      
    0, 1, 1, 1, 0                       
}; 

bool led_buffer4[NUM_PIXELS] = { // numero 4
    0, 1, 0, 0, 0, 
    0, 0, 0, 1, 0,                      
    0, 1, 1, 1, 0,                      
    0, 1, 0, 1, 0,                      
    0, 1, 0, 1, 0                       
}; 

bool led_buffer5[NUM_PIXELS] = {// numero 5
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0,                      
    0, 1, 1, 1, 0,                      
    0, 1, 0, 0, 0,                      
    0, 1, 1, 1, 0                       
}; 

bool led_buffer6[NUM_PIXELS] = {// numero 6
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0,                      
    0, 1, 1, 1, 0,                      
    0, 1, 0, 0, 0,                      
    0, 1, 1, 1, 0                       
}; 

bool led_buffer7[NUM_PIXELS] = {// numero 7
    0, 1, 0, 0, 0,                      // [0,1,2,3,4,
    0, 0, 0, 1, 0,                      //  5,6,7,8,9
    0, 1, 0, 0, 0,                      //  10,11,12,13,14   ---> como realmente é a ordem da matriz de leds
    0, 0, 0, 1, 0,                      //  15,16,17,18,19
    0, 1, 1, 1, 0                       //  20,21,22,23,24]
};                                      

bool led_buffer8[NUM_PIXELS] = {// numero 8
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0,                      
    0, 1, 1, 1, 0,                      
    0, 1, 0, 1, 0,                      
    0, 1, 1, 1, 0                       
}; 

bool led_buffer9[NUM_PIXELS] = {// numero 9
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0,                      
    0, 1, 1, 1, 0,                      
    0, 1, 0, 1, 0,                      
    0, 1, 1, 1, 0                       
}; 


static inline void put_pixel(uint32_t pixel_grb)  // protocolo WS18
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) //  protoclo WS2812
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}


void set_one_led(uint8_t r, uint8_t g, uint8_t b, int buffer_index)  // funcao para escrever o numero na matriz de leds
{
    bool* led_buffer[] = {led_buffer0, led_buffer1, led_buffer2, led_buffer3, led_buffer4, led_buffer5, led_buffer6, led_buffer7 ,led_buffer8 ,led_buffer9}; // vetor desenho numeros

    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);
    bool* buffer = led_buffer[buffer_index]; // armazena o array na variavel buffer, que será enviado para o vetor de 25 leds correspondente ao desenho 

    // Define todos os LEDs com a cor especificada
    
        for (int i = 0; i < NUM_PIXELS; i++)
        {
        if (buffer[i])
        {
        put_pixel(color); // Liga o LED
        }
        else
        {
        put_pixel(0);  // Desliga o LED
        }
    }

}

static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)


int main()
{
    stdio_init_all(); 
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    gpio_init(ledgreen);
    gpio_set_dir(ledgreen, GPIO_OUT);

    gpio_init(ledred);
    gpio_set_dir(ledred, GPIO_OUT);

    gpio_init(ledblue);
    gpio_set_dir(ledblue, GPIO_OUT);

    gpio_init(botaoA);
    gpio_set_dir(botaoA, GPIO_IN);
    gpio_pull_up(botaoA);

    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);

    gpio_set_irq_enabled_with_callback(botaoA, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);



    while (1)
    {
        if (zero == false) {
            set_one_led(led_r, led_g, led_b, 0); // Atualiza o LED inicial para comecar sempre no numero zero
            zero = true;
        }
        gpio_put(ledred, 1);
        sleep_ms(tempo);
        gpio_put(ledred, 0);
        //set_one_led(0, 0, 0);
        sleep_ms(tempo);
    }

    return 0;
}


void gpio_irq_handler(uint gpio, uint32_t events)
{
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (current_time - last_time > 350000) // 350 ms de debounce (foi testado e cheguei a este tempo que ficou bom)
    {
        last_time = current_time; // Atualiza o tempo do último evento

        if (gpio == botaoB) {
            if(var > 0){
            var -= 1; // Decrementa
            set_one_led(led_r, led_g, led_b, var); // Atualiza o LED
            } else {
                var = 9;
                set_one_led(led_r, led_g, led_b, var);  // logica para ficar ciclico 
            }
        } 
        if (gpio == botaoA) {
            if (var < 9){
            var += 1; // Incrementa
            set_one_led(led_r, led_g, led_b, var); // Atualiza o LED
            } else {
                var = 0;
                set_one_led(led_r, led_g, led_b, var); // logica para ficar ciclico
            }
        }
    }
}

// vale ressaltar que o codigo de wilton foi de extrema relevancia para atingir o objetivo da tarefa