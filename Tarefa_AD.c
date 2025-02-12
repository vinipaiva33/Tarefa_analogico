
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO_SSD1306 0x3C

// Definição dos pinos
#define VRX_PIN 26      // Eixo X do Joystick (ADC0)
#define VRY_PIN 27      // Eixo Y do Joystick (ADC1)
#define LED_RED_PIN 13  // LED Vermelho (PWM)
#define LED_BLUE_PIN 12 // LED Azul (PWM)
#define LED_GREEN_PIN 11 // LED Verde (ativado pelo botão)
#define BUTTON_PIN 22   // Botão central do Joystick

// Faixa de tolerância para considerar o joystick centralizado
#define DEADZONE 100  
#define CENTER_VALUE 2048  

volatile bool led_green_state = false;  // Estado do LED Verde
volatile bool rectangle_visible = false;  // Estado do retângulo extra
volatile bool pwm_active = true; // Variável global para controlar o estado dos LEDs PWM

ssd1306_t ssd;  // Instância do display SSD1306
bool cor = true; 

//Definições para controle do quadrado
int x_pos = 60;  // Posição inicial X
int y_pos = 28;  // Posição inicial Y
int min_x = 4;      // Limite mínimo X (borda esquerda)
int max_x = 117;    // Limite máximo X (borda direita)
int min_y = 4;      // Limite mínimo Y (borda superior)
int max_y = 54;     // Limite máximo Y (borda inferior)


// Callback da interrupção do botão do joystick (GPIO 22)
void button_callback(uint gpio, uint32_t events) {

    if(gpio == 22){
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (current_time - last_interrupt_time > 200) {  // Debouncing (200ms)
        led_green_state = !led_green_state;  // Alterna o estado do LED verde
        gpio_put(LED_GREEN_PIN, led_green_state);

        rectangle_visible = !rectangle_visible;  // Alterna o estado do retângulo
        ssd1306_fill(&ssd, false); // Limpa a tela antes de redesenhar
        
        // Redesenha o retângulo original
        ssd1306_rect(&ssd, 3, 3, 122, 60, true, false);  

        // Se o retângulo extra estiver ativado, desenhá-lo
        if (rectangle_visible) {
            ssd1306_rect(&ssd, 8, 8, 112, 50, true, false);
        }

        ssd1306_send_data(&ssd); // Atualiza o display
    }

    last_interrupt_time = current_time;
}
else
{
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (current_time - last_interrupt_time > 200) {  // Debouncing (200ms)
        pwm_active = !pwm_active;  // Alterna o estado do PWM
        printf("Botão A pressionado! PWM %s\n", pwm_active ? "ATIVADO" : "DESATIVADO");

        // Se o PWM estiver desativado, apagar os LEDs
        if (!pwm_active) {
            pwm_set_gpio_level(LED_RED_PIN, 0);
            pwm_set_gpio_level(LED_BLUE_PIN, 0);
        }
    }

    last_interrupt_time = current_time;
}
}


// Função para inicializar PWM em um pino específico
uint pwm_init_gpio(uint gpio, uint wrap) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);
    return slice_num;
}

// Função para inicializar o SSD1306 via I2C
void init_display() {
    // Inicializa o barramento I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display OLED SSD1306
    ssd1306_init(&ssd, 128, 64, false, ENDERECO_SSD1306, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    // Limpa a tela
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Desenha um retângulo na tela
    ssd1306_rect(&ssd, 3, 3, 122, 60, true, false); 

    // Atualiza o display
    ssd1306_send_data(&ssd);
}

// Função para mapear valores do joystick (0-4095) para coordenadas do display (min -> max)
int map_value(int value, int fromLow, int fromHigh, int toLow, int toHigh) {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}


void display_x_position() 
{
    ssd1306_fill(&ssd, false); // Limpa a tela
    ssd1306_rect(&ssd, 3, 3, 122, 60, true, false); // Redesenha a borda
    if (rectangle_visible) {
        ssd1306_rect(&ssd, 8, 8, 112, 50, true, false); //Redesenha borda menor
        min_x = 9;
        max_x = 111;
        min_y = 10;
        max_y = 48;
    }
    else {
        // Restaura os valores originais quando o retângulo extra não está visível
        min_x = 4;
        max_x = 117;
        min_y = 4;
        max_y = 54;
    }

    // Desenha a letra "X" na posição atualizada
    ssd1306_draw_string(&ssd, "X", x_pos, y_pos); 
    ssd1306_send_data(&ssd); // Atualiza o display
}

int main() {
    stdio_init_all();
    
    // Inicializa o ADC
    adc_init();
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);

    // Configuração do LED verde como saída
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_put(LED_GREEN_PIN, led_green_state);  // Inicialmente desligado

    // Configuração do botão com pull-up e interrupção
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_RISE, true, &button_callback);

    // Configuração do botão A com pull-up e interrupção
    gpio_init(5);
    gpio_set_dir(5, GPIO_IN);
    gpio_pull_up(5);
    gpio_set_irq_enabled_with_callback(5, GPIO_IRQ_EDGE_RISE, true, &button_callback);

    // Inicializa o display SSD1306
    init_display();

    // Configuração PWM para os LEDs
    uint pwm_wrap = 4096;  // Resolução do PWM
    pwm_init_gpio(LED_RED_PIN, pwm_wrap); 
    pwm_init_gpio(LED_BLUE_PIN, pwm_wrap);  

    uint32_t last_print_time = 0;

    while (true) {
        // Leitura do eixo X (VRX)
        adc_select_input(0);  
        uint16_t vrx_value = adc_read();  
        
        // Leitura do eixo Y (VRY)
        adc_select_input(1);  
        uint16_t vry_value = adc_read();  

        // Cálculo da intensidade proporcional ao deslocamento do centro
        uint16_t pwm_red = 0, pwm_blue = 0;

        // Se o joystick estiver fora da faixa central, calcular a intensidade do LED vermelho
        if (vrx_value > CENTER_VALUE + DEADZONE) {
            pwm_red = (vrx_value - CENTER_VALUE) * 2;
        } else if (vrx_value < CENTER_VALUE - DEADZONE) {
            pwm_red = (CENTER_VALUE - vrx_value) * 2;
        }

        // Se o joystick estiver fora da faixa central, calcular a intensidade do LED azul
        if (vry_value > CENTER_VALUE + DEADZONE) {
            pwm_blue = (vry_value - CENTER_VALUE) * 2;
        } else if (vry_value < CENTER_VALUE - DEADZONE) {
            pwm_blue = (CENTER_VALUE - vry_value) * 2;
        }

        // Aplicar valores de PWM apenas se estiver ativado
        if (pwm_active) {
            pwm_set_gpio_level(LED_RED_PIN, pwm_red);
            pwm_set_gpio_level(LED_BLUE_PIN, pwm_blue);
        }

        // Imprimir valores a cada segundo
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        if (current_time - last_print_time >= 1000) {
            printf("VRX: %u | PWM LED Vermelho: %u\n", vrx_value, pwm_red);
            printf("VRY: %u | PWM LED Azul: %u\n", vry_value, pwm_blue);
            printf("Botão pressionado: %s\n", led_green_state ? "ON" : "OFF");
            last_print_time = current_time;
        }

        // Mapeia os valores do joystick para a posição no display
        x_pos = map_value(vry_value, 0, 4095, min_x, max_x); 
        y_pos = map_value(vrx_value, 0, 4095, max_y, min_y);


        // Garante que o "X" fique dentro dos limites do retângulo
        if (x_pos < min_x) x_pos = min_x;
        if (x_pos > max_x) x_pos = max_x;
        if (y_pos < min_y) y_pos = min_y;
        if (y_pos > max_y) y_pos = max_y;

        display_x_position();

        sleep_ms(100);
    }

    return 0;
}

