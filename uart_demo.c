#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "ws2812.pio.h"

#include "hardware/pio.h"
#include "hardware/clocks.h"


#define UART_ID uart0 // Seleciona a UART0
#define BAUD_RATE 115200 // Define a taxa de transmissão
#define UART_TX_PIN 0 // Pino GPIO usado para TX
#define UART_RX_PIN 1 // Pino GPIO usado para RX
#define LED_verde 11
#define LED_azul 12
#define LED_vermelho 13
#define botao_A 5
#define botao_B 6
#define TEMPO_DEBOUNCING 200 // Tempo de debounce em milissegundos


#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7

static void gpio_irq_handler(uint gpio, uint32_t events);

uint32_t last_time_A = 0; // Armazenar o tempo da última interrupção do botão A
uint32_t last_time_B = 0; // Armazenar o tempo da última interrupção do botão B

uint8_t led_r = 100; // Intensidade do vermelho
uint8_t led_g = 0; // Intensidade do verde
uint8_t led_b = 200; // Intensidade do azul

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

PIO pio = pio0;
uint sm = 0;

void init_ws2812() {
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
}


bool num_0[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0
};

bool num_1[NUM_PIXELS] = {
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 1, 1, 0, 0, 
    0, 0, 1, 0, 0
};

bool num_2[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 1, 0, 0, 0, 
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0
};

bool num_3[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0
};

bool num_4[NUM_PIXELS] = {
    0, 1, 0, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 0, 1, 0
};

bool num_5[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 1, 0, 0, 0, 
    0, 1, 1, 1, 0
};

bool num_6[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 1, 0, 0, 0, 
    0, 1, 1, 1, 0
};

bool num_7[NUM_PIXELS] = {
    0, 1, 0, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 0, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0
};

bool num_8[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0
};

bool num_9[NUM_PIXELS] = {
    0, 1, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0
};
void set_num_0(uint8_t r, uint8_t g, uint8_t b)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (num_0[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}

void set_num_1(uint8_t r, uint8_t g, uint8_t b)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (num_1[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}

void set_num_2(uint8_t r, uint8_t g, uint8_t b)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (num_2[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}

void set_num_3(uint8_t r, uint8_t g, uint8_t b)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (num_3[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}

void set_num_4(uint8_t r, uint8_t g, uint8_t b)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (num_4[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}

void set_num_5(uint8_t r, uint8_t g, uint8_t b)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (num_5[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}

void set_num_6(uint8_t r, uint8_t g, uint8_t b)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (num_6[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}

void set_num_7(uint8_t r, uint8_t g, uint8_t b)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (num_7[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}

void set_num_8(uint8_t r, uint8_t g, uint8_t b)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (num_8[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}

void set_num_9(uint8_t r, uint8_t g, uint8_t b)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (num_9[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}


void exibe0(){
    for (int i = 0; i < NUM_PIXELS; i++)
    {
       
        if (i % 2 == 0)
        {
            uart_puts(UART_ID, "Par\r\n");
        }
        else
        {
            uart_puts(UART_ID, "Impar\r\n");
        }
    }
}

void exibe1(){
    for(int i = 0; i<NUM_PIXELS; i++){
        uart_puts(UART_ID, " Teste 1\r\n");
    }
}


int main() {
    // Inicializa a biblioteca padrão
    stdio_init_all();

    // Inicializa a UART
    uart_init(UART_ID, BAUD_RATE);
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    init_ws2812();


    // Configura os pinos GPIO para a UART
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); // Configura o pino 0 para TX
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); // Configura o pino 1 para RX

    // Mensagem inicial
    const char *init_message = "UART Demo - RP2\r\n"
                               "Digite algo e veja o eco:\r\n";
    uart_puts(UART_ID, init_message);

    gpio_set_irq_callback(&gpio_irq_handler);
    irq_set_enabled(IO_IRQ_BANK0, true);

    gpio_set_irq_enabled_with_callback(botao_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(botao_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);


    gpio_init(LED_verde);
    gpio_set_dir(LED_verde, GPIO_OUT);
    gpio_init(LED_azul);
    gpio_set_dir(LED_azul, GPIO_OUT);
    gpio_init(LED_vermelho);
    gpio_set_dir(LED_vermelho, GPIO_OUT);

    gpio_init(botao_A);
    gpio_set_dir(botao_A, GPIO_IN);
    gpio_pull_up(botao_A);
    gpio_init(botao_B);
    gpio_set_dir(botao_B, GPIO_IN);
    gpio_pull_up(botao_B);
    

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
    ssd1306_t ssd; // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    bool cor = true;

    while (1) {
        // if(gpio_get(botao_A) == 0){
        //     sleep_ms(100);
        //     gpio_put(LED_verde, !gpio_get(LED_verde));
        //     uart_puts(UART_ID, " Mudança de estado do LED verde\r\n");
        // }

        // if(gpio_get(botao_B) == 0){
        //     sleep_ms(100);
        //     gpio_put(LED_azul, !gpio_get(LED_azul));
        //     uart_puts(UART_ID, " Mudança de estado do LED azul\r\n");
        // }

        if (uart_is_readable(UART_ID)) {
            // Lê um caractere da UART
            char c = uart_getc(UART_ID);
            uart_putc(UART_ID, c);
            switch (c)
            {
                    case '0':
                        set_num_0(led_r, led_g, led_b);
                        break;
                    case '1':
                        set_num_1(led_r, led_g, led_b);
                        break;
                    case '2':
                        set_num_2(led_r, led_g, led_b);
                        break;
                    case '3':
                        set_num_3(led_r, led_g, led_b);
                        break;
                    case '4':
                        set_num_4(led_r, led_g, led_b);
                        break;
                    case '5':
                        set_num_5(led_r, led_g, led_b);
                        break;
                    case '6':
                        set_num_6(led_r, led_g, led_b);
                        break;
                    case '7':
                        set_num_7(led_r, led_g, led_b);
                        break;
                    case '8':
                        set_num_8(led_r, led_g, led_b);
                        break;
                    case '9':
                        set_num_9(led_r, led_g, led_b);
                        break;
                    default:
                        break;
            }

            // Envia de volta o caractere lido (eco)
            ssd1306_fill(&ssd, !cor);
            char str[2] = {c, '\0'}; // Converte o caractere para string
            ssd1306_draw_string(&ssd, str, 60, 25);
            ssd1306_send_data(&ssd);  // ATUALIZA O DISPLAY
            // Envia uma mensagem adicional para cada caractere recebido
            uart_puts(UART_ID, " <- sendo exibido no display\r\n");
        }
        sleep_ms(40);
    }

    return 0;
}

void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    // Verificar se o tempo de debounce passou para o botão A
    if (gpio == botao_A && (events & GPIO_IRQ_EDGE_FALL)) {
        if (current_time - last_time_A > TEMPO_DEBOUNCING) {
            gpio_put(LED_verde, !gpio_get(LED_verde));
            uart_puts(UART_ID, " Mudança de estado do LED verde\r\n");
            last_time_A = current_time; // Atualiza o tempo da última interrupção
        }
    }

    // Verificar se o tempo de debounce passou para o botão B
    if (gpio == botao_B && (events & GPIO_IRQ_EDGE_FALL)) {
        if (current_time - last_time_B > TEMPO_DEBOUNCING) {
            gpio_put(LED_azul, !gpio_get(LED_azul));
            uart_puts(UART_ID, " Mudança de estado do LED azul\r\n");
            last_time_B = current_time; // Atualiza o tempo da última interrupção
        }
    }
}
