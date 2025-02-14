# Projeto de Reconhecimento e Reprodução de Notas Musicais

Este projeto tem como objetivo reconhecer e reproduzir notas musicais utilizando a plataforma Raspberry Pi Pico e a biblioteca BitDogLab. O sistema reproduz uma nota sonora via buzzer, captura o som emitido pelo usuário com um microfone (utilizando ADC com DMA), estima a frequência e compara com a nota selecionada. O feedback é dado através de LEDs (verde para acerto e vermelho para erro), sinais sonoros e exibição de mensagens em um display OLED.

## Funcionalidades

- **Seleção de Notas Musicais:**  
  Utilize dois botões para avançar ou retroceder entre as notas (Dó, Ré, Mi, Fá, Sol, Lá, Si).

- **Reprodução de Notas:**  
  O sistema reproduz a nota selecionada utilizando um buzzer controlado via PWM.

- **Captura e Análise do Som:**  
  A frequência do som captado pelo microfone é estimada através de amostragem via ADC e DMA, comparada com a nota definida e o resultado é avaliado.

- **Feedback Visual e Sonoro:**  
  - LED verde e um som de acerto são acionados se o som captado estiver dentro do limite da nota selecionada.
  - LED vermelho e um som de erro são acionados se o som não corresponder à nota.
  - Mensagens informativas são exibidas tanto no Serial Monitor quanto em um display OLED.

- **Exibição Inicial no OLED:**  
  Ao iniciar, o display OLED exibe uma mensagem de boas-vindas.

## Requisitos

- **Hardware:**  
  - Raspberry Pi Pico  
  - Buzzer  
  - Microfone (conectado ao canal ADC 2 – pino 26 + canal)  
  - Botões (para navegação entre as notas)  
  - LEDs (um verde e um vermelho)  
  - Display OLED SSD1306  
  - Cabos e protoboard para montagem do circuito

- **Software:**  
  - SDK do Raspberry Pi Pico (C/C++)  
  - Biblioteca BitDogLab (para reconhecimento e reprodução de notas)  
  - Biblioteca SSD1306 (localizada em `ssd1306.h` conforme o exemplo utilizado)


## Como Compilar e Executar

1. **Configuração do Ambiente:**  
   Certifique-se de que o SDK do Raspberry Pi Pico esteja instalado e configurado corretamente no seu sistema.

2. **Configuração do Projeto:**  
   Edite o arquivo `CMakeLists.txt` se necessário para incluir os diretórios de cabeçalhos e as bibliotecas utilizadas (como a BitDogLab e a SSD1306).

3. **Compilação:**  
   No diretório raiz do projeto:
   Compile e execute o código para a placa BitDogLab
   Isso gerará o arquivo binário para ser carregado no Pico.

4. **Upload para o Pico:**  
   Conecte o Raspberry Pi Pico em modo de bootloader e copie o arquivo UF2 gerado para o dispositivo.

## Uso

- **Seleção de Notas:**  
  Pressione o botão A para avançar e o botão B para retroceder entre as notas musicais. Cada seleção aciona a reprodução da nota via buzzer e exibe a frequência correspondente no Serial Monitor e no display OLED.

- **Reconhecimento da Nota:**  
  Após a seleção, o sistema aguarda alguns segundos, capta o som emitido pelo usuário e estima a frequência. Se o som captado estiver dentro de um intervalo aceitável da frequência da nota selecionada, o sistema indicará acerto; caso contrário, indicará erro.

## Personalização

- **Limite de Comparação:**  
  Você pode ajustar o valor de tolerância na comparação entre a frequência capturada e a nota definida (no exemplo, é utilizada uma margem de 5 Hz).

- **Tempos e Divisores:**  
  Os tempos de reprodução, o divisor do PWM e outros parâmetros podem ser ajustados conforme a necessidade do seu projeto.

## Licença

Distribua este projeto sob a licença de sua preferência. Por exemplo, se optar pela [MIT License](https://opensource.org/licenses/MIT), adicione o arquivo `LICENSE` com os termos correspondentes.

