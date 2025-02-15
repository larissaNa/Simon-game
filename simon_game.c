#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "neopixel.c"          
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"

#define BUZZER 21
#define LED_VERMELHO 13
#define LED_VERDE 11
#define LED_AZUL 12   
#define BOTAO_A 5     
#define BOTAO_B 6     
// Definições para a matriz de LEDs
#define LED_MATRIX_PINO 7
#define LED_MATRIX_CONTA 25
#define LED_CONTA 25
// Número máximo de elementos na sequência do jogo
#define SEQUENCIA_MAXIMA 16

// Variáveis globais para o display OLED
struct render_area frame_area;
uint8_t ssd[1024]; // Tamanho máximo do buffer 

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

// Globais para o jogo Simon
int sequencia[SEQUENCIA_MAXIMA]; // Valores: 0 = vermelho, 1 = verde, 2 = azul
int sequencia_atual = 0;


void pwm_init_buzzer(uint pino) {
    gpio_set_function(pino, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pino);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f);
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pino, 0);
}

void tocarNotaDuracao(int frequencia, int duracao) {
    if (frequencia == 0) {
        pwm_set_gpio_level(BUZZER, 0);
        return;
    }
    uint slice_numero = pwm_gpio_to_slice_num(BUZZER);
    uint32_t clock_frequencia = clock_get_hz(clk_sys);
    float clkdiv = 8.0f;
    uint32_t top = (clock_frequencia / (clkdiv * frequencia)) - 1;
    pwm_set_wrap(slice_numero, top);
    pwm_set_clkdiv(slice_numero, clkdiv);
    pwm_set_gpio_level(BUZZER, top / 2);
    sleep_ms(duracao);
    pwm_set_gpio_level(BUZZER, 0);
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

void exibirMensagem(const char *mensagem) {
    memset(ssd, 0, ssd1306_buffer_length);
    int line_height = 8;    // Define a altura de cada linha (ajuste conforme a fonte utilizada)
    int y = 0;              // Posição vertical inicial para a primeira linha
    int start = 0;          // Índice inicial da substring atual
    int i;               
    char linha[128];        // Buffer para armazenar temporariamente cada linha de texto

    // Percorre a string mensagem até o final
    for (i = 0; mensagem[i] != '\0'; i++) {
        // Se encontrar o caractere de nova linha
        if (mensagem[i] == '\n') {
            int len = i - start;  // Calcula o tamanho da linha atual
            if (len >= sizeof(linha))
                len = sizeof(linha) - 1;
            strncpy(linha, mensagem + start, len); // Copia a substring para o buffer
            linha[len] = '\0';      // Finaliza a string com caractere nulo
            ssd1306_draw_string(ssd, 0, y, linha); // Desenha a linha no buffer do display
            y += line_height;       // Incrementa a posição vertical para a próxima linha
            start = i + 1;          // Atualiza o índice de início para a próxima linha
        }
    }
    if (i > start) {
        int len = i - start;
        if (len >= sizeof(linha))
            len = sizeof(linha) - 1;
        strncpy(linha, mensagem + start, len);
        linha[len] = '\0';
        ssd1306_draw_string(ssd, 0, y, linha);
    }
    render_on_display(ssd, &frame_area);
}

void cor(int color) {
    if (color == 0) {
        gpio_put(LED_VERMELHO, 1);
        tocarNotaDuracao(300, 300);
        gpio_put(LED_VERMELHO, 0);
    } else if (color == 1) {
        gpio_put(LED_VERDE, 1);
        tocarNotaDuracao(600, 300);
        gpio_put(LED_VERDE, 0);
    } else if (color == 2) {
        gpio_put(LED_AZUL, 1);
        tocarNotaDuracao(450, 300);
        gpio_put(LED_AZUL, 0);
    }
    sleep_ms(200);
}

void comecarSequencia(int *seq, int length) {
    char mensagem[50];
    sprintf(mensagem, "Rodada: %d", length);
    exibirMensagem(mensagem);
    sleep_ms(1000);
    for (int i = 0; i < length; i++) {
        cor(seq[i]);
        sleep_ms(200);
    }
    exibirMensagem("Sua vez!");
    sleep_ms(500);
}

int esperarPorUmBotaoPressionado(void) {
    while (1) {
        bool a = !gpio_get(BOTAO_A);
        bool b = !gpio_get(BOTAO_B);
        if (a || b) {
            sleep_ms(50); // Deounce para ambos serem pressionados
            a = !gpio_get(BOTAO_A);
            b = !gpio_get(BOTAO_B);
            if (a && b) {
                while (!gpio_get(BOTAO_A) || !gpio_get(BOTAO_B));
                sleep_ms(50);
                gpio_put(LED_AZUL, 1);
                tocarNotaDuracao(450, 300);
                exibirMensagem("Azul");
                sleep_ms(300);
                gpio_put(LED_AZUL, 0);
                return 2;
            } else if (a) {
                while (!gpio_get(BOTAO_A));
                sleep_ms(50);
                gpio_put(LED_VERMELHO, 1);
                tocarNotaDuracao(300, 300);
                exibirMensagem("Vermelho");
                sleep_ms(300);
                gpio_put(LED_VERMELHO, 0);
                return 0;
            } else if (b) {
                while (!gpio_get(BOTAO_B));
                sleep_ms(50);
                gpio_put(LED_VERDE, 1);
                tocarNotaDuracao(600, 300);
                exibirMensagem("Verde");
                sleep_ms(300);
                gpio_put(LED_VERDE, 0);
                return 1;
            }
        }
        sleep_ms(10);
    }
}

// Função para exibir uma cor na matriz de LEDs
void mostrarCorDisplay(uint8_t red, uint8_t green, uint8_t blue) {
    for (int i = 0; i < LED_CONTA; i++) {
        npSetLED(i, red, green, blue);
    }
    npWrite();
}

// Função para executar a sequência de cores
void tocarSequenciaVitoria() {
    // Defina as cores que serão exibidas (vermelho, verde, azul, amarelo, ciano, magenta, branco)
    uint8_t cores[][3] = {
        {255, 0, 0},    // Vermelho
        {0, 255, 0},    // Verde
        {0, 0, 255},    // Azul
        {255, 255, 0},  // Amarelo
        {0, 255, 255},  // Ciano
        {255, 0, 255},  // Magenta
        {255, 255, 255} // Branco
    };
    int numero_cores = sizeof(cores) / sizeof(cores[0]);

    // Duração em milissegundos que cada cor ficará ativa
    int duracao_cor = 2000 / numero_cores;

    // Exibe cada cor por um período específico
    for (int i = 0; i < numero_cores; i++) {
        mostrarCorDisplay(cores[i][0], cores[i][1], cores[i][2]);
        sleep_ms(duracao_cor);
    }

    mostrarCorDisplay(0, 0, 0);
}

void simonGame(void) {
    sequencia_atual = 0;
    exibirMensagem("\n   Simon Game\n   Comecando!");
    sleep_ms(2000);
    // Exibe mensagem inicial informando o mapeamento dos botões
    exibirMensagem("Botao A:Vermelho\n\nBotao B: Verde\n\nAmbos: Azul");
    sleep_ms(4000);
    exibirMensagem("3...2...1\n\n\nGo!");
    sleep_ms(1500);
    
    while (1) {
        if (sequencia_atual >= SEQUENCIA_MAXIMA) {
            exibirMensagem("\n\nVoce venceu!");
            tocarSomAcerto();
            tocarSequenciaVitoria();
            sleep_ms(3000);
            sequencia_atual = 0;
            return; // Encerra o jogo (pode reiniciar no main)
        }
        
        sequencia[sequencia_atual] = rand() % 3;
        sequencia_atual++;
        
        comecarSequencia(sequencia, sequencia_atual);
        
        for (int i = 0; i < sequencia_atual; i++) {
            int botao = esperarPorUmBotaoPressionado();
            if (botao != sequencia[i]) {
                exibirMensagem("\n\nGame Over!");
                tocarSomErro();
                sleep_ms(2000);
                return; 
            }
        }
        exibirMensagem("\n\nAcertou!");
        tocarSomAcerto();
        sleep_ms(1000);
    }
}

int main() {
    stdio_init_all();
    pwm_init_buzzer(BUZZER);
    
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);
    
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    
    npInit(LED_MATRIX_PINO, LED_MATRIX_CONTA);
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
    
    // Semente aleatória para o jogo
    srand(time_us_32());
    
    while (1) {
        simonGame();
        exibirMensagem("\n\nReiniciando...");
        sleep_ms(3000);
    }
    return 0;
}
