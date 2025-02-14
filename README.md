# Simon Game - BitDogLab

Este projeto implementa o clássico jogo **Simon** utilizando a **BitDogLab**. O jogo desafia a memória do jogador ao reproduzir uma sequência de cores e sons, que deve ser repetida corretamente para continuar avançando.

## 🚀 Sobre o Projeto

Este projeto foi desenvolvido como **trabalho final do curso Embarca Tech - IFPI**. O objetivo é explorar conceitos de **sistemas embarcados**, **interação com periféricos** e **lógica de jogo**, utilizando a placa **BitDogLab**. 

## ⚙️ Componentes Utilizados
- 🔴 **Matriz de LEDs e LED RGB**
- 🟢 **Botões A e B de entrada**
- 🔊 **Buzzer para sons**
- 🛠️ **Placa BitDogLab**

## 🎮 Como Funciona?

1. O jogo exibe uma sequência de cores e sons.
2. O jogador deve repetir a sequência pressionando os botões A e B corretamente.
3. A cada acerto, a sequência aumenta em um passo.
4. O jogo é vencido ao atingir **16 acertos consecutivos**.
5. Em caso de erro, o sistema exibe "Game Over" e reinicia automaticamente.
6. Após uma vitória, uma animação de múltiplas cores é exibida por 3 segundos e, em seguida, os LEDs são desligados.

## 🛠 Tecnologias, Interfaces

- **Linguagem C/C++**: Base para o desenvolvimento do firmware.
- **PWM (Pulse Width Modulation)**: Utilizado para controlar o buzzer e gerar diferentes tons sonoros.
- **I2C (Inter-Integrated Circuit)**: Comunicação com o display OLED para exibir mensagens e orientações durante o jogo.


### 📚 Bibliotecas

- `stdio.h` e `string.h`: Bibliotecas padrão do C para entrada/saída e manipulação de strings.
- `pico/stdlib.h`: Fornece funções essenciais, como controle de GPIOs, temporização e inicialização do sistema.
- `neopixel.c`: Biblioteca personalizada para o controle da matriz de LEDs.
- `hardware/pwm.h`: Gerencia o controle do buzzer utilizando **PWM (Pulse Width Modulation)**, que permite a geração de sons com diferentes frequências.
- `hardware/i2c.h`: Interface para comunicação com o display OLED via **I2C (Inter-Integrated Circuit)**, permitindo a troca de dados com apenas dois fios.
- `pico/binary_info.h`: Utilizada para fornecer metadados sobre a compilação e recursos utilizados.
- `inc/ssd1306.h`: Biblioteca responsável pelo controle do display OLED SSD1306.

## 📌 Recursos e Funcionalidades

- ✅ **Sequência Dinâmica**: O jogo aumenta o nível de dificuldade a cada acerto.
- ✅ **Feedback Sonoro**: Sons distintos para acerto, erro e vitória.
- ✅ **Animação de Vitória**: Exibição de diversas cores por 3 segundos ao alcançar o limite de 16 acertos.
- ✅ **Display OLED**: Instruções, status do jogo e mensagens de fim de jogo.
- ✅ **Desligamento Automático dos LEDs**: Os LEDs são desativados após a animação final para economia de energia.

## 🌐 Variáveis Globais
### 🎵 Controle Sonoro
- `#define BUZZER 21`: Define o pino conectado ao buzzer, responsável pelos sons emitidos durante o jogo.

### 💡 Controle de LEDs
- `#define LED_VERMELHO 13`: Pino associado ao LED vermelho.
- `#define LED_VERDE 11`: Pino associado ao LED verde.
- `#define LED_AZUL 12`: Pino associado ao LED azul.

### 🎛️ Entrada de Botões
- `#define BOTAO_A 5`: Pino do botão A, utilizado como entrada para as jogadas do jogador.
- `#define BOTAO_B 6`: Pino do botão B, também utilizado na interação com o jogo.

### 🌈 Matriz de LEDs
- `#define LED_MATRIX_PINO 7`: Pino conectado à matriz de LEDs.
- `#define LED_MATRIX_CONTA 25`: Quantidade de LEDs controlados pela matriz.
- `#define LED_CONTA 25`: Quantidade total de LEDs configurada para o projeto.

### 🔢 Lógica do Jogo
- `#define SEQUENCIA_MAXIMA 16`: Número máximo de elementos na sequência do jogo Simon.

### 🖥️ Display OLED
- `struct render_area frame_area`: Estrutura que define a área de renderização do display OLED.
- `uint8_t ssd[1024]`: Buffer de memória para exibição de mensagens no display OLED.

### 🔗 Comunicação I2C
- `const uint I2C_SDA = 14`: Pino utilizado para a linha de dados da comunicação I2C.
- `const uint I2C_SCL = 15`: Pino utilizado para a linha de clock da comunicação I2C.

### 🧠 Lógica do Jogo Simon
- `int sequencia[SEQUENCIA_MAXIMA]`: Array que armazena a sequência de cores que o jogador precisa repetir.
- `int sequencia_atual = 0`: Índice que indica a posição atual na sequência durante o jogo.

## 📜 Como Rodar o Projeto?

1. Clone este repositório:
   ```sh
   git clone https://github.com/seu-usuario/simon-game-bitdoglab.git](https://github.com/larissaNa/Simon-game.git
   ```
2. Compile e carregue o código na BitDogLab utilizando o ambiente adequado.
3. Conecte o cabo da BitDogLab no seu pc.
4. Envie o código para a placa e divirta-se jogando!

## 🚨 Observações Importantes
- Certifique-se de que a placa esteja corretamente conectada.
- O tempo de exibição das cores é ajustável no código.
- O efeito final de vitória exibe diversas cores por 2 segundos antes de desligar os LEDs.
- Após uma vitória ou derrota o jogo é inicalizado.

## 📸 Demonstração

## 👩‍💻 Estrutura do Código
- `simon_game.c` - Código principal com a lógica do jogo.
- `neopixel.c` - Funções para controle da matriz de LEDs.
- `display.c` - Controle do display OLED.
- `CMakeLists.txt` - Configuração para compilação.

## 👥 Autora

- **Larissa Souza do Nascimento**

--
🚀 **Divirta-se e desafie sua memória com o Simon Game!** 🧠💡

## 📜 Licença

Este projeto é de código aberto e pode ser utilizado livremente para fins educacionais e de aprendizado.

---

💡 Desenvolvido para o curso **Embarca Tech - IFPI**

