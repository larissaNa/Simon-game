#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "neopixel.c"          // Biblioteca para controle da matriz de LEDs
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"

// Variáveis globais para o display OLED
struct render_area frame_area;
uint8_t ssd[1024]; // Tamanho máximo do buffer (ajuste conforme definido em ssd1306.h)

// Definições de pinos para Simon e outros periféricos
#define BUZZER_PIN 21
#define LED_RED 13
#define LED_GREEN 11
#define LED_BLUE 12    // LED para azul (usado individualmente no jogo)
#define BUTTON_A 5     // Botão A -> Vermelho
#define BUTTON_B 6     // Botão B -> Verde (pressionados juntos: Azul)

// Definições para a matriz de LEDs
#define LED_MATRIX_PIN 7
#define LED_MATRIX_COUNT 25
#define LED_COUNT 25

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

// Parâmetros do ADC (não usados neste exemplo do Simon)
#define ADC_CLOCK_DIV 96.f
#define SAMPLES 2048
#define SAMPLE_RATE 10000.0f

// Número máximo de elementos na sequência do jogo
#define MAX_SEQUENCE 16

// Globais para o jogo Simon
int sequence[MAX_SEQUENCE]; // Valores: 0 = vermelho, 1 = verde, 2 = azul
int currentLength = 0;

// ======================================================================
// Funções de hardware
// ======================================================================

void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f);
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0);
}

void tocarNotaDuracao(int frequency, int duracao) {
    if (frequency == 0) {
        pwm_set_gpio_level(BUZZER_PIN, 0);
        return;
    }
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint32_t clock_freq = clock_get_hz(clk_sys);
    float clkdiv = 8.0f;
    uint32_t top = (clock_freq / (clkdiv * frequency)) - 1;
    pwm_set_wrap(slice_num, top);
    pwm_set_clkdiv(slice_num, clkdiv);
    pwm_set_gpio_level(BUZZER_PIN, top / 2);
    sleep_ms(duracao);
    pwm_set_gpio_level(BUZZER_PIN, 0);
}

void tocarSomAcerto() {
    tocarNotaDuracao(523, 150);
    sleep_ms(50);
    tocarNotaDuracao(659, 150);
}

void tocarSomErro() {
    tocarNotaDuracao(300, 150);
    sleep_ms(50);
    tocarNotaDuracao(300, 150);
}

// Exibe mensagem no display OLED
void exibirMensagem(const char *mensagem) {
    memset(ssd, 0, ssd1306_buffer_length);
    ssd1306_draw_string(ssd, 0, 0, (char *)mensagem);
    render_on_display(ssd, &frame_area);
}

// Debounce para botões
bool debounceButton(uint pin) {
    if (!gpio_get(pin)) {
        sleep_ms(50);
        if (!gpio_get(pin)) {
            while (!gpio_get(pin));
            return true;
        }
    }
    return false;
}

// ======================================================================
// Funções para o jogo Simon
// ======================================================================

// "Toca" uma cor: 0 = vermelho, 1 = verde, 2 = azul
void playColor(int color) {
    if (color == 0) {
        gpio_put(LED_RED, 1);
        tocarNotaDuracao(300, 300);
        gpio_put(LED_RED, 0);
    } else if (color == 1) {
        gpio_put(LED_GREEN, 1);
        tocarNotaDuracao(600, 300);
        gpio_put(LED_GREEN, 0);
    } else if (color == 2) {
        gpio_put(LED_BLUE, 1);
        tocarNotaDuracao(450, 300);
        gpio_put(LED_BLUE, 0);
    }
    sleep_ms(200);
}

// Reproduz a sequência atual
void playSequence(int *seq, int length) {
    char msg[50];
    sprintf(msg, "Rodada: %d", length);
    exibirMensagem(msg);
    sleep_ms(1000);
    for (int i = 0; i < length; i++) {
        playColor(seq[i]);
        sleep_ms(200);
    }
    exibirMensagem("Sua vez!");
    sleep_ms(500);
}


int waitForButtonPress(void) {
    while (1) {
        bool a = !gpio_get(BUTTON_A);
        bool b = !gpio_get(BUTTON_B);
        if (a || b) {
            sleep_ms(50); // Janela para ambos serem pressionados
            a = !gpio_get(BUTTON_A);
            b = !gpio_get(BUTTON_B);
            if (a && b) {
                while (!gpio_get(BUTTON_A) || !gpio_get(BUTTON_B));
                sleep_ms(50);
                gpio_put(LED_BLUE, 1);
                tocarNotaDuracao(450, 300);
                exibirMensagem("Azul");
                sleep_ms(300);
                gpio_put(LED_BLUE, 0);
                return 2;
            } else if (a) {
                while (!gpio_get(BUTTON_A));
                sleep_ms(50);
                gpio_put(LED_RED, 1);
                tocarNotaDuracao(300, 300);
                exibirMensagem("Vermelho");
                sleep_ms(300);
                gpio_put(LED_RED, 0);
                return 0;
            } else if (b) {
                while (!gpio_get(BUTTON_B));
                sleep_ms(50);
                gpio_put(LED_GREEN, 1);
                tocarNotaDuracao(600, 300);
                exibirMensagem("Verde");
                sleep_ms(300);
                gpio_put(LED_GREEN, 0);
                return 1;
            }
        }
        sleep_ms(10);
    }
}

// Função para exibir uma cor na matriz de LEDs
void display_color(uint8_t red, uint8_t green, uint8_t blue) {
    for (int i = 0; i < LED_COUNT; i++) {
        npSetLED(i, red, green, blue);
    }
    npWrite();
}


// Função para executar a sequência de cores
void play_victory_sequence() {
    // Defina as cores que serão exibidas (vermelho, verde, azul, amarelo, ciano, magenta, branco)
    uint8_t colors[][3] = {
        {255, 0, 0},    // Vermelho
        {0, 255, 0},    // Verde
        {0, 0, 255},    // Azul
        {255, 255, 0},  // Amarelo
        {0, 255, 255},  // Ciano
        {255, 0, 255},  // Magenta
        {255, 255, 255} // Branco
    };
    int num_colors = sizeof(colors) / sizeof(colors[0]);

    // Duração em milissegundos que cada cor ficará ativa
    int color_duration = 200 / num_colors;

    // Exibe cada cor por um período específico
    for (int i = 0; i < num_colors; i++) {
        display_color(colors[i][0], colors[i][1], colors[i][2]);
        sleep_ms(color_duration);
    }

    // Desliga todos os LEDs após a sequência
    display_color(0, 0, 0);
}

// Função principal do jogo Simon
void simonGame(void) {
    currentLength = 0;
    exibirMensagem("Simon Game\nComecando!");
    sleep_ms(2000);
    
    while (1) {
        // Se atingir o tamanho máximo da sequência, o jogador venceu
        if (currentLength >= MAX_SEQUENCE) {
            exibirMensagem("Voce venceu!");
            tocarSomAcerto();
            play_victory_sequence();
            sleep_ms(3000);
            currentLength = 0;
            return; // Encerra o jogo (pode reiniciar no main)
        }
        
        // Adiciona um novo elemento à sequência (valor aleatório entre 0 e 2)
        sequence[currentLength] = rand() % 3;
        currentLength++;
        
        playSequence(sequence, currentLength);
        
        // Aguarda a entrada do usuário para cada elemento da sequência
        for (int i = 0; i < currentLength; i++) {
            int botao = waitForButtonPress();
            if (botao != sequence[i]) {
                exibirMensagem("Game Over!");
                tocarSomErro();
                sleep_ms(2000);
                return; // Termina o jogo
            }
        }
        exibirMensagem("Acertou!");
        tocarSomAcerto();
        sleep_ms(1000);
    }
}

int main() {
    stdio_init_all();
    pwm_init_buzzer(BUZZER_PIN);
    
    // Inicializa botões e LEDs individuais para o jogo Simon
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    
    // Inicializa a matriz de LEDs
    npInit(LED_MATRIX_PIN, LED_MATRIX_COUNT);
    
    // Inicializa o I2C e o display OLED
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init();
    
    // Configuração da área de renderização do display OLED
    frame_area.start_column = 0;
    frame_area.end_column = ssd1306_width - 1;
    frame_area.start_page = 0;
    frame_area.end_page = ssd1306_n_pages - 1;
    calculate_render_area_buffer_length(&frame_area);
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
    
    // Exibe mensagem inicial informando o mapeamento dos botões
    exibirMensagem("Botao A: Vermelho");
    sleep_ms(3000);
    exibirMensagem("Botao B: Verde");
    sleep_ms(3000);
    exibirMensagem("Ambos: Azul");
    sleep_ms(3000);
    
    // Semente aleatória para o jogo
    srand(time_us_32());
    
    // Loop principal: reinicia o jogo Simon sempre que terminar
    while (1) {
        simonGame();
        exibirMensagem("Reiniciando...");
        sleep_ms(3000);
    }
    
    return 0;
}
