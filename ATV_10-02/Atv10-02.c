#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/bootrom.h"

#include "inc/ssd1306.h"    // Biblioteca para o display SSD1306

// Ajuste Ppara o centro do joystick
#define CENTRO_X            1939
#define CENTRO_Y            2180

//  definições para o tamanho do quadrado da tela 
#define TAMANHO_QUADRADO    8  
#define LARGURA_TELA        128
#define ALTURA_TELA         64

// I2C
#define PORTA_I2C           i2c1
#define SDA_I2C             14
#define SCL_I2C             15
#define OLED_ENDERECO       0x3C

// LEDs e botões
#define LED_VERDE           11    // LED verde (liga/desliga)
#define LED_AZUL            12    // LED azul controlado via PWM
#define LED_VERMELHO        13    // LED vermelho controlado via PWM
#define BOTAO_A             5     // Botão A: liga/desliga os LEDs via PWM
#define BOTAO_B             6     // Botão B: entra no modo bootloader
#define BOTAO_JOYSTICK      22    // Botão integrado ao joystick, que liga o led verde.

// Joystick (ADC)
#define JOYSTICK_X_ADC      26    // ADC para eixo X (ADC0)
#define JOYSTICK_Y_ADC      27    // ADC para eixo Y (ADC1)
#define ADC_MAX             4095  // Valor máximo do ADC (12 bits)

// Debounce (200 ms)
static volatile uint32_t tempo_ultimo_botaoA = 0;
static volatile uint32_t tempo_ultimo_botaoB = 0;
static volatile uint32_t tempo_ultimo_js = 0;
const uint32_t atraso_debounce_us = 200000;  // 200 ms

// Variáveis globais
static volatile bool led_verde_estado = false;   // Estado do LED verde (ON/OFF)
static volatile bool pwm_ativo = true;           // Habilita/desabilita os LEDs via PWM
// Borda: 0 = fina, 1 = grossa
static volatile int estilo_borda = 0;
static ssd1306_t ssd;                            // Estrutura do display SSD1306


// Função de mapeamento do ADC para a tela
int mapear_adc_para_tela(int valor_adc, int centro, int max_tela) {
    int desvio = valor_adc - centro;
    int valor_mapeado;
    if (desvio < 0) {
        valor_mapeado = ((desvio * (max_tela / 2)) / centro) + (max_tela / 2);
    } else {
        valor_mapeado = ((desvio * (max_tela / 2)) / (ADC_MAX - centro)) + (max_tela / 2);
    }
    if (valor_mapeado < 0) valor_mapeado = 0;
    if (valor_mapeado > max_tela) valor_mapeado = max_tela;
    return valor_mapeado;
}

static uint pwm_inicializar(uint pino) {
    gpio_set_function(pino, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pino);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_wrap(&cfg, 65535);
    pwm_init(slice_num, &cfg, true);
    uint canal = pwm_gpio_to_channel(pino);
    pwm_set_chan_level(slice_num, canal, 0);
    return slice_num;
}

static void pwm_definir_ciclo(uint pino, uint16_t ciclo) {
    if (!pwm_ativo && (pino == LED_VERMELHO || pino == LED_AZUL))
        ciclo = 0;
    uint slice_num = pwm_gpio_to_slice_num(pino);
    uint canal = pwm_gpio_to_channel(pino);
    pwm_set_chan_level(slice_num, canal, ciclo);
}


static void desenhar_quadrado(int posX, int posY) {
    ssd1306_rect(&ssd, posY, posX,TAMANHO_QUADRADO,TAMANHO_QUADRADO, true, true);
}

static void desenhar_borda(int estilo) {
    if (estilo == 0) { 
        ssd1306_rect(&ssd, 0, 0, LARGURA_TELA, ALTURA_TELA, true, false);
    } else if (estilo == 1) {
        ssd1306_rect(&ssd, 0, 0, LARGURA_TELA, ALTURA_TELA, true, false);
        ssd1306_rect(&ssd, 1, 1, LARGURA_TELA - 2, ALTURA_TELA - 2, true, false);
    }
}

static void atualizar_tela(int posX, int posY, int estilo) {
    ssd1306_fill(&ssd, false);
    desenhar_quadrado(posX, posY);
    desenhar_borda(estilo);
    ssd1306_send_data(&ssd);
}

// Callback de interrupção para os botões
static void interrupcao_gpio(uint pino, uint32_t eventos) {
    uint32_t agora = time_us_32();
    if (pino == BOTAO_A) {
        if ((agora - tempo_ultimo_botaoA) > atraso_debounce_us) {
            tempo_ultimo_botaoA = agora;
            pwm_ativo = !pwm_ativo;
            printf("[BOTAO_A] PWM = %s\n", pwm_ativo ? "ON" : "OFF");
        }
    } else if (pino == BOTAO_JOYSTICK) {
        if ((agora - tempo_ultimo_js) > atraso_debounce_us) {
            tempo_ultimo_js = agora;
            led_verde_estado = !led_verde_estado;
            gpio_put(LED_VERDE, led_verde_estado);
            estilo_borda = !estilo_borda;
            printf("[BOTAO_JOYSTICK] LED_VERDE=%s, Borda=%s\n",
                   led_verde_estado ? "ON" : "OFF", (estilo_borda ? "grossa" : "fina"));
        }
    } else if (pino == BOTAO_B) {
        if ((agora - tempo_ultimo_botaoB) > atraso_debounce_us) {
            tempo_ultimo_botaoB = agora;
            ssd1306_fill(&ssd, false);
            ssd1306_send_data(&ssd);
            printf("[BOTAO_B] Entrando no modo bootloader\n");
            reset_usb_boot(0, 0);
        }
    }
}


int main() {
    stdio_init_all();

    // Inicializa I2C e o display SSD1306
    i2c_init(PORTA_I2C, 400 * 1000);
    gpio_set_function(SDA_I2C, GPIO_FUNC_I2C);
    gpio_set_function(SCL_I2C, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_I2C);
    gpio_pull_up(SCL_I2C);
    ssd1306_init_config_clean(&ssd, SCL_I2C, SDA_I2C, PORTA_I2C, OLED_ENDERECO);

    // Inicializa o LED Verde
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERDE, led_verde_estado);

    // Inicializa os LEDs vermelho e azul via PWM
    pwm_inicializar(LED_VERMELHO);
    pwm_inicializar(LED_AZUL);

    // Configura os botões com interrupção
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, interrupcao_gpio);

    gpio_init(BOTAO_JOYSTICK);
    gpio_set_dir(BOTAO_JOYSTICK, GPIO_IN);
    gpio_pull_up(BOTAO_JOYSTICK);
    gpio_set_irq_enabled(BOTAO_JOYSTICK, GPIO_IRQ_EDGE_FALL, true);

    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);
    gpio_set_irq_enabled(BOTAO_B, GPIO_IRQ_EDGE_FALL, true);

    // Inicializa o ADC para o joystick
    adc_init();
    adc_gpio_init(JOYSTICK_X_ADC);
    adc_gpio_init(JOYSTICK_Y_ADC);

    // Exibe inicialmente o quadradinho centralizado
    int posX_inicial = LARGURA_TELA / 2 - TAMANHO_QUADRADO / 2;
    int posY_inicial = ALTURA_TELA / 2 - TAMANHO_QUADRADO / 2;
    atualizar_tela(posX_inicial, posY_inicial, estilo_borda);

    while (true) {
        adc_select_input(1);
        uint16_t adc_x = adc_read();
        adc_select_input(0);
        uint16_t adc_y = adc_read();

        int mapeado_x = mapear_adc_para_tela(adc_x, CENTRO_X, LARGURA_TELA - TAMANHO_QUADRADO);
        int mapeado_y = mapear_adc_para_tela(adc_y, CENTRO_Y, ALTURA_TELA - TAMANHO_QUADRADO);

        // Calcula a posição:
        // posX corresponde à posição horizontal (já mapeada)
        // posY é invertido para que 0 fique no topo do display
        int posX_val = mapeado_x;
        int posY_val = ALTURA_TELA - TAMANHO_QUADRADO - mapeado_y;

        atualizar_tela(posX_val, posY_val, estilo_borda);

        // Cálculo do PWM 
        int diff_x = (adc_x > CENTRO_X) ? (adc_x - CENTRO_X) : (CENTRO_X - adc_x);
        int diff_y = (adc_y > CENTRO_Y) ? (adc_y - CENTRO_Y) : (CENTRO_Y - adc_y);
        uint32_t ciclo_r = diff_x * 32;
        uint32_t ciclo_b = diff_y * 32;
        if (ciclo_r > 65535) ciclo_r = 65535;
        if (ciclo_b > 65535) ciclo_b = 65535;
        pwm_definir_ciclo(LED_VERMELHO, (uint16_t)ciclo_r);
        pwm_definir_ciclo(LED_AZUL, (uint16_t)ciclo_b);

        sleep_ms(30);
    }
    return 0;
}
